#ifndef __DOLPHIN_OS_IP_ICMP_H__
#define __DOLPHIN_OS_IP_ICMP_H__

#include <dolphin/ip/IP.h>

#ifdef __cplusplus
extern "C" {
#endif


void ICMPIn(IPInterface * interface /* r27 */, IPHeader * ip /* r31 */, u32 flag /* r26 */);

#ifdef __cplusplus
}
#endif

#endif
