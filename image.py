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

	image = mf.isolateColors('tape2.jpg', 300, 300, 10)


	# check the accumulator is greater than 0, to avoid a divide by 0
	# if acc_count > 0:
		# calculate the mean x and y positions
		# mean_x = acc_x / acc_count
		# mean_y = acc_y / acc_count

	gray_image = mf.convertToGrayScale(image)
	mArray = mf.mapToBinary(gray_image)
		# # draw a small cross in red at the mean position
		# image[mean_x + 0, mean_y - 1] = 255, 0, 0
		# image[mean_x - 1, mean_y + 0] = 255, 0, 0
		# image[mean_x + 0, mean_y + 0] = 255, 0, 0
		# image[mean_x + 1, mean_y + 0] = 255, 0, 0
		# image[mean_x + 0, mean_y + 1] = 255, 0, 0
	# display the image on the viewer

	print (mArray)
	cv2.imshow("images", mArray)
	cv2.waitKey(0)
