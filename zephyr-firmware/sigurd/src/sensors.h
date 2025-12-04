#ifndef __SENSOR_H
#define __SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_THREAD_STACK_SIZE 4096
#define SENSOR_THREAD_PRIORITY 7

struct k_thread sensor_thread_data;
k_tid_t sensor_thread_id;

void sensor_thread(void *p1, void *p2, void *p3);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_H */

