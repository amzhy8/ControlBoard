#include "bsp_task.h"
#include <string.h>
#include "bsp_protocol.h"
#include "bsp_uart.h"


TaskHandle_t LedTask_Handler;
TaskHandle_t CanTask_Handler;
TaskHandle_t NetWorkTask_Handler;
TaskHandle_t UartTask_Handler;

TaskHandle_t ProcessDataTask_Handler;

void LedTask_Func(void *parameter){
	while(1){
		LED_Toggle();
		vTaskDelay(1000);
	}
}




void CanTask_Func(void *parameter){
/*
	memset(Canbuf,0,20);
	Canbuf[0]=0xa5;
	Canbuf[1]=0x04;
	Canbuf[2]=0x00;
	Canbuf[3]=0x17;
	Canbuf[4]=0x00;
	Canbuf[5]=0x00;
	Canbuf[6]=0x00;
	Canbuf[7]=Canbuf[1]^Canbuf[2]^Canbuf[3]^Canbuf[4]^Canbuf[5]^Canbuf[6];
	//CanInit(&Bsp_Can_obj,&hcan1);//初始化CAN
	//vTaskDelay(100);
	
	while(1){
		if(BSP_CAN_SUCCESS==Can_Send(&Bsp_Can_obj,0x0001,Canbuf,8)){
			printf("Can Send Success\r\n");
		}else{
			printf("Can Send Error\r\n");
		}
		vTaskDelay(1000);
	}
	*/
	HAL_Delay(5000);
	while(1){
		Can_Server();
		//vTaskDelay(1000);
		vTaskDelay(100);
	}
	
}






/*
void NetWorkTask_Func(void *parameter){
	while(1){
		vTaskDelay(1000);
	}
}
*/

void ProcessData_Func(void *parameter){
	uint8_t data;
	UBaseType_t  len;
	UBaseType_t i;
	while(1){
		
		len=uxQueueMessagesWaiting(NetWorkRecieveFifo);
		if(len){
			for(i=0;i<len;i++){
				xQueueReceive( NetWorkRecieveFifo, &data, 5 ) ;//等待队列数据
				printf("%c",data);
				//检测帧
				/*
				if(CheckFrame(AFRAME,&AFrame,&data)==PROTOCOL_SUCCESS){
					//处理帧数据
					if(AFrame.cmd==0x20){//测试通信
						UartCmdReback(BSP_SUCCESS);		//返回成功命令		
					}
				}
				*/
			}
		}
		//printf("%c",data);
		
		vTaskDelay(50);
	}
		
}

