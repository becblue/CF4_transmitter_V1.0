/**
  ******************************************************************************
  * @file    dac7311.c
  * @brief   AD5621BK 驱动源文件
  *          该文件提供了AD5621BK 12位数模转换器的控制功能实现
  ******************************************************************************
  * @attention
  *
  * AD5621BK是一个12位数字-模拟转换器，使用GPIO模拟时序控制
  * 时序要求：
  * - 最大时钟频率：50MHz
  * - 数据建立时间(tDS)：最小10ns
  * - 数据保持时间(tDH)：最小10ns
  * - SYNC低电平持续时间：最小12.5ns
  * - SYNC高电平持续时间：最小12.5ns
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
static uint16_t lastValue = 0;       // 记录DAC最后设置的值

// 用于调试输出的宏定义
#define DEBUG_DAC 0  // 设置为1启用调试输出，设置为0禁用

#if DEBUG_DAC
#define DAC_DEBUG(format, ...) printf("[AD5621BK] " format "\r\n", ##__VA_ARGS__)
#else
#define DAC_DEBUG(format, ...)
#endif

// 私有函数声明
static uint8_t DAC7311_Write16Bits(uint16_t data);  // 写入16位数据
static void DAC7311_Delay(void);                    // 简单延时
static void DAC7311_DelayNs(uint32_t ns);          // 基于系统时钟的精确延时

/**
  * @brief  初始化AD5621BK
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
  * @note   帧结构：
  *         [15:14] PD1,PD0 电源管理位
  *         [13:02] DAC的12位有效数据
  *         [01:00] 无用数据（会被忽略）
  */
uint8_t DAC7311_SetValue(uint16_t value)
{
    uint16_t data;
    
    // 限制输入范围
    if(value > 4095) {
        value = 4095;
    }
    
    // 构造16位数据帧
    data = (uint16_t)((value << 2) & 0x3FFC);  // 将12位数据左移2位到[13:02]位置
    data |= (AD5621BK_POWER_DOWN_NORMAL << 14); // 设置电源管理位[15:14]为正常工作模式
    // [01:00]位保持为0，这两位会被忽略
    
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
    
    lastValue = value;  // 记录最后设置的值
    
    return 0;
}

/**
  * @brief  进入掉电模式
  * @retval None
  */
void DAC7311_PowerDown(void)
{
    uint16_t data;
    
    // 构造掉电模式数据帧
    data = (lastValue << 2) & 0x3FFC;  // 保持最后的DAC值
    data |= (AD5621BK_POWER_DOWN_1K << 14);  // 设置为1K下拉掉电模式
    
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
    uint16_t data;
    
    // 构造正常工作模式数据帧
    data = (lastValue << 2) & 0x3FFC;  // 恢复最后的DAC值
    data |= (AD5621BK_POWER_DOWN_NORMAL << 14);  // 设置为正常工作模式
    
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
        
        DAC7311_DelayNs(10);   // tDS ≥ 10ns（数据建立时间）
        
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
  * @note   电压计算说明：
  *         - 4mA对应0.588V，需要DAC输出值729
  *         - 20mA对应2.94V，需要DAC输出值3649
  */
uint8_t DAC7311_SetVoltage(float voltage, float vref)
{
    uint16_t dac_value;
    
    /* 参数检查 */
    if (voltage < 0 || voltage > vref) {
        return 1;  // 电压超出范围
    }
    
    /* 计算DAC数值 */
    if (voltage == 0) {
        dac_value = 729;  // 对应4mA输出
    } else if (voltage >= vref) {
        dac_value = 3649;  // 对应20mA输出
    } else {
        // 线性映射到729~3649范围
        dac_value = 729 + (uint16_t)((voltage * (3649 - 729)) / vref);
    }
    
    /* 设置DAC值 */
    return DAC7311_SetValue(dac_value);
}

/**
  * @brief  使用渐变方式设置DAC输出值
  * @param  targetValue: 目标输出值（0-4095）
  * @retval 无
  */
void DAC7311_RampToValue(uint16_t targetValue)
{
    uint16_t currentValue = lastValue;  // 获取当前DAC输出值
    
    // 如果目标值大于当前值，逐渐增加
    if (targetValue > currentValue)
    {
        while (currentValue < targetValue)
        {
            currentValue += DAC7311_RAMP_STEP;  // 按步进值增加
            if (currentValue > targetValue)
            {
                currentValue = targetValue;  // 确保不超过目标值
            }
            DAC7311_SetValue(currentValue);  // 设置新的输出值
            HAL_Delay(DAC7311_RAMP_DELAY);   // 短暂延时
        }
    }
    // 如果目标值小于当前值，逐渐减小
    else if (targetValue < currentValue)
    {
        while (currentValue > targetValue)
        {
            if (currentValue > DAC7311_RAMP_STEP)
            {
                currentValue -= DAC7311_RAMP_STEP;  // 按步进值减小
            }
            else
            {
                currentValue = targetValue;  // 确保不会出现负值
            }
            DAC7311_SetValue(currentValue);  // 设置新的输出值
            HAL_Delay(DAC7311_RAMP_DELAY);   // 短暂延时
        }
    }
}


