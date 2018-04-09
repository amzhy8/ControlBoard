#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__
#include "stm32f4xx_hal.h"
#include "wizchip_conf.h"

#define BSP_SUCCESS 0
#define BSP_ERROR 1

#define CONFIG_TABLE_FLAG 0xa8

typedef struct{
	uint8_t flag;
	uint32_t McuSn[8];//MCU序列号
	uint32_t BoardSn[8];//板子序列号
	uint16_t BoardAddress;//板卡地址
	
	uint8_t Swver_Big;//软件大版本号
	uint8_t Swver_Little;//小版本号
	
	
	//网络通信模块
	uint8_t Network_ip[4];
	uint8_t Network_gw[4];
	uint8_t Network_nm[4];
	uint8_t Network_dns[4];
	uint8_t Network_mac[6];
	uint16_t Network_port;
	dhcp_mode dhcp;
	
	//串口通信模块
	
	
}Board_Typedef ;

extern Board_Typedef Board;

void ReadConfigTable(Board_Typedef *table);
uint8_t WriteConfigTable(Board_Typedef *table);
void InitConfigTable(Board_Typedef *table);
#endif


