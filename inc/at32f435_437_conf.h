#ifndef __AT32F435_437_CONF_H
#define __AT32F435_437_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined  HEXT_VALUE
#define HEXT_VALUE                       ((uint32_t)8000000) /*!< value of the high speed external crystal in hz */
#endif

#define HEXT_STARTUP_TIMEOUT             ((uint16_t)0x3000)  /*!< time out for hext start up */
#define HICK_VALUE                       ((uint32_t)8000000) /*!< value of the high speed internal clock in hz */
#define LEXT_VALUE                       ((uint32_t)32768)   /*!< value of the low speed external clock in hz */

/* module define -------------------------------------------------------------*/
#define CRM_MODULE_ENABLED
#define TMR_MODULE_ENABLED
#define ERTC_MODULE_ENABLED
#define GPIO_MODULE_ENABLED
#define I2C_MODULE_ENABLED
#define USART_MODULE_ENABLED
#define PWC_MODULE_ENABLED
#define CAN_MODULE_ENABLED
#define ADC_MODULE_ENABLED
#define DAC_MODULE_ENABLED
#define SPI_MODULE_ENABLED
#define EDMA_MODULE_ENABLED
#define DMA_MODULE_ENABLED
#define DEBUG_MODULE_ENABLED
#define FLASH_MODULE_ENABLED
#define CRC_MODULE_ENABLED
#define WWDT_MODULE_ENABLED
#define WDT_MODULE_ENABLED
#define EXINT_MODULE_ENABLED
#define SDIO_MODULE_ENABLED
#define XMC_MODULE_ENABLED
#define USB_MODULE_ENABLED
#define ACC_MODULE_ENABLED
#define MISC_MODULE_ENABLED
#define QSPI_MODULE_ENABLED
#define DVP_MODULE_ENABLED
#define SCFG_MODULE_ENABLED
#define EMAC_MODULE_ENABLED

/* includes ------------------------------------------------------------------*/
#ifdef CRM_MODULE_ENABLED
#include "at32f435_437_crm.h"
#endif
#ifdef TMR_MODULE_ENABLED
#include "at32f435_437_tmr.h"
#endif
#ifdef ERTC_MODULE_ENABLED
#include "at32f435_437_ertc.h"
#endif
#ifdef GPIO_MODULE_ENABLED
#include "at32f435_437_gpio.h"
#endif
#ifdef I2C_MODULE_ENABLED
#include "at32f435_437_i2c.h"
#endif
#ifdef USART_MODULE_ENABLED
#include "at32f435_437_usart.h"
#endif
#ifdef PWC_MODULE_ENABLED
#include "at32f435_437_pwc.h"
#endif
#ifdef CAN_MODULE_ENABLED
#include "at32f435_437_can.h"
#endif
#ifdef ADC_MODULE_ENABLED
#include "at32f435_437_adc.h"
#endif
#ifdef DAC_MODULE_ENABLED
#include "at32f435_437_dac.h"
#endif
#ifdef SPI_MODULE_ENABLED
#include "at32f435_437_spi.h"
#endif
#ifdef DMA_MODULE_ENABLED
#include "at32f435_437_dma.h"
#endif
#ifdef DEBUG_MODULE_ENABLED
#include "at32f435_437_debug.h"
#endif
#ifdef FLASH_MODULE_ENABLED
#include "at32f435_437_flash.h"
#endif
#ifdef CRC_MODULE_ENABLED
#include "at32f435_437_crc.h"
#endif
#ifdef WWDT_MODULE_ENABLED
#include "at32f435_437_wwdt.h"
#endif
#ifdef WDT_MODULE_ENABLED
#include "at32f435_437_wdt.h"
#endif
#ifdef EXINT_MODULE_ENABLED
#include "at32f435_437_exint.h"
#endif
#ifdef SDIO_MODULE_ENABLED
#include "at32f435_437_sdio.h"
#endif
#ifdef XMC_MODULE_ENABLED
#include "at32f435_437_xmc.h"
#endif
#ifdef ACC_MODULE_ENABLED
#include "at32f435_437_acc.h"
#endif
#ifdef MISC_MODULE_ENABLED
#include "at32f435_437_misc.h"
#endif
#ifdef EDMA_MODULE_ENABLED
#include "at32f435_437_edma.h"
#endif
#ifdef QSPI_MODULE_ENABLED
#include "at32f435_437_qspi.h"
#endif
#ifdef SCFG_MODULE_ENABLED
#include "at32f435_437_scfg.h"
#endif
#ifdef EMAC_MODULE_ENABLED
#include "at32f435_437_emac.h"
#endif
#ifdef DVP_MODULE_ENABLED
#include "at32f435_437_dvp.h"
#endif
#ifdef USB_MODULE_ENABLED
#include "at32f435_437_usb.h"
#endif

#include "defines_bootloader.h"
#include <stdint.h>

#ifdef __cplusplus
}
#endif

#endif