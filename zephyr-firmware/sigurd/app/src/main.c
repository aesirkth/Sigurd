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
#include "can_com.h"


LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct gpio_dt_spec tx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);

void can_rx_cb(const struct device *const device, struct can_frame *frame, void *user_data) {
	// do something in here.
	// but dont use k_msleep!
}

const struct can_filter filter = {
    .flags = 0,
    .id = 0x124,
    .mask = 0b11111111111 
};


int main(void) {
	initializePins();

	// Uncomment these lines to enable can
	// init_can();
	// add_filter_can(can_rx_cb, filter, NULL);

	while(true) {
		gpio_pin_toggle_dt(&tx_led);
	}

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