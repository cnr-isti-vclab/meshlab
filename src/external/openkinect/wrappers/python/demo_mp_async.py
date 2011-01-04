#!/usr/bin/env python
import freenect
import matplotlib.pyplot as mp

mp.ion()
image_rgb = None
image_depth = None


def display_depth(dev, data, timestamp):
    global image_depth
    mp.figure(1)
    if image_depth:
        image_depth.set_data(data)
    else:
        image_depth = mp.imshow(data, interpolation='nearest', animated=True)
    mp.draw()


def display_rgb(dev, data, timestamp):
    global image_rgb
    mp.figure(2)
    if image_rgb:
        image_rgb.set_data(data)
    else:
        image_rgb = mp.imshow(data, interpolation='nearest', animated=True)
    mp.draw()

freenect.runloop(depth=display_depth,
                 video=display_rgb)
