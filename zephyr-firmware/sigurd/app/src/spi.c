#include <zephyr/drivers/spi.h>

#define COMMS_REG 0x00
#define ID_REG 0x07

//CONFIG_SPI=y

//https://academy.nordicsemi.com/courses/nrf-connect-sdk-intermediate/lessons/lesson-5-serial-peripheral-interface-spi/topic/zephyr-spi-api/

#define SPIOP      SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA

//fix
struct spi_dt_spec spi = SPI_DT_SPEC_GET(DT_NODELABEL(ext_adc1), SPIOP, 0)

ret = spi_is_ready_dt(&spi);
if (ret != 0) {
	LOG_ERR("Error: SPI device is not ready, err: %d", err);
	return 0;
}

uint8_t tx_buffer = 0x88;

uint16_t data;
struct spi_buf tx_spi_buf		= {.buf = (void *)&tx_buffer, .len = 1};
struct spi_buf_set tx_spi_buf_set 	= {.buffers = &tx_spi_buf, .count = 1};

struct spi_buf rx_spi_bufs 		= {.buf = data, .len = 2};
struct spi_buf_set rx_spi_buf_set	= {.buffers = &rx_spi_bufs, .count = 1};

ret = spi_transceive_dt(&spi, &tx_spi_buf_set, &rx_spi_buf_set);

if (ret!=0) { 
    LOG_ERR("ERROR: SPI DEVICE sucks")
    return 0
}


	


