#ifndef _DOLPHIN_TCP_H_
#define _DOLPHIN_TCP_H_

#include <dolphin/types.h>

typedef struct TCPHeader {
    u16 src;
    u16 dst;
    s32 seq;
    s32 ack;
    u16 flag;
    u16 win;
    u16 sum;
    u16 urg;
} TCPHeader;

typedef struct TCPResetInfo {
    u8 addr[6];
    u8 dstAddr[4];
    u8 srcAddr[4];
    u16 dstPort;
    u16 srcPort;
    s32 seq;
    s32 ack;
    u16 flag;
    u16 id;
} TCPResetInfo;



#endif
