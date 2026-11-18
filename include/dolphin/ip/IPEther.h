#ifndef __DOLPHIN_OS_IP_ETHER_H__
#define __DOLPHIN_OS_IP_ETHER_H__

#include <dolphin/ip/IP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ETHHeader {
    // total size: 0xE
    u8 dst[6]; // offset 0x0, size 0x6
    u8 src[6]; // offset 0x6, size 0x6
    u16 type; // offset 0xC, size 0x2
} ETHHeader;

BOOL IFMute(BOOL mute);
BOOL IFInit(s32 type /* r30 */);
void ETHOut(IPInterface* interface, IFDatagram* datagram);

#ifdef __cplusplus
}
#endif

#endif

