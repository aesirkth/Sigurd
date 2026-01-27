#ifndef AD4111_H
#define AD4111_H

// i copied these registers from the datasheet :D
typedef enum { 
    CH0  = 0x10,
    CH1  = 0x11,
    CH2  = 0x12,
    CH3  = 0x13,
    CH4  = 0x14,
    CH5  = 0x15,
    CH6  = 0x16,
    CH7  = 0x17,
    CH8  = 0x18,
    CH9  = 0x19,
    CH10 = 0x1A,
    CH11 = 0x1B,
    CH12 = 0x1C,
    CH13 = 0x1D,
    CH14 = 0x1E,
    CH15 = 0x1F
} ad4111_channel_register_t;

typedef enum {
    SETUP_CFG0 = 0x20,
    SETUP_CFG1 = 0x21,
    SETUP_CFG2 = 0x22,
    SETUP_CFG3 = 0x23,
    SETUP_CFG4 = 0x24,
    SETUP_CFG5 = 0x25,
    SETUP_CFG6 = 0x26,
    SETUP_CFG7 = 0x27
} ad4111_setup_register_t;

typedef enum {
    FILTER_CFG0 = 0x28,
    FILTER_CFG1 = 0x29,
    FILTER_CFG2 = 0x2A,
    FILTER_CFG3 = 0x2B,
    FILTER_CFG4 = 0x2C,
    FILTER_CFG5 = 0x2D,
    FILTER_CFG6 = 0x2E,
    FILTER_CFG7 = 0x2F
} ad4111_filter_register_t;

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
	ad4111_channel_register_t ch_id;
	uint8_t ch_en;
	uint8_t setup_sel;
	ad4111_input_t input;
} ad4111_channel_t;


typedef struct { 
	ad4111_setup_register_t setup_id;
	ad4111_biunipolar_t bi_unipolar;
	uint8_t refbuf_plus;
	uint8_t refbuf_minus;
	ad4111_inbuf_t inbuf;
	ad4111_ref_t ref_sel;
} ad4111_setup_t;


typedef struct {
	ad4111_filter_register_t filter_id;
	ad4111_enhfilten_t enhfilten;
	ad4111_enhfilt_t enhfilt;
	ad4111_filter_order_t order;
	ad4111_odr_t odr;
} ad4111_filter_t;




#endif
