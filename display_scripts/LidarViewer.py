#!/usr/bin/env python

import cv2
import numpy as np
import subprocess
import sys
import select
from time import sleep

WIDTH = 500
HEIGHT= 500
SCALE = 0.1

curindex = 0

filename = "/home/ubuntu/IRLLibs/data_storage/lidar_data.csv"

img_size = (WIDTH,HEIGHT)
img = np.ones(img_size) * 255

# polar to cartesian
def polar2cart(r, theta):
    x = r * np.cos(theta)
    y = r * np.sin(theta)
    return x, y

#This function turns points in plot to points in space
def cart2im(img,x,y):
    w = int(x + len(img)/2)
    h = int(-y + len(img[0])/2)
    return w,h

def wc():
    wcOut = subprocess.check_output(["wc", "-l", filename])
    temp = wcOut.split(" ")
    return int(temp[0])
    

#open file and read lidar data
mafile = open(filename, 'r')

while True:

    #if wc() - curindex > 4 :
    if True:
        mafile.readline()
        line = mafile.readline()
        stuff = line.split(",")
        stuff[1] = stuff[1].replace('\n','')

        #print curindex

        r = float(stuff[0])
        theta = float(stuff[1])
        curindex = curindex + 2

#    if r == -1:
#        img = np.ones(img_size) * 255
#        r = 0
#        theta = 0

    x,y = polar2cart(r * SCALE,theta)
    
    #print "x and y"
    #print x,y
    
    w,h = cart2im(img, x,y)

    #print "width and height:"
    #print w,h

    cv2.line(img, (w,h), (w,h), (0,0,0))
    cv2.imshow("Lidar",img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
         break
    #sleep(0.05)

