//*****************************************************************
//本头文件实现了用于液晶显示的缓冲区和基本的图形函数
//包括了画点，画线，画圆，画矩形等函数，每种函数都有三种方式画
//即 LCD_MODE_CLEAR， LCD_MODE_SET， LCD_MODE_XOR（清除，置位，异或）
//             ---北京交通大学电气学院 杨罡 2008.8
//*****************************************************************
#include  "graphic.h"

 unsigned char l_mask_array[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
 unsigned char l_display_array[LCD_Y_BYTES][LCD_X_BYTES]={0,0,0};
 /* TODO: avoid or PROGMEM */
 /* fill buffer and LCD with pattern */
void lcd_fill( unsigned char pattern)
 { unsigned char page, col;

   for (page=0; page<LCD_Y_BYTES; page++) 
    { for (col=0; col<LCD_X_BYTES; col++) 
         l_display_array[page][col]=pattern;
    }
   lcd_update_all();
 }


void lcd_erase(void)
 { lcd_fill(0x00);
   lcd_update_all();
 }

/*
**
** Updates area of the display. Writes data from "framebuffer" 
** RAM to the lcd display controller RAM.
** 
** Arguments Used:
**    top     top line of area to update.
**    bottom  bottom line of area to update.
**    from MJK-Code
**
*/
void lcd_update( unsigned char top,  unsigned char bottom)
 { unsigned char x;
   unsigned char y;
   unsigned char yt;
   unsigned char yb;
   unsigned char *colptr;
  
   /* setup bytes of range */
   yb = bottom >> 3;
   yt = top >> 3;		

   for(y = yt; y <= yb; y++)
    {lcd_out_ctl(LCD_SET_PAGE+y,3);	    /* set page */
                                       //     lcd_out_ctl(LCD_SET_COL+LCD_STARTCOL_REVERSE,3);
     lcd_out_ctl(LCD_SET_COL+0,3);
	 
     colptr = &l_display_array[y][0];

      for (x=0; x < LCD_X_BYTES; x++)
       { if ( x <LCD_X_BYTES/2 ) 
            lcd_out_dat_l(*colptr++);
         else 
            lcd_out_dat_r(*colptr++);
       }
 
    }
 }



void lcd_update_all(void)
 { lcd_update(SCRN_TOP,SCRN_BOTTOM);
 }



/* sets/clears/switchs(XOR) dot at (x,y) */
void lcd_dot( unsigned char x,  unsigned char y,  unsigned char mode) 
 { unsigned char bitnum, bitmask, yByte;
   unsigned char *pBuffer; /* pointer used for optimisation */

   if ( ( x > SCRN_RIGHT ) || ( y > SCRN_BOTTOM ) ) return;

   yByte   = y >> 3; 
   bitnum  = y & 0x07;
   bitmask = l_mask_array[bitnum]; // bitmask = ( 1 << (y & 0x07) );
   pBuffer = &(l_display_array[yByte][x]);
   switch (mode) 
    { case LCD_MODE_SET:
         *pBuffer |= bitmask;
         break;
      case LCD_MODE_CLEAR:
         *pBuffer &= ~bitmask;
         break;
      case LCD_MODE_XOR:
         *pBuffer ^= bitmask;
         break;
      default: break;
    }         
 }


/* line- and circle-function from a KS0108-library by F. Thiele */

void lcd_line( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,  uint8_t mode ) 
 { uint8_t length, xTmp, yTmp, i, y, yAlt;
   int16_t m;
   
   if(x1 == x2) 
    { // vertical line
      // x1|y1 must be the upper point
      if(y1 > y2) 
       { xTmp = x1;
         yTmp = y1;
         x1 = x2;
         y1 = y2;
         x2 = xTmp;
         y2 = yTmp;
       }
      length = y2-y1;
      for(i=0; i<=length; i++) 
         lcd_dot(x1, y1+i, mode);
    } 
   else if(y1 == y2) 
    { // horizontal line
      // x1|y1 must be the left point
      if(x1 > x2) 
       { xTmp = x1;
         yTmp = y1;
         x1 = x2;
         y1 = y2;
         x2 = xTmp;
         y2 = yTmp;
       }

      length = x2-x1;
      for(i=0; i<=length; i++) 
         lcd_dot(x1+i, y1, mode);
       
    } 
   else 
    { // x1 must be smaller than x2
      if(x1 > x2) 
       { xTmp = x1;
         yTmp = y1;
         x1 = x2;
         y1 = y2;
         x2 = xTmp;
         y2 = yTmp;
       }
		
      if((y2-y1) >= (x2-x1) || (y1-y2) >= (x2-x1)) 
       { // angle larger or equal 45?
         length = x2-x1;								// not really the length :)
         m = ((y2-y1)*200)/length;
         yAlt = y1;
         for(i=0; i<=length; i++) 
          { y = ((m*i)/200)+y1;
            if((m*i)%200 >= 100)
               y++;
            else if((m*i)%200 <= -100)
               y--;
				
            lcd_line(x1+i, yAlt, x1+i, y, mode ); /* wuff wuff recurs. */
            if(length<=(y2-y1) && y1 < y2)
               yAlt = y+1;
            else if(length<=(y1-y2) && y1 > y2)
               yAlt = y-1;
            else
               yAlt = y;
          }
       } 
      else
       { // angle smaller 45?
         // y1 must be smaller than y2
         if(y1 > y2)
          { xTmp = x1;
            yTmp = y1;
            x1 = x2;
            y1 = y2;
            x2 = xTmp;
            y2 = yTmp;
          }
         length = y2-y1;
         m = ((x2-x1)*200)/length;
         yAlt = x1;
         for(i=0; i<=length; i++)
          { y = ((m*i)/200)+x1;

            if((m*i)%200 >= 100)
               y++;
            else if((m*i)%200 <= -100)
               y--;

            lcd_line(yAlt, y1+i, y, y1+i, mode); /* wuff */
            if(length<=(x2-x1) && x1 < x2)
               yAlt = y+1;
            else if(length<=(x1-x2) && x1 > x2)
               yAlt = y-1;
            else
               yAlt = y;
          }
       }
    }
 }


void lcd_circle( uint8_t xCenter, uint8_t yCenter, 
                 uint8_t radius,  uint8_t mode) 
 {  int tSwitch, y, x = 0;
   uint8_t d;
   
   d = yCenter - xCenter;
   y = radius;
   tSwitch = 3 - 2 * radius;

   while (x <= y) 
    { lcd_dot(xCenter + x, yCenter + y, mode);
      lcd_dot(xCenter + x, yCenter - y, mode);
	
      lcd_dot(xCenter - x, yCenter + y, mode);
      lcd_dot(xCenter - x, yCenter - y, mode);
	
      lcd_dot(yCenter + y - d, yCenter + x, mode);
      lcd_dot(yCenter + y - d, yCenter - x, mode);
   
      lcd_dot(yCenter - y - d, yCenter + x, mode);
      lcd_dot(yCenter - y - d, yCenter - x, mode);

      if (tSwitch<0) 
         tSwitch += (4 * x + 6);
      else 
       { tSwitch += (4 * (x - y) + 10);
         y--;
       }
      x++;
    }
 }


void lcd_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t mode) 
 { width--;
   height--;
   lcd_line(x, y, x+width, y, mode);	// top
   lcd_line(x, y, x, y+height, mode);	// left
   lcd_line(x, y+height, x+width, y+height, mode);	// bottom
   lcd_line(x+width, y, x+width, y+height, mode);		// right
 }

void lcd_box( uint8_t x, uint8_t y, uint8_t width,  uint8_t height,  uint8_t mode) 
 { uint8_t i;
   if (!width) return; 

   width--;
	
   for (i=y;i<y+height;i++) 
      lcd_line(x, i, x+width, i, mode);
 }
