#ifndef AD4111_H
#define AD4111_H

#include <stdint.h>

int ad4111_init(void);

/*
 * Reports the status of the AD4111 driver.
 * @returns 1 If things are working good :)
 *          0 If things are working bad  :(
 */
int ad4111_get_status(void);

/*
 * Get the next conversion produced by the AD4111. 
 * @param code pointer to a value that should contain the next conversion.
 * @returns >=0: channel
 *          <0 : something has gone wrong.
*/
int ad4111_get_data_blocking(uint32_t *code);

static inline float data_to_volts_unipolar(int32_t data) {
	float code = data;
	float vref = 2.5;
	float two_to_the_power_24 = 2 << (24 - 1);
	float volts = (code * vref) / (0.1f * two_to_the_power_24);
	return volts;
}

// static inline float data_to_volts_bipolar(int32_t data) {
// 	float code = data;
// 	float vref = 2.5;
// 	float two_to_the_power_23 = 2 << (23 - 1);
// 	float volts = (code/two_to_the_power_23-1) * vref/0.1f;
// 	return volts;
// }

static inline float data_to_amps_bipolar(int32_t data) {
	float code = data;
	float vref = 2.5;
	float two_to_the_power_23 = 2 << (23 - 1);
	float amps = (code/two_to_the_power_23-1) * vref/50;
	return amps;
}

static inline float data_to_volts_bipolar(int32_t data) {
	float code = data;
	float vref = 2.5;
	float two_to_the_power_23 = 2 << (23 - 1);
	// float volts = (code/two_to_the_power_23-1) * vref/0.1f;
	float volts = 10*(1 - code/two_to_the_power_23)*vref;
	// (1 - codeneg24/2**23)*vref
	return volts;
}

#endif
