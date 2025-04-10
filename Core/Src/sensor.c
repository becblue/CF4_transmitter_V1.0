/**
  ******************************************************************************
  * @file    sensor.c
  * @brief   CF4传感器通信源文件
  ******************************************************************************
  * @attention
  *
  * 本文件实现了与CF4气体传感器通信的功能
  * 通过USART1与传感器进行通信，获取浓度、量程和零点数据
  * 并根据这些数据计算输出值、电压和电流
  *
  ******************************************************************************
  */

/* 包含文件 ------------------------------------------------------------------*/
#include "sensor.h"
#include "dac7311.h"
#include "oled.h"
#include <string.h>

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define SENSOR_MAX_RETRY           3       // 最大重试次数
#define SENSOR_RETRY_DELAY         50      // 重试延时(ms)

/* 错误代码定义 */
#define SENSOR_ERROR_NONE          0x00    // 无错误
#define SENSOR_ERROR_TIMEOUT       0x01    // 通信超时
#define SENSOR_ERROR_CHECKSUM      0x02    // 校验和错误
#define SENSOR_ERROR_FRAME         0x03    // 帧格式错误
#define SENSOR_ERROR_VALUE         0x04    // 数据值错误

/* 私有变量 ------------------------------------------------------------------*/
static uint8_t rxBuffer[SENSOR_BUFFER_SIZE];  // 接收缓冲区
static SensorData_t sensorData = {0};         // 传感器数据结构体
static uint8_t isInitialized = 0;             // 初始化标志
static uint8_t lastError = SENSOR_ERROR_NONE; // 最后一次错误代码

/* 私有函数原型 --------------------------------------------------------------*/
static uint8_t Sensor_SendCommand(uint8_t cmd);
static uint16_t Sensor_ParseResponse(void);
static uint8_t Sensor_VerifyChecksum(uint8_t *data, uint8_t length);
static uint8_t Sensor_CalculateChecksum(uint8_t *data, uint8_t length);
static uint8_t Sensor_IsValueValid(uint16_t value, uint8_t cmd);

/* 函数实现 ------------------------------------------------------------------*/

/**
  * @brief  传感器通信初始化
  * @retval 初始化结果: 0-成功, 1-失败
  */
uint8_t Sensor_Init(void)
{
  // 延时一段时间等待传感器上电稳定
  HAL_Delay(1000);  // 等待传感器上电稳定
  
  // 清空接收缓冲区
  memset(rxBuffer, 0, SENSOR_BUFFER_SIZE);  // 清空接收缓冲区
  
  // 清除错误代码
  lastError = SENSOR_ERROR_NONE;  // 清除上次的错误代码
  
  // 尝试获取传感器零点值和量程值
  uint16_t zeroPoint = Sensor_GetZeroPoint();  // 获取零点值
  HAL_Delay(100);  // 短暂延时，避免连续通信
  uint16_t rangeValue = Sensor_GetRange();  // 获取量程值
  
  // 检查获取值是否有效
  if (zeroPoint == 0xFFFF || rangeValue == 0xFFFF) {
    return 1;  // 初始化失败，无法获取基本参数
  }
  
  // 输出初始值检查
  if (rangeValue <= zeroPoint) {
    // 量程值必须大于零点值
    lastError = SENSOR_ERROR_VALUE;  // 记录参数错误
    return 1;  // 参数异常
  }
  
  // 更新传感器数据结构体
  sensorData.zeroPoint = zeroPoint;  // 保存零点值
  sensorData.rangeValue = rangeValue;  // 保存量程值
  sensorData.concentration = 0;  // 初始浓度值为0
  sensorData.outputValue = 0;  // 初始输出值为0
  sensorData.outputVoltage = 0.0f;  // 初始电压为0V
  sensorData.outputCurrent = 4.0f;  // 初始电流为4mA(最小输出)
  
  // 设置初始化标志
  isInitialized = 1;  // 标记初始化完成
  
  return 0;  // 初始化成功
}

/**
  * @brief  获取传感器浓度值
  * @retval 浓度值，失败则返回0xFFFF
  */
uint16_t Sensor_GetConcentration(void)
{
  uint8_t retry = 0;  // 重试计数器
  uint16_t concentration;  // 浓度值
  
  // 使用重试机制读取浓度值
  while (retry < SENSOR_MAX_RETRY) {
    // 发送读取浓度命令
    if (Sensor_SendCommand(SENSOR_CMD_READ_CONC) == 0) {
      // 解析响应数据
      concentration = Sensor_ParseResponse();
      
      // 检查数据有效性
      if (concentration != 0xFFFF && Sensor_IsValueValid(concentration, SENSOR_CMD_READ_CONC)) {
        // 更新传感器数据结构体
        sensorData.concentration = concentration;
        lastError = SENSOR_ERROR_NONE;  // 清除错误状态
        return concentration;  // 获取成功，返回浓度值
      }
    }
    
    retry++;  // 增加重试计数
    HAL_Delay(SENSOR_RETRY_DELAY);  // 延时一段时间后重试
  }
  
  // 全部重试失败，返回错误值
  return 0xFFFF;  // 读取失败
}

/**
  * @brief  获取传感器量程值
  * @retval 量程值，失败则返回0xFFFF
  */
uint16_t Sensor_GetRange(void)
{
  uint8_t retry = 0;  // 重试计数器
  uint16_t rangeValue;  // 量程值
  
  // 使用重试机制读取量程值
  while (retry < SENSOR_MAX_RETRY) {
    // 发送读取量程命令
    if (Sensor_SendCommand(SENSOR_CMD_READ_RANGE) == 0) {
      // 解析响应数据
      rangeValue = Sensor_ParseResponse();
      
      // 检查数据有效性
      if (rangeValue != 0xFFFF && Sensor_IsValueValid(rangeValue, SENSOR_CMD_READ_RANGE)) {
        // 更新传感器数据结构体
        sensorData.rangeValue = rangeValue;
        lastError = SENSOR_ERROR_NONE;  // 清除错误状态
        return rangeValue;  // 获取成功，返回量程值
      }
    }
    
    retry++;  // 增加重试计数
    HAL_Delay(SENSOR_RETRY_DELAY);  // 延时一段时间后重试
  }
  
  // 全部重试失败，返回错误值
  return 0xFFFF;  // 读取失败
}

/**
  * @brief  获取传感器零点值
  * @retval 零点值，失败则返回0xFFFF
  */
uint16_t Sensor_GetZeroPoint(void)
{
  uint8_t retry = 0;  // 重试计数器
  uint16_t zeroPoint;  // 零点值
  
  // 使用重试机制读取零点值
  while (retry < SENSOR_MAX_RETRY) {
    // 发送读取零点命令
    if (Sensor_SendCommand(SENSOR_CMD_READ_ZERO) == 0) {
      // 解析响应数据
      zeroPoint = Sensor_ParseResponse();
      
      // 检查数据有效性
      if (zeroPoint != 0xFFFF && Sensor_IsValueValid(zeroPoint, SENSOR_CMD_READ_ZERO)) {
        // 更新传感器数据结构体
        sensorData.zeroPoint = zeroPoint;
        lastError = SENSOR_ERROR_NONE;  // 清除错误状态
        return zeroPoint;  // 获取成功，返回零点值
      }
    }
    
    retry++;  // 增加重试计数
    HAL_Delay(SENSOR_RETRY_DELAY);  // 延时一段时间后重试
  }
  
  // 全部重试失败，返回错误值
  return 0xFFFF;  // 读取失败
}

/**
  * @brief  计算传感器输出值
  * @param  concentration: 当前浓度值
  * @param  zeroPoint: 零点值
  * @param  rangeValue: 量程值
  * @retval 输出值(0-4095)
  */
uint16_t Sensor_CalculateOutput(uint16_t concentration, uint16_t zeroPoint, uint16_t rangeValue)
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
  * @brief  计算输出电压
  * @param  value: 输出值(0-4095)
  * @retval 输出电压(0-3.3V)
  */
float Sensor_CalculateVoltage(uint16_t value)
{
  // 线性映射输出值到0-3.3V电压范围
  // 限制输入范围
  if (value > 4095) {
    value = 4095;  // 限制最大输入值
  }
  
  // 计算对应电压值
  return (float)value * 3.3f / 4095.0f;  // 线性映射到0-3.3V范围
}

/**
  * @brief  计算输出电流
  * @param  value: 输出值(0-4095)
  * @retval 输出电流(4-20mA)
  */
float Sensor_CalculateCurrent(uint16_t value)
{
  // 线性映射输出值到4-20mA电流范围
  // 限制输入范围
  if (value > 4095) {
    value = 4095;  // 限制最大输入值
  }
  
  // 计算对应电流值
  return 4.0f + (float)value * 16.0f / 4095.0f;  // 4mA + 线性映射范围(0-16mA)
}

/**
  * @brief  更新所有传感器数据
  * @param  data: 传感器数据结构体指针
  * @retval 更新结果: 0-成功, 1-失败
  */
uint8_t Sensor_UpdateAllData(SensorData_t *data)
{
  if (!isInitialized) {
    lastError = SENSOR_ERROR_VALUE;  // 设置错误代码
    return 1;  // 未初始化，返回错误
  }
  
  // 获取传感器浓度值
  uint16_t concentration = Sensor_GetConcentration();
  if (concentration == 0xFFFF) {
    return 1;  // 获取浓度失败
  }
  
  // 计算输出值
  uint16_t outputValue = Sensor_CalculateOutput(
    concentration,             // 当前浓度值
    sensorData.zeroPoint,      // 使用保存的零点值
    sensorData.rangeValue      // 使用保存的量程值
  );
  
  // 计算输出电压和电流
  float outputVoltage = Sensor_CalculateVoltage(outputValue);  // 计算对应电压
  float outputCurrent = Sensor_CalculateCurrent(outputValue);  // 计算对应电流
  
  // 更新传感器数据结构体
  sensorData.concentration = concentration;  // 更新浓度值
  sensorData.outputValue = outputValue;      // 更新输出值
  sensorData.outputVoltage = outputVoltage;  // 更新电压值
  sensorData.outputCurrent = outputCurrent;  // 更新电流值
  
  // 如果提供了外部结构体指针，则复制数据
  if (data != NULL) {
    memcpy(data, &sensorData, sizeof(SensorData_t));  // 复制数据到外部结构体
  }
  
  // 设置DAC输出
  Sensor_SetOutput(outputValue);  // 更新DAC输出
  
  return 0;  // 更新成功
}

/**
  * @brief  设置输出值
  * @param  value: 输出值(0-4095)
  * @retval 无
  */
void Sensor_SetOutput(uint16_t value)
{
  // 限制输入范围
  if (value > 4095) {
    value = 4095;  // 限制最大输入值
  }
  
  // 使用DAC7311设置输出电压
  if (DAC7311_SetValue(value) != 0) {
    // DAC设置失败时记录错误
    lastError = SENSOR_ERROR_VALUE;  // 记录DAC设置错误
  }
}

/**
  * @brief  获取最后一次错误代码
  * @retval 错误代码
  */
uint8_t Sensor_GetLastError(void)
{
  return lastError;  // 返回最后一次错误代码
}

/**
  * @brief  发送命令到传感器
  * @param  cmd: 命令字节
  * @retval 发送结果: 0-成功, 1-失败
  */
static uint8_t Sensor_SendCommand(uint8_t cmd)
{
  uint8_t txBuffer[5];  // 命令帧缓冲区
  
  // 构建命令帧
  txBuffer[0] = SENSOR_FRAME_HEADER;  // 帧头
  txBuffer[1] = cmd;                  // 命令字
  txBuffer[2] = 0x00;                 // 数据字节 (无数据)
  txBuffer[3] = Sensor_CalculateChecksum(txBuffer, 3);  // 校验和
  txBuffer[4] = SENSOR_FRAME_END;     // 帧尾
  
  // 清空接收缓冲区
  memset(rxBuffer, 0, SENSOR_BUFFER_SIZE);  // 清除上次接收的数据
  
  // 发送命令帧
  if (HAL_UART_Transmit(&huart1, txBuffer, 5, SENSOR_TIMEOUT) != HAL_OK) {
    lastError = SENSOR_ERROR_TIMEOUT;  // 设置超时错误
    return 1;  // 发送失败
  }
  
  // 接收响应数据
  if (HAL_UART_Receive(&huart1, rxBuffer, 7, SENSOR_TIMEOUT) != HAL_OK) {
    lastError = SENSOR_ERROR_TIMEOUT;  // 设置超时错误
    return 1;  // 接收失败
  }
  
  return 0;  // 发送成功
}

/**
  * @brief  解析传感器响应数据
  * @retval 解析结果，失败则返回0xFFFF
  */
static uint16_t Sensor_ParseResponse(void)
{
  // 检查帧头和帧尾
  if (rxBuffer[0] != SENSOR_FRAME_HEADER || rxBuffer[6] != SENSOR_FRAME_END) {
    lastError = SENSOR_ERROR_FRAME;  // 设置帧格式错误
    return 0xFFFF;  // 帧格式错误
  }
  
  // 验证校验和
  if (!Sensor_VerifyChecksum(rxBuffer, 5)) {
    lastError = SENSOR_ERROR_CHECKSUM;  // 设置校验和错误
    return 0xFFFF;  // 校验和错误
  }
  
  // 提取数据
  uint16_t value = (rxBuffer[3] << 8) | rxBuffer[4];  // 组合高低字节为16位数据
  
  return value;  // 返回解析出的数据
}

/**
  * @brief  验证校验和
  * @param  data: 数据缓冲区
  * @param  length: 需要校验的长度(不包括校验和本身)
  * @retval 校验结果: 1-通过, 0-失败
  */
static uint8_t Sensor_VerifyChecksum(uint8_t *data, uint8_t length)
{
  uint8_t calcChecksum = Sensor_CalculateChecksum(data, length);  // 计算校验和
  return (calcChecksum == data[length]);  // 比较计算值与接收值
}

/**
  * @brief  计算校验和
  * @param  data: 数据缓冲区
  * @param  length: 需要校验的长度
  * @retval 校验和
  */
static uint8_t Sensor_CalculateChecksum(uint8_t *data, uint8_t length)
{
  uint8_t sum = 0;  // 校验和初始值
  for (uint8_t i = 0; i < length; i++) {
    sum += data[i];  // 累加每个字节
  }
  return sum;  // 返回累加和作为校验和
}

/**
  * @brief  检查数据是否在有效范围内
  * @param  value: 数据值
  * @param  cmd: 命令类型
  * @retval 检查结果: 1-有效, 0-无效
  */
static uint8_t Sensor_IsValueValid(uint16_t value, uint8_t cmd)
{
  // 根据不同命令类型检查数据有效性
  switch (cmd) {
    case SENSOR_CMD_READ_CONC:
      // 浓度值范围检查 (可根据实际情况调整)
      return 1;  // 默认接受所有浓度值
      
    case SENSOR_CMD_READ_RANGE:
      // 量程值必须大于0
      if (value == 0) {
        lastError = SENSOR_ERROR_VALUE;
        return 0;
      }
      return 1;
      
    case SENSOR_CMD_READ_ZERO:
      // 零点值范围检查
      return 1;  // 默认接受所有零点值
      
    default:
      return 0;  // 未知命令类型
  }
}

/**
  * @brief  获取错误代码对应的描述字符串
  * @param  errorCode: 错误代码
  * @retval 错误描述字符串
  */
const char* Sensor_GetErrorString(uint8_t errorCode)
{
  // 根据错误代码返回对应的错误描述
  switch (errorCode) {
    case SENSOR_ERROR_NONE:
      return "无错误";  // 无错误
      
    case SENSOR_ERROR_TIMEOUT:
      return "通信超时";  // 通信超时
      
    case SENSOR_ERROR_CHECKSUM:
      return "校验和错误";  // 校验和错误
      
    case SENSOR_ERROR_FRAME:
      return "帧格式错误";  // 帧格式错误
      
    case SENSOR_ERROR_VALUE:
      return "数据值无效";  // 数据值无效
      
    default:
      return "未知错误";  // 未知错误代码
  }
} 

