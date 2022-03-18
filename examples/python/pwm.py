#!/usr/bin/env python

# Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
# Copyright (c) 2014 Intel Corporation.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Generates PWM at a step rate of 0.01 continuously.

import mraa
import time
import sys

pwm_num = 22 if len(sys.argv) < 2 else int(sys.argv[1])
pwm_period = 200 if len(sys.argv) < 3 else int(sys.argv[2])
value= 0.0

# chekc pin
print("Pin",pwm_num,":", mraa.getPinName(pwm_num))
print("Period:",pwm_period,"us")

# initialise PWM
x = mraa.Pwm(pwm_num)
time.sleep(0.05)

# initialise PWM Duty
x.write(value)
time.sleep(0.05)


# set PWM period
x.period_us(pwm_period)
time.sleep(0.05)

# enable PWM
x.enable(True)
time.sleep(0.05)

while True:
    # write PWM value
    x.write(value)

    time.sleep(0.05)

    value = value + 0.01
    if value >= 1:
        value = 0.0
