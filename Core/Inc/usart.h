/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   ���ļ�����usart.c�ļ������к���ԭ��
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
/* ���������غ��� */
void USART1_Printf(const char* format, ...);  // ͨ��USART1���͸�ʽ��������Ϣ

/* �ķ���̼������ͨ����غ��� */
HAL_StatusTypeDef USART2_TransmitData(uint8_t *pData, uint16_t Size);  // �������ݵ�������
HAL_StatusTypeDef USART2_ReceiveData(uint8_t *pData, uint16_t Size);   // �Ӵ�������������
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

