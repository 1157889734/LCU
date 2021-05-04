
#include "include.h"
#include "uart2.h"
#include "hw_config.h"


u8 uart2_recv_buf[UART2_BUF_LEN];
u8 uart2_send_buf[UART2_BUF_LEN];
u16 uart2_recv_fifo_in = 0;
u16 uart2_recv_fifo_out = 0;

uint16_t uart2_send_cnt;
uint16_t uart2_send_index;


volatile u8 uart2_stat = 0;  // 1:busy send now    0:idle

u8 uart2_busy()
{
	return uart2_stat;
}

void uart2_clr_busy()
{
	uart2_stat = 0;
}

void uart2_set_busy()
{
	uart2_stat = 1;
}

void uart2_config()
{
  CAR485_INPUT();

  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure); 
  USART_Cmd(USART2, ENABLE);

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

}

void uart2_write_char(u8 ch)
{
   USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
   USART_SendData(USART2, ch);
   while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
   {}
   USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void uart2_write_buf(u8 *buf, u16 length)
{
#if 0
  u16 cnt = 0;
  while(cnt < length)
  {
    uart2_write_char(buf[cnt++]);
  }
#else
	uart2_set_busy();
       //LED_EHP_COM_OPEN();
       LED_EHP_COM_CLOSE();
       CAR485_OUTPUT();
	memcpy( uart2_send_buf, buf, length );	
	uart2_send_index = 0;
	uart2_send_cnt = length-1;
	USART_SendData(USART2, uart2_send_buf[uart2_send_index++]);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
#endif
}

u8 uart2_read_char(u8 *ch)
{
    if(uart2_recv_fifo_out == uart2_recv_fifo_in)
	return 0;
    *ch = uart2_recv_buf[uart2_recv_fifo_out++];
    if(uart2_recv_fifo_out == UART2_BUF_LEN)
	uart2_recv_fifo_out = 0;
    return 1;
}
