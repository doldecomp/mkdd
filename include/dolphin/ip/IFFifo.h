#ifndef __DOLPHIN_OS_IP_IFFIFO_H__
#define __DOLPHIN_OS_IP_IFFIFO_H__

#include <dolphin/ip/IP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IFBlock {
    // total size: 0x8
    u8* ptr; // offset 0x0, size 0x4
    s32 len; // offset 0x4, size 0x4
} IFBlock;

typedef struct IFFifo {
    // total size: 0x10
    u8* buff; // offset 0x0, size 0x4
    s32 size; // offset 0x4, size 0x4
    u8* head; // offset 0x8, size 0x4
    s32 used; // offset 0xC, size 0x4
} IFFifo;

void IFFifoInit(IFFifo* fifo, void* buff, s32 size);
void * IFFifoAlloc(IFFifo* fifo, s32 len);
BOOL IFFifoFree(IFFifo* fifo, void* ptr, s32 len);
void IFDump(void* ptr, s32 len);

u8* IFRingIn(u8* buf, s32 size, u8* head, s32 used, const u8* data, s32 len);
u8* IFRingOut(u8* buf, s32 size, u8* head, s32 used, u8* data, s32 len);
int IFRingGet(u8* buf, s32 size, u8* head, s32 used, IFVec* vec, s32 len);
u8* IFRingPut(u8* buf, s32 size, u8* head, s32 used, s32 len);
u8* IFRingInEx(u8* buf, s32 size, u8* head, s32 used, s32 offset, const u8* data, s32 * adv, IFBlock* blockTable, s32 maxblock);

#ifdef __cplusplus
}
#endif

#endif
