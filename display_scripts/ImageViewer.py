#!/usr/bin/env python
import glob
import cv2
import numpy
import os

#images = glob.glob("/home/ubuntu/IRLLibs/data_storage/pics/*.bmp")
#images.sort()
#for image in images:
#    img = cv2.imread(image)
#    cv2.imshow('frame',img)
#    if cv2.waitKey(1) & 0xFF == ord('q'):
#        break

#cv2.destroyAllWindows()
os.chdir("/home/ubuntu/IRLLibs/data_storage/pics")
while True:
    image = sorted(glob.glob("*.bmp"))[-2]
    img = cv2.imread(image)
    cv2.imshow('UAV',img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
         break

cv2.destroyAllWindows()
