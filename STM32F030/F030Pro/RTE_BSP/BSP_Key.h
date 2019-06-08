#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "stm32f0xx.h"
#include "RTE_Include.h"
#define KEY_FIFO_SIZE 16
#define KEY_FILTER_TIME 25
#define KEY_LONGPRESS_ENABLE 1
#if KEY_LONGPRESS_ENABLE
#define KEY_LONGPRESS_TIME 500
#endif
typedef enum
{
    KEY_0 = 0,
	KEY_1 = 1,
    KEY_N,
}key_name_e;
typedef enum
{
    KEY_NONE = 0,
    KEY_PRESS = 1,
    KEY_RELEASE = 2,
#if KEY_LONGPRESS_ENABLE
    KEY_LONG = 3,
#endif
}key_state_e;
typedef struct
{
    key_state_e *Key_Buffer;
    uint8_t WritePointer;
    uint8_t ReadPointer;
}key_fifo_t;
typedef enum
{
    KEY_SM_POLL = 0,
    KEY_SM_PRESS = 1,
    KEY_SM_RELEASE = 2,
}key_sm_e;
typedef struct
{
    key_name_e KeyName;
    uint8_t KeyPressValue;
	GPIO_TypeDef* KeyPort; // Port
	uint16_t KeyPin; // Pin
	uint32_t KeyClk; // Clock
#if KEY_LONGPRESS_ENABLE
    uint32_t KeyStartTick;
#endif
    key_sm_e KeySM;
    key_fifo_t KeyFIFO;
}BSP_Key_Handle_t;
extern void BSP_Key_Init(void);
extern bool BSP_Key_ReadInstant(key_name_e KeyName);
extern void BSP_Key_ClearBuffer(key_name_e KeyName);
extern key_state_e BSP_Key_Read(key_name_e KeyName);
extern void BSP_Key_Timer_Callback(void* UserParameters);
#endif
