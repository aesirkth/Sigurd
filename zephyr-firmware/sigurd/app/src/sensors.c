#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>

#include <ad4111.h>
#include "sensors.h"

LOG_MODULE_REGISTER(sensors, LOG_LEVEL_INF);


//K_THREAD_DEFINE(sensors_tid, 4096, sensor_thread, NULL, NULL, NULL, 5, 0, 0);
//CAN thread
//sensor thread


void sensor_thread(void) {

    const struct device* ext_adc1 = DEVICE_DT_GET(DT_ALIAS(xadc1));

    if (!device_is_ready(ext_adc1)) {
        LOG_ERR("ext_adc1 not ready");
        return;
    }

    LOG_INF("ext_adc1 ready");

    //struct

    // temperature sensor 1
    // temp sensor ...
    // pressure sensor ... 

    while (1) {
        int32_t adc_values[10];

        for (int i = 0; i < 10; i++) {
            
            ad4111_read_channel(ext_adc1, i, &adc_values[i]);

            k_msleep(10);
        }

    }
}