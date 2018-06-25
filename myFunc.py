import numpy as np
import cv2

def isolateColors(imageName, resizeX, resizeY, thresh):
    # grab an image from the camera
    image = cv2.imread(imageName)
    image = cv2.resize(image, (resizeX,resizeY))
    height, width, channels = image.shape
	# iterate over every pixel
    for x in range(0, width-1):
           for y in range(0, height-1):
			# get the value of the current pixel
            red, green, blue = image[y, x]

			# check if the red intensity is greater than green and blue
            if green > red + thresh and green > blue + thresh:
				# accumulate x and y of found pixel
				# acc_x += x
				# acc_y += y
				# increment the accumulated pixels' count
				# acc_count += 1
				# colour pixels which pass the test black
                image[y, x] = 255, 255, 255
            else :
                image[y, x] = 0, 0, 0
    return image

def mapToBinary(image) :

    return image/255    
   

def convertToGrayScale(image):
    return cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

def detectStripHeight(anArray):
    print('TO DO')
    

def fillBetweenStrips(anArray, stripCount, stripHeights):

    width = anArray.shape[1]
    strips = 0
    while stripCount >= 2:
        for i in range (stripHeights[strips], stripHeights[strips + 1]):
            for j in range (0, width):
                anArray[i, j] = 1
        stripCount -= 2
        strips += 2
    return anArray


