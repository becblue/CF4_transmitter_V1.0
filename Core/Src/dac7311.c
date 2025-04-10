/**
  ******************************************************************************
  * @file    dac7311.c
  * @brief   DAC7311数字-模拟转换器驱动实现文件
  *          DAC7311是一款12位的数模转换器，通过SPI接口控制
  ******************************************************************************
  */

/* 包含头文件 */
#include "dac7311.h"
#include "main.h"
#include "spi.h"

/**
  * @brief  初始化DAC7311
  * @retval uint8_t: 0表示成功，1表示失败
  */
uint8_t DAC7311_Init(void)
{
  /* 设置DAC为正常工作模式 */
  return DAC7311_SetPowerMode(DAC7311_POWER_DOWN_NORMAL);
}

/**
  * @brief  设置DAC7311输出值
  * @param  value: 要设置的DAC值，范围0-4095(12位)
  * @retval uint8_t: 0表示成功，1表示失败
  */
uint8_t DAC7311_SetValue(uint16_t value)
{
  uint16_t txData;
  HAL_StatusTypeDef status;
  
  /* 确保值在0-4095范围内 */
  if (value > 4095)
  {
    value = 4095;
  }
  
  /* 构建发送的数据
     格式: [15:14]=00(正常工作模式), [13:12]=00(保留), [11:0]=DAC数据 */
  txData = (DAC7311_POWER_DOWN_NORMAL << 12) | value;
  
  /* 使用SPI发送数据，选择DAC的NSS引脚 */
  HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_RESET); /* NSS拉低，选中DAC */
  status = HAL_SPI_Transmit(&hspi1, (uint8_t*)&txData, 2, 100);      /* 通过SPI发送数据 */
  HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_SET);   /* NSS拉高，释放DAC */
  
  return (status == HAL_OK) ? 0 : 1; /* 返回状态：0成功，1失败 */
}

/**
  * @brief  设置DAC7311电源模式
  * @param  mode: 电源模式，可以是以下值:
  *               DAC7311_POWER_DOWN_NORMAL - 正常工作模式
  *               DAC7311_POWER_DOWN_1K - 1K欧姆接地模式
  *               DAC7311_POWER_DOWN_100K - 100K欧姆接地模式
  *               DAC7311_POWER_DOWN_HI_Z - 高阻模式
  * @retval uint8_t: 0表示成功，1表示失败
  */
uint8_t DAC7311_SetPowerMode(uint16_t mode)
{
  uint16_t txData;
  HAL_StatusTypeDef status;
  
  /* 构建发送的数据 */
  txData = (mode << 12); /* 电源模式放在高两位 */
  
  /* 使用SPI发送数据 */
  HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_RESET); /* NSS拉低，选中DAC */
  status = HAL_SPI_Transmit(&hspi1, (uint8_t*)&txData, 2, 100);      /* 通过SPI发送数据 */
  HAL_GPIO_WritePin(DAC1SYNC_GPIO_Port, DAC1SYNC_Pin, GPIO_PIN_SET);   /* NSS拉高，释放DAC */
  
  return (status == HAL_OK) ? 0 : 1; /* 返回状态：0成功，1失败 */
}