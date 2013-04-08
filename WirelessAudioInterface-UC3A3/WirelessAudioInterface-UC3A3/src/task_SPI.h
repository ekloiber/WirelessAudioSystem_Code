/*
 * task_SPI.h
 *
 * Created: 4/2/2013 11:48:30 PM
 *  Author: Eva
 */ 


#ifndef TASK_SPI_H_
#define TASK_SPI_H_

#define PIN_SCLK0			AVR32_SPI0_SCK_0_0_PIN
#define FUNC_SCLK0			AVR32_SPI0_SCK_0_0_FUNCTION
#define PIN_MOSI0			AVR32_SPI0_MOSI_0_0_PIN
#define FUNC_MOSI0			AVR32_SPI0_MOSI_0_0_FUNCTION
#define PIN_MISO0			AVR32_SPI0_MISO_0_0_PIN
#define FUNC_MISO0			AVR32_SPI0_MISO_0_0_FUNCTION

#define PIN_SCLK1			AVR32_SPI1_SCK_0_0_PIN
#define FUNC_SCLK1			AVR32_SPI1_SCK_0_0_FUNCTION
#define PIN_MOSI1			AVR32_SPI1_MOSI_0_0_PIN
#define FUNC_MOSI1			AVR32_SPI1_MOSI_0_0_FUNCTION
#define PIN_MISO1			AVR32_SPI1_MISO_0_0_PIN
#define FUNC_MISO1			AVR32_SPI1_MISO_0_0_FUNCTION

#define PIN_CC8530_nCS		AVR32_SPI0_NPCS_3_0_PIN
#define FUNC_CC8530_nCS		AVR32_SPI0_NPCS_3_0_FUNCTION
#define ID_CC8530_nCS		3
#define SPI_CC8530			(&AVR32_SPI0)

#define PIN_CODEC_nCS		AVR32_SPI0_NPCS_2_1_PIN
#define FUNC_CODEC_nCS		AVR32_SPI0_NPCS_2_1_FUNCTION
#define ID_CODEC_nCS		2
#define SPI_CODEC			(&AVR32_SPI0)

#define PIN_CLOCK_nCS		AVR32_SPI0_NPCS_1_0_PIN
#define FUNC_CLOCK_nCS		AVR32_SPI0_NPCS_1_0_FUNCTION
#define ID_CLOCK_nCS		1
#define SPI_CLOCK			(&AVR32_SPI0)

#define PIN_TOUCH_nCS		AVR32_SPI1_NPCS_1_1_PIN
#define FUNC_TOUCH_nCS		AVR32_SPI1_NPCS_1_1_FUNCTION
#define ID_TOUCH_nCS		1
#define SPI_TOUCH			(&AVR32_SPI1)

static struct spi_device spi_device_cc8530;
static struct spi_device spi_device_codec;
static struct spi_device spi_device_clock;
static struct spi_device spi_device_touch;

extern void task_spi_init(void);
extern void task_spi_start(void);

#endif /* TASK_SPI_H_ */