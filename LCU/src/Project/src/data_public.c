/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理数据用到的一些公共函数放到这里
**  创建日期: 2013.12.19
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/
#include "usb_lib.h"
#include "uart1.h"
#include "uart2.h"
#include "uart4.h"
#include "utils.h"
#include "include.h"

u8 lcu_st =0;

/*
执行转码并判断校验
*/
u8 VerifyPaket(u8 *paketBuf, u16 length)
{
    u8 recv_flag=0;
    u8 checksum=0;
    u16 src_length=0;//逆转码之前的长度
    u16 dst_length=0;//逆转码以后的长度
//    static u8 length_of_send = 0;
    	    
           //执行逆转码 并计算校验和
	   for(src_length=0; src_length<length; src_length++)
	    {
			//拷贝数据
			paketBuf[dst_length]=paketBuf[src_length];
			//判断刚刚拷贝的是否是转码符
			if(paketBuf[src_length]==TBA_PACKET_DLE && src_length<length-1)
			{
				//判断下一个字符是否是分割符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_TAB 并跳过下一个字符
				if(paketBuf[src_length+1]==TBA_PACKET_DLE_TAB)
				{
					paketBuf[dst_length]=TBA_PACKET_TAB;//把刚刚拷贝的转码符还原为TMS_PACKET_TAB
					src_length++;//并跳过下一个字符
				}
				//判断下一个字符是否是转码符的转码符 把刚刚拷贝的转码符还原为TMS_PACKET_DLE 并跳过下一个字符
				else if(paketBuf[src_length+1]==TBA_PACKET_DLE_DLE)
				{
					paketBuf[dst_length]=TBA_PACKET_DLE;//把刚刚拷贝的转码符还原为TMS_PACKET_DLE
					src_length++;//并跳过下一个字符
				}
				else
				{
					//print_line("tms 7f error");
					//数据错误----7f没有被正确转码
					dst_length=0;//清除转码以后的长度
					break;
				}
			} 
			checksum+=paketBuf[dst_length];
			dst_length++;
	    }
	   
		if(checksum == 0x55)
		{
			recv_flag = dst_length-1;  //减掉校验和的长度
		}
		else
		{
			recv_flag = 0;
		}
		
		return recv_flag;
}



/*
功能:  实现数据打包，
             1、添加包头7E
             2、添加包尾7E
             3、数据转码
             4、计算校验和
*/
u8 PackageData( u8 *destBuf, u8 *srcBuf, u8 length )
{
	u8 send_length = 0;
	u8 checksum=0;
	u8 cnt = 0;
	u8 *ptr = NULL;


	destBuf[send_length++] = 0x7e;

       ptr = (u8 *)srcBuf;
	for(cnt = 0; cnt < length; cnt++)
	{
	    if(*ptr == 0x7e)
	    {
	         destBuf[send_length++] = 0x7f;
		   destBuf[send_length++] = 0x80;
	    }
	    else if(*ptr == 0x7f)
	    {
	        destBuf[send_length++] = 0x7f;
		destBuf[send_length++] = 0x81;
	    }
	     else
	     {
	     	 destBuf[send_length++] = *ptr;
	     }
	     checksum += *ptr;
	     ptr++;
	}

	checksum = 0x55 -checksum;
      //校验和
	if(checksum == 0x7e)
	{
	    destBuf[send_length++] = 0x7f;
          destBuf[send_length++] = 0x80;
	}
	else if(checksum == 0x7f)
	{
	    destBuf[send_length++] = 0x7f;
           destBuf[send_length++] = 0x81;
	}
      else
	{
	    destBuf[send_length++] = checksum;
	}
	//send_buf[send_length++] = checksum;  //校验和
      destBuf[send_length++] = 0x7e;

	return send_length;
}



