#!/opt/home-auto/venv3.11/bin/python

# General modules
import os
import sys
import atexit

# Dedicated modules
import subprocess
import threading

# Personal modules
sys.path.append("/opt/Python-Utils/utils/")
from utils import Logger,DataBase,Timer,GetFuncName,GetTime,get_json_data,dict2json,writeFile,wait

# Init logger
_logger=Logger(log_path="/opt/home-auto/log/home.log")
# Unhandled exceptions
sys.excepthook = _logger.exception_handler

# Init timer
timer = Timer()

class Base:
	def __init__(self):
		self.json_file="/opt/home-auto/home-auto-web/config/conf.json"
		self.unipi_sys_base_dir=""
		self.database_info={}
		self.isVirtual=True
		self.run_mode=""
		self.application_running=True

		atexit.register(self.exit_application)

	def exit_application(self):
		self.application_running=False
	def set_conf(self):
		if self.isVirtual:self.run_mode="virtual"
		else:self.run_mode="normal"
	def set_running_mode(self,virtual_mode:bool=True):
		self.isVirtual=virtual_mode
		self.set_conf()

		iDic=get_json_data(aJsonFileDir=self.json_file)
		# Initialize mode variables
		self.unipi_sys_base_dir=iDic["config"]["unipi_sys_base_dir"]
		db_info=iDic["config"]["database_info"]
		self.database_info={
			"Host":db_info["Host"]
			,"User":db_info["User"]
			,"Password":db_info["Password"]
			,"DataBase":db_info["DataBase"]
		}

class DigitalPin(Base):
	def __init__(self,aPin:str):
		super().__init__()
		self.set_running_mode(virtual_mode=True)
		self.iPin=aPin # 2.(pin)
	
	def read(self):
		status = subprocess.run(["cat", f"{self.unipi_sys_base_dir}DI{self.iPin}/value"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		if status.returncode != 0:
			err = f"Error {status.returncode} : {status.stderr.decode().strip()}"
			_logger.error(err)
			return err
		else:
			data={"id":self.iPin,"state":str(status.stdout.decode().strip())}
			return data

class Relay(Base):
	def __init__(self,aPin:str):
		super().__init__()
		self.set_running_mode(virtual_mode=True)
		self.iPin=aPin
		self.calendar=Calendar(self)
		self.hist=Historify(self,aTable="historify")
		self.update_relay_state()


		_logger.debug(f"Relay {self.iPin} running in {self.run_mode} mode")
	
	def read(self):
		status = subprocess.run(["cat", f"{self.unipi_sys_base_dir}RO{self.iPin}/value"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

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
		status = subprocess.run(f"echo {str(newState)} | sudo tee {self.unipi_sys_base_dir}RO{self.iPin}/value", 
                        shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		if status.returncode != 0:
			err = f"Error {status.returncode} : {status.stderr.decode().strip()}"
			_logger.error(err)
			return err
		else:
			data={"status":200}
			return data
	
	def update_relay_state(self):
		"""
		Start the relay if specified in calendar
		"""

		if not self.calendar.isActive or (self.calendar.startDate and self.calendar.endDate):
			self.write(0)
			return
		
		now=GetTime()
		if now >= self.calendar.startDate and now <= self.calendar.endDate:
			self.write(newState=1)
		elif now < self.calendar.startDate or now > self.calendar.endDate:
			self.write(newState=0)
		


class Calendar:
	def __init__(self,aInstance:Relay):
		self.relay=aInstance
		self.isActive=False
		self.startDate=None
		self.endDate=None
		self.calendarInfo:dict={}
		self.iDb=DataBase(
			Host=self.relay.database_info["Host"]
			,User=self.relay.database_info["User"]
			,Password=self.relay.database_info["Password"]
			,DataBase=self.relay.database_info["DataBase"]
			,buffered=True
		)
		self.update_data()
	def set_data(self):
		# {
		#     "calendar": {
		#         "id": "RO2.1",
		#         "start_date": "",
		#         "end_date": "",
		#         "date_active": 0
		#     }
		# }
		iData=self.calendarInfo["calendar"]
		iStartDate=iData["start_date"] if iData["start_date"]!="" else None
		iEndDate=iData["end_date"] if iData["end_date"]!="" else None
		iDateActive=iData["date_active"]
		self.startDate=iStartDate
		self.endDate=iEndDate
		self.isActive=[False,True][iDateActive==1]
	def update_data(self):
		try:
			"""
			Start the relay at a specific date
			"""
			self.iDb.connect()
			if self.iDb is None:
				err=f"Could not connect to bd in Relay.Calendar.{self.update_data.__name__} function"
				_logger.error(err)
				pass
			iSql=f"SELECT * FROM relay WHERE id='RO{str(self.relay.iPin)}'"
			self.iDb.execute(aQuery=iSql)
			iRet=self.iDb.fetchdata()
			self.iDb.close()
			if iRet == [] or not isinstance(iRet,list):
				err="Could not fetch data : "+str(self.update_data.__name__+" function")
				err+="\n"
				err+=str(self.iDb.fetchdata.__name__)+" function response: "+str(iRet)
				_logger.error(err)
				raise Exception(err)
			
			self.calendarInfo["calendar"]=iRet[0]
			self.set_data()
			
		except Exception as e:
			_logger.error(f"Error in {str(self.update_data.__name__)} : {str(sys.exc_info())} : {str(e)}")
	def insert_new_data(self,aData:dict):
			# {'iObj':{
			# 	'pin': '2.1'
			# 	,'calendar':{
			# 		'start_date': '2023-12-20T15:29'
			# 		,'end_date': '2025-05-28T00:00'
			# 		}
			# 	}
			# }
		try:
			self.iDb.connect()
			if self.iDb is None:
				err=f"Could not connect to bd in Relay.Calendar.{self.update_data.__name__} function"
				_logger.error(err)
				pass

			iId="RO"+str(aData["iObj"]["pin"])
			iStartDate=NormDate(str(aData["iObj"]["calendar"]["start_date"]))
			iEndDate=NormDate(str(aData["iObj"]["calendar"]["end_date"]))
			iActive="1"
			iParams=[iStartDate,iEndDate,iActive,iId]
			iSql="UPDATE relay SET start_date=%s,end_date=%s,date_active=%s WHERE id = %s"
			self.iDb.execute(iSql,iParams)
			self.iDb.close()
			self.update_data()
		except Exception as e:
			_logger.error(f"Error in {str(self.insert_new_data.__name__)} : {str(sys.exc_info())} : {str(e)}")

class Historify:
	"""
	Historify changes in pin states
	"""
	def __init__(self,aInstance:Relay|DigitalPin,aTable:str):
		self.iTable=aTable
		self.relay=aInstance
		self.idrelay=aInstance.iPin
		self.iDb=DataBase(
			Host=self.relay.database_info["Host"]
			,User=self.relay.database_info["User"]
			,Password=self.relay.database_info["Password"]
			,DataBase=self.relay.database_info["DataBase"]
		)
		self.init_hist_thread()
	def init_hist_thread(self):
		try:
			iTarget=self.main_hist
			iThreadName="Thread_relay_"+str(self.idrelay)
			iThread=threading.Thread(target=iTarget,name=iThreadName,daemon=True)
			iThread.start()
		except Exception as e:
			err="Error creating new thread for "+str(self.idrelay)+" pin historification"
			_logger.error(err)
	def main_hist(self):
		try:
			last_pin_state=None
			while self.relay.application_running:
				if last_pin_state is None:
					last_pin_state=self.relay.read()
					last_pin_state=last_pin_state["state"]
				read_pin=self.relay.read()
				pin_state=read_pin["state"]
				if pin_state!=last_pin_state:
					self.add_hist(new_pin_state=pin_state)
					last_pin_state=pin_state
				wait(seconds=1)
		except Exception as e:
			err="Historify.main_hist : "+str(sys.exc_info)+" : "+str(e)
			_logger.error(err)
	def add_hist(self,new_pin_state:str):
		try:
			self.iDb.connect()
			iTs=GetTime(aTimeZone="Europe/Madrid",accuracy="s")
			iParams=[str(self.idrelay),str(new_pin_state),str(iTs)]
			iSql="INSERT INTO `{}` (idrelay,newstate,ts) VALUES (%s,%s,%s)".format(str(self.iTable))
			self.iDb.execute(iSql,iParams)
			self.iDb.close()
			# Debug
			# iSql=f"INSERT INTO {iParams[0]} (idrelay,newstate,ts) VALUES ('{iParams[1]}','{iParams[2]}','{iParams[3]}')"
			# _logger.debug(iSql)
		except Exception as e:
			err="Error in Relay.Historify.add_hist function : "+str(sys.exc_info())+" : "+str(e)
			_logger.error(err)

class RelayHandler:
	def __init__(self):
		self.relay1=Relay(aPin="2.1")
		self.relay2=Relay(aPin="2.2")
		self.relay3=Relay(aPin="2.3")
		self.relay4=Relay(aPin="2.4")
		self.relay5=Relay(aPin="2.5")
		self.relay6=Relay(aPin="2.6")
		self.relay7=Relay(aPin="2.7")
		self.relay8=Relay(aPin="2.8")
	
	def get_relay(self,aPin):
		match aPin:
			case "2.1":
				return self.relay1
			case "2.2":
				return self.relay2
			case "2.3":
				return self.relay3
			case "2.4":
				return self.relay4
			case "2.5":
				return self.relay5
			case "2.6":
				return self.relay6
			case "2.7":
				return self.relay7
			case "2.8":
				return self.relay8
		return None

def NormDate(aDate:str):
	"""
	Normalize a date coming from raw html form
	
	It must enter with the following format
	* 2023-12-20T15:29

	It must return with the following format
	* 2023-12-20 15:29
	"""
	iDate=aDate.replace("T"," ")
	return iDate


