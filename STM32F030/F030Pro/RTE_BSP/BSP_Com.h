#ifndef __BSP_COM_H
#define __BSP_COM_H
#include "stm32f0xx.h"
#include "RTE_Include.h"
typedef enum
{
	COM_1 = 0,   // COM1 (TX=PA9, RX=PA10)
	COM_2 = 1,   // COM1 (TX=PA2, RX=PA3)
	COM_N        // COMNum
}com_name_e;
//--------------------------------------------------------------
// 串口数据接收结构体
//--------------------------------------------------------------
typedef struct
{
	//结构体变量
	uint8_t *pu8Databuf;
	uint16_t u16Datalength;
#if RTE_USE_STREAM == 1
	stream_t ComStream;  //串口数据环形队列
#endif
}com_data_t;
typedef struct {
	com_name_e ComName;    // Name
	USART_TypeDef* USARTx;
	GPIO_TypeDef* ComPort; // Port
	uint8_t TxPinsource;
	uint8_t RxPinsource;
	uint8_t ComAF;
	uint16_t ComPintx; // Pin
	uint16_t ComPinrx; // Pin
	uint32_t GpioClk; // Clock
	DMA_Channel_TypeDef* DMARXChannel;
	uint16_t DataBufferLen;
	uint16_t DataQueneLen;
	com_data_t ComData;
}com_t;
/* Wait for TX empty */
#define USART_TXEMPTY(USARTx)               ((USARTx->ISR & USART_FLAG_TXE))
#define USART_WAIT(USARTx)                  while (!USART_TXEMPTY(USARTx))
#define USART_TX_REG(USARTx)                ((USARTx)->TDR)
#define USART_WRITE_DATA(USARTx, data)      ((USARTx)->TDR = (data))
extern void BSP_Com_Init(com_name_e usart_name,uint32_t baudrate);
extern void BSP_Com_Putc(com_name_e usart_name, volatile char c);
extern void BSP_Com_Puts(com_name_e usart_name, char* str);
extern void BSP_Com_Send(com_name_e usart_name, uint8_t* DataArray, uint32_t count);
extern com_data_t *BSP_Com_ReturnQue(com_name_e usart_name);
#endif
