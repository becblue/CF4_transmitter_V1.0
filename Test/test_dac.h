/**
  ******************************************************************************
  * @file           : test_dac.h
  * @brief          : DAC7311驱动测试模块头文件
  ******************************************************************************
  * @attention
  *
  * 本文件包含DAC7311驱动模块测试相关的函数声明
  *
  ******************************************************************************
  */

/* 防止重复包含 */
#ifndef __TEST_DAC_H
#define __TEST_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 */
#include "main.h"

/* 测试状态定义 */
#define TEST_PASS  0  /* 测试通过 */
#define TEST_FAIL  1  /* 测试失败 */

/* 公共函数声明 */

/**
  * @brief  运行DAC模块所有测试
  * @param  hspi: SPI句柄指针
  * @param  cs_port: 片选端口
  * @param  cs_pin: 片选引脚
  * @retval 测试结果: TEST_PASS测试通过, TEST_FAIL测试失败
  */
uint8_t Run_DAC_Tests(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

/**
  * @brief  获取最后一次测试的错误信息
  * @param  None
  * @retval 错误信息字符串指针
  */
char* Get_DAC_Test_Error(void);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_DAC_H */