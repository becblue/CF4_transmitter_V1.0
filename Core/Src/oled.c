/**
  ******************************************************************************
  * @file    oled.c
  * @brief   OLED��ʾ����Դ�ļ�
  ******************************************************************************
  * @attention
  *
  * ���ļ�ʵ����OLED��ʾ������������
  * ʹ��I2C1ͨ�ţ�֧���������»��ƣ��Ż�ˢ��Ч��
  * SSD1306�������������ֱ���128x64
  *
  ******************************************************************************
  */

/* �����ļ� ------------------------------------------------------------------*/
#include "oled.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>  // �����ѧ��ͷ�ļ����ṩpow����
#include "oledfont.h"  // ����ⶨ��

/* �������� ------------------------------------------------------------------*/
// 8x16�������ݣ�ASCII���ַ������ӿո�(0x20)��ʼ��
extern const unsigned char F8X16[];

// 6x8�������� 
extern const unsigned char F6x8[][6];

/* OLED��ʼ���������� */
static const uint8_t OLED_Init_CMD[] = {
    0xAE,   // �ر���ʾ
    0xD5,   // ������ʾʱ�ӷ�Ƶ��/����Ƶ��
    0x80,   // ���÷�Ƶ�ȣ�����ʱ��Ϊ100֡/��
    0xA8,   // ���ö�·������
    0x3F,   // 1/64 duty
    0xD3,   // ������ʾƫ��
    0x00,   // ��ƫ��
    0x40,   // ������ʾ��ʼ�� [5:0]
    0x8D,   // ���ó���
    0x14,   // ʹ�ܳ���
    0x20,   // �����ڴ�Ѱַģʽ
    0x02,   // ҳѰַģʽ
    0xA1,   // ���ض������ã�0xA1�����������ң���0xA0���ҷ���
    0xC8,   // ��ɨ��˳��0xC8���������ϵ��£���0xC0���·���
    0xDA,   // ����COM����Ӳ������
    0x12,   // ���ñ���COM��������
    0x81,   // ���öԱȶ�
    0xCF,   // �Աȶ�ֵ
    0xD9,   // ����Ԥ�������
    0xF1,   // ����Ԥ���
    0xDB,   // ����VCOMHȡ��ѡ�񼶱�
    0x30,   // Լ0.83xVcc
    0xA4,   // ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
    0xA6,   // ������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ
    0xAF    // ������ʾ
};

/* ˽�����Ͷ��� --------------------------------------------------------------*/

/* ˽�к궨�� ----------------------------------------------------------------*/

/* ˽�б��� ------------------------------------------------------------------*/
static OLED_DirtyFlag_t oledDirtyFlag;  // �������
static I2C_HandleTypeDef *oled_hi2c;    // I2C���

/* ˽�к���ԭ�� --------------------------------------------------------------*/
static uint8_t OLED_WriteCmd(uint8_t command);
static uint8_t OLED_WriteData(uint8_t data);
static void OLED_SetPosition(uint8_t x, uint8_t y);
static void OLED_MarkDirty(uint8_t page);
static void OLED_DrawChar(uint8_t x, uint8_t y, char chr);

/* ����ʵ�� ------------------------------------------------------------------*/

/**
  * @brief  OLED��ʼ��
  * @param  hi2c: I2C���ָ��
  * @retval ��ʼ�����: 0-�ɹ�, 1-ʧ��
  */
uint8_t OLED_Init(I2C_HandleTypeDef *hi2c)
{
    oled_hi2c = hi2c;  // ����I2C���
    
    // ��ʼ����ʾ���������������
    memset(&oledDirtyFlag, 0, sizeof(OLED_DirtyFlag_t));
    for(uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
        oledDirtyFlag.isDirty[i] = 1;  // ��ʼʱ�������ҳ��Ϊ����
    }
    
    HAL_Delay(100);    // �ȴ�OLED�ϵ��ȶ�
    
    /* ���ͳ�ʼ���������� */
    for(uint8_t i = 0; i < sizeof(OLED_Init_CMD); i++)
    {
        if (OLED_WriteCmd(OLED_Init_CMD[i]) != 0) {  // ���д������ʧ��
            return 1;  // ���ش���
        }
        HAL_Delay(1);  // ÿ���������Ӷ�����ʱ
    }
    
    /* ����������ˢ����ʾ */
    OLED_Clear();
    OLED_Refresh();  // ����ˢ����ʾ
    
    return 0;  // ��ʼ���ɹ�
}

/**
  * @brief  OLED����
  * @retval ��
  */
void OLED_Clear(void)
{
    // �����ʾ������
    memset(oledDirtyFlag.displayBuffer, 0, sizeof(oledDirtyFlag.displayBuffer));
    
    // �������ҳΪ����
    for(uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
        oledDirtyFlag.isDirty[i] = 1;
    }
}

/**
  * @brief  OLEDˢ����ʾ(����������)
  * @retval ��
  */
void OLED_Refresh(void)
{
    uint8_t i;
    
    for(i = 0; i < DIRTY_BLOCK_NUM; i++) {
        if(oledDirtyFlag.isDirty[i]) {
            OLED_SetPosition(0, i);  // ����λ�õ�ҳ�Ŀ�ʼ
            
            // ���͸�ҳ����������
            for(uint8_t j = 0; j < DIRTY_BLOCK_SIZE; j++) {
                OLED_WriteData(oledDirtyFlag.displayBuffer[i][j]);
            }
            
            oledDirtyFlag.isDirty[i] = 0;  // �������
        }
    }
}

/**
  * @brief  ����OLED��ʾ
  * @param  None
  * @retval None
  */
void OLED_Display_On(void)
{
    OLED_WriteCmd(OLED_CMD_DISPLAY_ON);
}

/**
  * @brief  �ر�OLED��ʾ
  * @param  None
  * @retval None
  */
void OLED_Display_Off(void)
{
    OLED_WriteCmd(OLED_CMD_DISPLAY_OFF);
}

/**
  * @brief  ����OLED��ʾλ��
  * @param  x: �����꣬��Χ0~127
  * @param  y: ҳ���꣬��Χ0~7
  * @retval None
  */
static void OLED_SetPosition(uint8_t x, uint8_t y)
{
    OLED_WriteCmd(0xB0 + y);                   // ����ҳ��ַ
    OLED_WriteCmd(0x00 + (x & 0x0F));         // �����е�ַ��4λ
    OLED_WriteCmd(0x10 + ((x >> 4) & 0x0F));  // �����е�ַ��4λ
}

/**
  * @brief  OLED��ʾһ���ַ�
  * @param  x: �����꣬��Χ0~127
  * @param  y: ҳ���꣬��Χ0~7
  * @param  chr: Ҫ��ʾ���ַ�
  * @retval None
  */
void OLED_ShowChar(uint8_t x, uint8_t y, char chr)
{
    uint8_t c = 0;
    c = chr - ' ';    // �õ��ַ���ƫ����
    
    if(x > OLED_WIDTH - 8) { x = 0; y = y + 2; }  // �Զ����У���Ϊ��16���ظ߶ȣ�����y��2
    if(y > OLED_PAGE_NUM - 2) { y = 0; }          // ҳ�����ã�Ԥ����ҳ�߶�
    
    // д��8x16�������ݵ���ʾ������
    for(uint8_t i = 0; i < 8; i++) {
        oledDirtyFlag.displayBuffer[y][x + i] = F8X16[c * 16 + i];         // д���ϰ벿��
        oledDirtyFlag.displayBuffer[y + 1][x + i] = F8X16[c * 16 + i + 8]; // д���°벿��
    }
    
    // �������ҳΪ����
    oledDirtyFlag.isDirty[y] = 1;
    oledDirtyFlag.isDirty[y + 1] = 1;
}

/**
  * @brief  OLED��ʾ�ַ���
  * @param  x: �����꣬��Χ0~127
  * @param  y: ҳ���꣬��Χ0~7
  * @param  str: Ҫ��ʾ���ַ���
  * @retval None
  */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str)
{
    uint8_t j = 0;
    while (str[j] != '\0')
    {
        OLED_ShowChar(x, y, str[j]);
        x += 8;  // 8x16���壬���Ϊ8
        if(x > OLED_WIDTH - 8)
        {
            x = 0;
            y += 2;  // 16���ظ߶ȣ���Ҫ������ҳ
        }
        if(y > OLED_PAGE_NUM - 2) { y = 0; }  // ��y������Χʱ����Ϊ0
        j++;
    }
}

/**
  * @brief  ����OLED��ʾ�Աȶ�
  * @param  contrast: �Աȶ�ֵ����Χ0x00-0xFF
  *         0x00���0xFF������Ĭ��ֵ0xCF
  * @retval None
  */
void OLED_SetContrast(uint8_t contrast)
{
    OLED_WriteCmd(OLED_CMD_SET_CONTRAST);  // ���öԱȶ�����
    OLED_WriteCmd(contrast);               // �Աȶ�ֵ
}

/**
  * @brief  OLED��ʾ����
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  num: Ҫ��ʾ������
  * @param  len: ���ֳ���
  * @retval ��
  */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
  uint8_t t, temp;
  uint8_t enshow = 0;
  
  // ������
  if (y > 7) return;
  
  // ҳ���Ϊ����
  OLED_MarkDirty(y);
  OLED_MarkDirty(y + 1);  // ��Ϊʹ��8x16���壬��Ҫ�����ҳ
  
  // ��λ��ʾ����
  for (t = 0; t < len; t++) {
    // ʹ�������������pow��������������ת������
    uint32_t divisor = 1;
    for (uint8_t i = 0; i < (len - t - 1); i++) {
      divisor *= 10;
    }
    temp = (num / divisor) % 10;
    
    if (enshow == 0 && t < (len - 1)) {
      if (temp == 0) {
        OLED_DrawChar(x + 8 * t, y, ' ');
        continue;
      } else {
        enshow = 1;
      }
    }
    OLED_DrawChar(x + 8 * t, y, temp + '0');
  }
}

/**
  * @brief  OLED��ʾ������
  * @param  x: ��ʼx����(0-127)
  * @param  y: ��ʼҳ����(0-7)
  * @param  num: Ҫ��ʾ�ĸ�����
  * @param  intLen: �������ֳ���
  * @param  fracLen: С�����ֳ���
  * @param  size: �����С
  * @retval ��
  */
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t intLen, uint8_t fracLen, uint8_t size)
{
  uint32_t intPart;  // ��������
  uint32_t fracPart; // С������
  
  // ������
  if (y > 7) return;
  
  // ҳ���Ϊ����
  OLED_MarkDirty(y);
  
  // �����16���ظ����壬������һҳΪ����
  if (size == 16 && y < 7) {
    OLED_MarkDirty(y + 1);
  }
  
  // ������
  if (num < 0) {
    OLED_DrawChar(x, y, '-');
    x += 8;
    num = -num;
  }
  
  // ����������С������
  intPart = (uint32_t)num;
  
  // ����10��fracLen�η�������ʹ��pow����
  uint32_t multiplier = 1;
  for (uint8_t i = 0; i < fracLen; i++) {
    multiplier *= 10;
  }
  fracPart = (uint32_t)((num - intPart) * multiplier);
  
  // ��ʾ��������
  OLED_ShowNum(x, y, intPart, intLen);
  
  // ��ʾС����
  OLED_DrawChar(x + intLen * 8, y, '.');
  
  // ��ʾС������
  OLED_ShowNum(x + (intLen + 1) * 8, y, fracPart, fracLen);
}

/**
  * @brief  OLED��ʾ����������
  * @param  zeroPoint: ���ֵ
  * @param  rangeValue: ����ֵ
  * @param  concentration: Ũ��ֵ
  * @param  outputValue: ���ֵ(0-4095)
  * @param  outputVoltage: �����ѹ(0-3.3V)
  * @param  outputCurrent: �������(4-20mA)
  * @retval ��
  */
void OLED_ShowValue(uint16_t zeroPoint, uint16_t rangeValue, uint16_t concentration, 
                   uint16_t outputValue, float outputVoltage, float outputCurrent)
{
  char buffer[20]; // �ַ���������
  static uint8_t currentPage = 0; // ��ǰ��ʾҳ�棬0-����������1-�������
  static uint32_t lastToggleTime = 0; // �ϴ��л�ʱ��
  uint32_t currentTime = HAL_GetTick(); // ��ȡ��ǰʱ��
  
  // ÿ3���Զ��л�ҳ��
  if(currentTime - lastToggleTime > 3000) {
    currentPage = (currentPage + 1) % 2; // ѭ���л�ҳ��
    lastToggleTime = currentTime; // �����л�ʱ��
    
    // �л�ҳ��ʱ����
    for (uint8_t i = 0; i < DIRTY_BLOCK_NUM; i++) {
      memset(oledDirtyFlag.displayBuffer[i], 0, 128); // �����ʾ������
      oledDirtyFlag.isDirty[i] = 1; // ���Ϊ����
    }
  }
  
  // ��ʾ�������������̶���ʾ��
  OLED_ShowString(0, 0, "CF4 Sensor System"); // ��ʾϵͳ����
  
  // ��ʾ���߷ָ���
  for(uint8_t i = 0; i < 128; i++) {
    oledDirtyFlag.displayBuffer[1][i] = 0x80; // ����ˮƽ��
  }
  oledDirtyFlag.isDirty[1] = 1; // ���Ϊ����
  
  // ���ݵ�ǰҳ����ʾ��ͬ����
  if(currentPage == 0) { // ��һҳ����������
    // ��ʾҳ�����
    OLED_ShowString(0, 1, "Basic Parameters");
    
    // Ũ��ֵ����������ʾ��
    OLED_ShowString(0, 2, "Concentration:"); // ��ʾŨ�ȱ���
    sprintf(buffer, "%5d", concentration); // ��ʽ��Ũ��ֵ
    OLED_ShowString(0, 3, buffer); // ��ʾŨ����ֵ
    
    // ��ʾ�淶ֵָʾ
    if (concentration >= rangeValue) {
      OLED_ShowString(64, 3, "[MAX]"); // ����������ʾ���ֵ��־
    } else if (concentration <= zeroPoint) {
      OLED_ShowString(64, 3, "[MIN]"); // ���������ʾ��Сֵ��־
    } else {
      // ����Ũ�Ȱٷֱ�
      uint8_t percent = 0;
      if (rangeValue > zeroPoint) {
        percent = (uint8_t)(((float)(concentration - zeroPoint) / (rangeValue - zeroPoint)) * 100);
      }
      sprintf(buffer, "[%d%%]", percent); // ��ʽ���ٷֱ�
      OLED_ShowString(64, 3, buffer); // ��ʾ�ٷֱ�
    }
    
    // ��ʾ��������
    OLED_ShowString(0, 4, "Zero Point:"); // ��ʾ������
    sprintf(buffer, "%d", zeroPoint); // ��ʽ�����ֵ
    OLED_ShowString(72, 4, buffer); // ��ʾ�����ֵ
    
    OLED_ShowString(0, 5, "Range Value:"); // ��ʾ���̱���
    sprintf(buffer, "%d", rangeValue); // ��ʽ������ֵ
    OLED_ShowString(72, 5, buffer); // ��ʾ������ֵ
    
    // ��ʾ���ֵ
    OLED_ShowString(0, 6, "Output Value:"); // ��ʾ���ֵ����
    sprintf(buffer, "%d", outputValue); // ��ʽ�����ֵ
    OLED_ShowString(72, 6, buffer); // ��ʾ�����ֵ
    
    // ��ʾ������
    uint8_t progressPercent = 0;
    if (outputValue > 4095) {
      progressPercent = 100; // ������Χ��ʾ100%
    } else {
      progressPercent = (uint8_t)((float)outputValue * 100 / 4095); // ������Ȱٷֱ�
    }
    OLED_ShowProgress(0, 7, 100, progressPercent); // ��ʾ���ֵ������
  } 
  else { // �ڶ�ҳ���������
    // ��ʾҳ�����
    OLED_ShowString(0, 1, "Output Details");
    
    // ��ʾ�������ֵ
    OLED_ShowString(0, 2, "Digital Value:"); // ��ʾ�����������
    sprintf(buffer, "%d", outputValue); // ��ʽ���������ֵ
    OLED_ShowString(72, 2, buffer); // ��ʾ���������ֵ
    
    // ��ͼ�η�ʽ��ʾ��ǰ���ֵ(��������ͼ)
    OLED_ShowString(0, 3, "Output Level:"); // ��ʾ�����ƽ����
    uint8_t barWidth = (outputValue > 4095) ? 100 : (uint8_t)((float)outputValue * 100 / 4095); // �������ο��
    
    for(uint8_t i = 0; i < 100; i++) {
      if(i < barWidth) {
        oledDirtyFlag.displayBuffer[4][i + 14] = 0xFF; // ���Ĳ���
      } else {
        oledDirtyFlag.displayBuffer[4][i + 14] = 0x81; // δ���Ĳ���(�߿�)
      }
    }
    oledDirtyFlag.isDirty[4] = 1; // ���Ϊ����
    
    // ��ʾ��ѹ���
    OLED_ShowString(0, 5, "Voltage Output:"); // ��ʾ��ѹ�������
    sprintf(buffer, "%.2f V", outputVoltage); // ��ʽ����ѹֵ
    OLED_ShowString(72, 5, buffer); // ��ʾ��ѹ��ֵ
    
    // ��ʾ�������
    OLED_ShowString(0, 6, "Current Output:"); // ��ʾ�����������
    sprintf(buffer, "%.2f mA", outputCurrent); // ��ʽ������ֵ
    OLED_ShowString(72, 6, buffer); // ��ʾ������ֵ
    
    // ��ʾ״ָ̬ʾ
    OLED_ShowString(0, 7, "Status:"); // ��ʾ״̬����
    if (outputCurrent < 4.0f) {
      OLED_ShowString(48, 7, "Under Range"); // ���ڷ�Χ
    } else if (outputCurrent > 20.0f) {
      OLED_ShowString(48, 7, "Over Range"); // ������Χ
    } else {
      OLED_ShowString(48, 7, "Normal"); // ������Χ
    }
  }
  
  // ˢ����ʾ
  OLED_Refresh(); // ������Ļ��ʾ
}

/**
  * @brief  ��ʾϵͳ����״̬
  * @param  status: ״̬��
  * @param  errorCode: ������
  * @retval ��
  */
void OLED_ShowStatus(uint8_t status, uint8_t errorCode)
{
  // ��յ�0ҳ�͵�1ҳ
  memset(oledDirtyFlag.displayBuffer[0], 0, 128);
  memset(oledDirtyFlag.displayBuffer[1], 0, 128);
  oledDirtyFlag.isDirty[0] = 1;
  oledDirtyFlag.isDirty[1] = 1;
  
  // ��ʾϵͳ״̬
  OLED_ShowString(0, 0, "System Status:");
  
  // ����״̬����ʾ��ͬ״̬
  switch (status) {
    case 0:  // ������
      OLED_ShowString(0, 1, "Starting...");
      break;
    case 1:  // Ԥ����
      OLED_ShowString(0, 1, "Warming up...");
      break;
    case 2:  // ��������
      OLED_ShowString(0, 1, "Running");
      break;
    case 3:  // ����״̬
      OLED_ShowString(0, 1, "Error:");
      // ��ʾ�������
      char errStr[8];
      sprintf(errStr, "0x%02X", errorCode);
      OLED_ShowString(64, 1, errStr);
      break;
    default:
      OLED_ShowString(0, 1, "Unknown");
      break;
  }
  
  // ˢ����ʾ
  OLED_Refresh();
}

/**
 * @brief  ��ʾ������
 * @param  x: ��ʼ������
 * @param  y: ��ʼ������
 * @param  max_value: ���ֵ
 * @param  value: ��ǰֵ(0-100)
 * @retval ��
 */
void OLED_ShowProgress(uint8_t x, uint8_t y, uint8_t max_value, uint8_t value)
{
    uint8_t i;
    uint8_t length = 100;
    uint8_t progress;
    
    // �������ֵ
    if (value > max_value) {
        progress = length;
    } else {
        progress = (value * length) / max_value;
    }
    
    // ���ƽ������߿�
    OLED_ShowChar(x, y, '[');
    OLED_ShowChar(x + length/6 + 1, y, ']');
    
    // ���ƽ������ڲ����
    for (i = 0; i < length/6; i++) {
        if (i < progress/6) {
            OLED_ShowChar(x + i + 1, y, '=');
        } else {
            OLED_ShowChar(x + i + 1, y, ' ');
        }
    }
    
    // ��ʾ�ٷֱ�
    char buffer[10];
    sprintf(buffer, "%3d%%", value);
    OLED_ShowString(x + length/6 + 3, y, buffer);
    
    // ���ø���Ϊ������
    oledDirtyFlag.isDirty[y] = 1;
}

/**
  * @brief  ���ҳΪ����
  * @param  page: ҳ��(0-7)
  * @retval ��
  */
static void OLED_MarkDirty(uint8_t page)
{
  if (page < DIRTY_BLOCK_NUM) {
    oledDirtyFlag.isDirty[page] = 1;
  }
}

/**
  * @brief  �����ַ�
  * @param  x: �е�ַ(0-127)
  * @param  y: ҳ��ַ(0-7)
  * @param  chr: Ҫ��ʾ���ַ�
  * @retval ��
  */
static void OLED_DrawChar(uint8_t x, uint8_t y, char chr)
{
  uint8_t c = 0, i = 0;
  
  // ���ַ�ASCII��תΪ�ֿ�����
  c = chr - ' ';
  
  // ʹ��8x16����
  for (i = 0; i < 8; i++) {
    oledDirtyFlag.displayBuffer[y][x + i] = F8X16[c * 16 + i];
    oledDirtyFlag.displayBuffer[y + 1][x + i] = F8X16[c * 16 + i + 8];
  }
}

/**
 * @brief  ��OLEDд������
 * @param  command: Ҫд�������
 * @retval 0: �ɹ�, 1: ʧ��
 */
uint8_t OLED_WriteCmd(uint8_t command)
{
    uint8_t data[2];                // �������ݻ�����
    data[0] = OLED_CMD;            // ��һ���ֽڱ�ʾд����
    data[1] = command;             // �ڶ����ֽ��Ǿ��������ֵ
    
    // ͨ��I2C���������ʱʱ������Ϊ100ms
    if (HAL_I2C_Master_Transmit(oled_hi2c, OLED_ADDR, data, 2, 100) != HAL_OK)
    {
        return 1;                   // ����ʧ�ܷ���1
    }
    
    return 0;                       // ���ͳɹ�����0
}

/**
 * @brief  ��OLEDд������
 * @param  data: Ҫд�������
 * @retval 0: �ɹ�, 1: ʧ��
 */
uint8_t OLED_WriteData(uint8_t data)
{
    uint8_t buf[2];                // �������ݻ�����
    buf[0] = OLED_DATA;           // ��һ���ֽڱ�ʾд����
    buf[1] = data;                // �ڶ����ֽ��Ǿ��������ֵ
    
    // ͨ��I2C�������ݣ���ʱʱ������Ϊ100ms
    if (HAL_I2C_Master_Transmit(oled_hi2c, OLED_ADDR, buf, 2, 100) != HAL_OK)
    {
        return 1;                  // ����ʧ�ܷ���1
    }
    
    return 0;                      // ���ͳɹ�����0
}


