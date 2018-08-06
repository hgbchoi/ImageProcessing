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

def take_image():
    
        with picamera.PiCamera() as camera:
           
           
            camera.resolution = (200,100)
            camera.start_preview()
            camera.capture("green_tape.png")



while True:
	
	CAPTURES_PER_STRIP = 6
    
	
	
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
            image = cv2.resize(image, (30, 15))
            image = mf.isolateColors(image, 10)
            height, width, channels = image.shape;
            
            if numCaptures == 0:
                fullArray = np.zeros((height * CAPTURES_PER_STRIP))
                    

            gray_image = mf.convertToGrayScale(image)
            gray_image = mf.oneDimensionalize(gray_image)
            print('*******************************')
            print(gray_image)
            #gray_image = mf.reverseArray(gray_image)
            gray_image.reverse()
            print(gray_image)
            print('*******************************')
            tempStripHeight = mf.detectStripIndex(gray_image)
            
            if tempStripHeight != -1 :
                stripHeights.append(tempStripHeight + numCaptures*height)
                stripCount += 1
            
            fullArray[height*numCaptures:height*(numCaptures + 1)] = gray_image[0:height]
                  		            
            numCaptures += 1

        while (byteReceived != b'D') and (byteReceived != b'S'):
             try:
                byteReceived = ser.read().strip()
                byteReceived = byteReceived.strip()
                print("Waiting for D: " + byteReceived)
             except:
                pass
        if (byteReceived == b'D'):
        	ser.write(b'D')
        while (byteReceived != b'R') and (byteReceived != b'S'):
             try:
                byteReceived = ser.read().strip()
                byteReceived = byteReceived.strip()
                print("Waiting for R or S: " + byteReceived)
             except:
                pass
        
        if (byteReceived == b'R'):
            
            byteReceived = '0'
            oneDArray = mf.fillBetweenStrips1D(fullArray, stripCount, stripHeights)
            oneDArray = mf.listify(oneDArray)
            ser.write(oneDArray)
            print(oneDArray)
            print("Data Length: " +  str(len(oneDArray)))
     
  
