#ifndef __DOLPHIN_OS_IP_FRAG_H__
#define __DOLPHIN_OS_IP_FRAG_H__

#include <dolphin/ip/IP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IP_FRAG_BITS 0x1FFF
#define IP_HAS_FRAG 0x2000
#define IP_DONT_FRAG 0x4000

#define IP_FRAG(ip) (((ip)->frag & IP_FRAG_BITS) << 3)

IPHeader* IPReassemble(IPInterface* interface, IPHeader* frag, u32 flag);

#ifdef __cplusplus
}
#endif

#endif
