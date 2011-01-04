#!/usr/bin/env python
import freenect
import cv
import numpy as np

cv.NamedWindow('Depth')
cv.NamedWindow('RGB')

def display_depth(dev, data, timestamp):
    data -= np.min(data.ravel())
    data *= 65536 / np.max(data.ravel())
    image = cv.CreateImageHeader((data.shape[1], data.shape[0]),
                                 cv.IPL_DEPTH_16U,
                                 1)
    cv.SetData(image, data.tostring(),
               data.dtype.itemsize * data.shape[1])
    cv.ShowImage('Depth', image)
    cv.WaitKey(5)


def display_rgb(dev, data, timestamp):
    image = cv.CreateImageHeader((data.shape[1], data.shape[0]),
                                 cv.IPL_DEPTH_8U,
                                 3)
    # Note: We swap from RGB to BGR here
    cv.SetData(image, data[:, :, ::-1].tostring(),
               data.dtype.itemsize * 3 * data.shape[1])
    cv.ShowImage('RGB', image)
    cv.WaitKey(5)

freenect.runloop(depth=display_depth,
                 video=display_rgb)
