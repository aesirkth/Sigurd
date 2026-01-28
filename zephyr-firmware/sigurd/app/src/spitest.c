#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/pinctrl.h>

LOG_MODULE_REGISTER(spitest, LOG_LEVEL_DBG);

#include "ad4111.h"

#define SPIOP SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA 

struct spi_dt_spec spi = SPI_DT_SPEC_GET(DT_NODELABEL(ext_adc1), SPIOP);

void runsetupspi() {
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

void AD4111_Reset() { 
    
    uint8_t tx_buffer_init_buffer[] = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };
    struct spi_buf tx_init_spi_buf		    = {.buf = (void *)tx_buffer_init_buffer, .len = 16};
    struct spi_buf_set tx_init_spi_buf_set 	= {.buffers = &tx_init_spi_buf, .count = 1};
    spi_write_dt(&spi, &tx_init_spi_buf_set);
}

int AD4111_ID() { 
    //send the 0x47, to read from the ID register

    int ret = spi_is_ready_dt(&spi);
    if (ret == 0) {
        LOG_ERR("Error: SPI device is not ready, err: %d", ret);
        return 0;
    }

    uint8_t tx_buffer[3] = {0b01000111, 0, 0}; // read from ID register


    struct spi_buf tx_spi_buf		    = {.buf = (void *)tx_buffer, .len = 3};
    struct spi_buf_set tx_spi_buf_set 	= {.buffers = &tx_spi_buf, .count = 1};

    uint8_t data[] = {0xFF, 0xFF, 0xFF};
    struct spi_buf rx_spi_buf 		    = {.buf = (void *)data, .len = 3};
    struct spi_buf_set rx_spi_buf_set	= {.buffers = &rx_spi_buf, .count = 1};

    ret = spi_transceive_dt(&spi, &tx_spi_buf_set, &rx_spi_buf_set);

    if (ret!=0) { 
        LOG_ERR("ERROR: SPI DEVICE sucks");
        return 0;
    }

    return data[2];
}



void ad4111_channel(ad4111_channel_t* channel) { 


    uint8_t comms = channel->ch_id & 0x3F;

    uint16_t chan_reg = 0;

    chan_reg |= (channel->ch_en & 0x1)      << 15;
    chan_reg |= (channel->setup_sel & 0x7)  << 12;
    chan_reg |= (channel->input & 0x3FF);


    uint8_t tx_buffer[3] = {comms, (chan_reg >> 8) & 0xFF, chan_reg & 0xFF}; // read from ID register


    struct spi_buf tx_spi_buf		    = {.buf = (void *)tx_buffer, .len = 3};
    struct spi_buf_set tx_spi_buf_set 	= {.buffers = &tx_spi_buf, .count = 1};


    //dummy data, not actually necessary, one could use spi_write_dt, but i hate zephyr
    uint8_t rx_dummy[] = {0xFF, 0xFF, 0xFF};
    struct spi_buf rx_spi_buf 		    = {.buf = (void *)rx_dummy, .len = 3};
    struct spi_buf_set rx_spi_buf_set	= {.buffers = &rx_spi_buf, .count = 1};

    spi_transceive_dt(&spi, &tx_spi_buf_set, &rx_spi_buf_set);

}

void ad4111_config_channel(ad4111_setup_t *setup)
{

    uint8_t comms = setup->setup_id & 0x3F;

    uint16_t cfg_reg = 0;

    cfg_reg |= ((setup->bi_unipolar & 0x1)  << 12);
    cfg_reg |= ((setup->refbuf_plus  & 0x1) << 11);
    cfg_reg |= ((setup->refbuf_minus & 0x1) << 10);
    cfg_reg |= ((setup->inbuf        & 0x3) << 8);
    cfg_reg |= ((setup->ref_sel      & 0x3) << 4);

    uint8_t tx_buffer[3] = {comms, (cfg_reg >> 8) & 0xFF, cfg_reg & 0xFF};

    struct spi_buf tx_spi_buf = {.buf = (void *)tx_buffer, .len = 3};

    struct spi_buf_set tx_spi_buf_set = {.buffers = &tx_spi_buf, .count = 1};

    
    uint8_t rx_dummy[3];
    struct spi_buf rx_spi_buf = {.buf = rx_dummy, .len = 3};

    struct spi_buf_set rx_spi_buf_set = {.buffers = &rx_spi_buf, .count = 1};

    spi_transceive_dt(&spi, &tx_spi_buf_set, &rx_spi_buf_set);

}



void ad4111_config_filter(ad4111_filter_t *filter)
{

    uint8_t comms = filter->filter_id & 0x3F;

    uint16_t filter_reg = 0;

    filter_reg |= ((filter->enhfilten & 0x1) << 11);
    filter_reg |= ((filter->enhfilt   & 0x7) << 8);
  
    filter_reg |= ((filter->order     & 0x3) << 5);
    filter_reg |= ((filter->odr       & 0x1F) << 0);

    uint8_t tx_buffer[3] = {comms, (filter_reg >> 8) & 0xFF, filter_reg & 0xFF};

    struct spi_buf tx_spi_buf = {.buf = (void *)tx_buffer, .len = 3};

    struct spi_buf_set tx_spi_buf_set = {.buffers = &tx_spi_buf, .count = 1};

    uint8_t rx_dummy[3];
    struct spi_buf rx_spi_buf = {.buf = (void *)rx_dummy, .len = 3};

    struct spi_buf_set rx_spi_buf_set = {.buffers = &rx_spi_buf, .count = 1};

    spi_transceive_dt(&spi, &tx_spi_buf_set, &rx_spi_buf_set);

}




//also make sure to tie the thermocouple amplifiers to GND via a certain solder bridge on Sigurd
//please check schematic for more info

void ad4111_adc_mode(void) { 
    //this must be written as well!
    //check Table 26 on datasheet!

}



