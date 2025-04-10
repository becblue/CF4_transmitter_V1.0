/**
  ******************************************************************************
  * @file    sensor.h
  * @brief   CF4������ͨ��ͷ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ���������CF4���崫����ͨ�ŵĽӿں����ݽṹ
  * ���ڵ�һģ�鴫����ͨѶЭ��ʵ��
  * ͨ��USART1�봫��������ͨ��
  *
  ******************************************************************************
  */

/* ��ֹ�ݹ����? -------------------------------------*/
#ifndef __SENSOR_H__
#define __SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* �����ļ� ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "stm32f1xx_hal.h"

/* �궨�� ------------------------------------------------------------*/
#define SENSOR_CMD_READ_CONC      0x03    // ��ȡŨ��ֵ����
#define SENSOR_CMD_READ_RANGE     0x04    // ��ȡ����ֵ����
#define SENSOR_CMD_READ_ZERO      0x05    // ��ȡ���ֵ����?

#define SENSOR_FRAME_HEADER       0x42    // ֡ͷ
#define SENSOR_FRAME_END          0x43    // ֡β

#define SENSOR_BUFFER_SIZE        32      // ������ͨ�Ż�������С
#define SENSOR_TIMEOUT            1000    // ������ͨ�ų�ʱʱ��(ms)

/* ������붨��? */
#define SENSOR_ERROR_NONE          0x00    // �޴���
#define SENSOR_ERROR_TIMEOUT       0x01    // ͨ�ų�ʱ
#define SENSOR_ERROR_CHECKSUM      0x02    // У��ʹ���?
#define SENSOR_ERROR_FRAME         0x03    // ֡��ʽ����
#define SENSOR_ERROR_VALUE         0x04    // ����ֵ����

/* ���ݽṹ���� -----------------------------------------------------------*/
/**
  * @brief  �����������ṹ��
  */
typedef struct {
    uint16_t zeroPoint;       // ����?
    uint16_t rangeValue;      // ����ֵ
    uint16_t concentration;   // ��ǰŨ��ֵ
    uint16_t outputValue;     // ����?(0-4095)
    float outputVoltage;      // ������?(0-3.3V)
    float outputCurrent;      // �������?(4-20mA)
} SensorData_t;

/* �������� --------------------------------------------------------------*/
/**
  * @brief  ������ͨ�ų�ʼ��
  * @retval ��ʼ�����?: 0-�ɹ�, 1-ʧ��
  */
uint8_t Sensor_Init(void);

/**
  * @brief  ��ȡ������Ũ��ֵ
  * @retval Ũ��ֵ
  */
uint16_t Sensor_GetConcentration(void);

/**
  * @brief  ��ȡ����������ֵ
  * @retval ����ֵ
  */
uint16_t Sensor_GetRange(void);

/**
  * @brief  ��ȡ����������?
  * @retval ����?
  */
uint16_t Sensor_GetZeroPoint(void);

/**
  * @brief  ���㴫��������?
  * @param  concentration: ��ǰŨ��ֵ
  * @param  zeroPoint: ����?
  * @param  rangeValue: ����ֵ
  * @retval ����?(0-4095)
  */
uint16_t Sensor_CalculateOutput(uint16_t concentration, uint16_t zeroPoint, uint16_t rangeValue);

/**
  * @brief  ����������?
  * @param  value: ����?(0-4095)
  * @retval ������?(0-3.3V)
  */
float Sensor_CalculateVoltage(uint16_t value);

/**
  * @brief  �����������?
  * @param  value: ����?(0-4095)
  * @retval �������?(4-20mA)
  */
float Sensor_CalculateCurrent(uint16_t value);

/**
  * @brief  �������д���������
  * @param  data: ���������ݽṹ��ָ��
  * @retval ���½��?: 0-�ɹ�, 1-ʧ��
  */
uint8_t Sensor_UpdateAllData(SensorData_t *data);

/**
  * @brief  ��������?
  * @param  value: ����?(0-4095)
  * @retval ��
  */
void Sensor_SetOutput(uint16_t value);

/**
  * @brief  ��ȡ���һ�δ������
  * @retval �������?
  */
uint8_t Sensor_GetLastError(void);

/**
  * @brief  ��ȡ��������Ӧ�������ַ���
  * @param  errorCode: �������?
  * @retval ���������ַ���
  */
const char* Sensor_GetErrorString(uint8_t errorCode);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_H__ */
