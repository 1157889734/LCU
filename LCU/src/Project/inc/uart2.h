#ifndef UART2_H
#define UART2_H



#define UART2_BUF_LEN 256


//flag
#define UART2_IDLE		  					0
#define UART2_START_CHECK  					1
#define UART2_CHECKING_NOW 					2
#define UART2_READ_STATUS					3
#define UART2_CHK_COMPLETE					4
#define UART2_CHK_WAITING					5




typedef struct {
	u8 status;			//正常: 00   开路: 01     短路:02
	u32 chk_1ms_tick;
	u8 error_cnt;
	u8 reply_flag;		// 1:已经回复， 0:没有回复
}tCommChkStatus;



extern u32 gUart2CommChkFlag;
extern u32 gUart2WaitFlag;
extern u32 gUart2WaitTick;
extern tCommChkStatus gSpkStatus[128];
extern u16 gCommBoardId;
extern u16 gUart2ChkIndex;
extern u8 gSpkStatusBuf[32];


extern u8 uart2_recv_buf[UART2_BUF_LEN];
extern u8 uart2_send_buf[UART2_BUF_LEN];
extern u16 uart2_recv_fifo_in;
extern u16 uart2_recv_fifo_out;

extern uint16_t uart2_send_cnt;
extern uint16_t uart2_send_index;


extern void uart2_config();
extern void uart2_write_char(u8 ch);
extern void uart2_write_buf(u8 *buf, u16 length);
extern u8 uart2_read_char(u8 *ch);
extern u8 uart2_busy();
extern void uart2_clr_busy();
extern void uart2_set_busy();


#endif
