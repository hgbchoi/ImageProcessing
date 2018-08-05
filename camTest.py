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

def take_image():
    
        with picamera.PiCamera() as camera:
           
           
            camera.resolution = (200,100)
            camera.start_preview()
     

while True:
    take_image()