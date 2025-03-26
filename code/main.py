#!/opt/home-auto/venv3.11/bin/python

# General modules
import os
import sys
import atexit

# Dedicated modules
import subprocess
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import uvicorn

# Personal modules
sys.path.append("/opt/Python-Utils/utils/")
from utils import Logger,DataBase,Timer,GetFuncName

# Init logger
_logger=Logger(log_path="/opt/home-auto/log/home.log")
_logger.info("Started program")
# Unhandled exceptions
sys.excepthook = _logger.exception_handler

# Init timer
timer = Timer()

def log_timer_stop():
	exc_time = timer.stop()
	_logger.info("Closed program")
	_logger.debug(f"Execution time: {exc_time}")

try:
	atexit.register(log_timer_stop)
except Exception as e:
	err = str(e)
	_logger.error(f"Could not register execution time : {e}")


# Init fastapi
try:
	app = FastAPI()
    origins = ["http://100.116.80.15:8020","http://192.168.1.100"]
	app.add_middleware(CORSMiddleware,allow_origins=origins,allow_credentials=True,allow_methods=["*"],allow_headers=["*"])
except Exception as e:
	err = "Could not start api" + ":" + str(e) + str(sys.exc_info())
	_logger.error(err)

class DigitalPin:
	def __init__(self,aPin:str):
		self.iPin=aPin # 2.(pin)
	
	def read(self):
		status = subprocess.run(["cat", f"/run/unipi-plc/by-sys/DI{self.iPin}/value"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		if status.returncode != 0:
			err = f"Error {status.returncode} : {status.stderr.decode().strip()}"
			_logger.error(err)
			return err
		else:
			data={"id":self.iPin,"state":str(status.stdout.decode().strip())}
			return data

class Relay:
	def __init__(self,aPin:str):
		self.iPin=aPin
	
	def write(self,newState:int):
        """
        Change a relay's state on or off

        newState: 1 (on), 2 (off)

        """

		" echo 1 | sudo tee value"
		status = subprocess.run(f"echo {str(newState)} | sudo tee /run/unipi-plc/by-sys/RO{self.iPin}/value", 
                        shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		if status.returncode != 0:
			err = f"Error {status.returncode} : {status.stderr.decode().strip()}"
			_logger.error(err)
			return err
		else:
			data={"status":200}
			return data


@app.get("/api/ReadPin/{iPin}")
async def ReadPin(iPin:str):
	pin = DigitalPin(aPin=iPin)
	return pin.read()

@app.get("/api/ReadAllPins")
async def ReadAllPins():
    pins_state={
        "pins": []
    }
    for i in range(1,9):
        iPin="2."+str(i)
        pin=DigitalPin(aPin=iPin)
        pin_status = pin.read()
        pins_state["pins"].append(pin_status)
    return pins_state

@app.get("/api/WriteRelay/{iPin}/{status}")
async def WriteRelay(iPin:str,status:int):
	if status != 0 and status != 1:
		return {"error": f"Relay status {status} can't be set: incorrect status form"}
	relay = Relay(aPin=iPin)
	return relay.write(newState=status)


if __name__ == "__main__":
	uvicorn.run(app, host="0.0.0.0", port=8000)











