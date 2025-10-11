#include "stm32u5xx_hal.h"

#include "no_os_spi.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"

#include <stdlib.h>
#include <stdint.h>


typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
} hal_spi_extra;


void ad_build_spi_init(SPI_HandleTypeDef *hspi,
                       GPIO_TypeDef *cs_port, uint16_t cs_pin,
                       struct no_os_spi_init_param *out)
{

    static hal_spi_extra extra;
    extra.hspi   = hspi;
    extra.cs_port = cs_port;
    extra.cs_pin  = cs_pin;

    out->device_id     = 0;
    out->max_speed_hz  = 0;
    out->chip_select   = 0;
    out->mode          = NO_OS_SPI_MODE_0;
    out->bit_order     = NO_OS_SPI_BIT_ORDER_MSB_FIRST;
    out->platform_ops  = NULL;
    out->platform_delays.cs_delay_first = 0;
    out->platform_delays.cs_delay_last  = 0;
    out->extra         = &extra;
    out->parent        = NULL;
}

int32_t no_os_spi_init(struct no_os_spi_desc **desc,
                       const struct no_os_spi_init_param *param)
{
    if (!desc || !param || !param->extra) return -EINVAL;

    struct no_os_spi_desc *d = (struct no_os_spi_desc *)calloc(1, sizeof(*d));
    if (!d) return -ENOMEM;

    d->extra = param->extra;
    d->mode  = param->mode;
    d->bit_order = param->bit_order;
    d->max_speed_hz = param->max_speed_hz;
    d->chip_select  = param->chip_select;

    *desc = d;
    return 0;
}

int32_t no_os_spi_remove(struct no_os_spi_desc *desc)
{
    if (desc) free(desc);
    return 0;
}

int32_t no_os_spi_write_and_read(struct no_os_spi_desc *desc,
                                 uint8_t *data, uint16_t bytes)
{
    if (!desc || !desc->extra || !data || !bytes) return -EINVAL;

    hal_spi_extra *x = (hal_spi_extra *)desc->extra;

    HAL_GPIO_WritePin(x->cs_port, x->cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef st =
        HAL_SPI_TransmitReceive(x->hspi, data, data, bytes, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(x->cs_port, x->cs_pin, GPIO_PIN_SET);

    return (st == HAL_OK) ? 0 : -EIO;
}

int32_t no_os_spi_transfer(struct no_os_spi_desc *desc,
                           struct no_os_spi_msg *msgs, uint32_t len)
{

    if (!desc || !desc->extra || !msgs || !len) return -EINVAL;
    hal_spi_extra *x = (hal_spi_extra *)desc->extra;

    for (uint32_t i = 0; i < len; i++) {
        struct no_os_spi_msg *m = &msgs[i];
        HAL_GPIO_WritePin(x->cs_port, x->cs_pin, GPIO_PIN_RESET);

        if (m->cs_delay_first) no_os_udelay(m->cs_delay_first);

        HAL_StatusTypeDef st = HAL_OK;
        if (m->tx_buff && m->rx_buff) {
            st = HAL_SPI_TransmitReceive(x->hspi, m->tx_buff, m->rx_buff,
                                         m->bytes_number, HAL_MAX_DELAY);
        } else if (m->tx_buff) {
            st = HAL_SPI_Transmit(x->hspi, m->tx_buff,
                                  m->bytes_number, HAL_MAX_DELAY);
        } else if (m->rx_buff) {
            st = HAL_SPI_Receive(x->hspi, m->rx_buff,
                                 m->bytes_number, HAL_MAX_DELAY);
        } else {

            for (uint32_t n = 0; n < m->bytes_number; n++) {
                uint8_t z = 0, r;
                st = HAL_SPI_TransmitReceive(x->hspi, &z, &r, 1, HAL_MAX_DELAY);
                if (st != HAL_OK) break;
            }
        }

        if (m->cs_delay_last) no_os_udelay(m->cs_delay_last);
        HAL_GPIO_WritePin(x->cs_port, x->cs_pin, GPIO_PIN_SET);

        if (st != HAL_OK) return -EIO;

        if (m->cs_change_delay) no_os_udelay(m->cs_change_delay);
    }
    return 0;
}

int32_t no_os_spi_transfer_dma_sync(struct no_os_spi_desc *desc,
                                    struct no_os_spi_msg *msgs, uint32_t len)
{
    NO_OS_UNUSED_PARAM(desc); NO_OS_UNUSED_PARAM(msgs); NO_OS_UNUSED_PARAM(len);
    return -ENOSYS;
}
int32_t no_os_spi_transfer_dma_async(struct no_os_spi_desc *desc,
                                     struct no_os_spi_msg *msgs, uint32_t len,
                                     void (*callback)(void *), void *ctx)
{
    NO_OS_UNUSED_PARAM(desc); NO_OS_UNUSED_PARAM(msgs);
    NO_OS_UNUSED_PARAM(len); NO_OS_UNUSED_PARAM(callback); NO_OS_UNUSED_PARAM(ctx);
    return -ENOSYS;
}

void no_os_mdelay(uint32_t msecs)
{
    HAL_Delay(msecs);
}

void no_os_udelay(uint32_t usecs)
{
    uint32_t ticks = usecs / 1000U;
    if (ticks) HAL_Delay(ticks);
}

struct no_os_time no_os_get_time(void)
{
    struct no_os_time t;
    uint32_t ms = HAL_GetTick();
    t.s  = ms / 1000U;
    t.us = (ms % 1000U) * 1000U;
    return t;
}

