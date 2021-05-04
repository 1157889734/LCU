#ifndef SYSTICK_H
#define SYSTICK_H

extern __IO u32 gSystemTick;

extern void mdelay(__IO uint32_t nTime);
extern void delay(__IO uint32_t nTime);
extern void TimingDelay_Decrement(void);
extern void SysTick_Configuration(void);
extern u8 check_timer(uint32_t *timer, uint32_t interval);
extern uint32_t get_clock(void);
#endif