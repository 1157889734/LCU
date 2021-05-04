/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : platform_config.h
* Author             : MCD Application Team
* Version            : V3.0.0
* Date               : 04/06/2009
* Description        : Evaluation board specific configuration file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */

  //#define USB_DISCONNECT                      GPIOA 
  //#define USB_DISCONNECT_PIN                  GPIO_Pin_2//GPIO_Pin_2
  #define USB_DISCONNECT                      GPIOB 
  #define USB_DISCONNECT_PIN                  GPIO_Pin_9//GPIO_Pin_2
  
  #define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOA
  #define RCC_APB2Periph_GPIO_KEY             RCC_APB2Periph_GPIOB

  #define GPIO_Pin_KEY                        GPIO_Pin_7   /* PB.07 */
  #define GPIO_Pin_UP                         GPIO_Pin_14  /* PC.14 */
  #define GPIO_Pin_DOWN                       GPIO_Pin_0   /* PA.00 */
  #define GPIO_Pin_LEFT                       GPIO_Pin_15  /* PC.15 */
  #define GPIO_Pin_RIGHT                      GPIO_Pin_13  /* PC.13 */ 

  #define RCC_APB2Periph_GPIO_JOY_SET1        RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_JOY_SET2        RCC_APB2Periph_GPIOA

  #define GPIO_RIGHT                          GPIOC
  #define GPIO_LEFT                           GPIOC
  #define GPIO_DOWN                           GPIOA
  #define GPIO_UP                             GPIOC
  #define GPIO_KEY                            GPIOB

  #define GPIO_KEY_PORTSOURCE                 GPIO_PortSourceGPIOB
  #define GPIO_KEY_PINSOURCE                  GPIO_PinSource7
  #define GPIO_KEY_EXTI_Line                  EXTI_Line7

  #define EXTI_KEY_IRQChannel                 EXTI9_5_IRQn

  #define RCC_APB2Periph_ALLGPIO             (RCC_APB2Periph_GPIOA \
                                              | RCC_APB2Periph_GPIOB \
                                              | RCC_APB2Periph_GPIOC \
                                              | RCC_APB2Periph_GPIOD \
                                              | RCC_APB2Periph_GPIOE |RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG )

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

