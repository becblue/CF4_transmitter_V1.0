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

// ˽�к�������
static void DAC7311_Write16Bits(uint16_t data);  // д��16λ����
static void DAC7311_Delay(void);                 // ����ʱ

/**
  * @brief  ��ʼ��DAC7311
  * @retval 0: �ɹ�, 1: ʧ��
  */
uint8_t DAC7311_Init(void)
{
    // ��ʼ״̬����
    DAC_CLK_LOW();    // CLK��ʼΪ��
    DAC_DIN_LOW();    // DIN��ʼΪ��
    DAC_SYNC_HIGH();  // SYNC��ʼΪ�ߣ�δѡ�У�
    
    // �ϵ���ʱ
    HAL_Delay(10);
    
    // �����ϵ縴λֵ���������Ϊ0��
    return DAC7311_SetValue(0);
}

/**
  * @brief  ����DAC���ֵ
  * @param  value: 12λDACֵ��0-4095��
  * @retval 0: �ɹ�, 1: ʧ��
  */
uint8_t DAC7311_SetValue(uint16_t value)
{
    uint16_t data;
    
    // �������뷶Χ
    if(value > 4095) {
        value = 4095;
    }
    
    // ����16λ����֡
    // [15:14]: PD1,PD0 = 00 (��������ģʽ)
    // [13:12]: ����λ = 00
    // [11:0]: 12λDAC����
    data = (uint16_t)(value & 0x0FFF);  // ȡ��12λ
    
    // ��ʼ����
    DAC_SYNC_LOW();   // ����SYNC����ʼ����
    DAC7311_Delay();  // ����ʱ
    
    // ����16λ����
    DAC7311_Write16Bits(data);
    
    // ��������
    DAC7311_Delay();  // ����ʱ
    DAC_SYNC_HIGH();  // ����SYNC����������
    
    return 0;
}

/**
  * @brief  �������ģʽ
  * @retval None
  */
void DAC7311_PowerDown(void)
{
    uint16_t data = 0x2000;  // PD1,PD0 = 10 (����ģʽ1)
    
    DAC_SYNC_LOW();
    DAC7311_Delay();
    DAC7311_Write16Bits(data);
    DAC7311_Delay();
    DAC_SYNC_HIGH();
}

/**
  * @brief  �˳�����ģʽ
  * @retval None
  */
void DAC7311_PowerUp(void)
{
    uint16_t data = 0x0000;  // PD1,PD0 = 00 (��������ģʽ)
    
    DAC_SYNC_LOW();
    DAC7311_Delay();
    DAC7311_Write16Bits(data);
    DAC7311_Delay();
    DAC_SYNC_HIGH();
}

/**
  * @brief  д��16λ����
  * @param  data: Ҫд���16λ����
  * @retval None
  */
static void DAC7311_Write16Bits(uint16_t data)
{
    uint8_t i;
    
    // �����λ��ʼ����
    for(i = 0; i < 16; i++) {
        DAC_CLK_LOW();  // CLK�͵�ƽ
        
        // ��������λ
        if(data & 0x8000) {
            DAC_DIN_HIGH();  // ����1
        } else {
            DAC_DIN_LOW();   // ����0
        }
        
        DAC7311_Delay();  // ����ʱ��
        DAC_CLK_HIGH();   // CLK�����أ���������
        DAC7311_Delay();  // ����ʱ��
        
        data <<= 1;  // ����һλ��׼��������һλ
    }
    
    DAC_CLK_LOW();  // ���������CLK�ص��͵�ƽ
}

/**
  * @brief  ����ʱ����
  * @retval None
  */
static void DAC7311_Delay(void)
{
    uint8_t i;
    for(i = 0; i < 10; i++) {
        __NOP();  // �ղ���
    }
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


