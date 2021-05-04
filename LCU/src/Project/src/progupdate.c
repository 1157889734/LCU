/*
*******************************************************************************
**  Copyright (c) 2013, 深圳市北海轨道交通技术有限公司
**  All rights reserved.
**	
**  文件说明: 升级程序 对FLASH的操作
**  创建日期: 2013.12.24
**
**  当前版本：1.0
**  作者：李军
*******************************************************************************
*/
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "core_cm3.h"
#include "progupdate.h"
#include "string.h"
#include "data_public.h"
#include "data_train.h"
#include "hw_config.h"
#include "led_control.h"

#define ERASEATSTART
#define FLASH_PAGE_LEN 1024

static void UpdateDisableAllNVIC(void)
{
  for(int i=19; i < 59; i++)
  {
    NVIC->ICER[i >> 0x05] =(unsigned int )0x01 << (i & (unsigned char)0x1F);
  }
}



////////////////////////////////////////////////////////////////////////////////
#define UPDATE_DATA_BUFFER_LEN 512
#define UPDATE_FUNC_BUFFER_LEN 2048
#define UPDATE_CLI()  __set_PRIMASK(1)

void map_flash_copy_func(u32 length);

static u8  update_copy_func[UPDATE_FUNC_BUFFER_LEN];          //存放拷贝程序
static u32 update_func_addr;
static u8  update_flash2flash_buffer[UPDATE_DATA_BUFFER_LEN]; // 数据缓冲区


void update_flash_init(void)
{
	update_func_addr=(u32)(update_copy_func+1);//(u32)&
}

void map_flash_copy_func_new(u32 length);
void update_flash_copy_run(u32 length)
{	
    update_copy_func[0] = 0x2d;
    memcpy((u8 *)(update_copy_func+1),(u8 *)&map_flash_copy_func,\
        UPDATE_FUNC_BUFFER_LEN-1);
    UPDATE_CLI();	
    UpdateDisableAllNVIC();
    //UpdateDisableAllGPIO();
        
    ((void(*)(u32 length0))update_func_addr)(length);//run copy
}



void map_flash_copy_func(u32 length)
{
    u32 i = 0,k = 0;
    u32 addr1 = TEMP_FLASH_ADDR,addr2 = BOOT_FLASH_ADDR; 
    u32 idx = 0;
    u32 *p = (u32*)update_flash2flash_buffer;
	FLASH_Status flashstatus;
	u8 led_flag;
    

	for(k = 0; k <= 98; k++)
	{	
    		//闪灯提示，正在搬移
		if (0 == led_flag)
		{
			led_flag = 1;
			GPIOD->BSRR = GPIO_Pin_8;
			GPIOD->BSRR = GPIO_Pin_9;
			GPIOD->BSRR = GPIO_Pin_10;
			GPIOD->BSRR = GPIO_Pin_11;
			GPIOD->BSRR = GPIO_Pin_12;
			GPIOD->BSRR = GPIO_Pin_13;
		}
		else 
		{
			led_flag = 0;
			GPIOD->BRR = GPIO_Pin_8;
			GPIOD->BRR = GPIO_Pin_9;
			GPIOD->BRR = GPIO_Pin_10;
			GPIOD->BRR = GPIO_Pin_11;
			GPIOD->BRR = GPIO_Pin_12;
			GPIOD->BRR = GPIO_Pin_13;
		}
	
		// flash unlock
		FLASH->KEYR =  ((u32)0x45670123);
		FLASH->KEYR = ((u32)0xCDEF89AB);

		FLASH->SR = FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR;		
		FLASH->CR|= ((u32)0x00000002);
		FLASH->AR = ((u32)(0x8000000 + (k * 0x800))); 
		FLASH->CR|= ((u32)0x00000040);
		IWDG->KR = ((u16)0xAAAA);

		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
		
		FLASH->CR &= ((u32)0x00001FFD);
	}

  
    while(length)
    {
    		//闪灯提示，正在搬移
    		led_flag ++;
		if (led_flag < 2)
		{
			GPIOD->BSRR = GPIO_Pin_8;
			GPIOD->BSRR = GPIO_Pin_9;
			GPIOD->BSRR = GPIO_Pin_10;
			GPIOD->BSRR = GPIO_Pin_11;
			GPIOD->BSRR = GPIO_Pin_12;
			GPIOD->BSRR = GPIO_Pin_13;
		}
		else if (led_flag < 10)
		{
			led_flag = 0;
			GPIOD->BRR = GPIO_Pin_8;
			GPIOD->BRR = GPIO_Pin_9;
			GPIOD->BRR = GPIO_Pin_10;
			GPIOD->BRR = GPIO_Pin_11;
			GPIOD->BRR = GPIO_Pin_12;
			GPIOD->BRR = GPIO_Pin_13;
		}

	
        FLASH->KEYR =  ((u32)0x45670123);
        FLASH->KEYR = ((u32)0xCDEF89AB);
        for(i=0;i<sizeof(update_flash2flash_buffer);i++)
        {
            update_flash2flash_buffer[i]=(*((volatile u8*)(addr1+i)));	
        }

	
	

        for (idx = 0; idx <  128; idx = idx + 1)
        {
				//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait

		
		FLASH->CR |= ((u32)0x00000001);
		*(__IO u16*)addr2 = (u16)p[idx];
 
		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
    

		*(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;

		//wait flash complete
		flashstatus = FLASH_BUSY;
		while(flashstatus != FLASH_COMPLETE)
		{
			if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
			{
				flashstatus = FLASH_BUSY;
			}
			else 
			{  
				if(FLASH->SR & FLASH_FLAG_PGERR)
				{ 
					flashstatus = FLASH_ERROR_PG;
				}
				else 
				{
					if(FLASH->SR & FLASH_FLAG_WRPRTERR)
					{
						flashstatus = FLASH_ERROR_WRP;
					}
					else
					{
						flashstatus = FLASH_COMPLETE;
					}
				}
			}
			IWDG->KR = ((u16)0xAAAA);
		}		        
		//end wait
     

            addr2 += 4;
		FLASH->CR &= ((u32)0x00001FFE);
        }
        addr1+=512;

        if(length>512)
        	length-=512;
	 else
	 	break;
    }

	GPIOD->BSRR = GPIO_Pin_8;
	GPIOD->BSRR = GPIO_Pin_9;
	GPIOD->BSRR = GPIO_Pin_10;
	GPIOD->BSRR = GPIO_Pin_11;
	GPIOD->BSRR = GPIO_Pin_12;
	GPIOD->BSRR = GPIO_Pin_13;

	SCB->AIRCR  = (NVIC_AIRCR_VECTKEY | (SCB->AIRCR & (0x700)) | (1<<NVIC_SYSRESETREQ));
    while(1)
    {
    }
}

/*******************************************************************************
*函数作用:保存程序数据
*参数说明:program_data: 指向数据的指针，program_total_len:程序数据
                                             的总长度
                        program_cur_num:当前包的数据包的编号
                        cur_len:当前数据包的长度.
*注意事项:数据包的编号从1开始计数，编号按1递增。每包
                        的长度必须为4的倍数,以四字节为单位，小端。
                        每包长度最大为FLASH页长度发包顺序必须从1开始
                        一直到最后后一包,若想混乱顺序发包，可在上
                        层封装一次发第一包数据的之前调用一次
                        update_init()；若不调用，在上次程序失败的情况下，
                        该次升级会失败。
*******************************************************************************/

static u32 program_rcv_len = 0;
static u16 last_pack_num = 0;

u8 update_init()
{
    program_rcv_len   = 0;
    last_pack_num     = 0;
    return update_ok;
}


void FLASH_If_Init(void)
{
   FLASH_Unlock();
   FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}


void FLASH_If_Erase_Prog_Bak(void)
{
	u32 i;

	for(i = PROG_BAK_ADDRESS; i <= (0x08060000-1024); i += FLASH_PAGE_LEN )
	{
   		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		FLASH_ErasePage(i);		
	}

}
void update_flash_erase(void)
{

    FLASH_If_Init();
    FLASH_If_Erase_Prog_Bak();
	
}
void exec_iap_fun();
u8 update_save_program(u8 *program_data,u32 program_total_len,
    u16 program_cur_num,u32 cur_len)
{
    u32 addr2,idx;
    u32 *p = (u32*)program_data;

    if(NULL == program_data||!program_total_len||!program_cur_num||!cur_len)
    {
        update_init();
        return update_param;
    }

    if(program_cur_num == 1)
    {
        update_init();
    }
    
    if(program_cur_num - last_pack_num != 1)
    {
        update_init();
        return update_uncontinuous;
    }

    last_pack_num = program_cur_num;

#ifndef ERASEATSTART
    if(FLASH_PAGE_LEN - packet_use_len < cur_len||!flash_first_erase)
    {
        FLASH->KEYR = ((u32)0x45670123);
        FLASH->KEYR = ((u32)0xCDEF89AB);
        if(FLASH_ErasePage(TEMP_FLASH_ADDR+(pagecounter*FLASH_PAGE_LEN)) != \
            FLASH_COMPLETE)
        {
            update_init();
            return update_erase_flash;
        }
        pagecounter++;
        
        if(!flash_first_erase)
        {
            flash_first_erase = 1;
            packet_use_len += cur_len;
        }
        else
        {
            packet_use_len = (packet_use_len+cur_len) % FLASH_PAGE_LEN;
        }
    }
    else
    {
        packet_use_len += cur_len;
    }
#endif

    FLASH->KEYR =  ((u32)0x45670123);
    FLASH->KEYR = ((u32)0xCDEF89AB);
    addr2 = TEMP_FLASH_ADDR + program_rcv_len;

#if 0
    for (idx = 0; idx <  (cur_len/4); idx = idx + 1)
    {
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        } 
        FLASH->CR |= ((u32)0x00000001);
        *(__IO u16*)addr2 = (u16)p[idx];
        
        /* Wait for last operation to be completed */
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        }
        *(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;
        for(j = 0x1F; j != 0; j--)
        {
          for(i = 0xFF; i != 0; i--)
          {
            IWDG->KR = ((u16)0xAAAA);
          }
        }  
        /* Disable the PG Bit */
        addr2 += 4;
        FLASH->CR &= ((u32)0x00001FFE);
    }
#else
    for (idx = 0; idx <  (cur_len/4); idx = idx + 1)
    {
        //can be better 
        while(FLASH_COMPLETE == FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            FLASH->CR |= ((u32)0x00000001);
            *(__IO u16*)addr2 = (u16)p[idx];
            break;
        }
        
        while(FLASH_COMPLETE == FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            *(__IO u16*)(addr2 + 2) = (u32)p[idx]>>16;
            break;
        }
        
        addr2 += 4;
        while(FLASH_BUSY != FLASH_WaitForLastOperation(((u32)0x0000000F)))
        {
            FLASH->CR &= ((u32)0x00001FFE);
            break;
        }
        
    }
#endif
    program_rcv_len += cur_len;

    if(program_rcv_len >= program_total_len)
    {
//#ifdef UPDATEUSERAM
        update_flash_init();
		update_flash_copy_run(program_rcv_len);
//#else
/*
        u16 tmp;
        for(tmp = 0; tmp < 150; tmp++);
        __set_FAULTMASK(1);
		exec_iap_fun();
*/		
//#endif
    }

    return update_ok;
}


typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
void exec_iap_fun()
{
    JumpAddress = *(__IO uint32_t*) (PROG_IAP_ADDRESS + 4);
      /* Jump to user application */
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) PROG_IAP_ADDRESS);
      Jump_To_Application();
}


static u32 rcv_len               = 0;
static u32 programmlen           = 0;
static u8  updat_rcv_packet_flag = 0;
static u16 cur_pack_num          = 0;
static u8 err_flag = 1;

void ProgramUpdate(u8 *cmd_buf)
{
    u32 cur_packet_len = 0;
    static u16 update_lastpack = 0;
    tUpdateFrame *update_packet;
    //u32 vector_num;
    
    update_packet = (tUpdateFrame *)cmd_buf;
    if((update_packet->data.header.update_type== 1)||\
        (update_packet->data.header.update_type == 0x7d)) //更新程序
	{
	    if(update_packet->data.header.update_flag == 0x01)
        {
        	
            //开始升级的标志
            update_init();
            cur_pack_num    = 0;
            update_lastpack = 0;
            rcv_len         = 0;
            if(!updat_rcv_packet_flag)
            {
			LED_TRAIN_COM_OPEN();
			LED_VCOM_COM_OPEN();
			LED_LISTEN_TALK_OPEN();
                programmlen = \
                ((u32)(update_packet->data.header.highest_bye_total_len)<<24)|\
                ((u32)(update_packet->data.header.higher_bye_total_len)<<16)|\
                ((u32)(update_packet->data.header.lower_bye_total_len)<<8)|\
                ((u32)(update_packet->data.header.lowest_bye_total_len));

                //vector_num = (programmlen%FLASH_PAGE_LEN)?(1+((programmlen/FLASH_PAGE_LEN))):(programmlen/FLASH_PAGE_LEN);
                
                update_flash_erase();
            }
            updat_rcv_packet_flag = 1;
            return;
        }
        
        if(updat_rcv_packet_flag)
        {
            programmlen = \
                ((u32)(update_packet->data.header.highest_bye_total_len)<<24)|\
                ((u32)(update_packet->data.header.higher_bye_total_len)<<16)|\
                ((u32)(update_packet->data.header.lower_bye_total_len)<<8)|\
                ((u32)(update_packet->data.header.lowest_bye_total_len));

            cur_pack_num = \
                (((u16)(update_packet->data.header.high_byte_cur_packet_num)<<8)|\
                ((u16)(update_packet->data.header.low_byte_cur_packet_num)));

            if((cur_pack_num - update_lastpack) != 1)
            {
			LED_TRAIN_COM_OPEN();
			LED_VCOM_COM_OPEN();
			LED_LISTEN_TALK_OPEN();
                updat_rcv_packet_flag = 0;
                cur_pack_num = 0;
                return;
            }
		
            update_lastpack = cur_pack_num;
            cur_packet_len = update_packet->header.len - \
                sizeof(update_packet->data.header);
            rcv_len += cur_packet_len;
		LED_TRAIN_COM_OPEN();
		LED_VCOM_COM_OPEN();
		LED_LISTEN_TALK_OPEN();
            update_save_program(&(update_packet->data.data[0]),programmlen,\
                cur_pack_num,\
                cur_packet_len);
		LED_TRAIN_COM_CLOSE();
		LED_VCOM_COM_CLOSE();
		LED_LISTEN_TALK_CLOSE();
        }

        if(update_packet->data.header.update_flag == 0x03)
        {
            //结束升级的标志
            updat_rcv_packet_flag = 0;
            return;
        }
    }
}


extern u8 EhpDataSend( u8* buf, u8 length );


void UsbProgramUpdate(u8 *cmd_buf)
{
    u32 cur_packet_len = 0;
    static u32 total_len = 1024*1024*1;
    static u16 update_lastpack = 0;
    tUsbUpdatePacket *update_packet;  

    update_packet = (tUsbUpdatePacket *)cmd_buf;
   
	  if(update_packet->step == 0x01)
        {
            //开始升级的标志
			LED_TRAIN_COM_OPEN();
			LED_VCOM_COM_OPEN();
			LED_LISTEN_TALK_OPEN();
            update_init();
            cur_pack_num    = 0;
            update_lastpack = 0;
            rcv_len         = 0;
		err_flag = 1;
#if 0  // help xiao zhu  debug  usb update
buf[0] = 0xff;		
buf[1] = 0xff;		
buf[2] = 0xff;		
buf[3] = 0xff;		
EhpDataSend(buf, 4);		
#endif
            if(!updat_rcv_packet_flag)
            {
                update_flash_erase();
            }
            updat_rcv_packet_flag = 1;
            return;
        }
        
        else if(update_packet->step == 0x02)
        {
            cur_pack_num = \
                (((u16)(update_packet->packet_num_h)<<8)|\
                ((u16)(update_packet->packet_num_l)));
#if 0 // help xiao zhu  debug  usb update
buf[0] = update_lastpack  / 1000;
buf[1] = (update_lastpack  % 1000) / 100;
buf[2] = (update_lastpack  % 100) / 10;
buf[3] = update_lastpack  % 10;
EhpDataSend(buf, 4);
#endif
            if((cur_pack_num - update_lastpack) != 1)
            {
			LED_TRAIN_COM_OPEN();
			LED_VCOM_COM_OPEN();
			LED_LISTEN_TALK_OPEN();
	       	err_flag = 1;
                updat_rcv_packet_flag = 0;
                cur_pack_num = 0;
                return;
            }
		err_flag = 0;
            update_lastpack = cur_pack_num;
            cur_packet_len = update_packet->len-4;
            rcv_len += cur_packet_len;
		LED_TRAIN_COM_OPEN();
		LED_VCOM_COM_OPEN();
		LED_LISTEN_TALK_OPEN();
            update_save_program(&(update_packet->data_area[0]),total_len,\
                cur_pack_num,\
                cur_packet_len);
		LED_TRAIN_COM_CLOSE();
		LED_VCOM_COM_CLOSE();
		LED_LISTEN_TALK_CLOSE();
        }

        else if(update_packet->step == 0x03)
        {
		if ( 1 == err_flag )
		{
			return;
		}
            //结束升级的标志
            updat_rcv_packet_flag = 0;
	       update_flash_init();
		update_flash_copy_run(program_rcv_len);
            return;
        }
  
}



