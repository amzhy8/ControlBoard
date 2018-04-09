#ifndef _BSP_UART_H__
#define _BSP_UART_H__

#include "usart.h"
#include "cmsis_os.h"

#define UART_SUCCESS 0
#define UART_FAIL 1

#define UART_RECIEVE_FIFO_LEN 256




void Bsp_UartInit(void);
void Uart1_Send(uint8_t *buf,uint16_t len);
void Uart_Receive_Start(UART_HandleTypeDef *huart);
void UartTask_Func(void *parameter);
void UartCmdReback(uint8_t status);

extern xQueueHandle UartRecieveFifo;
extern uint8_t uart_rxdata;

#endif

