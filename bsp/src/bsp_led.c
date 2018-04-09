#include "bsp_led.h"
#include "cmsis_os.h"
#include "tim.h"
#include "bsp_uart.h"
#include "bsp_can.h"


#define LED_TOGGLE_PERIOD 1000   //MS
uint32_t Led_Cunter=0;

void LED_Control(uint8_t state){
	if(state){
		HAL_GPIO_WritePin(LED_GPIO_PORT,LED_GPIO_PIN,GPIO_PIN_RESET);
	}else{
		HAL_GPIO_WritePin(LED_GPIO_PORT,LED_GPIO_PIN,GPIO_PIN_SET);
	}

}

void LED_Toggle(void){
	HAL_GPIO_TogglePin(LED_GPIO_PORT,LED_GPIO_PIN);
}



void LED_Time_Callback(){
	if(Led_Cunter==LED_TOGGLE_PERIOD){
				LED_Toggle();
				Led_Cunter=0;
			}
			Led_Cunter++;
}




