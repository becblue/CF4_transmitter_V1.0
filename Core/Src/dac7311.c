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
#include "stdio.h"  // 添加printf支持

/* 私有变量定义 */
static SPI_HandleTypeDef* dac_spi;  // SPI句柄
static GPIO_TypeDef* cs_port;       // 片选端口
static uint16_t cs_pin;             // 片选引脚

// 用于调试输出的宏定义
#define DEBUG_DAC7311 1  // 设置为1启用调试输出，设置为0禁用

#if DEBUG_DAC7311
#define DAC_DEBUG(format, ...) printf("[DAC7311] " format "\r\n", ##__VA_ARGS__)
#else
#define DAC_DEBUG(format, ...)
#endif

// 私有函数声明
static uint8_t DAC7311_Write16Bits(uint16_t data);  // 写入16位数据
static void DAC7311_Delay(void);                 // 简单延时
static void DAC7311_DelayNs(uint32_t ns);           // 基于系统时钟的精确延时

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
    uint8_t status;
    
    // 限制输入范围
    if(value > 4095) {
        value = 4095;
    }
    
    // 构造16位数据帧
    data = (uint16_t)(value & 0x0FFF);
    
    DAC_DEBUG("开始传输数据: 0x%04X (值: %d)", data, value);
    
    // 开始传输
    DAC_SYNC_LOW();          // 拉低SYNC，开始传输
    DAC_DEBUG("SYNC拉低 -> 开始传输");
    DAC7311_DelayNs(20);    // tSYNC ≥ 20ns
    
    // 发送16位数据
    status = DAC7311_Write16Bits(data);
    if(status != 0) {
        DAC_SYNC_HIGH();
        DAC_DEBUG("传输失败！SYNC提前拉高或数据不完整");
        return 1;  // 传输失败
    }
    
    // 结束传输
    DAC7311_DelayNs(20);    // tSYNCH ≥ 20ns
    DAC_SYNC_HIGH();        // 拉高SYNC，更新输出
    DAC_DEBUG("SYNC拉高 -> 传输完成");
    
    // 等待DAC更新完成
    DAC7311_DelayNs(8000);  // 转换时间 ≈ 8μs
    DAC_DEBUG("DAC更新完成，输出值设置为: %d", value);
    
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
  * @brief  基于系统时钟的精确延时
  * @param  ns: 需要延时的纳秒数
  * @retval None
  */
static void DAC7311_DelayNs(uint32_t ns)
{
    uint32_t cycles = (SystemCoreClock / 1000000) * ns / 1000;
    while(cycles--) {
        __NOP();
    }
}

/**
  * @brief  写入16位数据
  * @param  data: 要写入的16位数据
  * @retval 0: 成功, 1: 失败
  */
static uint8_t DAC7311_Write16Bits(uint16_t data)
{
    uint8_t i;
    uint8_t bit_count = 0;
    uint16_t data_temp = data;  // 保存原始数据用于调试输出
    
    // 确保CLK初始为低
    DAC_CLK_LOW();
    DAC_DEBUG("CLK初始化为低电平");
    DAC7311_DelayNs(10);  // tCL ≥ 10ns
    
    DAC_DEBUG("开始发送16位数据: 0x%04X", data);
    
    // 从最高位开始发送
    for(i = 0; i < 16; i++) {
        // 设置数据位
        if(data & 0x8000) {
            DAC_DIN_HIGH();
            DAC_DEBUG("位%2d: DIN=1 [CLK=L]", 15-i);
        } else {
            DAC_DIN_LOW();
            DAC_DEBUG("位%2d: DIN=0 [CLK=L]", 15-i);
        }
        
        DAC7311_DelayNs(5);   // tDS ≥ 5ns（数据建立时间）
        
        DAC_CLK_HIGH();       // CLK上升沿，锁存数据
        DAC_DEBUG("CLK上升沿 -> 锁存数据");
        DAC7311_DelayNs(10);  // tCH ≥ 10ns
        
        // 检查SYNC是否被提前拉高
        if(HAL_GPIO_ReadPin(DAC_GPIO_PORT, DAC_SYNC_PIN) == GPIO_PIN_SET) {
            DAC_DEBUG("错误：SYNC提前拉高！在位%d处中断", 15-i);
            return 1;  // 传输失败
        }
        
        DAC_CLK_LOW();        // CLK下降沿
        DAC_DEBUG("CLK下降沿 -> 准备下一位");
        DAC7311_DelayNs(10);  // tCL ≥ 10ns
        
        data <<= 1;
        bit_count++;
    }
    
    // 输出完整的传输摘要
    DAC_DEBUG("传输完成: 发送了%d位", bit_count);
    DAC_DEBUG("数据帧详情:");
    DAC_DEBUG("  - 电源模式[15:14]: %d%d", 
              (data_temp >> 15) & 0x01, 
              (data_temp >> 14) & 0x01);
    DAC_DEBUG("  - 保留位[13:12]: %d%d", 
              (data_temp >> 13) & 0x01, 
              (data_temp >> 12) & 0x01);
    DAC_DEBUG("  - DAC数据[11:0]: 0x%03X", 
              data_temp & 0x0FFF);
    
    return (bit_count == 16) ? 0 : 1;
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


