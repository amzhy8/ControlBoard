#ifndef _BSP_LED_H__
#define _BSP_LED_H__
#include "gpio.h"

#define LED_GPIO_PORT GPIOD
#define LED_GPIO_PIN GPIO_PIN_12

void LED_Control(uint8_t state);
void LED_Toggle(void);
void LED_Time_Callback(void);
void LedTask_Func(void *parameter);

extern uint32_t Led_Cunter;

#endif

