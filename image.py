# Import the necessary packages
import numpy as np
import myFunc as mf
import picamera
import time
import cv2
import serial
import time
import struct

ser = serial.Serial('/dev/ttyACM0',9600, timeout=1)
byteReceived = '0'

def take_image():
    
        with picamera.PiCamera() as camera:
            camera.resolution = (300, 268)    
            camera.capture("green_tape.png")


while True:
	
	
	# Run when Arduino signals a new strip
	# x and y position accumulators
	# acc_x = 0
	# acc_y = 0

	# number of pixels accumulated
	# acc_count = 0
	CAPTURES_PER_STRIP = 5
    
	tempStripHeight = 0
	stripHeights = []
	stripCount = 0
	numCaptures = 0
	
	#This will be sent to Arduino

	while numCaptures < CAPTURES_PER_STRIP:
            
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
            fullArray[image.shape[1]*numCaptures:image.shape[0]*(numCaptures + 1) - 1, 0:image.shape[1]-1] = mArray[0:image.shape[0] - 1, 0:image.shape[1] - 1]
            		

            #fullArray = mf.oneDimensionalize(fullArray);
            numCaptures += 1
        #fullArray = mf.oneDimensionalize(fullArray)
        fullArray = mf.fillBetweenStrips(fullArray, stripCount, stripHeights)    
        print(mf.oneDimensionalize(fullArray))
        while byteReceived != b'R':
             try:
                byteReceived = ser.read()
                byteReceived = byteReceived.strip()
            
             except:
                pass
        byteReceived = '0'
        ser.write(mf.oneDimensionalize(fullArray));
	cv2.imshow("images", fullArray)
	cv2.waitKey(0)
