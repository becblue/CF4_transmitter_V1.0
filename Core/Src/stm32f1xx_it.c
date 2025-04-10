/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   �жϷ������
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
#include "main.h"
#include "stm32f1xx_it.h"
/* ˽�а����ļ� ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* ˽�����Ͷ��� -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* ˽�к궨�� ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* ˽�к� -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* ˽�б��� ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* ˽�к���ԭ�� -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* ˽���û����� ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* �ⲿ���� --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3�������жϺ��쳣�������          */
/******************************************************************************/
/**
  * @brief �˺��������������ж�
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
  * @brief �˺�������Ӳ�������ж�
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
  * @brief �˺��������ڴ�������
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
  * @brief �˺�������Ԥȡ�����ڴ���ʴ���
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
  * @brief �˺�������δ����ָ���Ƿ�״̬
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
  * @brief �˺�������ͨ��SWIָ���ϵͳ�������
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief �˺���������Լ�����
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief �˺�������ϵͳ����Ĺ�������
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief �˺�������ϵͳ�δ�ʱ��
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
/* STM32F1xx�����жϴ������                                    */
/* �ڴ˴����ʹ�õ�������жϴ������                                 */
/* �йؿ��õ������жϴ���������ƣ�                      */
/* ����������ļ� (startup_stm32f1xx.s)                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
