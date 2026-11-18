#ifndef __DOLPHIN_OS_IP_ROUTE_H__
#define __DOLPHIN_OS_IP_ROUTE_H__

#include <dolphin/ip/IP.h>

#ifdef __cplusplus
extern "C" {
#endif

IPInterface* IPGetRoute(const u8* addr, u8* dst);
BOOL IPIsBroadcastAddr(IPInterface* interface, const u8* addr);
BOOL IPIsLoopbackAddr(IPInterface* interface, const u8* addr);
void IPSetMtu(IPInterface * interface /* r31 */, s32 mtu /* r30 */);

#ifdef __cplusplus
}
#endif

#endif
