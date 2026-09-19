#ifndef __DOLPHIN_OS_IP_SOCKET_H__
#define __DOLPHIN_OS_IP_SOCKET_H__

#include <dolphin/ip/IP.h>
#include <dolphin/ip/IPDns.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SO_MTU_MAX 1500

#define SO_GET_CONFIG_MTU(config) ((config)->mtu >= SO_MTU_MAX ? SO_MTU_MAX : (config)->mtu)

typedef struct SOInAddr {
    // total size: 0x4
    u32 addr; // offset 0x0, size 0x4
} SOInAddr;

typedef struct SOSockAddrIn {
    u8 len;
    u8 family;
    u16 port;
    SOInAddr addr;
} SOSockAddrIn;

typedef struct SOIpMreq {
    // total size: 0x8
    SOInAddr multiaddr; // offset 0x0, size 0x4
    SOInAddr interface; // offset 0x4, size 0x4
} SOIpMreq;

typedef struct SONode {
    // total size: 0x38
    u8 proto; // offset 0x0, size 0x1
    u8 flag; // offset 0x1, size 0x1
    s16 ref; // offset 0x2, size 0x2
    IPInfo* info; // offset 0x4, size 0x4
    OSMutex mutexRead; // offset 0x8, size 0x18
    OSMutex mutexWrite; // offset 0x20, size 0x18
} SONode;

typedef struct SOSockAddr {
    // total size: 0x8
    u8 len; // offset 0x0, size 0x1
    u8 family; // offset 0x1, size 0x1
    u8 data[6]; // offset 0x2, size 0x6
} SOSockAddr;

typedef struct SOHostEnt {
    // total size: 0x10
    char* name; // offset 0x0, size 0x4
    char** aliases; // offset 0x4, size 0x4
    s16 addrType; // offset 0x8, size 0x2
    s16 length; // offset 0xA, size 0x2
    u8** addrList; // offset 0xC, size 0x4
} SOHostEnt;

typedef struct SOResolver {
    // total size: 0x790
    DNSInfo info; // offset 0x0, size 0x560
    SOHostEnt ent; // offset 0x560, size 0x10
    char name[255]; // offset 0x570, size 0xFF
    char* zero; // offset 0x670, size 0x4
    u8 addrList[140]; // offset 0x674, size 0x8C
    u8* ptrList[36]; // offset 0x700, size 0x90
    u8 dns1[4]; // offset 0x790, size 0x4
    u8 dns2[4]; // offset 0x794, size 0x4
} SOResolver;

typedef void* (*SOAllocFunc)(u32, s32);
typedef void (*SOFreeFunc)(u32, void*, s32);

typedef struct SOConfig {
    // total size: 0x60
    u16 vendor; // offset 0x0, size 0x2
    u16 version; // offset 0x2, size 0x2
    SOAllocFunc alloc; // offset 0x4, size 0x4
    SOFreeFunc free; // offset 0x8, size 0x4
    u32 flag; // offset 0xC, size 0x4
    SOInAddr addr; // offset 0x10, size 0x4
    SOInAddr netmask; // offset 0x14, size 0x4
    SOInAddr router; // offset 0x18, size 0x4
    SOInAddr dns1; // offset 0x1C, size 0x4
    SOInAddr dns2; // offset 0x20, size 0x4
    s32 timeWaitBuffer; // offset 0x24, size 0x4
    s32 reassemblyBuffer; // offset 0x28, size 0x4
    s32 mtu; // offset 0x2C, size 0x4
    s32 rwin; // offset 0x30, size 0x4
    OSTime r2; // offset 0x38, size 0x8
    const char* peerid; // offset 0x40, size 0x4
    const char* passwd; // offset 0x44, size 0x4
    const char* serviceName; // offset 0x48, size 0x4
    const char* acName; // offset 0x4C, size 0x4
    s32 rdhcp; // offset 0x50, size 0x4
} SOConfig;

typedef struct SOLinger {
    // total size: 0x8
    int onoff; // offset 0x0, size 0x4
    int linger; // offset 0x4, size 0x4
} SOLinger;

typedef struct SOPollFD {
    // total size: 0x8
    int fd; // offset 0x0, size 0x4
    s16 events; // offset 0x4, size 0x2
    s16 revents; // offset 0x6, size 0x2
} SOPollFD;

s32 SOGetHostID();

#ifdef __cplusplus
}
#endif

#endif
