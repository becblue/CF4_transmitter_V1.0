/**
  ******************************************************************************
  * @file    dac7311.h
  * @brief   DAC7311��ģת��������ͷ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ���������DAC7311������صĺ����ͺ�
  * DAC7311��һ��12λ��ģת������ͨ��SPI�ӿڿ���
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
#include "spi.h"

/* �궨�� --------------------------------------------------------------------*/
// DAC7311�����ֽڶ���
#define DAC7311_POWER_DOWN_NORMAL     0x00    // ��������ģʽ
#define DAC7311_POWER_DOWN_1K         0x01    // 1K��������
#define DAC7311_POWER_DOWN_100K       0x02    // 100K��������
#define DAC7311_POWER_DOWN_HI_Z       0x03    // ����̬

// DAC7311��������
#define DAC7311_CMD_MASK              0x30    // �������� (Bit 4-5)
#define DAC7311_PD_MASK               0x0C    // ����ģʽ���� (Bit 2-3)

/* �������� ------------------------------------------------------------------*/
/**
  * @brief  DAC7311��ʼ��
  * @retval ��ʼ�����: 0-�ɹ�, 1-ʧ��
  */
uint8_t DAC7311_Init(void);

/**
  * @brief  ����DAC���ֵ
  * @param  value: 12λDAC���ֵ(0-4095)
  * @retval ���ý��: 0-�ɹ�, 1-ʧ��
  */
uint8_t DAC7311_SetValue(uint16_t value);

/**
  * @brief  ����DAC��Դģʽ
  * @param  mode: ��Դģʽ
  *               DAC7311_POWER_DOWN_NORMAL: ��������ģʽ
  *               DAC7311_POWER_DOWN_1K: 1K��������
  *               DAC7311_POWER_DOWN_100K: 100K��������
  *               DAC7311_POWER_DOWN_HI_Z: ����̬
  * @retval ���ý��: 0-�ɹ�, 1-ʧ��
  */
uint8_t DAC7311_SetPowerMode(uint16_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __DAC7311_H */ 

