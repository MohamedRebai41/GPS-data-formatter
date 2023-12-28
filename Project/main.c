/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "globals.h"
#include "utils.h"

/* Private typedef -----------------------------------------------------------*/


char Receive_Buffer[800]; 
char Transmit_Buffer[300];
char Time[20] = "\0";
char Date[10]= "\0";
char GPS_Pos_Valid[10]= "\0";
char Longitude[15]= "\0";
char Altitude[10]= "\0";
char Nbr_Satellites[5]= "\0";

// Structures
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;

 
 
/* Private functions ---------------------------------------------------------*/
void Delay(vu32 nCount);

void Config_USART1_RX_WITH_DMA (void);
void Config_Button_Interrupt(void);
void Config_USART2_TX_WITH_DMA(void);

int main(void)
{
	

	
/* ================  Configure the Periph Clocks ===================== */

  /* Enable AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* Enable USART2 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	/*Enable GPIOA and GPIOC clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	/* Enable USART1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
	
  Config_Button_Interrupt();
	Config_USART2_TX_WITH_DMA();	
	Config_USART1_RX_WITH_DMA();	
	
	//Config the Systick to generate interruptions 	each minute
		SysTick_Config(SystemCoreClock/10);

	
	
	
	

 while(1)
 {
	}

}//END MAIN                

  
/******************** Function Name  : Delay**************************/
void Delay(vu32 nCount)
{  for(; nCount != 0; nCount--);}



void Config_Button_Interrupt(void)
{
	 //Configure and enable GPIO

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
	 
	
	 //Configure EXTI
	 GPIO_EXTILineConfig( GPIO_PortSourceGPIOC, GPIO_PinSource13);
	 EXTI_InitStructure.EXTI_Line = EXTI_Line13;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);

	
	
	
	 //Configure NVIC
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn  ;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
   NVIC_Init(&NVIC_InitStructure);
	
}



void Config_USART2_TX_WITH_DMA(void)
{

  /* Configure USART2 Tx as output AF */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


/* ====================  USART2 configuration ======================*/
    /*  - BaudRate = 9600 baud - Word Length = 8 Bits- One Stop Bit - No parity
        - Transmission enabled   */
	
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
  	
	/*Enable  Tx DMA Request (s) on USART2*/
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	
	

	
/* ====================  DMA configuration ======================*/

		//DMA1 channel7 (UART2 TX) configuration */
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &USART2->DR;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Transmit_Buffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_BufferSize = sizeof Transmit_Buffer;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
		DMA_Init(DMA1_Channel7, &DMA_InitStructure);

	 /* Enable DMAx Channely Transfer Complete/ Half Transfer interrupts */
	 DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
   /* Configure NVIC for DMA  */
   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
}





void Config_USART1_RX_WITH_DMA (void)
{

  /* Configure USART1 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


/* ====================  USART1 configuration ======================*/
    /*  - BaudRate = 9600 baud - Word Length = 8 Bits- One Stop Bit - No parity
        - Receive enabled   */
	
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;

  USART_Init(USART1, &USART_InitStructure);
  	
	/*Enable  Rx DMA Request (s) on USART1*/
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	
	/*Enable RXNE interrupt*/
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);



		/*configure NVIC for RXNE*/
	 NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure); 





  /* Enable the USART1*/
  USART_Cmd(USART1, ENABLE);
	
	
	
	
	
/* ====================  DMA configuration ======================*/

		//DMAx channelx (UART1 RX) configuration */
		
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &USART1->DR;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Receive_Buffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = sizeof Receive_Buffer;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
		
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);






}
	