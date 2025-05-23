/**
  ******************************************************************************
  * @file    dac7311.h
  * @brief   AD5621BK数模转换器驱动头文件
  ******************************************************************************
  * @attention
  *
  * 本文件包含所有AD5621BK控制相关的函数和宏定义
  * AD5621BK是一个12位数模转换器，通过SPI接口控制
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

// GPIO引脚定义
#define DAC_CLK_PIN     GPIO_PIN_13    // PB13 - CLK
#define DAC_DIN_PIN     GPIO_PIN_14    // PB14 - DIN
#define DAC_SYNC_PIN    GPIO_PIN_15    // PB15 - SYNC
#define DAC_GPIO_PORT   GPIOB          // 所有引脚都在GPIOB上

// GPIO位操作宏定义
#define DAC_CLK_HIGH()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_CLK_PIN, GPIO_PIN_SET)     // CLK置高
#define DAC_CLK_LOW()    HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_CLK_PIN, GPIO_PIN_RESET)   // CLK置低
#define DAC_DIN_HIGH()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_DIN_PIN, GPIO_PIN_SET)     // DIN置高
#define DAC_DIN_LOW()    HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_DIN_PIN, GPIO_PIN_RESET)   // DIN置低
#define DAC_SYNC_HIGH()  HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_SYNC_PIN, GPIO_PIN_SET)    // SYNC置高
#define DAC_SYNC_LOW()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_SYNC_PIN, GPIO_PIN_RESET)  // SYNC置低

/* 宏定义 --------------------------------------------------------------------*/
// AD5621BK电源模式定义
#define AD5621BK_POWER_DOWN_NORMAL     0x00    // 正常工作模式
#define AD5621BK_POWER_DOWN_1K         0x01    // 1K下拉关断
#define AD5621BK_POWER_DOWN_100K       0x02    // 100K下拉关断
#define AD5621BK_POWER_DOWN_HI_Z       0x03    // 高阻态

// AD5621BK命令定义
#define AD5621BK_CMD_WRITE_UPDATE_DAC  0x03    // 写入并更新DAC寄存器
#define AD5621BK_CMD_MASK              0x03    // 命令掩码 (Bit 4-5)
#define AD5621BK_PD_MASK              0x0C    // 电源模式掩码 (Bit 2-3)

// 兼容性定义（保持与旧代码兼容）
#define DAC7311_POWER_DOWN_NORMAL      AD5621BK_POWER_DOWN_NORMAL
#define DAC7311_POWER_DOWN_1K          AD5621BK_POWER_DOWN_1K
#define DAC7311_POWER_DOWN_100K        AD5621BK_POWER_DOWN_100K
#define DAC7311_POWER_DOWN_HI_Z        AD5621BK_POWER_DOWN_HI_Z
#define DAC7311_CMD_WRITE_UPDATE_DAC   AD5621BK_CMD_WRITE_UPDATE_DAC
#define DAC7311_CMD_MASK               AD5621BK_CMD_MASK
#define DAC7311_PD_MASK               AD5621BK_PD_MASK

// DAC相关定义
#define DAC_FULL_SCALE      4095    // DAC满量程值（12位DAC，最大值为4095）
#define DAC_RAMP_STEP       100     // DAC渐变步进值
#define DAC_RAMP_DELAY      10      // 每次渐变的延时(ms)

/* DAC7311相关宏定义 */
#define DAC7311_FULL_SCALE    4095    // DAC满量程值（12位DAC，最大值为4095）
#define DAC7311_RAMP_STEP     100     // DAC渐变步进值
#define DAC7311_RAMP_DELAY    10      // DAC渐变延时(ms)

/* 函数声明 ------------------------------------------------------------------*/
/**
  * @brief  DAC7311鍒濆鍖?
  * @param  hspi: SPI閫氫俊鍙ユ焺鎸囬拡
  * @param  cs_port: 鐗囬�夊紩鑴氭墍鍦ㄧ殑GPIO绔彛
  * @param  cs_pin: 鐗囬�夊紩鑴氱紪鍙?
  * @retval 鍒濆鍖栫粨鏋?: 0-鎴愬姛, 1-澶辫触
  */
uint8_t DAC7311_Init(void);

/**
  * @brief  璁剧疆DAC杈撳嚭鍊?
  * @param  value: 12浣岲AC鏁板�?(0-4095)
  * @retval 璁剧疆缁撴灉: 0-鎴愬姛, 1-澶辫触
  */
uint8_t DAC7311_SetValue(uint16_t value);

/**
  * @brief  璁剧疆DAC鐢垫簮妯″紡
  * @param  mode: 鐢垫簮妯″紡
  *               DAC7311_POWER_DOWN_NORMAL: 姝ｅ父宸ヤ綔妯″紡
  *               DAC7311_POWER_DOWN_1K: 1K涓嬫媺鍏虫柇
  *               DAC7311_POWER_DOWN_100K: 100K涓嬫媺鍏虫柇
  *               DAC7311_POWER_DOWN_HI_Z: 楂橀樆鎬?
  * @retval 璁剧疆缁撴灉: 0-鎴愬姛, 1-澶辫触
  */
uint8_t DAC7311_SetPowerMode(uint8_t mode);

/**
  * @brief  璁剧疆DAC杈撳嚭鐢靛帇
  * @param  voltage: 鏈熸湜杈撳嚭鐢靛帇(鍗曚綅:浼忕壒)
  * @param  vref: 鍙傝�冪數鍘?(鍗曚綅:浼忕壒)
  * @retval 璁剧疆缁撴灉: 0-鎴愬姛, 1-澶辫触
  */
uint8_t DAC7311_SetVoltage(float voltage, float vref);

void DAC7311_PowerDown(void);                // 进入掉电模式
void DAC7311_PowerUp(void);                  // 退出掉电模式

void DAC7311_RampToValue(uint16_t targetValue);                // 使用渐变方式设置DAC输出值

#ifdef __cplusplus
}
#endif

#endif /* __DAC7311_H */


