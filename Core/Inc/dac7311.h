/**
  ******************************************************************************
  * @file    dac7311.h
  * @brief   DAC7311数模转换器驱动头文件
  ******************************************************************************
  * @attention
  *
  * 本文件定义了与DAC7311控制相关的函数和宏
  * DAC7311是一个12位数模转换器，通过SPI接口控制
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
#include "spi.h"

/* 宏定义 --------------------------------------------------------------------*/
// DAC7311控制字节定义
#define DAC7311_POWER_DOWN_NORMAL     0x00    // 正常工作模式
#define DAC7311_POWER_DOWN_1K         0x01    // 1K下拉至地
#define DAC7311_POWER_DOWN_100K       0x02    // 100K下拉至地
#define DAC7311_POWER_DOWN_HI_Z       0x03    // 高阻态

// DAC7311命令掩码
#define DAC7311_CMD_MASK              0x30    // 命令掩码 (Bit 4-5)
#define DAC7311_PD_MASK               0x0C    // 掉电模式掩码 (Bit 2-3)

/* 函数声明 ------------------------------------------------------------------*/
/**
  * @brief  DAC7311初始化
  * @retval 初始化结果: 0-成功, 1-失败
  */
uint8_t DAC7311_Init(void);

/**
  * @brief  设置DAC输出值
  * @param  value: 12位DAC输出值(0-4095)
  * @retval 设置结果: 0-成功, 1-失败
  */
uint8_t DAC7311_SetValue(uint16_t value);

/**
  * @brief  设置DAC电源模式
  * @param  mode: 电源模式
  *               DAC7311_POWER_DOWN_NORMAL: 正常工作模式
  *               DAC7311_POWER_DOWN_1K: 1K下拉至地
  *               DAC7311_POWER_DOWN_100K: 100K下拉至地
  *               DAC7311_POWER_DOWN_HI_Z: 高阻态
  * @retval 设置结果: 0-成功, 1-失败
  */
uint8_t DAC7311_SetPowerMode(uint16_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __DAC7311_H */ 

