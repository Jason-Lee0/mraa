#!/usr/bin/env python

# Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
# Copyright (c) 2018 Linaro Ltd.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Reads maximum brightness value for user1 led and turns it
#                on/off depending on current state. Then sets led trigger
#                to heartbeat

import mraa
import time
import sys

led_num = 0 if len(sys.argv) < 2 else int(sys.argv[1])
led_val = 0 if len(sys.argv) < 3 else int(sys.argv[2])

# initialise LED
led_1 = mraa.Led(led_num)

# read maximum brightness
val = led_1.readMaxBrightness()

# turn led on/off depending on read max_brightness value
if val >= 1:
    print("maximum brightness value for LED is: %d" % val)
# never reached mostly
else:
    print("readMaxBrightness is not supported")

# set LED brightness
led_1.setBrightness(led_val)

# sleep for 1 seconds
time.sleep(1)

if led_1.trigger("heartbeat") == 0:
    print("led trigger set to: heartbeat")
else:
    print("Setting LED to heartbeat is not supported")
