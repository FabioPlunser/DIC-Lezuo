/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <gpio.h>
#include <pthread.h> 


void main(void)
{
	printk("Lezuo Stinkt: Auf diesem Board: %s\n", CONFIG_BOARD);
}

