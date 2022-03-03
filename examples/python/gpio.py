#!/usr/bin/env python

# Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
# Copyright (c) 2018 Linaro Ltd.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Toggles two GPIO pins continuously in an alternative pattern

import mraa
import time
import sys

gpio_num1 = 7 if len(sys.argv) < 2 else int(sys.argv[1])
gpio_num2 = 8 if len(sys.argv) < 3 else int(sys.argv[2])

# initialise gpio 1
gpio_1 = mraa.Gpio(gpio_num1)

# initialise gpio 2
gpio_2 = mraa.Gpio(gpio_num2)

# set gpio 1 to output
gpio_1.dir(mraa.DIR_OUT)

# set gpio 2 to output
gpio_2.dir(mraa.DIR_OUT)

# toggle both gpio's
while True:
    gpio_1.write(1)
    gpio_2.write(0)

    time.sleep(1)

    gpio_1.write(0)
    gpio_2.write(1)

    time.sleep(1)
