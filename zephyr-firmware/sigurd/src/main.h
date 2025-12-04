#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int32_t adc_values[10];
	float thermocouples[4];
	float RTDs[2];
	float pressures[4];
	int initialized;
} sigurd_t;


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
