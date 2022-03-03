#!/usr/bin/env python

# Author: ChenYing Kuo <chenying.kuo@adlinktech.com>
# Copyright (c) 2022 ADLINK Technology Inc.
#
# SPDX-License-Identifier: MIT
#

import mraa
import sys
import time

i2c_bus_num = 0 if len(sys.argv) < 2 else int(sys.argv[1])
i2c_dev_addr = 0x50 if len(sys.argv) < 3 else int(sys.argv[2], 16)

# initialise I2C
print("Initial i2c-bus %d, device address 0x%02x" % (i2c_bus_num, i2c_dev_addr))
x = mraa.I2c(i2c_bus_num)
x.address(i2c_dev_addr)

message = 2
addr = 0x00

while True:
    print("message write to 0x%02x is 0x%02x" % (addr, message))
    x.writeReg(addr, message)
    time.sleep(1)
    val = x.readReg(addr)
    print("message read from 0x%02x is 0x%02x" % (addr, val))
    message += 3
    addr += 1
