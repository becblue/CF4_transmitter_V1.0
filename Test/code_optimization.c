/**
  ******************************************************************************
  * @file    code_optimization.c
  * @brief   代码优化示例和测试文件
  ******************************************************************************
  * @attention
  *
  * 本文件包含针对CF4传感器系统的代码优化示例
  * 测试并验证各种优化策略的效果
  *
  ******************************************************************************
  */

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
#include "sensor.h"
#include "dac7311.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 性能测试变量 -----------------------------------------------------------*/
static uint32_t testStartTime;      // 测试开始时间
static uint32_t testEndTime;        // 测试结束时间
static uint32_t executionTime;      // 执行时间
static uint32_t loopCount = 1000;   // 测试循环次数

/* 测试函数原型 -----------------------------------------------------------*/
static void Test_CalculateOutput_Original(void);      // 原始输出计算函数测试
static void Test_CalculateOutput_Optimized(void);     // 优化后输出计算函数测试

static void Test_OLEDUpdate_Original(void);           // 原始OLED更新函数测试
static void Test_OLEDUpdate_Optimized(void);          // 优化后OLED更新函数测试

static void Test_SensorComm_Original(void);           // 原始传感器通信函数测试
static void Test_SensorComm_Optimized(void);          // 优化后传感器通信函数测试

static void Print_TestResult(const char* testName, uint32_t originalTime, uint32_t optimizedTime);

/**
  * @brief  原始版本的输出值计算函数
  * @param  concentration: 浓度值
  * @param  zeroPoint: 零点值
  * @param  rangeValue: 量程值
  * @retval 输出值(0-4095)
  * @note   此函数为原始版本，用于优化比较
  */
uint16_t CalculateOutput_Original(uint16_t concentration, uint16_t zeroPoint, uint16_t rangeValue)
{
  // 防止除零错误
  if (rangeValue <= zeroPoint) {
    return 0;  // 参数无效，返回0
  }
  
  // 计算输出值 (线性映射浓度值到0-4095范围)
  uint32_t output;  // 使用32位变量避免计算溢出
  
  // 如果浓度小于零点，输出为0
  if (concentration <= zeroPoint) {
    output = 0;  // 浓度小于零点，输出最小值
  }
  // 如果浓度大于量程，输出为最大值
  else if (concentration >= rangeValue) {
    output = 4095;  // 浓度大于量程，输出最大值
  }
  // 否则线性映射
  else {
    // 使用32位计算避免溢出
    output = (uint32_t)(concentration - zeroPoint) * 4095 / (rangeValue - zeroPoint);
    // 确保输出值在有效范围内
    if (output > 4095) {
      output = 4095;  // 限制最大输出值
    }
  }
  
  return (uint16_t)output;  // 返回计算后的输出值
}

/**
  * @brief  优化版本的输出值计算函数
  * @param  concentration: 浓度值
  * @param  zeroPoint: 零点值
  * @param  rangeValue: 量程值
  * @retval 输出值(0-4095)
  * @note   此函数为优化版本，通过减少条件判断提高性能
  */
uint16_t CalculateOutput_Optimized(uint16_t concentration, uint16_t zeroPoint, uint16_t rangeValue)
{
  // 快速路径: 防止除零并进行边界检查
  if (rangeValue <= zeroPoint || concentration <= zeroPoint) {
    return 0;  // 参数无效或浓度小于零点，直接返回0
  }
  
  if (concentration >= rangeValue) {
    return 4095;  // 浓度大于量程，直接返回最大值
  }
  
  // 使用乘法代替除法，并使用移位运算提高性能
  // 4095 约等于 4096 = 2^12，可以使用移位优化
  uint32_t delta = concentration - zeroPoint;
  uint32_t range = rangeValue - zeroPoint;
  
  // 使用更高效的计算方法
  // 先乘以4096(2^12)，然后除以range，再根据需要调整结果
  uint32_t output = (delta << 12) / range;
  
  // 如果使用移位，需要调整最大值
  return (output > 4095) ? 4095 : (uint16_t)output;
}

/**
  * @brief  执行输出计算性能测试
  * @retval 无
  */
void Run_CalculateOutput_Test(void)
{
  uint32_t originalTime, optimizedTime;
  
  // 测试原始版本
  testStartTime = HAL_GetTick();
  Test_CalculateOutput_Original();
  testEndTime = HAL_GetTick();
  originalTime = testEndTime - testStartTime;
  
  // 测试优化版本
  testStartTime = HAL_GetTick();
  Test_CalculateOutput_Optimized();
  testEndTime = HAL_GetTick();
  optimizedTime = testEndTime - testStartTime;
  
  // 打印测试结果
  Print_TestResult("输出值计算函数", originalTime, optimizedTime);
}

/**
  * @brief  测试原始输出计算函数
  * @retval 无
  */
static void Test_CalculateOutput_Original(void)
{
  volatile uint16_t result = 0;
  
  // 使用不同的参数多次调用原始函数
  for (uint32_t i = 0; i < loopCount; i++) {
    // 使用随机值作为参数，确保编译器不会优化掉循环
    uint16_t concentration = rand() % 8000;
    uint16_t zeroPoint = 1000;
    uint16_t rangeValue = 5000;
    
    result = CalculateOutput_Original(concentration, zeroPoint, rangeValue);
  }
}

/**
  * @brief  测试优化输出计算函数
  * @retval 无
  */
static void Test_CalculateOutput_Optimized(void)
{
  volatile uint16_t result = 0;
  
  // 使用相同的参数多次调用优化函数
  for (uint32_t i = 0; i < loopCount; i++) {
    // 使用随机值作为参数，确保编译器不会优化掉循环
    uint16_t concentration = rand() % 8000;
    uint16_t zeroPoint = 1000;
    uint16_t rangeValue = 5000;
    
    result = CalculateOutput_Optimized(concentration, zeroPoint, rangeValue);
  }
}

/**
  * @brief  打印测试结果
  * @param  testName: 测试名称
  * @param  originalTime: 原始版本执行时间
  * @param  optimizedTime: 优化版本执行时间
  * @retval 无
  */
static void Print_TestResult(const char* testName, uint32_t originalTime, uint32_t optimizedTime)
{
  float improvement = 0;
  
  // 避免除零错误
  if (originalTime > 0) {
    improvement = ((float)originalTime - optimizedTime) / originalTime * 100.0f;
  }
  
  // 输出测试结果
  printf("测试项目: %s\r\n", testName);
  printf("原始版本执行时间: %lu ms\r\n", originalTime);
  printf("优化版本执行时间: %lu ms\r\n", optimizedTime);
  printf("性能提升: %.2f%%\r\n\r\n", improvement);
}

/**
  * @brief  初始化性能测试
  * @retval 无
  */
void Performance_Test_Init(void)
{
  // 初始化随机数生成器
  srand(HAL_GetTick());
  
  // 初始化串口输出
  printf("\r\n=== 性能优化测试开始 ===\r\n\r\n");
}

/**
  * @brief  运行所有性能测试
  * @retval 无
  */
void Run_All_Performance_Tests(void)
{
  // 初始化测试
  Performance_Test_Init();
  
  // 运行各项测试
  Run_CalculateOutput_Test();
  
  // 完成测试
  printf("=== 性能优化测试完成 ===\r\n");
}