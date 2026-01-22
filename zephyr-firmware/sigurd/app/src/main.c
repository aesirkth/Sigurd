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
#include <zephyr/drivers/pinctrl.h>
// #include <zephyr/dt-bindings/pinctrl/stm32-pinctrl.h>

#include "main.h"
#include "sensors.h"
#include "can_com.h"
#include "spitest.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static const struct gpio_dt_spec tx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
//static const struct gpio_dt_spec rx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);
static const struct gpio_dt_spec g_led = GPIO_DT_SPEC_GET(DT_NODELABEL(g), gpios);
static const struct gpio_dt_spec r_led = GPIO_DT_SPEC_GET(DT_NODELABEL(r), gpios);
static const struct gpio_dt_spec chipselect = GPIO_DT_SPEC_GET(DT_NODELABEL(chipselect1), gpios);
static const struct gpio_dt_spec miso_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(spiadcready), gpios);



// static const struct gpio_dt_spec miso_int = GPIO_DT_SPEC_GET(DT_NODELABEL(spiready1), gpios);
// static const struct gpio_dt_spec miso_spec = {
// 	.port = DEVICE_DT_GET(DT_NODELABEL(gpioa)),
// 	.pin = 6,
// 	.dt_flags = GPIO_ACTIVE_HIGH
// };

// static struct gpio_callback miso_cb;

// int spi_adc_int_count = 0;

// void miso_interrupt_handler(const struct device *const device, struct gpio_callback *cb, uint32_t pins) {
// 	printk("ADC data ready!\n");
// 	spi_adc_int_count++;
// 	// Handle ADC data ready
// }

// void setup_miso_interrupt(void) {
//     if (!device_is_ready(miso_spec.port)) {
//         printk("Error: MISO GPIO device not ready\n");
//         return;
//     }

//     int ret = gpio_pin_configure(miso_spec.port, miso_spec.pin, GPIO_INPUT);
//     if (ret < 0) {
//         printk("Error configuring MISO pin\n");
//         return;
//     }

//     gpio_init_callback(&miso_cb, miso_interrupt_handler, BIT(miso_spec.pin));
//     gpio_add_callback(miso_spec.port, &miso_cb);
//     gpio_pin_interrupt_configure(miso_spec.port, miso_spec.pin, GPIO_INT_EDGE_FALLING);
// }



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
	ret = gpio_pin_configure_dt(&tx_led, GPIO_OUTPUT_ACTIVE);

	ret = gpio_pin_configure_dt(&chipselect, GPIO_OUTPUT_INACTIVE);

	gpio_pin_set_dt(&chipselect, 1);

	if (ret < 0) LOG_ERR("FAILED TO CONFIGURE LED");
	

	if (!gpio_is_ready_dt(&r_led)) LOG_ERR("LED NOT READY");


	gpio_pin_toggle_dt(&g_led);
	ret = gpio_pin_configure_dt(&r_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) LOG_ERR("FAILED TO CONFIGURE LED");
	gpio_pin_toggle_dt(&g_led);

	ret = gpio_pin_configure_dt(&miso_spec, GPIO_INPUT);
	runsetupspi();

	gpio_pin_toggle_dt(&g_led);
	k_msleep(1020);
	gpio_pin_toggle_dt(&g_led);
	k_msleep(500);
	gpio_pin_toggle_dt(&g_led);
	k_msleep(250);
	gpio_pin_toggle_dt(&g_led);
	k_msleep(125);

	// setup_miso_interrupt();
	
	// init_can();
	// add_filter_can(can_rx_cb, test_filter, NULL);

	// init_sensors();
	int test_store = 42;	
	int i = 0;
	while(true) {
		i++;

		// gpio_pin_toggle_dt(&r_led);
		// if(spi_adc_int_count > 0) {
		// 	spi_adc_int_count = 0;
		// 	int x = runspitest();
		// 	gpio_pin_toggle_dt(&g_led);
		// }
		// gpio_pin_toggle_dt(&r_led);
		int y = gpio_pin_get_dt(&miso_spec);
		// if (spi_adc_int_count > 0) {
		if (y == 0) {
		// if (1) {
			// spi_adc_int_count = 0;
			int x = runspitest();
			if(x == 0xDE) {
				gpio_pin_toggle_dt(&r_led);
			}
			if(x == 0x00) {
				gpio_pin_toggle_dt(&tx_led);
			}
			if (x != 0x00) {
				test_store = x;
			}
		} 
		// if (i > 50) {
		// 	gpio_pin_set_dt(&tx_led, test_store == 0 ? 0 : 1);
		// 	gpio_pin_toggle_dt(&r_led);
		// 	i = 0;

		// }

		// k_msleep(500);
		// k_usleep(1);
	}

	// brown = VCC
	// blue = Iout


	return 0;
}