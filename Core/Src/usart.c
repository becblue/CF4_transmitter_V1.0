/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   ���ļ��ṩUSARTʵ�������ô���
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * �����ʹ�õ����������ڱ��������ĸ�Ŀ¼�е�LICENSE�ļ����ҵ�����
  * ���û���汾����ṩLICENSE�ļ�����"ԭ��"�ṩ��
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* ����USART1�Ƿ����������ĺ��� */
void USART1_Test(void)
{
    char testMsg[] = "USART1 �����������\r\n";  // ������Ϣ
    HAL_UART_Transmit(&huart1, (uint8_t *)testMsg, strlen(testMsg), 0xFFFF);  // ֱ��ͨ��HAL�⺯�����Ͳ�����Ϣ
    
    USART1_Printf("USART1_Printf����: %d\r\n", 123);  // ����USART1_Printf����
    printf("printf�ض������\r\n");  // ����printf�ض���
}

/* ���崮��ͨ�ŵĳ�ʱʱ�䣬��λΪ���� */
#define UART_TIMEOUT 1000  // ����1��ĳ�ʱʱ�䣬���Ը���ʵ��Ӧ���������

/* ������ջ�����������С����ֹ��������� */
#define MAX_RECEIVE_SIZE 256  // �����������ֽ���Ϊ256�ֽڣ����Ը���ʵ��Ӧ���������

/* ͨ��USART3����ָ�����ȵ����ݣ���������ͳ�ʱ���� */
HAL_StatusTypeDef USART3_TransmitData(uint8_t *pData, uint16_t Size)
{
    /* ��������������Ч�� */
    if(pData == NULL || Size == 0)  // �ж�����ָ���Ƿ�Ϊ�ջ����ݳ����Ƿ�Ϊ0
    {
        return HAL_ERROR;  // ���������Ч�����ش���״̬
    }

    /* ʹ��HAL�⺯���������ݣ�ʹ��Ԥ��ĳ�ʱʱ�� */
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart3, pData, Size, UART_TIMEOUT);  // �������ݣ�����ȡ����״̬

    /* ��鷢�ͽ�������д����� */
    if(status != HAL_OK)  // ������Ͳ��ɹ�
    {
        /* ������������Ӵ�������룬�������Ի��¼������־ */
        Error_Handler();  // ���ô�������
    }

    return status;  // ���ط��Ͳ�����״̬
}

/* ͨ��USART3����ָ�����ȵ����ݣ�����������ʱ���ƺͻ��������� */
HAL_StatusTypeDef USART3_ReceiveData(uint8_t *pData, uint16_t Size)
{
    /* ��������������Ч�� */
    if(pData == NULL || Size == 0)  // �ж�����ָ���Ƿ�Ϊ�ջ����ݳ����Ƿ�Ϊ0
    {
        return HAL_ERROR;  // ���������Ч�����ش���״̬
    }

    /* ���������ݴ�С�Ƿ񳬹�������������� */
    if(Size > MAX_RECEIVE_SIZE)  // �ж�Ҫ���յ����ݳ����Ƿ񳬹��������
    {
        return HAL_ERROR;  // ������������մ�С�����ش���״̬
    }

    /* ʹ��HAL�⺯���������ݣ�ʹ��Ԥ��ĳ�ʱʱ�� */
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart3, pData, Size, UART_TIMEOUT);  // �������ݣ�����ȡ����״̬

    /* �����ս�������д����� */
    if(status != HAL_OK)  // ������ղ��ɹ�
    {
        /* ������������Ӵ�������룬�������Ի��¼������־ */
        Error_Handler();  // ���ô�������
    }

    return status;  // ���ؽ��ղ�����״̬
}

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;  // ѡ��USART3��Ϊ����ʵ��
  huart3.Init.BaudRate = 9600;  // ���ò�����Ϊ9600
  huart3.Init.WordLength = UART_WORDLENGTH_8B;  // ��������λΪ8λ
  huart3.Init.StopBits = UART_STOPBITS_1;  // ����ֹͣλΪ1λ
  huart3.Init.Parity = UART_PARITY_NONE;  // ������У��λ
  huart3.Init.Mode = UART_MODE_TX_RX;  // ����Ϊ�շ�ģʽ
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;  // ������Ӳ������
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;  // ����16��������
  if (HAL_UART_Init(&huart3) != HAL_OK)  // ��ʼ��UART����
  {
    Error_Handler();  // �����ʼ��ʧ�ܣ����ô�������
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/* �ض���printf��USART1 */
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);  // ͨ��USART1�����ַ�
    return ch;
}

/* ͨ��USART1���͵�����Ϣ */
void USART1_Printf(const char* format, ...)
{
    char buf[256];  // ���建����
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);  // ��ʽ���ַ���
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 0xFFFF);  // ���͸�ʽ������ַ���???
    va_end(args);
}

/* USER CODE END 1 */
