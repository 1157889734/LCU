#ifndef UART3_H
#define UART3_H

#define UART3_BUF_LEN 256

extern u8 uart3_recv_buf[UART3_BUF_LEN];
extern u8 uart3_send_buf[UART3_BUF_LEN];
extern u16 uart3_recv_fifo_in;
extern u16 uart3_recv_fifo_out;

extern void uart3_config();
extern void uart3_write_char(u8 ch);
extern void uart3_write_buf(u8 *buf, u16 length);
extern u8 uart3_read_char(u8 *ch);
#endif