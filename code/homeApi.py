# General modules
import sys

# Personal modules
from homeMain import RelayHandler

sys.path.append("/opt/Python-Utils/utils/")
from utils import Logger
from api import Api

# Init logger
_logger=Logger(log_path="/opt/home-auto/log/home.log")
# Unhandled exceptions
sys.excepthook = _logger.exception_handler

def ReadPin(data,aPin:str):
	iPin = rl_handler.get_relay(aPin)
	return iPin.read()

def ReadAllPins():
    pins_state={
        "pins": []
    }
    for i in range(1,9):
        iPin="2."+str(i)
        pin=rl_handler.get_relay(iPin)
        pin_status = pin.read()
        pins_state["pins"].append(pin_status)
    return pins_state

def WriteRelay(data,aPin:str,aStatus:str):
	aStatus=int(aStatus)
	if aStatus != 0 and aStatus != 1:
		return {"error": f"Relay status {aStatus} can't be set: incorrect status form"}
	relay = rl_handler.get_relay(aPin)
	
	return relay.write(newState=aStatus)

def GetCalendar(data,aPin:str):
	relay=rl_handler.get_relay(aPin)
	calendar_data=relay.calendar.calendarInfo
	return calendar_data

def SetCalendar(data,aPin:str):
	if not isinstance(data,dict):
		err=f"Cannot insert new calendar data to {aPin}, received data is not dict. Received data is {type(data)}"
		_logger.error(str(err))
		return {"status":500,"error":str(err)}
	
	relay=rl_handler.get_relay(aPin)
	relay.calendar.insert_new_data(aData=data)

	return {"status":200}

if __name__ == "__main__":
	rl_handler=RelayHandler()
	_api = Api(Port=8000,logger=_logger,init_message="Started home automation api",exit_message="Closed home automation api",allowed_origins=["http://100.116.80.15:8020","http://homeserver:8020","http://100.117.134.68","http://127.0.0.1"])
	_api.add_get_request("/api/ReadAllPins",ReadAllPins)
	_api.add_post_request(r"/api/ReadPin/(?P<aPin>2\.[1-9])",ReadPin)
	_api.add_post_request(r"/api/WriteRelay/(?P<aPin>2\.[1-9])/(?P<aStatus>[0-1])",WriteRelay)
	_api.add_post_request(r"/api/GetCalendar/(?P<aPin>2\.[1-9])",GetCalendar)
	_api.add_post_request(r"/api/SetCalendar/(?P<aPin>2\.[1-9])",SetCalendar)
	_api.init_app()