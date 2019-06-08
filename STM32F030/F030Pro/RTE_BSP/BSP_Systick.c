#include "BSP_Systick.h"
//10us
void BSP_TimeBase_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 9;
	TIM_TimeBaseStructure.TIM_Prescaler = 47;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	/* TIM Interrupts enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}
void BSP_Systick_Init(void)
{
	SysTick_Config(SystemCoreClock / 100000);	//Set SysTick Timer for 10us interrupts
}
#if RTE_USE_OS == 0
void SysTick_Handler(void)
{
//	static uint32_t WDTTick = 0;
//	WDTTick++;
//	if(WDTTick == 4000)
//	{
//		WWDG_SetCounter(127);
//		WDTTick=0;
//	}
	RoundRobin_TickHandler();
}
/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		static uint32_t WDTTick = 0;
		WDTTick++;
		if(WDTTick == 4000)
		{
			WWDG_SetCounter(127);
			WDTTick=0;
		}
		RoundRobin_TickHandler();
	}
}
#endif
