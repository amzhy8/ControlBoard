#include "bsp_event.h"
#include <string.h>

Event_Typedef Event_obj;




void Event_Init(Event_Typedef *obj){
	uint8_t i;
	obj->bussy=EVENT_NOBUSSY;
	obj->Event_cnt=0;
	obj->Evnet_index=0;
	for(i=0;i<EVENT_BUF_LEN;i++){
		obj->buf->len=0;
		obj->buf->type=NOEVENT;
		memset(obj->buf->argument,0,EVENT_ARGUMENT_LEN);
	}
}

//设置事件

uint8_t Evnet_Set(Event_Typedef *obj,Event_Type type,uint8_t *argument,uint8_t len){
	uint8_t i=0;
	if((type==NOEVENT)||(type>MAXEVENT))return BSP_ERROR;
	if(obj->Event_cnt>=EVENT_BUF_LEN)return BSP_ERROR;
	for(i=0;i<EVENT_BUF_LEN;i++){
		if(obj->buf[i].type==NOEVENT){
			obj->bussy=EVENT_NOBUSSY;
			obj->buf[i].type=type;
			obj->buf[i].len=len;
			memcpy(obj->buf[i].argument,argument,len);
			obj->Event_cnt++;
			return BSP_SUCCESS;
		}
	}
	return BSP_ERROR;
}
	
//查询事件
uint8_t Event_Check(Event_Typedef *obj,Event_Type type,uint8_t *argument,uint8_t *len){
	uint8_t i;
	for(i=0;i<EVENT_BUF_LEN;i++){
		if(obj->buf[i].type==type){
			obj->bussy=EVENT_BUSSY;
			*len=obj->buf[i].len;
			memcpy(argument,obj->buf[i].argument,obj->buf[i].len);
			obj->Event_cnt--;	
			obj->buf[i].type=NOEVENT;		
			obj->bussy=EVENT_NOBUSSY;
			return BSP_SUCCESS;
		}
	}
	return BSP_ERROR;
}
