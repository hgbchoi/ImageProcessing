# Import the necessary packages
import numpy as np
import myFunc as mf
import cv2

while True:
	
	
	# Run when Arduino signals a new strip
	# x and y position accumulators
	# acc_x = 0
	# acc_y = 0

	# number of pixels accumulated
	# acc_count = 0
	CAPTURES_PER_STRIP = 5

	image = cv2.imread('tape.png')
	image = cv2.resize(image, (100, 100))
	image = mf.isolateColors(image, 10)
	tempStripHeight = 0
	stripHeights = []
	stripCount = 0
	numCaptures = 0
	
	#This will be sent to Arduino
	fullArray = np.zeros((image.shape[1] * CAPTURES_PER_STRIP, image.shape[0]))

	while numCaptures < CAPTURES_PER_STRIP:		
		# image = cv2.imread('tape.png')
		# image = cv2.resize(image, (100,100))
		# height, width, channels = image.shape
		# test = np.zeros((height * CAPTURES_PER_STRIP, width))

	# check the accumulator is greater than 0, to avoid a divide by 0
	# if acc_count > 0:
		# calculate the mean x and y positions
		# mean_x = acc_x / acc_count
		# mean_y = acc_y / acc_count

		gray_image = mf.convertToGrayScale(image)
		mArray = mf.mapToBinary(gray_image)
		tempStripHeight = mf.detectStripHeight(mArray)
		if tempStripHeight != -1 :
			stripHeights.append(tempStripHeight + numCaptures*image.shape[1])
			stripCount += 1
		fullArray[image.shape[1]*numCaptures:image.shape[1]*(numCaptures + 1) - 1, 0:image.shape[0]-1] = mArray[0:image.shape[1] - 1, 0:image.shape[0] - 1]
		
		# # draw a small cross in red at the mean position
		# image[mean_x + 0, mean_y - 1] = 255, 0, 0
		# image[mean_x - 1, mean_y + 0] = 255, 0, 0
		# image[mean_x + 0, mean_y + 0] = 255, 0, 0
		# image[mean_x + 1, mean_y + 0] = 255, 0, 0
		# image[mean_x + 0, mean_y + 1] = 255, 0, 0
	# display the image on the viewer

		numCaptures += 1

	#fullArray = mf.fillBetweenStrips(fullArray,stripCount,stripHeights)	
	cv2.imshow("images", fullArray)
	cv2.waitKey(0)
