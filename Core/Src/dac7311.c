/**
  ******************************************************************************
  * @file    dac7311.c
  * @brief   DAC7311 ����Դ�ļ�
  *          ���ļ��ṩ��DAC7311 12λ��ģת�����Ŀ��ƹ���ʵ��
  ******************************************************************************
  * @attention
  *
  * DAC7311��һ��12λ����-ģ��ת������ʹ��GPIOģ��ʱ�����
  *
  ******************************************************************************
  */

/* �����ļ� */
#include "dac7311.h"
#include "gpio.h"
#include "main.h"
#include "stdio.h"  // ���printf֧��

/* ˽�б������� */
static uint8_t dac_initialized = 0;  // DAC��ʼ����־

// ���ڵ�������ĺ궨��
#define DEBUG_DAC7311 0  // ����Ϊ1���õ������������Ϊ0����

#if DEBUG_DAC7311
#define DAC_DEBUG(format, ...) printf("[DAC7311] " format "\r\n", ##__VA_ARGS__)
#else
#define DAC_DEBUG(format, ...)
#endif

// ˽�к�������
static uint8_t DAC7311_Write16Bits(uint16_t data);  // д��16λ����
static void DAC7311_Delay(void);                    // ����ʱ
static void DAC7311_DelayNs(uint32_t ns);          // ����ϵͳʱ�ӵľ�ȷ��ʱ

/**
  * @brief  ��ʼ��DAC7311
  * @retval 0: �ɹ�, 1: ʧ��
  */
uint8_t DAC7311_Init(void)
{
    if (dac_initialized) {  // ����Ѿ���ʼ����
        return 0;
    }

    // ��ʼ״̬����
    DAC_CLK_LOW();    // CLK��ʼΪ��
    DAC_DIN_LOW();    // DIN��ʼΪ��
    DAC_SYNC_HIGH();  // SYNC��ʼΪ�ߣ�δѡ�У�
    
    // �ϵ���ʱ
    HAL_Delay(10);
    
    // �����ϵ縴λֵ���������Ϊ0��
    if (DAC7311_SetValue(0) != 0) {
        return 1;
    }

    dac_initialized = 1;  // ��ǳ�ʼ�����
    return 0;
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
    data = (uint16_t)(value & 0x0FFF);  // ������12λ
    
    DAC_DEBUG("��ʼ��������: 0x%04X (ֵ: %d)", data, value);
    
    // ��ʼ����
    DAC_SYNC_LOW();          // ����SYNC����ʼ����
    DAC_DEBUG("SYNC���� -> ��ʼ����");
    DAC7311_DelayNs(20);    // tSYNC �� 20ns
    
    // ����16λ����
    if (DAC7311_Write16Bits(data) != 0) {
        DAC_SYNC_HIGH();
        DAC_DEBUG("����ʧ�ܣ�SYNC��ǰ���߻����ݲ�����");
        return 1;  // ����ʧ��
    }
    
    // ��������
    DAC7311_DelayNs(20);    // tSYNCH �� 20ns
    DAC_SYNC_HIGH();        // ����SYNC���������
    DAC_DEBUG("SYNC���� -> �������");
    
    // �ȴ�DAC�������
    DAC7311_DelayNs(8000);  // ת��ʱ�� �� 8��s
    DAC_DEBUG("DAC������ɣ����ֵ����Ϊ: %d", value);
    
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
  * @retval 0: �ɹ�, 1: ʧ��
  */
static uint8_t DAC7311_Write16Bits(uint16_t data)
{
    uint8_t i;
    uint8_t bit_count = 0;
    
    // ȷ��CLK��ʼΪ��
    DAC_CLK_LOW();
    DAC_DEBUG("CLK��ʼ��Ϊ�͵�ƽ");
    DAC7311_DelayNs(10);  // tCL �� 10ns
    
    DAC_DEBUG("��ʼ����16λ����: 0x%04X", data);
    
    // �����λ��ʼ����
    for(i = 0; i < 16; i++) {
        // ��������λ
        if(data & 0x8000) {
            DAC_DIN_HIGH();
            DAC_DEBUG("λ%2d: DIN=1 [CLK=L]", 15-i);
        } else {
            DAC_DIN_LOW();
            DAC_DEBUG("λ%2d: DIN=0 [CLK=L]", 15-i);
        }
        
        DAC7311_DelayNs(5);   // tDS �� 5ns�����ݽ���ʱ�䣩
        
        DAC_CLK_HIGH();       // CLK�����أ���������
        DAC_DEBUG("CLK������ -> ��������");
        DAC7311_DelayNs(10);  // tCH �� 10ns
        
        // ���SYNC�Ƿ���ǰ����
        if(HAL_GPIO_ReadPin(DAC_GPIO_PORT, DAC_SYNC_PIN) == GPIO_PIN_SET) {
            DAC_DEBUG("����SYNC��ǰ���ߣ���λ%d���ж�", 15-i);
            return 1;  // ����ʧ��
        }
        
        DAC_CLK_LOW();        // CLK�½���
        DAC_DEBUG("CLK�½��� -> ׼����һλ");
        DAC7311_DelayNs(10);  // tCL �� 10ns
        
        data <<= 1;
        bit_count++;
    }
    
    DAC_DEBUG("�������: ������%dλ", bit_count);
    
    return (bit_count == 16) ? 0 : 1;
}

/**
  * @brief  ����ϵͳʱ�ӵľ�ȷ��ʱ
  * @param  ns: ��Ҫ��ʱ��������
  * @retval None
  */
static void DAC7311_DelayNs(uint32_t ns)
{
    uint32_t cycles = (SystemCoreClock / 1000000) * ns / 1000;
    while(cycles--) {
        __NOP();
    }
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
  * @brief  ����DAC�����ѹ
  * @param  voltage: ���������ѹ(��λ:����)
  * @param  vref: �ο���ѹ(��λ:����)
  * @retval 0: �ɹ�, 1: ʧ��
  */
uint8_t DAC7311_SetVoltage(float voltage, float vref)
{
    uint16_t dac_value;
    
    /* ������� */
    if (voltage < 0 || voltage > vref) {
        return 1;  // ��ѹ������Χ
    }
    
    /* ����DAC��ֵ */
    dac_value = (uint16_t)((voltage * 4095) / vref);
    
    /* ����DACֵ */
    return DAC7311_SetValue(dac_value);
}


