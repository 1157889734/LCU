#ifndef DATA_PUBLIC_H_
#define DATA_PUBLIC_H_


#include "device_info.h"

#define  SUVEN_DEBUG   1   // ˾�����Ե���
//#define DEBUG_TEST_UART			1

#define DrmdNumMax  10  		//  10 ����̬��ͼ
#define IduNumMax  2 	 	//  4 �ڲ���
#define SduNumMax  2 	 	//  4 �ڲ���
#define FduNumMax  2 	 	//  �յ�վ��

// lcu to pis dsp devs version
#define PECU_NUM             5    
#define DRMD_NUM            10
#define IDU_NUM                2
#define SDU_NUM               2
#define FDU_NUM               1

#define LINE_MAX_STATION_NUM				30


//�豸���Ͷ���
#define DEVICE_TYPE_PISC						0x01	//���������
#define DEVICE_TYPE_BCB						0x02	//�㲥���ƺ�
#define DEVICE_TYPE_LCU						0x03	//���ؿ�����
#define DEVICE_TYPE_DMP						0x04
#define DEVICE_TYPE_EHP						0x05	//�Խ�������
#define DEVICE_TYPE_IDU						0x06	//�ڲ���
#define DEVICE_TYPE_FDU						0x07    //ǰ����
#define DEVICE_TYPE_SDU						0x08    //�ಿ��
#define DEVICE_TYPE_VAU						0x09	   //VAU



#define PROCESS_PACKET			0x00
#define P2P_PACKET				0x01
#define TOKEN_PACKET			0x02
#define CLR_TOKEN				0x03
#define DOWNLOAD_PACKET		0x04


//����Э�����ݰ�ͷ
typedef struct
{
    u8 dest_net_id;
	u8 dest_device_id;
	u8 src_net_id;
	u8 src_device_id;
	u8 cmd;
	u8 len;
}tBaseProtocolHeader;


typedef enum{
	RUN_STAT_PRE_DOOR_SIDE = 0,
	RUN_STAT_OPEN_DOOR,
	RUN_STAT_CLOSE_DOOR,
	RUN_STAT_LEAVE_BROADCAST,
}eRunStat;


typedef enum {
    DOOR_SIDE_NONE = 0,
    DOOR_SIDE_LEFT = 1,
    DOOR_SIDE_RIGHT = 2,
    DOOR_SIDE_BOTH = 3,
}tDoorLorR;

// ������̬��ͼ���ݽṹ

typedef struct
{
	//u8 len;     // 1
	u8 cmd;       
	u8 start_station;   // 3
	u8 end_station;	
	u8 current_station;// 5 ��ǰվ
	u8 next_station;	
	//u8 line_id;
	u8 door;                //0--close  1--left  2--right  3--all
	u8 tms_status_display;  // 7
	u8 year;
	u8 month;
	u8 date; // 10
	u8 hour;
	u8 minute;
	u8 second; //13
}tStationStat;

__packed typedef struct  {
	u8 dest_netnum;
	//��λ��ǰ
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
	}src_eqnum;
	u8 cmd;
	//u8 len;
	tStationStat data_area;
}tDRMDHeader;


//��������
typedef struct
{
    u8 update_type;             //Ϊ1��ʱ���ǳ�������
    u8 update_flag;             // 1:��ʼ������2:�������ݣ�3:��������
    u8 lowest_bye_total_len;    //�ܳ��ȵ�����ֽ�
    u8 lower_bye_total_len;     //�ܳ��ȵĴε��ֽ�
    u8 higher_bye_total_len;    //�ܳ��ȵĴθ��ֽ�
    u8 highest_bye_total_len;   //�ܳ��ȵ�����ֽ�
    u8 low_byte_cur_packet_num; //��ǰ���ŵĵ��ֽ�
    u8 high_byte_cur_packet_num;//��ǰ���ŵĸ��ֽ�
}tUpdateHeader;


typedef struct
{
    tUpdateHeader header;
    u8 data[255];
}tUpdateData;

//��������������
typedef struct
{
    tBaseProtocolHeader header;
    tUpdateData         data;
}tUpdateFrame;



__packed typedef struct
{
	u8 len;
	u8 cmd;
	u8 step;
	u8 packet_num_h;
	u8 packet_num_l;
	u8 data_area[3];
}tUsbUpdatePacket;

__packed typedef struct 
{
	u8 dest_netnum;
	//��λ��ǰ
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
	}src_eqnum;
	__packed struct
	{
		u8 packet_type:4;
		u8 token_return:1;
		u8 m_active:1;
		u8 m_error:1;
		u8 m_include:1;			
	}cmd;
	u8 len;
}tFrameHeader;



//===========================��������=======================================================


__packed typedef struct 
{
    //������־---��λ��ǰ
    __packed struct
    {
        u8 leave_broadcast:1;
        u8 arrived_broadcast:1;
        u8 jump:1;
        u8 urgent_broadcast:1;
        u8 closedoor:1;
        u8 opendoor:1;
        u8 leave:1;
        u8 reach_stop:1;
    }trigger_hbit;

    u8 start_station;//��ʼվ
    u8 end_station;//�յ�վ
    u8 current_station;//��ǰվ
    u8 next_station;//��һվ
    u8 jump_broadcast_no;//Խվ�㲥����
    u8 urgent_broadcast_no;//�����㲥����  7
	
    //״̬��־---��λ��ǰ
    __packed struct
    {
        u8 jump_valid:1;
        u8 active:1;
        u8 master:1;
        u8 dir_valid:1;
        u8 door_sides:2;    //0--close  1--left  2--right  3--all
        u8 dir_down:1;
        u8 dir_up:1;
    }status_hbit;
	
    //uint32 jump_stations;//Խվ����
    u8 city_no;//���к�
    u8 year;
    u8 month;
    u8 date;        // 12	
	
    //�㲥���ȼ�
    __packed struct
    {
        u32 pecu:4;
        u32 talk:4;
        u32 manual:4;
        u32 occ:4;
        u32 media:4;
        u32 dva:4;
        u32 door:4;
        u32 special:4;
    }broadcast_priority;   // 13 - 16
	
	
    //�㲥��Դ---��λ��ǰ
    __packed struct
    {
        u8 broadcast_req:1;
        u8 broadcasting:1;
        u8 broadcast_type:6;
    }broadcast_signal;  // 17

    //32��������״̬
    //u8 esu_status[8];
    u16 esu_status[8];   // 18 - 33

    //ý����ʾģʽ
    u8 line_no;   // 34 

    //˾����״̬
    __packed struct
    {
        u8 media_error:1;
        u8 dva_error:1;
        u8 reserve:6;
    }cab_status;   // 35 

    __packed struct
    {
        u8 tms_valid:1;
        u8 tms_active:1;
        u8 lamp_test:1;
        u8 atc_valid:1;
        u8 map_display:1;  // 1:�رն�̬��ͼ
        u8 tms_mode:1;    // Operation in HMI : =1 manual mode (TCMS) =0 auto mode.(ATC, default)
        u8 meida_play_off:1;
        u8 reserve:1;
    }tms_status;  // 36 

    u8 panel_mode;//������õ�����ģʽ	  37
    u8 panel_volume;//������õ���������	  38

    u8 dcp_volume;// 39
    u8 hour;   // 40
    u8 min;   // 41
    __packed struct
    {
        u8 led_test:1;
        u8 broadcast_test:1;
        u8 master_req:1;
        u8 slave_req:1;
        u8 reserve:1;
        u8 link_flag:1;
        u8 run30p:1;
        u8 run60p:1;
    }test_st; // 42

    u8 hmi_select_pecu;  // 43 hmi ѡ���ͨ��������id��
    u8 pisc_version_h;   // 44
    u8 pisc_version_l;   // 45
    u8  car_index;         // 46
    __packed struct
    {
        u8  door1:1;
        u8  door2:1;
        u8  door3:1;
        u8  door4:1;
        u8  door5:1;
        u8  door6:1;
        u8  door7:1;
        u8  door8:1;
    }door_err1;               //  47
    __packed struct
    {
        u8  door9:1;
        u8  door10:1;
        u8  reserve:6;
    }door_err2;               //  48
    u8 reserve[2];        // 49-50	
}pisc_data_t;

__packed typedef struct  {
	u8 dest_netnum;
	//��λ��ǰ
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;
	}src_eqnum;
	u8 cmd;
	u8 len;
	pisc_data_t data_area;
}tPiscHeader;

//===========================��������END=======================================================



//============================�㲥���ƺ�����=====================================================


__packed typedef struct 
{

	u8 start_station;//��ʼվ
	u8 end_station;//�յ�վ
	u8 current_station;//��ǰվ
	u8 next_station;//��һվ
	u8 jump_broadcast_no;//Խվ�㲥����
	u8 urgent_broadcast_no;//�����㲥����

	//������־---��λ��ǰ
	__packed struct
	{
		u8 leave_broadcast:1;
		u8 arrived_broadcast:1;
		u8 jump:1;
		u8 urgent_broadcast:1;
		u8 man_req:1;
		u8 man_ing:1;
		u8 brdcst_mute:1;
		u8 brdcst_stop:1;
	}trigger_hbit;  //7
	
	__packed struct
	{
		u8 talk_st:2;	//0:�޶Խ�  1:�жԽ�
		u8 link:1;   //����״̬λ
		u8 pas_talk:1;  //�˿ͽ��������Խ�
		u8 up_down:1;	//0:����   1:����  ���д�С����
		u8 test_st:1;
		u8 force_st:1;
		u8 active:1;
	}status_hbit;  //8

	u16 esu_status[8];  // 9-24һ�ڳ�����2���ֽڱ�ʾ����������(8��)  8*8 ��64��  

	u8 jump_stations[6];// 25-30 32��Խվ��Ϣ

        __packed struct
	{
		u8 moni_broadcast:4;	
		u8 moni_driver_talk:4;	
		u8 moni_pas_talk:4;	
		u8 mic_broadcast:4; 
		u8 mic_driver_talk:4; 
		u8 mic_pas_talk:4; 
	}volume; //31-33

	__packed struct
	{
		u8 pisc_mode:2;
		u8 city_id:2;
		u8 moni_broadcast_volume:4; // ��������
	}line_mode;   //34

       u8 reserve35_36[2]; // 35-36

	u8 box_version_h; //  37
	u8 box_version_l;  //  38

       u8 version_h; //  39
	u8 version_l;  //  40
	
}bcb_data_t;

__packed typedef struct  {
	u8 dest_netnum;
	//��λ��ǰ
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
	}src_eqnum;
	u8 cmd;
	u8 len;
	bcb_data_t data_area;
}tBcbHeader;

//============================�㲥���ƺ�����END=================================================




//============================���ؿ���������============================


__packed typedef struct 
{
    //u8 ehp_st;   //������״̬           8  7  6 5  4  3 2  1         
    u16 pecust1_8 ;   //                   00 00 00 00 00 00 00 00
    u8 pecu_err;	  
    u8 spk_st;
    u8 led_st;
    u8 dmp1_st;
    u8 dmp2_st; 
    u8 vau_volume_mode;  // 8
    u8 vau_volume_value;  // 9
    u8 pecu_version[PECU_NUM]; // 10-14
    u8 sapu_version;           // 15
    u8 vau_version;             // 16
    u8 drmd_version[DRMD_NUM]; // 17-26
    u8 idu_version[IDU_NUM];  // 27-28
    u8 sdu_version[SDU_NUM]; // 29-30
    u8 fdu_version[FDU_NUM];  // 31  
    u8 lcu_version;   // 32
    u8 reserve31_32[2]; // 33-34
}lcu_data_t;

__packed typedef struct  {
	u8 dest_netnum;
	//��λ��ǰ
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
	}src_eqnum;
	u8 cmd;
	u8 len;
	lcu_data_t data_area;
}tLcuHeader;

//============================���ؿ���������END============================

//�г�Ѳ��
typedef struct
{
	u8 token;           //��������
	u8 drmd_index;  //��̬��ͼindex
	u8 idu_index;		//������index
	u8 sdu_index;
	u8 fdu_index;    
	u8 drmd_error[2]; //��̬��ͼ����λ
	u8 idu_error;
       u8 sdu_error;
       u8 fdu_error;
	u8 drmdpoll_error_cnt[DrmdNumMax];  //��̬��ͼѲ���޻ظ�����
	u8 idupoll_error_cnt[IduNumMax];
    	u8 sdupoll_error_cnt[SduNumMax];
    	u8 fdupoll_error_cnt[FduNumMax];        
       u8 drmd_version_h[DrmdNumMax];
       u8 drmd_version_l [DrmdNumMax];
       u8 idu_version_h[IduNumMax];
       u8 idu_version_l [IduNumMax];
       u8 sdu_version_h[SduNumMax];
       u8 sdu_version_l [SduNumMax];     
       u8 fdu_version_h[FduNumMax];
       u8 fdu_version_l [FduNumMax];       
	u8 poll_type;   //0 Ϊdrmd   ��  1Ϊ idu    2: sdu   3: fdu
	u32 poll_time_value;  //Ѳ��ʱ��
}tCarCheck;

extern tCarCheck gCarCheck;

// VAU 
__packed typedef struct  {
	u8 dest_netnum;
	//��λ��ǰ
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
		
	}dest_eqnum;
	u8 src_netnum;
	__packed struct
	{
		u8 eq_type:4;
		u8 eq_num:4;		
	}src_eqnum;
	u8 cmd;
	u8 len;
	u8 data_area[6];
}tVauHeader;


extern u8 lcu_st;
extern u8 VerifyPaket(u8 *paketBuf, u16 length);
extern u8 PackageData( u8 *destBuf, u8 *srcBuf, u8 length );
extern void CheckUartSendControl();

#endif


