#ifndef __SENSOR_H
#define __SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_PRESSURE_TRANSDUCERS 4

double get_pressure(size_t index);
int32_t get_pressure_adc(size_t index);

void init_sensors(void);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_H */

