#ifndef _BSP_CAN_H__
#define _BSP_CAN_H__
#include "can.h"


#define BSP_CAN_FALSE 1
#define BSP_CAN_SUCCESS 0


#define CAN_INT_FLAG_TRUE 1
#define CAN_INT_FLAG_FALSE 0


#define EVENT_TURE 1
#define EVENT_FLASE 0

#define CAN_ADDR_CAST 0x00000700



#define CAN_SEND_HEAD 0xa5



typedef struct{
	CAN_HandleTypeDef *can_ptr;
	CAN_FilterConfTypeDef  				sFilterConfig;
	CanTxMsgTypeDef        TxMessage;
	CanRxMsgTypeDef        RxMessage;
	uint32_t Can_Address;
	uint8_t CanIntFlag;
}Bsp_Can_Typedef;




typedef struct{
	uint8_t address;
	uint16_t adc_value;
	uint8_t  source_switch;
	uint8_t channel_switch;
}ChannelBoard_Typedef;

extern ChannelBoard_Typedef ChannelBoard[60];

extern Bsp_Can_Typedef Bsp_Can_obj;
extern uint8_t CanSendEventFlag;
extern uint8_t CanSendBuf[20];


void CanInit(Bsp_Can_Typedef *obj,CAN_HandleTypeDef *can_ptr);
uint8_t Can_Send(Bsp_Can_Typedef *obj,uint16_t id,uint8_t *buf,uint8_t len);
uint8_t Can_Receive(Bsp_Can_Typedef *obj,uint8_t *buf,uint8_t *len);
uint8_t Can_Server(void);
uint8_t CanSendCtrlCmd(uint16_t boardaddr,uint8_t channelvale,uint8_t sourcetype,uint16_t adcvalue);

#endif

