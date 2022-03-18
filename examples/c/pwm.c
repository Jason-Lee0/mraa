/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Generates PWM signal of period 200us with variable dutycyle
 *                repeately. Press Ctrl+C to exit
 */

/* standard headers */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/pwm.h"

/* PWM declaration */
#define PWM_PIN 22

/* PWM period in us */
#define PWM_FREQ 200

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}

int
main(int argc, char *argv[])
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_pwm_context pwm;
    int pwm_pin, pwm_freq, pwm_duty;
    pwm_pin = (argc >= 2)?atoi(argv[1]):PWM_PIN;
    pwm_freq = (argc >= 3)?atoi(argv[2]):PWM_FREQ;
    printf("Will set PWM to %d and period to %d us\n", pwm_pin, pwm_freq);
    float value = 0.0f;
    float output;
    /* install signal handler */
    signal(SIGINT, sig_handler);
    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    pwm = mraa_pwm_init(pwm_pin);
    usleep(50000);
    if (pwm == NULL) {
        fprintf(stderr, "Failed to initialize PWM\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }
    
    /* Set PWM duty cyle */
    status = mraa_pwm_write(pwm, value);
    usleep(50000);
    if (status != MRAA_SUCCESS) {
        printf("Failed to setting duty\n");
        goto err_exit;
        
    }

    /* set PWM period */
    status = mraa_pwm_period_us(pwm, pwm_freq);
    usleep(50000);
    if (status != MRAA_SUCCESS) {
        printf("Failed to setting period\n");
        goto err_exit;
        
    }

    /* enable PWM */
    status = mraa_pwm_enable(pwm, 1);
    usleep(50000);
    if (status != MRAA_SUCCESS) {
        printf("Failed to setting enable\n");
        goto err_exit;
    }

    while (flag) {
        value = value + 0.01f;

        /* write PWM duty cyle */
        status = mraa_pwm_write(pwm, value);
        usleep(50000);
        if (status != MRAA_SUCCESS) {
            printf("Failed to setting value\n");
            goto err_exit;
        }

        if (value >= 1.0f) {
            value = 0.0f;
        }

        /* read PWM duty cyle */
        output = mraa_pwm_read(pwm);
        fprintf(stdout, "PWM value is %f\n", output);
    }
    printf("Close\n");
    /* close PWM */
    mraa_pwm_close(pwm);

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* close PWM */
    mraa_pwm_close(pwm);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
