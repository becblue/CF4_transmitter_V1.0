/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f1xx_hal_msp.c
  * @brief        此文件提供MSP初始化和反初始化代码
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

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* 私有类型定义 -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* 私有宏定义 ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* 私有宏 -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* 私有变量 ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* 私有函数原型 -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* 外部函数 --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * 初始化全局MSP
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* 系统中断初始化 */

  /** NOJTAG: JTAG-DP禁用，SW-DP启用
  */
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
