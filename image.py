# Import the necessary packages
import numpy as np
import RPi.GPIO as GPIO
import myFunc as mf
import picamera
import time
import cv2
import serial
import time
import struct

ser = serial.Serial('/dev/ttyACM0',9600, timeout = 1)
byteReceived = '0'
oneDArray = [];

GPIO.setmode(GPIO.BOARD)
GPIO.setup(12, GPIO.OUT)
p = GPIO.PWM(12, 50)
p.start(7.5)


def take_image():
    
        with picamera.PiCamera() as camera:
           
           
            camera.resolution = (500,500)
            camera.start_preview()
            time.sleep(0.2)
            camera.capture("green_tape.png")


while True:
	
	CAPTURES_PER_STRIP = 5
    
	
	
	#This will be sent to Arduino
	while byteReceived != b'S':
            try:
                byteReceived = ser.read().strip()
                byteReceived = byteReceived.strip()
                print("Waiting For S: " + byteReceived)
            except:
                pass
            
        ser.write('S')
        print("Sent S")
        
        tempStripHeight = 0
	stripHeights = []
	stripCount = 0
	numCaptures = 0

        while numCaptures < CAPTURES_PER_STRIP:
            
            while byteReceived != b'C':
                try:
                    byteReceived = ser.read().strip()
                    print("Waiting for C: " + byteReceived);
                    if (byteReceived == b'S'):
                        tempStripHeight = 0
                        stripHeights = []
                        stripCount = 0
                        numCaptures = 0
                        ser.write('S')
                except:
                    pass
            
            byteReceived = '0'            
            take_image()
            image = cv2.imread('green_tape.png')
            image = cv2.resize(image, (30, 30))
            image = mf.isolateColors(image, 10)
            
            if numCaptures == 0:
                fullArray = np.zeros((image.shape[0] * CAPTURES_PER_STRIP, image.shape[1]))
                    

            gray_image = mf.convertToGrayScale(image)
            mArray = mf.mapToBinary(gray_image)
            tempStripHeight = mf.detectStripHeight(mArray)
            
            if tempStripHeight != -1 :
                stripHeights.append(tempStripHeight + numCaptures*image.shape[1])
                stripCount += 1
            
            fullArray[image.shape[0]*numCaptures:image.shape[0]*(numCaptures + 1) - 1, 0:image.shape[1]-1] = mArray[0:image.shape[0] - 1, 0:image.shape[1] - 1]
                  		            
            numCaptures += 1


        while (byteReceived != b'R') and (byteReceived != b'S'):
             try:
                byteReceived = ser.read().strip()
                byteReceived = byteReceived.strip()
                print("Waiting for R or S: " + byteReceived)
             except:
                pass
        
        if (byteReceived == b'R'):
            
            byteReceived = '0'
            fullArray = mf.fillBetweenStrips(fullArray, stripCount, stripHeights)
            oneDArray = (mf.oneDimensionalize(fullArray));
            ser.write(oneDArray)
            print(oneDArray)
            print("Data Length: " +  str(len(oneDArray)));
     
