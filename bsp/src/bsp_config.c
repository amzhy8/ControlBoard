#include "bsp_config.h"
#include "bsp_interal_flash.h"
#include <string.h>
#include "wizchip_conf.h"



Board_Typedef Board;


//系统配置初始化
void InitConfigTable(Board_Typedef *table){
	Board_Typedef tmp_table;
	 uint8_t ip[4]={192,168,16,150};
	 uint8_t gw[4]={192,168,16,254};
	 uint8_t nm[4]={255,255,255,0};
	 uint16_t port=9001;
	 
	 uint8_t mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
	 uint8_t dns[4]={8,8,8,8};
	 
	//检查是否是第一次上电
	ReadConfigTable(&tmp_table);
	if(tmp_table.flag!=CONFIG_TABLE_FLAG){//第一次上电  写默认初始化配置到注册表
		memset(&tmp_table,0,sizeof(Board_Typedef));
		tmp_table.flag=CONFIG_TABLE_FLAG;
		//tmp_table.BoardSn={0};
		tmp_table.BoardAddress=0;
		//tmp_table.McuSn={0};
		
		//设置默认网络参数
		memcpy(&tmp_table.Network_ip,ip,4*sizeof(uint8_t));
		memcpy(&tmp_table.Network_gw,gw,4*sizeof(uint8_t));
		memcpy(&tmp_table.Network_nm,nm,4*sizeof(uint8_t));
		memcpy(&tmp_table.Network_port,&port,1*sizeof(uint16_t));
		memcpy(&tmp_table.Network_mac,&mac,6*sizeof(uint8_t));//MAC
		memcpy(&tmp_table.Network_dns,&dns,4*sizeof(uint8_t));//dns
		tmp_table.dhcp=NETINFO_STATIC;//静态分配IP地址
	}
	
	#define SW_VER_BIG 0
	#define SW_VER_LITTLE 1
	
	tmp_table.Swver_Big=SW_VER_BIG;
	tmp_table.Swver_Little=SW_VER_LITTLE;
	tmp_table.BoardAddress=0;
	//tmp_table.BoardSn=NULL;
	//tmp_table.McuSn
	
	
	
	//拷贝注册表到全局变量
		memcpy(&Board,&tmp_table,sizeof(Board_Typedef));
		WriteConfigTable(&tmp_table);
}


//读系统配置表
void ReadConfigTable(Board_Typedef *table){
	 Interal_Flash_ReadByByte(USER_FLASH_ADDR_START,(uint8_t*)table,1*sizeof(Board_Typedef));
}

//写系统配置表
uint8_t WriteConfigTable(Board_Typedef *table){
	if(INTERAL_FLASH_SUCCESS==Interal_Flash_WriteByByte(USER_FLASH_ADDR_START,(uint8_t *)table,sizeof(Board_Typedef))){
			return BSP_SUCCESS;
			//打印参数表
		}else{
			return BSP_ERROR;
		}
}

