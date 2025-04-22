#ifndef __SENSOR_H
#define __SENSOR_H

#include <stdint.h>  // 添加标准整数类型定义
#include "main.h"    // 包含STM32相关定义
#include <stdio.h>   // 添加标准输入输出定义
#include "led.h"  // 添加LED控制头文件

/* 缓冲区大小定义 */
#define SENSOR_BUFFER_SIZE     32      // 通信缓冲区大小
#define SENSOR_ZERO_POINT      0       // 定义零点常数为0

/* 通讯丢失相关定义 */
#define COMM_LOST_MAX_COUNT     3        // 通讯丢失最大次数（3次，约3秒）
#define COMM_LOST_DAC_VALUE     3649     // 通讯丢失时DAC输出值（对应20mA）

/* 传感器数据结构体定义 */
typedef struct {
    uint16_t concentration;    // 浓度值
    uint32_t rangeValue;      // 量程值（32位）
    uint16_t outputValue;     // 输出值(0-4095)
    float outputVoltage;      // 输出电压(0-3.3V)
    float outputCurrent;      // 输出电流(4-20mA)
} SensorData_t;

/* 函数声明 */
uint8_t Sensor_Init(void);
uint16_t Sensor_GetConcentration(void);
uint32_t Sensor_GetRange(void);  // 修改返回值为32位
uint16_t Sensor_GetZeroPoint(void);
uint16_t Sensor_CalculateOutput(uint16_t concentration, uint32_t rangeValue);  // 移除zeroPoint参数
float Sensor_CalculateVoltage(uint16_t value);
float Sensor_CalculateCurrent(uint16_t value);
uint8_t Sensor_UpdateAllData(SensorData_t *data);
void Sensor_SetOutput(uint16_t value);
uint8_t Sensor_GetLastError(void);
const char* Sensor_GetErrorString(uint8_t errorCode);
uint8_t Sensor_CheckConnection(void);  // 添加通信检查函数声明

/* 外部变量声明 */
extern UART_HandleTypeDef huart3;  // UART3句柄，用于与传感器通信

#endif /* __SENSOR_H */ 


