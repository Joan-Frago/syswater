#!/bin/python3


# General modules
import os
import sys
import atexit

# Personal modules
sys.path.append("/opt/dev/Python-Utils/utils/")
from utils import Logger,DataBase,Timer,GetFuncName

# Init logger
_logger=Logger(log_path="/opt/dev/home-auto/log/home.log")
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

