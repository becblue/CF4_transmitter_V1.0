/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* 配置GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** 配置引脚为
        * 模拟量
        * 输入
        * 输出
        * 事件输出
        * 外部中断
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO端口时钟使能 */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*配置GPIO引脚输出电平 */
  HAL_GPIO_WritePin(GPIOA, PowerLed_Pin|RunLed_Pin, GPIO_PIN_RESET);

  /*配置GPIO引脚输出电平 */
  HAL_GPIO_WritePin(GPIOB, DAC1CLK_Pin|DAC1DIN_Pin|DAC1SYNC_Pin, GPIO_PIN_SET);

  /*配置GPIO引脚 : PowerLed_Pin RunLed_Pin */
  GPIO_InitStruct.Pin = PowerLed_Pin|RunLed_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*配置GPIO引脚 : Key1_Pin Key2_Pin */
  GPIO_InitStruct.Pin = Key1_Pin|Key2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*配置GPIO引脚 : DAC1CLK_Pin DAC1DIN_Pin DAC1SYNC_Pin */
  GPIO_InitStruct.Pin = DAC1CLK_Pin|DAC1DIN_Pin|DAC1SYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
