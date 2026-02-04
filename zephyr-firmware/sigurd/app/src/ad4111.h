#ifndef AD4111_H
#define AD4111_H

#include <stdint.h>

int ad4111_init(void);
uint8_t *ad4111_get_data(void);
int ad4111_is_running(void);

#endif
