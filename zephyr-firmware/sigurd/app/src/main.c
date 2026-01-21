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
#include "spitest.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static const struct gpio_dt_spec tx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
//static const struct gpio_dt_spec rx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);
static const struct gpio_dt_spec g_led = GPIO_DT_SPEC_GET(DT_NODELABEL(g), gpios);
static const struct gpio_dt_spec r_led = GPIO_DT_SPEC_GET(DT_NODELABEL(r), gpios);

bool test_led_flag = false;


const struct can_filter test_filter = {
    .flags = 0,
    .id = 0x122,
    .mask = 0b11111111111 
};

void can_rx_cb(const struct device *const device, struct can_frame *frame, void *user_data) {
	// do something in here.
	// but dont use k_msleep!

	if (frame->dlc != 1) {
        LOG_ERR("received packet with id %d has length %d. Sigurd expects all packets to have size 1.", frame->id, frame->dlc);
    }

	test_led_flag = true;

}

int main(void) {

	int ret;
	
	if (!gpio_is_ready_dt(&g_led)) LOG_ERR("LED NOT READY");

	ret = gpio_pin_configure_dt(&g_led, GPIO_OUTPUT_ACTIVE);
	
	if (ret < 0) LOG_ERR("FAILED TO CONFIGURE LED");
	

	if (!gpio_is_ready_dt(&r_led)) LOG_ERR("LED NOT READY");

	ret = gpio_pin_configure_dt(&r_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) LOG_ERR("FAILED TO CONFIGURE LED");


	
	// init_can();
	// add_filter_can(can_rx_cb, test_filter, NULL);

	// init_sensors();

	while(true) {
		int x = runspitest();
		// gpio_pin_toggle_dt(&r_led);
		if(x == 50) {
			gpio_pin_toggle_dt(&g_led);
		} else {
			// gpio_pin_toggle_dt(&r_led);
		}


		k_msleep(12);
	}

	// brown = VCC
	// blue = Iout


	return 0;
}