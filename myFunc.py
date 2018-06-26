import numpy as np
import cv2


def isolateColors(image, thresh):
    # grab an image from the camera        

    
    height, width, channels = image.shape
	# iterate over every pixel
    
    for x in range(0 , width-1):
        for y in range(0, height-1):
	    #get the value of the current pixel
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

def mapToBinary(anArray):
    return np.ceil(anArray/255)    
   

def convertToGrayScale(image):
    return cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

def detectStripHeight(anArray):
    for x in range(anArray.shape[1]-1):
        for y in range(anArray.shape[0]-1):
            if anArray[x, y] == 1 and anArray[x + 1, y + 1] == 1:
                return x
    return -1
             


    #Will take processed array as param of the whole strip an return an array of strip heights'
    


#anArray: fully processed array matching the height of the full cycle
#stripCount: How many tape strips are in the cycle
#stripHeights: A list of strip positions

#This function works based on the assumption that it will encounter only a single strip in a full cycle (covering a wall plug) OR contains only pairs of strips
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


