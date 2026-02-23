#ifndef __SENSORS_H
#define __SENSORS_H

int init_sensors(void);
int sensors_get_status(void);
/*
 * Get the running average voltage of the four thermocouples and clear the running average buffer.
 * Note: This is a blocking function!
 * @param averages A pointer to an array of four floats where the data will be written.
 * @returns error_code 0  => it all worked
 *                     <0 => there was a thermocouple with no conversions
 *          -error_code = bxxx0 means thermocouple 1 had an error
 *          -error_code = bxx0x means thermocouple 2 had an error
 *                        etc....
 */
int thermocouple_get_voltages(float *voltages);
/*
 * Same as thermocouple_get_voltages but for pressure gauges
 */
int pressure_gauges_get_currents(float *currents);
/*
 * Same as thermocouple_get_voltages but for rtds
 */
int rtds_get_voltages(float *voltages);


#endif /* __SENSORS_H */

