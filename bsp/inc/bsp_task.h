#ifndef _BSP_TASK_H__
#define _BSP_TASK_H__
#include "cmsis_os.h"
#include "bsp_led.h"
#include "bsp_can.h"
#include "bsp_uart.h"
#include "bsp_network.h"





#define LED_TASK_STK_SIZE 128
#define LED_TASK_PRIO 4
extern TaskHandle_t LedTask_Handler;
void LedTask_Func(void *parameter);


#define CAN_TASK_STK_SIZE 1000
#define CAN_TASK_PRIO 3
extern TaskHandle_t CanTask_Handler;
void CanTask_Func(void *parameter);

#define NETWORK_TASK_STK_SIZE 512
#define NETWORK_TASK_PRIO 1
extern TaskHandle_t NetWorkTask_Handler;
void NetWorkTask_Func(void *parameter);


#define UART_TASK_STK_SIZE 1024
#define UART_TASK_PRIO 2
extern TaskHandle_t UartTask_Handler;
void UartTask_Func(void *parameter);


#define PROCESSDATA_TASK_STK_SIZE 1024
#define PROCESSDATA_TASK_PRIO 5
extern TaskHandle_t ProcessDataTask_Handler;
void ProcessData_Func(void *parameter);


#endif
