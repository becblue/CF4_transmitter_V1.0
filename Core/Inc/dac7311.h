/**
  ******************************************************************************
  * @file    dac7311.h
  * @brief   AD5621BKÊýÄ£×ª»»Æ÷Çý¶¯Í·ÎÄ¼þ
  ******************************************************************************
  * @attention
  *
  * ±¾ÎÄ¼þ°üº¬ËùÓÐAD5621BK¿ØÖÆÏà¹ØµÄº¯ÊýºÍºê¶¨Òå
  * AD5621BKÊÇÒ»¸ö12Î»ÊýÄ£×ª»»Æ÷£¬Í¨¹ýSPI½Ó¿Ú¿ØÖÆ
  * 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DAC7311_H
#define __DAC7311_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// GPIOÒý½Å¶¨Òå
#define DAC_CLK_PIN     GPIO_PIN_13    // PB13 - CLK
#define DAC_DIN_PIN     GPIO_PIN_14    // PB14 - DIN
#define DAC_SYNC_PIN    GPIO_PIN_15    // PB15 - SYNC
#define DAC_GPIO_PORT   GPIOB          // ËùÓÐÒý½Å¶¼ÔÚGPIOBÉÏ

// GPIOÎ»²Ù×÷ºê¶¨Òå
#define DAC_CLK_HIGH()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_CLK_PIN, GPIO_PIN_SET)     // CLKÖÃ¸ß
#define DAC_CLK_LOW()    HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_CLK_PIN, GPIO_PIN_RESET)   // CLKÖÃµÍ
#define DAC_DIN_HIGH()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_DIN_PIN, GPIO_PIN_SET)     // DINÖÃ¸ß
#define DAC_DIN_LOW()    HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_DIN_PIN, GPIO_PIN_RESET)   // DINÖÃµÍ
#define DAC_SYNC_HIGH()  HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_SYNC_PIN, GPIO_PIN_SET)    // SYNCÖÃ¸ß
#define DAC_SYNC_LOW()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_SYNC_PIN, GPIO_PIN_RESET)  // SYNCÖÃµÍ

/* ºê¶¨Òå --------------------------------------------------------------------*/
// AD5621BKµçÔ´Ä£Ê½¶¨Òå
#define AD5621BK_POWER_DOWN_NORMAL     0x00    // Õý³£¹¤×÷Ä£Ê½
#define AD5621BK_POWER_DOWN_1K         0x01    // 1KÏÂÀ­¹Ø¶Ï
#define AD5621BK_POWER_DOWN_100K       0x02    // 100KÏÂÀ­¹Ø¶Ï
#define AD5621BK_POWER_DOWN_HI_Z       0x03    // ¸ß×èÌ¬

// AD5621BKÃüÁî¶¨Òå
#define AD5621BK_CMD_WRITE_UPDATE_DAC  0x03    // Ð´Èë²¢¸üÐÂDAC¼Ä´æÆ÷
#define AD5621BK_CMD_MASK              0x03    // ÃüÁîÑÚÂë (Bit 4-5)
#define AD5621BK_PD_MASK              0x0C    // µçÔ´Ä£Ê½ÑÚÂë (Bit 2-3)

// ¼æÈÝÐÔ¶¨Òå£¨±£³ÖÓë¾É´úÂë¼æÈÝ£©
#define DAC7311_POWER_DOWN_NORMAL      AD5621BK_POWER_DOWN_NORMAL
#define DAC7311_POWER_DOWN_1K          AD5621BK_POWER_DOWN_1K
#define DAC7311_POWER_DOWN_100K        AD5621BK_POWER_DOWN_100K
#define DAC7311_POWER_DOWN_HI_Z        AD5621BK_POWER_DOWN_HI_Z
#define DAC7311_CMD_WRITE_UPDATE_DAC   AD5621BK_CMD_WRITE_UPDATE_DAC
#define DAC7311_CMD_MASK               AD5621BK_CMD_MASK
#define DAC7311_PD_MASK               AD5621BK_PD_MASK

// DACÏà¹Ø¶¨Òå
#define DAC_FULL_SCALE      4095    // DACÂúÁ¿³ÌÖµ£¨12Î»DAC£¬×î´óÖµÎª4095£©
#define DAC_RAMP_STEP       100     // DAC½¥±ä²½½øÖµ
#define DAC_RAMP_DELAY      10      // Ã¿´Î½¥±äµÄÑÓÊ±(ms)

/* º¯ÊýÉùÃ÷ ------------------------------------------------------------------*/
/**
  * @brief  DAC7311åˆå§‹åŒ?
  * @param  hspi: SPIé€šä¿¡å¥æŸ„æŒ‡é’ˆ
  * @param  cs_port: ç‰‡é€‰å¼•è„šæ‰€åœ¨çš„GPIOç«¯å£
  * @param  cs_pin: ç‰‡é€‰å¼•è„šç¼–å?
  * @retval åˆå§‹åŒ–ç»“æž?: 0-æˆåŠŸ, 1-å¤±è´¥
  */
uint8_t DAC7311_Init(void);

/**
  * @brief  è®¾ç½®DACè¾“å‡ºå€?
  * @param  value: 12ä½DACæ•°å€?(0-4095)
  * @retval è®¾ç½®ç»“æžœ: 0-æˆåŠŸ, 1-å¤±è´¥
  */
uint8_t DAC7311_SetValue(uint16_t value);

/**
  * @brief  è®¾ç½®DACç”µæºæ¨¡å¼
  * @param  mode: ç”µæºæ¨¡å¼
  *               DAC7311_POWER_DOWN_NORMAL: æ­£å¸¸å·¥ä½œæ¨¡å¼
  *               DAC7311_POWER_DOWN_1K: 1Kä¸‹æ‹‰å…³æ–­
  *               DAC7311_POWER_DOWN_100K: 100Kä¸‹æ‹‰å…³æ–­
  *               DAC7311_POWER_DOWN_HI_Z: é«˜é˜»æ€?
  * @retval è®¾ç½®ç»“æžœ: 0-æˆåŠŸ, 1-å¤±è´¥
  */
uint8_t DAC7311_SetPowerMode(uint8_t mode);

/**
  * @brief  è®¾ç½®DACè¾“å‡ºç”µåŽ‹
  * @param  voltage: æœŸæœ›è¾“å‡ºç”µåŽ‹(å•ä½:ä¼ç‰¹)
  * @param  vref: å‚è€ƒç”µåŽ?(å•ä½:ä¼ç‰¹)
  * @retval è®¾ç½®ç»“æžœ: 0-æˆåŠŸ, 1-å¤±è´¥
  */
uint8_t DAC7311_SetVoltage(float voltage, float vref);

void DAC7311_PowerDown(void);                // ½øÈëµôµçÄ£Ê½
void DAC7311_PowerUp(void);                  // ÍË³öµôµçÄ£Ê½

void DAC7311_RampToValue(uint16_t targetValue);                // Ê¹ÓÃ½¥±ä·½Ê½ÉèÖÃDACÊä³öÖµ

#ifdef __cplusplus
}
#endif

#endif /* __DAC7311_H */


