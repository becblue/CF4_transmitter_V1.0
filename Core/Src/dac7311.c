/**
  ******************************************************************************
  * @file    dac7311.c
  * @brief   DAC7311 驱动源文件
  *          该文件提供了DAC7311 12位数模转换器的控制功能实现
  ******************************************************************************
  * @attention
  *
  * DAC7311是一个12位数字-模拟转换器，使用GPIO模拟时序控制
  *
  ******************************************************************************
  */

/* 包含文件 */
#include "dac7311.h"
#include "gpio.h"
#include "main.h"
#include "stdio.h"  // 添加printf支持

/* 私有变量定义 */
static uint8_t dac_initialized = 0;  // DAC初始化标志

// 用于调试输出的宏定义
#define DEBUG_DAC7311 0  // 设置为1启用调试输出，设置为0禁用

#if DEBUG_DAC7311
#define DAC_DEBUG(format, ...) printf("[DAC7311] " format "\r\n", ##__VA_ARGS__)
#else
#define DAC_DEBUG(format, ...)
#endif

// 私有函数声明
static uint8_t DAC7311_Write16Bits(uint16_t data);  // 写入16位数据
static void DAC7311_Delay(void);                    // 简单延时
static void DAC7311_DelayNs(uint32_t ns);          // 基于系统时钟的精确延时

/**
  * @brief  初始化DAC7311
  * @retval 0: 成功, 1: 失败
  */
uint8_t DAC7311_Init(void)
{
    if (dac_initialized) {  // 如果已经初始化过
        return 0;
    }

    // 初始状态设置
    DAC_CLK_LOW();    // CLK初始为低
    DAC_DIN_LOW();    // DIN初始为低
    DAC_SYNC_HIGH();  // SYNC初始为高（未选中）
    
    // 上电延时
    HAL_Delay(10);
    
    // 发送上电复位值（设置输出为0）
    if (DAC7311_SetValue(0) != 0) {
        return 1;
    }

    dac_initialized = 1;  // 标记初始化完成
    return 0;
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
    data = (uint16_t)(value & 0x0FFF);  // 保留低12位
    
    DAC_DEBUG("开始传输数据: 0x%04X (值: %d)", data, value);
    
    // 开始传输
    DAC_SYNC_LOW();          // 拉低SYNC，开始传输
    DAC_DEBUG("SYNC拉低 -> 开始传输");
    DAC7311_DelayNs(20);    // tSYNC ≥ 20ns
    
    // 发送16位数据
    if (DAC7311_Write16Bits(data) != 0) {
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
  * @brief  写入16位数据
  * @param  data: 要写入的16位数据
  * @retval 0: 成功, 1: 失败
  */
static uint8_t DAC7311_Write16Bits(uint16_t data)
{
    uint8_t i;
    uint8_t bit_count = 0;
    
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
    
    DAC_DEBUG("传输完成: 发送了%d位", bit_count);
    
    return (bit_count == 16) ? 0 : 1;
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
  * @brief  设置DAC输出电压
  * @param  voltage: 期望输出电压(单位:伏特)
  * @param  vref: 参考电压(单位:伏特)
  * @retval 0: 成功, 1: 失败
  */
uint8_t DAC7311_SetVoltage(float voltage, float vref)
{
    uint16_t dac_value;
    
    /* 参数检查 */
    if (voltage < 0 || voltage > vref) {
        return 1;  // 电压超出范围
    }
    
    /* 计算DAC数值 */
    dac_value = (uint16_t)((voltage * 4095) / vref);
    
    /* 设置DAC值 */
    return DAC7311_SetValue(dac_value);
}


