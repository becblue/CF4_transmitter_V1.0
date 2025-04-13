#ifndef __LED_H
#define __LED_H

#include "main.h"

/* LED���Ŷ��� */
#define LED1_PIN                GPIO_PIN_1        // LED1���ӵ�PA1
#define LED1_GPIO_PORT          GPIOA             // LED1��GPIO�˿�
#define LED2_PIN                GPIO_PIN_2        // LED2���ӵ�PA2
#define LED2_GPIO_PORT          GPIOA             // LED2��GPIO�˿�

/* LED״̬���� */
#define LED_ON                  GPIO_PIN_RESET    // LED�������͵�ƽ������
#define LED_OFF                 GPIO_PIN_SET      // LEDϨ�𣨸ߵ�ƽϨ��

/* ϵͳ״̬���� */
typedef enum {
    SYSTEM_INIT = 0,           // ϵͳ��ʼ��״̬
    SYSTEM_WARMUP,             // ϵͳԤ��״̬
    SYSTEM_WORKING,            // ϵͳ����״̬
    SYSTEM_ERROR              // ϵͳ����״̬
} SystemState_t;

/* �������� */
void LED_Init(void);                             // LED��ʼ��
void LED_SetState(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState state);  // ����LED״̬
void LED_Toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);                         // ��תLED״̬
void LED_UpdateState(SystemState_t state);       // ����ϵͳ״̬����LED
void LED_HandleDAC7311Update(void);              // ����DAC7311���º��LED��ʾ

#endif /* __LED_H */ 

