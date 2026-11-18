#ifndef __IP_PRIVATE_H__
#define __IP_PRIVATE_H__

#include <dolphin/ip.h>

#ifdef __cplusplus
extern "C" {
#endif

extern IPInterface __IFDefault;
extern SOResolver __SOResolver;
extern PPPConf PPPLcpConf;
extern PPPConf PPPIpcpConf;
extern const u8 IPLimited[4];
extern IFQueue TCPInfoQueue;

#ifdef __cplusplus
}
#endif

#endif
