/**
  ******************************************************************************
  * @file    oled.h
  * @brief   OLED显示驱动头文件
  ******************************************************************************
  * @attention
  *
  * 本文件定义了OLED显示屏的驱动接口和数据结构
  * 使用I2C通信，支持脏区更新机制，优化刷新效率
  * 
  ******************************************************************************
  */

/* 防止递归包含 -------------------------------------*/
#ifndef __OLED_H__
#define __OLED_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 包含文件 ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"

/* 显示参数定义 -----------------------------------------------------------*/
#define OLED_WIDTH      128     // OLED宽度像素
#define OLED_HEIGHT     64      // OLED高度像素
#define OLED_PAGE_NUM   8       // OLED页数(每页8个像素)

/* OLED地址和命令定义 */
#define OLED_ADDR       0x78    // OLED从机地址(0x3C<<1)
#define OLED_CMD        0x00    // 写命令
#define OLED_DATA       0x40    // 写数据

/* OLED控制引脚定义 */
#define OLED_DC_GPIO_Port  GPIOB   // DC引脚端口，用于数据/命令选择
#define OLED_DC_Pin        GPIO_PIN_0   // DC引脚编号
#define OLED_CS_GPIO_Port  GPIOB   // CS片选引脚端口
#define OLED_CS_Pin        GPIO_PIN_1   // CS片选引脚编号
#define OLED_RST_GPIO_Port GPIOB   // RST复位引脚端口
#define OLED_RST_Pin       GPIO_PIN_2   // RST复位引脚编号

/* 脏区相关定义 */
#define DIRTY_BLOCK_NUM  8      // 脏区块数量(每页一个)
#define DIRTY_BLOCK_SIZE 128    // 脏区块大小(每页128个字节)

/* OLED命令定义 */
#define OLED_CMD_DISPLAY_ON     0xAF    // 开启显示
#define OLED_CMD_DISPLAY_OFF    0xAE    // 关闭显示
#define OLED_CMD_SET_CONTRAST   0x81    // 设置对比度命令

/* 数据结构定义 -----------------------------------------------------------*/
/**
  * @brief  脏区标记结构体
  */
typedef struct {
    uint8_t isDirty[DIRTY_BLOCK_NUM];          // 页脏标记
    uint8_t displayBuffer[OLED_PAGE_NUM][128]; // 显示缓冲区
} OLED_DirtyFlag_t;

/* 函数声明 ------------------------------------------------------------------*/
/**
  * @brief  OLED初始化
  * @retval 初始化结果: 0-成功, 1-失败
  */
uint8_t OLED_Init(I2C_HandleTypeDef *hi2c);

/**
  * @brief  OLED清屏
  * @retval 无
  */
void OLED_Clear(void);

/**
  * @brief  OLED刷新显示(仅更新脏区)
  * @retval 无
  */
void OLED_Refresh(void);

/**
  * @brief  OLED显示字符串
  * @param  x: 起始x坐标(0-127)
  * @param  y: 起始页坐标(0-7)
  * @param  str: 要显示的字符串
  * @retval 无
  */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);

/**
  * @brief  OLED显示单个字符
  * @param  x: 起始x坐标(0-127)
  * @param  y: 起始页坐标(0-7)
  * @param  chr: 要显示的字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t x, uint8_t y, char chr);

/**
  * @brief  OLED显示数字
  * @param  x: 起始x坐标(0-127)
  * @param  y: 起始页坐标(0-7)
  * @param  num: 要显示的数字
  * @param  len: 数字长度
  * @retval 无
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

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
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t intLen, uint8_t fracLen, uint8_t size);

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
                    uint16_t outputValue, float outputVoltage, float outputCurrent);

/**
  * @brief  显示系统运行状态
  * @param  status: 状态码
  * @param  errorCode: 错误码
  * @retval 无
  */
void OLED_ShowStatus(uint8_t status, uint8_t errorCode);

/**
  * @brief  OLED显示进度条
  * @param  x: 起始x坐标(0-127)
  * @param  y: 起始页坐标(0-7)
  * @param  max_value: 最大值
  * @param  value: 当前值
  * @retval 无
  */
void OLED_ShowProgress(uint8_t x, uint8_t y, uint8_t max_value, uint8_t value);

void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_SetPosition(uint8_t x, uint8_t y);
void OLED_SetContrast(uint8_t contrast);
void OLED_RefreshGram(void);

uint8_t OLED_WriteCmd(uint8_t command);  // 写命令，返回0表示成功，1表示失败
uint8_t OLED_WriteData(uint8_t data);    // 写数据，返回0表示成功，1表示失败

#ifdef __cplusplus
}
#endif

#endif /* __OLED_H__ */ 

