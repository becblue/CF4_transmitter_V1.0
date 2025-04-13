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
  * �����ʹ�õ����������ڱ��������ĸ�Ŀ¼�е�LICENSE�ļ����ҵ���
  * ���û���汾����ṩLICENSE�ļ�����"ԭ��"�ṩ��
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};  // ����GPIO��ʼ���ṹ�岢����

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();  // ʹ��GPIOCʱ��
  __HAL_RCC_GPIOD_CLK_ENABLE();  // ʹ��GPIODʱ��
  __HAL_RCC_GPIOA_CLK_ENABLE();  // ʹ��GPIOAʱ��
  __HAL_RCC_GPIOB_CLK_ENABLE();  // ʹ��GPIOBʱ��

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, POWER_Pin|RUN_Pin, GPIO_PIN_SET);  // ����POWER��RUN���ų�ʼ״̬Ϊ�ߵ�ƽ

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DAC1CLK_Pin|DAC1DIN_Pin|DAC1SYNC_Pin, GPIO_PIN_SET);  // ����DAC������ų�ʼ״̬Ϊ�ߵ�ƽ

  /*Configure GPIO pin : KEY2_Pin */
  GPIO_InitStruct.Pin = KEY2_Pin;  // ����KEY2����
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // ����Ϊ����ģʽ
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // ʹ������
  HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct);  // ��ʼ��KEY2����

  /*Configure GPIO pin : KEY1_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin;  // ����KEY1����
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // ����Ϊ����ģʽ
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // ʹ������
  HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);  // ��ʼ��KEY1����

  /*Configure GPIO pins : POWER_Pin RUN_Pin */
  GPIO_InitStruct.Pin = POWER_Pin|RUN_Pin;  // ����POWER��RUN����
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // ����Ϊ�������ģʽ
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // �޸�Ϊ����
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // ����Ϊ����ģʽ
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  // ��ʼ��POWER��RUN����

  /*Configure GPIO pins : DAC1CLK_Pin DAC1DIN_Pin DAC1SYNC_Pin */
  GPIO_InitStruct.Pin = DAC1CLK_Pin|DAC1DIN_Pin|DAC1SYNC_Pin;  // ����DAC�������
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // ����Ϊ�������ģʽ
  GPIO_InitStruct.Pull = GPIO_NOPULL;  // ��������
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // ����Ϊ����ģʽ
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  // ��ʼ��DAC�������
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
