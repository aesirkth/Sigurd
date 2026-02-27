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
// #include "spitest.h"
#include "ad4111.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static const struct gpio_dt_spec tx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
//static const struct gpio_dt_spec rx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);
static const struct gpio_dt_spec g_led = GPIO_DT_SPEC_GET(DT_NODELABEL(g), gpios);
static const struct gpio_dt_spec r_led = GPIO_DT_SPEC_GET(DT_NODELABEL(r), gpios);
static const struct gpio_dt_spec chipselect = GPIO_DT_SPEC_GET(DT_NODELABEL(chipselect1), gpios);

/*
static const struct gpio_dt_spec miso_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(spiadcready), gpios);


static struct gpio_callback miso_cb_data;
*/

//according to ad4111 faq, and datasheet this is the following way to configure:
//reset
//check ID
//configure ADC mode (not sure before or after which config) - I have not written a function for this!
//first configure channel (check channel registers on datasheet)
//then configure setup
//then configure filter
//done

//ad4111.h, a new header file i wrote contains helpful enums and structs

/*
int spi_adc_int_count = 0;

void miso_interrupt_handler(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins) {
	spi_adc_int_count++;
}
*/

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

int initialize_pins(void) { 
	int ret;

	if (!gpio_is_ready_dt(&g_led)) LOG_ERR("LED NOT READY");

	ret = gpio_pin_configure_dt(&g_led, GPIO_OUTPUT_ACTIVE);
	ret = gpio_pin_configure_dt(&tx_led, GPIO_OUTPUT_ACTIVE);

	ret = gpio_pin_configure_dt(&chipselect, GPIO_OUTPUT_INACTIVE); // TODO: Move this ad4111.c
	gpio_pin_set_dt(&chipselect, 1);

	if (ret < 0) LOG_ERR("FAILED TO CONFIGURE LED");

	if (!gpio_is_ready_dt(&r_led)) LOG_ERR("LED NOT READY");

	gpio_pin_toggle_dt(&g_led);
	ret = gpio_pin_configure_dt(&r_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) LOG_ERR("FAILED TO CONFIGURE LED");
	gpio_pin_toggle_dt(&g_led);
	return ret;
}

void flash_led(void) { 
	gpio_pin_toggle_dt(&g_led);
	k_msleep(400);
	gpio_pin_toggle_dt(&g_led);
	k_msleep(300);
	gpio_pin_toggle_dt(&g_led);
	k_msleep(200);
	gpio_pin_toggle_dt(&g_led);
	k_msleep(100);
}

void error_sequence_led(void) { 
	for (int i = 0; i < 50; i++) {
		gpio_pin_toggle_dt(&r_led);
		k_msleep(100);
	}
}

// Convert an arry of floats into a packed array of 2 bytes
void floats_to_bytes(uint8_t *bytes, float *floats, int n, float factor) {
	for (int i = 0; i < n; i++) {
		uint16_t value = factor * floats[i];
		// uint16_t value = floats[i]*10000-545;
		bytes[2*i] = (value >> 8 & 0xFF);
		bytes[2*i+1] = (value & 0xFF);
	}
}

void floats_to_degrees(uint8_t *bytes, float *floats, int n) {
	for (int i = 0; i < n; i++) {
		// uint16_t value = factor * floats[i];
		// uint16_t value = floats[i]*10000-545;
		uint16_t value = floats[i]*10000-545;
		bytes[2*i] = (value >> 8 & 0xFF);
		bytes[2*i+1] = (value & 0xFF);
	}
}

int main(void) {

	int ret;
	ret = initialize_pins();
	if (ret != 0) {
		LOG_ERR("FAILED TO CONFIGURE PINS");
		error_sequence_led();
		return 0;
	}
	// ret = initialize_interrupt();
	// if (ret != 0) {
	// 	LOG_ERR("FAILED TO CONFIGURE INTERRUPT");
	// 	return 0;
	// }
	
	// runsetupspi();
	flash_led();

	// ret = ad4111_init();
	// if (ret !=0) {
	// 	error_sequence_led();
	// 	return 0;
	// }

	init_can();
	// uint8_t batch_data[20]; // data from a full batch of all channels, scaled to nV
	// int16_t channel_readout_stat = 0; // is 0000001111111111 when all 10 channels are read out

	// struct running_average adc_running_averages[10];

	init_sensors();
	float V_TO_mV_factor = 1000, A_TO_uA_factor = 1000000;
	while(true) {
		k_msleep(1000);
		// Thermalcouples CAN message
		float thermocouple_voltages[4];
		uint8_t thermocouple_bytes[8];
		ret = thermocouple_get_voltages(thermocouple_voltages);
		if(ret < 0) {
			error_sequence_led();
			continue;
		}
		floats_to_bytes(thermocouple_bytes, thermocouple_voltages, 4, V_TO_mV_factor);
		// submit_can_pkt(0x120, thermocouple_bytes, 8);

		// Pressure guages CAN message
		float pressure_gauges_currents[4];
		uint8_t pressure_guages_bytes[8];
		ret = pressure_gauges_get_currents(pressure_gauges_currents);
		if(ret < 0) {
			error_sequence_led();
			continue;
		}
		floats_to_bytes(pressure_guages_bytes, pressure_gauges_currents, 4, A_TO_uA_factor);
		// submit_can_pkt(0x121, pressure_guages_bytes, 8);

		// RTDs CAN message
		float rtd_voltages[2];
		uint8_t rtd_bytes[4];
		ret = rtds_get_voltages(rtd_voltages);
		if(ret < 0) {
			error_sequence_led();
			continue;
		}
		floats_to_degrees(rtd_bytes, rtd_voltages, 2);
		submit_can_pkt(0x122, rtd_bytes, 4);
	}
	
	return 0;
}
