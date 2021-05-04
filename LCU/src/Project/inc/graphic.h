//*****************************************************************
//本头文件定义了用于液晶显示的缓冲区和基本的图形函数
//包括了画点，画线，画圆，画矩形等函数，每种函数都有三种方式画
//即 LCD_MODE_CLEAR， LCD_MODE_SET， LCD_MODE_XOR（清除，置位，异或）
//             ---北京交通大学电气学院 杨罡 2008.8
//*****************************************************************
#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "m32macro.h"
#include "LCD_DRIVER.H"

//****************************************************************
//the pattern used to identify lcd dots
//****************************************************************
#define LCD_MODE_CLEAR     0
#define LCD_MODE_SET       1
#define LCD_MODE_XOR       2
extern unsigned char l_mask_array[8];
 // TODO: progmem
//****************************************************************

//****************************************************************
//the lcd screen buffer 
//****************************************************************
extern unsigned char l_display_array[LCD_Y_BYTES][LCD_X_BYTES];
//****************************************************************

//****************************************************************
//the lcd graphic functions
//****************************************************************
void lcd_fill( unsigned char pattern);
void lcd_erase(void);
void lcd_update( unsigned char top,  unsigned char bottom);
void lcd_update_all(void);

void lcd_dot( unsigned char x,  unsigned char y,  unsigned char mode);
#define lcd_dot_set(x,y)    lcd_dot(x, y, LCD_MODE_SET)
#define lcd_dot_clear(x,y)  lcd_dot(x, y, LCD_MODE_CLEAR)
#define lcd_dot_switch(x,y) lcd_dot(x, y, LCD_MODE_XOR)

void lcd_line( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode );
void lcd_rect(  uint8_t x, uint8_t y, uint8_t width, uint8_t height,  uint8_t mode);
void lcd_box( uint8_t x,  uint8_t y, uint8_t width,  uint8_t height,  uint8_t mode);
void lcd_circle( uint8_t xCenter,  uint8_t yCenter, uint8_t radius,  uint8_t mode);
//*****************************************************************


#endif