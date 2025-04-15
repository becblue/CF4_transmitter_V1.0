/**
  ******************************************************************************
  * @file    dac7311.h
  * @brief   AD5621BK��ģת��������ͷ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ���������AD5621BK������صĺ����ͺ궨��
  * AD5621BK��һ��12λ��ģת������ͨ��SPI�ӿڿ���
  * 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DAC7311_H
#define __DAC7311_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// GPIO���Ŷ���
#define DAC_CLK_PIN     GPIO_PIN_13    // PB13 - CLK
#define DAC_DIN_PIN     GPIO_PIN_14    // PB14 - DIN
#define DAC_SYNC_PIN    GPIO_PIN_15    // PB15 - SYNC
#define DAC_GPIO_PORT   GPIOB          // �������Ŷ���GPIOB��

// GPIOλ�����궨��
#define DAC_CLK_HIGH()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_CLK_PIN, GPIO_PIN_SET)     // CLK�ø�
#define DAC_CLK_LOW()    HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_CLK_PIN, GPIO_PIN_RESET)   // CLK�õ�
#define DAC_DIN_HIGH()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_DIN_PIN, GPIO_PIN_SET)     // DIN�ø�
#define DAC_DIN_LOW()    HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_DIN_PIN, GPIO_PIN_RESET)   // DIN�õ�
#define DAC_SYNC_HIGH()  HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_SYNC_PIN, GPIO_PIN_SET)    // SYNC�ø�
#define DAC_SYNC_LOW()   HAL_GPIO_WritePin(DAC_GPIO_PORT, DAC_SYNC_PIN, GPIO_PIN_RESET)  // SYNC�õ�

/* �궨�� --------------------------------------------------------------------*/
// AD5621BK��Դģʽ����
#define AD5621BK_POWER_DOWN_NORMAL     0x00    // ��������ģʽ
#define AD5621BK_POWER_DOWN_1K         0x01    // 1K�����ض�
#define AD5621BK_POWER_DOWN_100K       0x02    // 100K�����ض�
#define AD5621BK_POWER_DOWN_HI_Z       0x03    // ����̬

// AD5621BK�����
#define AD5621BK_CMD_WRITE_UPDATE_DAC  0x03    // д�벢����DAC�Ĵ���
#define AD5621BK_CMD_MASK              0x03    // �������� (Bit 4-5)
#define AD5621BK_PD_MASK              0x0C    // ��Դģʽ���� (Bit 2-3)

// �����Զ��壨������ɴ�����ݣ�
#define DAC7311_POWER_DOWN_NORMAL      AD5621BK_POWER_DOWN_NORMAL
#define DAC7311_POWER_DOWN_1K          AD5621BK_POWER_DOWN_1K
#define DAC7311_POWER_DOWN_100K        AD5621BK_POWER_DOWN_100K
#define DAC7311_POWER_DOWN_HI_Z        AD5621BK_POWER_DOWN_HI_Z
#define DAC7311_CMD_WRITE_UPDATE_DAC   AD5621BK_CMD_WRITE_UPDATE_DAC
#define DAC7311_CMD_MASK               AD5621BK_CMD_MASK
#define DAC7311_PD_MASK               AD5621BK_PD_MASK

// DAC��ض���
#define DAC_FULL_SCALE      4095    // DAC������ֵ��12λDAC�����ֵΪ4095��
#define DAC_RAMP_STEP       100     // DAC���䲽��ֵ
#define DAC_RAMP_DELAY      10      // ÿ�ν������ʱ(ms)

/* �������� ------------------------------------------------------------------*/
/**
  * @brief  DAC7311初始�?
  * @param  hspi: SPI通信句柄指针
  * @param  cs_port: 片选引脚所在的GPIO端口
  * @param  cs_pin: 片选引脚编�?
  * @retval 初始化结�?: 0-成功, 1-失败
  */
uint8_t DAC7311_Init(void);

/**
  * @brief  设置DAC输出�?
  * @param  value: 12位DAC数�?(0-4095)
  * @retval 设置结果: 0-成功, 1-失败
  */
uint8_t DAC7311_SetValue(uint16_t value);

/**
  * @brief  设置DAC电源模式
  * @param  mode: 电源模式
  *               DAC7311_POWER_DOWN_NORMAL: 正常工作模式
  *               DAC7311_POWER_DOWN_1K: 1K下拉关断
  *               DAC7311_POWER_DOWN_100K: 100K下拉关断
  *               DAC7311_POWER_DOWN_HI_Z: 高阻�?
  * @retval 设置结果: 0-成功, 1-失败
  */
uint8_t DAC7311_SetPowerMode(uint8_t mode);

/**
  * @brief  设置DAC输出电压
  * @param  voltage: 期望输出电压(单位:伏特)
  * @param  vref: 参考电�?(单位:伏特)
  * @retval 设置结果: 0-成功, 1-失败
  */
uint8_t DAC7311_SetVoltage(float voltage, float vref);

void DAC7311_PowerDown(void);                // �������ģʽ
void DAC7311_PowerUp(void);                  // �˳�����ģʽ

void DAC7311_RampToValue(uint16_t targetValue);                // ʹ�ý��䷽ʽ����DAC���ֵ

#ifdef __cplusplus
}
#endif

#endif /* __DAC7311_H */


