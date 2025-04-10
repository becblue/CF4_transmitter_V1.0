/**
  ******************************************************************************
  * @file    spi.h
  * @brief   SPI通信头文�?
  ******************************************************************************
  * @attention
  *
  * 本文件定义了SPI通信相关的函数接�?
  * 用于与DAC7311等设备通信
  *
  ******************************************************************************
  */

/* 防止递归包含 -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"  // 添加HAL库头文件
#include "stm32f1xx_hal_spi.h"  // 添加SPI相关定义

/* 导出变量 ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;  // SPI1句柄，用于DAC7311通信

/* 函数声明 ------------------------------------------------------------------*/
/**
  * @brief  SPI1 初始化函�?
  * @retval �?
  */
void MX_SPI1_Init(void);

/**
  * @brief  SPI错误回调函数
  * @param  hspi: SPI句柄指针
  * @retval �?
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);

/**
  * @brief  SPI反初始化函数
  * @param  hspi: SPI句柄指针
  * @retval �?
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */


