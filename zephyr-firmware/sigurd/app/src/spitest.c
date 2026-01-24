#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/pinctrl.h>

LOG_MODULE_REGISTER(spitest, LOG_LEVEL_DBG);

#define COMMS_REG 0x00
#define ID_REG 0x07

//CONFIG_SPI=y

//https://academy.nordicsemi.com/courses/nrf-connect-sdk-intermediate/lessons/lesson-5-serial-peripheral-interface-spi/topic/zephyr-spi-api/

#define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA 
// #define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_LOCK_ON
// #define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL
// #define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPHA
// #define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB
struct spi_dt_spec spi = SPI_DT_SPEC_GET(DT_NODELABEL(ext_adc1), SPIOP);

void runsetupspi() {
    // struct spi_dt_spec spi = SPI_DT_SPEC_GET(DT_NODELABEL(ext_adc1), SPIOP);
    // pinctrl_apply_state(pcfg, PINCTRL_STATE_DEFAULT);
    uint8_t tx_buffer_init_buffer[] = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };
    struct spi_buf tx_init_spi_buf		= {.buf = (void *)tx_buffer_init_buffer, .len = 16};
    struct spi_buf_set tx_init_spi_buf_set 	= {.buffers = &tx_init_spi_buf, .count = 1};
    spi_write_dt(&spi, &tx_init_spi_buf_set);
}

/**
 * @param rx_buffer Needs to be length 4.
 */
int runspitest(uint8_t *rx_buffer) {

    int ret = spi_is_ready_dt(&spi);
    if (ret == 0) {
        LOG_ERR("Error: SPI device is not ready, err: %d", ret);
        return 0;
    }

    // uint8_t tx_buffer[3] = {0b01000111, 0, 0}; // read from ID register
    uint8_t tx_buffer[] = {0x44, 0, 0, 0}; // read from data register
    // loop
    // transcieve, timing between sending and recieving.
    // soft reset

    struct spi_buf tx_spi_buf		= {.buf = (void *)tx_buffer, .len = 4};
    struct spi_buf_set tx_spi_buf_set 	= {.buffers = &tx_spi_buf, .count = 1};

    // uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
    struct spi_buf rx_spi_buf 		= {.buf = (void *)rx_buffer, .len = 4};
    struct spi_buf_set rx_spi_buf_set	= {.buffers = &rx_spi_buf, .count = 1};

    ret = spi_transceive_dt(&spi, &tx_spi_buf_set, &rx_spi_buf_set);

    if (ret!=0) { 
        LOG_ERR("ERROR: SPI DEVICE sucks");
        return 1;
    }
    return 0;

    // ret = spi_read_dt(&spi, &rx_spi_buf_set);
    // if (ret!=0) { 
    //     LOG_ERR("ERROR: SPI DEVICE sucks");
    //     return 0;
    // }

    // LOG_INF("%d", data);

    // return data[2];

}
	


