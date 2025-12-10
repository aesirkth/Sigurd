#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>

#include <ad4111.h>
#include "sensors.h"

LOG_MODULE_REGISTER(sensors, LOG_LEVEL_INF);


//CAN thread
//sensor thread


double ad4111_current_to_pressure(int32_t val) {
    double pressure = val * 2.5 / ((1 << 24) * 0.1);

    return pressure;
}

double pressures[NUM_PRESSURE_TRANSDUCERS];
int32_t pressure_adc_vals[NUM_PRESSURE_TRANSDUCERS];

double get_pressure(size_t index) {
    return pressures[index];
}

int32_t get_pressure_adc(size_t index) {
    return pressure_adc_vals[index];
}

void sensor_thread_fn(void) {

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

        // for (int i = 0; i < 10; i++) {
        //     int32_t adc_value;
        //     ad4111_read_channel(ext_adc1, i, &adc_value);

        //     pressures[i] = ad4111_current_to_pressure(adc_value);

        //     k_sleep(K_MSEC(10));

        // }

        // for (int i = 8; i <= 11; i++) {
            int32_t adc_value;
            ad4111_read_channel(ext_adc1, 8, &adc_value);

            pressure_adc_vals[0] = adc_value;
            // pressure_adc_vals[i-6] = 0xAABBCCDD;
            // pressures[i-6] = ad4111_current_to_pressure(adc_value);

            k_sleep(K_MSEC(10));
        // }
        k_sleep(K_MSEC(1000));

    }
}

K_THREAD_DEFINE(sensors_thread, 4096, sensor_thread_fn, NULL, NULL, NULL, 5, 0, 0);

void init_sensors(void) {
    k_thread_start(sensors_thread);

}
