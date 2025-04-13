/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   锟斤拷锟侥硷拷锟结供USART实锟斤拷锟斤拷锟斤拷锟矫达拷锟斤拷
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * 锟斤拷锟斤拷锟绞癸拷玫锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷诒锟斤拷锟斤拷锟斤拷锟斤拷母锟侥柯硷拷械锟絃ICENSE锟侥硷拷锟斤拷锟揭碉拷锟斤拷锟斤拷
  * 锟斤拷锟矫伙拷锟斤拷姹撅拷锟斤拷锟结供LICENSE锟侥硷拷锟斤拷锟斤拷"原锟斤拷"锟结供锟斤拷
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* 锟斤拷锟斤拷USART1锟角凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟侥猴拷锟斤拷 */
void USART1_Test(void)
{
    char testMsg[] = "USART1 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟絓r\n";  // 锟斤拷锟斤拷锟斤拷息
    HAL_UART_Transmit(&huart1, (uint8_t *)testMsg, strlen(testMsg), 0xFFFF);  // 直锟斤拷通锟斤拷HAL锟解函锟斤拷锟斤拷锟酵诧拷锟斤拷锟斤拷息
    
    USART1_Printf("USART1_Printf锟斤拷锟斤拷: %d\r\n", 123);  // 锟斤拷锟斤拷USART1_Printf锟斤拷锟斤拷
    printf("printf锟截讹拷锟斤拷锟斤拷锟絓r\n");  // 锟斤拷锟斤拷printf锟截讹拷锟斤拷
}

/* 定义串口通信的超时时间，单位为毫秒 */
#define UART_TIMEOUT 1000  // 设置1秒的超时时间，可以根据实际应用需求调整

/* 定义接收缓冲区的最大大小，防止缓冲区溢出 */
#define MAX_RECEIVE_SIZE 256  // 设置最大接收字节数为256字节，可以根据实际应用需求调整

/* 通过USART3发送指定长度的数据，带错误处理和超时机制 */
HAL_StatusTypeDef USART3_TransmitData(uint8_t *pData, uint16_t Size)
{
    /* 检查输入参数的有效性 */
    if(pData == NULL || Size == 0)  // 判断数据指针是否为空或数据长度是否为0
    {
        return HAL_ERROR;  // 如果参数无效，返回错误状态
    }

    /* 使用HAL库函数发送数据，使用预设的超时时间 */
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart3, pData, Size, UART_TIMEOUT);  // 发送数据，并获取发送状态

    /* 检查发送结果并进行错误处理 */
    if(status != HAL_OK)  // 如果发送不成功
    {
        /* 可以在这里添加错误处理代码，比如重试或记录错误日志 */
        Error_Handler();  // 调用错误处理函数
    }

    return status;  // 返回发送操作的状态
}

/* 通过USART3接收指定长度的数据，带错误处理、超时机制和缓冲区保护 */
HAL_StatusTypeDef USART3_ReceiveData(uint8_t *pData, uint16_t Size)
{
    /* 检查输入参数的有效性 */
    if(pData == NULL || Size == 0)  // 判断数据指针是否为空或数据长度是否为0
    {
        return HAL_ERROR;  // 如果参数无效，返回错误状态
    }

    /* 检查接收数据大小是否超过缓冲区最大容量 */
    if(Size > MAX_RECEIVE_SIZE)  // 判断要接收的数据长度是否超过最大限制
    {
        return HAL_ERROR;  // 如果超过最大接收大小，返回错误状态
    }

    /* 使用HAL库函数接收数据，使用预设的超时时间 */
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart3, pData, Size, UART_TIMEOUT);  // 接收数据，并获取接收状态

    /* 检查接收结果并进行错误处理 */
    if(status != HAL_OK)  // 如果接收不成功
    {
        /* 可以在这里添加错误处理代码，比如重试或记录错误日志 */
        Error_Handler();  // 调用错误处理函数
    }

    return status;  // 返回接收操作的状态
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
  huart3.Instance = USART3;  // 选择USART3作为串口实例
  huart3.Init.BaudRate = 9600;  // 设置波特率为9600
  huart3.Init.WordLength = UART_WORDLENGTH_8B;  // 设置数据位为8位
  huart3.Init.StopBits = UART_STOPBITS_1;  // 设置停止位为1位
  huart3.Init.Parity = UART_PARITY_NONE;  // 设置无校验位
  huart3.Init.Mode = UART_MODE_TX_RX;  // 设置为收发模式
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;  // 设置无硬件流控
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;  // 设置16倍过采样
  if (HAL_UART_Init(&huart3) != HAL_OK)  // 初始化UART配置
  {
    Error_Handler();  // 如果初始化失败，调用错误处理函数
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

/* 锟截讹拷锟斤拷printf锟斤拷USART1 */
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);  // 通锟斤拷USART1锟斤拷锟斤拷锟街凤拷
    return ch;
}

/* 通锟斤拷USART1锟斤拷锟酵碉拷锟斤拷锟斤拷息 */
void USART1_Printf(const char* format, ...)
{
    char buf[256];  // 锟斤拷锟藉缓锟斤拷锟斤拷
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);  // 锟斤拷式锟斤拷锟街凤拷锟斤拷
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 0xFFFF);  // 锟斤拷锟酵革拷式锟斤拷锟斤拷锟斤拷址锟斤拷锟???
    va_end(args);
}

/* USER CODE END 1 */
