/**
  ******************************************************************************
  * @file    spi.c
  * @brief   SPI通信源文件
  ******************************************************************************
  * @attention
  *
  * 本文件实现了SPI通信相关的函数
  * 用于与DAC7311等设备通信
  *
  ******************************************************************************
  */

/* 包含文件 ------------------------------------------------------------------*/
#include "spi.h"

/* 私有类型定义 -----------------------------------------------------------*/
/* 私有宏定义 ------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;  // SPI1句柄，用于DAC7311通信

/* 函数实现 --------------------------------------------------------------*/

/**
  * @brief  SPI1 初始化函数
  * @retval 无
  */
void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;                       // 使用SPI1
  hspi1.Init.Mode = SPI_MODE_MASTER;           // 主机模式
  hspi1.Init.Direction = SPI_DIRECTION_2LINES; // 双线单向模式
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;     // 8位数据帧
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;   // 时钟极性，空闲状态为低电平
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;       // 第一个边沿采样
  hspi1.Init.NSS = SPI_NSS_SOFT;               // 软件控制片选信号
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // 波特率，PCLK2/16
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;      // 高位先行
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;      // 禁用TI模式
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // 禁用CRC校验
  hspi1.Init.CRCPolynomial = 10;               // CRC多项式

  // 初始化SPI
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();  // 初始化失败进入错误处理
  }
}

/**
  * @brief  SPI错误回调函数
  * @param  hspi: SPI句柄指针
  * @retval 无
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  if(hspi->Instance == SPI1)
  {
    // 使能SPI1和GPIOA时钟
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 配置SPI1的SCK引脚
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;     // 复用推挽输出
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置SPI1的MOSI引脚
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置DAC1的CS引脚(DAC1SYNC)
    GPIO_InitStruct.Pin = DAC1SYNC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;           // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(DAC1SYNC_GPIO_Port, &GPIO_InitStruct);
    
    // 初始状态下CS引脚拉高（禁用DAC）
    HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_SET);
  }
}

/**
  * @brief  SPI反初始化函数
  * @param  hspi: SPI句柄指针
  * @retval 无
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance == SPI1)
  {
    // 禁用SPI1时钟
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    // 取消SPI引脚的配置
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_7);
    
    // 取消DAC1 CS引脚的配置
    HAL_GPIO_DeInit(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin);
  }
} 