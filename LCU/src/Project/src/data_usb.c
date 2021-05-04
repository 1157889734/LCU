/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理USB数据
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
#include "data_public.h"
#include "device_info.h"
#include "progupdate.h"
#include "led_control.h"


static u8 gUsbRecvPacketBuf[256];
static u8 gUsbRecvPaketLength = 0;

static u8 UsbDataGetPacket(void)
{
	u8 temp;
	while(usb_read_char(&temp))
	{
		LedUsbOpen();
		if(temp == TBA_PACKET_TAB)
		{
			//判断长度 判断是否有数据
			if(gUsbRecvPaketLength >= 3)
			{
				return 1;
			}
			gUsbRecvPaketLength=0;
		}
		//其他字符串直接送缓冲区
		else
		{
			gUsbRecvPacketBuf[gUsbRecvPaketLength] = temp;
			if( ++gUsbRecvPaketLength >= 255 )
				gUsbRecvPaketLength = 0;
		}
	}
	return 0;
}



void UsbDataProc()
{
	u8 len;

	if (UsbDataGetPacket()) 
	{
		len = VerifyPaket( gUsbRecvPacketBuf, gUsbRecvPaketLength );
		if ( len > 0 ) 
		{
			if ( gUsbRecvPacketBuf[1] == 2 ) //版本查询
			{
				PrintDeviceInfo();
			}
			else if ( gUsbRecvPacketBuf[1] == 1 ) //软件升级
			{
				UsbProgramUpdate( gUsbRecvPacketBuf );
			}
		}
		gUsbRecvPaketLength = 0;
	}

}


