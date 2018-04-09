#include "bsp_network.h"
#include <string.h>
#include "bsp_uart.h"
#include "bsp_config.h"
#include "bsp_protocol.h"
#include "bsp_can.h"

W5500_Conifg_Typedef W5500_Conifg;
xQueueHandle NetWorkRecieveFifo;
uint8_t USART_RX_BUF[USART_REC_LEN];     // 定义一个用于串口数据接收的缓存，大小为USART_REC_LEN
uint16_t USART_RX_STA=0;       					// 定义并初始化串口接收标志位

uint8_t buffer[NETWORK_FIFO_LEN];		
void Reset_W5500(void)
{
	HAL_GPIO_WritePin(W5500_RST_GPIO_PORT,W5500_RST_GPIO_PIN,GPIO_PIN_RESET);
	vTaskDelay(1);
	HAL_GPIO_WritePin(W5500_RST_GPIO_PORT,W5500_RST_GPIO_PIN,GPIO_PIN_SET);
  vTaskDelay(2);
}



uint8_t NetWorkModuleInit(W5500_Conifg_Typedef *obj){
	//Board_Typedef tmp_table;
	

	//创建网络模块中的接收FIFO
	NetWorkRecieveFifo=xQueueCreate(NETWORK_FIFO_LEN,sizeof(uint8_t));
	//ReadConfigTable(&tmp_table);//读取注册表
	memcpy(obj->netinfo.ip,Board.Network_ip,4*sizeof(uint8_t));//从系统配置表读取默认IP
	memcpy(obj->netinfo.gw,Board.Network_gw,4*sizeof(uint8_t));//网关
	memcpy(obj->netinfo.sn,Board.Network_nm,4*sizeof(uint8_t));//子网掩码
	memcpy(obj->netinfo.dns,Board.Network_dns,4*sizeof(uint8_t));//DNS
	memcpy(obj->netinfo.mac,Board.Network_mac,6*sizeof(uint8_t));//MAC
	memcpy(&obj->netinfo.dhcp,&Board.dhcp,1*sizeof(dhcp_mode));//DHCP
	memcpy(&obj->port,&Board.Network_port,1*sizeof(uint16_t));//PORT
	
	
	
	
	obj->debug=1;
	obj->fw_len=0;
	obj->sw_ver[0]=FW_VER_HIGH;
	obj->sw_ver[1]=FW_VER_LOW;
	obj->state=NORMAL_STATE;
	
	//不需要重启
	obj->NeedRestart=NETWORK_NONEED_RESTART;
	
	return NETWORK_SUCCESS;
}


//检查网线连接状态
uint8_t NetWork_LinkIsChanged(W5500_Conifg_Typedef *obj,uint8_t *curentstatus){
	if(0==ctlwizchip(CW_GET_PHYLINK, curentstatus)){
		obj->link_status.pre_status=obj->link_status.cur_status;
		obj->link_status.cur_status=*curentstatus;
		if(obj->link_status.cur_status!=obj->link_status.pre_status){
			return NETWORK_SUCCESS;
		}else{
			return NETWORK_FALSE;
		}
	}
	return NETWORK_FALSE;
}


//检查网络连接状态
uint8_t NetWork_SocketStatusIsChanged(W5500_Conifg_Typedef *obj){
	uint8_t status=getSn_SR(0);

		obj->socket_stauts.pre_status=obj->socket_stauts.cur_status;
		obj->socket_stauts.cur_status=status;
		if(obj->socket_stauts.cur_status!=obj->socket_stauts.pre_status){
					switch(status){
				case SOCK_INIT:
					printf("Socket Status:SOCK_INIT\r\n");
					break;
				case SOCK_LISTEN:
					printf("Socket Status:SOCK_LISTEN\r\n");
					break;
				case SOCK_ESTABLISHED:
					printf("Socket Status:SOCK_ESTABLISHED\r\n");
					break;
				case SOCK_CLOSE_WAIT:
					printf("Socket Status:SOCK_CLOSE_WAIT\r\n");
					break;
				case SOCK_CLOSED:
					printf("Socket Status:SOCK_CLOSED\r\n");
					break;
				case SOCK_SYNSENT:
					printf("Socket Status:SOCK_SYNSENT\r\n");
					break;
				case SOCK_SYNRECV:
					printf("Socket Status:SOCK_SYNRECV\r\n");
					break;
				case SOCK_FIN_WAIT:
					printf("Socket Status:SOCK_FIN_WAIT\r\n");
					break;
				
				case SOCK_CLOSING:
					printf("Socket Status:SOCK_CLOSING\r\n");
					break;
				
				case SOCK_TIME_WAIT:
					printf("Socket Status:SOCK_TIME_WAIT\r\n");
					break;		
				case SOCK_LAST_ACK:
					printf("Socket Status:SOCK_LAST_ACK\r\n");
					break;
				case SOCK_UDP:
					printf("Socket Status:SOCK_UDP\r\n");
					break;
				case SOCK_IPRAW:
					printf("Socket Status:SOCK_IPRAW\r\n");
					break;
				default:break;
			}
	}
			
			

	return NETWORK_SUCCESS;
}



// 设置默认MAC、IP、GW、SUB、DNS
void set_default(void)	
{  
	
	
	
	wiz_NetInfo netinfo={
		.mac={0x00,0x08,0xdc,0x11,0x11,0x11},
		.ip={192,168,16,150},
		.sn={255,255,255,0},
		.gw={192,168,16,254},
		.dns={8,8,8,8},
		.dhcp=NETINFO_STATIC
	};
	memcpy(&W5500_Conifg.netinfo, &netinfo, sizeof(wiz_NetInfo));
	
  W5500_Conifg.debug=1;
  W5500_Conifg.fw_len=0;
  
  W5500_Conifg.state=NORMAL_STATE;
  W5500_Conifg.sw_ver[0]=FW_VER_HIGH;
  W5500_Conifg.sw_ver[1]=FW_VER_LOW;
	//不需要重启
	W5500_Conifg.NeedRestart=NETWORK_NONEED_RESTART;
}



/*
// 配置初始化IP信息并打印，初始化8个Socket
void set_network(void)														
{
	uint8_t link;
  uint8_t ip[4];

//	wiz_PhyConf phyConf={
//		.by=PHY_CONFBY_SW,       ///< set by @ref PHY_CONFBY_HW or @ref PHY_CONFBY_SW
//    .mode=PHY_MODE_MANUAL,     ///< set by @ref PHY_MODE_MANUAL or @ref PHY_MODE_AUTONEGO
//    .speed=PHY_SPEED_100,    ///< set by @ref PHY_SPEED_10 or @ref PHY_SPEED_100
//    .duplex=PHY_DUPLEX_FULL  ///< set by @ref PHY_DUPLEX_HALF @ref PHY_DUPLEX_FULL 
//	};

	ctlnetwork(CN_SET_NETINFO, &W5500_Conifg.netinfo);//设置网络参数
	vTaskDelay(10);

	//ctlwizchip(CW_SET_PHYCONF, &phyConf);//设置PHY
	vTaskDelay(1000);
	//获取网线连接情况
	if(0==ctlwizchip(CW_GET_PHYLINK, &link)){
		if(link==PHY_LINK_ON){
			printf("网线已连接！\r\n");
		}else{
			printf("网线未连接！\r\n");
		}
	}
//set rx tx buf 2k
	wizchip_init(NULL,NULL);
  
  setRTR(2000);																		// 设置重传超时时间
  setRCR(3);																			// 设置最大重新发送次数
  
	getSIPR (ip);
  printf("IP : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getSUBR(ip);
  printf("SN : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getGAR(ip);
  printf("GW : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]); 
	
	
}
*/


void set_network(wiz_NetInfo *info,uint16_t port){
	uint8_t ip[4];
	uint8_t link;
	
	
	Reset_W5500();//复位W5500
	vTaskDelay(10);
	//set_default();
	//vTaskDelay(10);
	ctlnetwork(CN_SET_NETINFO, info);//设置网络参数
	vTaskDelay(10);
	//ctlwizchip(CW_SET_PHYCONF, &phyConf);//设置PHY
	vTaskDelay(1000);
	//获取网线连接情况
	if(0==ctlwizchip(CW_GET_PHYLINK, &link)){
		if(link==PHY_LINK_ON){
			printf("网线已连接！\r\n");
		}else{
			printf("网线未连接！\r\n");
		}
	}
//set rx tx buf 2k
	wizchip_init(NULL,NULL);
  setRTR(2000);																		// 设置重传超时时间
  setRCR(3);																			// 设置最大重新发送次数
  
	getSIPR (ip);
  printf("IP : %d.%d.%d.%d\r\n", info->ip[0],info->ip[1],info->ip[2],info->ip[3]);
  getSUBR(ip);
  printf("SN : %d.%d.%d.%d\r\n", info->sn[0],info->sn[1],info->sn[2],info->sn[3]);
  getGAR(ip);
  printf("GW : %d.%d.%d.%d\r\n", info->gw[0],info->gw[1],info->gw[2],info->gw[3]); 
	vTaskDelay(10);
	//setkeepalive(0);
	vTaskDelay(10);
	printf("W5500 Init Complete!\r\n");	
	printf("TCP Server: %d.%d.%d.%d:%d\r\n",info->ip[0],info->ip[1],info->ip[2],info->ip[3],port);
}	




void NetWorkTask_Func(void *parameter){
	 uint16_t len=0; 
	 uint16_t queuelen=0; 
	 uint8_t queuedata;
	 uint16_t  reallen;
	 uint8_t i;
	 uint8_t linkstatus;
	ChannelBoard_Typedef tmp_channelboard;
	//uint16_t can_send_addr;

	printf("Board.Network_ip:%d.%d.%d.%d\r\n",Board.Network_ip[0],Board.Network_ip[1],Board.Network_ip[2],Board.Network_ip[3]);
	printf("Board.Network_gw:%d.%d.%d.%d\r\n",Board.Network_gw[0],Board.Network_gw[1],Board.Network_gw[2],Board.Network_gw[3]);
	printf("Board.Network_nm:%d.%d.%d.%d\r\n",Board.Network_nm[0],Board.Network_nm[1],Board.Network_nm[2],Board.Network_nm[3]);
	printf("Board.Network_dns:%d.%d.%d.%d\r\n",Board.Network_dns[0],Board.Network_dns[1],Board.Network_dns[2],Board.Network_dns[3]);
	printf("Board.Network_port:%d\r\n",Board.Network_port);
	
	printf("W5500_Conifg->netinfo.ip:%d.%d.%d.%d\r\n",W5500_Conifg.netinfo.ip[0],W5500_Conifg.netinfo.ip[1],W5500_Conifg.netinfo.ip[2],W5500_Conifg.netinfo.ip[3]);
	printf("W5500_Conifg->netinfo.gw:%d.%d.%d.%d\r\n",W5500_Conifg.netinfo.gw[0],W5500_Conifg.netinfo.gw[1],W5500_Conifg.netinfo.gw[2],W5500_Conifg.netinfo.gw[3]);
	printf("W5500_Conifg->netinfo.nm:%d.%d.%d.%d\r\n",W5500_Conifg.netinfo.sn[0],W5500_Conifg.netinfo.sn[1],W5500_Conifg.netinfo.sn[2],W5500_Conifg.netinfo.sn[3]);
	printf("W5500_Conifg->netinfo.dns:%d.%d.%d.%d\r\n",W5500_Conifg.netinfo.dns[0],W5500_Conifg.netinfo.dns[1],W5500_Conifg.netinfo.dns[2],W5500_Conifg.netinfo.dns[3]);
	printf("W5500_Conifg->port:%d\r\n",W5500_Conifg.port);
	
	
	Reset_W5500();//复位W5500
	vTaskDelay(10);
	//set_default();
	//vTaskDelay(10);
	set_network(&W5500_Conifg.netinfo,W5500_Conifg.port);
	vTaskDelay(10);
	//setkeepalive(0);
	vTaskDelay(10);

	
	while(1){
		
		switch(getSn_SR(0))										// 获取socket0的状态
			{ 
				case SOCK_INIT:											// Socket处于初始化完成(打开)状态
						listen(0);											// 监听刚刚打开的本地端口，等待客户端连接
				break;
				case SOCK_LISTEN:
				break;
				case SOCK_ESTABLISHED:							// Socket处于连接建立状态
						if(getSn_IR(0) & Sn_IR_CON)			
						{
							setSn_IR(0, Sn_IR_CON);				// Sn_IR的CON位置1，通知W5500连接已建立
						}
						// 数据回环测试程序：数据从上位机客户端发给W5500，W5500接收到数据后再回给客户端
						len=getSn_RX_RSR(0);						// 读取W5500空闲接收缓存寄存器的值并赋给len，Sn_RX_RSR表示接收缓存中已接收和保存的数据大小
						if(len>0)
						{
							reallen=recv(0,buffer,len);						// W5500接收来自客户端的数据，并通过SPI发送给MCU
							//将数据存入队列
							//prvLockueue();
							for(i=0;i<reallen;i++){
								xQueueSend(NetWorkRecieveFifo,&buffer[i],0);
							}
							//preUnlockQueue();
							reallen=0;
							//printf("%s\r\n",buffer);			// 串口打印接收到的数据
							//send(0,buffer,len);						// 接收到数据后再回给客户端，完成数据回环
						}
						// W5500从串口发数据给客户端程序，数据需以回车结束
						if(USART_RX_STA & 0x8000)				// 判断串口数据是否接收完成
						{					   
							len=USART_RX_STA & 0x3fff;		// 获取串口接收到数据的长度
							send(0,USART_RX_BUF,len);			// W5500向客户端发送数据
							USART_RX_STA=0;								// 串口接收状态标志位清0
							memset(USART_RX_BUF,0,len+1);	// 串口接收缓存清0
						}
						
									
						
				break;
				case SOCK_CLOSE_WAIT:								// Socket处于等待关闭状态
						close(0);												// 关闭Socket0
				break;
				case SOCK_CLOSED:										// Socket处于关闭状态
						socket(0,Sn_MR_TCP,W5500_Conifg.port,Sn_MR_ND);		// 打开Socket0，并配置为TCP无延时模式，打开一个本地端口
				break;
			}
		
			
		//检测网线连接状态
			//NetWork_LinkIsChanged(&W5500_Conifg,&linkstatus);
			if(NETWORK_SUCCESS==NetWork_LinkIsChanged(&W5500_Conifg,&linkstatus)){
				if(linkstatus==PHY_LINK_ON){
					printf("网线已连接！\r\n");
				}else{
					printf("网线已断开！\r\n");
					close(0);	
				}
			}

			if(W5500_Conifg.NeedRestart==NETWORK_NEED_RESTART){//需要重启
					Reset_W5500();//复位W5500
					vTaskDelay(10);
					set_network(&W5500_Conifg.netinfo,W5500_Conifg.port);
					W5500_Conifg.NeedRestart=NETWORK_NONEED_RESTART;
			}
			NetWork_SocketStatusIsChanged(&W5500_Conifg);	

			queuelen=uxQueueMessagesWaiting(NetWorkRecieveFifo);
			if(queuelen){
				for(i=0;i<len;i++){
					if(pdPASS==xQueueReceive(NetWorkRecieveFifo,&queuedata,10)){
						if(CheckFrame(AFRAME,&AFrame_Net,&queuedata)==PROTOCOL_SUCCESS){
						
							switch(AFrame_Net.cmd){
								case 0x30:
										tmp_channelboard.address=AFrame_Net.data[0];
										tmp_channelboard.channel_switch=AFrame_Net.data[1];
										tmp_channelboard.source_switch=AFrame_Net.data[2];
										tmp_channelboard.adc_value=((uint16_t)AFrame_Net.data[3])|(((uint16_t)AFrame_Net.data[4])<<8);
										memcpy(&ChannelBoard[AFrame_Net.data[0]],&tmp_channelboard,sizeof(ChannelBoard_Typedef));
								
										//通知CAN发送消息给通道板
										if(BSP_SUCCESS==CanSendCtrlCmd(AFrame_Net.data[0],AFrame_Net.data[1],AFrame_Net.data[2],((uint16_t)AFrame_Net.data[3])|(((uint16_t)AFrame_Net.data[4])<<8))){
										//if(BSP_SUCCESS==CanSendCtrlCmd(tmp_channelboard.address,tmp_channelboard.channel_switch,tmp_channelboard.source_switch,tmp_channelboard.adc_value)){
											//发送成功  TCP返回成功命令
											TCP_SendBack(TCP_SEND_CMD_OK);
											printf("Can Send Success!\r\n");
										}else{
											//发送失败  TCP返回错误命令
											TCP_SendBack(TCP_SEND_CMD_ERROR);
											printf("Can Send Error!\r\n");
										}
								/*
											if(Can_Send(&Bsp_Can_obj,0x0001,Canbuf,8)==BSP_CAN_SUCCESS){
												printf("Can Send Success!\r\n");
											}else{
												printf("Can Send Error!\r\n");
											}
								*/
										//返回成功
										
									break;
								default:break;
							}
						
						
						}
					}
				}
			}

			
			vTaskDelay(20);	
	}
}


void TCP_SendBack(uint8_t cmd){
	uint8_t *buf=pvPortMalloc(30);
	buf[0]=AFRAME_HEAD;
	buf[2]=cmd;
	switch(cmd){
		case TCP_SEND_CMD_OK:
			buf[1]=0x00;
			buf[3]=TCP_SEND_CMD_OK;
			if(SOCK_ESTABLISHED==getSn_SR(0)){
				send(0,buf,4);
			}
			break;
		case TCP_SEND_CMD_ERROR:
			buf[1]=0x00;
			buf[3]=TCP_SEND_CMD_ERROR;
			if(SOCK_ESTABLISHED==getSn_SR(0)){
				send(0,buf,4);
			}
			break;
		default:
			break;
	}
	vPortFree(buf);
	
}



