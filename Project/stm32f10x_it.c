// Interupts EXTI Project

/**
  ******************************************************************************
  * @file    EXTI/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    15/09/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */



/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "globals.h"
#include "utils.h"
#include<string.h>
#include<stdio.h>


void Delay(vu32 nCount);
uint8_t stringCompare(char str1[],char str2[], uint8_t StrLength);


/** @addtogroup Examples
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */



//tested this to get one minute with system core clock
int ONE_MINUTE = 900;

int CURR = 0;

void SysTick_Handler(void)
{
	CURR++;
	if(CURR == ONE_MINUTE)
	{
		CURR = 0;
		//Initialize values
		Time[0] = '\0';
		Date[0] = '\0';
		GPS_Pos_Valid[0] = '\0';
		Longitude[0] = '\0';
		Altitude[0] = '\0';
		Nbr_Satellites[0] = '\0';
		memset(Receive_Buffer, '\0', sizeof(Receive_Buffer));
		// enable USART1 to listen for GPS frame
		
		DMA_SetCurrDataCounter(DMA1_Channel5, sizeof Receive_Buffer);
		USART_Cmd(USART1, ENABLE);
		
	}

}






/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles External line0 interrupt request.
  * @param  None
  * @retval None
  */

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetFlagStatus(EXTI_Line13)==SET)
	{
		// To protect against button bouncing
		NVIC_DisableIRQ(EXTI15_10_IRQn);
		Delay(1000000);
		
		
		buildTransmitMessage();

		/* Enable the USART2*/
		USART_Cmd(USART2, ENABLE);
		/*Configure DMA buffer size and enable it*/
		DMA_SetCurrDataCounter(DMA1_Channel7,strlen(Transmit_Buffer));
    

		DMA_Cmd(DMA1_Channel7, ENABLE);

		// Remise à zéro du bit déclenchant l'interruption
		EXTI_ClearITPendingBit(EXTI_Line13);
		
		
		NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles DMA interrupt requests.
  * @param  None
  * @retval None
  */

 
void DMA1_Channel7_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC7))
	{
		/*disable UART2 and DMA channel*/
		DMA_Cmd(DMA1_Channel7, DISABLE);
		USART_Cmd(USART2, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_TC7);  
	}
}





int LAST_POS = 1;
int DMA5_ENABLED = 0;

//Handler for USART1 for RXNE

void USART1_IRQHandler(void)
{	
			
			char received_data = USART1->DR;
			
			
		  //Detect new NMEA sentence
			if(received_data == '$')
			{
			  // Activate the DMA transfer only if we detect a new sentence (the first of the frame)
				if(DMA5_ENABLED == 0){

					DMA5_ENABLED = 1;
					DMA_Cmd(DMA1_Channel5, ENABLE);
					LAST_POS = 1;
				}	
				else 
				{
					// Save the position of the last sentence
					int curr_counter = DMA_GetCurrDataCounter(DMA1_Channel5);
					LAST_POS = sizeof Receive_Buffer - curr_counter; 
					
				}
				//Detect end of NMEA sentence
			} else if(received_data == '\n' && DMA5_ENABLED == 1)
			{
				char Curr_Sentence [90];
				strcpy(Curr_Sentence,Receive_Buffer + LAST_POS);
				extractData(Curr_Sentence);
				if(DataExtracted())
				{
						DMA5_ENABLED = 0;
						DMA_Cmd(DMA1_Channel5,DISABLE);
						USART_Cmd(USART1, DISABLE);
				}
			}
		
	
}



/*void DMA1_Channel5_IRQHandler(void)
{

  if(DMA_GetITStatus(DMA1_IT_TC5))
	{

	}
}*/





/**
  * @brief  This function handles USART1 RX interrupt requests.
  * @param  None
  * @retval None
  */


//int txcount=0;
//extern char Transmit_Buffer[];

//void USART2_IRQHandler(void)
//{
  /* Test on USART2 TX Interrupt */
	
  //if(USART_GetITStatus(USART2, USART_IT_TXE))
  //{
    /* Code a ajouter pour assurer le fonctionnement désiré */

		//txcount++;
		//USART_SendData (USART2, Transmit_Buffer[txcount++]);
   /* Clear USART1 Rx interrupt pending bit */
    //USART_ClearITPendingBit(..............);
	  //if (txcount==7){
			//txcount=0;
			//USART_Cmd(USART2, DISABLE);
		//}
	//}

//}


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
