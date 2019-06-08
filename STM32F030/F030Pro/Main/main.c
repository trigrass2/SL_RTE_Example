/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "RTE_Include.h"
#include "BSP_Com.h"
#include "BSP_Led.h"
#include "BSP_Systick.h"
#include "BSP_SoftPWM.h"
static void WWDG_Config(void);
uint8_t per = 100;
static void LEDTimer_Callback(void *arg)
{
//	BSP_Led_Toggle(LED_0);
	per = per - 5;
	if(per==0)
		per = 100;
	BSP_SoftPWM_SetPer(SOFTPWM_1,per);
}
static void RTE_Putc(char data)
{
	BSP_Com_Putc(COM_1,data);
}
int main(void)
{
	/* Check if the system has resumed from WWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	{ 
		/* Clear reset flags */
		RCC_ClearFlag();
	}
	RTE_Init();
	BSP_Com_Init(COM_1,115200);
	Printf_RegPutC(RTE_Putc);
	uprintf("%10s    Run At %d Hz!\r\n","[SYSTEM]",SystemCoreClock);
	KVDB_Init();
#if RTE_USE_SHELL == 1
	RoundRobin_CreateTimer(RoundRobin_GetTimerNum(),1000,1,1,Shell_Poll,(void *)0);
#endif
	RoundRobin_CreateTimer(RoundRobin_GetTimerNum(),50000,1,1,LEDTimer_Callback,NULL);
	BSP_SoftPWM_Init(SOFTPWM_1,1000,100);
	BSP_SoftPWM_Start(SOFTPWM_1);
	WWDG_Config();
	BSP_Led_Init();
	BSP_Systick_Init();
	while (1)
	{
		RoundRobin_Run();
		BSP_SoftPWM_Check();
	}
}
/**
  * @brief  WWDG configuration
  * @param  None
  * @retval None
  */
static void WWDG_Config(void)
{
	/* Enable WWDG clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	/* WWDG clock counter = (PCLK1 (48MHz)/4096)/8 = 1464Hz (~683 us)  */
	WWDG_SetPrescaler(WWDG_Prescaler_8);

	/* Set Window value to 80; WWDG counter should be refreshed only when the counter
	is below 80 (and greater than 64) otherwise a reset will be generated */
	WWDG_SetWindowValue(80);

	/* Enable WWDG and set counter value to 127, WWDG timeout = ~683 us * 64 = 43.7 ms 
	 In this case the refresh window is: ~683 * (127-80)= 32.1ms < refresh window < ~683 * 64 = 43.7ms
	 */
	WWDG_Enable(127);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
