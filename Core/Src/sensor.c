/**
  ******************************************************************************
  * @file    sensor.c
  * @brief   CF4������ͨ��Դ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ�ʵ������CF4���崫����ͨ�ŵĹ���
  * ͨ��USART1�봫��������ͨ�ţ���ȡŨ�ȡ����̺��������
  * ��������Щ���ݼ������ֵ����ѹ�͵���
  *
  ******************************************************************************
  */

/* �����ļ� ------------------------------------------------------------------*/
#include "sensor.h"
#include "dac7311.h"
#include "oled.h"
#include <string.h>

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define SENSOR_MAX_RETRY           3       // ������Դ���
#define SENSOR_RETRY_DELAY         50      // ������ʱ(ms)

/* ������붨�� */
#define SENSOR_ERROR_NONE          0x00    // �޴���
#define SENSOR_ERROR_TIMEOUT       0x01    // ͨ�ų�ʱ
#define SENSOR_ERROR_CHECKSUM      0x02    // У��ʹ���
#define SENSOR_ERROR_FRAME         0x03    // ֡��ʽ����
#define SENSOR_ERROR_VALUE         0x04    // ����ֵ����

/* ˽�б��� ------------------------------------------------------------------*/
static uint8_t rxBuffer[SENSOR_BUFFER_SIZE];  // ���ջ�����
static SensorData_t sensorData = {0};         // ���������ݽṹ��
static uint8_t isInitialized = 0;             // ��ʼ����־
static uint8_t lastError = SENSOR_ERROR_NONE; // ���һ�δ������

/* ˽�к���ԭ�� --------------------------------------------------------------*/
static uint8_t Sensor_SendCommand(uint8_t cmd);
static uint16_t Sensor_ParseResponse(void);
static uint8_t Sensor_VerifyChecksum(uint8_t *data, uint8_t length);
static uint8_t Sensor_CalculateChecksum(uint8_t *data, uint8_t length);
static uint8_t Sensor_IsValueValid(uint16_t value, uint8_t cmd);

/* ����ʵ�� ------------------------------------------------------------------*/

/**
  * @brief  ������ͨ�ų�ʼ��
  * @retval ��ʼ�����: 0-�ɹ�, 1-ʧ��
  */
uint8_t Sensor_Init(void)
{
  // ��ʱһ��ʱ��ȴ��������ϵ��ȶ�
  HAL_Delay(1000);  // �ȴ��������ϵ��ȶ�
  
  // ��ս��ջ�����
  memset(rxBuffer, 0, SENSOR_BUFFER_SIZE);  // ��ս��ջ�����
  
  // ����������
  lastError = SENSOR_ERROR_NONE;  // ����ϴεĴ������
  
  // ���Ի�ȡ���������ֵ������ֵ
  uint16_t zeroPoint = Sensor_GetZeroPoint();  // ��ȡ���ֵ
  HAL_Delay(100);  // ������ʱ����������ͨ��
  uint16_t rangeValue = Sensor_GetRange();  // ��ȡ����ֵ
  
  // ����ȡֵ�Ƿ���Ч
  if (zeroPoint == 0xFFFF || rangeValue == 0xFFFF) {
    return 1;  // ��ʼ��ʧ�ܣ��޷���ȡ��������
  }
  
  // �����ʼֵ���
  if (rangeValue <= zeroPoint) {
    // ����ֵ����������ֵ
    lastError = SENSOR_ERROR_VALUE;  // ��¼��������
    return 1;  // �����쳣
  }
  
  // ���´��������ݽṹ��
  sensorData.zeroPoint = zeroPoint;  // �������ֵ
  sensorData.rangeValue = rangeValue;  // ��������ֵ
  sensorData.concentration = 0;  // ��ʼŨ��ֵΪ0
  sensorData.outputValue = 0;  // ��ʼ���ֵΪ0
  sensorData.outputVoltage = 0.0f;  // ��ʼ��ѹΪ0V
  sensorData.outputCurrent = 4.0f;  // ��ʼ����Ϊ4mA(��С���)
  
  // ���ó�ʼ����־
  isInitialized = 1;  // ��ǳ�ʼ�����
  
  return 0;  // ��ʼ���ɹ�
}

/**
  * @brief  ��ȡ������Ũ��ֵ
  * @retval Ũ��ֵ��ʧ���򷵻�0xFFFF
  */
uint16_t Sensor_GetConcentration(void)
{
  uint8_t retry = 0;  // ���Լ�����
  uint16_t concentration;  // Ũ��ֵ
  
  // ʹ�����Ի��ƶ�ȡŨ��ֵ
  while (retry < SENSOR_MAX_RETRY) {
    // ���Ͷ�ȡŨ������
    if (Sensor_SendCommand(SENSOR_CMD_READ_CONC) == 0) {
      // ������Ӧ����
      concentration = Sensor_ParseResponse();
      
      // ���������Ч��
      if (concentration != 0xFFFF && Sensor_IsValueValid(concentration, SENSOR_CMD_READ_CONC)) {
        // ���´��������ݽṹ��
        sensorData.concentration = concentration;
        lastError = SENSOR_ERROR_NONE;  // �������״̬
        return concentration;  // ��ȡ�ɹ�������Ũ��ֵ
      }
    }
    
    retry++;  // �������Լ���
    HAL_Delay(SENSOR_RETRY_DELAY);  // ��ʱһ��ʱ�������
  }
  
  // ȫ������ʧ�ܣ����ش���ֵ
  return 0xFFFF;  // ��ȡʧ��
}

/**
  * @brief  ��ȡ����������ֵ
  * @retval ����ֵ��ʧ���򷵻�0xFFFF
  */
uint16_t Sensor_GetRange(void)
{
  uint8_t retry = 0;  // ���Լ�����
  uint16_t rangeValue;  // ����ֵ
  
  // ʹ�����Ի��ƶ�ȡ����ֵ
  while (retry < SENSOR_MAX_RETRY) {
    // ���Ͷ�ȡ��������
    if (Sensor_SendCommand(SENSOR_CMD_READ_RANGE) == 0) {
      // ������Ӧ����
      rangeValue = Sensor_ParseResponse();
      
      // ���������Ч��
      if (rangeValue != 0xFFFF && Sensor_IsValueValid(rangeValue, SENSOR_CMD_READ_RANGE)) {
        // ���´��������ݽṹ��
        sensorData.rangeValue = rangeValue;
        lastError = SENSOR_ERROR_NONE;  // �������״̬
        return rangeValue;  // ��ȡ�ɹ�����������ֵ
      }
    }
    
    retry++;  // �������Լ���
    HAL_Delay(SENSOR_RETRY_DELAY);  // ��ʱһ��ʱ�������
  }
  
  // ȫ������ʧ�ܣ����ش���ֵ
  return 0xFFFF;  // ��ȡʧ��
}

/**
  * @brief  ��ȡ���������ֵ
  * @retval ���ֵ��ʧ���򷵻�0xFFFF
  */
uint16_t Sensor_GetZeroPoint(void)
{
  uint8_t retry = 0;  // ���Լ�����
  uint16_t zeroPoint;  // ���ֵ
  
  // ʹ�����Ի��ƶ�ȡ���ֵ
  while (retry < SENSOR_MAX_RETRY) {
    // ���Ͷ�ȡ�������
    if (Sensor_SendCommand(SENSOR_CMD_READ_ZERO) == 0) {
      // ������Ӧ����
      zeroPoint = Sensor_ParseResponse();
      
      // ���������Ч��
      if (zeroPoint != 0xFFFF && Sensor_IsValueValid(zeroPoint, SENSOR_CMD_READ_ZERO)) {
        // ���´��������ݽṹ��
        sensorData.zeroPoint = zeroPoint;
        lastError = SENSOR_ERROR_NONE;  // �������״̬
        return zeroPoint;  // ��ȡ�ɹ����������ֵ
      }
    }
    
    retry++;  // �������Լ���
    HAL_Delay(SENSOR_RETRY_DELAY);  // ��ʱһ��ʱ�������
  }
  
  // ȫ������ʧ�ܣ����ش���ֵ
  return 0xFFFF;  // ��ȡʧ��
}

/**
  * @brief  ���㴫�������ֵ
  * @param  concentration: ��ǰŨ��ֵ
  * @param  zeroPoint: ���ֵ
  * @param  rangeValue: ����ֵ
  * @retval ���ֵ(0-4095)
  */
uint16_t Sensor_CalculateOutput(uint16_t concentration, uint16_t zeroPoint, uint16_t rangeValue)
{
  // ��ֹ�������
  if (rangeValue <= zeroPoint) {
    return 0;  // ������Ч������0
  }
  
  // �������ֵ (����ӳ��Ũ��ֵ��0-4095��Χ)
  uint32_t output;  // ʹ��32λ��������������
  
  // ���Ũ��С����㣬���Ϊ0
  if (concentration <= zeroPoint) {
    output = 0;  // Ũ��С����㣬�����Сֵ
  }
  // ���Ũ�ȴ������̣����Ϊ���ֵ
  else if (concentration >= rangeValue) {
    output = 4095;  // Ũ�ȴ������̣�������ֵ
  }
  // ��������ӳ��
  else {
    // ʹ��32λ����������
    output = (uint32_t)(concentration - zeroPoint) * 4095 / (rangeValue - zeroPoint);
    // ȷ�����ֵ����Ч��Χ��
    if (output > 4095) {
      output = 4095;  // ����������ֵ
    }
  }
  
  return (uint16_t)output;  // ���ؼ��������ֵ
}

/**
  * @brief  ���������ѹ
  * @param  value: ���ֵ(0-4095)
  * @retval �����ѹ(0-3.3V)
  */
float Sensor_CalculateVoltage(uint16_t value)
{
  // ����ӳ�����ֵ��0-3.3V��ѹ��Χ
  // �������뷶Χ
  if (value > 4095) {
    value = 4095;  // �����������ֵ
  }
  
  // �����Ӧ��ѹֵ
  return (float)value * 3.3f / 4095.0f;  // ����ӳ�䵽0-3.3V��Χ
}

/**
  * @brief  �����������
  * @param  value: ���ֵ(0-4095)
  * @retval �������(4-20mA)
  */
float Sensor_CalculateCurrent(uint16_t value)
{
  // ����ӳ�����ֵ��4-20mA������Χ
  // �������뷶Χ
  if (value > 4095) {
    value = 4095;  // �����������ֵ
  }
  
  // �����Ӧ����ֵ
  return 4.0f + (float)value * 16.0f / 4095.0f;  // 4mA + ����ӳ�䷶Χ(0-16mA)
}

/**
  * @brief  �������д���������
  * @param  data: ���������ݽṹ��ָ��
  * @retval ���½��: 0-�ɹ�, 1-ʧ��
  */
uint8_t Sensor_UpdateAllData(SensorData_t *data)
{
  if (!isInitialized) {
    lastError = SENSOR_ERROR_VALUE;  // ���ô������
    return 1;  // δ��ʼ�������ش���
  }
  
  // ��ȡ������Ũ��ֵ
  uint16_t concentration = Sensor_GetConcentration();
  if (concentration == 0xFFFF) {
    return 1;  // ��ȡŨ��ʧ��
  }
  
  // �������ֵ
  uint16_t outputValue = Sensor_CalculateOutput(
    concentration,             // ��ǰŨ��ֵ
    sensorData.zeroPoint,      // ʹ�ñ�������ֵ
    sensorData.rangeValue      // ʹ�ñ��������ֵ
  );
  
  // ���������ѹ�͵���
  float outputVoltage = Sensor_CalculateVoltage(outputValue);  // �����Ӧ��ѹ
  float outputCurrent = Sensor_CalculateCurrent(outputValue);  // �����Ӧ����
  
  // ���´��������ݽṹ��
  sensorData.concentration = concentration;  // ����Ũ��ֵ
  sensorData.outputValue = outputValue;      // �������ֵ
  sensorData.outputVoltage = outputVoltage;  // ���µ�ѹֵ
  sensorData.outputCurrent = outputCurrent;  // ���µ���ֵ
  
  // ����ṩ���ⲿ�ṹ��ָ�룬��������
  if (data != NULL) {
    memcpy(data, &sensorData, sizeof(SensorData_t));  // �������ݵ��ⲿ�ṹ��
  }
  
  // ����DAC���
  Sensor_SetOutput(outputValue);  // ����DAC���
  
  return 0;  // ���³ɹ�
}

/**
  * @brief  �������ֵ
  * @param  value: ���ֵ(0-4095)
  * @retval ��
  */
void Sensor_SetOutput(uint16_t value)
{
  // �������뷶Χ
  if (value > 4095) {
    value = 4095;  // �����������ֵ
  }
  
  // ʹ��DAC7311���������ѹ
  if (DAC7311_SetValue(value) != 0) {
    // DAC����ʧ��ʱ��¼����
    lastError = SENSOR_ERROR_VALUE;  // ��¼DAC���ô���
  }
}

/**
  * @brief  ��ȡ���һ�δ������
  * @retval �������
  */
uint8_t Sensor_GetLastError(void)
{
  return lastError;  // �������һ�δ������
}

/**
  * @brief  �������������
  * @param  cmd: �����ֽ�
  * @retval ���ͽ��: 0-�ɹ�, 1-ʧ��
  */
static uint8_t Sensor_SendCommand(uint8_t cmd)
{
  uint8_t txBuffer[5];  // ����֡������
  
  // ��������֡
  txBuffer[0] = SENSOR_FRAME_HEADER;  // ֡ͷ
  txBuffer[1] = cmd;                  // ������
  txBuffer[2] = 0x00;                 // �����ֽ� (������)
  txBuffer[3] = Sensor_CalculateChecksum(txBuffer, 3);  // У���
  txBuffer[4] = SENSOR_FRAME_END;     // ֡β
  
  // ��ս��ջ�����
  memset(rxBuffer, 0, SENSOR_BUFFER_SIZE);  // ����ϴν��յ�����
  
  // ��������֡
  if (HAL_UART_Transmit(&huart1, txBuffer, 5, SENSOR_TIMEOUT) != HAL_OK) {
    lastError = SENSOR_ERROR_TIMEOUT;  // ���ó�ʱ����
    return 1;  // ����ʧ��
  }
  
  // ������Ӧ����
  if (HAL_UART_Receive(&huart1, rxBuffer, 7, SENSOR_TIMEOUT) != HAL_OK) {
    lastError = SENSOR_ERROR_TIMEOUT;  // ���ó�ʱ����
    return 1;  // ����ʧ��
  }
  
  return 0;  // ���ͳɹ�
}

/**
  * @brief  ������������Ӧ����
  * @retval ���������ʧ���򷵻�0xFFFF
  */
static uint16_t Sensor_ParseResponse(void)
{
  // ���֡ͷ��֡β
  if (rxBuffer[0] != SENSOR_FRAME_HEADER || rxBuffer[6] != SENSOR_FRAME_END) {
    lastError = SENSOR_ERROR_FRAME;  // ����֡��ʽ����
    return 0xFFFF;  // ֡��ʽ����
  }
  
  // ��֤У���
  if (!Sensor_VerifyChecksum(rxBuffer, 5)) {
    lastError = SENSOR_ERROR_CHECKSUM;  // ����У��ʹ���
    return 0xFFFF;  // У��ʹ���
  }
  
  // ��ȡ����
  uint16_t value = (rxBuffer[3] << 8) | rxBuffer[4];  // ��ϸߵ��ֽ�Ϊ16λ����
  
  return value;  // ���ؽ�����������
}

/**
  * @brief  ��֤У���
  * @param  data: ���ݻ�����
  * @param  length: ��ҪУ��ĳ���(������У��ͱ���)
  * @retval У����: 1-ͨ��, 0-ʧ��
  */
static uint8_t Sensor_VerifyChecksum(uint8_t *data, uint8_t length)
{
  uint8_t calcChecksum = Sensor_CalculateChecksum(data, length);  // ����У���
  return (calcChecksum == data[length]);  // �Ƚϼ���ֵ�����ֵ
}

/**
  * @brief  ����У���
  * @param  data: ���ݻ�����
  * @param  length: ��ҪУ��ĳ���
  * @retval У���
  */
static uint8_t Sensor_CalculateChecksum(uint8_t *data, uint8_t length)
{
  uint8_t sum = 0;  // У��ͳ�ʼֵ
  for (uint8_t i = 0; i < length; i++) {
    sum += data[i];  // �ۼ�ÿ���ֽ�
  }
  return sum;  // �����ۼӺ���ΪУ���
}

/**
  * @brief  ��������Ƿ�����Ч��Χ��
  * @param  value: ����ֵ
  * @param  cmd: ��������
  * @retval �����: 1-��Ч, 0-��Ч
  */
static uint8_t Sensor_IsValueValid(uint16_t value, uint8_t cmd)
{
  // ���ݲ�ͬ�������ͼ��������Ч��
  switch (cmd) {
    case SENSOR_CMD_READ_CONC:
      // Ũ��ֵ��Χ��� (�ɸ���ʵ���������)
      return 1;  // Ĭ�Ͻ�������Ũ��ֵ
      
    case SENSOR_CMD_READ_RANGE:
      // ����ֵ�������0
      if (value == 0) {
        lastError = SENSOR_ERROR_VALUE;
        return 0;
      }
      return 1;
      
    case SENSOR_CMD_READ_ZERO:
      // ���ֵ��Χ���
      return 1;  // Ĭ�Ͻ����������ֵ
      
    default:
      return 0;  // δ֪��������
  }
}

/**
  * @brief  ��ȡ��������Ӧ�������ַ���
  * @param  errorCode: �������
  * @retval ���������ַ���
  */
const char* Sensor_GetErrorString(uint8_t errorCode)
{
  // ���ݴ�����뷵�ض�Ӧ�Ĵ�������
  switch (errorCode) {
    case SENSOR_ERROR_NONE:
      return "�޴���";  // �޴���
      
    case SENSOR_ERROR_TIMEOUT:
      return "ͨ�ų�ʱ";  // ͨ�ų�ʱ
      
    case SENSOR_ERROR_CHECKSUM:
      return "У��ʹ���";  // У��ʹ���
      
    case SENSOR_ERROR_FRAME:
      return "֡��ʽ����";  // ֡��ʽ����
      
    case SENSOR_ERROR_VALUE:
      return "����ֵ��Ч";  // ����ֵ��Ч
      
    default:
      return "δ֪����";  // δ֪�������
  }
} 

