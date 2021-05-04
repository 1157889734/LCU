#ifndef DATA_DEVICE_INFO_H_
#define DATA_DEVICE_INFO_H_

#include "include.h"

#define DEVICE_SOFTWARE_VERSION_HIGHT			1
#define DEVICE_SOFTWARE_VERSION_LOW				1

extern u8 GetDeviceId( );
extern void GetSoftwareVersion(u8 *vh, u8 *vl);
extern void PrintDeviceInfo();
extern void SetDeviceId();

#endif


