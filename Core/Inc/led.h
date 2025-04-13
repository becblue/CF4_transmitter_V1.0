#ifndef __LED_H
#define __LED_H

#include "main.h"

/* LED引脚定义 */
#define LED1_PIN                GPIO_PIN_1        // LED1连接到PA1
#define LED1_GPIO_PORT          GPIOA             // LED1的GPIO端口
#define LED2_PIN                GPIO_PIN_2        // LED2连接到PA2
#define LED2_GPIO_PORT          GPIOA             // LED2的GPIO端口

/* LED状态定义 */
#define LED_ON                  GPIO_PIN_RESET    // LED点亮（低电平点亮）
#define LED_OFF                 GPIO_PIN_SET      // LED熄灭（高电平熄灭）

/* 系统状态定义 */
typedef enum {
    SYSTEM_INIT = 0,           // 系统初始化状态
    SYSTEM_WARMUP,             // 系统预热状态
    SYSTEM_WORKING,            // 系统工作状态
    SYSTEM_ERROR              // 系统错误状态
} SystemState_t;

/* 函数声明 */
void LED_Init(void);                             // LED初始化
void LED_SetState(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState state);  // 设置LED状态
void LED_Toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);                         // 翻转LED状态
void LED_UpdateState(SystemState_t state);       // 根据系统状态更新LED
void LED_HandleDAC7311Update(void);              // 处理DAC7311更新后的LED显示

#endif /* __LED_H */ 

