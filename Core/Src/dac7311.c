/**
  ******************************************************************************
  * @file    dac7311.c
  * @brief   DAC7311 ����Դ�ļ�
  *          ���ļ��ṩ��DAC7311 12λ��ģת�����Ŀ��ƹ���ʵ��
  ******************************************************************************
  * @attention
  *
  * DAC7311是一�?12位数�?-模拟转换器，通过SPI接口控制
  *
  ******************************************************************************
  */

/* 包含头文�? */
#include "dac7311.h"
#include "main.h"

/* ˽�б������� */
static SPI_HandleTypeDef* dac_spi;  // SPI���
static GPIO_TypeDef* cs_port;       // Ƭѡ�˿�
static uint16_t cs_pin;             // Ƭѡ����

/**
  * @brief  ��ʼ��DAC7311
  * @param  hspi: SPI���ָ��
  * @param  CSPort: ƬѡGPIO�˿�
  * @param  CSPin: ƬѡGPIO����
  * @retval uint8_t: 0��ʾ�ɹ���1��ʾʧ��
  */
uint8_t DAC7311_Init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* CSPort, uint16_t CSPin)
{
    /* ����SPI��GPIO���� */
    dac_spi = hspi;
    cs_port = CSPort;
    cs_pin = CSPin;
    
    /* ��ʼ��ʱ����CS */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    
    /* ���ó�ʼ���Ϊ0����ʹ����������ģʽ */
    return DAC7311_SetValue(0);
}

/**
  * @brief  ����DAC���ֵ
  * @param  value: 12λDACֵ(0-4095)
  * @retval uint8_t: 0��ʾ�ɹ���1��ʾʧ��
  */
uint8_t DAC7311_SetValue(uint16_t value)
{
    uint16_t dac_cmd;
    HAL_StatusTypeDef status;
    
    /* �������뷶Χ */
    if(value > 4095) value = 4095;
    
    /* ���������� */
    dac_cmd = (DAC7311_CMD_WRITE_UPDATE_DAC << 12) | value;
    
    /* ����CS��ʼ���� */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    
    /* �������� */
    status = HAL_SPI_Transmit(dac_spi, (uint8_t*)&dac_cmd, 2, HAL_MAX_DELAY);
    
    /* ����CS�������� */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    
    return (status == HAL_OK) ? 0 : 1;
}

/**
  * @brief  ����DAC��Դģʽ
  * @param  mode: ��Դģʽ(��dac7311.h�еĺ궨��)
  * @retval uint8_t: 0��ʾ�ɹ���1��ʾʧ��
  */
uint8_t DAC7311_SetPowerMode(uint8_t mode)
{
    uint16_t dac_cmd;
    HAL_StatusTypeDef status;
    
    /* ���������֣����ֵ�ǰDACֵ���� */
    dac_cmd = (mode << 12);
    
    /* ����CS��ʼ���� */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    
    /* �������� */
    status = HAL_SPI_Transmit(dac_spi, (uint8_t*)&dac_cmd, 2, HAL_MAX_DELAY);
    
    /* ����CS�������� */
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    
    return (status == HAL_OK) ? 0 : 1;
}

/**
  * @brief  设置DAC输出电压
  * @param  voltage: 期望输出电压(单位:伏特)
  * @param  vref: 参考电�?(单位:伏特)
  * @retval 0: 成功, 1: 失败
  */
uint8_t DAC7311_SetVoltage(float voltage, float vref)
{
  uint16_t dac_value;
  
  /* 参数检�? */
  if (voltage < 0 || voltage > vref)
  {
    return 1; // 电压超出范围
  }
  
  /* ����DAC��ֵ */
  dac_value = (uint16_t)((voltage * 4095) / vref);
  
  /* ����DACֵ */
  return DAC7311_SetValue(dac_value);
}


