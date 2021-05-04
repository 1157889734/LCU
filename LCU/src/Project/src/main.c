/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理车辆总线数据，其他模块获取车辆总线数据都从这里获得
**  创建日期: 2013.12.19
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/


#include "include.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "stm32f10x_dbgmcu.h"
#include "led_control.h"
#include "data_usb.h"
#include "data_ehp.h"
#include "data_train.h"
#include "data_car.h"
#include "led_control.h"


void Delay(__IO uint32_t nCount);

/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void startWatchdog(void)
{
	DBGMCU_Config( DBGMCU_CR_DBG_IWDG_STOP, DISABLE);

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32);	
	IWDG_SetReload(0xfff);
	IWDG_ReloadCounter();	
	IWDG_Enable();	
	//IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
}

void resetWatchDog(void)
{
	IWDG_ReloadCounter();
}


//////////////////////////////////////////////////////////////////
void NVIC_Configuration()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


  //USART1中断配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  //USART3中断配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  //USART2中断配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  //UART4中断配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  


  /* used for USB resume interrupt */
  EXTI_ClearITPendingBit(EXTI_Line18);  
  EXTI_InitStructure.EXTI_Line = EXTI_Line18; 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_ClearITPendingBit(GPIO_KEY_EXTI_Line);
}


void RCC_Configuration(void)
{	
  ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

	//设置ADC时钟频率
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);
    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
    
  //为各个模块开启时钟
  //GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |RCC_APB2Periph_GPIOF, ENABLE);
  //AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  //USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  //USART2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  //USART3时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  //UART4时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

}


void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure USART1 Tx (PA.09) as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  // Configure USART1 Rx (PA.10) as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

   // Configure USART2 Tx (PA.2) as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  // Configure USART2 Rx (PA.3) as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

   // Configure USART3 Tx (PB.10) as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  // Configure USART3 Rx (PB.11) as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

   // Configure USART4 Tx (PC.10) as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  // Configure USART4 Rx (PC.11) as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
 
  
  //usb
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

  //led  output
  GPIO_InitStructure.GPIO_Pin = LED_RUN_BIT | LED_ALARM_BIT | LED_TRAIN_COM_BIT | LED_EHP_COM_BIT | LED_LISTEN_TALK_BIT | LED_VCOM_COM_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  //alarm talk control
  GPIO_InitStructure.GPIO_Pin = LISTEN_CTRL_BIT | SPEAK_CTRL_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //ptt control
  GPIO_InitStructure.GPIO_Pin = PTT_CTRL_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //train485 output
  GPIO_InitStructure.GPIO_Pin = TRAIN485_CTRL_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //car485 output
  GPIO_InitStructure.GPIO_Pin = CAR485_CTRL_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //alarm485 output
  GPIO_InitStructure.GPIO_Pin = ALARM485_CTRL_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);


//------------------------------------------------------------------------------------------------
// below input 

  //addr
  GPIO_InitStructure.GPIO_Pin = ADDR0_BIT | ADDR1_BIT | ADDR2_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  //ptt check
  GPIO_InitStructure.GPIO_Pin = PTT_CHK_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
	SPEAK_CTRL_DISCONN();
	LISTEN_CTRL_DISCONN();
  PTT_CTRL_FREE();
		LED_RUN_CLOSE();
		LED_ALARM_CLOSE();
		LED_TRAIN_COM_CLOSE();
		LED_EHP_COM_CLOSE();
		LED_VCOM_COM_CLOSE();
		LED_LISTEN_TALK_CLOSE();
  ALARM485_INPUT();
  CAR485_INPUT();
  TRAIN485_INPUT();
  
}




void SysStatInit()
{
	SetDeviceId();

		LED_RUN_CLOSE();
		LED_ALARM_CLOSE();
		LED_TRAIN_COM_CLOSE();
		LED_EHP_COM_CLOSE();
		LED_VCOM_COM_CLOSE();
		LED_LISTEN_TALK_CLOSE();
}


extern void ptu_proc(void);
////////////////////////////////////////////////////////////////////
int main(void)
{
//  注意: 请谨慎修改初始化顺序。后面的以前面的为基础

	static unsigned long tick;

	RCC_Configuration();

	NVIC_Configuration();
	SysTick_Configuration();
	
	GPIO_Configuration();

	tick = gSystemTick;
	while(gSystemTick-tick<500);
	
#if 1	
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
#endif
	SysStatInit();

	//train
	uart1_config();
	
	//car
	uart2_config();

	//vau
	uart3_config();
	
	//led 485
	uart4_config();
	

	EhpDataInit();
	TrainDataInit();
	CarDataInit();

	LedDispDeviceId();

	MapInit();

	startWatchdog();     
    
	while (1)
	{
		TrainDataProc();
		CarDataProc();    // LED 
		EhpDataProc();  // VAU 合并到PECU 485
		UsbDataProc();
		//Vau_Proc();   //VAU
		//LedProc();
		resetWatchDog();
	}
}


/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
  for (; nCount != 0;nCount--);
}

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
