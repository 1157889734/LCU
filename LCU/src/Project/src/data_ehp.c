/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 处理紧急报警器总线数据，其他模块获取紧急报警器总线数据都从这里获得
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
#include "data_ehp.h"
#include "data_train.h"
#include "led_control.h"


static u8 gAlarmRecvPaketBuf[256];
static u8 gAlarmRecvPaketLength = 0;

static u8 gEhpCommStat = 0;
static u16 gEhpAlarmStat = 0;
static u32 pecu_adc[5]={0,0,0,0,0};
static tEhpTokenStat gEhpTokenStat;

u32 GetAdcData(u8 pecu_id)
{
    return pecu_adc[pecu_id];
}

u8 GetEhpComStat()
{
	return gEhpCommStat;
}

u16 GetEhpAlarmStat()
{
	return gEhpAlarmStat;
}

u8 GetPttStat()
{
	static u8 ptt_stat = 1;
	u32 tick = 0;
	
	if (0 == PTT_CHK())
	{
		if (1 == ptt_stat)
		{
			if (gSystemTick - tick > 50)
			{
				ptt_stat = 0;
			}
		}
		else 
		{
			tick = gSystemTick;
		}
	}
	else 
	{
		if (0 == ptt_stat)
		{
			if (gSystemTick - tick > 50)
			{
				ptt_stat = 1;
			}			
		}
		else 
		{
			tick = gSystemTick;
		}
	}
	return ptt_stat;
}

static void CarDataAudioChannelSwitch()
{
	u8 talk_flag = 0;
	u8 i;
	eEhpBcbStat stat;

	// 判断本客室是否有正在接通的对话
	for ( i=0; i<EHP_MAX_NUM; i++ )
	{
		stat = TrainDataGetEhpStat( i+1 );
		if ( EHP_STAT_ANSWER == stat )
		{
			talk_flag = 1;
			break;
		}
	}

	// 本客室有对讲正在进行
	if (talk_flag == 1)
	{
		LedAlarmStatSet( LED_ALARM_TYPE_CONN, LED_ALARM_CONN );
		// 司机听，乘客讲,
		if ( 1 == GetPttStat() )
		{
			LedPcStatSet( LED_PC_TALK );
			PTT_CTRL_FREE();
			SPEAK_CTRL_DISCONN();
			LISTEN_CTRL_CONN();
		}
		else  //总线拉低司机讲
		{
			LedPcStatSet( LED_PC_LISTEN );
			PTT_CTRL_PULL_DOWN();
			SPEAK_CTRL_CONN();
			LISTEN_CTRL_DISCONN();
		}
	}
	else  // 本客室没有对讲在进行
	{
		LedPcStatSet( LED_PC_IDLE );
		LedAlarmStatSet( LED_ALARM_TYPE_CONN, LED_ALARM_IDLE );
		SPEAK_CTRL_DISCONN();
		LISTEN_CTRL_DISCONN();
		PTT_CTRL_FREE();
	}

	
}


/*
Ehp故障个数 = LED 灯显示次数
*/
static void EhpDataLedAlarmSet()
{
	u8 cnt = 0;
	u8 i;
	u8 temp;
	u16 temp1;

	//判断是否存在没有通讯的紧急报警器
	temp = gEhpCommStat;
	for ( i=0;i<EHP_MAX_NUM;i++ )
	{
		if (temp & 0x01) 
		{
			cnt ++;
		}
		temp >>= 1;
	}
	//if(GetDeviceId()==car5_pecu||GetDeviceId()==car6_pecu)
	//	LedAlarmStatSet( LED_ALARM_TYPE_LOSE, (eLedAlarmStat)(LED_ALARM_IDLE+cnt-2) ); // 只有3个
	//else
		LedAlarmStatSet( LED_ALARM_TYPE_LOSE, (eLedAlarmStat)(LED_ALARM_IDLE+cnt) );

	cnt = 0;
	//判断是否有报警
	temp1 = gEhpAlarmStat;
	for ( i=0;i<EHP_MAX_NUM;i++ )
	{
		if ((temp1 & 0x0001) == 0x01) 
		{
			cnt ++;  //查找正则报警的报警器个数
		}
		temp1 >>= 2;
	}
	if (cnt > 0)
	{
		LedAlarmStatSet( LED_ALARM_TYPE_ALARM, LED_ALARM_ALARM );
	}
	else 
	{
		LedAlarmStatSet( LED_ALARM_TYPE_ALARM, LED_ALARM_IDLE );
	}
	
}

u8 EhpDataSend( u8* buf, u8 length )
{
	u8 sendBuf[255];
	u8 sendLen;
	sendLen = PackageData( sendBuf, buf, length );
	uart2_write_buf( sendBuf, sendLen );
        return 0;
}

static u8 EhpDataGetPacket(void)
{
	u8 temp;
	while(uart2_read_char(&temp))
	{			
		if(temp == TBA_PACKET_TAB)
		{
			//判断长度 判断是否有数据
			if(gAlarmRecvPaketLength >= 3)
			{
				return 1;
			}
			gAlarmRecvPaketLength=0;
		}
		//其他字符串直接送缓冲区
		else
		{
			gAlarmRecvPaketBuf[gAlarmRecvPaketLength] = temp;
			if( ++gAlarmRecvPaketLength >= 255 )
				gAlarmRecvPaketLength = 0;
		}
	}
	return 0;
}



#ifdef DEBUG_TEST_UART
void EhpDataProc(void)
{
	tFrameHeader *frameHeader;
	tPiscHeader *piscData;
	u8 deviceId;
	u8 debugBuf[50] = {0xaa, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	u8 len;

	if (EhpDataGetPacket()) 
	{
		len = VerifyPaket( gAlarmRecvPaketBuf, gAlarmRecvPaketLength );
		if ( len > 0 ) 
		{
			frameHeader = (tFrameHeader *)gAlarmRecvPaketBuf;
			switch (frameHeader->cmd.packet_type) 
			{
				case PROCESS_PACKET:
					//if ((frameHeader->dest_eqnum.eq_num != 0xF) || (frameHeader->dest_eqnum.eq_type != 0xF)) break;
					
					//是否为过程数据
					if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_PISC ) 
					{
						EhpDataSend( gAlarmRecvPaketBuf, gAlarmRecvPaketLength-1 );					
					}
					else if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_BCB ) 
					{
						//解析数据 保存对讲状态
						// TO DO ...
					}
					break;
				case TOKEN_PACKET:
					EhpDataSend( gAlarmRecvPaketBuf, gAlarmRecvPaketLength-1 );
					break;
				case DOWNLOAD_PACKET:
					EhpDataSend( debugBuf, 16 );
					break;
				default :
					break;
			}
		}
		gAlarmRecvPaketLength = 0;
	}

}

#else 
/*
功能:   等待回复巡检命令
返回:   0: 数据错误,继续巡检下一个设备
        1: 继续等待
        2: 成功返回
*/
static u8 EhpDataWaitReply()
{
    tFrameHeader *frameHeader;
    u8 ret = 1;
    u16 temp;
    u8 len;

    if (EhpDataGetPacket()){  //  读取485数据，格式7E头 7E尾    
    
        len = VerifyPaket( gAlarmRecvPaketBuf, gAlarmRecvPaketLength );
        
        if ( len > 0 ){
            frameHeader = (tFrameHeader *)gAlarmRecvPaketBuf;            
            if ( frameHeader->cmd.token_return ){
                if ( (frameHeader->src_eqnum.eq_type == DEVICE_TYPE_EHP) && (frameHeader->src_eqnum.eq_num == gEhpTokenStat.mDeviceId)){
                    temp = gAlarmRecvPaketBuf[6];
                    temp &=0x0003;
                    gEhpAlarmStat &= ~(0x0003<<(gEhpTokenStat.mDeviceId-1)*2);
                    gEhpAlarmStat |= temp << (gEhpTokenStat.mDeviceId-1)*2;

                    switch(gEhpTokenStat.mDeviceId)
                    {
                        case 1:                            
                            pecu_adc[0] = gAlarmRecvPaketBuf[10];
                            pecu_adc[0] = (pecu_adc[0]<<8)|gAlarmRecvPaketBuf[9];
                            pecu_adc[0] = (pecu_adc[0]<<8)|gAlarmRecvPaketBuf[8];
                            pecu_adc[0] = (pecu_adc[0]<<8)|gAlarmRecvPaketBuf[7];
                            
                            glcuData.data_area.pecu_version[0] = (gAlarmRecvPaketBuf[11]<<4)|(gAlarmRecvPaketBuf[12]&0x0f);
                            break;
                        case 2:
                            pecu_adc[1] = gAlarmRecvPaketBuf[10];
                            pecu_adc[1] = (pecu_adc[1]<<8)|gAlarmRecvPaketBuf[9];
                            pecu_adc[1] = (pecu_adc[1]<<8)|gAlarmRecvPaketBuf[8];
                            pecu_adc[1] = (pecu_adc[1]<<8)|gAlarmRecvPaketBuf[7];
                            
                            glcuData.data_area.pecu_version[1] = (gAlarmRecvPaketBuf[11]<<4)|(gAlarmRecvPaketBuf[12]&0x0f);                                                      

                            break;
                        case 3:
                            pecu_adc[2] = gAlarmRecvPaketBuf[10];
                            pecu_adc[2] = (pecu_adc[2]<<8)|gAlarmRecvPaketBuf[9];
                            pecu_adc[2] = (pecu_adc[2]<<8)|gAlarmRecvPaketBuf[8];
                            pecu_adc[2] = (pecu_adc[2]<<8)|gAlarmRecvPaketBuf[7];

                            glcuData.data_area.pecu_version[2] = (gAlarmRecvPaketBuf[11]<<4)|(gAlarmRecvPaketBuf[12]&0x0f);                                               
                            break;
                        case 4:
                            pecu_adc[3] = gAlarmRecvPaketBuf[10];
                            pecu_adc[3] = (pecu_adc[3]<<8)|gAlarmRecvPaketBuf[9];
                            pecu_adc[3] = (pecu_adc[3]<<8)|gAlarmRecvPaketBuf[8];
                            pecu_adc[3] = (pecu_adc[3]<<8)|gAlarmRecvPaketBuf[7];                          

                            glcuData.data_area.pecu_version[3] = (gAlarmRecvPaketBuf[11]<<4)|(gAlarmRecvPaketBuf[12]&0x0f);                                           
                            break;
                        case 5:
                            pecu_adc[4] = gAlarmRecvPaketBuf[10];
                            pecu_adc[4] = (pecu_adc[4]<<8)|gAlarmRecvPaketBuf[9];
                            pecu_adc[4] = (pecu_adc[4]<<8)|gAlarmRecvPaketBuf[8];
                            pecu_adc[4] = (pecu_adc[4]<<8)|gAlarmRecvPaketBuf[7];                           

                            glcuData.data_area.pecu_version[4] = (gAlarmRecvPaketBuf[11]<<4)|(gAlarmRecvPaketBuf[12]&0x0f);                                                
                            break;
                        default:
                            break;
                    }

                    glcuData.data_area.sapu_version = (gAlarmRecvPaketBuf[13]<<4)|(gAlarmRecvPaketBuf[14]&0x0f);// sapu
          
                    ret = 2;
                }
                if( (frameHeader->src_eqnum.eq_type == DEVICE_TYPE_VAU)){
                    //获取vau 模式状态
                    tVauHeader *LVauHeader;

    			LVauHeader =(tVauHeader *) gAlarmRecvPaketBuf;
    			Vau_set_panel_mode(LVauHeader->data_area[0]);	
    			Vau_set_panel_volume(LVauHeader->data_area[1]);

                     glcuData.data_area.vau_version = (LVauHeader->data_area[4]<<4)|(LVauHeader->data_area[5]&0x0f);
                         		
        		//gVauRecvPaketLength = 0;
                    ret = 2;
                }           
            }
        }
        else {
            ret = 0;
        }
        gAlarmRecvPaketLength = 0;
    }
    return ret;
}

/*
功能:  管理巡检紧急报警器
*/
void EhpDataProc(void)
{
    u8 ret;
    u8 sendBuf[10] = { 0X00, 0xF5, 0x00, 0xF3, 0x02, 0x01, 0xFF};
    u16 temp;
    u8 update_flag = 0;
    static u8 index=0;
    static u8 poll_dev = 0; // 巡检设备 
    static u8 poll_cnt[EHP_MAX_NUM]={0,0,0};

    if ( 1 == gEhpTokenStat.mWaitFlag ){
        ret = EhpDataWaitReply();

        #if 0
        if ( 0 == ret ){ //收到错误包
        
            if(poll_dev == DEVICE_TYPE_EHP ){

                poll_cnt[gEhpTokenStat.mDeviceId-1]++;
                if(poll_cnt[gEhpTokenStat.mDeviceId-1]>=4){       
                    poll_cnt[gEhpTokenStat.mDeviceId-1] = 4;
                    gEhpCommStat |= (0x01 << (gEhpTokenStat.mDeviceId-1));//记录故障
                    temp = 0x0003;
                    temp <<= ((gEhpTokenStat.mDeviceId-1)*2);
                    gEhpAlarmStat &= ~temp;   //清除该pecu 报警状态
                    gEhpTokenStat.mWaitFlag = 0;
                    gEhpTokenStat.mTick = gSystemTick;
                }
            }
            else if(poll_dev == DEVICE_TYPE_VAU){
                //记录vau 故障
                gEhpTokenStat.mWaitFlag = 0;
                gEhpTokenStat.mTick = gSystemTick;
            }
        }
        else if ( 1 == ret ){
            
            if (( gSystemTick - gEhpTokenStat.mTick > EHP_TOKEN_WAIT_OUTTIME )&&
                (poll_dev == DEVICE_TYPE_EHP )){

                poll_cnt[gEhpTokenStat.mDeviceId-1]++;
                if(poll_cnt[gEhpTokenStat.mDeviceId-1]>=4){       
                    poll_cnt[gEhpTokenStat.mDeviceId-1] = 4;
                    gEhpCommStat |= (0x01 <<  (gEhpTokenStat.mDeviceId-1));//记录故障
                    temp = 0x0003;
                    temp <<= ((gEhpTokenStat.mDeviceId-1)*2);
                    gEhpAlarmStat &= ~temp;   //清除该pecu 报警状态

                    gEhpTokenStat.mWaitFlag = 0;	
                    gEhpTokenStat.mTick = gSystemTick;
                    ret = 0;
                }
            }
            
            if(( gSystemTick - gEhpTokenStat.mTick > EHP_TOKEN_WAIT_OUTTIME )&&
                (poll_dev == DEVICE_TYPE_VAU )){
                //记录vau 故障
                gEhpTokenStat.mWaitFlag = 0;	
                gEhpTokenStat.mTick = gSystemTick;
                ret = 0;
            }
        } 
        #endif
        if ( 2 == ret ){
            LED_EHP_COM_OPEN();

            if(poll_dev == DEVICE_TYPE_EHP ){

                #if SUVEN_DEBUG
                if(gEhpTokenStat.mDeviceId > EHP_MAX_NUM){
                    lcu_st = 0x05;
                    return;
                }
                #endif
                
                poll_cnt[gEhpTokenStat.mDeviceId-1]=0;
                gEhpCommStat &= ~(0x01 <<  (gEhpTokenStat.mDeviceId-1));//清除故障
                gEhpTokenStat.mTick = gSystemTick;
                gEhpTokenStat.mWaitFlag = 0;
            }
            else if(poll_dev == DEVICE_TYPE_VAU ){
                // 清除vau故障
                gEhpTokenStat.mTick = gSystemTick;
                gEhpTokenStat.mWaitFlag = 0;
            }
        }

        if ( gSystemTick - gEhpTokenStat.mTick > EHP_TOKEN_WAIT_OUTTIME ){
            gEhpTokenStat.mWaitFlag = 0;
            if(poll_dev == DEVICE_TYPE_EHP ){

                poll_cnt[gEhpTokenStat.mDeviceId-1]++;
                if(poll_cnt[gEhpTokenStat.mDeviceId-1]>=5){       
                    poll_cnt[gEhpTokenStat.mDeviceId-1] = 5;
                    gEhpCommStat |= (0x01 << (gEhpTokenStat.mDeviceId-1));//记录故障
                    temp = 0x0003;
                    temp <<= ((gEhpTokenStat.mDeviceId-1)*2);
                    gEhpAlarmStat &= ~temp;   //清除该pecu 报警状态
                    gEhpTokenStat.mTick = gSystemTick;


                    switch(gEhpTokenStat.mDeviceId)
                    {
                        case 1:               
                            glcuData.data_area.pecu_version[0]= 0;                            
                            break;
                        case 2:
                            glcuData.data_area.pecu_version[1]= 0;                                                                      

                            break;
                        case 3:
                            glcuData.data_area.pecu_version[2]= 0;                                                                                    
                            break;
                        case 4:                       
                            glcuData.data_area.pecu_version[3]= 0;                                                                             
                            break;
                        case 5:                        
                            glcuData.data_area.pecu_version[4]= 0;                                                                       
                            break;
                        default:
                            break;
                    }
                }
            }
            else if(poll_dev == DEVICE_TYPE_VAU){
                //记录vau 故障
                gEhpTokenStat.mTick = gSystemTick;
                glcuData.data_area.vau_version = 0;
            }            
        }
    }
    else {
        
        if ( gSystemTick - gEhpTokenStat.mTick > EHP_TOKEN_INTERVAL_TIME ){

           update_flag = get_update_flag();
           if(update_flag ==I485_UPDATE_FLAG)
                return;
            
            if(index%3==1){  // 巡检VAU
            //if(1){
                Vau_send();
                gEhpTokenStat.mTick = gSystemTick;
                gEhpTokenStat.mWaitFlag = 1;
                poll_dev = DEVICE_TYPE_VAU;
            }
            else{ // 巡检紧急报警器
                gEhpTokenStat.mDeviceId ++;
                
                if ( gEhpTokenStat.mDeviceId > EHP_MAX_NUM ){
                    gEhpTokenStat.mDeviceId = 1;
                } 
                
                temp = gEhpTokenStat.mDeviceId;
                temp <<= 4;
                temp += 0x05;
                sendBuf[1] = temp;
                temp = GetDeviceId();
                temp <<= 4;
                temp += 3;
                sendBuf[3] = temp;
                sendBuf[6] = (u8)TrainDataGetEhpStat( gEhpTokenStat.mDeviceId );

                EhpDataSend( sendBuf, 7 );
                gEhpTokenStat.mTick = gSystemTick;
                gEhpTokenStat.mWaitFlag = 1;
                poll_dev = DEVICE_TYPE_EHP;
            }
            index++;
        }
    }

    EhpDataLedAlarmSet();
    //CarDataAudioChannelSwitch(); // 拉报警器通话对讲总线 ， 移动到对讲控制器
}


void EhpDataInit()
{
        
	gEhpTokenStat.mDeviceId = 1;
	gEhpTokenStat.mTick = gSystemTick;
	gEhpTokenStat.mWaitFlag = 0;

      
}

#endif



