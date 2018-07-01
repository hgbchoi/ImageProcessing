#NOTE: must run as: sudo python PBCom.py

import os
import glob
import time
import psutil
import subprocess
import serial
import signal
import RPi.GPIO as GPIO
from bluetooth import *


#setup serial communication
ser = serial.Serial('/dev/ttyACM0',9600)
ser.flushInput()

server_sock=BluetoothSocket( RFCOMM )
server_sock.bind(("",PORT_ANY))
server_sock.listen(1)

port = server_sock.getsockname()[1]

uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"

PROCNAME = "python /home/pi/Documents/PaintBot/tape_detect.py 1"

advertise_service( server_sock, "AquaNPiServer",
                   service_id = uuid,
                   service_classes = [ uuid, SERIAL_PORT_CLASS ],
                   profiles = [ SERIAL_PORT_PROFILE ], 
#                   protocols = [ OBEX_UUID ] 
                    )
while True:          
	print "Waiting for connection on RFCOMM channel %d" % port

	client_sock, client_info = server_sock.accept()
	print "Accepted connection from ", client_info

	try:
	        data = client_sock.recv(1024)
        	if len(data) == 0: break
	        print "received [%s]" % data

		if data == 'temp':
			data = str(read_temp())+'!'
		elif data == 'on':
			data = 'SENDING ON!'
			ser.write(b'Y')
                        p = subprocess.Popen("python /home/pi/Documents/ImageProcessing/image.py 1",stdout=subprocess.PIPE, shell = True,preexec_fn=os.setsid)

		elif data == 'off':
			data = 'SENDING OFF!'
			ser.write(b'N')
                        os.killpg(p.pid, 9)
                else:
			data = 'WTF!' 
	        client_sock.send(data)
		print "sending [%s]" % data

	except IOError:
		pass

	except KeyboardInterrupt:

		print "disconnected"

		client_sock.close()
		server_sock.close()
		print "all done"

		break
