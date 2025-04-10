/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : main.c�ļ���ͷ�ļ�
  *                   ���ļ�����Ӧ�ó����ͨ�ö���
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

/* ��ֹ�ݹ�����Ķ��� -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* �����ļ� ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* ˽�а����ļ� ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* �������Ͷ��� ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* �������� --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* ������ ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* ��������ԭ�� ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* ˽�к궨�� -----------------------------------------------------------*/
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
