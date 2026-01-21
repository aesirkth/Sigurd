#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(spitest, LOG_LEVEL_DBG);

#define COMMS_REG 0x00
#define ID_REG 0x07

//CONFIG_SPI=y

//https://academy.nordicsemi.com/courses/nrf-connect-sdk-intermediate/lessons/lesson-5-serial-peripheral-interface-spi/topic/zephyr-spi-api/

#define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA
// #define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL
// #define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPHA
// #define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB

int runspitest() {
    struct spi_dt_spec spi = SPI_DT_SPEC_GET(DT_NODELABEL(ext_adc1), SPIOP);

    int ret = spi_is_ready_dt(&spi);
    if (ret == 0) {
        LOG_ERR("Error: SPI device is not ready, err: %d", ret);
        return 0;
    }

    uint8_t tx_buffer[] = {0b01000111, 0, 0}; // this should be 0x44 maybe?
    // loop
    // transcieve, timing between sending and recieving.
    // soft reset

    uint8_t data[] = {25, 35, 45};
    struct spi_buf tx_spi_buf		= {.buf = (void *)tx_buffer, .len = 3};
    struct spi_buf_set tx_spi_buf_set 	= {.buffers = &tx_spi_buf, .count = 1};

    struct spi_buf rx_spi_bufs 		= {.buf = (void *)data, .len = 3};
    struct spi_buf_set rx_spi_buf_set	= {.buffers = &rx_spi_bufs, .count = 1};

    ret = spi_transceive_dt(&spi, &tx_spi_buf_set, &rx_spi_buf_set);

    if (ret!=0) { 
        LOG_ERR("ERROR: SPI DEVICE sucks");
        return 0;
    }

    // ret = spi_read_dt(&spi, &rx_spi_buf_set);
    // if (ret!=0) { 
    //     LOG_ERR("ERROR: SPI DEVICE sucks");
    //     return 0;
    // }

    // LOG_INF("%d", data);

    return data[0];

}
	


