//******************************************************
//本程序实现了m32macro.h头文件中函数
//                          ---北京交通大学电气学院 杨罡 
//******************************************************
#include "m32macro.h"

void _delay_us(unsigned char i)
{
  while(--i);
}
void _delay_ms(unsigned int i)
{
  unsigned int ii;
  for(;i>0;--i)
    for(ii=500;ii>0;--ii);  
}

unsigned char pgm_read_byte(const unsigned char *p)
{
 return *p;
}
unsigned int pgm_read_word( const unsigned int *p)
{
 return *p;
}