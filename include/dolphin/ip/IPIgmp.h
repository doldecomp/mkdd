#ifndef __DOLPHIN_OS_IP_IGMP_H__
#define __DOLPHIN_OS_IP_IGMP_H__

#include <dolphin/ip/IP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IGMP {
    // total size: 0x8
    u8 vertype; // offset 0x0, size 0x1
    u8 unused; // offset 0x1, size 0x1
    u16 sum; // offset 0x2, size 0x2
    u8 addr[4]; // offset 0x4, size 0x4
} IGMP;

s32 IPMulticastLookup(const u8* groupAddr, const u8* interface);
s32 IPMulticastJoin(const u8* groupAddr, const u8* interface);
s32 IPMulticastLeave(const u8* groupAddr, const u8* interface);

u16 IGMPCheckSum(IGMP* igmp);
void IGMPIn(IPInterface * interface /* r28 */, IPHeader * ip /* r30 */, u32 flag);

#ifdef __cplusplus
}
#endif

#endif
