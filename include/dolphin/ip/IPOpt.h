#ifndef __DOLPHIN_OS_IP_OPT_H__
#define __DOLPHIN_OS_IP_OPT_H__

#include <dolphin/ip/IP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IP_OPT_TOS 7
#define IP_OPT_TTL 8
#define IP_OPT_9 9
#define IP_OPT_MTTL 10
#define IP_OPT_JOIN_MCAST 11
#define IP_OPT_LEAVE_MCAST 12

s32 IPProcessSourceRoute(IPHeader* ip);

#ifdef __cplusplus
}
#endif

#endif
