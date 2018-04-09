#ifndef __BSP_CANPROTOCOL_H__
#define __BSP_CANPROTOCOL_H__

#include "stm32f4xx_hal.h"
#include "bsp_config.h"

#define PROTOCOL_SUCCESS BSP_SUCCESS
#define PROTOCOL_ERROR   BSP_ERROR
#define AFRAME_HEAD 0xa5
#define AFRAME_LEN

typedef struct{
	uint8_t head;
	uint8_t len;
	uint8_t cmd;
	uint8_t data[128];
	uint8_t parity;
}AFrame_Typedef;

extern AFrame_Typedef AFrame;
extern AFrame_Typedef AFrame_Net;




typedef enum{HEAD_CHECK=0,LEN_CHECK,CMD_CHECK,DATA_CHECK,PARITY_CHECK}ProtocolStatus_Typedef;
typedef enum{AFRAME=0,BFRAME}ProtocolType_Typedef;

typedef struct{
	ProtocolStatus_Typedef status;
	void *frame;
}Protocol_Typedef;

uint8_t CanFrameAnasyse(ProtocolType_Typedef type,uint8_t *buf,uint8_t len);
uint8_t CheckFrame(uint8_t type,void *frame,uint8_t *data);

#endif

