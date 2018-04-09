#include "bsp_network.h"
#include <string.h>
#include "bsp_uart.h"
#include "bsp_config.h"
#include "bsp_protocol.h"
#include "bsp_can.h"

W5500_Conifg_Typedef W5500_Conifg;
xQueueHandle NetWorkRecieveFifo;
uint8_t USART_RX_BUF[USART_REC_LEN];     // ����һ�����ڴ������ݽ��յĻ��棬��СΪUSART_REC_LEN
uint16_t USART_RX_STA=0;       					// ���岢��ʼ�����ڽ��ձ�־λ

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
	

	//��������ģ���еĽ���FIFO
	NetWorkRecieveFifo=xQueueCreate(NETWORK_FIFO_LEN,sizeof(uint8_t));
	//ReadConfigTable(&tmp_table);//��ȡע���
	memcpy(obj->netinfo.ip,Board.Network_ip,4*sizeof(uint8_t));//��ϵͳ���ñ��ȡĬ��IP
	memcpy(obj->netinfo.gw,Board.Network_gw,4*sizeof(uint8_t));//����
	memcpy(obj->netinfo.sn,Board.Network_nm,4*sizeof(uint8_t));//��������
	memcpy(obj->netinfo.dns,Board.Network_dns,4*sizeof(uint8_t));//DNS
	memcpy(obj->netinfo.mac,Board.Network_mac,6*sizeof(uint8_t));//MAC
	memcpy(&obj->netinfo.dhcp,&Board.dhcp,1*sizeof(dhcp_mode));//DHCP
	memcpy(&obj->port,&Board.Network_port,1*sizeof(uint16_t));//PORT
	
	
	
	
	obj->debug=1;
	obj->fw_len=0;
	obj->sw_ver[0]=FW_VER_HIGH;
	obj->sw_ver[1]=FW_VER_LOW;
	obj->state=NORMAL_STATE;
	
	//����Ҫ����
	obj->NeedRestart=NETWORK_NONEED_RESTART;
	
	return NETWORK_SUCCESS;
}


//�����������״̬
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


//�����������״̬
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



// ����Ĭ��MAC��IP��GW��SUB��DNS
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
	//����Ҫ����
	W5500_Conifg.NeedRestart=NETWORK_NONEED_RESTART;
}



/*
// ���ó�ʼ��IP��Ϣ����ӡ����ʼ��8��Socket
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

	ctlnetwork(CN_SET_NETINFO, &W5500_Conifg.netinfo);//�����������
	vTaskDelay(10);

	//ctlwizchip(CW_SET_PHYCONF, &phyConf);//����PHY
	vTaskDelay(1000);
	//��ȡ�����������
	if(0==ctlwizchip(CW_GET_PHYLINK, &link)){
		if(link==PHY_LINK_ON){
			printf("���������ӣ�\r\n");
		}else{
			printf("����δ���ӣ�\r\n");
		}
	}
//set rx tx buf 2k
	wizchip_init(NULL,NULL);
  
  setRTR(2000);																		// �����ش���ʱʱ��
  setRCR(3);																			// ����������·��ʹ���
  
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
	
	
	Reset_W5500();//��λW5500
	vTaskDelay(10);
	//set_default();
	//vTaskDelay(10);
	ctlnetwork(CN_SET_NETINFO, info);//�����������
	vTaskDelay(10);
	//ctlwizchip(CW_SET_PHYCONF, &phyConf);//����PHY
	vTaskDelay(1000);
	//��ȡ�����������
	if(0==ctlwizchip(CW_GET_PHYLINK, &link)){
		if(link==PHY_LINK_ON){
			printf("���������ӣ�\r\n");
		}else{
			printf("����δ���ӣ�\r\n");
		}
	}
//set rx tx buf 2k
	wizchip_init(NULL,NULL);
  setRTR(2000);																		// �����ش���ʱʱ��
  setRCR(3);																			// ����������·��ʹ���
  
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
	
	
	Reset_W5500();//��λW5500
	vTaskDelay(10);
	//set_default();
	//vTaskDelay(10);
	set_network(&W5500_Conifg.netinfo,W5500_Conifg.port);
	vTaskDelay(10);
	//setkeepalive(0);
	vTaskDelay(10);

	
	while(1){
		
		switch(getSn_SR(0))										// ��ȡsocket0��״̬
			{ 
				case SOCK_INIT:											// Socket���ڳ�ʼ�����(��)״̬
						listen(0);											// �����ոմ򿪵ı��ض˿ڣ��ȴ��ͻ�������
				break;
				case SOCK_LISTEN:
				break;
				case SOCK_ESTABLISHED:							// Socket�������ӽ���״̬
						if(getSn_IR(0) & Sn_IR_CON)			
						{
							setSn_IR(0, Sn_IR_CON);				// Sn_IR��CONλ��1��֪ͨW5500�����ѽ���
						}
						// ���ݻػ����Գ������ݴ���λ���ͻ��˷���W5500��W5500���յ����ݺ��ٻظ��ͻ���
						len=getSn_RX_RSR(0);						// ��ȡW5500���н��ջ���Ĵ�����ֵ������len��Sn_RX_RSR��ʾ���ջ������ѽ��պͱ�������ݴ�С
						if(len>0)
						{
							reallen=recv(0,buffer,len);						// W5500�������Կͻ��˵����ݣ���ͨ��SPI���͸�MCU
							//�����ݴ������
							//prvLockueue();
							for(i=0;i<reallen;i++){
								xQueueSend(NetWorkRecieveFifo,&buffer[i],0);
							}
							//preUnlockQueue();
							reallen=0;
							//printf("%s\r\n",buffer);			// ���ڴ�ӡ���յ�������
							//send(0,buffer,len);						// ���յ����ݺ��ٻظ��ͻ��ˣ�������ݻػ�
						}
						// W5500�Ӵ��ڷ����ݸ��ͻ��˳����������Իس�����
						if(USART_RX_STA & 0x8000)				// �жϴ��������Ƿ�������
						{					   
							len=USART_RX_STA & 0x3fff;		// ��ȡ���ڽ��յ����ݵĳ���
							send(0,USART_RX_BUF,len);			// W5500��ͻ��˷�������
							USART_RX_STA=0;								// ���ڽ���״̬��־λ��0
							memset(USART_RX_BUF,0,len+1);	// ���ڽ��ջ�����0
						}
						
									
						
				break;
				case SOCK_CLOSE_WAIT:								// Socket���ڵȴ��ر�״̬
						close(0);												// �ر�Socket0
				break;
				case SOCK_CLOSED:										// Socket���ڹر�״̬
						socket(0,Sn_MR_TCP,W5500_Conifg.port,Sn_MR_ND);		// ��Socket0��������ΪTCP����ʱģʽ����һ�����ض˿�
				break;
			}
		
			
		//�����������״̬
			//NetWork_LinkIsChanged(&W5500_Conifg,&linkstatus);
			if(NETWORK_SUCCESS==NetWork_LinkIsChanged(&W5500_Conifg,&linkstatus)){
				if(linkstatus==PHY_LINK_ON){
					printf("���������ӣ�\r\n");
				}else{
					printf("�����ѶϿ���\r\n");
					close(0);	
				}
			}

			if(W5500_Conifg.NeedRestart==NETWORK_NEED_RESTART){//��Ҫ����
					Reset_W5500();//��λW5500
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
								
										//֪ͨCAN������Ϣ��ͨ����
										if(BSP_SUCCESS==CanSendCtrlCmd(AFrame_Net.data[0],AFrame_Net.data[1],AFrame_Net.data[2],((uint16_t)AFrame_Net.data[3])|(((uint16_t)AFrame_Net.data[4])<<8))){
										//if(BSP_SUCCESS==CanSendCtrlCmd(tmp_channelboard.address,tmp_channelboard.channel_switch,tmp_channelboard.source_switch,tmp_channelboard.adc_value)){
											//���ͳɹ�  TCP���سɹ�����
											TCP_SendBack(TCP_SEND_CMD_OK);
											printf("Can Send Success!\r\n");
										}else{
											//����ʧ��  TCP���ش�������
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
										//���سɹ�
										
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



