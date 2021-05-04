
#include "include.h"
#include "uart4.h"
#include "hw_config.h"


u8 uart4_recv_buf[UART4_BUF_LEN];
u8 uart4_send_buf[UART4_BUF_LEN];
u16 uart4_recv_fifo_in = 0;
u16 uart4_recv_fifo_out = 0;

u16 uart4_send_cnt;
u16 uart4_send_index;


volatile u8 uart4_stat = 0;  // 1:send now    0:idle

u8 uart4_busy()
{
	return uart4_stat;
}

void uart4_clr_busy()
{
	uart4_stat = 0;
}

void uart4_set_busy()
{
	uart4_stat = 1;
}


void uart4_config()
{
  ALARM485_INPUT();

  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_2;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART4, &USART_InitStructure); 
  USART_Cmd(UART4, ENABLE);

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

}

void uart4_write_char(u8 ch)
{
   USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
   USART_SendData(UART4, ch);
   while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
   {}
   USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

void uart4_write_buf(uint8_t *buf, uint16_t length)
{
    u32 tick;
#if 0
  uint16_t cnt = 0;
  while(cnt < length)
  {
    uart4_write_char(buf[cnt++]);
  }
#else
    uart4_set_busy();
    //LED_VCOM_COM_OPEN();
    LED_VCOM_COM_CLOSE();
    ALARM485_OUTPUT();
    for (tick=0; tick<0x3FF; tick++);
    memcpy( uart4_send_buf, buf, length );
    uart4_send_index = 0;
    uart4_send_cnt = length-1;
    USART_SendData(UART4, uart4_send_buf[uart4_send_index++]);
    USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
    //uart4_recv_fifo_in = 0;
    //uart4_recv_fifo_out = 0;
#endif
}

uint8_t uart4_read_char(uint8_t *ch)
{ 
    if(uart4_recv_fifo_out == uart4_recv_fifo_in)
	return 0;
    *ch = uart4_recv_buf[uart4_recv_fifo_out++];
    if(uart4_recv_fifo_out == UART4_BUF_LEN)
	uart4_recv_fifo_out = 0;
    return 1;
}


