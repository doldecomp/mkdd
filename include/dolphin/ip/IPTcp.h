#ifndef __DOLPHIN_OS_IP_TCP_H__
#define __DOLPHIN_OS_IP_TCP_H__

#include <dolphin/ip/IP.h>
#include <dolphin/ip/IFFifo.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TCP_STATE_LISTEN 1

#define TCP_FLAG_FIN (1 << 0)
#define TCP_FLAG_SYN (1 << 1)
#define TCP_FLAG_RST (1 << 2)
#define TCP_FLAG_PSH (1 << 3)
#define TCP_FLAG_ACK (1 << 4)
#define TCP_FLAG_URG (1 << 5)

typedef struct TCPHeader {
    // total size: 0x14
    u16 src; // offset 0x0, size 0x2
    u16 dst; // offset 0x2, size 0x2
    s32 seq; // offset 0x4, size 0x4
    s32 ack; // offset 0x8, size 0x4
    u16 flag; // offset 0xC, size 0x2
    u16 win; // offset 0xE, size 0x2
    u16 sum; // offset 0x10, size 0x2
    u16 urg; // offset 0x12, size 0x2
} TCPHeader;

typedef struct TCPSackHole {
    // total size: 0x10
    s32 start; // offset 0x0, size 0x4
    s32 end; // offset 0x4, size 0x4
    int dupAcks; // offset 0x8, size 0x4
    s32 rxmit; // offset 0xC, size 0x4
} TCPSackHole;

typedef struct TCPInfo TCPInfo;
typedef void (*TCPCallback)(TCPInfo*, s32);

struct TCPInfo {
    // total size: 0x330
    IPInfo pair; // offset 0x0, size 0x20
    OSThreadQueue queueThread; // offset 0x20, size 0x8
    IPInterface* interface; // offset 0x28, size 0x4
    s32 err; // offset 0x2C, size 0x4
    s32 sendUna; // offset 0x30, size 0x4
    s32 sendNext; // offset 0x34, size 0x4
    s32 sendWin; // offset 0x38, size 0x4
    s32 sendUp; // offset 0x3C, size 0x4
    s32 sendWL1; // offset 0x40, size 0x4
    s32 sendWL2; // offset 0x44, size 0x4
    s32 iss; // offset 0x48, size 0x4
    s32 sendMaxWin; // offset 0x4C, size 0x4
    s32 sendMax; // offset 0x50, size 0x4
    s32 recvNext; // offset 0x54, size 0x4
    s32 recvWin; // offset 0x58, size 0x4
    s32 recvUp; // offset 0x5C, size 0x4
    s32 irs; // offset 0x60, size 0x4
    s32 segLen; // offset 0x64, size 0x4
    u8* segBegin; // offset 0x68, size 0x4
    IFBlock asb[4]; // offset 0x6C, size 0x20
    TCPSackHole scoreboard[4]; // offset 0x8C, size 0x40
    int sendHoles; // offset 0xCC, size 0x4
    s32 sendFack; // offset 0xD0, size 0x4
    s32 sendAwin; // offset 0xD4, size 0x4
    s32 rxmitData; // offset 0xD8, size 0x4
    s32 sendRecover; // offset 0xDC, size 0x4
    s32 lastSack; // offset 0xE0, size 0x4
    s32 state; // offset 0xE4, size 0x4
    u32 flag; // offset 0xE8, size 0x4
    TCPCallback closeCallback; // offset 0xEC, size 0x4
    s32* closeResult; // offset 0xF0, size 0x4
    s32 mss; // offset 0xF4, size 0x4
    volatile s32 sendBusy; // offset 0xF8, size 0x4
    u8 header[120]; // offset 0xFC, size 0x78
    u8* sendData; // offset 0x174, size 0x4
    s32 sendBuff; // offset 0x178, size 0x4
    u8* sendPtr; // offset 0x17C, size 0x4
    s32 sendLen; // offset 0x180, size 0x4
    IFDatagram datagram; // offset 0x184, size 0x3C
    IFVec vec[3]; // offset 0x1C0, size 0x18
    TCPCallback sendCallback; // offset 0x1D8, size 0x4
    s32* sendResult; // offset 0x1DC, size 0x4
    s32 userAcked; // offset 0x1E0, size 0x4
    u8* userSendData; // offset 0x1E4, size 0x4
    s32 userSendLen; // offset 0x1E8, size 0x4
    OSTime lastSend; // offset 0x1F0, size 0x8
    u8* recvData; // offset 0x1F8, size 0x4
    s32 recvBuff; // offset 0x1FC, size 0x4
    s32 recvUser; // offset 0x200, size 0x4
    u8* recvPtr; // offset 0x204, size 0x4
    s32 recvAcked; // offset 0x208, size 0x4
    s32 dupAcks; // offset 0x20C, size 0x4
    TCPCallback recvCallback; // offset 0x210, size 0x4
    s32* recvResult; // offset 0x214, size 0x4
    u8* userData; // offset 0x218, size 0x4
    s32 userBuff; // offset 0x21C, size 0x4
    s32 userLen; // offset 0x220, size 0x4
    u8 oob; // offset 0x224, size 0x1
    s32 recvUrg; // offset 0x228, size 0x4
    TCPCallback urgCallback; // offset 0x22C, size 0x4
    s32* urgResult; // offset 0x230, size 0x4
    u8* urgData; // offset 0x234, size 0x4
    s32 rxmitCount; // offset 0x238, size 0x4
    OSTime rto; // offset 0x240, size 0x8
    OSTime r0; // offset 0x248, size 0x8
    OSTime r2; // offset 0x250, size 0x8
    OSAlarm rxmitAlarm; // offset 0x258, size 0x28
    s32 cWin; // offset 0x280, size 0x4
    s32 ssThresh; // offset 0x284, size 0x4
    BOOL rttTiming; // offset 0x288, size 0x4
    s32 rttSeq; // offset 0x28C, size 0x4
    OSTime rtt; // offset 0x290, size 0x8
    OSTime srtt; // offset 0x298, size 0x8
    OSTime rttDe; // offset 0x2A0, size 0x8
    OSTime rttMin; // offset 0x2A8, size 0x8
    OSTime rttMax; // offset 0x2B0, size 0x8
    TCPInfo* listening; // offset 0x2B8, size 0x4
    IPSocket* local; // offset 0x2BC, size 0x4
    IPSocket* remote; // offset 0x2C0, size 0x4
    IFQueue queueListen; // offset 0x2C4, size 0x8
    IFLink linkListen; // offset 0x2CC, size 0x8
    TCPCallback openCallback; // offset 0x2D4, size 0x4
    s32* openResult; // offset 0x2D8, size 0x4
    int linger; // offset 0x2DC, size 0x4
    OSAlarm lingerAlarm; // offset 0x2E0, size 0x28
    TCPInfo* logging; // offset 0x308, size 0x4
    IFQueue queueBacklog; // offset 0x30C, size 0x8
    IFQueue queueCompleted; // offset 0x314, size 0x8
    IFLink linkLog; // offset 0x31C, size 0x8
    s32 accepting; // offset 0x324, size 0x4
    void* node; // offset 0x328, size 0x4
};

u16 TCPCheckSum(IFVec* vec, s32 nVec);
void TCPIn(IPInterface * interface /* r28 */, IPHeader * ip /* r29 */, u32 flag);

#ifdef __cplusplus
}
#endif

#endif
