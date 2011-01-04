#!/usr/bin/env python
import freenect
import cv
import numpy as np

cv.NamedWindow('Depth')
cv.NamedWindow('Video')
while 1:
    depth, timestamp = freenect.sync_get_depth()
    rgb, timestamp = freenect.sync_get_video()
    cv.ShowImage('Depth', depth.astype(np.uint8))
    cv.ShowImage('Video', rgb[:, :, ::-1].astype(np.uint8))
    cv.WaitKey(10)
