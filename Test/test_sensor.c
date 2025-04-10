/**
  ******************************************************************************
  * @file    test_sensor.c
  * @brief   传感器模块单元测试文件
  ******************************************************************************
  * @attention
  *
  * 本文件提供CF4传感器模块的测试函数
  * 用于单元测试和进行故障分析
  *
  ******************************************************************************
  */

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
#include "sensor.h"
#include "dac7311.h"
#include <stdio.h>
#include <string.h>

/* 私有宏定义 ---------------------------------------------------------------*/
#define TEST_PASS        1  // 测试通过标志
#define TEST_FAIL        0  // 测试失败标志

/* 私有变量 -----------------------------------------------------------------*/
static uint8_t testStatus = TEST_PASS;  // 测试状态
static char testMessage[100];           // 测试消息

/* 测试函数原型 -------------------------------------------------------------*/
static uint8_t Test_SensorInit(void);
static uint8_t Test_SensorRead(void);
static uint8_t Test_SensorCalibration(void);
static uint8_t Test_OutputCalculation(void);

/**
  * @brief  测试传感器初始化函数
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
static uint8_t Test_SensorInit(void)
{
  uint8_t result = TEST_PASS;
  
  // 尝试初始化传感器
  if (Sensor_Init() != HAL_OK) {
    sprintf(testMessage, "传感器初始化失败");
    result = TEST_FAIL;
  } else {
    sprintf(testMessage, "传感器初始化成功");
  }
  
  // 输出测试结果
  printf("[传感器初始化测试] %s\r\n", testMessage);
  return result;
}

/**
  * @brief  测试传感器读取函数
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
static uint8_t Test_SensorRead(void)
{
  uint8_t result = TEST_PASS;
  uint16_t sensorValue;
  
  // 读取传感器值
  if (Sensor_GetValue(&sensorValue) != HAL_OK) {
    sprintf(testMessage, "传感器读取失败");
    result = TEST_FAIL;
  } else if (sensorValue < 100 || sensorValue > 40000) {
    // 判断传感器值是否在合理范围内（根据实际情况调整范围）
    sprintf(testMessage, "传感器值超出合理范围: %u", sensorValue);
    result = TEST_FAIL;
  } else {
    sprintf(testMessage, "传感器读取成功，值: %u", sensorValue);
  }
  
  // 输出测试结果
  printf("[传感器读取测试] %s\r\n", testMessage);
  return result;
}

/**
  * @brief  测试传感器校准函数
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
static uint8_t Test_SensorCalibration(void)
{
  uint8_t result = TEST_PASS;
  uint16_t beforeCal, afterCal;
  
  // 读取校准前的传感器值
  if (Sensor_GetValue(&beforeCal) != HAL_OK) {
    sprintf(testMessage, "校准前传感器读取失败");
    return TEST_FAIL;
  }
  
  // 执行校准过程
  if (Sensor_Calibration() != HAL_OK) {
    sprintf(testMessage, "传感器校准失败");
    result = TEST_FAIL;
  } else {
    // 读取校准后的传感器值
    HAL_Delay(500); // 延时等待稳定
    
    if (Sensor_GetValue(&afterCal) != HAL_OK) {
      sprintf(testMessage, "校准后传感器读取失败");
      result = TEST_FAIL;
    } else {
      sprintf(testMessage, "传感器校准成功，校准前: %u, 校准后: %u", beforeCal, afterCal);
    }
  }
  
  // 输出测试结果
  printf("[传感器校准测试] %s\r\n", testMessage);
  return result;
}

/**
  * @brief  测试输出值计算函数
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
static uint8_t Test_OutputCalculation(void)
{
  uint8_t result = TEST_PASS;
  
  // 测试用例数组 {浓度, 零点, 量程, 预期输出}
  const struct {
    uint16_t concentration;
    uint16_t zeroPoint;
    uint16_t rangeValue;
    uint16_t expectedOutput;
  } testCases[] = {
    {1000, 1000, 5000, 0},      // 浓度等于零点，预期输出为0
    {5000, 1000, 5000, 4095},   // 浓度等于量程，预期输出为4095
    {3000, 1000, 5000, 2047},   // 浓度在中间，预期输出约为一半
    {500, 1000, 5000, 0},       // 浓度小于零点，预期输出为0
    {6000, 1000, 5000, 4095},   // 浓度大于量程，预期输出为4095
  };
  
  const int testCount = sizeof(testCases) / sizeof(testCases[0]);
  int passCount = 0;
  
  // 执行所有测试用例
  for (int i = 0; i < testCount; i++) {
    uint16_t output = Sensor_CalculateOutput(
      testCases[i].concentration,
      testCases[i].zeroPoint,
      testCases[i].rangeValue
    );
    
    // 允许有±1的误差（可能由于舍入导致）
    if (abs(output - testCases[i].expectedOutput) <= 1) {
      passCount++;
    } else {
      printf("[输出计算测试] 测试用例 #%d 失败: 浓度=%u, 零点=%u, 量程=%u\r\n", 
             i+1, testCases[i].concentration, testCases[i].zeroPoint, testCases[i].rangeValue);
      printf("              预期输出=%u, 实际输出=%u\r\n", 
             testCases[i].expectedOutput, output);
    }
  }
  
  // 判断测试结果
  if (passCount == testCount) {
    sprintf(testMessage, "所有测试用例通过 (%d/%d)", passCount, testCount);
  } else {
    sprintf(testMessage, "部分测试用例失败 (通过: %d/%d)", passCount, testCount);
    result = TEST_FAIL;
  }
  
  // 输出测试结果
  printf("[输出计算测试] %s\r\n", testMessage);
  return result;
}

/**
  * @brief  运行传感器模块的所有测试
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
uint8_t Run_Sensor_Tests(void)
{
  uint8_t overallResult = TEST_PASS;
  
  printf("\r\n========== 传感器模块测试开始 ==========\r\n");
  
  // 运行各项测试
  if (Test_SensorInit() != TEST_PASS) overallResult = TEST_FAIL;
  HAL_Delay(200);
  
  if (Test_SensorRead() != TEST_PASS) overallResult = TEST_FAIL;
  HAL_Delay(200);
  
  if (Test_SensorCalibration() != TEST_PASS) overallResult = TEST_FAIL;
  HAL_Delay(200);
  
  if (Test_OutputCalculation() != TEST_PASS) overallResult = TEST_FAIL;
  
  // 输出总体测试结果
  printf("\r\n传感器模块测试结果: %s\r\n", 
         overallResult == TEST_PASS ? "全部通过" : "部分失败");
  printf("========== 传感器模块测试结束 ==========\r\n\r\n");
  
  return overallResult;
}