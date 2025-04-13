#ifndef __SENSOR_H
#define __SENSOR_H

#include <stdint.h>  // ��ӱ�׼�������Ͷ���
#include "main.h"    // ����STM32��ض���
#include <stdio.h>   // ��ӱ�׼�����������

/* ��������С���� */
#define SENSOR_BUFFER_SIZE     32      // ͨ�Ż�������С

/* ���������ݽṹ�嶨�� */
typedef struct {
    uint16_t concentration;    // Ũ��ֵ
    uint32_t rangeValue;      // ����ֵ��32λ��
    uint16_t zeroPoint;       // ���ֵ
    uint16_t outputValue;     // ���ֵ(0-4095)
    float outputVoltage;      // �����ѹ(0-3.3V)
    float outputCurrent;      // �������(4-20mA)
} SensorData_t;

/* �������� */
uint8_t Sensor_Init(void);
uint16_t Sensor_GetConcentration(void);
uint32_t Sensor_GetRange(void);  // �޸ķ���ֵΪ32λ
uint16_t Sensor_GetZeroPoint(void);
uint16_t Sensor_CalculateOutput(uint16_t concentration, uint16_t zeroPoint, uint32_t rangeValue);  // �޸Ĳ�������
float Sensor_CalculateVoltage(uint16_t value);
float Sensor_CalculateCurrent(uint16_t value);
uint8_t Sensor_UpdateAllData(SensorData_t *data);
void Sensor_SetOutput(uint16_t value);
uint8_t Sensor_GetLastError(void);
const char* Sensor_GetErrorString(uint8_t errorCode);
uint8_t Sensor_CheckConnection(void);  // ���ͨ�ż�麯������

/* �ⲿ�������� */
extern UART_HandleTypeDef huart3;  // UART3����������봫����ͨ��

#endif /* __SENSOR_H */ 


