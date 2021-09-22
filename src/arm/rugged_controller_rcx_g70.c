 /*
 * Author: Chester Tseng <chester.tseng@adlinktech.com>
 * Copyright (c) 2021 ADLINK Technology Inc.
 * SPDX-License-Identifier: MIT

*/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <linux/i2c-dev.h>
#include "linux/gpio.h"

#include "common.h"
#include "gpio.h"
#include "arm/rugged_controller_rcx_g70.h"
#include "gpio/gpio_chardev.h"

#define SYSFS_CLASS_GPIO "/sys/class/gpio"

#define PLATFORM_NAME "RCX-G70"

#define MRAA_RCXG70_GPIOCOUNT 20
#define MRAA_RCXG70_UARTCOUNT 2
#define MRAA_RCXG70_LEDCOUNT 4

static char* RCXG70_LED[MRAA_RCXG70_LEDCOUNT] = { "LED1", "LED2", "LED3","LED4"};
static char* uart_name[MRAA_RCXG70_UARTCOUNT] = {"RS485", "RS232"};
static char* uart_path[MRAA_RCXG70_UARTCOUNT] = {"/dev/ttyTHS0", "/dev/ttyTHS1"};

static mraa_result_t mraa_roscube_set_pininfo(mraa_board_t* board, int mraa_index, char* name,
                                              mraa_pincapabilities_t caps, int sysfs_pin)
{
    if (mraa_index < board->phy_pin_count) {
        mraa_pininfo_t* pin_info = &board->pins[mraa_index];
        strncpy(pin_info->name, name, MRAA_PIN_NAME_SIZE);
        pin_info->capabilities = caps;
        if (caps.gpio) {
            pin_info->gpio.pinmap = sysfs_pin;
            pin_info->gpio.mux_total = 0;
        }
        if (caps.i2c) {
            pin_info->i2c.pinmap = 1;
            pin_info->i2c.mux_total = 0;
        }
        if (caps.uart) {
            pin_info->uart.mux_total = 0;
        }
        if (caps.spi) {
            pin_info->spi.mux_total = 0;
        }
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_result_t mraa_roscube_get_pin_index(mraa_board_t* board, char* name, int* pin_index)
{
    for (int i = 0; i < board->phy_pin_count; ++i) {
        if (strncmp(name, board->pins[i].name, MRAA_PIN_NAME_SIZE) == 0) {
            *pin_index = i;
            return MRAA_SUCCESS;
        }
    }

    syslog(LOG_CRIT, "RCX-G70: Failed to find pin name %s", name);

    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_result_t mraa_rcx_g70_uart(mraa_board_t* board, int index)
{
    if (index >= MRAA_RCXG70_UARTCOUNT)
        return MRAA_ERROR_INVALID_RESOURCE;
    board->uart_dev[index].index = index;
    board->uart_dev[index].device_path = uart_path[index];
    board->uart_dev[index].name = uart_name[index];
    board->uart_dev[index].tx = -1;
    board->uart_dev[index].rx = -1;
    board->uart_dev[index].cts = -1;
    board->uart_dev[index].rts = -1;
    return MRAA_SUCCESS;
}

mraa_board_t* mraa_rcx_g70()
{
    int i2c_bus_num;

    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof (mraa_board_t));

    if (b == NULL) {
        return NULL;
    }

    b->led_dev[0].name = (char*) RCXG70_LED[0];
    b->led_dev[1].name = (char*) RCXG70_LED[1];
    b->led_dev[2].name = (char*) RCXG70_LED[2];
    b->led_dev[3].name = (char*) RCXG70_LED[3];
    b->led_dev_count = MRAA_RCXG70_LEDCOUNT;
    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = MRAA_RCXG70_PINCOUNT;
    b->gpio_count = MRAA_RCXG70_GPIOCOUNT;
    b->chardev_capable = 0;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_RCXG70_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t *) calloc(1, sizeof (mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    b->adv_func->gpio_isr_replace = NULL;
    b->adv_func->gpio_close_pre = NULL;
    b->adv_func->gpio_init_pre = NULL;

    mraa_roscube_set_pininfo(b, 1,  "GPIO1",       (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 2,  "GPIO2",       (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 3,  "GPIO3",       (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 4,  "GPIO4",       (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 5,  "ISO_DI0",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 432);
    mraa_roscube_set_pininfo(b, 6,  "ISO_DI1",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 433);
    mraa_roscube_set_pininfo(b, 7,  "ISO_DO0",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 351);
    mraa_roscube_set_pininfo(b, 8,  "ISO_DO1",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 352);
    mraa_roscube_set_pininfo(b, 9,  "SYNC_PPS",    (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 444);
    mraa_roscube_set_pininfo(b, 10, "SYNC_OUT",    (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 434);
    mraa_roscube_set_pininfo(b, 11, "SYNC_IN",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 353);
    mraa_roscube_set_pininfo(b, 12, "SPI_CLK",     (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 13, "SPI_CS",      (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 14, "SPI_MISO",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 15, "SPI_MOSI",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 16, "RS485_TX",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_roscube_set_pininfo(b, 17, "RS485_RX",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_roscube_set_pininfo(b, 18, "RS485_RTS",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_roscube_set_pininfo(b, 19, "RS485_CTS",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_roscube_set_pininfo(b, 20, "RS232_TX",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_roscube_set_pininfo(b, 21, "RS232_RX",    (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_roscube_set_pininfo(b, 22, "RS232_RTS",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_roscube_set_pininfo(b, 23, "RS232_CTS",   (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 }, -1);
    mraa_roscube_set_pininfo(b, 24, "I2C_CLK",     (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 25, "I2C_SDA",     (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 }, -1);
    mraa_roscube_set_pininfo(b, 26, "FPGA_TDI",    (mraa_pincapabilities_t){ 1, 1, 0, 1, 0, 0, 0, 0 }, 420);
    mraa_roscube_set_pininfo(b, 27, "FPGA_TDO",    (mraa_pincapabilities_t){ 1, 1, 0, 1, 0, 0, 0, 0 }, 423);
    mraa_roscube_set_pininfo(b, 28, "FPGA_TMS",    (mraa_pincapabilities_t){ 1, 1, 0, 1, 0, 0, 0, 0 }, 412);
    mraa_roscube_set_pininfo(b, 29, "FPGA_TCK",    (mraa_pincapabilities_t){ 1, 1, 0, 1, 0, 0, 0, 0 }, 417);
   
    b->uart_dev_count = MRAA_RCXG70_UARTCOUNT;
    b->uart_dev[0].index = 0;
    b->uart_dev[0].device_path = uart_path[0];
    b->uart_dev[0].name = uart_name[0];
    mraa_roscube_get_pin_index(b, "RS485_TX",  &(b->uart_dev[0].tx));
    mraa_roscube_get_pin_index(b, "RS485_RX",  &(b->uart_dev[0].rx));
    mraa_roscube_get_pin_index(b, "RS485_CTS",  &(b->uart_dev[0].cts));
    mraa_roscube_get_pin_index(b, "RS485_RTS",  &(b->uart_dev[0].rts));

    b->uart_dev[0].index = 1;
    b->uart_dev[0].device_path = uart_path[1];
    b->uart_dev[0].name = uart_name[1];
    mraa_roscube_get_pin_index(b, "RS232_TX",  &(b->uart_dev[0].tx));
    mraa_roscube_get_pin_index(b, "RS232_RX",  &(b->uart_dev[0].rx));
    mraa_roscube_get_pin_index(b, "RS232_CTS",  &(b->uart_dev[0].cts));
    mraa_roscube_get_pin_index(b, "RS232_RTS",  &(b->uart_dev[0].rts));
    b->def_uart_dev = 0;

    // Configure SPI #1 CS0
    b->spi_bus_count = 1;
    b->spi_bus[0].bus_id = 1;
    b->spi_bus[0].slave_s = 0; 
    mraa_roscube_get_pin_index(b, "SPI_CS",  &(b->spi_bus[0].cs));
    mraa_roscube_get_pin_index(b, "SPI_MOSI", &(b->spi_bus[0].mosi));
    mraa_roscube_get_pin_index(b, "SPI_MISO", &(b->spi_bus[0].miso));
    mraa_roscube_get_pin_index(b, "SPI_CLK",  &(b->spi_bus[0].sclk));

    b->i2c_bus_count = 0;
    b->def_i2c_bus = 0;
    i2c_bus_num = mraa_find_i2c_bus("31b0000.i2c",0);
    if (i2c_bus_num != -1) {
        b->i2c_bus[0].bus_id = i2c_bus_num;
        mraa_roscube_get_pin_index(b, "I2C_SDA", (int*) &(b->i2c_bus[0].sda));
        mraa_roscube_get_pin_index(b, "I2C_CLK", (int*) &(b->i2c_bus[0].scl));
        b->i2c_bus_count = 1;
    }
    return b;
error:
    syslog(LOG_CRIT, "RCX-G70: Platform failed to initialise");
    free(b);
    return NULL;
}
