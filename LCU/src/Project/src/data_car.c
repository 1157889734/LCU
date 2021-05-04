/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理车辆总线数据，其他模块获取车辆总线数据都从这里获得
**  创建日期: 2013.12.19
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/
#include "data_public.h"
#include "data_car.h"
#include "usb_lib.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "utils.h"
#include "device_info.h"
//#include "data_public.h"
#include "include.h"

tCarCheck gCarCheck;
tDRMDHeader gDrmdData={0};
extern tPiscHeader gpiscData;

static u8 gCarRecvPaketBuf[256];
static u8 gCarRecvPaketLength = 0;

static u8 gVauRecvPaketBuf[256];
static u8 gVauRecvPaketLength = 0;

#define CAR_DATA_SEND_CNT					2
#define CAR_POLL_ERROR_NUM				3

//static u8 gCarDataSendCnt = 0;
//static tStationStat gStationStatOld;

static u8 panel_mode= 0;
static u8 panel_volume= 0;

static u8 recv_flag = 0 ;

/*
static u8 CalcuSum(u8 *buf, u8 len)
{
	u8 i;
	u8 sum;

	sum = 0;
	for (i=0; i<len; i++)
	{
		sum += buf[i];
	}
	return sum;
}
*/
void CarDataClearDmp()
{
	u8 clearBuf[18] = {0xFF, 0xC0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFE, 0x00, 0x00, 0x00};

	uart4_write_buf( clearBuf, 18 );
	print_line("clear dmp");

}

/*
static void CarDataSendToDmp( tStationStat stationStat )
{
	u8 sendBuf[15] = {0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xF0, 0xFE};

	//等待串口空闲
	while(uart2_busy());

	if ( (gStationStatOld.mStartStation!=stationStat.mStartStation) || (gStationStatOld.mEndStation!=stationStat.mEndStation) )
	{
		gCarDataSendCnt = CAR_DATA_SEND_CNT*2 - 1;
		CarDataClearDmp();
	}
	else if (gCarDataSendCnt > CAR_DATA_SEND_CNT) 
	{
		CarDataClearDmp();
	}
	else 
	{
		switch (stationStat.mRunStat)
		{
			case RUN_STAT_OPEN_DOOR:
				sendBuf[6] = 0x00;
				sendBuf[7] = 0x80 + stationStat.mDoorSide;
				break;
			case RUN_STAT_CLOSE_DOOR:
				sendBuf[6] = 0x00;
				sendBuf[7] = 0x03;
				break;
			case RUN_STAT_LEAVE_BROADCAST:
				sendBuf[6] = 0x00;
				sendBuf[7] = 0x00;
				break;
			case RUN_STAT_PRE_DOOR_SIDE:
				sendBuf[6] = stationStat.mDoorSide;
				sendBuf[7] = 0x00;				
				break;
				break;
		}
		sendBuf[3] = stationStat.mStartStation;
		sendBuf[4] = stationStat.mEndStation;
		sendBuf[5] = stationStat.mNextStation;
		sendBuf[13] = CalcuSum(&sendBuf[1], 12);
		uart2_write_buf( sendBuf, 15 );
	}
	memcpy( &gStationStatOld, &stationStat, sizeof(tStationStat) );

}
*/

/*
void CarDataStartToSend( tStationStat stationStat )
{
	gCarDataSendCnt = 1;  // 这里只能放到首行，因为下面的函数进去可能会修改该值
	CarDataSendToDmp( stationStat );
	print_line("start station:");
	print_int("%d", stationStat.mStartStation);
	print_line("end station:");
	print_int("%d", stationStat.mEndStation);
	print_line("next station:");
	print_int("%d", stationStat.mNextStation);
}
*/

/*
static void CarDataSendRepeat()
{
	static u32 tick;
	
	if (uart2_busy())  // car 485
	{
		tick = gSystemTick;
	}
	else 
	{
		if (gSystemTick - tick > 20)
		{
			if ( gCarDataSendCnt > 0 )
			{
				CarDataSendToDmp( gStationStatOld );
				gCarDataSendCnt --;
			}
		}
	}

}
*/

u8 CarDataSend( u8* buf, u8 length )
{
	u8 sendBuf[255];
	u8 sendLen;

	//等待串口空闲
	while(uart4_busy());
	
	sendLen = PackageData( sendBuf, buf, length );
	uart4_write_buf( sendBuf, sendLen );
        return 0;
}

static u8 CarDataGetPacket(void)
{
	u8 temp;
	while(uart4_read_char(&temp))
	{			
		if(temp == TBA_PACKET_TAB)
		{
			//判断长度 判断是否有数据
			if(gCarRecvPaketLength >= 3)
			{
				return 1;
			}
			gCarRecvPaketLength=0;
		}
		//其他字符串直接送缓冲区
		else
		{
			gCarRecvPaketBuf[gCarRecvPaketLength] = temp;
			if( ++gCarRecvPaketLength >= 255 )
			{
				gCarRecvPaketLength = 0;
			}
		}
	}
	return 0;
}

void FduTokenSend(void)
{
	//static u8 index=0;
	tLcuHeader *lcuHeader;
	u8 tempBuf[100];
	//u8 vh, vl;

	//u32 tick;

	gCarCheck.fdu_index++;
	if(gCarCheck.fdu_index>FduNumMax)
		gCarCheck.fdu_index =1;

	//tick = gSystemTick;
	//while(gSystemTick-tick<=3);

	lcuHeader = (tLcuHeader*)tempBuf;
	
	lcuHeader->dest_netnum = (u8)0x00;
	lcuHeader->dest_eqnum.eq_type = (u8)DEVICE_TYPE_FDU;
	lcuHeader->dest_eqnum.eq_num = gCarCheck.fdu_index;
	lcuHeader->src_netnum = (u8)0x00;
	lcuHeader->src_eqnum.eq_type = DEVICE_TYPE_LCU;
	lcuHeader->src_eqnum.eq_num = GetDeviceId();
	lcuHeader->cmd = (u8)0x12;   //巡检 命令 02
	lcuHeader->len = (u8)0x00;
    
	CarDataSend( tempBuf, 6 );
}

void SduTokenSend(void)
{
	//static u8 index=0;
	tLcuHeader *lcuHeader;
	u8 tempBuf[100];
	//u8 vh, vl;

	//u32 tick;

	gCarCheck.sdu_index++;
	if(gCarCheck.sdu_index>SduNumMax)
		gCarCheck.sdu_index =1;

	//tick = gSystemTick;
	//while(gSystemTick-tick<=3);

	lcuHeader = (tLcuHeader*)tempBuf;
	
	lcuHeader->dest_netnum = (u8)0x00;
	lcuHeader->dest_eqnum.eq_type = (u8)DEVICE_TYPE_SDU;
	lcuHeader->dest_eqnum.eq_num = gCarCheck.sdu_index;
	lcuHeader->src_netnum = (u8)0x00;
	lcuHeader->src_eqnum.eq_type = DEVICE_TYPE_LCU;
	lcuHeader->src_eqnum.eq_num = GetDeviceId();
	lcuHeader->cmd = (u8)0x12;   //巡检 命令 02
	lcuHeader->len = (u8)0x00;
    
	CarDataSend( tempBuf, 6 );
}

void IduTokenSend(void)
{
	//static u8 index=0;
	tLcuHeader *lcuHeader;
	u8 tempBuf[100];
	//u8 vh, vl;

	//u32 tick;

	gCarCheck.idu_index++;
	if(gCarCheck.idu_index>IduNumMax)
		gCarCheck.idu_index =1;

	//tick = gSystemTick;
	//while(gSystemTick-tick<=3);

	lcuHeader = (tLcuHeader*)tempBuf;
	
	lcuHeader->dest_netnum = (u8)0x00;
	lcuHeader->dest_eqnum.eq_type = (u8)DEVICE_TYPE_IDU;
	lcuHeader->dest_eqnum.eq_num = gCarCheck.idu_index;
	lcuHeader->src_netnum = (u8)0x00;
	lcuHeader->src_eqnum.eq_type = DEVICE_TYPE_LCU;
	lcuHeader->src_eqnum.eq_num = GetDeviceId();
	lcuHeader->cmd = (u8)0x12;   //巡检 命令 02
	lcuHeader->len = (u8)0x00;

	
	CarDataSend( tempBuf, 6 );
}

void DrmdTokenSend(void)
{
	//static u8 index=0;
	tLcuHeader *lcuHeader;
	u8 tempBuf[20];
	//u8 vh, vl;

	//u32 tick;

	gCarCheck.drmd_index++;
	if(gCarCheck.drmd_index>DrmdNumMax)
		gCarCheck.drmd_index =1;

	//tick = gSystemTick;
	//while(gSystemTick-tick<=3);

	lcuHeader = (tLcuHeader*)tempBuf;
	
	lcuHeader->dest_netnum = (u8)0x00;
	lcuHeader->dest_eqnum.eq_type = (u8)DEVICE_TYPE_DMP;
	lcuHeader->dest_eqnum.eq_num = gCarCheck.drmd_index;
	lcuHeader->src_netnum = (u8)0x00;
	lcuHeader->src_eqnum.eq_type = DEVICE_TYPE_LCU;
	lcuHeader->src_eqnum.eq_num = GetDeviceId();
	lcuHeader->cmd = (u8)0x12;   //巡检 命令 02
	lcuHeader->len = (u8)0x00;
	
	CarDataSend( tempBuf, 6 );
}

/*************************************************
功能: 巡检Car 总线设备
*************************************************/
void CarTokenSend()
{
	static u8 cnt=0;
	
	if(gCarCheck.token)   //获得令牌权
	{
		if(cnt%2==1)      //巡检drmd
		{
			gCarCheck.poll_type = 0 ;
			DrmdTokenSend();
		}
		else //if(cnt ==2)           //巡检Idu
		{
			gCarCheck.poll_type = 1;
			IduTokenSend();
		}
              #if 0
              else if(cnt ==4)           //巡检Sdu
		{
			gCarCheck.poll_type = 2;
			SduTokenSend();
		}

              else if(cnt ==6)           //巡检Ddu
		{
		       if((GetDeviceId()==1) ||(GetDeviceId()==6))
			{
			    gCarCheck.poll_type = 3;
			    FduTokenSend();
                     }
		}
              #endif

    		cnt++;
              if(cnt>6)
                cnt = 0;
		gCarCheck.token = 0 ;  //释放令牌
		gCarCheck.poll_time_value= gSystemTick ;  
	}
}


void CarDataProc(void)
{
	tFrameHeader *frameHeader;
	//tPiscHeader *piscData;
	u8 len;
	//static u8 index=0;
	static u32 car_scan_time = 0;
	//static u8 pre_arrive = 0;
	//static u8 reach_stop = 0;
	//static u8 closedoor = 0;
	//static u8 dis_drmd = 0;
	//static u8 pre_cmd_status = 0;

	if (CarDataGetPacket()) 
	{
		len = VerifyPaket( gCarRecvPaketBuf, gCarRecvPaketLength );
              LED_VCOM_COM_OPEN();
		if ( len > 0 ) 
		{
			frameHeader = (tFrameHeader *)gCarRecvPaketBuf;
			switch (frameHeader->cmd.packet_type) 
			{
				case PROCESS_PACKET:
					
					break;
				case TOKEN_PACKET:
					car_scan_time = gSystemTick;
					while(gSystemTick - car_scan_time <5);
					gCarCheck.token = 1; //回收令牌权限
					//if(gSystemTick - gCarCheck.check_time_value <200)
					{
						//收到令牌回复清故障包
						if(frameHeader->src_eqnum.eq_type == DEVICE_TYPE_DMP)
						{
						       #if SUVEN_DEBUG
                                                if((frameHeader->src_eqnum.eq_num>DrmdNumMax)
                                                    ||(frameHeader->src_eqnum.eq_num== 0)){
                                                    lcu_st = 0x04;
                                                    break;
                                                }
                                                #endif
                                                gCarCheck.drmd_version_h[frameHeader->src_eqnum.eq_num-1] = gCarRecvPaketBuf[6];
                                                gCarCheck.drmd_version_l [frameHeader->src_eqnum.eq_num-1] = gCarRecvPaketBuf[7];
							gCarCheck.drmdpoll_error_cnt[frameHeader->src_eqnum.eq_num-1] =0;
							gCarCheck.drmd_error[(frameHeader->src_eqnum.eq_num-1)/8] &= ~(1<< (frameHeader->src_eqnum.eq_num-1)%8);
						}
						else if(frameHeader->src_eqnum.eq_type == DEVICE_TYPE_IDU)
						{
						       #if SUVEN_DEBUG
                                                if((frameHeader->src_eqnum.eq_num>IduNumMax)
                                                    ||(frameHeader->src_eqnum.eq_num== 0)){
                                                    lcu_st = 0x06;
                                                    break;
                                                }
                                                #endif
                                                gCarCheck.idu_version_h[frameHeader->src_eqnum.eq_num-1] = gCarRecvPaketBuf[6];
                                                gCarCheck.idu_version_l [frameHeader->src_eqnum.eq_num-1] = gCarRecvPaketBuf[7];						
							gCarCheck.idupoll_error_cnt[gCarCheck.idu_index-1] = 0;
							gCarCheck.idu_error &= ~(1<< (frameHeader->src_eqnum.eq_num-1));
						}
                                         else if(frameHeader->src_eqnum.eq_type == DEVICE_TYPE_SDU)
						{
						       #if SUVEN_DEBUG
                                                if((frameHeader->src_eqnum.eq_num>SduNumMax)
                                                    ||(frameHeader->src_eqnum.eq_num== 0)){
                                                    lcu_st = 0x08;
                                                    break;
                                                }
                                                #endif
                                                gCarCheck.sdu_version_h[frameHeader->src_eqnum.eq_num-1] = gCarRecvPaketBuf[6];
                                                gCarCheck.sdu_version_l [frameHeader->src_eqnum.eq_num-1] = gCarRecvPaketBuf[7];					
							gCarCheck.sdupoll_error_cnt[gCarCheck.sdu_index-1] = 0;
							gCarCheck.sdu_error &= ~(1<< (frameHeader->src_eqnum.eq_num-1));
						}
                                         else if(frameHeader->src_eqnum.eq_type == DEVICE_TYPE_FDU)
						{
						       #if SUVEN_DEBUG
                                                if((frameHeader->src_eqnum.eq_num>FduNumMax)
                                                    ||(frameHeader->src_eqnum.eq_num== 0)){
                                                    lcu_st = 0x07;
                                                    break;
                                                }
                                                #endif
                                                gCarCheck.fdu_version_h[frameHeader->src_eqnum.eq_num-1] = gCarRecvPaketBuf[6];
                                                gCarCheck.fdu_version_l [frameHeader->src_eqnum.eq_num-1] = gCarRecvPaketBuf[7];						
							gCarCheck.fdupoll_error_cnt[gCarCheck.fdu_index-1] = 0;
							gCarCheck.fdu_error &= ~(1<< (frameHeader->src_eqnum.eq_num-1));
						}                                         
					}
					break;
				case DOWNLOAD_PACKET:

					break;
				default :
					break;
			}
		}
		gCarRecvPaketLength = 0;
	}

	if(gSystemTick - gCarCheck.poll_time_value>200 && gCarCheck.token==0)
	{
		gCarCheck.token = 1;
		if(gCarCheck.poll_type == 0){
			gCarCheck.drmdpoll_error_cnt[gCarCheck.drmd_index-1] ++;
			if(gCarCheck.drmdpoll_error_cnt[gCarCheck.drmd_index-1] >CAR_POLL_ERROR_NUM){
				gCarCheck.drmd_error[(gCarCheck.drmd_index-1)/8] |= (1<< (gCarCheck.drmd_index-1)%8 );
                           gCarCheck.drmd_version_h[gCarCheck.drmd_index-1] = 0;
                           gCarCheck.drmd_version_l [gCarCheck.drmd_index-1] = 0;	
                    }
		}
		else if(gCarCheck.poll_type == 1){
			gCarCheck.idupoll_error_cnt[gCarCheck.idu_index-1] ++;
			if(gCarCheck.idupoll_error_cnt[gCarCheck.idu_index-1] >CAR_POLL_ERROR_NUM){
				gCarCheck.idu_error |= (1<< (gCarCheck.idu_index-1));
                           gCarCheck.idu_version_h[gCarCheck.idu_index-1] = 0;
                           gCarCheck.idu_version_l [gCarCheck.idu_index-1] = 0;
                    }
		}
		else if(gCarCheck.poll_type == 2){
			gCarCheck.sdupoll_error_cnt[gCarCheck.sdu_index-1] ++;
			if(gCarCheck.sdupoll_error_cnt[gCarCheck.sdu_index-1] >CAR_POLL_ERROR_NUM){
				gCarCheck.sdu_error |= (1<< (gCarCheck.sdu_index-1));
                           gCarCheck.sdu_version_h[gCarCheck.sdu_index-1] = 0;
                           gCarCheck.sdu_version_l [gCarCheck.sdu_index-1] = 0;
                    }
		}    
		else if(gCarCheck.poll_type == 3){
			gCarCheck.fdupoll_error_cnt[gCarCheck.fdu_index-1] ++;
			if(gCarCheck.fdupoll_error_cnt[gCarCheck.fdu_index-1] >CAR_POLL_ERROR_NUM){
				gCarCheck.fdu_error |= (1<< (gCarCheck.fdu_index-1));
                           gCarCheck.fdu_version_h[gCarCheck.fdu_index-1] = 0;
                           gCarCheck.fdu_version_l [gCarCheck.fdu_index-1] = 0;
                    }
		}           
	}
	

	// 巡检二级总线设备
	//CarDataSendRepeat();
}


void MapInit(void)
{
#if 0
	u8 sendBuf[15] = {0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFE};

	sendBuf[13] = CalcuSum(&sendBuf[1], 12);
	uart2_write_buf( sendBuf, 15 );
#endif	
}


void CarDataInit()
{

	//CarDataClearDmp();
/*
	//发送版本号
	buf[0] = DEVICE_SOFTWARE_VERSION_HIGHT;
	buf[1] = DEVICE_SOFTWARE_VERSION_LOW;
	CarDataSend( buf, 2 );
	LED_CAR_COM_OPEN();
*/	
}

static u8 GetVauData(void)
{
	u8 temp;
	//u8 check_sum;
	//u8 cnt;
	
	while(uart3_read_char(&temp))
	{			
		//if(temp == TBA_PACKET_TAB)
		if (recv_flag == 0)
		{
			if(temp == TBA_PACKET_TAB)
			{
				recv_flag = 1; //  帧头
				gVauRecvPaketLength=0;
			}
		}
		//其他字符串直接送缓冲区
		else  if(recv_flag == 1)
		{
			gVauRecvPaketBuf[gVauRecvPaketLength++] = temp;
			
			if(temp == 0x7F)   // 帧尾
			{
				recv_flag = 0;
				if(gVauRecvPaketLength == 10)
				{
					//for(cnt=0;cnt<8;cnt++)
					//{
					//	check_sum += gVauRecvPaketBuf[cnt];
					//}
					
					//if(check_sum == gVauRecvPaketBuf[8])
						return 1;
				}
			}
			if(gVauRecvPaketLength >= 255 )
			{
				gVauRecvPaketLength = 0;
				recv_flag = 0;
			}
		}
	}
	return 0;
}

/*
u8 VauDataSend( u8* buf, u8 length )
{
	u8 sendBuf[126];
	u8 sendLen;
	sendLen = PackageData( sendBuf, buf, length );
	uart3_write_buf( sendBuf, sendLen );
        return sendLen;
}
*/

void Vau_set_panel_volume(u8 volume)
{
	panel_volume = volume ;
}

u8 Vau_get_panel_volume(void)
{
	return panel_volume;
}

void Vau_set_panel_mode(u8 volume)
{
	panel_mode = volume ;
}

u8 Vau_get_panel_mode(void)
{
	return panel_mode;
}

void Vau_send(void)
{
	u8 sendBuf[50];
	u8 sendLen;
       u32 adc_temp;
	u8 tempBuf[50];
	u8 cnt=0;
       u8 i =0 ;
	u8 check_sum =0;
       u8 panel_mode_car_id=0;

	//tempBuf[cnt++]  = (u8)0x7E;
	tempBuf[cnt++]  = (u8)0x00;
	tempBuf[cnt++]  = (u8)0x19;
	tempBuf[cnt++]  = (u8)0x00;
	tempBuf[cnt++]  = (u8)(0x03|(GetDeviceId()<<4));
	tempBuf[cnt++]  = (u8)0x02; // cmd
	tempBuf[cnt++]  = (u8)0x16; // len

	tempBuf[cnt++]  = (u8)(gpiscData.data_area.panel_mode&0x0f);

	//if(tempBuf[7] == 0)
	//	return;

       panel_mode_car_id = (gpiscData.data_area.panel_mode >>4)&0x0f;

	if((gpiscData.data_area.panel_mode&0x0f)==1)   // 等于集控档
	{
		if((panel_mode_car_id == GetDeviceId())
              ||(panel_mode_car_id == 0x0F))
			tempBuf[cnt++]  = gpiscData.data_area.panel_volume;
		else
			tempBuf[cnt++]  = 0; 
	}
	else
		tempBuf[cnt++]  = 0;

      for(i=0;i<5;i++)
      {
          adc_temp = GetAdcData(i);
          tempBuf[cnt++]  = adc_temp; 
          tempBuf[cnt++]  = adc_temp>>8; 
          tempBuf[cnt++]  = adc_temp>>16; 
          tempBuf[cnt++]  = adc_temp>>24; 
      }       

      tempBuf[cnt++]  = gpiscData.data_area.tms_status.meida_play_off;

      	sendLen = PackageData( sendBuf, tempBuf, cnt );
	uart2_write_buf( sendBuf, sendLen );

}

void Vau_Proc(void)
{
	//static u8 vau_mode =0 ;
	//static u8 send_times =0 ;
	//u8 len;
	static u32 tick;
	//u8 val_recv =0; 
	//u8 check_sum;
	// 1. 收VAU 数据
	if(GetVauData())
	{
		
		tVauHeader *LVauHeader;
		//len = VerifyPaket( gVauRecvPaketBuf, gVauRecvPaketLength );
		//if(len>0)  // 处理接收数据
		{
			LVauHeader =(tVauHeader *) gVauRecvPaketBuf;
			Vau_set_panel_mode(LVauHeader->data_area[0]);	
			Vau_set_panel_volume(LVauHeader->data_area[1]);
		}
		gVauRecvPaketLength = 0;
	}
	// 2. 发送数据
	// if((gpiscData.data_area.panel_mode != panel_mode )
	//     ||((gpiscData.data_area.panel_mode>>4 )== GetDeviceId()))
	if(gSystemTick - tick > 500)
	 {	
	 	tick = gSystemTick ; 
		//vau_mode = gpiscData.data_area.panel_mode ; 
		Vau_send();
	}
	
	 
}


