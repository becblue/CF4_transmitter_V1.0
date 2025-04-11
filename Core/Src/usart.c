/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   ï¿½ï¿½ï¿½Ä¼ï¿½ï¿½á¹©USARTÊµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã´ï¿½ï¿½ï¿½
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * ï¿½ï¿½ï¿½ï¿½ï¿½Ê¹ï¿½Ãµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ú±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä¸ï¿½Ä¿Â¼ï¿½Ðµï¿½LICENSEï¿½Ä¼ï¿½ï¿½ï¿½ï¿½Òµï¿½ï¿½ï¿½
  * ï¿½ï¿½ï¿½Ã»ï¿½ï¿½ï¿½æ±¾ï¿½ï¿½ï¿½ï¿½á¹©LICENSEï¿½Ä¼ï¿½ï¿½ï¿½ï¿½ï¿½"Ô­ï¿½ï¿½"ï¿½á¹©ï¿½ï¿½
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* ï¿½ï¿½ï¿½ï¿½USART1ï¿½Ç·ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Äºï¿½ï¿½ï¿½ */
void USART1_Test(void)
{
    char testMsg[] = "USART1 ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n";  // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ï¢
    HAL_UART_Transmit(&huart1, (uint8_t *)testMsg, strlen(testMsg), 0xFFFF);  // Ö±ï¿½ï¿½Í¨ï¿½ï¿½HALï¿½âº¯ï¿½ï¿½ï¿½ï¿½ï¿½Í²ï¿½ï¿½ï¿½ï¿½ï¿½Ï¢
    
    USART1_Printf("USART1_Printfï¿½ï¿½ï¿½ï¿½: %d\r\n", 123);  // ï¿½ï¿½ï¿½ï¿½USART1_Printfï¿½ï¿½ï¿½ï¿½
    printf("printfï¿½Ø¶ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");  // ï¿½ï¿½ï¿½ï¿½printfï¿½Ø¶ï¿½ï¿½ï¿½
}

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;  // USART1±£³Ö115200²¨ÌØÂÊ
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
  
  /* USART1 interrupt Init */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);  // ÉèÖÃUSART1ÖÐ¶ÏÓÅÏÈ¼¶
  HAL_NVIC_EnableIRQ(USART1_IRQn);  // Ê¹ÄÜUSART1ÖÐ¶Ï
}

/* USART2 init function */

void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;  // ÐÞ¸ÄUSART2²¨ÌØÂÊÎª9600
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* USART2 interrupt Init */
  HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);  // ÉèÖÃUSART2ÖÐ¶ÏÓÅÏÈ¼¶
  HAL_NVIC_EnableIRQ(USART2_IRQn);  // Ê¹ÄÜUSART2ÖÐ¶Ï
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
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
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
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/* ï¿½Ø¶ï¿½ï¿½ï¿½printfï¿½ï¿½USART1 */
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);  // Í¨ï¿½ï¿½USART1ï¿½ï¿½ï¿½ï¿½ï¿½Ö·ï¿½
    return ch;
}

/* Í¨ï¿½ï¿½USART1ï¿½ï¿½ï¿½Íµï¿½ï¿½ï¿½ï¿½ï¿½Ï¢ */
void USART1_Printf(const char* format, ...)
{
    char buf[256];  // ï¿½ï¿½ï¿½å»ºï¿½ï¿½ï¿½ï¿½
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);  // ï¿½ï¿½Ê½ï¿½ï¿½ï¿½Ö·ï¿½ï¿½ï¿½
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 0xFFFF);  // ï¿½ï¿½ï¿½Í¸ï¿½Ê½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö·ï¿½ï¿½ï¿?
    va_end(args);
}

/* Í¨ï¿½ï¿½USART2ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ýµï¿½ï¿½Ä·ï¿½ï¿½ï¿½Ì¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
HAL_StatusTypeDef USART2_TransmitData(uint8_t *pData, uint16_t Size)
{
    return HAL_UART_Transmit(&huart2, pData, Size, 0xFFFF);  // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ýµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
}

/* ï¿½ï¿½ï¿½Ä·ï¿½ï¿½ï¿½Ì¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
HAL_StatusTypeDef USART2_ReceiveData(uint8_t *pData, uint16_t Size)
{
    return HAL_UART_Receive(&huart2, pData, Size, 0xFFFF);  // ï¿½Ó´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
}

/* USER CODE END 1 */
