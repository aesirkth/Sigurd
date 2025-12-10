/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>

#include "main.h"
#include "sensors.h"


LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct gpio_dt_spec tx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);


int main(void)
{
	initializePins();

	return 0;
}

int initializePins(void) {

	int ret;

	if (!gpio_is_ready_dt(&tx_led))
		LOG_ERR("LED NOT READY");
	return 0;

	ret = gpio_pin_configure_dt(&tx_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
		LOG_ERR("FAILED TO CONFIGURE LED");
	return 0;

}