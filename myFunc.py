import numpy as np
import cv2


def isolateColors(image, thresh):
    # grab an image from the camera        

    
    height, width, channels = image.shape
	# iterate over every pixel
    
    for x in range(0 , width):
        for y in range(0, height):
            blue, green, red = image[y, x]		
            if green > red - 7 + thresh and green > blue + thresh :
                image[y, x] = 1, 1, 1
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
            if anArray[y, x] == 1 and anArray[y + 1, x + 1] == 1 and anArray[y + 1, x] == 1 and anArray[y, x + 1] == 1:
                return y
    return -1

def detectStripIndex(anArray):
    for i in range(len(anArray)):
        if (anArray[i] == 1):
                return i;
            
    return -1;

def oneDimensionalize(anArray):
    
    oneDimensional = []
    for i in range (0, anArray.shape[0]) :
            if (anArray[i, :].mean() > 0.2) :
                oneDimensional.append(1)
            else :
                oneDimensional.append(0)
    return oneDimensional
    #Will take processed array as param of the whole strip an return an array of strip heights'
    
def listify(anArray):
    
    oneDimensional = []
    for i in range (0, len(anArray)) :
            if (anArray[i] == 1) :
                oneDimensional.append(1)
            else :
                oneDimensional.append(0)
    return oneDimensional

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

def fillBetweenStrips1D(anArray, stripCount, stripHeights):

    strips = 0
    while stripCount >= 2:
        for i in range (stripHeights[strips], stripHeights[strips + 1]):
            anArray[i] = 1
        stripCount -= 2
        strips += 2
    return anArray