#!/usr/bin/python 

import glob
import cv2
import numpy
import os
import sys

controls = """
Controls:
Scale Image (use number keys):
    1: x0.1
    2: x0.2
    3: x0.3
    4: x0.4
    5: x0.5
    6: x0.6
    7: x0.7
    8: x0.8
    9: x0.9
    0: x1.0
Quit: q
"""

multiplier = 3

if len(sys.argv) < 2: exit(1)
print controls

os.chdir(sys.argv[1])
while True:

    # Find the latest file
    filename = sorted(glob.glob("*.bmp"))[-2]
    
    # Open it
    img     = cv2.imread(filename)
    
    # Resize it for viewing
    img_sm  = cv2.resize(img, (0,0), fx=0.1 * multiplier, fy=0.1 * multiplier) 
    
    # Show it
    cv2.imshow('Press q to exit',img_sm)
    
    # If 'q' key is pressed, exit
    key = cv2.waitKey(1) & 0xFF
    
    if key == ord('q'): break
    if key == ord('1'): multiplier = 1
    if key == ord('2'): multiplier = 2
    if key == ord('3'): multiplier = 3
    if key == ord('4'): multiplier = 4
    if key == ord('5'): multiplier = 5
    if key == ord('6'): multiplier = 6
    if key == ord('7'): multiplier = 7
    if key == ord('8'): multiplier = 8
    if key == ord('9'): multiplier = 9
    if key == ord('0'): multiplier = 10

cv2.destroyAllWindows()
