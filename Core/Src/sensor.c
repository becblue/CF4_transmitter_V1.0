/**
  ******************************************************************************
  * @file    sensor.c
  * @brief   CF4传感器通信源文件
  ******************************************************************************
  * @attention
  *
  * 本文件实现了与CF4气体传感器通信的功能
  * 通过USART2与传感器进行通信，获取浓度、量程和零点数据
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

/* 帧格式定义 */
#define SENSOR_TX_HEADER          0xAA    // 发送帧头 0xAA
#define SENSOR_TX_END            0xBB    // 发送帧尾 0xBB
#define SENSOR_RX_HEADER          0x55    // 接收帧头 0x55
#define SENSOR_RX_END            0xAA    // 接收帧尾 0xAA
#define SENSOR_ADDR              0x00    // 传感器默认地址 0x00

/* 命令定义 */
#define SENSOR_CMD_READ_CONC     0x03    // 读取浓度命令
#define SENSOR_CMD_READ_RANGE    0x04    // 读取量程命令
#define SENSOR_CMD_READ_ZERO     0x05    // 读取零点命令

/* 错误代码定义 */
#define SENSOR_ERROR_NONE          0x00    // 无错误
#define SENSOR_ERROR_TIMEOUT       0x01    // 通信超时
#define SENSOR_ERROR_CHECKSUM      0x02    // 校验和错误
#define SENSOR_ERROR_FRAME         0x03    // 帧格式错误
#define SENSOR_ERROR_VALUE         0x04    // 数据值错误
#define SENSOR_ERROR_TRANSMIT      0x05    // 发送错误
#define SENSOR_ERROR_RECEIVE       0x06    // 接收错误
#define SENSOR_ERROR_FORMAT        0x07    // 帧格式错误
#define SENSOR_ERROR_CMD           0x08    // 命令不匹配
#define SENSOR_ERROR_STATUS        0x09    // 状态异常
#define SENSOR_ERROR_EXECUTE       0x0A    // 命令执行错误
#define SENSOR_ERROR_NO_ZERO       0x0B    // 无零点信息
#define SENSOR_ERROR_RESPONSE      0x0C    // 未知响应状态
#define SENSOR_ERROR_NOT_CALIBRATED 0x0D    // 传感器未标定
#define SENSOR_ERROR_ZERO_SHIFTING 0x0E    // 传感器零点平移中
#define SENSOR_ERROR_CALIBRATING   0x0F    // 传感器标定过程中
#define SENSOR_ERROR_FAULT         0x10    // 传感器故障
#define SENSOR_ERROR_UNKNOWN_STATUS 0x11   // 未知传感器状态

/* 私有变量 ------------------------------------------------------------------*/
static uint8_t rxBuffer[SENSOR_BUFFER_SIZE];  // 接收缓冲区
static uint8_t isInitialized = 0;             // 初始化标志
static uint8_t lastError = SENSOR_ERROR_NONE; // 最后一次错误代码
static uint8_t commLostCount = 0;             // 通讯丢失计数器
static uint32_t currentRangeValue = 0;        // 当前量程值

/* 私有函数原型 --------------------------------------------------------------*/
static uint8_t Sensor_SendCommand(uint8_t cmd, uint8_t *data, uint8_t dataLen);
static uint32_t Sensor_ParseResponse(void);
static uint8_t Sensor_CalculateChecksum(uint8_t *data, uint8_t length);
static uint8_t Sensor_IsValueValid(uint32_t value, uint8_t cmd);

/* 函数实现 ------------------------------------------------------------------*/

/**
  * @brief  传感器通信初始化
  * @retval 初始化结果: 0-成功, 1-失败
  */
uint8_t Sensor_Init(void)
{
  // 等待传感器上电稳定
  HAL_Delay(3000);  // 修改为3秒预热时间
  
  // 清空接收缓冲区
  memset(rxBuffer, 0, SENSOR_BUFFER_SIZE);  // 清空接收缓冲区
  
  // 清除错误代码
  lastError = SENSOR_ERROR_NONE;  // 清除上次的错误代码
  commLostCount = 0;             // 清零通讯失败计数
  
  // 获取传感器量程值
  currentRangeValue = Sensor_GetRange();  // 获取量程值
  
  // 检查量程值是否有效
  if (currentRangeValue == 0) {  // 只有0是无效值
    lastError = SENSOR_ERROR_VALUE;  // 记录参数错误
    return 1;  // 初始化失败，量程值无效
  }
  
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
    if (Sensor_SendCommand(SENSOR_CMD_READ_CONC, NULL, 0) == 0) {
      // 解析响应数据
      concentration = Sensor_ParseResponse();
      
      // 检查数据有效性
      if (concentration != 0xFFFF && Sensor_IsValueValid(concentration, SENSOR_CMD_READ_CONC)) {
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
  * @retval 量程值，失败则返回0xFFFFFFFF
  */
uint32_t Sensor_GetRange(void)
{
  uint8_t retry = 0;  // 重试计数器
  uint32_t rangeValue;  // 量程值（32位）
  
  // 使用重试机制读取量程值
  while (retry < SENSOR_MAX_RETRY) {
    // 发送读取量程命令
    if (Sensor_SendCommand(SENSOR_CMD_READ_RANGE, NULL, 0) == 0) {
      // 解析响应数据
      rangeValue = Sensor_ParseResponse();
      
      // 检查数据有效性（只要不是0就是有效的）
      if (Sensor_IsValueValid(rangeValue, SENSOR_CMD_READ_RANGE)) {
        return rangeValue;  // 获取成功，返回量程值
      }
    }
    
    retry++;  // 增加重试计数
    HAL_Delay(SENSOR_RETRY_DELAY);  // 延时一段时间后重试
  }
  
  // 全部重试失败，返回错误值
  return 0xFFFFFFFF;  // 读取失败
}

/**
  * @brief  获取传感器零点值
  * @retval 零点值（固定为0）
  */
uint16_t Sensor_GetZeroPoint(void)
{
  return SENSOR_ZERO_POINT;  // 直接返回零点常量值
}

/**
  * @brief  计算传感器输出值
  * @param  concentration: 当前浓度值
  * @param  rangeValue: 量程值
  * @retval 输出值(729-3649)
  */
uint16_t Sensor_CalculateOutput(uint16_t concentration, uint32_t rangeValue)
{
  // 防止除零错误
  if (rangeValue == 0) {
    return 729;  // 参数无效，返回对应4mA的DAC值
  }
  
  // 计算输出值 (线性映射浓度值到729-3649范围)
  uint32_t output;  // 使用32位变量避免计算溢出
  
  // 如果浓度小于等于零点（0），输出为最小值
  if (concentration <= SENSOR_ZERO_POINT) {
    output = 729;  // 对应4mA的DAC值
  }
  // 如果浓度大于等于量程值，输出为最大值
  else if (concentration >= rangeValue) {
    output = 3649;  // 对应20mA的DAC值
  }
  // 否则进行线性映射
  else {
    // 使用32位计算避免溢出
    // 映射公式：output = 729 + (concentration * (3649 - 729)) / rangeValue
    output = 729 + (concentration * (uint32_t)(3649 - 729)) / rangeValue;
  }
  
  return (uint16_t)output;  // 返回计算结果
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
  * @param  sensorData: 传感器数据结构体指针
  * @retval 0:成功, 非0:失败
  */
uint8_t Sensor_UpdateAllData(SensorData_t* sensorData)
{
    uint8_t result = 0;  // 函数返回值
    uint16_t concentration;  // 浓度值临时变量

    // 检查初始化状态
    if (!isInitialized) {
        lastError = SENSOR_ERROR_VALUE;  // 设置错误代码
        return 1;  // 未初始化，返回错误
    }

    // 获取浓度值
    concentration = Sensor_GetConcentration();  // 读取浓度值
    
    // 修改判断逻辑：只有在浓度值无效时才认为是通讯失败
    if (concentration == 0xFFFF)  // 如果读取失败
    {
        commLostCount++;  // 通讯失败计数加1
        printf("[调试] 通讯失败计数: %d\r\n", commLostCount);  // 添加调试信息
        
        // 如果连续失败次数达到阈值，设置为最大输出报警
        if (commLostCount >= COMM_LOST_MAX_COUNT)
        {
            printf("[调试] 通讯丢失，设置最大输出报警\r\n");  // 添加调试信息
            // 设置DAC输出为最大值
            sensorData->outputValue = COMM_LOST_DAC_VALUE;  // 设置为最大DAC值
            sensorData->outputVoltage = (float)COMM_LOST_DAC_VALUE * 3.3f / 4095.0f;  // 计算对应电压
            sensorData->outputCurrent = 4.0f + (sensorData->outputVoltage * 16.0f / 3.3f);  // 计算对应电流
            
            // 更新DAC输出
            Sensor_SetOutput(COMM_LOST_DAC_VALUE);  // 设置DAC输出
        }
        result = 1;  // 返回错误状态
    }
    else  // 通讯正常
    {
        commLostCount = 0;  // 清零通讯失败计数
        
        // 更新传感器数据
        sensorData->concentration = concentration;  // 更新浓度值
        sensorData->rangeValue = currentRangeValue;  // 使用存储的量程值
        sensorData->outputValue = Sensor_CalculateOutput(concentration, currentRangeValue);  // 计算输出值
        sensorData->outputVoltage = (float)sensorData->outputValue * 3.3f / 4095.0f;  // 计算输出电压
        sensorData->outputCurrent = 4.0f + (sensorData->outputVoltage * 16.0f / 3.3f);  // 计算对应电流
        
        // 更新DAC输出
        Sensor_SetOutput(sensorData->outputValue);  // 设置DAC输出
        
        printf("[调试] 通讯正常，浓度值: %d, DAC输出值: %d\r\n", 
               concentration, sensorData->outputValue);  // 添加调试信息
        
        result = 0;  // 返回成功状态
    }

    return result;  // 返回执行结果
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
  if (DAC7311_SetValue(value) == 0) {  // DAC设置成功
    LED_HandleDAC7311Update();  // 更新LED2状态
  } else {
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
  * @param  data: 数据缓冲区
  * @param  dataLen: 数据长度
  * @retval 发送结果: 0-成功, 1-失败
  */
static uint8_t Sensor_SendCommand(uint8_t cmd, uint8_t *data, uint8_t dataLen)
{
    uint8_t txBuffer[10];  // 发送缓冲区，仅保留发送缓冲区
    
    // 构建发送帧
    txBuffer[0] = 0xAA;        // 帧头
    txBuffer[1] = 0x00;        // 设备地址
    txBuffer[2] = cmd;         // 命令字
    
    // 填充命令内容（5字节）
    for(uint8_t i = 0; i < 5; i++) {
        txBuffer[3+i] = (i < dataLen) ? data[i] : 0x00;
    }
    
    // 计算校验和
    txBuffer[8] = Sensor_CalculateChecksum(txBuffer, 8);  // 使用校验和函数
    txBuffer[9] = 0xBB;        // 帧尾
    
    // 打印发送的数据帧
    printf("\r\n[发送] ");
    for(uint8_t i = 0; i < 10; i++) {
        printf("%02X ", txBuffer[i]);
    }
    printf("\r\n");
    
    // 清空全局接收缓冲区
    memset(rxBuffer, 0, SENSOR_BUFFER_SIZE);  // 在接收前清空全局rxBuffer
    
    // 发送数据
    if(HAL_UART_Transmit(&huart3, txBuffer, 10, 500) != HAL_OK) {  // 使用UART3发送数据，超时时间500ms
        printf("[错误] 发送失败\r\n");
        lastError = SENSOR_ERROR_TRANSMIT;
        return 1;
    }
    
    // 等待接收数据，使用全局rxBuffer
    HAL_StatusTypeDef rcvStatus = HAL_UART_Receive(&huart3, rxBuffer, 10, 1000);  // 使用UART3接收数据，超时时间1000ms
    
    // 打印接收状态和数据
    printf("[接收] ");
    if(rcvStatus == HAL_OK) {
        // 接收成功，打印数据
        for(uint8_t i = 0; i < 10; i++) {
            printf("%02X ", rxBuffer[i]);
        }
        printf("\r\n");
        
        // 验证接收帧格式
        if(rxBuffer[0] != 0x55 || rxBuffer[9] != 0xAA) {  // 检查帧头帧尾
            printf("[错误] 帧格式错误\r\n");
            lastError = SENSOR_ERROR_FORMAT;
            return 1;
        }
        
        if(rxBuffer[1] != cmd) {  // 检查命令字匹配
            printf("[错误] 命令不匹配: 期望%02X, 实际%02X\r\n", cmd, rxBuffer[1]);
            lastError = SENSOR_ERROR_CMD;
            return 1;
        }
        
        // 检查响应字节(Byte2)
        switch(rxBuffer[2]) {
            case 0x01:  // 命令正确且执行
                break;  // 继续处理
            case 0x02:
                printf("[错误] 命令正确但执行错误\r\n");
                lastError = SENSOR_ERROR_EXECUTE;
                return 1;
            case 0x03:
                printf("[错误] 命令错误\r\n");
                lastError = SENSOR_ERROR_CMD;
                return 1;
            case 0x04:
                printf("[错误] 无零点信息\r\n");
                lastError = SENSOR_ERROR_NO_ZERO;
                return 1;
            default:
                printf("[错误] 未知响应状态：0x%02X\r\n", rxBuffer[2]);
                lastError = SENSOR_ERROR_RESPONSE;
                return 1;
        }
        
        // 验证校验和
        if(rxBuffer[8] != Sensor_CalculateChecksum(rxBuffer, 8)) {
            printf("[错误] 校验和错误\r\n");
            lastError = SENSOR_ERROR_CHECKSUM;
            return 1;
        }
        
        // 检查状态字节(Byte7)
        switch(rxBuffer[7]) {
            case 0x00:
                printf("[警告] 传感器未标定\r\n");
                lastError = SENSOR_ERROR_NOT_CALIBRATED;
                return 1;
            case 0x01:
                // 正常工作中，继续处理
                break;
            case 0x02:
                printf("[警告] 传感器零点平移中\r\n");
                lastError = SENSOR_ERROR_ZERO_SHIFTING;
                return 1;
            case 0x03:
                printf("[警告] 传感器标定过程中\r\n");
                lastError = SENSOR_ERROR_CALIBRATING;
                return 1;
            case 0x04:
                printf("[错误] 传感器故障\r\n");
                lastError = SENSOR_ERROR_FAULT;
                return 1;
            default:
                printf("[错误] 未知传感器状态：0x%02X\r\n", rxBuffer[7]);
                lastError = SENSOR_ERROR_UNKNOWN_STATUS;
                return 1;
        }
        
        return 0;  // 通信成功
    }
    else if(rcvStatus == HAL_TIMEOUT) {
        printf("接收超时\r\n");
        lastError = SENSOR_ERROR_TIMEOUT;
    }
    else {
        printf("接收错误: %d\r\n", rcvStatus);
        lastError = SENSOR_ERROR_RECEIVE;
    }
    
    return 1;  // 通信失败
}

/**
  * @brief  解析传感器响应数据
  * @retval 解析结果，失败则返回0xFFFFFFFF（对于32位值）或0xFFFF（对于16位值）
  */
static uint32_t Sensor_ParseResponse(void)
{
  uint32_t value;
  
  // 所有命令都使用32位解析（4字节数据）
  // 低16位在前（Byte3-4），高16位在后（Byte5-6）
  uint32_t lowWord = ((uint32_t)rxBuffer[3] << 8) |  // 低16位高字节
                    ((uint32_t)rxBuffer[4]);          // 低16位低字节
  uint32_t highWord = ((uint32_t)rxBuffer[5] << 8) | // 高16位高字节
                     ((uint32_t)rxBuffer[6]);         // 高16位低字节
  
  // 组合成32位值
  value = (highWord << 16) | lowWord;  // 组合：高16位在高位，低16位在低位
  
  // 打印调试信息
  printf("[调试] 数据解析: 命令=0x%02X, Byte3-6: %02X %02X %02X %02X\r\n", 
         rxBuffer[1], rxBuffer[3], rxBuffer[4], rxBuffer[5], rxBuffer[6]);
  printf("[调试] 低16位: 0x%04X, 高16位: 0x%04X, 组合后: 0x%08X\r\n", 
         (uint16_t)lowWord, (uint16_t)highWord, value);
  
  return value;  // 返回解析出的数据
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
  
  // 累加每个字节
  for (uint8_t i = 0; i < length; i++) {
    sum += data[i];  // 累加每个字节
  }
  
  // 取反加1
  sum = (~sum) + 1;  // 按照协议要求：取反加1
  
  return sum;  // 返回最终的校验和
}

/**
  * @brief  检查数据是否在有效范围内
  * @param  value: 数据值
  * @param  cmd: 命令类型
  * @retval 检查结果: 1-有效, 0-无效
  */
static uint8_t Sensor_IsValueValid(uint32_t value, uint8_t cmd)
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
        printf("[错误] 量程值无效: 等于0\r\n");  // 修改错误提示更明确
        return 0;
      }
      printf("[调试] 当前量程值: 0x%08X (%u)\r\n", value, value);  // 添加调试信息
      return 1;
      
    case SENSOR_CMD_READ_ZERO:
      return 1;  // 接受所有零点值
      
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
      
    case SENSOR_ERROR_TRANSMIT:
      return "发送错误";  // 发送错误
      
    case SENSOR_ERROR_RECEIVE:
      return "接收错误";  // 接收错误
      
    case SENSOR_ERROR_FORMAT:
      return "帧格式错误";  // 帧格式错误
      
    case SENSOR_ERROR_CMD:
      return "命令不匹配";  // 命令不匹配
      
    case SENSOR_ERROR_STATUS:
      return "状态异常";  // 状态异常
      
    case SENSOR_ERROR_EXECUTE:
      return "命令执行错误";  // 命令执行错误
      
    case SENSOR_ERROR_NO_ZERO:
      return "无零点信息";  // 无零点信息
      
    case SENSOR_ERROR_RESPONSE:
      return "未知响应状态";  // 未知响应状态
      
    case SENSOR_ERROR_NOT_CALIBRATED:
      return "传感器未标定";  // 传感器未标定
      
    case SENSOR_ERROR_ZERO_SHIFTING:
      return "传感器零点平移中";  // 传感器零点平移中
      
    case SENSOR_ERROR_CALIBRATING:
      return "传感器标定过程中";  // 传感器标定过程中
      
    case SENSOR_ERROR_FAULT:
      return "传感器故障";  // 传感器故障
      
    case SENSOR_ERROR_UNKNOWN_STATUS:
      return "未知传感器状态";  // 未知传感器状态
      
    default:
      return "未知错误";  // 未知错误代码
  }
}

/**
  * @brief  检查传感器通信状态
  * @retval 0: 通信正常, 1: 通信失败
  */
uint8_t Sensor_CheckConnection(void)
{
    uint8_t retry = 0;  // 重试计数器
    
    // 使用重试机制检查通信
    while (retry < SENSOR_MAX_RETRY) {
        // 发送读取零点命令（用05指令检查通信）
        if (Sensor_SendCommand(SENSOR_CMD_READ_ZERO, NULL, 0) == 0) {
            // 解析响应数据
            uint32_t response = Sensor_ParseResponse();
            
            // 只要能收到正确的响应就表示通信正常
            if (response != 0xFFFFFFFF) {
                lastError = SENSOR_ERROR_NONE;  // 清除错误状态
                return 0;  // 通信正常
            }
        }
        
        retry++;  // 增加重试计数
        HAL_Delay(SENSOR_RETRY_DELAY);  // 延时一段时间后重试
    }
    
    // 全部重试失败
    lastError = SENSOR_ERROR_TIMEOUT;  // 设置超时错误
    return 1;  // 通信失败
} 

