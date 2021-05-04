

#include "usb_lib.h"
#include "uart1.h"
#include "stm32f10x_usart.h"
#include "hw_config.h"

uint8_t uart1_recv_buf[UART1_BUF_LEN];
uint8_t uart1_send_buf[UART1_BUF_LEN];
uint16_t uart1_recv_fifo_in = 0;
uint16_t uart1_recv_fifo_out =0;

uint16_t uart1_send_cnt;
uint16_t uart1_send_index;

volatile u8 uart1_stat = 0;  // 1:send now    0:idle

u8 uart1_busy()
{
	return uart1_stat;
}

void uart1_set_busy()
{
	uart1_stat = 1;
}

void uart1_clr_busy()
{
	uart1_stat = 0;
}


void uart1_config()
{
  TRAIN485_INPUT();

  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure); 
  USART_Cmd(USART1, ENABLE);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void uart1_write_char(uint8_t ch)
{
   USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
   USART_SendData(USART1, ch);
   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
   {}
   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void uart1_write_buf(uint8_t *buf, uint16_t length)
{
#if 0
  uint16_t cnt = 0;
  while(cnt < length)
  {
    uart1_write_char(buf[cnt++]);
  }
#else
	uart1_set_busy();
	LED_TRAIN_COM_OPEN();
	TRAIN485_OUTPUT();
	memcpy( uart1_send_buf, buf, length );
	uart1_send_index = 0;
	uart1_send_cnt = length-1;
	USART_SendData(USART1, uart1_send_buf[uart1_send_index++]);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

#endif
}

uint8_t uart1_read_char(uint8_t *ch)
{ 
    if(uart1_recv_fifo_out == uart1_recv_fifo_in)
	return 0;
    *ch = uart1_recv_buf[uart1_recv_fifo_out++];
    if(uart1_recv_fifo_out == UART1_BUF_LEN)
	uart1_recv_fifo_out = 0;
    return 1;
}



