
#include "include.h"
#include "uart3.h"
#include "hw_config.h"


u8 uart3_recv_buf[UART3_BUF_LEN];
u8 uart3_send_buf[UART3_BUF_LEN];
u16 uart3_recv_fifo_in = 0;
u16 uart3_recv_fifo_out = 0;

void uart3_config()
{
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure); 
  USART_Cmd(USART3, ENABLE);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

void uart3_write_char(u8 ch)
{
   USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
   USART_SendData(USART3, ch);
   while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
   {}
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}
void uart3_write_buf(u8 *buf, u16 length)
{
  u16 cnt = 0;
  while(cnt < length)
  {
    uart3_write_char(buf[cnt++]);
  }
}

u8 uart3_read_char(u8 *ch)
{
    if(uart3_recv_fifo_out == uart3_recv_fifo_in)
	return 0;
    *ch = uart3_recv_buf[uart3_recv_fifo_out++];
	
    if(uart3_recv_fifo_out == UART3_BUF_LEN)
	uart3_recv_fifo_out = 0;
    return 1;
}
