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
#include "ad4111.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static const struct gpio_dt_spec tx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);
//static const struct gpio_dt_spec rx_led = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);
static const struct gpio_dt_spec g_led = GPIO_DT_SPEC_GET(DT_NODELABEL(g), gpios);
static const struct gpio_dt_spec r_led = GPIO_DT_SPEC_GET(DT_NODELABEL(r), gpios);
static const struct gpio_dt_spec chipselect = GPIO_DT_SPEC_GET(DT_NODELABEL(chipselect1), gpios);
static const struct gpio_dt_spec miso_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(spiadcready), gpios);


static struct gpio_callback miso_cb_data;


//according to ad4111 faq, and datasheet this is the following way to configure:
//reset
//check ID
//configure ADC mode (not sure before or after which config) - I have not written a function for this!
//first configure channel (check channel registers on datasheet)
//then configure setup
//then configure filter
//done

//ad4111.h, a new header file i wrote contains helpful enums and structs




void adc_setup() { 

	//there are 15 channel numbers, note these are not the physical pins! Its an internal thing on the ADC, 
	ad4111_channel_t channel0;
	channel0.ch_id = CH0;
	channel0.ch_en = 1;
	channel0.setup_sel = 0;
	channel0.input = VIN0_VINCOM;


	//use this for voltage measurements, multiple channels can use the same setup
	ad4111_setup_t setup0;
	setup0.setup_id 	= SETUP_CFG0;
	setup0.bi_unipolar	= UNIPOLAR;
	setup0.refbuf_plus 	= 1;
	setup0.refbuf_minus = 1;
	setup0.inbuf		= AD4111_INBUF_DISABLED;
	setup0.ref_sel 		= INTERNAL_REF;


	/* 
	use this for current measurements
	
	ad4111_setup_t setup1;
	setup1.setup_id = 	...
	setup0.bi_unipolar	= UNIPOLAR;
	setup0.refbuf_plus 	= 0;
	setup0.refbuf_minus = 0;
	setup0.ref_sel 		= INTERNAL_REF;

*/

// filter(x) corresponds to setup(x)
// e.g. filter0 to setup0
ad4111_filter_t filter0;
filter0.filter_id 	= FILTER_CFG0;
filter0.enhfilten 	= AD4111_ENHFILT_DISABLED;
filter0.enhfilt 	= AD4111_ENHFILT_27SPS_47DB;
filter0.order 		= AD4111_FILTER_SINC5_SINC1;
filter0.odr 		= AD4111_ODR_200_SPS;


}



int spi_adc_int_count = 0;

void miso_interrupt_handler(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins) {
	spi_adc_int_count++;
}

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

	ret = gpio_pin_configure_dt(&chipselect, GPIO_OUTPUT_INACTIVE);

	gpio_pin_set_dt(&chipselect, 1);

	if (ret < 0) LOG_ERR("FAILED TO CONFIGURE LED");
	

	if (!gpio_is_ready_dt(&r_led)) LOG_ERR("LED NOT READY");


	gpio_pin_toggle_dt(&g_led);
	ret = gpio_pin_configure_dt(&r_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) LOG_ERR("FAILED TO CONFIGURE LED");
	gpio_pin_toggle_dt(&g_led);
	return ret;
}

int initialize_interrupt() {
	int ret;

	if (!gpio_is_ready_dt(&miso_spec)) {
		LOG_ERR("Error: button device %s is not ready\n",
		       miso_spec.port->name);
		return 1;
	}

	ret = gpio_pin_configure_dt(&miso_spec, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, miso_spec.port->name, miso_spec.pin);
		return 1;
	}

	// ret = gpio_pin_configure_dt(&miso_spec, GPIO_INPUT);
	ret = gpio_pin_interrupt_configure_dt(&miso_spec, GPIO_INT_EDGE_TO_INACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, miso_spec.port->name, miso_spec.pin);
		return 1;
	}
	gpio_init_callback(&miso_cb_data, miso_interrupt_handler, BIT(miso_spec.pin));
	gpio_add_callback(miso_spec.port, &miso_cb_data);
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

int main(void) {

	int ret;
	ret = initialize_pins();
	if (ret != 0) {
		LOG_ERR("FAILED TO CONFIGURE PINS");
		return 0;
	}
	ret = initialize_interrupt();
	if (ret != 0) {
		LOG_ERR("FAILED TO CONFIGURE INTERRUPT");
		return 0;
	}
	
	runsetupspi();

	flash_led();

	
	// init_can();
	// add_filter_can(can_rx_cb, test_filter, NULL);

	// init_sensors();
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
		// int y = gpio_pin_get_dt(&miso_spec);
		if (spi_adc_int_count > 0) {
			spi_adc_int_count = 0;
			gpio_pin_toggle_dt(&r_led);
		// if (y == 0) {
		// if (1) {
			// spi_adc_int_count = 0;
			uint8_t data[4];

			ret = gpio_pin_interrupt_configure_dt(&miso_spec, GPIO_INT_DISABLE);
			if (ret != 0) {
				printk("Error %d: failed to configure interrupt on %s pin %d\n",
					ret, miso_spec.port->name, miso_spec.pin);
				return 0;
			}

			runspitest(data);

			ret = gpio_pin_interrupt_configure_dt(&miso_spec, GPIO_INT_EDGE_TO_INACTIVE);
			if (ret != 0) {
				printk("Error %d: failed to configure interrupt on %s pin %d\n",
					ret, miso_spec.port->name, miso_spec.pin);
				return 0;
			}
			if(data[3] != 0) {
				gpio_pin_toggle_dt(&tx_led);
			}
			if(i > 0) {
			// if(i > 5000000) {
				// submit_can_pkt(data, 4);
				i = 0;
			}

		} 

		k_msleep(10);
		// uint8_t testing[]={0xAA,0xBB};
		// submit_can_pkt(testing, 2);
		// k_usleep(100);
	}

	// brown = VCC
	// blue = Iout


	return 0;
}