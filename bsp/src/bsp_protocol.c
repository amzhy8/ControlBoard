#include "bsp_protocol.h"




static uint16_t Aframe_index=0;
static Protocol_Typedef Protocol;
AFrame_Typedef AFrame;
AFrame_Typedef AFrame_Net;



//协议初始化
uint8_t ProtocolInit(void *frame){
	Protocol.frame=frame;//注册需要帧输出对象
	return PROTOCOL_SUCCESS;
}





//检测协议帧
uint8_t CheckFrame(uint8_t type,void *frame,uint8_t *data){
	//uint8_t data;
	uint8_t temp;
	uint8_t i;
	AFrame_Typedef *aframe=(AFrame_Typedef*)frame;
	if(AFRAME==type){
		
		switch(Protocol.status){
			case HEAD_CHECK:
				if(*data==AFRAME_HEAD){
					aframe->head=*data;
					Protocol.status=LEN_CHECK;
				}
				break;
			case LEN_CHECK:
					aframe->len=*data;
					Protocol.status=CMD_CHECK;
				break;
			case CMD_CHECK:
					aframe->cmd=*data;
					Aframe_index=0;
					if(aframe->len==0){
						Protocol.status=PARITY_CHECK;
					}else{
						Protocol.status=DATA_CHECK;
					}
				break;
			case DATA_CHECK:
					aframe->data[Aframe_index]=*data;
					Aframe_index++;
					if(Aframe_index==aframe->len){
						Aframe_index=0;
						Protocol.status=PARITY_CHECK;
					}
				break;
			case PARITY_CHECK:
					temp=(aframe->len)^(aframe->cmd);
					for(i=0;i<aframe->len;i++){
						temp^=aframe->data[i];
					}
				if(*data==temp){
					aframe->parity=*data;
					Protocol.status=HEAD_CHECK;
					return PROTOCOL_SUCCESS;
				}else{
					Protocol.status=HEAD_CHECK;
				}
				break;
			default:
				break;	
		}
	}
	return PROTOCOL_ERROR;
}





