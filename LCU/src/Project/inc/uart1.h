#ifndef UART1_H
#define UART1_H


#define UART1_BUF_LEN 256



extern u8 uart1_recv_buf[UART1_BUF_LEN];
extern u8 uart1_send_buf[UART1_BUF_LEN];
extern u16 uart1_recv_fifo_in;
extern u16 uart1_recv_fifo_out;
extern u16 uart1_send_cnt;
extern u16 uart1_send_index;

extern void uart1_config();
extern void uart1_write_char(u8 ch);
extern void uart1_write_buf(u8 *buf, u16 length);
extern u8 uart1_read_char(u8 *ch);
extern u8 uart1_busy();
extern void uart1_set_busy();
extern void uart1_clr_busy();

#endif
