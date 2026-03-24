#!/bin/bash

# $1 -> mode: sim | prod
# $2 -> register (e.g. 80 for temperature)

if [ $1 = "prod" ]; then
	sudo mbpoll -0 -m rtu -a 1 -b 9600 -d 8 -p e -s 1 -t 4:int -B -r $2 -c 1 /dev/extcomm/0/0
elif [ $1 = "sim" ]; then
	mbpoll -0 -m tcp -a 1 -p 5020 -t 4:int -B -r $2 -c 1 127.0.0.1
else
	echo "First argument must be \"sim\" or \"prod\""
fi
