#ifndef __DOLPHIN_OS_IP_TCPTIMEWAIT_H__
#define __DOLPHIN_OS_IP_TCPTIMEWAIT_H__

#include <dolphin/ip.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL TCPLookupTimeWaitInfo(const u8* src, u16 srcPort, const u8* dst, u16 dstPort);

#ifdef __cplusplus
}
#endif

#endif
