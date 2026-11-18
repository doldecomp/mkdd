#ifndef __DOLPHIN_OS_IP_PPP_H__
#define __DOLPHIN_OS_IP_PPP_H__

#include <dolphin/ip/IP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LCPHeader {
    // total size: 0x4
    u8 code; // offset 0x0, size 0x1
    u8 id; // offset 0x1, size 0x1
    u16 len; // offset 0x2, size 0x2
} LCPHeader;

typedef struct PPPConf PPPConf;
typedef void (*PPPConfCallback)(PPPConf*);
typedef int (*PPPConfRecvCallback)(PPPConf*, LCPHeader*);
typedef int (*PPPConfTransCallback)(PPPConf*);

struct PPPConf {
    // total size: 0xA0
    u16 protocol; // offset 0x0, size 0x2
    int state; // offset 0x4, size 0x4
    u8 id; // offset 0x8, size 0x1
    u8 idTerminate; // offset 0x9, size 0x1
    u8 idReject; // offset 0xA, size 0x1
    u8 idEcho; // offset 0xB, size 0x1
    int rxmit; // offset 0xC, size 0x4
    int configure; // offset 0x10, size 0x4
    int terminate; // offset 0x14, size 0x4
    int failure; // offset 0x18, size 0x4
    u8 data[20]; // offset 0x1C, size 0x14
    u16 len; // offset 0x30, size 0x2
    OSAlarm alarm; // offset 0x38, size 0x28
    IPInterface* interface; // offset 0x60, size 0x4
    PPPConfRecvCallback receiveConfigureRequest; // offset 0x64, size 0x4
    PPPConfRecvCallback receiveConfigureAck; // offset 0x68, size 0x4
    PPPConfRecvCallback receiveConfigureNak; // offset 0x6C, size 0x4
    PPPConfRecvCallback receiveConfigureReject; // offset 0x70, size 0x4
    PPPConfTransCallback up; // offset 0x74, size 0x4
    PPPConfTransCallback down; // offset 0x78, size 0x4
    PPPConfTransCallback started; // offset 0x7C, size 0x4
    PPPConfTransCallback finished; // offset 0x80, size 0x4
    PPPConfCallback callback; // offset 0x84, size 0x4
    IFLink link; // offset 0x88, size 0x8
    u16 mru; // offset 0x90, size 0x2
    u16 auth; // offset 0x92, size 0x2
    u32 magic; // offset 0x94, size 0x4
    u32 remote; // offset 0x98, size 0x4
};

#ifdef __cplusplus
}
#endif

#endif
