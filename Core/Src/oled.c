/**
  ******************************************************************************
  * @file    oled.c
  * @brief   OLED显示驱动源文件
  ******************************************************************************
  * @attention
  *
  * 本文件实现了OLED显示屏的驱动功能
  * 使用I2C1通信，支持脏区更新机制，优化刷新效率
  * SSD1306驱动控制器，分辨率128x64
  *
  ******************************************************************************
  */

/* 包含文件 ------------------------------------------------------------------*/
#include "oled.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>  // 添加数学库头文件，提供pow函数
#include "oledfont.h"  // 字体库定义

/* 字体数据 ------------------------------------------------------------------*/
// 8x16字体数据，ASCII码字符集（从空格(0x20)开始）
extern const unsigned char F8X16[];

// 6x8字体数据 
extern const unsigned char F6x8[][6];

/* OLED初始化命令数组 */
static const uint8_t OLED_Init_CMD[] = {
    0xAE,   // 关闭显示
    0xD5,   // 设置显示时钟分频比/振荡器频率
    0x80,   // 设置分频比，设置时钟为100帧/秒
    0xA8,   // 设置多路复用率
    0x3F,   // 1/64 duty
    0xD3,   // 设置显示偏移
    0x00,   // 不偏移
    0x40,   // 设置显示开始行 [5:0]
    0x8D,   // 设置充电泵
    0x14,   // 使能充电泵
    0x20,   // 设置内存寻址模式
    0x02,   // 页寻址模式
    0xA1,   // 段重定向设置，0xA1正常（从左到右），0xA0左右反置
    0xC8,   // 行扫描顺序，0xC8正常（从上到下），0xC0上下反置
    0xDA,   // 设置COM引脚硬件配置
    0x12,   // 启用备用COM引脚配置
    0x81,   // 设置对比度
    0xCF,   // 对比度值
    0xD9,   // 设置预充电周期
    0xF1,   // 设置预充电
    0xDB,   // 设置VCOMH取消选择级别
    0x30,   // 约0.83xVcc
    0xA4,   // 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
    0xA6,   // 设置显示方式;bit0:1,反相显示;0,正常显示
    0xAF    // 开启显示
};

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/

/* 私有变量 ------------------------------------------------------------------*/
static OLED_DirtyFlag_t oledDirtyFlag;  // 脏区标记
static I2C_HandleTypeDef *oled_hi2c;    // I2C句柄

/* 私有函数原型 --------------------------------------------------------------*/
static uint8_t OLED_WriteCmd(uint8_t command);
static uint8_t OLED_WriteData(uint8_t data);
static void OLED_SetPosition(uint8_t x, uint8_t y);
static void OLED_MarkDirty(uint8_t page);
static void OLED_DrawChar(uint8_t x, uint8_t y, char chr);

/* 函数实现 ------------------------------------------------------------------*/

/**
  * @brief  OLED初始化
  * @param  hi2c: I2C句柄指针
  * @retval 初始化结果: 0-成功, 1-失败
  */
uint8_t OLED_Init(I2C_HandleTypeDef *hi2c)
{
    oled_hi2c = hi2c;  // 保存I2C句柄
    
    // 初始化显示缓冲区和脏区标记
    memset(&oledDirtyFlag, 0, sizeof(OLED_DirtyFlag_t));
    for(uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
        oledDirtyFlag.isDirty[i] = 1;  // 初始时标记所有页面为脏区
    }
    
    HAL_Delay(100);    // 等待OLED上电稳定
    
    /* 发送初始化命令序列 */
    for(uint8_t i = 0; i < sizeof(OLED_Init_CMD); i++)
    {
        if (OLED_WriteCmd(OLED_Init_CMD[i]) != 0) {  // 如果写入命令失败
            return 1;  // 返回错误
        }
        HAL_Delay(1);  // 每条命令后添加短暂延时
    }
    
    /* 清屏并立即刷新显示 */
    OLED_Clear();
    OLED_Refresh();  // 立即刷新显示
    
    return 0;  // 初始化成功
}

/**
  * @brief  OLED清屏
  * @retval 无
  */
void OLED_Clear(void)
{
    // 清空显示缓冲区
    memset(oledDirtyFlag.displayBuffer, 0, sizeof(oledDirtyFlag.displayBuffer));
    
    // 标记所有页为脏区
    for(uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
        oledDirtyFlag.isDirty[i] = 1;
    }
}

/**
  * @brief  OLED刷新显示(仅更新脏区)
  * @retval 无
  */
void OLED_Refresh(void)
{
    uint8_t i;
    
    for(i = 0; i < DIRTY_BLOCK_NUM; i++) {
        if(oledDirtyFlag.isDirty[i]) {
            OLED_SetPosition(0, i);  // 设置位置到页的开始
            
            // 发送该页的所有数据
            for(uint8_t j = 0; j < DIRTY_BLOCK_SIZE; j++) {
                OLED_WriteData(oledDirtyFlag.displayBuffer[i][j]);
            }
            
            oledDirtyFlag.isDirty[i] = 0;  // 清除脏标记
        }
    }
}

/**
  * @brief  开启OLED显示
  * @param  None
  * @retval None
  */
void OLED_Display_On(void)
{
    OLED_WriteCmd(OLED_CMD_DISPLAY_ON);
}

/**
  * @brief  关闭OLED显示
  * @param  None
  * @retval None
  */
void OLED_Display_Off(void)
{
    OLED_WriteCmd(OLED_CMD_DISPLAY_OFF);
}

/**
  * @brief  设置OLED显示位置
  * @param  x: 列坐标，范围0~127
  * @param  y: 页坐标，范围0~7
  * @retval None
  */
static void OLED_SetPosition(uint8_t x, uint8_t y)
{
    OLED_WriteCmd(0xB0 + y);                   // 设置页地址
    OLED_WriteCmd(0x00 + (x & 0x0F));         // 设置列地址低4位
    OLED_WriteCmd(0x10 + ((x >> 4) & 0x0F));  // 设置列地址高4位
}

/**
  * @brief  OLED显示一个字符
  * @param  x: 列坐标，范围0~127
  * @param  y: 页坐标，范围0~7
  * @param  chr: 要显示的字符
  * @retval None
  */
void OLED_ShowChar(uint8_t x, uint8_t y, char chr)
{
    uint8_t c = 0;
    c = chr - ' ';    // 得到字符的偏移量
    
    if(x > OLED_WIDTH - 8) { x = 0; y = y + 2; }  // 自动换行，因为是16像素高度，所以y加2
    if(y > OLED_PAGE_NUM - 2) { y = 0; }          // 页面重置，预留两页高度
    
    // 写入8x16字体数据到显示缓冲区
    for(uint8_t i = 0; i < 8; i++) {
        oledDirtyFlag.displayBuffer[y][x + i] = F8X16[c * 16 + i];         // 写入上半部分
        oledDirtyFlag.displayBuffer[y + 1][x + i] = F8X16[c * 16 + i + 8]; // 写入下半部分
    }
    
    // 标记这两页为脏区
    oledDirtyFlag.isDirty[y] = 1;
    oledDirtyFlag.isDirty[y + 1] = 1;
}

/**
  * @brief  OLED显示字符串
  * @param  x: 列坐标，范围0~127
  * @param  y: 页坐标，范围0~7
  * @param  str: 要显示的字符串
  * @retval None
  */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str)
{
    uint8_t j = 0;
    while (str[j] != '\0')
    {
        OLED_ShowChar(x, y, str[j]);
        x += 8;  // 8x16字体，宽度为8
        if(x > OLED_WIDTH - 8)
        {
            x = 0;
            y += 2;  // 16像素高度，需要跳过两页
        }
        if(y > OLED_PAGE_NUM - 2) { y = 0; }  // 当y超出范围时重置为0
        j++;
    }
}

/**
  * @brief  设置OLED显示对比度
  * @param  contrast: 对比度值，范围0x00-0xFF
  *         0x00最暗，0xFF最亮，默认值0xCF
  * @retval None
  */
void OLED_SetContrast(uint8_t contrast)
{
    OLED_WriteCmd(OLED_CMD_SET_CONTRAST);  // 设置对比度命令
    OLED_WriteCmd(contrast);               // 对比度值
}

/**
  * @brief  OLED显示数字
  * @param  x: 起始x坐标(0-127)
  * @param  y: 起始页坐标(0-7)
  * @param  num: 要显示的数字
  * @param  len: 数字长度
  * @retval 无
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
  uint8_t t, temp;
  uint8_t enshow = 0;
  
  // 检查参数
  if (y > 7) return;
  
  // 页标记为脏区
  OLED_MarkDirty(y);
  OLED_MarkDirty(y + 1);  // 因为使用8x16字体，需要标记两页
  
  // 逐位显示数字
  for (t = 0; t < len; t++) {
    // 使用整数计算代替pow函数，避免类型转换问题
    uint32_t divisor = 1;
    for (uint8_t i = 0; i < (len - t - 1); i++) {
      divisor *= 10;
    }
    temp = (num / divisor) % 10;
    
    if (enshow == 0 && t < (len - 1)) {
      if (temp == 0) {
        OLED_DrawChar(x + 8 * t, y, ' ');
        continue;
      } else {
        enshow = 1;
      }
    }
    OLED_DrawChar(x + 8 * t, y, temp + '0');
  }
}

/**
  * @brief  OLED显示浮点数
  * @param  x: 起始x坐标(0-127)
  * @param  y: 起始页坐标(0-7)
  * @param  num: 要显示的浮点数
  * @param  intLen: 整数部分长度
  * @param  fracLen: 小数部分长度
  * @param  size: 字体大小
  * @retval 无
  */
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t intLen, uint8_t fracLen, uint8_t size)
{
  uint32_t intPart;  // 整数部分
  uint32_t fracPart; // 小数部分
  
  // 检查参数
  if (y > 7) return;
  
  // 页标记为脏区
  OLED_MarkDirty(y);
  
  // 如果是16像素高字体，则标记下一页为脏区
  if (size == 16 && y < 7) {
    OLED_MarkDirty(y + 1);
  }
  
  // 处理负数
  if (num < 0) {
    OLED_DrawChar(x, y, '-');
    x += 8;
    num = -num;
  }
  
  // 分离整数和小数部分
  intPart = (uint32_t)num;
  
  // 计算10的fracLen次方，避免使用pow函数
  uint32_t multiplier = 1;
  for (uint8_t i = 0; i < fracLen; i++) {
    multiplier *= 10;
  }
  fracPart = (uint32_t)((num - intPart) * multiplier);
  
  // 显示整数部分
  OLED_ShowNum(x, y, intPart, intLen);
  
  // 显示小数点
  OLED_DrawChar(x + intLen * 8, y, '.');
  
  // 显示小数部分
  OLED_ShowNum(x + (intLen + 1) * 8, y, fracPart, fracLen);
}

/**
  * @brief  OLED显示传感器数据
  * @param  zeroPoint: 零点值
  * @param  rangeValue: 量程值
  * @param  concentration: 浓度值
  * @param  outputValue: 输出值(0-4095)
  * @param  outputVoltage: 输出电压(0-3.3V)
  * @param  outputCurrent: 输出电流(4-20mA)
  * @retval 无
  */
void OLED_ShowValue(uint16_t zeroPoint, uint16_t rangeValue, uint16_t concentration, 
                   uint16_t outputValue, float outputVoltage, float outputCurrent)
{
  char buffer[20]; // 字符串缓冲区
  static uint8_t currentPage = 0; // 当前显示页面，0-基本参数，1-输出详情
  static uint32_t lastToggleTime = 0; // 上次切换时间
  uint32_t currentTime = HAL_GetTick(); // 获取当前时间
  
  // 每3秒自动切换页面
  if(currentTime - lastToggleTime > 3000) {
    currentPage = (currentPage + 1) % 2; // 循环切换页面
    lastToggleTime = currentTime; // 更新切换时间
    
    // 切换页面时清屏
    for (uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
      memset(oledDirtyFlag.displayBuffer[i], 0, 128); // 清空显示缓冲区
      oledDirtyFlag.isDirty[i] = 1; // 标记为脏区
    }
  }
  
  // 显示顶部标题栏（固定显示）
  OLED_ShowString(0, 0, "CF4 Sensor System"); // 显示系统标题
  
  // 显示横线分隔符
  for(uint8_t i = 0; i < 128; i++) {
    oledDirtyFlag.displayBuffer[1][i] = 0x80; // 绘制水平线
  }
  oledDirtyFlag.isDirty[1] = 1; // 标记为脏区
  
  // 根据当前页面显示不同内容
  if(currentPage == 0) { // 第一页：基本参数
    // 显示页面标题
    OLED_ShowString(0, 1, "Basic Parameters");
    
    // 浓度值（大字体显示）
    OLED_ShowString(0, 2, "Concentration:"); // 显示浓度标题
    sprintf(buffer, "%5d", concentration); // 格式化浓度值
    OLED_ShowString(0, 3, buffer); // 显示浓度数值
    
    // 显示规范值指示
    if (concentration >= rangeValue) {
      OLED_ShowString(64, 3, "[MAX]"); // 超过量程显示最大值标志
    } else if (concentration <= zeroPoint) {
      OLED_ShowString(64, 3, "[MIN]"); // 低于零点显示最小值标志
    } else {
      // 计算浓度百分比
      uint8_t percent = 0;
      if (rangeValue > zeroPoint) {
        percent = (uint8_t)(((float)(concentration - zeroPoint) / (rangeValue - zeroPoint)) * 100);
      }
      sprintf(buffer, "[%d%%]", percent); // 格式化百分比
      OLED_ShowString(64, 3, buffer); // 显示百分比
    }
    
    // 显示零点和量程
    OLED_ShowString(0, 4, "Zero Point:"); // 显示零点标题
    sprintf(buffer, "%d", zeroPoint); // 格式化零点值
    OLED_ShowString(72, 4, buffer); // 显示零点数值
    
    OLED_ShowString(0, 5, "Range Value:"); // 显示量程标题
    sprintf(buffer, "%d", rangeValue); // 格式化量程值
    OLED_ShowString(72, 5, buffer); // 显示量程数值
    
    // 显示输出值
    OLED_ShowString(0, 6, "Output Value:"); // 显示输出值标题
    sprintf(buffer, "%d", outputValue); // 格式化输出值
    OLED_ShowString(72, 6, buffer); // 显示输出数值
    
    // 显示进度条
    uint8_t progressPercent = 0;
    if (outputValue > 4095) {
      progressPercent = 100; // 超出范围显示100%
    } else {
      progressPercent = (uint8_t)((float)outputValue * 100 / 4095); // 计算进度百分比
    }
    OLED_ShowProgress(0, 7, 100, progressPercent); // 显示输出值进度条
  } 
  else { // 第二页：输出详情
    // 显示页面标题
    OLED_ShowString(0, 1, "Output Details");
    
    // 显示数字输出值
    OLED_ShowString(0, 2, "Digital Value:"); // 显示数字输出标题
    sprintf(buffer, "%d", outputValue); // 格式化数字输出值
    OLED_ShowString(72, 2, buffer); // 显示数字输出数值
    
    // 以图形方式显示当前输出值(横向条形图)
    OLED_ShowString(0, 3, "Output Level:"); // 显示输出电平标题
    uint8_t barWidth = (outputValue > 4095) ? 100 : (uint8_t)((float)outputValue * 100 / 4095); // 计算条形宽度
    
    for(uint8_t i = 0; i < 100; i++) {
      if(i < barWidth) {
        oledDirtyFlag.displayBuffer[4][i + 14] = 0xFF; // 填充的部分
      } else {
        oledDirtyFlag.displayBuffer[4][i + 14] = 0x81; // 未填充的部分(边框)
      }
    }
    oledDirtyFlag.isDirty[4] = 1; // 标记为脏区
    
    // 显示电压输出
    OLED_ShowString(0, 5, "Voltage Output:"); // 显示电压输出标题
    sprintf(buffer, "%.2f V", outputVoltage); // 格式化电压值
    OLED_ShowString(72, 5, buffer); // 显示电压数值
    
    // 显示电流输出
    OLED_ShowString(0, 6, "Current Output:"); // 显示电流输出标题
    sprintf(buffer, "%.2f mA", outputCurrent); // 格式化电流值
    OLED_ShowString(72, 6, buffer); // 显示电流数值
    
    // 显示状态指示
    OLED_ShowString(0, 7, "Status:"); // 显示状态标题
    if (outputCurrent < 4.0f) {
      OLED_ShowString(48, 7, "Under Range"); // 低于范围
    } else if (outputCurrent > 20.0f) {
      OLED_ShowString(48, 7, "Over Range"); // 超出范围
    } else {
      OLED_ShowString(48, 7, "Normal"); // 正常范围
    }
  }
  
  // 刷新显示
  OLED_Refresh(); // 更新屏幕显示
}

/**
  * @brief  显示系统运行状态
  * @param  status: 状态码
  * @param  errorCode: 错误码
  * @retval 无
  */
void OLED_ShowStatus(uint8_t status, uint8_t errorCode)
{
  // 清空第0页和第1页
  memset(oledDirtyFlag.displayBuffer[0], 0, 128);
  memset(oledDirtyFlag.displayBuffer[1], 0, 128);
  oledDirtyFlag.isDirty[0] = 1;
  oledDirtyFlag.isDirty[1] = 1;
  
  // 显示系统状态
  OLED_ShowString(0, 0, "System Status:");
  
  // 根据状态码显示不同状态
  switch (status) {
    case 0:  // 启动中
      OLED_ShowString(0, 1, "Starting...");
      break;
    case 1:  // 预热中
      OLED_ShowString(0, 1, "Warming up...");
      break;
    case 2:  // 正常运行
      OLED_ShowString(0, 1, "Running");
      break;
    case 3:  // 错误状态
      OLED_ShowString(0, 1, "Error:");
      // 显示错误代码
      char errStr[8];
      sprintf(errStr, "0x%02X", errorCode);
      OLED_ShowString(64, 1, errStr);
      break;
    default:
      OLED_ShowString(0, 1, "Unknown");
      break;
  }
  
  // 刷新显示
  OLED_Refresh();
}

/**
 * @brief  显示进度条
 * @param  x: 起始横坐标
 * @param  y: 起始纵坐标
 * @param  max_value: 最大值
 * @param  value: 当前值(0-100)
 * @retval 无
 */
void OLED_ShowProgress(uint8_t x, uint8_t y, uint8_t max_value, uint8_t value)
{
    uint8_t i;
    uint8_t length = 100;
    uint8_t progress;
    
    // 计算进度值
    if (value > max_value) {
        progress = length;
    } else {
        progress = (value * length) / max_value;
    }
    
    // 绘制进度条边框
    OLED_ShowChar(x, y, '[');
    OLED_ShowChar(x + length/6 + 1, y, ']');
    
    // 绘制进度条内部填充
    for (i = 0; i < length/6; i++) {
        if (i < progress/6) {
            OLED_ShowChar(x + i + 1, y, '=');
        } else {
            OLED_ShowChar(x + i + 1, y, ' ');
        }
    }
    
    // 显示百分比
    char buffer[10];
    sprintf(buffer, "%3d%%", value);
    OLED_ShowString(x + length/6 + 3, y, buffer);
    
    // 设置该行为脏区域
    oledDirtyFlag.isDirty[y] = 1;
}

/**
  * @brief  标记页为脏区
  * @param  page: 页号(0-7)
  * @retval 无
  */
static void OLED_MarkDirty(uint8_t page)
{
  if (page < DIRTY_BLOCK_NUM) {
    oledDirtyFlag.isDirty[page] = 1;
  }
}

/**
  * @brief  绘制字符
  * @param  x: 列地址(0-127)
  * @param  y: 页地址(0-7)
  * @param  chr: 要显示的字符
  * @retval 无
  */
static void OLED_DrawChar(uint8_t x, uint8_t y, char chr)
{
  uint8_t c = 0, i = 0;
  
  // 将字符ASCII码转为字库索引
  c = chr - ' ';
  
  // 使用8x16字体
  for (i = 0; i < 8; i++) {
    oledDirtyFlag.displayBuffer[y][x + i] = F8X16[c * 16 + i];
    oledDirtyFlag.displayBuffer[y + 1][x + i] = F8X16[c * 16 + i + 8];
  }
}

/**
 * @brief  向OLED写入命令
 * @param  command: 要写入的命令
 * @retval 0: 成功, 1: 失败
 */
uint8_t OLED_WriteCmd(uint8_t command)
{
    uint8_t data[2];                // 定义数据缓冲区
    data[0] = OLED_CMD;            // 第一个字节表示写命令
    data[1] = command;             // 第二个字节是具体的命令值
    
    // 通过I2C发送命令，超时时间设置为100ms
    if (HAL_I2C_Master_Transmit(oled_hi2c, OLED_ADDR, data, 2, 100) != HAL_OK)
    {
        return 1;                   // 发送失败返回1
    }
    
    return 0;                       // 发送成功返回0
}

/**
 * @brief  向OLED写入数据
 * @param  data: 要写入的数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t OLED_WriteData(uint8_t data)
{
    uint8_t buf[2];                // 定义数据缓冲区
    buf[0] = OLED_DATA;           // 第一个字节表示写数据
    buf[1] = data;                // 第二个字节是具体的数据值
    
    // 通过I2C发送数据，超时时间设置为100ms
    if (HAL_I2C_Master_Transmit(oled_hi2c, OLED_ADDR, buf, 2, 100) != HAL_OK)
    {
        return 1;                  // 发送失败返回1
    }
    
    return 0;                      // 发送成功返回0
}


