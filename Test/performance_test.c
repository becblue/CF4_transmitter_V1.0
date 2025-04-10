/**
  ******************************************************************************
  * @file    performance_test.c
  * @brief   性能测试和优化文件
  ******************************************************************************
  * @attention
  *
  * 本文件包含系统性能测试和优化函数
  * 用于测试系统响应时间、刷新率和资源利用率
  *
  ******************************************************************************
  */

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
#include "sensor.h"
#include "dac7311.h"
#include "oled.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 宏定义 ------------------------------------------------------------------*/
#define NUM_TEST_ITERATIONS   1000  // 测试迭代次数
#define NUM_DAC_LEVELS        16    // DAC测试等级数
#define TEST_UPDATE_INTERVAL  5     // 测试更新间隔(ms)

/* 私有变量 ------------------------------------------------------------------*/
static uint32_t startTime;          // 测试开始时间
static uint32_t endTime;            // 测试结束时间
static uint32_t totalTime;          // 总测试时间
static uint32_t maxTime;            // 最大响应时间
static uint32_t minTime;            // 最小响应时间
static float avgTime;               // 平均响应时间
static char testOutput[100];        // 测试输出缓冲区

/* 函数原型 ------------------------------------------------------------------*/
static void CPU_Usage_Test(void);
static void Response_Time_Test(void);
static void Refresh_Rate_Test(void);
static void Memory_Usage_Test(void);
static void DAC_Performance_Test(void);

/**
  * @brief  执行性能测试
  * @retval 无
  */
void Run_Performance_Tests(void)
{
  // 初始化测试
  printf("\r\n===== 系统性能测试开始 =====\r\n\r\n");
  
  // 显示测试开始信息
  OLED_Clear();
  OLED_ShowString(0, 0, "Performance Test");
  OLED_ShowString(0, 1, "----------------");
  OLED_ShowString(0, 3, "Running tests...");
  OLED_Refresh();
  HAL_Delay(1000);
  
  // 执行各项测试
  Response_Time_Test();
  HAL_Delay(1000);
  
  Refresh_Rate_Test();
  HAL_Delay(1000);
  
  DAC_Performance_Test();
  HAL_Delay(1000);
  
  // 显示测试完成信息
  OLED_Clear();
  OLED_ShowString(0, 0, "Test Complete");
  OLED_ShowString(0, 1, "----------------");
  OLED_ShowString(0, 3, "All tests passed");
  OLED_Refresh();
  
  printf("===== 系统性能测试完成 =====\r\n\r\n");
}

/**
  * @brief  响应时间测试
  * @retval 无
  */
static void Response_Time_Test(void)
{
  uint32_t iterationTime;
  SensorData_t sensorData;
  
  printf("正在执行响应时间测试...\r\n");
  
  // 初始化测试参数
  maxTime = 0;
  minTime = 0xFFFFFFFF;
  totalTime = 0;
  
  // 显示测试信息
  OLED_Clear();
  OLED_ShowString(0, 0, "Response Time Test");
  OLED_ShowString(0, 1, "----------------");
  OLED_Refresh();
  
  // 模拟传感器数据
  sensorData.zeroPoint = 1000;
  sensorData.rangeValue = 5000;
  sensorData.concentration = 2500;
  sensorData.outputValue = 2048;
  sensorData.outputVoltage = 1.65f;
  sensorData.outputCurrent = 12.0f;
  
  // 执行多次测试
  for (uint16_t i = 0; i < NUM_TEST_ITERATIONS; i++) {
    // 更新测试进度
    if (i % 100 == 0) {
      sprintf(testOutput, "Progress: %d%%", i / 10);
      OLED_ShowString(0, 3, testOutput);
      OLED_Refresh();
    }
    
    // 测量单次迭代所需时间
    startTime = HAL_GetTick();
    
    // 模拟完整数据流程处理
    sensorData.concentration = 1000 + (i % 4000);
    sensorData.outputValue = Sensor_CalculateOutput(
      sensorData.concentration,
      sensorData.zeroPoint,
      sensorData.rangeValue
    );
    sensorData.outputVoltage = sensorData.outputValue * 3.3f / 4095;
    sensorData.outputCurrent = 4.0f + (sensorData.outputValue * 16.0f / 4095);
    
    // 设置DAC输出
    DAC7311_SetValue(sensorData.outputValue);
    
    // 测量时间
    endTime = HAL_GetTick();
    iterationTime = endTime - startTime;
    
    // 统计数据
    totalTime += iterationTime;
    if (iterationTime > maxTime) maxTime = iterationTime;
    if (iterationTime < minTime) minTime = iterationTime;
    
    // 使用接近真实工作负载的延迟
    HAL_Delay(TEST_UPDATE_INTERVAL);
  }
  
  // 计算平均响应时间
  avgTime = (float)totalTime / NUM_TEST_ITERATIONS;
  
  // 显示测试结果
  OLED_Clear();
  OLED_ShowString(0, 0, "Response Time Test");
  OLED_ShowString(0, 1, "----------------");
  
  sprintf(testOutput, "Min: %lu ms", minTime);
  OLED_ShowString(0, 2, testOutput);
  
  sprintf(testOutput, "Max: %lu ms", maxTime);
  OLED_ShowString(0, 3, testOutput);
  
  sprintf(testOutput, "Avg: %.2f ms", avgTime);
  OLED_ShowString(0, 4, testOutput);
  
  OLED_ShowString(0, 6, "Test Complete");
  OLED_Refresh();
  
  // 打印测试结果
  printf("响应时间测试结果:\r\n");
  printf("  最小响应时间: %lu ms\r\n", minTime);
  printf("  最大响应时间: %lu ms\r\n", maxTime);
  printf("  平均响应时间: %.2f ms\r\n\r\n", avgTime);
}

/**
  * @brief  刷新率测试
  * @retval 无
  */
static void Refresh_Rate_Test(void)
{
  uint32_t frameCount = 0;
  uint32_t testDuration = 5000; // 测试持续5秒
  float frameRate;
  
  printf("正在执行刷新率测试...\r\n");
  
  // 显示测试信息
  OLED_Clear();
  OLED_ShowString(0, 0, "Refresh Rate Test");
  OLED_ShowString(0, 1, "----------------");
  OLED_ShowString(0, 3, "Running...");
  OLED_Refresh();
  
  // 记录开始时间
  startTime = HAL_GetTick();
  endTime = startTime;
  
  // 连续刷新屏幕，直到测试时间结束
  while (endTime - startTime < testDuration) {
    // 更新屏幕内容
    OLED_Clear();
    
    // 显示一些动态内容
    OLED_ShowString(0, 0, "Refresh Rate Test");
    sprintf(testOutput, "Frame: %lu", frameCount);
    OLED_ShowString(0, 2, testOutput);
    
    // 显示进度条
    uint8_t progress = ((endTime - startTime) * 100) / testDuration;
    OLED_ShowProgress(0, 4, 100, progress);
    
    // 刷新屏幕
    OLED_Refresh();
    
    // 计数帧数
    frameCount++;
    
    // 更新当前时间
    endTime = HAL_GetTick();
  }
  
  // 计算实际测试持续时间（毫秒）
  totalTime = endTime - startTime;
  
  // 计算帧率 (帧/秒)
  frameRate = (float)frameCount * 1000 / totalTime;
  
  // 显示测试结果
  OLED_Clear();
  OLED_ShowString(0, 0, "Refresh Rate Test");
  OLED_ShowString(0, 1, "----------------");
  
  sprintf(testOutput, "Frames: %lu", frameCount);
  OLED_ShowString(0, 2, testOutput);
  
  sprintf(testOutput, "Time: %.1f s", totalTime / 1000.0f);
  OLED_ShowString(0, 3, testOutput);
  
  sprintf(testOutput, "Rate: %.1f fps", frameRate);
  OLED_ShowString(0, 4, testOutput);
  
  OLED_ShowString(0, 6, "Test Complete");
  OLED_Refresh();
  
  // 打印测试结果
  printf("刷新率测试结果:\r\n");
  printf("  总帧数: %lu\r\n", frameCount);
  printf("  测试时间: %.2f 秒\r\n", totalTime / 1000.0f);
  printf("  平均刷新率: %.2f fps\r\n\r\n", frameRate);
}

/**
  * @brief  DAC性能测试
  * @retval 无
  */
static void DAC_Performance_Test(void)
{
  uint32_t singleTransferTime;
  uint32_t totalTransferTime = 0;
  uint16_t dacValues[NUM_DAC_LEVELS];
  
  printf("正在执行DAC性能测试...\r\n");
  
  // 初始化DAC测试值
  for (uint8_t i = 0; i < NUM_DAC_LEVELS; i++) {
    dacValues[i] = (uint16_t)((i * 4095) / (NUM_DAC_LEVELS - 1));
  }
  
  // 显示测试信息
  OLED_Clear();
  OLED_ShowString(0, 0, "DAC Performance");
  OLED_ShowString(0, 1, "----------------");
  OLED_ShowString(0, 3, "Running...");
  OLED_Refresh();
  
  // 测量DAC设置时间
  startTime = HAL_GetTick();
  
  // 执行DAC输出扫描
  for (uint16_t i = 0; i < NUM_TEST_ITERATIONS; i++) {
    // 更新进度
    if (i % 100 == 0) {
      sprintf(testOutput, "Progress: %d%%", i / 10);
      OLED_ShowString(0, 3, testOutput);
      OLED_Refresh();
    }
    
    // 循环设置不同的DAC值
    for (uint8_t j = 0; j < NUM_DAC_LEVELS; j++) {
      uint32_t setValueStartTime = HAL_GetTick();
      DAC7311_SetValue(dacValues[j]);
      uint32_t setValueEndTime = HAL_GetTick();
      
      // 统计单次传输时间
      singleTransferTime = setValueEndTime - setValueStartTime;
      totalTransferTime += singleTransferTime;
      
      // 短暂延时，让DAC输出稳定
      HAL_Delay(1);
    }
  }
  
  // 计算总时间
  endTime = HAL_GetTick();
  totalTime = endTime - startTime;
  
  // 计算平均传输时间
  float avgTransferTime = (float)totalTransferTime / (NUM_TEST_ITERATIONS * NUM_DAC_LEVELS);
  
  // 计算DAC更新率 (每秒更新次数)
  float dacUpdateRate = 1000.0f / avgTransferTime;
  
  // 显示测试结果
  OLED_Clear();
  OLED_ShowString(0, 0, "DAC Performance");
  OLED_ShowString(0, 1, "----------------");
  
  sprintf(testOutput, "Count: %lu", (uint32_t)(NUM_TEST_ITERATIONS * NUM_DAC_LEVELS));
  OLED_ShowString(0, 2, testOutput);
  
  sprintf(testOutput, "Avg: %.2f ms", avgTransferTime);
  OLED_ShowString(0, 3, testOutput);
  
  sprintf(testOutput, "Rate: %.0f Hz", dacUpdateRate);
  OLED_ShowString(0, 4, testOutput);
  
  OLED_ShowString(0, 6, "Test Complete");
  OLED_Refresh();
  
  // 打印测试结果
  printf("DAC性能测试结果:\r\n");
  printf("  设置次数: %lu\r\n", (uint32_t)(NUM_TEST_ITERATIONS * NUM_DAC_LEVELS));
  printf("  平均传输时间: %.2f ms\r\n", avgTransferTime);
  printf("  DAC更新率: %.2f Hz\r\n\r\n", dacUpdateRate);
}