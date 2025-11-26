Script to communicate with Magnetic Actuation Board in Asynchronous mode.

Mode of use: $main.py [file_name] [Arduino_COM_port_#] [interpolation (y/n)]

e.g. $main.py test.txt 5 y

To run main.py, make sure to install the python3 libraries:
-numpy
-scipy
-pyserial

Minimum parameters:
	- Minimum time_step tested to work = 0.1s