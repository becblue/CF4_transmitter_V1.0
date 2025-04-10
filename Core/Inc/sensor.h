/**
  ******************************************************************************
  * @file    sensor.h
  * @brief   CF4传感器通信头文件
  ******************************************************************************
  * @attention
  *
  * 本文件定义了与CF4气体传感器通信的接口和数据结构
  * 基于单一模组传感器通讯协议实现
  * 通过USART1与传感器进行通信
  *
  ******************************************************************************
  */

/* 防止递归包含 -------------------------------------*/
#ifndef __SENSOR_H__
#define __SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"

/* 宏定义 ------------------------------------------------------------*/
#define SENSOR_CMD_READ_CONC      0x03    // 读取浓度值命令
#define SENSOR_CMD_READ_RANGE     0x04    // 读取量程值命令
#define SENSOR_CMD_READ_ZERO      0x05    // 读取零点值命令

#define SENSOR_FRAME_HEADER       0x42    // 帧头
#define SENSOR_FRAME_END          0x43    // 帧尾

#define SENSOR_BUFFER_SIZE        32      // 传感器通信缓冲区大小
#define SENSOR_TIMEOUT            1000    // 传感器通信超时时间(ms)

/* 错误代码定义 */
#define SENSOR_ERROR_NONE          0x00    // 无错误
#define SENSOR_ERROR_TIMEOUT       0x01    // 通信超时
#define SENSOR_ERROR_CHECKSUM      0x02    // 校验和错误
#define SENSOR_ERROR_FRAME         0x03    // 帧格式错误
#define SENSOR_ERROR_VALUE         0x04    // 数据值错误

/* 数据结构定义 -----------------------------------------------------------*/
/**
  * @brief  传感器参数结构体
  */
typedef struct {
    uint16_t zeroPoint;       // 零点值
    uint16_t rangeValue;      // 量程值
    uint16_t concentration;   // 当前浓度值
    uint16_t outputValue;     // 输出值(0-4095)
    float outputVoltage;      // 输出电压(0-3.3V)
    float outputCurrent;      // 输出电流(4-20mA)
} SensorData_t;

/* 函数声明 --------------------------------------------------------------*/
/**
  * @brief  传感器通信初始化
  * @retval 初始化结果: 0-成功, 1-失败
  */
uint8_t Sensor_Init(void);

/**
  * @brief  获取传感器浓度值
  * @retval 浓度值
  */
uint16_t Sensor_GetConcentration(void);

/**
  * @brief  获取传感器量程值
  * @retval 量程值
  */
uint16_t Sensor_GetRange(void);

/**
  * @brief  获取传感器零点值
  * @retval 零点值
  */
uint16_t Sensor_GetZeroPoint(void);

/**
  * @brief  计算传感器输出值
  * @param  concentration: 当前浓度值
  * @param  zeroPoint: 零点值
  * @param  rangeValue: 量程值
  * @retval 输出值(0-4095)
  */
uint16_t Sensor_CalculateOutput(uint16_t concentration, uint16_t zeroPoint, uint16_t rangeValue);

/**
  * @brief  计算输出电压
  * @param  value: 输出值(0-4095)
  * @retval 输出电压(0-3.3V)
  */
float Sensor_CalculateVoltage(uint16_t value);

/**
  * @brief  计算输出电流
  * @param  value: 输出值(0-4095)
  * @retval 输出电流(4-20mA)
  */
float Sensor_CalculateCurrent(uint16_t value);

/**
  * @brief  更新所有传感器数据
  * @param  data: 传感器数据结构体指针
  * @retval 更新结果: 0-成功, 1-失败
  */
uint8_t Sensor_UpdateAllData(SensorData_t *data);

/**
  * @brief  设置输出值
  * @param  value: 输出值(0-4095)
  * @retval 无
  */
void Sensor_SetOutput(uint16_t value);

/**
  * @brief  获取最后一次错误代码
  * @retval 错误代码
  */
uint8_t Sensor_GetLastError(void);

/**
  * @brief  获取错误代码对应的描述字符串
  * @param  errorCode: 错误代码
  * @retval 错误描述字符串
  */
const char* Sensor_GetErrorString(uint8_t errorCode);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_H__ */ 