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

// 私有函数声明
static void DAC7311_Write16Bits(uint16_t data);  // 写入16位数据
static void DAC7311_Delay(void);                 // 简单延时

/**
  * @brief  初始化DAC7311
  * @retval 0: 成功, 1: 失败
  */
uint8_t DAC7311_Init(void)
{
    // 初始状态设置
    DAC_CLK_LOW();    // CLK初始为低
    DAC_DIN_LOW();    // DIN初始为低
    DAC_SYNC_HIGH();  // SYNC初始为高（未选中）
    
    // 上电延时
    HAL_Delay(10);
    
    // 发送上电复位值（设置输出为0）
    return DAC7311_SetValue(0);
}

/**
  * @brief  设置DAC输出值
  * @param  value: 12位DAC值（0-4095）
  * @retval 0: 成功, 1: 失败
  */
uint8_t DAC7311_SetValue(uint16_t value)
{
    uint16_t data;
    
    // 限制输入范围
    if(value > 4095) {
        value = 4095;
    }
    
    // 构造16位数据帧
    // [15:14]: PD1,PD0 = 00 (正常工作模式)
    // [13:12]: 保留位 = 00
    // [11:0]: 12位DAC数据
    data = (uint16_t)(value & 0x0FFF);  // 取低12位
    
    // 开始传输
    DAC_SYNC_LOW();   // 拉低SYNC，开始传输
    DAC7311_Delay();  // 短延时
    
    // 发送16位数据
    DAC7311_Write16Bits(data);
    
    // 结束传输
    DAC7311_Delay();  // 短延时
    DAC_SYNC_HIGH();  // 拉高SYNC，结束传输
    
    return 0;
}

/**
  * @brief  进入掉电模式
  * @retval None
  */
void DAC7311_PowerDown(void)
{
    uint16_t data = 0x2000;  // PD1,PD0 = 10 (掉电模式1)
    
    DAC_SYNC_LOW();
    DAC7311_Delay();
    DAC7311_Write16Bits(data);
    DAC7311_Delay();
    DAC_SYNC_HIGH();
}

/**
  * @brief  退出掉电模式
  * @retval None
  */
void DAC7311_PowerUp(void)
{
    uint16_t data = 0x0000;  // PD1,PD0 = 00 (正常工作模式)
    
    DAC_SYNC_LOW();
    DAC7311_Delay();
    DAC7311_Write16Bits(data);
    DAC7311_Delay();
    DAC_SYNC_HIGH();
}

/**
  * @brief  写入16位数据
  * @param  data: 要写入的16位数据
  * @retval None
  */
static void DAC7311_Write16Bits(uint16_t data)
{
    uint8_t i;
    
    // 从最高位开始发送
    for(i = 0; i < 16; i++) {
        DAC_CLK_LOW();  // CLK低电平
        
        // 设置数据位
        if(data & 0x8000) {
            DAC_DIN_HIGH();  // 发送1
        } else {
            DAC_DIN_LOW();   // 发送0
        }
        
        DAC7311_Delay();  // 建立时间
        DAC_CLK_HIGH();   // CLK上升沿，数据锁存
        DAC7311_Delay();  // 保持时间
        
        data <<= 1;  // 左移一位，准备发送下一位
    }
    
    DAC_CLK_LOW();  // 传输结束，CLK回到低电平
}

/**
  * @brief  简单延时函数
  * @retval None
  */
static void DAC7311_Delay(void)
{
    uint8_t i;
    for(i = 0; i < 10; i++) {
        __NOP();  // 空操作
    }
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


