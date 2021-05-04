/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 设备的相关信息
**  创建日期: 2013.12.20
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
#include "device_info.h"
#include "data_public.h"
#include "include.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"




static u8 gDeviceId = 1;

u8 GetDeviceId( )
{
	return gDeviceId;
}

void GetSoftwareVersion(u8 *vh, u8 *vl)
{
	*vh = (u8)(DEVICE_SOFTWARE_VERSION_HIGHT);
	*vl = (u8)(DEVICE_SOFTWARE_VERSION_LOW);
}


void SetDeviceId()
{
	u8 bit0, bit1, bit2;
	u8 temp;
	u16 cnt = 0;


addr_next:
	bit0 = ADDR0_READ();
	bit1 = ADDR1_READ();
	bit2 = ADDR2_READ();

	if ( (0==bit2) && (1==bit1) && (0==bit0) )
	{
		temp = 6;
	}
	else if ( (0==bit2) && (1==bit1) && (1==bit0) )
	{
		temp = 5;
	}
	else if ( (1==bit2) && (0==bit1) && (0==bit0) )
	{
		temp = 4;
	}
	else if ( (1==bit2) && (0==bit1) && (1==bit0) )
	{
		temp = 3;
	}
	else if ( (1==bit2) && (1==bit1) && (0==bit0) )
	{
		temp = 2;
	}
	else if ( (1==bit2) && (1==bit1) && (1==bit0) )
	{
		temp = 1;
	}

	if (temp == gDeviceId)
	{
		cnt ++;
	}
	else 
	{
		cnt = 0;
	}
	gDeviceId = temp;

	if (cnt < 50)
	{
		goto addr_next;
	}
	
	gDeviceId = temp;
}

void PrintDeviceInfo()
{
	char buf[100];

	print_line("type      :lcu");	
	print_line("scope      :WLMQ 1#");
	sprintf(buf, "version     :%d.%d",DEVICE_SOFTWARE_VERSION_HIGHT, DEVICE_SOFTWARE_VERSION_LOW );
	print_line(buf);
	sprintf(buf, "ID         :%d", gDeviceId );
	print_line(buf);
	
//	usb_write_str(buf);
//	print_line(char * str)
}



