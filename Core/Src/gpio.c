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
  * 锟斤拷锟斤拷锟绞癸拷玫锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷诒锟斤拷锟斤拷锟斤拷锟斤拷母锟侥柯硷拷械锟絃ICENSE锟侥硷拷锟斤拷锟揭碉拷锟斤拷
  * 锟斤拷锟矫伙拷锟斤拷姹撅拷锟斤拷锟结供LICENSE锟侥硷拷锟斤拷锟斤拷"原锟斤拷"锟结供锟斤拷
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};  // 定义GPIO初始化结构体并清零

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();  // 使能GPIOC时钟
  __HAL_RCC_GPIOD_CLK_ENABLE();  // 使能GPIOD时钟
  __HAL_RCC_GPIOA_CLK_ENABLE();  // 使能GPIOA时钟
  __HAL_RCC_GPIOB_CLK_ENABLE();  // 使能GPIOB时钟

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, POWER_Pin|RUN_Pin, GPIO_PIN_SET);  // 设置POWER和RUN引脚初始状态为高电平

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DAC1CLK_Pin|DAC1DIN_Pin|DAC1SYNC_Pin, GPIO_PIN_SET);  // 设置DAC相关引脚初始状态为高电平

  /*Configure GPIO pin : KEY2_Pin */
  GPIO_InitStruct.Pin = KEY2_Pin;  // 配置KEY2引脚
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // 设置为输入模式
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // 使能上拉
  HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct);  // 初始化KEY2引脚

  /*Configure GPIO pin : KEY1_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin;  // 配置KEY1引脚
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // 设置为输入模式
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // 使能上拉
  HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);  // 初始化KEY1引脚

  /*Configure GPIO pins : POWER_Pin RUN_Pin */
  GPIO_InitStruct.Pin = POWER_Pin|RUN_Pin;  // 配置POWER和RUN引脚
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 设置为推挽输出模式
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // 修改为上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置为高速模式
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  // 初始化POWER和RUN引脚

  /*Configure GPIO pins : DAC1CLK_Pin DAC1DIN_Pin DAC1SYNC_Pin */
  GPIO_InitStruct.Pin = DAC1CLK_Pin|DAC1DIN_Pin|DAC1SYNC_Pin;  // 配置DAC相关引脚
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 设置为推挽输出模式
  GPIO_InitStruct.Pull = GPIO_NOPULL;  // 无上下拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置为高速模式
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  // 初始化DAC相关引脚
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
