#ifndef __DOLPHIN_OS_IP_H__
#define __DOLPHIN_OS_IP_H__

#include <dolphin/ip/IFQueue.h>
#include <dolphin/ip/IP.h>
#include <dolphin/ip/IPIgmp.h>
#include <dolphin/ip/IPIcmp.h>
#include <dolphin/ip/IPRoute.h>
#include <dolphin/ip/IPTcpTimeWait.h>
#include <dolphin/ip/IPOpt.h>
#include <dolphin/ip/IPFrag.h>
#include <dolphin/ip/IPTcp.h>
#include <dolphin/ip/IPUdp.h>
#include <dolphin/ip/IPSocket.h>
#include <dolphin/ip/IFFifo.h>
#include <dolphin/ip/IPDns.h>
#include <dolphin/ip/IPPpp.h>
#include <dolphin/ip/IPDhcp.h>
#include <dolphin/ip/IPEther.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPU32(ipBytes) (*(u32*)(ipBytes))
#define IPEQ(ip0, ip1) (IPU32(ip0) == IPU32(ip1))
#define IPNEQ(ip0, ip1) (IPU32(ip0) != IPU32(ip1))

#define IP_CLASSA(ip) (((ip)[0] & 0x80) == 0x00) /*   0 - 127 */
#define IP_CLASSB(ip) (((ip)[0] & 0xC0) == 0x80) /* 128 - 191 */
#define IP_CLASSC(ip) (((ip)[0] & 0xE0) == 0xC0) /* 192 - 223 */
#define IP_CLASSD(ip) (((ip)[0] & 0xF0) == 0xE0) /* 224 - 239 | Multicasting */
#define IP_CLASSE(ip) (((ip)[0] & 0xF8) == 0xF0) /* 240 - 255 | Experimental */

extern const u8 IPAddrAny[4];

char* IPAtoN(const char* dotted, u8* addr);
char* IPNtoA(const u8* addr);
BOOL IPRecoverGateway(const u8* dst);
BOOL IPAutoConfig(void);
s32 IPSetConfigError(IPInterface* interface, s32 err);

#ifdef __cplusplus
}
#endif

#endif
