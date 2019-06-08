#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f0xx.h"
#include "RTE_Include.h"
/* Exported types ------------------------------------------------------------*/
//--------------------------------------------------------------
// Liste aller LEDs
// (keine Nummer doppelt und von 0 beginnend)
//--------------------------------------------------------------
typedef enum 
{
	LED_0=0,  			 // LED0
	LED_N,
}led_name_e;
//--------------------------------------------------------------
// Status einer LED
//--------------------------------------------------------------
typedef enum {
	LED_OFF = 0,  // LED AUS
	LED_ON        // LED EIN
}led_status_e;
//--------------------------------------------------------------
// Struktur einer LED
//--------------------------------------------------------------
typedef struct {
	led_name_e LedName;    // Name
	GPIO_TypeDef* LedPort; // Port
	uint16_t LedPin; // Pin
	uint32_t LedClk; // Clock
	led_status_e LedInitStatus;  // Init
}led_t;
void BSP_Led_Init(void);
void BSP_Led_Toggle(led_name_e led_name);
void BSP_Led_On(led_name_e led_name);
void BSP_Led_Off(led_name_e led_name);
#endif

