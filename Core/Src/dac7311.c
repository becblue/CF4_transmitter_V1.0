/**
  ******************************************************************************
  * @file    dac7311.c
  * @brief   DAC7311 驱动源文件
  *          该文件提供了DAC7311 12位数模转换器的控制功能实现
  ******************************************************************************
  * @attention
  *
  * DAC7311鏄竴涓?12浣嶆暟瀛?-妯℃嫙杞崲鍣紝閫氳繃SPI鎺ュ彛鎺у埗
  *
  ******************************************************************************
  */

/* 鍖呭惈澶存枃浠? */
#include "dac7311.h"
#include "main.h"

/* 私有变量定义 */
static SPI_HandleTypeDef* dac_spi;  // SPI句柄
static GPIO_TypeDef* cs_port;       // 片选端口
static uint16_t cs_pin;             // 片选引脚

/**
  * @brief  初始化DAC7311
  * @param  hspi: SPI句柄指针
  * @param  CSPort: 片选GPIO端口
  * @param  CSPin: 片选GPIO引脚
  * @retval uint8_t: 0表示成功，1表示失败
  */
uint8_t DAC7311_Init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* CSPort, uint16_t CSPin)
{
    /* 保存SPI和GPIO配置 */
    dac_spi = hspi;
    cs_port = CSPort;
    cs_pin = CSPin;
    
    /* 初始化时拉高CS */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    
    /* 设置初始输出为0，并使用正常工作模式 */
    return DAC7311_SetValue(0);
}

/**
  * @brief  设置DAC输出值
  * @param  value: 12位DAC值(0-4095)
  * @retval uint8_t: 0表示成功，1表示失败
  */
uint8_t DAC7311_SetValue(uint16_t value)
{
    uint16_t dac_cmd;
    HAL_StatusTypeDef status;
    
    /* 限制输入范围 */
    if(value > 4095) value = 4095;
    
    /* 构建命令字 */
    dac_cmd = (DAC7311_CMD_WRITE_UPDATE_DAC << 12) | value;
    
    /* 拉低CS开始传输 */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    
    /* 发送数据 */
    status = HAL_SPI_Transmit(dac_spi, (uint8_t*)&dac_cmd, 2, HAL_MAX_DELAY);
    
    /* 拉高CS结束传输 */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    
    return (status == HAL_OK) ? 0 : 1;
}

/**
  * @brief  设置DAC电源模式
  * @param  mode: 电源模式(见dac7311.h中的宏定义)
  * @retval uint8_t: 0表示成功，1表示失败
  */
uint8_t DAC7311_SetPowerMode(uint8_t mode)
{
    uint16_t dac_cmd;
    HAL_StatusTypeDef status;
    
    /* 构建命令字，保持当前DAC值不变 */
    dac_cmd = (mode << 12);
    
    /* 拉低CS开始传输 */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    
    /* 发送数据 */
    status = HAL_SPI_Transmit(dac_spi, (uint8_t*)&dac_cmd, 2, HAL_MAX_DELAY);
    
    /* 拉高CS结束传输 */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    
    return (status == HAL_OK) ? 0 : 1;
}

/**
  * @brief  璁剧疆DAC杈撳嚭鐢靛帇
  * @param  voltage: 鏈熸湜杈撳嚭鐢靛帇(鍗曚綅:浼忕壒)
  * @param  vref: 鍙傝�冪數鍘?(鍗曚綅:浼忕壒)
  * @retval 0: 鎴愬姛, 1: 澶辫触
  */
uint8_t DAC7311_SetVoltage(float voltage, float vref)
{
  uint16_t dac_value;
  
  /* 鍙傛暟妫�鏌? */
  if (voltage < 0 || voltage > vref)
  {
    return 1; // 鐢靛帇瓒呭嚭鑼冨洿
  }
  
  /* 计算DAC数值 */
  dac_value = (uint16_t)((voltage * 4095) / vref);
  
  /* 设置DAC值 */
  return DAC7311_SetValue(dac_value);
}


