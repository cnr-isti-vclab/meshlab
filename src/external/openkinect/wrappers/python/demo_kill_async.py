#!/usr/bin/env python
import freenect
import time


def body(dev, ctx):
    if time.time() - start_time > 1.:
        print('1 sec passed, killing')
        raise freenect.Kill

start_time = time.time()
freenect.runloop(body=body)
start_time = time.time()
freenect.runloop(body=body)
