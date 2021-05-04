
#include "include.h"

static __IO uint32_t TimingDelay;   //延迟时间
static __IO uint32_t time_clock;      //系统时钟


__IO u32 gSystemTick;

void SysTick_Configuration(void)
{
  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemFrequency / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
  
 /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

// milliseconds
void mdelay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

//seconds
void delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime * 1000;

  while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
  time_clock++;
}

u8 check_timer(uint32_t *timer, uint32_t interval)
{
	if(interval)
	{
		//判断溢出
		if(time_clock<(*timer))
			*timer=0;
		//比较是否时间到
		if(time_clock-(*timer)>=interval)
		{
			*timer=time_clock;
			return 1;
		}
	}
	return 0;
}

uint32_t get_clock(void)
{
	return time_clock;
}