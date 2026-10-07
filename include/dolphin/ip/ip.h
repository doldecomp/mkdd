#ifndef __DOLPHIN_OS_IP_IP_H__
#define __DOLPHIN_OS_IP_IP_H__

#include <dolphin/types.h>
#include <dolphin/os.h>
#include <dolphin/ip/IFQueue.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IP_SOCKLEN 8
#define IF_MAX_VEC 4

#define IP_HLEN(ip) (((ip)->verlen & 0xF) << 2)

#define IP_INET 2

// TODO: where does this go? IPEth? IPArp?
#define ETH_IP 0x0800

/* Supported IP protocols */
#define IP_PROTO_ICMP 0x01
#define IP_PROTO_IGMP 0x02
#define IP_PROTO_TCP  0x06
#define IP_PROTO_UDP  0x11

typedef struct IPSocket {
    // total size: 0x8
    u8 len; // offset 0x0, size 0x1
    u8 family; // offset 0x1, size 0x1
    u16 port; // offset 0x2, size 0x2
    u8 addr[4]; // offset 0x4, size 0x4
} IPSocket;

typedef struct IPInfo {
    // total size: 0x20
    u8 proto; // offset 0x0, size 0x1
    u8 ttl; // offset 0x1, size 0x1
    u8 tos; // offset 0x2, size 0x1
    u8 mttl; // offset 0x3, size 0x1
    s16 poll; // offset 0x4, size 0x2
    u16 flag; // offset 0x6, size 0x2
    IPSocket local; // offset 0x8, size 0x8
    IPSocket remote; // offset 0x10, size 0x8
    IFLink link; // offset 0x18, size 0x8
} IPInfo;

typedef struct IFVec {
    // total size: 0x8
    void* data; // offset 0x0, size 0x4
    s32 len; // offset 0x4, size 0x4
} IFVec;

typedef struct IPInterface IPInterface;

typedef struct IFDatagram {
    // total size: 0x3C
    IPInterface* interface; // offset 0x0, size 0x4
    IFQueue* queue; // offset 0x4, size 0x4
    IFLink link; // offset 0x8, size 0x8
    u16 type; // offset 0x10, size 0x2
    u8 hwAddr[6]; // offset 0x12, size 0x6
    u8 dst[4]; // offset 0x18, size 0x4
    u8 prefix[8]; // offset 0x1c, size 0x8
    u8 prefixLen; // offset 0x24, size 0x1
    u8 flag; // offset 0x25, size 0x1
    void (*callback)(void*, s32); // offset 0x28, size 0x4
    void* param; // offset 0x2C, size 0x4
    s32 nVec; // offset 0x30, size 0x4
    IFVec vec[1]; // offset 0x34, size 0x8
} IFDatagram;

typedef struct IPInterfaceConf {
    // total size: 0x40
    IPInterface* interface; // offset 0x0, size 0x4
    IFQueue link; // offset 0x4, size 0x8
    u8 addr[4]; // offset 0xC, size 0x4
    OSAlarm alarm; // offset 0x10, size 0x28
    s32 count; // offset 0x38, size 0x4
    void (* callback)(void *, long); // offset 0x3C, size 0x4
} IPInterfaceConf;

typedef struct IPInterfaceStat {
    // total size: 0x24
    u32 inUcastPackets; // offset 0x0, size 0x4
    u32 inNonUcastPackets; // offset 0x4, size 0x4
    u32 inDiscards; // offset 0x8, size 0x4
    u32 inErrors; // offset 0xC, size 0x4
    u32 outUcastPackets; // offset 0x10, size 0x4
    u32 outNonUcastPackets; // offset 0x14, size 0x4
    u32 outDiscards; // offset 0x18, size 0x4
    u32 outErrors; // offset 0x1C, size 0x4
    u32 outCollisions; // offset 0x20, size 0x4
} IPInterfaceStat;

struct IPInterface {
    // total size: 0xA8
    s32 type; // offset 0x0, size 0x4
    BOOL up; // offset 0x4, size 0x4
    s32 err; // offset 0x8, size 0x4
    OSAlarm gratuitousAlarm; // offset 0x10, size 0x28
    u8 mac[6]; // offset 0x38, size 0x6
    s32 mtu; // offset 0x40, size 0x4
    u8 addr[4]; // offset 0x44, size 0x4
    u8 netmask[4]; // offset 0x48, size 0x4
    u8 broadcast[4]; // offset 0x4C, size 0x4
    u8 gateway[4]; // offset 0x50, size 0x4
    u8 alias[4]; // offset 0x54, size 0x4
    IFQueue ppp; // offset 0x58, size 0x8
    void (*out)(IPInterface*, IFDatagram*); // offset 0x60, size 0x4
    void (*cancel)(IPInterface*, IFDatagram*); // offset 0x48, size 0x4
    void* (*alloc)(IPInterface*, s32); // offset 0x68, size 0x4
    BOOL (*free)(IPInterface*, void*, s32); // offset 0x6c, size 0x4
    BOOL (*inFilter)(IPInterface*, void*, s32); // offset 0x70, size 0x4
    BOOL (*outFilter)(IPInterface*, void*, s32); // offset 0x74, size 0x4
    IFQueue queue; // offset 0x78, size 0x8
    IPInterfaceStat stat; // offset 0x80, size 0x24
};

typedef struct IPHeader {
    // total size: 0x14
    u8 verlen; // offset 0x0, size 0x1
    u8 tos; // offset 0x1, size 0x1
    u16 len; // offset 0x2, size 0x2
    u16 id; // offset 0x4, size 0x2
    u16 frag; // offset 0x6, size 0x2
    u8 ttl; // offset 0x8, size 0x1
    u8 proto; // offset 0x9, size 0x1
    u16 sum; // offset 0xA, size 0x2
    u8 src[4]; // offset 0xC, size 0x4
    u8 dst[4]; // offset 0x10, size 0x4
} IPHeader;

char* IPNtoA(const u8* addr);
void IFInitDatagram(IFDatagram* datagram, u16 type, int nVec);

#ifdef __cplusplus
}
#endif

#endif
