/*
*******************************************************************************
**  Copyright (c) 2013, �����б��������ͨ�������޹�˾
**  All rights reserved.
**	
**  �ļ�˵��: ��������������������ݣ�����ģ���ȡ�����������������ݶ���������
**  ��������: 2013.12.19
**
**  ��ǰ�汾��1.0
**  ���ߣ����
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

	// �жϱ������Ƿ������ڽ�ͨ�ĶԻ�
	for ( i=0; i<EHP_MAX_NUM; i++ )
	{
		stat = TrainDataGetEhpStat( i+1 );
		if ( EHP_STAT_ANSWER == stat )
		{
			talk_flag = 1;
			break;
		}
	}

	// �������жԽ����ڽ���
	if (talk_flag == 1)
	{
		LedAlarmStatSet( LED_ALARM_TYPE_CONN, LED_ALARM_CONN );
		// ˾�������˿ͽ�,
		if ( 1 == GetPttStat() )
		{
			LedPcStatSet( LED_PC_TALK );
			PTT_CTRL_FREE();
			SPEAK_CTRL_DISCONN();
			LISTEN_CTRL_CONN();
		}
		else  //��������˾����
		{
			LedPcStatSet( LED_PC_LISTEN );
			PTT_CTRL_PULL_DOWN();
			SPEAK_CTRL_CONN();
			LISTEN_CTRL_DISCONN();
		}
	}
	else  // ������û�жԽ��ڽ���
	{
		LedPcStatSet( LED_PC_IDLE );
		LedAlarmStatSet( LED_ALARM_TYPE_CONN, LED_ALARM_IDLE );
		SPEAK_CTRL_DISCONN();
		LISTEN_CTRL_DISCONN();
		PTT_CTRL_FREE();
	}

	
}


/*
Ehp���ϸ��� = LED ����ʾ����
*/
static void EhpDataLedAlarmSet()
{
	u8 cnt = 0;
	u8 i;
	u8 temp;
	u16 temp1;

	//�ж��Ƿ����û��ͨѶ�Ľ���������
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
	//	LedAlarmStatSet( LED_ALARM_TYPE_LOSE, (eLedAlarmStat)(LED_ALARM_IDLE+cnt-2) ); // ֻ��3��
	//else
		LedAlarmStatSet( LED_ALARM_TYPE_LOSE, (eLedAlarmStat)(LED_ALARM_IDLE+cnt) );

	cnt = 0;
	//�ж��Ƿ��б���
	temp1 = gEhpAlarmStat;
	for ( i=0;i<EHP_MAX_NUM;i++ )
	{
		if ((temp1 & 0x0001) == 0x01) 
		{
			cnt ++;  //�������򱨾��ı���������
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
			//�жϳ��� �ж��Ƿ�������
			if(gAlarmRecvPaketLength >= 3)
			{
				return 1;
			}
			gAlarmRecvPaketLength=0;
		}
		//�����ַ���ֱ���ͻ�����
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
					
					//�Ƿ�Ϊ��������
					if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_PISC ) 
					{
						EhpDataSend( gAlarmRecvPaketBuf, gAlarmRecvPaketLength-1 );					
					}
					else if ( frameHeader->src_eqnum.eq_type == DEVICE_TYPE_BCB ) 
					{
						//�������� ����Խ�״̬
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
����:   �ȴ��ظ�Ѳ������
����:   0: ���ݴ���,����Ѳ����һ���豸
        1: �����ȴ�
        2: �ɹ�����
*/
static u8 EhpDataWaitReply()
{
    tFrameHeader *frameHeader;
    u8 ret = 1;
    u16 temp;
    u8 len;

    if (EhpDataGetPacket()){  //  ��ȡ485���ݣ���ʽ7Eͷ 7Eβ    
    
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
                    //��ȡvau ģʽ״̬
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
����:  ����Ѳ�����������
*/
void EhpDataProc(void)
{
    u8 ret;
    u8 sendBuf[10] = { 0X00, 0xF5, 0x00, 0xF3, 0x02, 0x01, 0xFF};
    u16 temp;
    u8 update_flag = 0;
    static u8 index=0;
    static u8 poll_dev = 0; // Ѳ���豸 
    static u8 poll_cnt[EHP_MAX_NUM]={0,0,0};

    if ( 1 == gEhpTokenStat.mWaitFlag ){
        ret = EhpDataWaitReply();

        #if 0
        if ( 0 == ret ){ //�յ������
        
            if(poll_dev == DEVICE_TYPE_EHP ){

                poll_cnt[gEhpTokenStat.mDeviceId-1]++;
                if(poll_cnt[gEhpTokenStat.mDeviceId-1]>=4){       
                    poll_cnt[gEhpTokenStat.mDeviceId-1] = 4;
                    gEhpCommStat |= (0x01 << (gEhpTokenStat.mDeviceId-1));//��¼����
                    temp = 0x0003;
                    temp <<= ((gEhpTokenStat.mDeviceId-1)*2);
                    gEhpAlarmStat &= ~temp;   //�����pecu ����״̬
                    gEhpTokenStat.mWaitFlag = 0;
                    gEhpTokenStat.mTick = gSystemTick;
                }
            }
            else if(poll_dev == DEVICE_TYPE_VAU){
                //��¼vau ����
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
                    gEhpCommStat |= (0x01 <<  (gEhpTokenStat.mDeviceId-1));//��¼����
                    temp = 0x0003;
                    temp <<= ((gEhpTokenStat.mDeviceId-1)*2);
                    gEhpAlarmStat &= ~temp;   //�����pecu ����״̬

                    gEhpTokenStat.mWaitFlag = 0;	
                    gEhpTokenStat.mTick = gSystemTick;
                    ret = 0;
                }
            }
            
            if(( gSystemTick - gEhpTokenStat.mTick > EHP_TOKEN_WAIT_OUTTIME )&&
                (poll_dev == DEVICE_TYPE_VAU )){
                //��¼vau ����
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
                gEhpCommStat &= ~(0x01 <<  (gEhpTokenStat.mDeviceId-1));//�������
                gEhpTokenStat.mTick = gSystemTick;
                gEhpTokenStat.mWaitFlag = 0;
            }
            else if(poll_dev == DEVICE_TYPE_VAU ){
                // ���vau����
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
                    gEhpCommStat |= (0x01 << (gEhpTokenStat.mDeviceId-1));//��¼����
                    temp = 0x0003;
                    temp <<= ((gEhpTokenStat.mDeviceId-1)*2);
                    gEhpAlarmStat &= ~temp;   //�����pecu ����״̬
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
                //��¼vau ����
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
            
            if(index%3==1){  // Ѳ��VAU
            //if(1){
                Vau_send();
                gEhpTokenStat.mTick = gSystemTick;
                gEhpTokenStat.mWaitFlag = 1;
                poll_dev = DEVICE_TYPE_VAU;
            }
            else{ // Ѳ�����������
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
    //CarDataAudioChannelSwitch(); // ��������ͨ���Խ����� �� �ƶ����Խ�������
}


void EhpDataInit()
{
        
	gEhpTokenStat.mDeviceId = 1;
	gEhpTokenStat.mTick = gSystemTick;
	gEhpTokenStat.mWaitFlag = 0;

      
}

#endif



