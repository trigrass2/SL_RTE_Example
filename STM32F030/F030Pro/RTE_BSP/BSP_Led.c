#include "BSP_Led.h"
//--------------------------------------------------------------
// Definition aller LEDs
// Reihenfolge wie bei LED_NAME_t
//
// Init : [LED_OFF,LED_ON]
//--------------------------------------------------------------
static led_t LedHandle[LED_N]=
{
	// Name    ,PORT , PIN ,                        Init
	{LED_0,GPIOB,GPIO_Pin_1,RCC_AHBPeriph_GPIOB,LED_OFF},   // PD2,LED0
};
//--------------------------------------------------------------
// LED ausschalten
//--------------------------------------------------------------
void BSP_Led_Off(led_name_e led_name)
{
	GPIO_SetBits(LedHandle[led_name].LedPort, LedHandle[led_name].LedPin);
}

//--------------------------------------------------------------
// LED einschalten
//--------------------------------------------------------------
void BSP_Led_On(led_name_e led_name)
{
	GPIO_ResetBits(LedHandle[led_name].LedPort, LedHandle[led_name].LedPin);
} 
//--------------------------------------------------------------
// LED toggeln
//--------------------------------------------------------------
void BSP_Led_Toggle(led_name_e led_name)
{
	LedHandle[led_name].LedPort->ODR ^= LedHandle[led_name].LedPin;
}
void BSP_Led_Init(void)
{
	for(led_name_e led_name=(led_name_e)0;led_name<LED_N;led_name++) 
	{
		GPIO_InitTypeDef GPIO_InitStructure = {0};
		// Clock Enable
		RCC_AHBPeriphClockCmd(LedHandle[led_name].LedClk, ENABLE);
		// Config als Digital-Ausgang
		GPIO_InitStructure.GPIO_Pin = LedHandle[led_name].LedPin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(LedHandle[led_name].LedPort, &GPIO_InitStructure);
		// Default Wert einstellen
		if(LedHandle[led_name].LedInitStatus==LED_OFF) 
			BSP_Led_Off(led_name);
		else 
			BSP_Led_On(led_name);
	}
}
