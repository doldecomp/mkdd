#ifndef __DOLPHIN_OS_IP_DNS_H__
#define __DOLPHIN_OS_IP_DNS_H__

#include <dolphin/ip/IP.h>
#include <dolphin/ip/IPUdp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DNSCommand DNSCommand;
typedef struct DNSInfo DNSInfo;

typedef s32 (*DNSPreCallback)(DNSInfo*, DNSCommand*);
typedef void (*DNSPostCallback)(DNSCommand*, s32);
typedef void (*DNSCallback)(DNSInfo*, s32);

struct DNSCommand {
    // total size: 0x3C
    DNSInfo* info; // offset 0x0, size 0x4
    IFLink link; // offset 0x4, size 0x8
    int func; // offset 0xC, size 0x4
    DNSPreCallback precallback; // offset 0x10, size 0x4
    DNSPostCallback callback; // offset 0x14, size 0x4
    s32* result; // offset 0x18, size 0x4
    union {
        struct {
            // total size: 0x10
            const char* name; // offset 0x0, size 0x4
            u8* addr; // offset 0x4, size 0x4
            s32 addrLen; // offset 0x8, size 0x4
            DNSCallback callback; // offset 0xC, size 0x4
        } ga; // offset 0x0, size 0x10
        struct {
            // total size: 0xC
            const u8* addr; // offset 0x0, size 0x4
            char* name; // offset 0x4, size 0x4
            DNSCallback callback; // offset 0x8, size 0x4
        } gn; // offset 0x0, size 0xC
        struct {
            // total size: 0x14
            const u8* query; // offset 0x0, size 0x4
            s32 queryLen; // offset 0x4, size 0x4
            u8* response; // offset 0x8, size 0x4
            s32 responseLen; // offset 0xC, size 0x4
            DNSCallback callback; // offset 0x10, size 0x4
        } lu; // offset 0x0, size 0x14
        struct {
            // total size: 0x20
            const char* nodeName; // offset 0x0, size 0x4
            const char* servName; // offset 0x4, size 0x4
            u16 type; // offset 0x8, size 0x2
            int sockType; // offset 0xC, size 0x4
            u16 port; // offset 0x10, size 0x2
            u8* addrList; // offset 0x14, size 0x4
            char* cname; // offset 0x18, size 0x4
            DNSCallback callback; // offset 0x1C, size 0x4
        } ai; // offset 0x0, size 0x20
        struct {
            // total size: 0xC
            void* sa; // offset 0x0, size 0x4
            char* node; // offset 0x4, size 0x4
            unsigned int nodeLen; // offset 0x8, size 0x4
        } ni; // offset 0x0, size 0xC
    } data; // offset 0x1C, size 0x20
};

struct DNSInfo {
    // total size: 0x560
    UDPInfo udp; // offset 0x0, size 0xF0
    IPSocket socket; // offset 0xF0, size 0x8
    DNSCallback callback; // offset 0xF8, size 0x4
    OSTime rxmit; // offset 0x100, size 0x8
    OSAlarm alarm; // offset 0x108, size 0x28
    u32 flag; // offset 0x130, size 0x4
    u8 query[512]; // offset 0x134, size 0x200
    s32 queryLen; // offset 0x334, size 0x4
    u8 response[512]; // offset 0x338, size 0x200
    s32 responseLen; // offset 0x538, size 0x4
    volatile s32 * result; // offset 0x53C, size 0x4
    u16 id; // offset 0x540, size 0x2
    int func; // offset 0x544, size 0x4
    u8* data; // offset 0x548, size 0x4
    s32 datalen; // offset 0x54C, size 0x4
    int ref; // offset 0x550, size 0x4
    OSThreadQueue queueThread; // offset 0x54C, size 0x8
};

s32 DNSClose(DNSInfo * info /* r31 */);
s32 DNSOpen(DNSInfo * info /* r1+0x8 */, const unsigned char * addr /* r1+0xC */);
s32 DNSOpen2(DNSInfo * info /* r31 */, const unsigned char * dns1 /* r27 */, const unsigned char * dns2 /* r28 */);

#ifdef __cplusplus
}
#endif

#endif
