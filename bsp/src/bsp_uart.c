#include "bsp_uart.h"
#include <stdio.h>
#include "bsp_protocol.h"
#include "bsp_config.h"
#include <string.h>
#include "bsp_network.h"

xQueueHandle UartRecieveFifo;
uint8_t uart_rxdata;
uint8_t uart_txbuf[50];

#ifdef __GNUC__  
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf  
set to 'Yes') calls __io_putchar() */  
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)  
#else  
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)  
#endif /* __GNUC__ */  

PUTCHAR_PROTOTYPE  
{  
	while(HAL_OK!=HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&ch, 1)){
	}
  return ch;  
}  



void Uart1_Send(uint8_t *buf,uint16_t len){
		while(HAL_OK!=HAL_UART_Transmit_DMA(&huart1, buf, len));
}


void Bsp_UartInit(void){
	//创建串口FIFO队列
	UartRecieveFifo=xQueueCreate(UART_RECIEVE_FIFO_LEN,sizeof(uint8_t));
	Uart_Receive_Start(&huart1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	if(huart->Instance==USART1){
		HAL_UART_Receive_IT(&huart1,&uart_rxdata,1);
		xQueueSendFromISR(UartRecieveFifo,&uart_rxdata,&xHigherPriorityTaskWoken);
		
	}
}

//开启串口接收
void Uart_Receive_Start(UART_HandleTypeDef *huart){
	HAL_UART_Receive_IT(huart,&uart_rxdata,1);
	xQueueSend(UartRecieveFifo,&uart_rxdata,0);
	xQueueReceive(UartRecieveFifo,&uart_rxdata,10);
	
}
void UartCmdReback(uint8_t status){
		//uint8_t buf[20];
			uart_txbuf[0]=0xa5;//头
			uart_txbuf[1]=0x00;//长度
			
			if(status==BSP_SUCCESS){
			uart_txbuf[2]=0x80;//CMD
			uart_txbuf[3]=0x00^0x80;//数据/校验		
			}else{
			uart_txbuf[2]=0x81;//CMD
			uart_txbuf[3]=0x00^0x81;//数据/校验		
			}
			Uart1_Send(uart_txbuf,4);		
}

void UartTask_Func(void *parameter){
	uint8_t data;
	UBaseType_t  len;
	UBaseType_t i;
	Board_Typedef tmp_table;
	
	
	while(1){
		vTaskDelay(50);
		len=uxQueueMessagesWaiting(UartRecieveFifo);
		if(len){
			for(i=0;i<len;i++){
				if(pdPASS==xQueueReceive(UartRecieveFifo,&data,10)){
					if(CheckFrame(AFRAME,&AFrame,&data)==PROTOCOL_SUCCESS){
					//检测完成 
					if(AFrame.cmd==0x80){//成功命令

					}
					if(AFrame.cmd==0x81){//失败命令
		
					}
					if(AFrame.cmd==0x10){//测试通信
						UartCmdReback(BSP_SUCCESS);		//返回成功命令		
					}
					
					if(AFrame.cmd==0x11){//测试通信
						UartCmdReback(BSP_SUCCESS);		//返回成功命令		
					}
					if(AFrame.cmd==0x12){//
					
						UartCmdReback(BSP_SUCCESS);		//返回成功命令		
					}
					if(AFrame.cmd==0x13){//
					
						UartCmdReback(BSP_SUCCESS);		//返回成功命令		
					}
					if(AFrame.cmd==0x14){//
						ReadConfigTable(&tmp_table);//读取注册表
						printf("Read tmp_table 1:\r\n");
						printf("tmp_table.Network_ip:%d.%d.%d.%d\r\n",tmp_table.Network_ip[0],tmp_table.Network_ip[1],tmp_table.Network_ip[2],tmp_table.Network_ip[3]);
						printf("tmp_table.Network_gw:%d.%d.%d.%d\r\n",tmp_table.Network_gw[0],tmp_table.Network_gw[1],tmp_table.Network_gw[2],tmp_table.Network_gw[3]);
						printf("tmp_table.Network_nm:%d.%d.%d.%d\r\n",tmp_table.Network_nm[0],tmp_table.Network_nm[1],tmp_table.Network_nm[2],tmp_table.Network_nm[3]);
						printf("tmp_table.Network_dns:%d.%d.%d.%d\r\n",tmp_table.Network_dns[0],tmp_table.Network_dns[1],tmp_table.Network_dns[2],tmp_table.Network_dns[3]);
						printf("tmp_table.Network_port:%d\r\n",tmp_table.Network_port);
	

						
						memcpy(tmp_table.Network_ip,&AFrame.data[0],4*sizeof(uint8_t));
						memcpy(tmp_table.Network_gw,&AFrame.data[4],4*sizeof(uint8_t));
						memcpy(tmp_table.Network_nm,&AFrame.data[8],4*sizeof(uint8_t));
						memcpy(&tmp_table.Network_port,&AFrame.data[12],1*sizeof(uint16_t));
						
						WriteConfigTable(&tmp_table);//写回注册表
						
						//读出注册表
						ReadConfigTable(&tmp_table);
						printf("Read tmp_table 2:\r\n");
						printf("tmp_table->Network_ip:%d.%d.%d.%d\r\n",tmp_table.Network_ip[0],tmp_table.Network_ip[1],tmp_table.Network_ip[2],tmp_table.Network_ip[3]);
						printf("tmp_table->Network_gw:%d.%d.%d.%d\r\n",tmp_table.Network_gw[0],tmp_table.Network_gw[1],tmp_table.Network_gw[2],tmp_table.Network_gw[3]);
						printf("tmp_table->Network_nm:%d.%d.%d.%d\r\n",tmp_table.Network_nm[0],tmp_table.Network_nm[1],tmp_table.Network_nm[2],tmp_table.Network_nm[3]);
						printf("tmp_table->Network_dns:%d.%d.%d.%d\r\n",tmp_table.Network_dns[0],tmp_table.Network_dns[1],tmp_table.Network_dns[2],tmp_table.Network_dns[3]);
						printf("tmp_table->Network_port:%d\r\n",tmp_table.Network_port);
	
	
						//写Board 
						memcpy(Board.Network_ip,tmp_table.Network_ip,4*sizeof(uint8_t));
						memcpy(Board.Network_gw,tmp_table.Network_gw,4*sizeof(uint8_t));
						memcpy(Board.Network_nm,tmp_table.Network_nm,4*sizeof(uint8_t));
						memcpy(&Board.Network_port,&tmp_table.Network_port,1*sizeof(uint16_t));
						
						//更新网络模块的网络参数表
						memcpy(W5500_Conifg.netinfo.ip,tmp_table.Network_ip,4*sizeof(uint8_t));
						memcpy(W5500_Conifg.netinfo.gw,tmp_table.Network_gw,4*sizeof(uint8_t));
						memcpy(W5500_Conifg.netinfo.sn,tmp_table.Network_nm,4*sizeof(uint8_t));
						memcpy(&W5500_Conifg.port,&tmp_table.Network_port,1*sizeof(uint16_t));
						
		
						
						//发送重启网络事件标志
						W5500_Conifg.NeedRestart=NETWORK_NEED_RESTART;
						UartCmdReback(BSP_SUCCESS);		//返回成功命令		
					}
				}
				//printf("%c",data);
				}
		
			}
		}
		
	}
}




