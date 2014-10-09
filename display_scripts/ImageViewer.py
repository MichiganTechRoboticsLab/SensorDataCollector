#!/usr/bin/python

import glob
import cv2
import numpy
import os

os.chdir("data_storage/pics")
while True:

	# Find the latest file
    filename = sorted(glob.glob("*.bmp"))[-2]
	
	# Open it
    img     = cv2.imread(filename)
	
	# Resize it for viewing
    img_sm  = cv2.resize(img, (0,0), fx=0.3, fy=0.3) 
	
	# Show it
    cv2.imshow('RGB',img_sm)
	
	# If 'q' key is pressed, exit
    if cv2.waitKey(1) & 0xFF == ord('q'): break

cv2.destroyAllWindows()
