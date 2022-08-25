#!/usr/bin/env python
import freenect
import time
import threading
import random
import time

last_time = 0


def body(dev, ctx):
    global last_time
    if time.time() - last_time < 3:
        return
    last_time = time.time()
    led = random.randint(0, 6)
    tilt = random.randint(0, 30)
    freenect.set_led(dev, led)
    freenect.set_tilt_degs(dev, tilt)
    print('led[%d] tilt[%d] accel[%s]' % (led, tilt, freenect.get_accel(dev)))

freenect.runloop(body=body)
