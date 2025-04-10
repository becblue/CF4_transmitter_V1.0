/**
  ******************************************************************************
  * @file    test_dac.c
  * @brief   DAC7311驱动模块单元测试文件
  ******************************************************************************
  * @attention
  *
  * 本文件提供DAC7311数模转换器的测试函数
  * 用于验证DAC驱动程序的正确性和进行故障诊断
  *
  ******************************************************************************
  */

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
#include "dac7311.h"
#include "test_dac.h"
#include <stdio.h>
#include <string.h>

/* 私有宏定义 ---------------------------------------------------------------*/
/* 测试标志值与头文件定义一致性检查 */
#if TEST_PASS != 0 || TEST_FAIL != 1
#error "测试标志值与头文件定义不一致"
#endif

/* 私有变量 -----------------------------------------------------------------*/
static uint8_t testStatus = TEST_PASS;  // 测试状态
static char testErrorMsg[100] = "无错误";  // 测试错误消息

/* 测试函数原型 -------------------------------------------------------------*/
static uint8_t Test_DACInit(void);
static uint8_t Test_DACSetValue(void);
static uint8_t Test_DACPowerMode(void);

/**
  * @brief  测试DAC初始化函数
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
static uint8_t Test_DACInit(void)
{
  uint8_t result = TEST_PASS;
  
  // 尝试初始化DAC
  if (DAC7311_Init(hspi, cs_port, cs_pin) != HAL_OK) {
    sprintf(testErrorMsg, "DAC初始化失败");
    result = TEST_FAIL;
  } else {
    // 检查是否能成功设置初始值为0
    if (DAC7311_SetValue(0) != HAL_OK) {
      sprintf(testErrorMsg, "DAC初始化后设置值失败");
      result = TEST_FAIL;
    } else {
      sprintf(testErrorMsg, "无错误");
    }
  }
  
  // 输出测试结果
  printf("[DAC初始化测试] %s\r\n", result == TEST_PASS ? "通过" : testErrorMsg);
  return result;
}

/**
  * @brief  测试DAC设置数值函数
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
static uint8_t Test_DACSetValue(void)
{
  uint8_t result = TEST_PASS;
  uint16_t testValues[] = {0, 1024, 2048, 3072, 4095}; // 测试值: 最小、1/4、中间、3/4、最大
  const int testCount = sizeof(testValues) / sizeof(testValues[0]);
  int passCount = 0;
  
  // 确保DAC已初始化
  if (DAC7311_Init(hspi, cs_port, cs_pin) != HAL_OK) {
    sprintf(testErrorMsg, "DAC初始化失败，无法测试设置值");
    return TEST_FAIL;
  }
  
  // 测试不同的DAC值
  for (int i = 0; i < testCount; i++) {
    if (DAC7311_SetValue(testValues[i]) == HAL_OK) {
      passCount++;
      // 在实际应用中可能需要添加模拟读回值的验证
      printf("[DAC设置值测试] 设置值 %u 成功\r\n", testValues[i]);
      
      // 等待一段时间使输出稳定
      HAL_Delay(100);
    } else {
      printf("[DAC设置值测试] 设置值 %u 失败\r\n", testValues[i]);
    }
  }
  
  // 判断测试结果
  if (passCount == testCount) {
    sprintf(testErrorMsg, "无错误");
  } else {
    sprintf(testErrorMsg, "部分测试值设置失败 (成功: %d/%d)", passCount, testCount);
    result = TEST_FAIL;
  }
  
  // 输出测试结果
  printf("[DAC设置值测试] %s\r\n", result == TEST_PASS ? "通过" : testErrorMsg);
  return result;
}

/**
  * @brief  测试DAC电源模式设置函数
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
static uint8_t Test_DACPowerMode(void)
{
  uint8_t result = TEST_PASS;
  uint8_t modes[] = {
    DAC7311_POWER_DOWN_NORMAL,     // 正常模式
    DAC7311_POWER_DOWN_1K,        // 关断模式，1K负载
    DAC7311_POWER_DOWN_100K,      // 关断模式，100K负载
    DAC7311_POWER_DOWN_HI_Z,      // 关断模式，高阻抗
    DAC7311_POWER_DOWN_NORMAL     // 恢复正常模式
  };
  const char* modeNames[] = {
    "正常模式",
    "关断模式 (1K负载)",
    "关断模式 (100K负载)",
    "关断模式 (高阻抗)",
    "恢复正常模式"
  };
  const int testCount = sizeof(modes) / sizeof(modes[0]);
  int passCount = 0;
  
  // 确保DAC已初始化
  if (DAC7311_Init(hspi, cs_port, cs_pin) != HAL_OK) {
    sprintf(testErrorMsg, "DAC初始化失败，无法测试电源模式");
    return TEST_FAIL;
  }
  
  // 测试不同的电源模式
  for (int i = 0; i < testCount; i++) {
    if (DAC7311_SetPowerMode(modes[i]) == HAL_OK) {
      passCount++;
      printf("[DAC电源模式测试] 设置为%s成功\r\n", modeNames[i]);
      
      // 等待模式切换稳定
      HAL_Delay(100);
    } else {
      printf("[DAC电源模式测试] 设置为%s失败\r\n", modeNames[i]);
    }
  }
  
  // 判断测试结果
  if (passCount == testCount) {
    sprintf(testErrorMsg, "无错误");
  } else {
    sprintf(testErrorMsg, "部分电源模式设置失败 (成功: %d/%d)", passCount, testCount);
    result = TEST_FAIL;
  }
  
  // 输出测试结果
  printf("[DAC电源模式测试] %s\r\n", result == TEST_PASS ? "通过" : testErrorMsg);
  return result;
}

/**
  * @brief  运行DAC模块的所有测试
  * @retval 测试结果: TEST_PASS 或 TEST_FAIL
  */
uint8_t Run_DAC_Tests(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
  uint8_t overallResult = TEST_PASS;
  
  printf("\r\n========== DAC7311模块测试开始 ==========\r\n");
  
  // 运行各项测试
  if (Test_DACInit() != TEST_PASS) overallResult = TEST_FAIL;
  HAL_Delay(200);
  
  if (Test_DACSetValue() != TEST_PASS) overallResult = TEST_FAIL;
  HAL_Delay(200);
  
  if (Test_DACPowerMode() != TEST_PASS) overallResult = TEST_FAIL;
  
  // 确保最后恢复到正常工作状态
  DAC7311_Init(hspi, cs_port, cs_pin);
  DAC7311_SetPowerMode(DAC7311_POWER_DOWN_NORMAL);
  
  // 输出总体测试结果
  printf("\r\nDAC7311模块测试结果: %s\r\n", 
         overallResult == TEST_PASS ? "全部通过" : "部分失败");
  printf("========== DAC7311模块测试结束 ==========\r\n\r\n");
  
  return overallResult;
}

/**
  * @brief  获取最后一次测试的错误信息
  * @param  None
  * @retval 错误信息字符串指针
  */
char* Get_DAC_Test_Error(void)
{
  return testErrorMsg;
}