/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   此文件包含usart.c文件的所有函数原型
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * 本软件使用的许可条款可在本软件组件的根目录中的LICENSE文件中找到。
  * 如果没有随本软件提供LICENSE文件，则按"原样"提供。
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* 防止递归包含的定义 -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;  // USART1句柄，用于调试输出
extern UART_HandleTypeDef huart2;  // USART2句柄，用于传感器通信

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);  // USART1初始化函数
void MX_USART2_UART_Init(void);  // USART2初始化函数

/* USER CODE BEGIN Prototypes */
/* 调试输出相关函数 */
void USART1_Printf(const char* format, ...);  // 通过USART1发送格式化调试信息

/* 四氟化碳传感器通信相关函数 */
HAL_StatusTypeDef USART2_TransmitData(uint8_t *pData, uint16_t Size);  // 发送数据到传感器
HAL_StatusTypeDef USART2_ReceiveData(uint8_t *pData, uint16_t Size);   // 从传感器接收数据
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

