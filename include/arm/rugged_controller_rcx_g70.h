/*
 * Author: Author: Chester Tseng <chester.tseng@adlinktech.com>
 * Copyright (c) 2021 ADLINK Technology Inc.
 * SPDX-License-Identifier: MIT   
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

// +1 as pins are "1 indexed"
#define MRAA_RCXG70_PINCOUNT    (29 + 1)

mraa_board_t*
mraa_rcx_g70();

#ifdef __cplusplus
}
#endif
