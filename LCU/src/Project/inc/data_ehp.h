#ifndef DATA_EHP_H_
#define DATA_EHP_H_

#include "data_public.h"

#define EHP_MAX_NUM						4    //紧急报警器个数
#define EHP_TOKEN_INTERVAL_TIME			30   //报警器巡检间隔时间  MS
#define EHP_TOKEN_WAIT_OUTTIME			150   // 巡检等待超时时间  MS

#define car5_pecu  5
#define car6_pecu  6


//本地控制器巡检紧急报警器状态
typedef enum {
	EHP_STAT_IDLE = 0,
	EHP_STAT_BUSY,
	EHP_STAT_ANSWER,
	EHP_STAT_HANGUP,
}eEhpBcbStat;

//存放巡检状态值
typedef struct {
	u8 mDeviceId;  //当前正在巡检的设备 下标从1开始
	u8 mWaitFlag;  //等待回复标记， 1:正在等待回复     0: 不在等待
	u32 mTick;  //记录巡检时的时间, 两个作用 1巡检超时计数  2巡检间隔计数
	
}tEhpTokenStat;


extern void EhpDataProc(void);
extern u8 GetEhpComStat();
extern u16 GetEhpAlarmStat();
extern void EhpDataInit();
extern u32 GetAdcData(u8 pecu_id);

#endif


