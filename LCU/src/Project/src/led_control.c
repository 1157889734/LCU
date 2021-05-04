/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 状态指示灯的控制
**  创建日期: 2013.12.26
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/

#include "led_control.h"
#include "include.h"
#include "device_info.h"

static u32 gUsbLedTick;


static tLedAlarmParam gLedAlarmParam;
static tLedPcParam gLedPcParam;

static void LedRunCtrl(void)
{
	static u32 tick;
	static u8 flag;

	if (gSystemTick - tick >= 500) {
		tick = gSystemTick;
		if (flag == 0) {
			flag = 1;
			LED_RUN_OPEN();
		}
		else {
			flag = 0;
			LED_RUN_CLOSE();
		}
	}
}




//pc led ==================================================================================
/*
功能:  报警灯亮灭控制
          1 : 点亮    0:熄灭
*/
void LedPcSwitch(eLedStat stat)
{
	if ( LED_STAT_CLOSE == stat )
	{
		LED_LISTEN_TALK_CLOSE();
		gLedPcParam.mLedStat = LED_STAT_CLOSE;
		gLedPcParam.mTick = gSystemTick;
	}
	else 
	{
		LED_LISTEN_TALK_OPEN();
		gLedPcParam.mLedStat = LED_STAT_OPEN;
		gLedPcParam.mTick = gSystemTick;
	}
}



/*
功能:  设置报警灯的报警状态
返回 1:成功      0:失败
*/
u8 LedPcStatSet( eLedPcStat stat )
{

	gLedPcParam.mPcStat = stat;

	return 1;
}


static void LedPcRun(u32 openMs, u32 closeMs)
{
	if ( gLedPcParam.mLedStat == LED_STAT_OPEN )
	{
		if ( gSystemTick - gLedPcParam.mTick >= openMs )
		{
			LedPcSwitch( LED_STAT_CLOSE );
		}
	}
	else {
		if ( gSystemTick - gLedPcParam.mTick >= closeMs )
		{
			LedPcSwitch( LED_STAT_OPEN );			
		}
	}
}

static void LedPcCtrl(void)
{
	switch( gLedPcParam.mPcStat )
	{
		case LED_PC_IDLE:
			LedPcSwitch( LED_STAT_CLOSE );
			break;
		case LED_PC_LISTEN:
			LedPcRun( 1000, 0 );
			break;
		case LED_PC_TALK:
			LedPcRun( 200, 400 );			
			break;
	}
}
//pc led end==================================================================================




//alarm led ==================================================================================
/*
功能:  报警灯亮灭控制
          1 : 点亮    0:熄灭
*/
void LedAlarmSwitch(eLedStat stat)
{
	if ( LED_STAT_CLOSE == stat )
	{
		LED_ALARM_CLOSE();
		gLedAlarmParam.mLedStat = LED_STAT_CLOSE;
		gLedAlarmParam.mTick = gSystemTick;
	}
	else 
	{
		LED_ALARM_OPEN();
		gLedAlarmParam.mLedStat = LED_STAT_OPEN;
		gLedAlarmParam.mTick = gSystemTick;
	}
}



/*
功能:  设置报警灯的报警状态
返回 1:成功      0:失败
*/
u8 LedAlarmStatSet( eLedAlarmType type, eLedAlarmStat stat )
{

	static eLedAlarmStat statLose = LED_ALARM_IDLE;
	static eLedAlarmStat statAlarm = LED_ALARM_IDLE;
	static eLedAlarmStat statConn = LED_ALARM_IDLE;

	switch( type )
	{
		case LED_ALARM_TYPE_ALARM:
			statAlarm = stat;
			break;
		case LED_ALARM_TYPE_CONN:
			statConn = stat;
			break;
		case LED_ALARM_TYPE_LOSE:
			statLose = stat;
			break;
	}

	if (statConn != LED_ALARM_IDLE)
	{
		gLedAlarmParam.mAlarmStat = LED_ALARM_CONN;
		return 1;
	}
	if (statAlarm != LED_ALARM_IDLE)
	{
		gLedAlarmParam.mAlarmStat = LED_ALARM_ALARM;
		return 1;
	}
	if (statLose != LED_ALARM_IDLE)
	{
		gLedAlarmParam.mAlarmStat = statLose;
		return 1;
	}
	LedAlarmSwitch( LED_STAT_CLOSE );
	gLedAlarmParam.mAlarmStat = LED_ALARM_IDLE;
	

	return 1;
}


static void LedAlarmRun(u32 openMs, u32 closeMs)
{
	if ( gLedAlarmParam.mLedStat == LED_STAT_OPEN )
	{
		if ( gSystemTick - gLedAlarmParam.mTick >= openMs )
		{
			LedAlarmSwitch( LED_STAT_CLOSE );
		}
	}
	else {
		if ( gSystemTick - gLedAlarmParam.mTick >= closeMs )
		{
			LedAlarmSwitch( LED_STAT_OPEN );			
		}
	}
}

static void LedEhpLose(u8 loseCnt)
{
	static u8 step = 0;  //0:亮  1:灭    2:delay
	static u32 tick;
	static u8 loop = 0;

	if (step == 0)  // init  open
	{
		tick = gSystemTick;
		LedAlarmSwitch( LED_STAT_OPEN );	
		step = 1;
	}
	else if (step == 1) //open
	{
		if (gSystemTick - tick > 10)
		{
			tick = gSystemTick;
			step = 2;
			LedAlarmSwitch( LED_STAT_CLOSE);		
		}
	}
	else if (step == 2) // close
	{
		if (gSystemTick - tick > 350)
		{
			tick = gSystemTick;
			loop ++;
			if (loop >= loseCnt)
			{
				step = 3;
			}
			else 
			{
				step = 0;
			}
		}
	}
	else if (step == 3) // wait 
	{
		if (gSystemTick - tick > 1500)
		{
			loop = 0;
			step = 0;
		}
	}

	
}


static void LedAlarmCtrl(void)
{
	switch( gLedAlarmParam.mAlarmStat )
	{
		case LED_ALARM_IDLE:
			LedAlarmSwitch( LED_STAT_CLOSE );
			break;
		case LED_ALARM_ALARM:
			LedAlarmRun( 200, 400 );
			break;
		case LED_ALARM_CONN:
			LedAlarmRun( 1000, 0 );			
			break;
		case LED_ALARM_EHP_LOSE1:
			LedEhpLose( 1 );
			break;
		case LED_ALARM_EHP_LOSE2:
			LedEhpLose( 2 );
			break;
		case LED_ALARM_EHP_LOSE3:
			LedEhpLose( 3 );
			break;
		case LED_ALARM_EHP_LOSE4:
			LedEhpLose(4);
			break;
		case LED_ALARM_EHP_LOSE5:
			LedEhpLose(5);
			break;
		
	}
}
//alarm led end==================================================================================


void LedUsbOpen(void)
{
	gUsbLedTick = gSystemTick;
	//LED_RS232_COM_OPEN();
}

static void LedUsbCtrl(void)
{
	// 1秒后熄灭
	if (gSystemTick - gUsbLedTick > 100) 
	{
		//LED_RS232_COM_CLOSE();
	}
}



/*
功能:  通过上电时闪烁所有LED 亮灭次数表明设备ID号
*/
void LedDispDeviceId(void)
{
	u8 deviceId;
	u8 i;
	u32 tick;
	
	deviceId = GetDeviceId();

	for (i=0; i<deviceId; i++)
	{
		LED_RUN_OPEN();
		LED_ALARM_OPEN();
		LED_TRAIN_COM_OPEN();
		LED_EHP_COM_OPEN();
		LED_LISTEN_TALK_OPEN();
		LED_VCOM_COM_OPEN();
		tick = gSystemTick;
		while (gSystemTick-tick<200);
		
		LED_RUN_CLOSE();
		LED_ALARM_CLOSE();
		LED_TRAIN_COM_CLOSE();
		LED_EHP_COM_CLOSE();
		LED_LISTEN_TALK_CLOSE();
		LED_VCOM_COM_CLOSE();
		tick = gSystemTick;
		while (gSystemTick-tick<300);		
	}
	
	tick = gSystemTick;
	while(gSystemTick-tick<10000);
	
}


void LedProc(void)
{
	LedRunCtrl();
	LedUsbCtrl();
	LedAlarmCtrl();
	LedPcCtrl();

}


