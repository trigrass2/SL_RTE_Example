#ifndef __BSP_SOFTPWM_H
#define __BSP_SOFTPWM_H
#include "stm32f0xx.h"
#include "RTE_Include.h"
typedef enum 
{
	SOFTPWM_1 = 0,
	SOFTPWM_2 = 1,
	SOFTPWM_3 = 2,
	SOFTPWM_4 = 3,
	SOFTPWM_N,
}softpwm_name_e;
typedef struct {
	softpwm_name_e name;
	GPIO_TypeDef* PWMPort; // Port
	uint16_t PWMPin; // Pin
	uint32_t PWMClk; // Clock
	bool Output;
	uint8_t TimerID;
	uint32_t PWMPer;
	uint32_t PWMFre;
}softpwm_t;
extern void BSP_SoftPWM_Init(softpwm_name_e pwm,uint32_t Fre,uint8_t Period);
extern void BSP_SoftPWM_SetPer(softpwm_name_e pwm,uint8_t Period);
extern void BSP_SoftPWM_Stop(softpwm_name_e pwm);
extern void BSP_SoftPWM_Start(softpwm_name_e pwm);
extern void BSP_SoftPWM_Check(void);
#endif
