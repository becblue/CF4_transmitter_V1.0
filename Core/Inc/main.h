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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
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
