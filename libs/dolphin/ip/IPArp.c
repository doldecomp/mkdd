#include "dolphin/os.h"
#include <dolphin/ip/IPArp.h>
#include <dolphin/private/ip.h>
#include <string.h>
#include <cstddef.h>

#ifdef NULL
#undef NULL
#endif

#define NULL 0
#define ARP_CACHE_SIZE 64

static ARPCache Cache[ARP_CACHE_SIZE]; // size: 0x2A00, address: 0x0
static IFQueue Up; // size: 0x8, address: 0x0
static IFQueue Free; // size: 0x8, address: 0x8
static u8 HwBroadcastAddr[6] = { 255, 255, 255, 255, 255, 255 }; // size: 0x6, address: 0x0
static OSAlarm GratuitousAlarm; // size: 0x28, address: 0x2A00

static void ARPCancel(ARPCache* cache);

// Range: 0x0 -> 0xBC
static char* ARPNtoA(const u8* addr /* r1+0x8 */, s32 len /* r31 */) {
    // Local variables
    static char ascii[18];
    int i; // r30
    char* p; // r29

    ASSERT(1 <= len);
    len = len < 6 ? len : 6;

    for (i = 0, p = ascii; i < len; i++) {
        p += sprintf(p, "%02x", addr[i]);
        if (i < len - 1) {
            p += sprintf(p, ":");
        }
    }

    return ascii;
}

// Range: 0xBC -> 0x2C4
void ARPDumpPacket(const ETHHeader* eh /* r29 */, s32 len /* r28 */) {
    // Local variables
    ARPHeader* arp; // r31

    arp = (ARPHeader*)(eh+1);
    OSReport("%s ", ARPNtoA(eh->src, sizeof(eh->src)));
    OSReport("%s ", ARPNtoA(eh->dst, sizeof(eh->dst)));

    switch (eh->type) {
        case 0x0806:
            OSReport("arp %d:\n", len);
            break;
        case 0x8035:
            OSReport("rarp %d:\n", len);
            break;
    }

    switch (arp->opCode) {
        case 1:
            OSReport("arp who-has %s ", IPNtoA((u8*)arp + arp->hwAddrLen * sizeof(u16) + arp->prAddrLen + sizeof(ARPHeader)));
            OSReport("tell %s\n", IPNtoA((u8*)arp + arp->hwAddrLen + sizeof(ARPHeader)));
            break;
        case 2:
            OSReport("arp reply %s ", IPNtoA((u8*)arp + arp->hwAddrLen + sizeof(ARPHeader)));
            OSReport("is-at %s\n", ARPNtoA((u8*)arp + sizeof(ARPHeader), arp->hwAddrLen));
            break;
        case 3:
            OSReport("rarp who-is %s ", ARPNtoA((u8*)arp + sizeof(ARPHeader), arp->hwAddrLen));
            OSReport("tell %s\n", ARPNtoA((u8*)arp + sizeof(ARPHeader), arp->hwAddrLen));
            break;
        case 4:
            OSReport("rarp reply %s ", ARPNtoA((u8*)arp + arp->hwAddrLen + arp->prAddrLen + sizeof(ARPHeader), arp->hwAddrLen));
            OSReport("at %s\n", IPNtoA((u8*)arp + arp->hwAddrLen * sizeof(u16) + arp->prAddrLen + sizeof(ARPHeader)));
            break;
    }
}

// Range: 0x2C4 -> 0x47C
void ARPDump(void) {
    // Local variables
    ARPCache* ent; // r31
    ARPCache* next; // r29

    // References
    // -> static struct IFQueue Up;
    // -> struct IPInterface __IFDefault;
    // -> unsigned char IPAddrAny[4];

    if (IPNEQ(__IFDefault.addr, IPAddrAny)) {
        OSReport("Internet Address: %d.%d.%d.%d\n", __IFDefault.addr[0], __IFDefault.addr[1], __IFDefault.addr[2], __IFDefault.addr[3]);
    }

    if (IPNEQ(__IFDefault.alias, IPAddrAny)) {
        OSReport("Alias Address: %d.%d.%d.%d\n", __IFDefault.alias[0], __IFDefault.alias[1], __IFDefault.alias[2], __IFDefault.alias[3]);
    }

    OSReport("Hardware Address: %02x:%02x:%02x:%02x:%02x:%02x\n", __IFDefault.mac[0], __IFDefault.mac[1], __IFDefault.mac[2], __IFDefault.mac[3], __IFDefault.mac[4], __IFDefault.mac[5]);

    OSReport("  Internet Address    Hardware Address\n");
    IFQueueIterator(ARPCache*, &Up, ent, next) {
        OSReport("  %3d.%3d.%3d.%3d     %02x:%02x:%02x:%02x:%02x:%02x\n", ent->prAddr[0], ent->prAddr[1], ent->prAddr[2], ent->prAddr[3], ent->hwAddr[0], ent->hwAddr[1], ent->hwAddr[2], ent->hwAddr[3], ent->hwAddr[4], ent->hwAddr[5]);
    }

    OSReport("\n");
}

// Range: 0x47C -> 0x560
static void DiscardPendingPackets(ARPCache* cache /* r29 */, s32 result /* r1+0xC */) {
    // Local variables
    IPInterface* interface; // r28
    IFDatagram* datagram; // r31

    interface = cache->interface;
    while (cache->queue.next) {
        IFQueueDequeueHead(IFDatagram*, &cache->queue, datagram);
        ASSERT(datagram->queue == &cache->queue);
        ASSERT(datagram->interface == interface);
        datagram->interface = NULL;
        datagram->queue = NULL;
        if (datagram->callback) {
            datagram->callback(datagram->param, result);
        }
    }

    (void)0;
}

// Range: 0x560 -> 0x6B8
static void Revalidate(ARPCache* cache /* r31 */) {
    // Local variables
    IPInterface* interface; // r27
    // struct IFQueue * ___next; // r30
    // struct IFQueue * ___prev; // r29
    // struct IFQueue * ___next; // r28

    switch (cache->state) {
        case 0:
        default:
            break;
        case ARP_CACHE_RESOVLED:
            cache->state = ARP_CACHE_POLLING;
            cache->rxmit = 1;
        // fallthrough
        case 1:
        case ARP_CACHE_POLLING:
            interface = cache->interface;
            ARPCancel(cache);
            if (cache->rxmit < 32) {
                cache->rxmit <<= 1;
                ARPOut(interface, 1, cache->prAddr, cache->state == ARP_CACHE_POLLING ? cache->hwAddr : NULL, IPEQ(interface->addr, IPAddrAny) ? interface->alias : interface->addr, cache);
            } else {
                IPRecoverGateway(cache->prAddr);
                IFQueueDequeueEntry(ARPCache*, &Up, cache);
                OSCancelAlarm(&cache->alarm);
                cache->state = 0;
                IFQueueEnqueueHead(ARPCache*, &Free, cache);
                DiscardPendingPackets(cache, -2);
            }
            break;
    }
    // References
    // -> static struct IFQueue Free;
    // -> static struct IFQueue Up;
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0x6B8 -> 0x6F0
static void TimeoutCallback(OSAlarm* alarm /* r1+0x8 */, OSContext* context) {
    // Local variables
    ARPCache* cache; // r31

    cache = (ARPCache*)((s32)alarm - offsetof(ARPCache, alarm));
    Revalidate(cache);
}

// // Range: 0x6F0 -> 0x7CC
static void SendCallback(ARPCache* cache /* r31 */) {
    ASSERT(cache->datagram.interface == NULL);
    ASSERT(cache->datagram.queue == NULL);

    switch (cache->state) {
        case 1:
        case ARP_CACHE_POLLING:
            OSCancelAlarm(&cache->alarm);
            OSSetAlarm(&cache->alarm, OSSecondsToTicks((OSTime)cache->rxmit), TimeoutCallback);
            break;
    }
}

// // Range: 0x7CC -> 0x878
void ARPInit(void) {
    // Local variables
    ARPCache* ent; // r31
    // struct IFQueue * ___prev; // r30

    memset(Cache, 0, sizeof(Cache));
    IFQueueInit(&Up);
    IFQueueInit(&Free);
    for (ent = &Cache[0]; ent < &Cache[ARP_CACHE_SIZE]; ent++) {
        OSCreateAlarm(&ent->alarm);
        IFQueueEnqueueTail(ARPCache*, &Free, ent);
    }
    
    // References
    // -> static struct ARPCache Cache[64];
    // -> static struct IFQueue Free;
    // -> static struct IFQueue Up;
}

// // Range: 0x878 -> 0x8F8
static ARPCache* Lookup(u8* prAddr /* r3 */) {
    // Local variables
    ARPCache* ent; // r31
    ARPCache* next; // r30

    IFQueueIterator(ARPCache*, &Up, ent, next) {
        if (IPEQ(ent->prAddr, prAddr)) {
            if (ent->state == ARP_CACHE_RESOVLED || ent->state == ARP_CACHE_POLLING) {
                return ent;
            }
        }
    }

    return NULL;

    // References
    // -> static struct IFQueue Up;
}

// // Range: 0x8F8 -> 0xA68
s32 ARPLookup(IPInterface* interface /* r25 */, u8* prAddr /* r26 */, u8* hwAddr /* r27 */) {
    // Local variables
    ARPCache* ent; // r31
    // struct IFQueue * ___next; // r30
    // struct IFQueue * ___prev; // r29
    // struct IFQueue * ___next; // r28

    if (IP_CLASSD(prAddr)) {
        hwAddr[0] = 1;
        hwAddr[1] = 0;
        hwAddr[2] = 94;
        hwAddr[3] = prAddr[1] & 0x7F;
        hwAddr[4] = prAddr[2];
        hwAddr[5] = prAddr[3];
        return ARP_MULTICAST;
    }

    if (IPIsBroadcastAddr(interface, prAddr)) {
        memset(hwAddr, 0xFF, 6);
        return ARP_BROADCAST;
    }

    if (IPIsLoopbackAddr(interface, prAddr)) {
        memmove(hwAddr, interface->mac, 6);
        return ARP_LOOPBACK;
    }

    ent = Lookup(prAddr);
    if (ent) {
        if (Up.next != &ent->link) {
            IFQueueDequeueEntry(ARPCache*, &Up, ent);
            IFQueueEnqueueHead(ARPCache*, &Up, ent);
        }

        memmove(hwAddr, ent->hwAddr, 6);
        return ARP_FOUND;
    }

    return ARP_NOTFOUND;

    // References
    // -> static struct IFQueue Up;
}

// // Range: 0xA68 -> 0xAAC
void ARPRevalidate(u8* prAddr /* r1+0x8 */) {
    // Local variables
    ARPCache* ent; // r31

    ent = Lookup(prAddr);
    if (ent) {
        Revalidate(ent);
    }
}

// // Range: 0xAAC -> 0xC60
static ARPCache* ARPAlloc(u8* prAddr /* r25 */, BOOL alloc /* r1+0xC */) {
    // Local variables
    ARPCache* ent; // r30
    ARPCache* next; // r26
    ARPCache* free; // r31
    // struct IFQueue * ___next; // r29
    // struct IFQueue * ___prev; // r28
    // struct IFQueue * ___next; // r27

    IFQueueIterator(ARPCache*, &Up, ent, next) {
        if (IPEQ(ent->prAddr, prAddr)) {
            return ent;
        }
    }

    if (!alloc) {
        return NULL;
    }

    if (Free.next != NULL) {
        IFQueueDequeueHead(ARPCache*, &Free, free);
    } else {
        IFQueueDequeueTail(ARPCache*, &Up, free);
        ARPCancel(free);
        OSCancelAlarm(&free->alarm);
        if (free->state == 1) {
            DiscardPendingPackets(free, -7);
        }

        ASSERT(IFIsEmptyQueue(&free->queue));
    }

    memset(free, 0, sizeof(ARPCache));
    IFQueueInit(&free->queue);
    OSCreateAlarm(&free->alarm);
    free->rxmit = 1;
    memmove(free->prAddr, prAddr, sizeof(free->prAddr));
    IFQueueEnqueueHead(ARPCache*, &Up, free);
    return free;

    // References
    // -> static struct IFQueue Up;
    // -> static struct IFQueue Free;
}

// // Range: 0xC60 -> 0xD40
void ARPAdd(IPInterface* interface /* r1+0x8 */, u8* prAddr /* r1+0xC */, u8* hwAddr /* r1+0x10 */) {
    // Local variables
    ARPCache* cache; // r31

    cache = ARPAlloc(prAddr, TRUE);
    if (cache) {
        ASSERT(cache->state != ARP_CACHE_RESOVLED && cache->state != ARP_CACHE_POLLING);
        cache->state = ARP_CACHE_RESOVLED;
        cache->interface = interface;
        cache->rxmit = 1200;
        OSSetAlarm(&cache->alarm, OSSecondsToTicks((OSTime)cache->rxmit), TimeoutCallback);
        memmove(cache->hwAddr, hwAddr, 6);
    }
}

// // Range: 0xD40 -> 0xF5C
void ARPHold(IPInterface* interface /* r29 */, struct IFDatagram * datagram /* r31 */) {
    // Local variables
    ARPCache* free; // r30
    int state; // r22
    void (* callback)(void *, long); // r24
    void* param; // r21
    int discard; // r25
    s32 nVec; // r27
    IFVec va[4]; // r1+0x10
    IFVec* vec; // r23
    // struct IFQueue * ___prev; // r28

    ASSERT(datagram->interface == interface);
    callback = datagram->callback;
    param = datagram->param;
    datagram->interface = NULL;
    datagram->queue = NULL;
    free = ARPAlloc(datagram->dst, TRUE);
    ASSERT(free);
    ASSERT(free->state != ARP_CACHE_RESOVLED && free->state != ARP_CACHE_POLLING);
    state = free->state;
    free->state = 1;
    free->interface = interface;
    nVec = datagram->nVec;
    ASSERT(0 < nVec && nVec <= IF_MAX_VEC);
    memmove(va, datagram->vec, nVec * sizeof(IFVec));
    
    discard = 0;
    while (nVec-- > 0) {
        vec = &va[nVec];
        discard |= interface->free(interface, vec->data, vec->len);
    }

    discard |= interface->free(interface, datagram, sizeof(IFDatagram) + (datagram->nVec > 1 ? (datagram->nVec - 1) * sizeof(IFVec) : 0));

    if (discard == 0) {
        datagram->interface = interface;
        datagram->queue = &free->queue;
        IFQueueEnqueueTail(IFDatagram*, &free->queue, datagram);
    } else {
        if (callback) {
            (*callback)(param, -7);
        }
    }

    if (state == 0) {
        ARPOut(interface, 1, free->prAddr, NULL, IPEQ(interface->addr, IPAddrAny) ? interface->alias : interface->addr, free);
    }
    // References
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0xF5C -> 0x11A4
static void ARPUpdate(IPInterface* interface /* r27 */, ARPHeader* arp /* r29 */) {
    // Local variables
    ARPCache* cache; // r31
    int state; // r24
    u8* src; // r26
    IFDatagram* datagram; // r28
    // struct IFQueue * ___next; // r30

    src = ARPHeader2PrAddr(arp);
    if (IP_CLASSD(src) || IP_CLASSE(src) || IPEQ(src, IPAddrAny) || IPIsBroadcastAddr(interface, src)) {
        return;
    }

    cache = ARPAlloc(src, IPEQ(ARPHeader2Addr(arp), interface->addr) || IPEQ(ARPHeader2Addr(arp), interface->alias));
    if (cache != NULL) {
        ARPCancel(cache);
        OSCancelAlarm(&cache->alarm);
        cache->rxmit = 1200;
        OSSetAlarm(&cache->alarm, OSSecondsToTicks((OSTime)cache->rxmit), TimeoutCallback);
        state = cache->state;
        cache->state = ARP_CACHE_RESOVLED;
        memmove(cache->hwAddr, ARPHeader2MACAddr(arp), 6);
        if (cache->interface != interface) {
            DiscardPendingPackets(cache, -2);
            cache->interface = interface;
        }

        if (state == 1) {
            while (cache->queue.next) {
                IFQueueDequeueHead(IFDatagram*, &cache->queue, datagram);
                ASSERT(datagram->queue == &cache->queue);
                ASSERT(datagram->type == ETH_IP);
                ASSERT(ARPLookup(interface, datagram->dst, datagram->hwAddr) == ARP_FOUND);
                datagram->queue = NULL;
                interface->out(interface, datagram);
            }
        }
    }

    // References
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0x11A4 -> 0x13C0
void ARPOut(IPInterface* interface /* r29 */, u16 opCode /* r1+0xC */, const u8* dstPrAddr /* r1+0x10 */, const u8* dstHwAddr /* r27 */, const u8* srcPrAddr /* r25 */, ARPCache* cache /* r28 */) {
    // Local variables
    BOOL enabled; // r26
    IFDatagram* datagram; // r30
    ARPHeader* arp; // r31

    enabled = OSDisableInterrupts();
    if (interface->out != ETHOut) {
        OSRestoreInterrupts(enabled);
        return;
    }

    if (cache != NULL) {
        datagram = &cache->datagram;
        arp = (ARPHeader*)cache->arp;
        ASSERT(datagram->queue == NULL);
    } else {
        datagram = (IFDatagram*)interface->alloc(interface, sizeof(IFDatagram) + sizeof(cache->arp));
        arp = (ARPHeader*)((s32)datagram + sizeof(IFDatagram));
    }

    if (datagram != NULL) {
        arp->hwType = 1;
        arp->prType = ETH_IP;
        arp->hwAddrLen = 6;
        arp->prAddrLen = 4;
        arp->opCode = opCode;
        memmove(ARPHeader2Addr(arp), dstPrAddr, 4);
        
        if (dstHwAddr) {
            memmove(ArpHeader2HwAddr(arp), dstHwAddr, 6);
        } else {
            memset(ArpHeader2HwAddr(arp), 0, 6);
        }

        memmove(ARPHeader2PrAddr(arp), srcPrAddr, 4);
        memmove(ARPHeader2MACAddr(arp), interface->mac, 6);
        
        datagram->nVec = 1;
        datagram->vec[0].data = arp;
        datagram->vec[0].len = sizeof(cache->arp);
        datagram->callback = cache ? (void (*)(void *, s32))SendCallback : NULL;
        datagram->param = cache;
        datagram->type = ETH_IP | 6;
        datagram->flag = 0;

        if (dstHwAddr && (srcPrAddr[0] != 169 || srcPrAddr[1] != 254)) {
            memmove(datagram->hwAddr, dstHwAddr, 6);
        } else {
            memmove(datagram->hwAddr, HwBroadcastAddr, 6);
        }
        
        datagram->queue = NULL;
        interface->out(interface, datagram);
    }

    OSRestoreInterrupts(enabled);
    // References
    // -> static unsigned char HwBroadcastAddr[6];
}

// // Range: 0x13C0 -> 0x1410
static void ARPCancel(ARPCache* cache /* r1+0x8 */) {
    // Local variables
    IFDatagram* datagram; // r30
    IPInterface* interface; // r31

    datagram = &cache->datagram;
    interface = datagram->interface;
    if (interface != NULL) {
        interface->cancel(interface, datagram);
    }
}

// // Range: 0x1410 -> 0x1498
static void GratuitousCallback(OSAlarm* alarm /* r1+0x8 */, OSContext* context) {
    // Local variables
    IPInterface* interface; // r31

    interface = (IPInterface*)((u8*)alarm - offsetof(IPInterface, gratuitousAlarm));
    if (IPNEQ(interface->addr, IPAddrAny)) {
        ARPOut(interface, 1, interface->addr, NULL, interface->addr, NULL);
    }

    if (IPNEQ(interface->alias, IPAddrAny)) {
        ARPOut(interface, 1, interface->alias, NULL, interface->alias, NULL);
    }

    // References
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0x1498 -> 0x1544
void ARPGratuitous(IPInterface* interface /* r31 */) {
    OSCancelAlarm(&interface->gratuitousAlarm);
    OSSetAlarm(&interface->gratuitousAlarm, OSSecondsToTicks(3), GratuitousCallback);

    if (IPNEQ(interface->addr, IPAddrAny)) {
        ARPOut(interface, 1, interface->addr, NULL, interface->addr, NULL);
    }

    if (IPNEQ(interface->alias, IPAddrAny)) {
        ARPOut(interface, 1, interface->alias, NULL, interface->alias, NULL);
    }

    // References
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0x1544 -> 0x1584
void ARPProbe(IPInterface* interface /* r1+0x8 */, u8* prAddr /* r1+0xC */) {

    ARPOut(interface, 1, prAddr, NULL, IPAddrAny, NULL);
    // References
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0x1584 -> 0x1614
static void ClaimHandler(OSAlarm* alarm /* r1+0x8 */, OSContext* context) {
    // Local variables
    IPInterfaceConf* conf; // r31
    IPInterface* interface; // r30

    conf = (IPInterfaceConf*)((u8*)alarm - offsetof(IPInterfaceConf, alarm));
    interface = conf->interface;
    
    conf->count++;
    if (conf->count <= 4) {
        ARPProbe(interface, conf->addr);
        OSSetAlarm(&conf->alarm, OSSecondsToTicks(2), ClaimHandler);
    } else {
        conf->callback(conf, 0);
    }
}

// // Range: 0x1614 -> 0x17CC
void ARPClaim(IPInterface* interface /* r27 */, IPInterfaceConf* conf /* r31 */) {
    // Local variables
    BOOL enabled; // r23
    OSTime wait; // r25
    OSTick r; // r24
    // struct IFQueue * ___prev; // r28
    // struct IFQueue * ___next; // r30
    // struct IFQueue * ___prev; // r29

    ASSERT(interface);
    enabled = OSDisableInterrupts();

    if (IPNEQ(conf->addr, IPAddrAny)) {
        ASSERT(conf->callback);

        if (conf->interface == NULL) {
            conf->interface = interface;
            IFQueueEnqueueTail(IPInterfaceConf*, &interface->queue, conf);
        }

        OSCancelAlarm(&conf->alarm);
        conf->count = 0;
        wait = OSMillisecondsToTicks(1200);
        r = OSGetTick();
        r %= OSMillisecondsToTicks(1600);
        wait = (wait + r) - OSMillisecondsToTicks(800);
        OSSetAlarm(&conf->alarm, wait, ClaimHandler);
    } else if (conf->interface == interface) {
        OSCancelAlarm(&conf->alarm);
        IFQueueDequeueEntry(IPInterfaceConf*, &interface->queue, conf);
        conf->interface = NULL;
    }

    OSRestoreInterrupts(enabled);

    // References
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0x17CC -> 0x1978
static BOOL DetectCollision(IPInterface* interface /* r29 */, ARPHeader* arp /* r30 */) {
    // Local variables
    IPInterfaceConf* conf; // r31
    IPInterfaceConf* next; // r28

    IFQueueIterator(IPInterfaceConf*, &interface->queue, conf, next) {
        if (
            IPNEQ(IPAddrAny, conf->addr) && (IPEQ(ARPHeader2PrAddr(arp), conf->addr) && memcmp(ARPHeader2MACAddr(arp), interface->mac, 6) != 0 ||
            (IPEQ(ARPHeader2Addr(arp), conf->addr) && IPEQ(ARPHeader2PrAddr(arp), IPAddrAny)))
        ) {
            OSCancelAlarm(&conf->alarm);

            if (conf->callback) {
                conf->callback(conf, -111);
            }
        }
    }

    if (IPNEQ(IPAddrAny, interface->addr) && IPEQ(ARPHeader2PrAddr(arp), interface->addr) && memcmp(ARPHeader2MACAddr(arp), interface->mac, 6) != 0) {
        IPSetConfigError(interface, -111);
        return TRUE;
    } else if (IPNEQ(IPAddrAny, interface->alias) && IPEQ(ARPHeader2PrAddr(arp), interface->alias) && memcmp(ARPHeader2MACAddr(arp), interface->mac, 6) != 0) {
        IPAutoConfig();
        return TRUE;
    }

    return FALSE;

    // References
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0x1978 -> 0x1AEC
void ARPIn(IPInterface* interface /* r30 */, ETHHeader* eh /* r1+0xC */, s32 len /* r29 */) {
    // Local variables
    ARPHeader* arp; // r31

    arp = (ARPHeader*)(eh+1);
    if (interface->out != ETHOut || len < 22 || len < (2 * (arp->hwAddrLen + arp->prAddrLen) + 22)) {
        return;
    }

    if (arp->hwType != 1 || arp->prType != ETH_IP || arp->hwAddrLen != 6 || arp->prAddrLen != 4) {
        return;
    }

    DetectCollision(interface, arp);
    ARPUpdate(interface, arp);

    if (IPEQ(ARPHeader2Addr(arp), IPAddrAny) ||
        ((IPNEQ(ARPHeader2Addr(arp), interface->addr) && IPNEQ(ARPHeader2Addr(arp), interface->alias)))) {
        return;
    }

    if (arp->opCode == 1) {
        ARPOut(interface, 2, ARPHeader2PrAddr(arp), ARPHeader2MACAddr(arp), ARPHeader2Addr(arp), NULL);
    }
    // References
    // -> unsigned char IPAddrAny[4];
}

// // Range: 0x1AEC -> 0x1BAC
void ARPRefresh(void) {
    // Local variables
    ARPCache* cache; // r31
    BOOL enabled; // r28
    // struct IFQueue * ___next; // r30
    // struct IFQueue * ___next; // r29

    enabled = OSDisableInterrupts();
    
    while (Up.next) {
        IFQueueDequeueHead(ARPCache*, &Up, cache);
        ARPCancel(cache);
        OSCancelAlarm(&cache->alarm);
        cache->state = 0;
        IFQueueEnqueueHead(ARPCache*, &Free, cache);
        DiscardPendingPackets(cache, 0);
    }

    OSRestoreInterrupts(enabled);

    // References
    // -> static struct IFQueue Up;
    // -> static struct IFQueue Free;
}
