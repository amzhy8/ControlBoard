#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__
#include "stm32f4xx_hal.h"
#include "wizchip_conf.h"

#define BSP_SUCCESS 0
#define BSP_ERROR 1

#define CONFIG_TABLE_FLAG 0xa8

typedef struct{
	uint8_t flag;
	uint32_t McuSn[8];//MCU���к�
	uint32_t BoardSn[8];//�������к�
	uint16_t BoardAddress;//�忨��ַ
	
	uint8_t Swver_Big;//�����汾��
	uint8_t Swver_Little;//С�汾��
	
	
	//����ͨ��ģ��
	uint8_t Network_ip[4];
	uint8_t Network_gw[4];
	uint8_t Network_nm[4];
	uint8_t Network_dns[4];
	uint8_t Network_mac[6];
	uint16_t Network_port;
	dhcp_mode dhcp;
	
	//����ͨ��ģ��
	
	
}Board_Typedef ;

extern Board_Typedef Board;

void ReadConfigTable(Board_Typedef *table);
uint8_t WriteConfigTable(Board_Typedef *table);
void InitConfigTable(Board_Typedef *table);
#endif


