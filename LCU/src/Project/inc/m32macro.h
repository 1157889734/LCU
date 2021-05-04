//******************************************************
//本程序用于定义一些通用函数和宏
//                  ---北京交通大学电气学院 杨罡 2008.8
//******************************************************

#ifndef MACRO_H
#define MACRO_H

//#define uint8_t    unsigned char
//#define uint16_t   unsigned int 
//#define int16_t    int
//#define uint32_t   unsigned long
//#define int32_t    long
unsigned char pgm_read_byte(const unsigned char *p);
unsigned int pgm_read_word( const unsigned int *p);
void _delay_ms(unsigned int i);
void _delay_us(unsigned char i);
#endif