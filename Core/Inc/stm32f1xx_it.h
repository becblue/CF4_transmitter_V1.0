/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.h
  * @brief   此文件包含中断处理程序的头文件
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
#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

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
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
