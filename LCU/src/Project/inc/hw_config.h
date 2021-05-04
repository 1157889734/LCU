/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : hw_config.h
* Author             : MCD Application Team
* Version            : V3.0.0
* Date               : 04/06/2009
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define DOWN            1
//#define LEFT            2
//#define RIGHT           3
#define UP              4
#define CURSOR_STEP     30




#define TRAIN485_CTRL_GPIO				GPIOA
#define TRAIN485_CTRL_BIT				GPIO_Pin_8
#define TRAIN485_OUTPUT()				GPIO_SetBits(TRAIN485_CTRL_GPIO, TRAIN485_CTRL_BIT)
#define TRAIN485_INPUT()				GPIO_ResetBits(TRAIN485_CTRL_GPIO, TRAIN485_CTRL_BIT)

#define CAR485_CTRL_GPIO				GPIOA
#define CAR485_CTRL_BIT				GPIO_Pin_1
#define CAR485_OUTPUT()				GPIO_SetBits(CAR485_CTRL_GPIO, CAR485_CTRL_BIT)
#define CAR485_INPUT()					GPIO_ResetBits(CAR485_CTRL_GPIO, CAR485_CTRL_BIT)

#define ALARM485_CTRL_GPIO			GPIOC
#define ALARM485_CTRL_BIT				GPIO_Pin_12
#define ALARM485_OUTPUT()				GPIO_SetBits(ALARM485_CTRL_GPIO, ALARM485_CTRL_BIT)
#define ALARM485_INPUT()				GPIO_ResetBits(ALARM485_CTRL_GPIO, ALARM485_CTRL_BIT)

//运行灯
#define LED_RUN_GPIO					GPIOD
#define LED_RUN_BIT						GPIO_Pin_13
#define LED_RUN_OPEN()					GPIO_SetBits(LED_RUN_GPIO, LED_RUN_BIT)
#define LED_RUN_CLOSE()					GPIO_ResetBits(LED_RUN_GPIO, LED_RUN_BIT)

//动态地图
#define LED_VCOM_COM_GPIO				GPIOD //
#define LED_VCOM_COM_BIT				GPIO_Pin_10
#define LED_VCOM_COM_OPEN()			GPIO_SetBits(LED_VCOM_COM_GPIO, LED_VCOM_COM_BIT)
#define LED_VCOM_COM_CLOSE()			GPIO_ResetBits(LED_VCOM_COM_GPIO, LED_VCOM_COM_BIT)

//报警
#define LED_ALARM_GPIO					GPIOD
#define LED_ALARM_BIT					GPIO_Pin_12
#define LED_ALARM_OPEN()				GPIO_SetBits(LED_ALARM_GPIO, LED_ALARM_BIT)
#define LED_ALARM_CLOSE()				GPIO_ResetBits(LED_ALARM_GPIO, LED_ALARM_BIT)


//列车485总线数据
#define LED_TRAIN_COM_GPIO			GPIOD //
#define LED_TRAIN_COM_BIT				GPIO_Pin_9
#define LED_TRAIN_COM_OPEN()			GPIO_SetBits(LED_TRAIN_COM_GPIO, LED_TRAIN_COM_BIT)
#define LED_TRAIN_COM_CLOSE()			GPIO_ResetBits(LED_TRAIN_COM_GPIO, LED_TRAIN_COM_BIT)


//紧急报警器通讯指示灯
#define LED_EHP_COM_GPIO				GPIOD
#define LED_EHP_COM_BIT				GPIO_Pin_11
#define LED_EHP_COM_OPEN()				GPIO_SetBits(LED_EHP_COM_GPIO, LED_EHP_COM_BIT)
#define LED_EHP_COM_CLOSE()			GPIO_ResetBits(LED_EHP_COM_GPIO, LED_EHP_COM_BIT)


//听讲灯
#define LED_LISTEN_TALK_GPIO			GPIOD
#define LED_LISTEN_TALK_BIT			GPIO_Pin_11
#define LED_LISTEN_TALK_OPEN()			GPIO_SetBits(LED_LISTEN_TALK_GPIO, LED_LISTEN_TALK_BIT)
#define LED_LISTEN_TALK_CLOSE()		GPIO_ResetBits(LED_LISTEN_TALK_GPIO, LED_LISTEN_TALK_BIT)



//控制紧急报警器的中心抽头					
#define PTT_CTRL_GPIO					GPIOB
#define PTT_CTRL_BIT					GPIO_Pin_12
#define PTT_CTRL_PULL_DOWN()			GPIO_ResetBits(PTT_CTRL_GPIO, PTT_CTRL_BIT)
#define PTT_CTRL_FREE()					GPIO_SetBits(PTT_CTRL_GPIO, PTT_CTRL_BIT)

//司机听
#define LISTEN_CTRL_GPIO				GPIOC
#define LISTEN_CTRL_BIT					GPIO_Pin_2
#define LISTEN_CTRL_CONN()				GPIO_ResetBits(LISTEN_CTRL_GPIO, LISTEN_CTRL_BIT)
#define LISTEN_CTRL_DISCONN()			GPIO_SetBits(LISTEN_CTRL_GPIO, LISTEN_CTRL_BIT)

//司机讲
#define SPEAK_CTRL_GPIO				GPIOC
#define SPEAK_CTRL_BIT					GPIO_Pin_6
#define SPEAK_CTRL_CONN()				GPIO_ResetBits(SPEAK_CTRL_GPIO, SPEAK_CTRL_BIT)
#define SPEAK_CTRL_DISCONN()			GPIO_SetBits(SPEAK_CTRL_GPIO, SPEAK_CTRL_BIT)

// below input
#define PTT_CHK_GPIO					GPIOC
#define PTT_CHK_BIT						GPIO_Pin_0
#define PTT_CHK()						GPIO_ReadInputDataBit(PTT_CHK_GPIO, PTT_CHK_BIT)

#define ADDR0_GPIO						GPIOF
#define ADDR0_BIT						GPIO_Pin_3
#define ADDR0_READ()					GPIO_ReadInputDataBit(ADDR0_GPIO, ADDR0_BIT)

#define ADDR1_GPIO						GPIOF
#define ADDR1_BIT						GPIO_Pin_1
#define ADDR1_READ()					GPIO_ReadInputDataBit(ADDR1_GPIO, ADDR1_BIT)

#define ADDR2_GPIO						GPIOF
#define ADDR2_BIT						GPIO_Pin_6
#define ADDR2_READ()					GPIO_ReadInputDataBit(ADDR2_GPIO, ADDR2_BIT)





/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void GPIO_AINConfig(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void Joystick_Send(uint8_t Keys);
uint8_t JoyState(void);
void Get_SerialNum(void);

#endif  /*__HW_CONFIG_H*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
