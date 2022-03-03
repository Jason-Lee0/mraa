/*
 * Author: Jeffrey Chang <chih-chieh.chang@adlinktech.com>
 * Copyright (c) 2019 ADLINK Technology Inc.
 *
 * SPDX-License-Identifier: MIT
 */

/* standard headers */
#include <endian.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/i2c.h"

#define I2C_BUS 0
#define ADDR_1 0x50

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}

int16_t
i2c_read_word(mraa_i2c_context dev, uint8_t command)
{
    return be16toh(mraa_i2c_read_word_data(dev, command));
}

int
main(int argc, char *argv[])
{
    mraa_result_t status = MRAA_SUCCESS;
    uint8_t data;
    int message = 2;
    int addr = 0x00;
    int msg ;
    mraa_i2c_context i2c;
    int i2c_bus_num = (argc >= 2)?atoi(argv[1]):I2C_BUS;
    int i2c_dev_addr = (argc >= 3)?strtol(argv[2], NULL, 16):ADDR_1;
    fprintf(stderr, "We'll operate on i2c-bus %d, device address 0x%02x\n", i2c_bus_num, i2c_dev_addr);

    /* install signal handler */
    signal(SIGINT, sig_handler);

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    /* initialize I2C bus */
    i2c = mraa_i2c_init(i2c_bus_num);
    if (i2c == NULL) {
        fprintf(stderr, "Failed to initialize I2C\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* set slave address */
    status = mraa_i2c_address(i2c, i2c_dev_addr);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    while (flag) {
        mraa_i2c_write_byte_data(i2c, message, addr);
        sleep(1);
        msg = mraa_i2c_read_byte_data(i2c, addr);

        fprintf(stdout, "massage write to 0x%02x is 0x%02x\n", addr, message );
        fprintf(stdout, "message read from 0x%02x is 0x%02x\n\n", addr, msg);

        message += 3;
        addr += 1;
        if (addr== 0xff+1){
            addr = 0x00;
        }
    }

    /* stop i2c */
    mraa_i2c_stop(i2c);

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* stop i2c */
    mraa_i2c_stop(i2c);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
