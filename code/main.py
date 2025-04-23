#!/opt/home-auto/venv3.11/bin/python

# General modules
import os
import sys
import atexit

# Dedicated modules
import subprocess

# Personal modules
sys.path.append("/opt/Python-Utils/utils/")
from utils import Logger,DataBase,Timer,GetFuncName
from api import Api

# Init logger
_logger=Logger(log_path="/opt/home-auto/log/home.log")
_logger.info("Started program")
# Unhandled exceptions
sys.excepthook = _logger.exception_handler

# Init timer
timer = Timer()

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
	
	def read(self):
		status = subprocess.run(["cat", f"/run/unipi-plc/by-sys/RO{self.iPin}/value"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		if status.returncode != 0:
			err = f"Error {status.returncode} : {status.stderr.decode().strip()}"
			_logger.error(err)
			return err
		else:
			data={"id":self.iPin,"state":str(status.stdout.decode().strip())}
			return data
	
	def write(self,newState:int):
		"""
        Change a relay's state on or off

        newState: 1 (on), 0 (off)

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

def ReadPin(data,iPin:str):
	pin = Relay(aPin=iPin)
	return pin.read()

def ReadAllPins():
    pins_state={
        "pins": []
    }
    for i in range(1,9):
        iPin="2."+str(i)
        pin=Relay(aPin=iPin)
        pin_status = pin.read()
        pins_state["pins"].append(pin_status)
    return pins_state

def WriteRelay(data,iPin:str,status:str):
	status=int(status)
	if status != 0 and status != 1:
		return {"error": f"Relay status {status} can't be set: incorrect status form"}
	relay = Relay(aPin=iPin)
	return relay.write(newState=status)

if __name__ == "__main__":
	_api = Api(Port=8000,logger=_logger,init_message="Started test api",exit_message="Closed test api",allowed_origins=["http://100.116.80.15:8020"])
	_api.add_get_request("/api/ReadAllPins",ReadAllPins)
	_api.add_post_request(r"/api/ReadPin/(?P<iPin>2\.[1-9])",ReadPin)
	_api.add_post_request(r"/api/WriteRelay/(?P<iPin>2\.[1-9])/(?P<status>[0-1])",WriteRelay)
	_api.init_app()

