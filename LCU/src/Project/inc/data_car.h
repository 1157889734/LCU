#ifndef DATA_CAR_H_
#define DATA_CAR_H_

#include "data_public.h"



extern void CarDataProc(void);
extern void CarDataStartToSend(tStationStat stationStat);
extern void CarDataInit();
extern u8 CarDataSend( u8* buf, u8 length );
extern void MapInit(void);
extern void CarDataClearDmp();
extern void Vau_Proc(void);
extern u8 Vau_get_panel_volume(void);
extern u8 Vau_get_panel_mode(void);
extern void CarTokenSend();
extern void Vau_set_panel_volume(u8 volume);
extern void Vau_set_panel_mode(u8 volume);
extern void Vau_send(void);

#endif


