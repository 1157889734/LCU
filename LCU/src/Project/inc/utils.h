//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 通用函数库 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:utils.h

//*文件作用:通用函数库

//*文件作者:李智能

//*创建日期:2010年10月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef UTILS_H
#define UTILS_H



extern uint16_t swap_int16(uint16_t n);
extern uint8_t int_to_dec(uint8_t *dec, uint32_t n);
extern void buf_to_hex(uint8_t *hex, uint8_t *buf, uint16_t length);
extern void char_to_hex(uint8_t *hex, uint8_t n);
extern void int_to_hex(uint8_t *hex, uint32_t n);
extern uint8_t hex_to_int8(uint8_t *hex, uint8_t *n);
extern uint8_t hex_to_int16(uint8_t *hex, uint16_t *n);
extern uint8_t hex_to_int32(uint8_t *hex, uint32_t *n);
extern uint8_t dec_to_int8(uint8_t *dec, uint8_t *n);
extern uint8_t dec_to_int16(uint8_t *dec, uint16_t *n);
extern uint8_t dec_to_int32(uint8_t *dec, uint32_t *n);
extern uint16_t hex_to_buf(uint8_t *hex, uint8_t *buf);
extern void print_line(char *str);
extern void print_int(char *args, uint32_t n);
extern void print_buf(uint8_t *buf, uint16_t length);

extern void debug_print_line(char *str);
extern void debug_print_int(char *args, uint32_t n);

#define DEBUG_RECEIVE_CHAR		usb_read_char
#define DEBUG_SEND_CHAR			usb_write_char
#define DEBUG_SEND_STR			usb_write_str
#define DEBUG_SEND_BUF			usb_write_buf
#define PTU_SEND_BUF		        usb_write_buf
#define TBA_PACKET_TAB			0x7E	//数据包分割符
#define TBA_PACKET_DLE			0x7F	//数据包全局转码符
#define TBA_PACKET_DLE_TAB		0x80	//数据包分割符的转码符
#define TBA_PACKET_DLE_DLE		0x81	//数据包转码符的转码符
#define TBA_BUF_SIZE		256
#define PTU_BUF_SIZE		1024

#endif


