#include "ad4111.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ADC_AD4111, LOG_LEVEL_DBG);

#define AD4111_CMD_READ       0x40
#define AD4111_CMD_WRITE      0x0

typedef enum {
	AD4111_STATUS_REG = 0x00,
	AD4111_MODE_REG = 0x01,
	AD4111_IFMODE_REG = 0x02,
	AD4111_REGCHECK = 0x03,
	AD4111_DATA_REG = 0x04,
	AD4111_GPIOCON_REG = 0x06,
	AD4111_ID_REG = 0x07,
	AD4111_CHANNEL_0_REG = 0x10,
	AD4111_CHANNEL_1_REG = 0x11,
	AD4111_CHANNEL_2_REG = 0x12,
	AD4111_CHANNEL_3_REG = 0x13,
	AD4111_CHANNEL_4_REG = 0x14,
	AD4111_CHANNEL_5_REG = 0x15,
	AD4111_CHANNEL_6_REG = 0x16,
	AD4111_CHANNEL_7_REG = 0x17,
	AD4111_CHANNEL_8_REG = 0x18,
	AD4111_CHANNEL_9_REG = 0x19,
	AD4111_CHANNEL_10_REG = 0x1A,
	AD4111_CHANNEL_11_REG = 0x1B,
	AD4111_CHANNEL_12_REG = 0x1C,
	AD4111_CHANNEL_13_REG = 0x1D,
	AD4111_CHANNEL_14_REG = 0x1E,
	AD4111_CHANNEL_15_REG = 0x1F,
	AD4111_SETUPCON0_REG = 0x20,
	AD4111_SETUPCON1_REG = 0x21,
	AD4111_SETUPCON2_REG = 0x22,
	AD4111_SETUPCON3_REG = 0x23,
	AD4111_SETUPCON4_REG = 0x24,
	AD4111_SETUPCON5_REG = 0x25,
	AD4111_SETUPCON6_REG = 0x26,
	AD4111_SETUPCON7_REG = 0x27,
	AD4111_FILTCON0_REG = 0x28,
	AD4111_FILTCON1_REG = 0x29,
	AD4111_FILTCON2_REG = 0x2A,
	AD4111_FILTCON3_REG = 0x2B,
	AD4111_FILTCON4_REG = 0x2C,
	AD4111_FILTCON5_REG = 0x2D,
	AD4111_FILTCON6_REG = 0x2E,
	AD4111_FILTCON7_REG = 0x2F,
	AD4111_OFFSET0_REG = 0x30,
	AD4111_OFFSET1_REG = 0x31,
	AD4111_OFFSET2_REG = 0x32,
	AD4111_OFFSET3_REG = 0x33,
	AD4111_OFFSET4_REG = 0x34,
	AD4111_OFFSET5_REG = 0x35,
	AD4111_OFFSET6_REG = 0x36,
	AD4111_OFFSET7_REG = 0x37,
	AD4111_GAIN0_REG = 0x38,
	AD4111_GAIN1_REG = 0x39,
	AD4111_GAIN2_REG = 0x3A,
	AD4111_GAIN3_REG = 0x3B,
	AD4111_GAIN4_REG = 0x3C,
	AD4111_GAIN5_REG = 0x3D,
	AD4111_GAIN6_REG = 0x3E,
	AD4111_GAIN7_REG = 0x3F,
} ad4111_reg_t;

typedef enum  {
    VIN0_VIN1 = 0x1,
    VIN0_VINCOM = 0x10,
    VIN1_VIN0 = 0x20,
    VIN1_VINCOM = 0x30,
    VIN2_VIN3 = 0x43,
    VIN2_VINCOM = 0x50,
    VIN3_VIN2 = 0x62,
    VIN3_VINCOM = 0x70,
    VIN4_VIN5 = 0x85,
    VIN4_VINCOM = 0x90,
    VIN5_VIN4 = 0xA4,
    VIN5_VINCOM = 0xB0,
    VIN6_VIN7 = 0xC7,
    VIN6_VINCOM = 0xD0,
    VIN7_VIN6 = 0xE6,
    VIN7_VINCOM = 0xF0,
    IIN3P_IIN3M = 0x18B,
    IIN2P_IIN2M = 0x1AA,
    IIN1P_IIN1M = 0x1C9,
    IIN0P_IIN0M = 0x1E8,
    TEMPERATURE_SENSOR = 0x232,
    REFERENCE = 0x2B6
} ad4111_input_t;

typedef enum {
    EXTERNAL_REF = 0x0,
    INTERNAL_REF = 0x2,
    AVDD_AVSS    = 0x3
} ad4111_ref_t;

typedef enum { 
    UNIPOLAR = 0x0,
    BIPOLAR  = 0x1
} ad4111_biunipolar_t;

typedef enum {
    AD4111_INBUF_DISABLED = 0x0,
    AD4111_INBUF_RSVD1    = 0x1,
    AD4111_INBUF_RSVD2    = 0x2,
    AD4111_INBUF_ENABLED  = 0x3
} ad4111_inbuf_t;


//doublecheck if these hex values are correct and align with the datasheet!
typedef enum {
    AD4111_ENHFILT_DISABLED = 0x0,
    AD4111_ENHFILT_ENABLED  = 0x1
} ad4111_enhfilten_t;

typedef enum {
    AD4111_ENHFILT_27SPS_47DB = 0x2,
    AD4111_ENHFILT_25SPS_62DB = 0x3,
    AD4111_ENHFILT_20SPS_86DB = 0x5,
    AD4111_ENHFILT_16SPS_92DB = 0x6
} ad4111_enhfilt_t;

typedef enum {
    AD4111_FILTER_SINC5_SINC1 = 0x0,
    AD4111_FILTER_SINC3       = 0x3
} ad4111_filter_order_t;

typedef enum {
    AD4111_ODR_31250_SPS   = 0x00,
    AD4111_ODR_15625_SPS   = 0x06,
    AD4111_ODR_10417_SPS   = 0x07,
    AD4111_ODR_5208_SPS    = 0x08,
    AD4111_ODR_2597_SPS    = 0x09,
    AD4111_ODR_1007_SPS    = 0x0A,
    AD4111_ODR_503_SPS     = 0x0B,
    AD4111_ODR_381_SPS     = 0x0C,
    AD4111_ODR_200_SPS     = 0x0D,
    AD4111_ODR_100_SPS     = 0x0E,
    AD4111_ODR_59_SPS      = 0x0F,
    AD4111_ODR_49_SPS      = 0x10,
    AD4111_ODR_20_SPS      = 0x11,
    AD4111_ODR_16_SPS      = 0x12,
    AD4111_ODR_10_SPS      = 0x13,
    AD4111_ODR_5_SPS       = 0x14,
    AD4111_ODR_2_5_SPS     = 0x15,
    AD4111_ODR_1_25_SPS    = 0x16
} ad4111_odr_t;

typedef struct {
	ad4111_reg_t reg;
	uint8_t ch_en;
	uint8_t setup_sel;
	ad4111_input_t input;
} ad4111_channel_t;

typedef struct { 
	ad4111_reg_t reg;
	ad4111_biunipolar_t bi_unipolar;
	uint8_t refbuf_plus;
	uint8_t refbuf_minus;
	ad4111_inbuf_t inbuf;
	ad4111_ref_t ref_sel;
} ad4111_setupcon_t;

#define AD4111_ADCMODE_REF_EN 15
#define AD4111_IFMODE_DATA_STAT 6

typedef struct {
	ad4111_reg_t reg;
	ad4111_enhfilten_t enhfilten;
	ad4111_enhfilt_t enhfilt;
	ad4111_filter_order_t order;
	ad4111_odr_t odr;
} ad4111_filter_t;

#define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA 

struct spi_dt_spec spi = SPI_DT_SPEC_GET(DT_NODELABEL(ext_adc1), SPIOP);


static int ad4111_read_reg(ad4111_reg_t reg_addr, uint8_t *buffer,
			   size_t reg_size) {
	int ret;

	uint8_t buffer_tx[6] = {0};
	uint8_t buffer_rx[ARRAY_SIZE(buffer_tx)] = {0xFF};
	const struct spi_buf tx_buf[] = {{
		.buf = buffer_tx,
		.len = ARRAY_SIZE(buffer_tx),
	}};
	const struct spi_buf rx_buf[] = {{
		.buf = buffer_rx,
		.len = ARRAY_SIZE(buffer_rx),
	}};
	const struct spi_buf_set tx = {
		.buffers = tx_buf,
		.count = ARRAY_SIZE(tx_buf),
	};
	const struct spi_buf_set rx = {
		.buffers = rx_buf,
		.count = ARRAY_SIZE(rx_buf),
	};
	buffer_tx[0] = AD4111_CMD_READ | reg_addr;

	ret = spi_transceive_dt(&spi, &tx, &rx);
	if (ret != 0) {
		LOG_ERR("error reading register 0x%X (%d)", reg_addr, ret);
		return ret;
	}

	// TODO: Check whether this can be avoided by use of sets.
	/* Copy received data in output buffer */
	for (uint8_t index = 0; index < reg_size; index++) {
		buffer[index] = buffer_rx[index + 1];
	}

	return ret;
}

static int ad4111_write_reg(ad4111_reg_t reg_addr, uint8_t *buffer, size_t reg_size) {
	int ret;
	uint8_t buffer_tx[5] = {0}; /* One byte command, max 4 bytes data */

	const struct spi_buf tx_buf[] = {{
		.buf = buffer_tx,
		.len = ARRAY_SIZE(buffer_tx),
	}};
	const struct spi_buf_set tx = {
		.buffers = tx_buf,
		.count = ARRAY_SIZE(tx_buf),
	};

	buffer_tx[0] = AD4111_CMD_WRITE | reg_addr;

	if (reg_size > 4) {
		LOG_ERR("Invalid size, max data write size is 4");
		return -ENOMEM;
	}

	// TODO: Check whether this can be avoided by use of sets.
	/* Fill the data */
	for (uint8_t index = 0; index < reg_size; index++) {
		buffer_tx[1 + index] = buffer[index];
	}

	ret = spi_write_dt(&spi, &tx);
	if (ret != 0) {
		LOG_ERR("error writing register 0x%X (%d)", reg_addr, ret);
		return ret;
	}

	return ret;
}

int ad4111_reset(void) {
    uint8_t tx_buffer_init_buffer[] = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    struct spi_buf tx_init_spi_buf		= {.buf = (void *)tx_buffer_init_buffer, .len = 16};
    struct spi_buf_set tx_init_spi_buf_set 	= {.buffers = &tx_init_spi_buf, .count = 1};

	int ret = spi_write_dt(&spi, &tx_init_spi_buf_set);
	if (ret != 0) {
		LOG_ERR("error resetting (%d)", ret);
		return ret;
	}
	k_msleep(100); // AD4111 seems to have some bootup
    return ret;
}

int ad4111_check_id(void) {
	uint8_t id[2];

	int ret = ad4111_read_reg(AD4111_ID_REG, id, 2);
	if (ret != 0) {
		LOG_ERR("error reading id (%d)", ret);
		return ret;
	}

	/* Check that this is the expected ID : 0x30DX, where x is don’t care */
	if ((((id[0] << 8) | id[1]) & 0xFFF0) != 0x30D0) {
		LOG_ERR("Read wrong ID register 0x%X 0x%X", id[0], id[1]);
		return -EIO;
	}

	return 0;
}

int ad4111_set_channel(ad4111_channel_t channel) { 

    uint16_t channel_reg = 0;
    channel_reg |= (channel.ch_en & 0x1)      << 15;
    channel_reg |= (channel.setup_sel & 0x7)  << 12;
    channel_reg |= (channel.input & 0x3FF);

    uint8_t tx_buffer[2] = {(channel_reg >> 8) & 0xFF, channel_reg & 0xFF};

	return ad4111_write_reg(channel.reg, tx_buffer, 2);
}

int ad4111_set_setupcon(ad4111_setupcon_t setupcon) {
    uint16_t setupcon_reg = 0;

    setupcon_reg |= ((setupcon.bi_unipolar & 0x1)  << 12);
    setupcon_reg |= ((setupcon.refbuf_plus  & 0x1) << 11);
    setupcon_reg |= ((setupcon.refbuf_minus & 0x1) << 10);
    setupcon_reg |= ((setupcon.inbuf        & 0x3) << 8);
    setupcon_reg |= ((setupcon.ref_sel      & 0x3) << 4);

    uint8_t tx_buffer[2] = {(setupcon_reg >> 8) & 0xFF, setupcon_reg & 0xFF};

	return ad4111_write_reg(setupcon.reg, tx_buffer, 2);
}

int ad4111_set_filter(ad4111_filter_t filter) {

    uint16_t filter_reg = 0;

    filter_reg |= ((filter.enhfilten & 0x1) << 11);
    filter_reg |= ((filter.enhfilt   & 0x7) << 8);
  
    filter_reg |= ((filter.order     & 0x3) << 5);
    filter_reg |= ((filter.odr       & 0x1F) << 0);

    uint8_t tx_buffer[2] = {(filter_reg >> 8) & 0xFF, filter_reg & 0xFF};

	return ad4111_write_reg(filter.reg, tx_buffer, 2);
}

/*
 * totally hard coded adc mode.
 * REF_EN = 1, everything else = 0
 */
int ad4111_set_adcmode(void) { 
	uint16_t adcmode_reg = BIT(AD4111_ADCMODE_REF_EN);
	uint8_t tx_buffer[2] = {(adcmode_reg >> 8) & 0xFF, adcmode_reg & 0xFF};
	return ad4111_write_reg(AD4111_MODE_REG, tx_buffer, 2);
}
/*
 * totally hard coded if mode.
 * DATA_STAT = 1, everything else = 0
 */
int ad4111_set_ifmode(void) { 
	uint16_t ifmode_reg = BIT(AD4111_IFMODE_DATA_STAT);
	uint8_t tx_buffer[2] = {(ifmode_reg >> 8) & 0xFF, ifmode_reg & 0xFF};

	return ad4111_write_reg(AD4111_IFMODE_REG, tx_buffer, 2);
}

// volatile int spi_adc_int_count = 0;
K_SEM_DEFINE(adc_data_ready, 0, 1);
void miso_interrupt_handler(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins) {
	// spi_adc_int_count++;
	k_sem_give(&adc_data_ready);
}

volatile int ad4111_working = 0;
uint8_t data[4];

static const struct gpio_dt_spec miso_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(spiadcready), gpios);
static struct gpio_callback miso_cb_data;

int initialize_interrupt() {
	int ret;

	if (!gpio_is_ready_dt(&miso_spec)) {
		LOG_ERR("Error: button device %s is not ready\n",
		       miso_spec.port->name);
		return -1;
	}

	ret = gpio_pin_configure_dt(&miso_spec, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, miso_spec.port->name, miso_spec.pin);
		return ret;
	}

	// ret = gpio_pin_configure_dt(&miso_spec, GPIO_INPUT);
	ret = gpio_pin_interrupt_configure_dt(&miso_spec, GPIO_INT_EDGE_TO_INACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, miso_spec.port->name, miso_spec.pin);
		return ret;
	}
	gpio_init_callback(&miso_cb_data, miso_interrupt_handler, BIT(miso_spec.pin));
	gpio_add_callback(miso_spec.port, &miso_cb_data);
	return 0;
}

int initialize_gpio_miso() {
	int ret;
	if (!gpio_is_ready_dt(&miso_spec)) {
		LOG_ERR("Error: button device %s is not ready\n",
		       miso_spec.port->name);
		return -1;
	}

	ret = gpio_pin_configure_dt(&miso_spec, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, miso_spec.port->name, miso_spec.pin);
		return ret;
	}
	return 0;
}


void ad4111_thread_fn(void) {
    int ret;

	ad4111_working = 0;
    while (true) {
		// int not_ready = gpio_pin_get_dt(&miso_spec);
		// if(!not_ready) {
		// if(spi_adc_int_count > 0) {
		if (k_sem_take(&adc_data_ready, K_MSEC(50)) != 0) {
			ad4111_working = 0;	
		} else {
			ad4111_working = 1;
			// spi_adc_int_count = 0;	

			// Disable interrupt while reading data.

			ret = gpio_pin_interrupt_configure_dt(&miso_spec, GPIO_INT_DISABLE);
			if (ret != 0) break;

			ret = ad4111_read_reg(AD4111_DATA_REG, data, 4);
			if (ret != 0) break;

			ret = gpio_pin_interrupt_configure_dt(&miso_spec, GPIO_INT_EDGE_TO_INACTIVE);
			if (ret != 0) break;
			
		}
		// k_msleep(1);
		// k_usleep(1);
    }
	ad4111_working = 0;
}

uint8_t *ad4111_get_data(void) {
	return data;
}

int ad4111_is_running(void) {
	return ad4111_working;
}



K_THREAD_DEFINE(ad4111_thread,
                1024,
                ad4111_thread_fn,
                NULL, NULL, NULL,
                5, // low-ish priority because it never yields
                0,
                -1); // do not start

int ad4111_init(void) {
	int ret;

    if (!spi_is_ready_dt(&spi)) {
        LOG_ERR("Error: SPI device is not ready");
        return -1;
    }

	ret = ad4111_reset();
    if (ret != 0) return ret;

	ret = ad4111_check_id();
	if(ret != 0) return ret;

	uint8_t status;
	ad4111_read_reg(AD4111_STATUS_REG, &status, 1);
	LOG_INF("Found AD4111 with status %d", status);

	// Configuration for Sigurd
	// TODO: One day, someone could look into making this configurable
	//       via devicetree. Today, we are not the ones to do it.

	ret = ad4111_set_adcmode();
	if(ret != 0) return ret;

	ret = ad4111_set_ifmode();
	if(ret != 0) return ret;

	ad4111_setupcon_t thermocouple_setupcon;
	thermocouple_setupcon.reg = AD4111_SETUPCON0_REG;
	thermocouple_setupcon.bi_unipolar = UNIPOLAR;
	thermocouple_setupcon.refbuf_plus = 1;
	thermocouple_setupcon.refbuf_minus = 1;
	thermocouple_setupcon.inbuf = AD4111_INBUF_DISABLED;
	thermocouple_setupcon.ref_sel = INTERNAL_REF;
	ret = ad4111_set_setupcon(thermocouple_setupcon);
	if(ret != 0) return ret;

	ad4111_filter_t thermocouple_filter;
	thermocouple_filter.reg = AD4111_FILTCON0_REG;
	thermocouple_filter.enhfilten = AD4111_ENHFILT_DISABLED;
	thermocouple_filter.enhfilt = AD4111_ENHFILT_27SPS_47DB;
	thermocouple_filter.order = AD4111_FILTER_SINC5_SINC1;
	thermocouple_filter.odr = AD4111_ODR_200_SPS;
	ret = ad4111_set_filter(thermocouple_filter);
	if(ret != 0) return ret;

	ad4111_channel_t thermocouple0;
	thermocouple0.reg = AD4111_CHANNEL_0_REG;
	thermocouple0.ch_en = 1;
	thermocouple0.setup_sel = 0;
	thermocouple0.input = VIN7_VINCOM;
	ret = ad4111_set_channel(thermocouple0);
	if(ret != 0) return ret;


	/* 
	use this for current measurements
	
	ad4111_setup_t setup1;
	setup1.setup_id = 	...
	setup0.bi_unipolar	= UNIPOLAR;
	setup0.refbuf_plus 	= 0;
	setup0.refbuf_minus = 0;
	setup0.ref_sel 		= INTERNAL_REF;
	*/

    k_thread_start(ad4111_thread);

	// ret = initialize_gpio_miso();
	ret = initialize_interrupt();
	if(ret != 0) return ret;

	return 0;
}
