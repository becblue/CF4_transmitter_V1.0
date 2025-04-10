/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   中断服务程序
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
#include "stm32f1xx_it.h"
/* 私有包含文件 ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* 私有类型定义 -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* 私有宏定义 ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* 私有宏 -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* 私有变量 ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* 私有函数原型 -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* 私有用户代码 ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* 外部变量 --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3处理器中断和异常处理程序          */
/******************************************************************************/
/**
  * @brief 此函数处理不可屏蔽中断
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief 此函数处理硬件错误中断
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief 此函数处理内存管理错误
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief 此函数处理预取错误，内存访问错误
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief 此函数处理未定义指令或非法状态
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief 此函数处理通过SWI指令的系统服务调用
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief 此函数处理调试监视器
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief 此函数处理系统服务的挂起请求
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief 此函数处理系统滴答定时器
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx外设中断处理程序                                    */
/* 在此处添加使用的外设的中断处理程序                                 */
/* 有关可用的外设中断处理程序名称，                      */
/* 请参阅启动文件 (startup_stm32f1xx.s)                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
