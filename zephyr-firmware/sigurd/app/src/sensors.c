#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ad4111.h"
#include "sensors.h"

LOG_MODULE_REGISTER(sensors, LOG_LEVEL_INF);

struct running_average {
	int n;
	float sum;
};

void add_new_data(struct running_average *average, float x) {
    average->sum += x;
    average->n++;
}

K_MUTEX_DEFINE(sensor_data_mutex);
static struct running_average thermocouples[4] = {0};
static struct running_average pressure_gauges[4] = {0};
static struct running_average rtds[2] = {0};

static int sensors_working = 0;

int sensors_get_status(void) {
    return sensors_working; // TODO: technically a race condition
}


void sensor_thread_fn(void) {
	while(true) {
        sensors_working = 1;
		// extracting the channel number for this reading
			
		int32_t data_reg;
		int channel = ad4111_get_data_blocking(&data_reg);
		if(channel < 0) {
            sensors_working = 0;
            continue;
        }

        if (k_mutex_lock(&sensor_data_mutex, K_MSEC(500))) {
            sensors_working = 0;
            continue;
        }
		switch(channel) {
			case 0: // thermocouple0-3
				add_new_data(&thermocouples[0], data_to_volts_unipolar(data_reg));
                break;
			case 1:
				add_new_data(&thermocouples[1], data_to_volts_unipolar(data_reg));
                break;
			case 2:
				add_new_data(&thermocouples[2], data_to_volts_unipolar(data_reg));
                break;
			case 3:
				add_new_data(&thermocouples[3], data_to_volts_unipolar(data_reg));
				break;
			case 4: // pressure_gauge0-3
				add_new_data(&pressure_gauges[0], data_to_amps_bipolar(data_reg));
				break;
			case 5:
				add_new_data(&pressure_gauges[1], data_to_amps_bipolar(data_reg));
				break;
			case 6:
				add_new_data(&pressure_gauges[2], data_to_amps_bipolar(data_reg));
				break;
			case 7:
				add_new_data(&pressure_gauges[3], data_to_amps_bipolar(data_reg));
				break;
			case 8: // RTD0-1
				add_new_data(&rtds[0], data_to_volts_bipolar(data_reg));
				break;
			case 9:
				add_new_data(&rtds[1], data_to_volts_bipolar(data_reg));
				break;
			default:
                sensors_working = 0;
				continue;
				break;
        }
        k_mutex_unlock(&sensor_data_mutex);
	}
}

K_THREAD_DEFINE(sensors_thread, 4096, sensor_thread_fn, NULL, NULL, NULL, 5, 0, 0);

int init_sensors(void) {
	int ret = ad4111_init();
	if (ret < 0) {
		return ret;
	}


    k_thread_start(sensors_thread);

    return 0;
}

int get_averages(float *averages, struct running_average *running_averages, int n) {
    if (k_mutex_lock(&sensor_data_mutex, K_MSEC(3000))) {
        // Failed to acquire mutex
        return -BIT(5);
    }
    for (int i = 0; i < n; i++) {
        averages[i] = running_averages[i].sum/running_averages[i].n;
        running_averages[i].sum = 0;
        running_averages[i].n = 0;
    }
    k_mutex_unlock(&sensor_data_mutex);
    return 0;
}

int thermocouple_get_voltages(float *voltages) {
    return get_averages(voltages, thermocouples, 4);
}

int pressure_gauges_get_currents(float *currents) {
    return get_averages(currents, pressure_gauges, 4);
}

int rtds_get_voltages(float *voltages) {
    return get_averages(voltages, rtds, 2);
}
