#ifndef __BSP_EVENT_H__
#define __BSP_EVENT_H__
#include "bsp_config.h"

#define EVENT_ARGUMENT_LEN 16
#define EVENT_BUF_LEN 10

//#define NOEVENT 0
#define EVENT_NOBUSSY 0
#define EVENT_BUSSY 1

typedef enum{NOEVENT=0,CAN_CALLME_EVENT,CAN_CAST_EVENT,MAXEVENT}Event_Type;

typedef struct{
	Event_Type type;//事件类别
	uint8_t argument[EVENT_ARGUMENT_LEN];
	uint8_t len;
}EventElement_Typedef;

typedef struct{
	uint8_t bussy;
	EventElement_Typedef buf[EVENT_BUF_LEN];
	uint8_t Event_cnt;
	uint8_t Evnet_index;
	
	
	
	
//设置事件
	
//查询事件
	
//删除事件


}Event_Typedef;


extern Event_Typedef Event_obj;
void Event_Init(Event_Typedef *obj);
uint8_t Evnet_Set(Event_Typedef *obj,Event_Type type,uint8_t *argument,uint8_t len);
uint8_t Event_Check(Event_Typedef *obj,Event_Type type,uint8_t *argument,uint8_t *len);
#endif

