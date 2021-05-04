/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理列车总线数据，其他模块获取列车总线数据都从这里获得
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
#include "data_ehp.h"
#include "data_train.h"
#include "data_car.h"
#include "progupdate.h"
#include "led_control.h"

#define CarCheckTime   	100



static u32 gNoDataTick;

static u8 gTrainRecvPacketTempBuf[256];

static u8 gTrainRecvPacketBuf[256];
static u8 gTrainRecvPaketLength = 0;

//static tStationStat gStationStat;
static u8 package_cnt = 0;
static u32 checktime = 0;
//static u32 traintime = 0;
static u8 trainbuf[256];
static u8 trainlen = 0;

static u8 update_flag=0;

tPiscHeader gpiscData;
tLcuHeader glcuData;

//存放将要发送给紧急报警器的数据
static eEhpBcbStat gEhpBcbBuf[EHP_MAX_NUM];


u8 get_update_flag(void)
{
    return update_flag;
}

/*
功能:  报警器
deviceId: 下标从1开始
*/
eEhpBcbStat TrainDataGetEhpStat( u8 deviceId )
{
	if (deviceId > EHP_MAX_NUM) return EHP_STAT_IDLE;
	if (deviceId == 0) return EHP_STAT_IDLE;
	
	return gEhpBcbBuf[deviceId-1];
}

u8 TrainDataSend( u8* buf, u8 length )
{
	u8 sendBuf[255];
	u8 sendLen;
	sendLen = PackageData( sendBuf, buf, length );
	uart1_write_buf( sendBuf, sendLen );
        return sendLen;
}

static void TrainDataTokenReply( void )
{
    //tLcuHeader *lcuHeader;
    u8 tempBuf[50];
    u8 vh, vl;
    u32 tick;

    tick = gSystemTick;
    while(gSystemTick-tick<=6);

    glcuData.dest_netnum = (u8)0x00;
    glcuData.dest_eqnum.eq_type = (u8)0x0f;
    glcuData.dest_eqnum.eq_num = (u8)0x0f;
    glcuData.src_netnum = (u8)0x00;
    glcuData.src_eqnum.eq_type = DEVICE_TYPE_LCU;
    glcuData.src_eqnum.eq_num = GetDeviceId();
    glcuData.cmd = (u8)0x10;
    glcuData.len = (u8)0x22;

    glcuData.data_area.pecust1_8 = GetEhpAlarmStat();
    glcuData.data_area.pecu_err = GetEhpComStat();
    glcuData.data_area.spk_st = 0x00;
    glcuData.data_area.spk_st = gCarCheck.fdu_error << 4;
    glcuData.data_area.led_st = gCarCheck.idu_error;
    glcuData.data_area.led_st |= (gCarCheck.sdu_error<<4);
    
    glcuData.data_area.dmp1_st = gCarCheck.drmd_error[0];
    glcuData.data_area.dmp2_st = gCarCheck.drmd_error[1];
    glcuData.data_area.vau_volume_value = Vau_get_panel_volume();
    glcuData.data_area.vau_volume_mode = Vau_get_panel_mode();

    glcuData.data_area.drmd_version[0] = (gCarCheck.drmd_version_h[0]<<4)|(gCarCheck.drmd_version_l[0]&0x0f);
    glcuData.data_area.drmd_version[1] = (gCarCheck.drmd_version_h[1]<<4)|(gCarCheck.drmd_version_l[1]&0x0f);
    glcuData.data_area.drmd_version[2] = (gCarCheck.drmd_version_h[2]<<4)|(gCarCheck.drmd_version_l[2]&0x0f);
    glcuData.data_area.drmd_version[3] = (gCarCheck.drmd_version_h[3]<<4)|(gCarCheck.drmd_version_l[3]&0x0f);
    glcuData.data_area.drmd_version[4] = (gCarCheck.drmd_version_h[4]<<4)|(gCarCheck.drmd_version_l[4]&0x0f);
    glcuData.data_area.drmd_version[5] = (gCarCheck.drmd_version_h[5]<<4)|(gCarCheck.drmd_version_l[5]&0x0f);
    glcuData.data_area.drmd_version[6] = (gCarCheck.drmd_version_h[6]<<4)|(gCarCheck.drmd_version_l[6]&0x0f);
    glcuData.data_area.drmd_version[7] = (gCarCheck.drmd_version_h[7]<<4)|(gCarCheck.drmd_version_l[7]&0x0f);
    glcuData.data_area.drmd_version[8] = (gCarCheck.drmd_version_h[8]<<4)|(gCarCheck.drmd_version_l[8]&0x0f);
    glcuData.data_area.drmd_version[9] = (gCarCheck.drmd_version_h[9]<<4)|(gCarCheck.drmd_version_l[9]&0x0f);

    glcuData.data_area.idu_version[0] = (gCarCheck.idu_version_h[0]<<4)|(gCarCheck.idu_version_l[0]&0x0f);
    glcuData.data_area.idu_version[1] = (gCarCheck.idu_version_h[1]<<4)|(gCarCheck.idu_version_l[1]&0x0f);

    glcuData.data_area.sdu_version[0] = (gCarCheck.sdu_version_h[0]<<4)|(gCarCheck.sdu_version_l[0]&0x0f);
    glcuData.data_area.sdu_version[1] = (gCarCheck.sdu_version_h[1]<<4)|(gCarCheck.sdu_version_l[1]&0x0f);

    glcuData.data_area.fdu_version[0] = (gCarCheck.fdu_version_h[0]<<4)|(gCarCheck.fdu_version_l[0]&0x0f);
    glcuData.data_area.fdu_version[1] = (gCarCheck.fdu_version_h[1]<<4)|(gCarCheck.fdu_version_l[1]&0x0f);    
    
    GetSoftwareVersion(&vh, &vl);
    glcuData.data_area.lcu_version= (vh<<4)|(vl&0x0f);    

    memcpy(tempBuf,(u8*)&glcuData,40);
    TrainDataSend( tempBuf, 40 );
}


//const u8 shiftNumBuf[EHP_MAX_NUM] = { 0, 2, 4, 6, 8};
const u8 shiftNumBuf[EHP_MAX_NUM] = { 0, 2, 4,6};

/*
功能: 报警器的状态解析
             报警器的状态有广播控制盒发出到列车总线
             本地控制器发出到列车总线
             本地控制器发出到车辆总线
             紧急报警器发出到车辆总线
             报警器的状态分3种
             其中本地控制器发往列车总线和广播控制盒发出一致
             其余两个各有不同 可参考协议文档


             本地控制器巡检紧急报警器
             00：空闲
             01：忙碌
             02：应答

		广播控制盒发出
		00	空闲
		01	呼叫状态
		02	应答状态
		03	乘客紧急报警挂起

		只要一个客室接通了报警，所有的报警器都被发送忙碌

*/
 void TrainDataBcbAnalyse(u8 *buf)
 {
	tBcbHeader *bcbData;
	u8 i, j;
	u8 deviceId;
	u16 temp1, temp2;

	bcbData = (tBcbHeader*)buf;

	deviceId = GetDeviceId();
	for ( i=0; i<LCU_MAX_NUM; i++ )
	{
		temp1 = bcbData->data_area.esu_status[i];
		for ( j=0; j<EHP_MAX_NUM; j++ )
		{
			temp2 = temp1 >> shiftNumBuf[j];
			temp2 &= 0x03;
			if ( i== (deviceId-1) )
			{
				if ( temp2 ==  0x00 )  //空闲
				{
					//if ( gEhpBcbBuf[j] != EHP_STAT_ANSWER )
					{
						gEhpBcbBuf[j] = EHP_STAT_IDLE;
					}
				}
				else if ( temp2 ==  0x01 ) // 呼叫
				{
					gEhpBcbBuf[j] = EHP_STAT_IDLE;
				}
				else if ( temp2 ==  0x02 )  // 接通
				{
					gEhpBcbBuf[j] = EHP_STAT_ANSWER;
					//其他报警器要发送忙碌
					for ( i=0; i<EHP_MAX_NUM; i++ )
					{
						if (i != j)
						{
							gEhpBcbBuf[i] = EHP_STAT_BUSY;
						}
					}
					return;
				}
				else if ( temp2 ==  0x03 )  // 挂断
				{
					gEhpBcbBuf[j] = EHP_STAT_HANGUP;
				}
			}
			else 
			{
				if (temp2 ==  EHP_STAT_ANSWER ) 
				{	
					// 只要其他客室有一个为应答状态，那么本客室全部为忙碌
					for ( i=0; i<EHP_MAX_NUM; i++ )
					{
						gEhpBcbBuf[i] = EHP_STAT_BUSY;
					}
					return;
				}	
			}
		}
		
	}
 }

 void TrainDataPiscAnalyse(u8 *buf , u8 len)
{
    memcpy(trainbuf , buf , len);
    memcpy((u8*)&gpiscData , buf , sizeof(tPiscHeader));
    trainlen = len; 
}


static u8 TrainDataGetPacket(void)
{
	u8 temp;
	while(uart1_read_char(&temp))
	{			
		if(temp == TBA_PACKET_TAB)
		{
			//判断长度 判断是否有数据
			if(gTrainRecvPaketLength >= 3)
			{
				return 1;
			}
			gTrainRecvPaketLength=0;
		}
		//其他字符串直接送缓冲区
		else
		{
			gTrainRecvPacketBuf[gTrainRecvPaketLength] = temp;
			if( ++gTrainRecvPaketLength >= 255 )
				gTrainRecvPaketLength = 0;
		}
	}
	return 0;
}




void TrainDataProc(void)
{
	tFrameHeader *frameHeader;
	tPiscHeader *piscData;
	tBcbHeader *bcbData;
	u8 deviceId;
	u8 len=0;

	if (TrainDataGetPacket()) 
	{
		len = VerifyPaket( gTrainRecvPacketBuf, gTrainRecvPaketLength );
              
		if ( len > 0 ) 
		{
			frameHeader = (tFrameHeader *)gTrainRecvPacketBuf;
			switch (frameHeader->cmd.packet_type) 
			{
				case PROCESS_PACKET:
					//if ((frameHeader->dest_eqnum.eq_num != 0xF) || (frameHeader->dest_eqnum.eq_type != 0xF)) break;
					update_flag = 0;
					//是否为过程数据
					if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_PISC ) 
					{
						piscData = (tPiscHeader *)gTrainRecvPacketBuf;
						//是否为主
						if ( piscData->data_area.status_hbit.master ) 
						{
							//解析数据并转发给列车总线
							TrainDataPiscAnalyse( gTrainRecvPacketBuf,len );
						}
					}
					else if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_BCB ) 
					{
						bcbData = (tBcbHeader*)gTrainRecvPacketBuf;
						if (bcbData->data_area.status_hbit.active)
						{
							//解析数据 保存对讲状态
							TrainDataBcbAnalyse( gTrainRecvPacketBuf );
						}
					}
					break;
				case TOKEN_PACKET:
					if ( frameHeader->dest_eqnum.eq_type == DEVICE_TYPE_LCU )
					{
						deviceId = GetDeviceId();
						if ( frameHeader->dest_eqnum.eq_num == deviceId )
						{
							TrainDataTokenReply();
						}
					}
					
					break;
				case DOWNLOAD_PACKET:
					if ( frameHeader->dest_eqnum.eq_type == DEVICE_TYPE_LCU )
					{
						ProgramUpdate( gTrainRecvPacketBuf );
					}
                                  else if(( frameHeader->dest_eqnum.eq_type == DEVICE_TYPE_EHP )
                                    ||( frameHeader->dest_eqnum.eq_type == DEVICE_TYPE_VAU ))
                                  {
					       update_flag = I485_UPDATE_FLAG;
						memcpy(gTrainRecvPacketTempBuf, gTrainRecvPacketBuf, len);
						EhpDataSend( gTrainRecvPacketTempBuf, len );
					}
					else 
					{
					       update_flag = CAR485_UPDATE_FLAG;
						memcpy(gTrainRecvPacketTempBuf, gTrainRecvPacketBuf, len);
						CarDataSend( gTrainRecvPacketTempBuf, len );
					}
					break;
				default :
					break;
			}
			gNoDataTick = gSystemTick;
		}
		gTrainRecvPaketLength = 0;
	}

       //巡检 二级总线
	if((gSystemTick - checktime > CarCheckTime) &&(gCarCheck.token)&&(!update_flag))
	{
		checktime = gSystemTick ;
		package_cnt ++;
              #if 1
		if(package_cnt %2 ==0)
			CarTokenSend(); //巡检动态地图和idu
		else
		{
	           if(trainlen>0)	
		        CarDataSend( trainbuf, trainlen ); //发到car 总线	
              }	
              #endif
	}		

	if ( gSystemTick-gNoDataTick > 3000 )
	{
		LED_TRAIN_COM_OPEN();
	}

}


void TrainDataInit(void)
{
	u8 i;

	for (i=0; i<EHP_MAX_NUM; i++)
	{
		gEhpBcbBuf[i] = EHP_STAT_IDLE;
	}

       memset((u8*)&gpiscData,0,sizeof(tPiscHeader));
       memset((u8*)&glcuData,0,sizeof(tLcuHeader));
 	
        /*
	gStationStat.mStartStation = 0;
	gStationStat.mEndStation = 0;
	gStationStat.mDoorSide = DOOR_SIDE_NONE;
	gStationStat.mNextStation = 0;
	gStationStat.mRunStat = RUN_STAT_CLOSE_DOOR;
        */

}



