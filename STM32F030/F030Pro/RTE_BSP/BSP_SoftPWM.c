#include "BSP_SoftPWM.h"
static softpwm_t SoftPWMHandle[SOFTPWM_N] = 
{
	{SOFTPWM_1,GPIOB,GPIO_Pin_1,RCC_AHBPeriph_GPIOA},
	{SOFTPWM_2,GPIOA,GPIO_Pin_11,RCC_AHBPeriph_GPIOA},
	{SOFTPWM_3,GPIOA,GPIO_Pin_12,RCC_AHBPeriph_GPIOA},
	{SOFTPWM_4,GPIOA,GPIO_Pin_15,RCC_AHBPeriph_GPIOA},
};
static void SoftPWMFreTimer_Callback(void *arg)
{
	softpwm_name_e pwm = *(softpwm_name_e *)arg;
	GPIO_SetBits(SoftPWMHandle[pwm].PWMPort,SoftPWMHandle[pwm].PWMPin);
}
void BSP_SoftPWM_Init(softpwm_name_e pwm,uint32_t Fre,uint8_t Period)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	// Clock Enable
	RCC_AHBPeriphClockCmd(SoftPWMHandle[pwm].PWMClk, ENABLE);
	// Config als Digital-Ausgang
	GPIO_InitStructure.GPIO_Pin = SoftPWMHandle[pwm].PWMPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SoftPWMHandle[pwm].PWMPort, &GPIO_InitStructure);
	GPIO_ResetBits(SoftPWMHandle[pwm].PWMPort,SoftPWMHandle[pwm].PWMPin);
	//创建PWM定时器
	uint32_t ARR = 100000/Fre;
	SoftPWMHandle[pwm].PWMFre = ARR;
	SoftPWMHandle[pwm].PWMPer = ARR - Period*ARR/100;
	SoftPWMHandle[pwm].TimerID = RoundRobin_GetTimerNum();
	RoundRobin_CreateTimer(SoftPWMHandle[pwm].TimerID,SoftPWMHandle[pwm].PWMFre,1,0,SoftPWMFreTimer_Callback,(void *)&SoftPWMHandle[pwm].name);
}
void BSP_SoftPWM_SetPer(softpwm_name_e pwm,uint8_t Period)
{
	uint32_t ARR = SoftPWMHandle[pwm].PWMFre;
	SoftPWMHandle[pwm].PWMPer = ARR - Period*ARR/100;
}
void BSP_SoftPWM_Stop(softpwm_name_e pwm)
{
	RoundRobin_PauseTimer(SoftPWMHandle[pwm].TimerID);
	SoftPWMHandle[pwm].Output = false;
	GPIO_ResetBits(SoftPWMHandle[pwm].PWMPort,SoftPWMHandle[pwm].PWMPin);
}
void BSP_SoftPWM_Start(softpwm_name_e pwm)
{
	GPIO_SetBits(SoftPWMHandle[pwm].PWMPort,SoftPWMHandle[pwm].PWMPin);
	RoundRobin_ResumeTimer(SoftPWMHandle[pwm].TimerID);
	SoftPWMHandle[pwm].Output = true;
}
void BSP_SoftPWM_Check(void)
{
	for(softpwm_name_e pwm = SOFTPWM_1;pwm<SOFTPWM_N;pwm++)
		if(SoftPWMHandle[pwm].Output)
			if(RoundRobin_GetTimerCNT(SoftPWMHandle[pwm].TimerID) == SoftPWMHandle[pwm].PWMPer)
				GPIO_ResetBits(SoftPWMHandle[pwm].PWMPort,SoftPWMHandle[pwm].PWMPin);

}
