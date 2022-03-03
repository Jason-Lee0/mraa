#!/usr/bin/env python

# Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
# Copyright (c) 2018 Linaro Ltd.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Sends data through UART

import mraa
import time
import sys

# serial port
uart_port = 0 if len(sys.argv) < 2 else int(sys.argv[1])

data = 'Hello Mraa!'

# initialise UART
uart = mraa.Uart(uart_port)

# send data through UART
uart.write(bytearray(data, 'utf-8'))
