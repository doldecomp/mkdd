#ifndef __DOLPHIN_OS_IP_ARP_H__
#define __DOLPHIN_OS_IP_ARP_H__

#include <dolphin/ip/IP.h>
#include <dolphin/ip/IPEther.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARP_FOUND 0
#define ARP_LOOPBACK 1
#define ARP_BROADCAST 2
#define ARP_MULTICAST 3
#define ARP_NOTFOUND -1

#define ARP_CACHE_RESOVLED 2
#define ARP_CACHE_POLLING 3

typedef struct ARPCache {
    // total size: 0xA8
    IFQueue link; // offset 0x0, size 0x8
    OSAlarm alarm; // offset 0x8, size 0x28
    int rxmit; // offset 0x30, size 0x4
    int state; // offset 0x34, size 0x4
    u8 hwAddr[6]; // offset 0x38, size 0x6
    u8 prAddr[4]; // offset 0x3E, size 0x4
    IPInterface* interface; // offset 0x44, size 0x4
    IFDatagram datagram; // offset 0x48, size 0x3C
    u8 arp[28]; // offset 0x84, size 0x1C
    IFQueue queue; // offset 0xA0, size 0x8
} ARPCache;

typedef struct ARPHeader {
    // total size: 0x8
    u16 hwType; // offset 0x0, size 0x2
    u16 prType; // offset 0x2, size 0x2
    u8 hwAddrLen; // offset 0x4, size 0x1
    u8 prAddrLen; // offset 0x5, size 0x1
    u16 opCode; // offset 0x6, size 0x2
} ARPHeader;

#define ARPHeader2MACAddr(header) ((u8*)(header) + sizeof(ARPHeader))
#define ARPHeader2PrAddr(header) ((u8*)(header) + sizeof(ARPHeader) + (header)->hwAddrLen)
#define ArpHeader2HwAddr(header) ((u8*)(header) + sizeof(ARPHeader) + header->hwAddrLen + header->prAddrLen)
#define ARPHeader2Addr(header) ((u8*)(header) + sizeof(ARPHeader) + 2 * (header)->hwAddrLen + (header)->prAddrLen)

void ARPDumpPacket(const ETHHeader* eh, s32 len);
void ARPDump(void);
void ARPInit(void);
s32 ARPLookup(IPInterface* interface, u8* prAddr, u8* hwAddr);
void ARPRevalidate(u8* prAddr);
void ARPAdd(IPInterface* interface, u8* prAddr, u8* hwAddr);
void ARPHold(IPInterface* interface, struct IFDatagram * datagram);
void ARPOut(IPInterface* interface, u16 opCode, const u8* dstPrAddr, const u8* dstHwAddr, const u8* srcPrAddr, ARPCache* cache);
void ARPGratuitous(IPInterface* interface);
void ARPProbe(IPInterface* interface, u8* prAddr);
void ARPClaim(IPInterface* interface, IPInterfaceConf* conf);
void ARPIn(IPInterface* interface, ETHHeader* eh, s32 len);
void ARPRefresh(void);

#ifdef __cplusplus
}
#endif

#endif

