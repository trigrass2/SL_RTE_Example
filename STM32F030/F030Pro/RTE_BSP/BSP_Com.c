#include "BSP_Com.h"
static com_t ComHandle[COM_N]=
{
  // Name  ,                           
  {
		COM_1 ,USART1 ,GPIOA, GPIO_PinSource9, GPIO_PinSource10, GPIO_AF_1, 
		GPIO_Pin_9 , GPIO_Pin_10 , RCC_AHBPeriph_GPIOA,
		DMA1_Channel3,
		64,4,
	}, 
};
static void BSP_Com_HardInit(com_name_e usart_name,uint32_t baudrate)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	if(ComHandle[usart_name].USARTx==USART1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	else if(ComHandle[usart_name].USARTx==USART2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(ComHandle[usart_name].USARTx); 

	RCC_AHBPeriphClockCmd(ComHandle[usart_name].GpioClk , ENABLE);
	/* Connect pin to Periph */
	GPIO_PinAFConfig(ComHandle[usart_name].ComPort, ComHandle[usart_name].TxPinsource, ComHandle[usart_name].ComAF); 
	GPIO_PinAFConfig(ComHandle[usart_name].ComPort, ComHandle[usart_name].RxPinsource, ComHandle[usart_name].ComAF); 
	GPIO_InitStructure.GPIO_Pin = ComHandle[usart_name].ComPintx|ComHandle[usart_name].ComPinrx; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(ComHandle[usart_name].ComPort, &GPIO_InitStructure); 

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx| USART_Mode_Rx;
	USART_Init(ComHandle[usart_name].USARTx, &USART_InitStructure);  
	USART_ClearFlag(ComHandle[usart_name].USARTx, USART_FLAG_TC); 
}
static void BSP_Com_InterruptInit(com_name_e usart_name)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_ITConfig(ComHandle[usart_name].USARTx, USART_IT_IDLE, ENABLE);
	if (ComHandle[usart_name].USARTx == USART1) {
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	}
	else if(ComHandle[usart_name].USARTx == USART2) {
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	}	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
}
void BSP_Com_Putc(com_name_e usart_name, volatile char c) {
	/* Wait to be ready, buffer empty */
	USART_WAIT(ComHandle[usart_name].USARTx);
	/* Send data */
	USART_WRITE_DATA(ComHandle[usart_name].USARTx, (uint16_t)(c & 0x01FF));
	/* Wait to be ready, buffer empty */
	USART_WAIT(ComHandle[usart_name].USARTx);
}
void BSP_Com_Puts(com_name_e usart_name, char* str) {
	/* Go through entire string */
	while (*str) {
		/* Wait to be ready, buffer empty */
		USART_WAIT(ComHandle[usart_name].USARTx);
		/* Send data */
		USART_WRITE_DATA(ComHandle[usart_name].USARTx, (uint16_t)(*str++));
		/* Wait to be ready, buffer empty */
		USART_WAIT(ComHandle[usart_name].USARTx);
	}
}
void BSP_Com_Send(com_name_e usart_name, uint8_t* Data, uint32_t count) {
	/* Go through entire data  */
	while (count--) {
		/* Wait to be ready, buffer empty */
		USART_WAIT(ComHandle[usart_name].USARTx);
		/* Send data */
		USART_WRITE_DATA(ComHandle[usart_name].USARTx, (uint16_t)(*Data++));
		/* Wait to be ready, buffer empty */
		USART_WAIT(ComHandle[usart_name].USARTx);
	}
}
static void BSP_Com_DMAInit(com_name_e usart_name)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	DMA_DeInit(ComHandle[usart_name].DMARXChannel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ComHandle[usart_name].USARTx->RDR;    //DMA外设基地址 串口 具体参考手册
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ComHandle[usart_name].ComData.pu8Databuf;   //DMA内存基地址 存放DMA数据 接收
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;        //数据传输方向，从外设到内存（接收数据）
	DMA_InitStructure.DMA_BufferSize = ComHandle[usart_name].DataBufferLen;              //接收DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;             //正常模式，即满了就不在接收了，而不是循环存储
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;   //同DMA优先级很高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;              //DMA通道内存与外设传输
	DMA_Init(ComHandle[usart_name].DMARXChannel, &DMA_InitStructure);              //初始化DMA
	
	DMA_RemapConfig(DMA1, DMA1_CH3_USART1_RX);
	USART_DMACmd(ComHandle[usart_name].USARTx, USART_DMAReq_Rx, ENABLE);// 使能DMA串口接受请求
	DMA_Cmd(ComHandle[usart_name].DMARXChannel, ENABLE);                           //启动DMA
} 
static void BSP_Com_DMARecReset(com_name_e usart_name)
{
	DMA_Cmd(ComHandle[usart_name].DMARXChannel, DISABLE);
	ComHandle[usart_name].DMARXChannel->CNDTR = ComHandle[usart_name].DataBufferLen ;
	DMA_Cmd(ComHandle[usart_name].DMARXChannel, ENABLE);
}
void BSP_Com_Init(com_name_e usart_name,uint32_t baudrate) {
	//申请内存
	ComHandle[usart_name].ComData.pu8Databuf = Memory_Alloc0(MEM_RTE,ComHandle[usart_name].DataBufferLen);
	ComHandle[usart_name].ComData.u16Datalength = 0;
#if RTE_USE_STREAM == 1
	Stream_Init(&ComHandle[usart_name].ComData.ComStream,ComHandle[usart_name].DataQueneLen);
#endif
	BSP_Com_HardInit(usart_name,baudrate);
	BSP_Com_InterruptInit(usart_name);
	BSP_Com_DMAInit(usart_name);
	USART_Cmd(ComHandle[usart_name].USARTx, ENABLE);	
}
com_data_t *BSP_Com_ReturnQue(com_name_e usart_name)
{
	return &ComHandle[usart_name].ComData;
}
static void BSP_Com_RecCallback(com_name_e usart_name)
{
	ComHandle[usart_name].USARTx->ISR;
	ComHandle[usart_name].USARTx->RDR;
	ComHandle[usart_name].ComData.u16Datalength = ComHandle[usart_name].DataBufferLen - ComHandle[usart_name].DMARXChannel->CNDTR;            //可以接收的总长度减去剩余长度，得到接收到数据的长度
	if(ComHandle[usart_name].ComData.u16Datalength)
	{
#if RTE_USE_SHELL == 1
		if(usart_name == COM_1)
		{
			Shell_Input(ComHandle[usart_name].ComData.pu8Databuf,
				ComHandle[usart_name].ComData.u16Datalength);
		}
		else
#endif
			Stream_Enquene(&ComHandle[usart_name].ComData.ComStream,ComHandle[usart_name].ComData.pu8Databuf,
				ComHandle[usart_name].ComData.u16Datalength);
	}
	memset(ComHandle[usart_name].ComData.pu8Databuf,0,ComHandle[usart_name].ComData.u16Datalength);
	ComHandle[usart_name].ComData.u16Datalength = 0;
	BSP_Com_DMARecReset(usart_name);
	USART_ClearITPendingBit(ComHandle[usart_name].USARTx, USART_IT_IDLE);    //清除串口空闲
}
void USART1_IRQHandler(void) {
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		BSP_Com_RecCallback(COM_1);
	}
}
void USART2_IRQHandler(void) {
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		BSP_Com_RecCallback(COM_2);
	}
}
