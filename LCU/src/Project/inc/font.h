//*****************************************************************
//本头文件定义了用于字符显示的宏以及显示函数和显示的数据
//可以选择字体的大小，这里只选择了EN_SIX_DOT,EN_NINE_DOT两种字体
//其他字体可以自行选择
//             ---北京交通大学电气学院 杨罡 2008.8
//*****************************************************************
#ifndef FONT_H
#define FONT_H

#include  "graphic.h"
#include "m32macro.h"
#include "stdio.h"
//****************************************************************
/* select desired fonts. (Simply comment out those not needed) */
//****************************************************************
//#define EN_FIVE_DOT
  #define EN_SIX_DOT
//#define EN_SEVEN_DOT
  #define EN_NINE_DOT
//#define EN_TEN_DOT
//#define EN_FIFTEEN_DOT
//#define EN_EIGHTEEN_DOT

//definition for the lcd_outtext()-----on the condition of font SXT_DOT
#define page_char_num   112
#define line_per_page    4
#define line_char_num        28
//*********************************************************
void lcd_glyph(uint8_t left, uint8_t top, uint8_t width, uint8_t height,
               uint8_t *glyph_ptr, uint8_t store_width);
void lcd_text(uint8_t left, uint8_t top, uint8_t font,   unsigned char *str);
void lcd_text_p(uint8_t left, uint8_t top, uint8_t font,  unsigned const char *str);			   

//////////////////////////////////////////////
/* define number labels for the font selections */
typedef enum
 {
#ifdef EN_FIVE_DOT
   FONT_FIVE_DOT,
#endif

#ifdef EN_SIX_DOT
   FONT_SIX_DOT,
#endif

#ifdef EN_SEVEN_DOT
   FONT_SEVEN_DOT,
#endif

#ifdef EN_NINE_DOT
   FONT_NINE_DOT,
#endif

#ifdef EN_TEN_DOT
   FONT_TEN_DOT,
#endif

#ifdef EN_FIFTEEN_DOT
   FONT_FIFTEEN_DOT,
#endif

#ifdef EN_EIGHTEEN_DOT
   FONT_EIGHTEEN_DOT,
#endif

   FONT_COUNT
 } FONT_BASE;

struct FONT_DEF 
 { const unsigned char store_width;              /* glyph storage width in bytes */
   const unsigned char glyph_height;  	         /* glyph height for storage */
   const unsigned char *glyph_table;             /* font table start address in memory */
   const unsigned char fixed_width;              /* fixed width of glyphs. If zero */
                                                 /* then use the width table. */
   const unsigned char *width_table; 	         /* variable width table start adress */
   const unsigned char glyph_beg;			 	 /* start ascii offset in table */
   const unsigned char glyph_end;				 /* end ascii offset in table */
   const unsigned char glyph_def;				 /* code for undefined glyph code */
};

/* font definition tables for the three fonts */
 extern const struct FONT_DEF fonts[FONT_COUNT] ;

/* glyph bitmap and width tables for the fonts */ 
#ifdef EN_FIVE_DOT
extern  const unsigned char five_dot_glyph_table[] ;
extern  const unsigned char five_dot_width_table[] ;
#endif

#ifdef EN_SIX_DOT
extern  const unsigned char six_dot_glyph_table[] ;
extern  const unsigned char six_dot_width_table[] ;
#endif

#ifdef EN_SEVEN_DOT
#define DEG_CHAR ('~'+1)
extern  const unsigned char seven_dot_glyph_table[] ;
extern  const unsigned char seven_dot_width_table[] ;
#endif

#ifdef EN_NINE_DOT
extern  const unsigned char nine_dot_glyph_table[] ;
#endif

#ifdef EN_TEN_DOT
extern  const unsigned char ten_dot_glyph_table[] ;
#endif

#ifdef EN_FIFTEEN_DOT
extern  const unsigned char fifteen_dot_glyph_table[] ;
extern  const unsigned char fifteen_dot_width_table[] ;
#endif

#ifdef EN_EIGHTEEN_DOT
extern  const unsigned char eighteen_dot_glyph_table[] ;
extern  const unsigned char eighteen_dot_width_table[] ;
#endif




#endif