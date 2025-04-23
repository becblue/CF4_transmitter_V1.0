/**
  ******************************************************************************
  * @file    sensor.c
  * @brief   CF4������ͨ��Դ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ�ʵ������CF4���崫����ͨ�ŵĹ���
  * ͨ��USART2�봫��������ͨ�ţ���ȡŨ�ȡ����̺��������
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

/* ֡��ʽ���� */
#define SENSOR_TX_HEADER          0xAA    // ����֡ͷ 0xAA
#define SENSOR_TX_END            0xBB    // ����֡β 0xBB
#define SENSOR_RX_HEADER          0x55    // ����֡ͷ 0x55
#define SENSOR_RX_END            0xAA    // ����֡β 0xAA
#define SENSOR_ADDR              0x00    // ������Ĭ�ϵ�ַ 0x00

/* ����� */
#define SENSOR_CMD_READ_CONC     0x03    // ��ȡŨ������
#define SENSOR_CMD_READ_RANGE    0x04    // ��ȡ��������
#define SENSOR_CMD_READ_ZERO     0x05    // ��ȡ�������

/* ������붨�� */
#define SENSOR_ERROR_NONE          0x00    // �޴���
#define SENSOR_ERROR_TIMEOUT       0x01    // ͨ�ų�ʱ
#define SENSOR_ERROR_CHECKSUM      0x02    // У��ʹ���
#define SENSOR_ERROR_FRAME         0x03    // ֡��ʽ����
#define SENSOR_ERROR_VALUE         0x04    // ����ֵ����
#define SENSOR_ERROR_TRANSMIT      0x05    // ���ʹ���
#define SENSOR_ERROR_RECEIVE       0x06    // ���մ���
#define SENSOR_ERROR_FORMAT        0x07    // ֡��ʽ����
#define SENSOR_ERROR_CMD           0x08    // ���ƥ��
#define SENSOR_ERROR_STATUS        0x09    // ״̬�쳣
#define SENSOR_ERROR_EXECUTE       0x0A    // ����ִ�д���
#define SENSOR_ERROR_NO_ZERO       0x0B    // �������Ϣ
#define SENSOR_ERROR_RESPONSE      0x0C    // δ֪��Ӧ״̬
#define SENSOR_ERROR_NOT_CALIBRATED 0x0D    // ������δ�궨
#define SENSOR_ERROR_ZERO_SHIFTING 0x0E    // ���������ƽ����
#define SENSOR_ERROR_CALIBRATING   0x0F    // �������궨������
#define SENSOR_ERROR_FAULT         0x10    // ����������
#define SENSOR_ERROR_UNKNOWN_STATUS 0x11   // δ֪������״̬

/* ˽�б��� ------------------------------------------------------------------*/
static uint8_t rxBuffer[SENSOR_BUFFER_SIZE];  // ���ջ�����
static uint8_t isInitialized = 0;             // ��ʼ����־
static uint8_t lastError = SENSOR_ERROR_NONE; // ���һ�δ������
static uint8_t commLostCount = 0;             // ͨѶ��ʧ������
static uint32_t currentRangeValue = 0;        // ��ǰ����ֵ

/* ˽�к���ԭ�� --------------------------------------------------------------*/
static uint8_t Sensor_SendCommand(uint8_t cmd, uint8_t *data, uint8_t dataLen);
static uint32_t Sensor_ParseResponse(void);
static uint8_t Sensor_CalculateChecksum(uint8_t *data, uint8_t length);
static uint8_t Sensor_IsValueValid(uint32_t value, uint8_t cmd);

/* ����ʵ�� ------------------------------------------------------------------*/

/**
  * @brief  ������ͨ�ų�ʼ��
  * @retval ��ʼ�����: 0-�ɹ�, 1-ʧ��
  */
uint8_t Sensor_Init(void)
{
  // �ȴ��������ϵ��ȶ�
  HAL_Delay(3000);  // �޸�Ϊ3��Ԥ��ʱ��
  
  // ��ս��ջ�����
  memset(rxBuffer, 0, SENSOR_BUFFER_SIZE);  // ��ս��ջ�����
  
  // ����������
  lastError = SENSOR_ERROR_NONE;  // ����ϴεĴ������
  commLostCount = 0;             // ����ͨѶʧ�ܼ���
  
  // ��ȡ����������ֵ
  currentRangeValue = Sensor_GetRange();  // ��ȡ����ֵ
  
  // �������ֵ�Ƿ���Ч
  if (currentRangeValue == 0) {  // ֻ��0����Чֵ
    lastError = SENSOR_ERROR_VALUE;  // ��¼��������
    return 1;  // ��ʼ��ʧ�ܣ�����ֵ��Ч
  }
  
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
    if (Sensor_SendCommand(SENSOR_CMD_READ_CONC, NULL, 0) == 0) {
      // ������Ӧ����
      concentration = Sensor_ParseResponse();
      
      // ���������Ч��
      if (concentration != 0xFFFF && Sensor_IsValueValid(concentration, SENSOR_CMD_READ_CONC)) {
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
  * @retval ����ֵ��ʧ���򷵻�0xFFFFFFFF
  */
uint32_t Sensor_GetRange(void)
{
  uint8_t retry = 0;  // ���Լ�����
  uint32_t rangeValue;  // ����ֵ��32λ��
  
  // ʹ�����Ի��ƶ�ȡ����ֵ
  while (retry < SENSOR_MAX_RETRY) {
    // ���Ͷ�ȡ��������
    if (Sensor_SendCommand(SENSOR_CMD_READ_RANGE, NULL, 0) == 0) {
      // ������Ӧ����
      rangeValue = Sensor_ParseResponse();
      
      // ���������Ч�ԣ�ֻҪ����0������Ч�ģ�
      if (Sensor_IsValueValid(rangeValue, SENSOR_CMD_READ_RANGE)) {
        return rangeValue;  // ��ȡ�ɹ�����������ֵ
      }
    }
    
    retry++;  // �������Լ���
    HAL_Delay(SENSOR_RETRY_DELAY);  // ��ʱһ��ʱ�������
  }
  
  // ȫ������ʧ�ܣ����ش���ֵ
  return 0xFFFFFFFF;  // ��ȡʧ��
}

/**
  * @brief  ��ȡ���������ֵ
  * @retval ���ֵ���̶�Ϊ0��
  */
uint16_t Sensor_GetZeroPoint(void)
{
  return SENSOR_ZERO_POINT;  // ֱ�ӷ�����㳣��ֵ
}

/**
  * @brief  ���㴫�������ֵ
  * @param  concentration: ��ǰŨ��ֵ
  * @param  rangeValue: ����ֵ
  * @retval ���ֵ(729-3649)
  */
uint16_t Sensor_CalculateOutput(uint16_t concentration, uint32_t rangeValue)
{
  // ��ֹ�������
  if (rangeValue == 0) {
    return 729;  // ������Ч�����ض�Ӧ4mA��DACֵ
  }
  
  // �������ֵ (����ӳ��Ũ��ֵ��729-3649��Χ)
  uint32_t output;  // ʹ��32λ��������������
  
  // ���Ũ��С�ڵ�����㣨0�������Ϊ��Сֵ
  if (concentration <= SENSOR_ZERO_POINT) {
    output = 729;  // ��Ӧ4mA��DACֵ
  }
  // ���Ũ�ȴ��ڵ�������ֵ�����Ϊ���ֵ
  else if (concentration >= rangeValue) {
    output = 3649;  // ��Ӧ20mA��DACֵ
  }
  // �����������ӳ��
  else {
    // ʹ��32λ����������
    // ӳ�乫ʽ��output = 729 + (concentration * (3649 - 729)) / rangeValue
    output = 729 + (concentration * (uint32_t)(3649 - 729)) / rangeValue;
  }
  
  return (uint16_t)output;  // ���ؼ�����
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
  * @param  sensorData: ���������ݽṹ��ָ��
  * @retval 0:�ɹ�, ��0:ʧ��
  */
uint8_t Sensor_UpdateAllData(SensorData_t* sensorData)
{
    uint8_t result = 0;  // ��������ֵ
    uint16_t concentration;  // Ũ��ֵ��ʱ����

    // ����ʼ��״̬
    if (!isInitialized) {
        lastError = SENSOR_ERROR_VALUE;  // ���ô������
        return 1;  // δ��ʼ�������ش���
    }

    // ��ȡŨ��ֵ
    concentration = Sensor_GetConcentration();  // ��ȡŨ��ֵ
    
    // �޸��ж��߼���ֻ����Ũ��ֵ��Чʱ����Ϊ��ͨѶʧ��
    if (concentration == 0xFFFF)  // �����ȡʧ��
    {
        commLostCount++;  // ͨѶʧ�ܼ�����1
        printf("[����] ͨѶʧ�ܼ���: %d\r\n", commLostCount);  // ��ӵ�����Ϣ
        
        // �������ʧ�ܴ����ﵽ��ֵ������Ϊ����������
        if (commLostCount >= COMM_LOST_MAX_COUNT)
        {
            printf("[����] ͨѶ��ʧ����������������\r\n");  // ��ӵ�����Ϣ
            // ����DAC���Ϊ���ֵ
            sensorData->outputValue = COMM_LOST_DAC_VALUE;  // ����Ϊ���DACֵ
            sensorData->outputVoltage = (float)COMM_LOST_DAC_VALUE * 3.3f / 4095.0f;  // �����Ӧ��ѹ
            sensorData->outputCurrent = 4.0f + (sensorData->outputVoltage * 16.0f / 3.3f);  // �����Ӧ����
            
            // ����DAC���
            Sensor_SetOutput(COMM_LOST_DAC_VALUE);  // ����DAC���
        }
        result = 1;  // ���ش���״̬
    }
    else  // ͨѶ����
    {
        commLostCount = 0;  // ����ͨѶʧ�ܼ���
        
        // ���´���������
        sensorData->concentration = concentration;  // ����Ũ��ֵ
        sensorData->rangeValue = currentRangeValue;  // ʹ�ô洢������ֵ
        sensorData->outputValue = Sensor_CalculateOutput(concentration, currentRangeValue);  // �������ֵ
        sensorData->outputVoltage = (float)sensorData->outputValue * 3.3f / 4095.0f;  // ���������ѹ
        sensorData->outputCurrent = 4.0f + (sensorData->outputVoltage * 16.0f / 3.3f);  // �����Ӧ����
        
        // ����DAC���
        Sensor_SetOutput(sensorData->outputValue);  // ����DAC���
        
        printf("[����] ͨѶ������Ũ��ֵ: %d, DAC���ֵ: %d\r\n", 
               concentration, sensorData->outputValue);  // ��ӵ�����Ϣ
        
        result = 0;  // ���سɹ�״̬
    }

    return result;  // ����ִ�н��
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
  if (DAC7311_SetValue(value) == 0) {  // DAC���óɹ�
    LED_HandleDAC7311Update();  // ����LED2״̬
  } else {
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
  * @param  data: ���ݻ�����
  * @param  dataLen: ���ݳ���
  * @retval ���ͽ��: 0-�ɹ�, 1-ʧ��
  */
static uint8_t Sensor_SendCommand(uint8_t cmd, uint8_t *data, uint8_t dataLen)
{
    uint8_t txBuffer[10];  // ���ͻ����������������ͻ�����
    
    // ��������֡
    txBuffer[0] = 0xAA;        // ֡ͷ
    txBuffer[1] = 0x00;        // �豸��ַ
    txBuffer[2] = cmd;         // ������
    
    // ����������ݣ�5�ֽڣ�
    for(uint8_t i = 0; i < 5; i++) {
        txBuffer[3+i] = (i < dataLen) ? data[i] : 0x00;
    }
    
    // ����У���
    txBuffer[8] = Sensor_CalculateChecksum(txBuffer, 8);  // ʹ��У��ͺ���
    txBuffer[9] = 0xBB;        // ֡β
    
    // ��ӡ���͵�����֡
    printf("\r\n[����] ");
    for(uint8_t i = 0; i < 10; i++) {
        printf("%02X ", txBuffer[i]);
    }
    printf("\r\n");
    
    // ���ȫ�ֽ��ջ�����
    memset(rxBuffer, 0, SENSOR_BUFFER_SIZE);  // �ڽ���ǰ���ȫ��rxBuffer
    
    // ��������
    if(HAL_UART_Transmit(&huart3, txBuffer, 10, 500) != HAL_OK) {  // ʹ��UART3�������ݣ���ʱʱ��500ms
        printf("[����] ����ʧ��\r\n");
        lastError = SENSOR_ERROR_TRANSMIT;
        return 1;
    }
    
    // �ȴ��������ݣ�ʹ��ȫ��rxBuffer
    HAL_StatusTypeDef rcvStatus = HAL_UART_Receive(&huart3, rxBuffer, 10, 1000);  // ʹ��UART3�������ݣ���ʱʱ��1000ms
    
    // ��ӡ����״̬������
    printf("[����] ");
    if(rcvStatus == HAL_OK) {
        // ���ճɹ�����ӡ����
        for(uint8_t i = 0; i < 10; i++) {
            printf("%02X ", rxBuffer[i]);
        }
        printf("\r\n");
        
        // ��֤����֡��ʽ
        if(rxBuffer[0] != 0x55 || rxBuffer[9] != 0xAA) {  // ���֡ͷ֡β
            printf("[����] ֡��ʽ����\r\n");
            lastError = SENSOR_ERROR_FORMAT;
            return 1;
        }
        
        if(rxBuffer[1] != cmd) {  // ���������ƥ��
            printf("[����] ���ƥ��: ����%02X, ʵ��%02X\r\n", cmd, rxBuffer[1]);
            lastError = SENSOR_ERROR_CMD;
            return 1;
        }
        
        // �����Ӧ�ֽ�(Byte2)
        switch(rxBuffer[2]) {
            case 0x01:  // ������ȷ��ִ��
                break;  // ��������
            case 0x02:
                printf("[����] ������ȷ��ִ�д���\r\n");
                lastError = SENSOR_ERROR_EXECUTE;
                return 1;
            case 0x03:
                printf("[����] �������\r\n");
                lastError = SENSOR_ERROR_CMD;
                return 1;
            case 0x04:
                printf("[����] �������Ϣ\r\n");
                lastError = SENSOR_ERROR_NO_ZERO;
                return 1;
            default:
                printf("[����] δ֪��Ӧ״̬��0x%02X\r\n", rxBuffer[2]);
                lastError = SENSOR_ERROR_RESPONSE;
                return 1;
        }
        
        // ��֤У���
        if(rxBuffer[8] != Sensor_CalculateChecksum(rxBuffer, 8)) {
            printf("[����] У��ʹ���\r\n");
            lastError = SENSOR_ERROR_CHECKSUM;
            return 1;
        }
        
        // ���״̬�ֽ�(Byte7)
        switch(rxBuffer[7]) {
            case 0x00:
                printf("[����] ������δ�궨\r\n");
                lastError = SENSOR_ERROR_NOT_CALIBRATED;
                return 1;
            case 0x01:
                // ���������У���������
                break;
            case 0x02:
                printf("[����] ���������ƽ����\r\n");
                lastError = SENSOR_ERROR_ZERO_SHIFTING;
                return 1;
            case 0x03:
                printf("[����] �������궨������\r\n");
                lastError = SENSOR_ERROR_CALIBRATING;
                return 1;
            case 0x04:
                printf("[����] ����������\r\n");
                lastError = SENSOR_ERROR_FAULT;
                return 1;
            default:
                printf("[����] δ֪������״̬��0x%02X\r\n", rxBuffer[7]);
                lastError = SENSOR_ERROR_UNKNOWN_STATUS;
                return 1;
        }
        
        return 0;  // ͨ�ųɹ�
    }
    else if(rcvStatus == HAL_TIMEOUT) {
        printf("���ճ�ʱ\r\n");
        lastError = SENSOR_ERROR_TIMEOUT;
    }
    else {
        printf("���մ���: %d\r\n", rcvStatus);
        lastError = SENSOR_ERROR_RECEIVE;
    }
    
    return 1;  // ͨ��ʧ��
}

/**
  * @brief  ������������Ӧ����
  * @retval ���������ʧ���򷵻�0xFFFFFFFF������32λֵ����0xFFFF������16λֵ��
  */
static uint32_t Sensor_ParseResponse(void)
{
  uint32_t value;
  
  // �������ʹ��32λ������4�ֽ����ݣ�
  // ��16λ��ǰ��Byte3-4������16λ�ں�Byte5-6��
  uint32_t lowWord = ((uint32_t)rxBuffer[3] << 8) |  // ��16λ���ֽ�
                    ((uint32_t)rxBuffer[4]);          // ��16λ���ֽ�
  uint32_t highWord = ((uint32_t)rxBuffer[5] << 8) | // ��16λ���ֽ�
                     ((uint32_t)rxBuffer[6]);         // ��16λ���ֽ�
  
  // ��ϳ�32λֵ
  value = (highWord << 16) | lowWord;  // ��ϣ���16λ�ڸ�λ����16λ�ڵ�λ
  
  // ��ӡ������Ϣ
  printf("[����] ���ݽ���: ����=0x%02X, Byte3-6: %02X %02X %02X %02X\r\n", 
         rxBuffer[1], rxBuffer[3], rxBuffer[4], rxBuffer[5], rxBuffer[6]);
  printf("[����] ��16λ: 0x%04X, ��16λ: 0x%04X, ��Ϻ�: 0x%08X\r\n", 
         (uint16_t)lowWord, (uint16_t)highWord, value);
  
  return value;  // ���ؽ�����������
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
  
  // �ۼ�ÿ���ֽ�
  for (uint8_t i = 0; i < length; i++) {
    sum += data[i];  // �ۼ�ÿ���ֽ�
  }
  
  // ȡ����1
  sum = (~sum) + 1;  // ����Э��Ҫ��ȡ����1
  
  return sum;  // �������յ�У���
}

/**
  * @brief  ��������Ƿ�����Ч��Χ��
  * @param  value: ����ֵ
  * @param  cmd: ��������
  * @retval �����: 1-��Ч, 0-��Ч
  */
static uint8_t Sensor_IsValueValid(uint32_t value, uint8_t cmd)
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
        printf("[����] ����ֵ��Ч: ����0\r\n");  // �޸Ĵ�����ʾ����ȷ
        return 0;
      }
      printf("[����] ��ǰ����ֵ: 0x%08X (%u)\r\n", value, value);  // ��ӵ�����Ϣ
      return 1;
      
    case SENSOR_CMD_READ_ZERO:
      return 1;  // �����������ֵ
      
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
      
    case SENSOR_ERROR_TRANSMIT:
      return "���ʹ���";  // ���ʹ���
      
    case SENSOR_ERROR_RECEIVE:
      return "���մ���";  // ���մ���
      
    case SENSOR_ERROR_FORMAT:
      return "֡��ʽ����";  // ֡��ʽ����
      
    case SENSOR_ERROR_CMD:
      return "���ƥ��";  // ���ƥ��
      
    case SENSOR_ERROR_STATUS:
      return "״̬�쳣";  // ״̬�쳣
      
    case SENSOR_ERROR_EXECUTE:
      return "����ִ�д���";  // ����ִ�д���
      
    case SENSOR_ERROR_NO_ZERO:
      return "�������Ϣ";  // �������Ϣ
      
    case SENSOR_ERROR_RESPONSE:
      return "δ֪��Ӧ״̬";  // δ֪��Ӧ״̬
      
    case SENSOR_ERROR_NOT_CALIBRATED:
      return "������δ�궨";  // ������δ�궨
      
    case SENSOR_ERROR_ZERO_SHIFTING:
      return "���������ƽ����";  // ���������ƽ����
      
    case SENSOR_ERROR_CALIBRATING:
      return "�������궨������";  // �������궨������
      
    case SENSOR_ERROR_FAULT:
      return "����������";  // ����������
      
    case SENSOR_ERROR_UNKNOWN_STATUS:
      return "δ֪������״̬";  // δ֪������״̬
      
    default:
      return "δ֪����";  // δ֪�������
  }
}

/**
  * @brief  ��鴫����ͨ��״̬
  * @retval 0: ͨ������, 1: ͨ��ʧ��
  */
uint8_t Sensor_CheckConnection(void)
{
    uint8_t retry = 0;  // ���Լ�����
    
    // ʹ�����Ի��Ƽ��ͨ��
    while (retry < SENSOR_MAX_RETRY) {
        // ���Ͷ�ȡ��������05ָ����ͨ�ţ�
        if (Sensor_SendCommand(SENSOR_CMD_READ_ZERO, NULL, 0) == 0) {
            // ������Ӧ����
            uint32_t response = Sensor_ParseResponse();
            
            // ֻҪ���յ���ȷ����Ӧ�ͱ�ʾͨ������
            if (response != 0xFFFFFFFF) {
                lastError = SENSOR_ERROR_NONE;  // �������״̬
                return 0;  // ͨ������
            }
        }
        
        retry++;  // �������Լ���
        HAL_Delay(SENSOR_RETRY_DELAY);  // ��ʱһ��ʱ�������
    }
    
    // ȫ������ʧ��
    lastError = SENSOR_ERROR_TIMEOUT;  // ���ó�ʱ����
    return 1;  // ͨ��ʧ��
} 

