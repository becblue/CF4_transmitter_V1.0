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

/* �����ļ� ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* ����GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** ��������Ϊ
        * ģ����
        * ����
        * ���
        * �¼����
        * �ⲿ�ж�
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO�˿�ʱ��ʹ�� */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*����GPIO���������ƽ */
  HAL_GPIO_WritePin(GPIOA, PowerLed_Pin|RunLed_Pin, GPIO_PIN_RESET);

  /*����GPIO���������ƽ */
  HAL_GPIO_WritePin(GPIOB, DAC1CLK_Pin|DAC1DIN_Pin|DAC1SYNC_Pin, GPIO_PIN_SET);

  /*����GPIO���� : PowerLed_Pin RunLed_Pin */
  GPIO_InitStruct.Pin = PowerLed_Pin|RunLed_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*����GPIO���� : Key1_Pin Key2_Pin */
  GPIO_InitStruct.Pin = Key1_Pin|Key2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*����GPIO���� : DAC1CLK_Pin DAC1DIN_Pin DAC1SYNC_Pin */
  GPIO_InitStruct.Pin = DAC1CLK_Pin|DAC1DIN_Pin|DAC1SYNC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
