#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>

#include <ad4111.h>
#include "sensors.h"


LOG_MODULE_REGISTER(sensors, LOG_LEVEL_INF);

K_THREAD_STACK_DEFINE(sensor_thread_stack, SENSOR_THREAD_STACK_SIZE);


void init_sensor(sigurd_t *sigurd) {
    sensor_thread_id = k_thread_create(
        &sensor_thread_data,
        sensor_thread_stack,
        K_THREAD_STACK_SIZEOF(sensor_thread_stack),
        (k_thread_entry_t)sensor_thread,
        sigurd, NULL, NULL,
        SENSOR_THREAD_PRIORITY, 0, K_NO_WAIT
    );
    k_thread_name_set(sensor_thread_id, "sensor");
}

void sensor_thread(void* p1, void* p2, void* p3) {

    sigurd_t* sigurd = p1;

    const struct device* ext_adc1 = DEVICE_DT_GET(DT_ALIAS(xadc1));

    if (!device_is_ready(ext_adc1)) {
        LOG_ERR("ext_adc1 not ready");
        return;
    }

    sigurd->initialized = 1;

    LOG_INF("ext_adc1 ready");

    LOG_INF("Sensor thread is running!");

    while (1) {
        int32_t adc_values[10];

        for (int i = 0; i < 10; i++) {
            
            LOG_INF("ALIVE");

            int e = ad4111_read_channel(ext_adc1, i, &adc_values[i]);

            if (e) {
                LOG_ERR("ADC read failed on ch %d (err %d)", i, err);
            }
            else {
                LOG_INF("CH%d = %d", i, adc_values[i]);
                sigurd->adc_values[i] = value;
            }

            k_msleep(10);
        }

    }
}