/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : main.c文件的头文件
  *                   此文件包含应用程序的通用定义
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含文件 ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* 私有包含文件 ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* 导出类型定义 ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* 导出常量 --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* 导出宏 ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* 导出函数原型 ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* 私有宏定义 -----------------------------------------------------------*/
#define PowerLed_Pin GPIO_PIN_4
#define PowerLed_GPIO_Port GPIOA
#define RunLed_Pin GPIO_PIN_5
#define RunLed_GPIO_Port GPIOA
#define Key1_Pin GPIO_PIN_6
#define Key1_GPIO_Port GPIOA
#define Key2_Pin GPIO_PIN_7
#define Key2_GPIO_Port GPIOA
#define DAC1CLK_Pin GPIO_PIN_13
#define DAC1CLK_GPIO_Port GPIOB
#define DAC1DIN_Pin GPIO_PIN_14
#define DAC1DIN_GPIO_Port GPIOB
#define DAC1SYNC_Pin GPIO_PIN_15
#define DAC1SYNC_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
