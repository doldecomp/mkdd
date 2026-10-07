#include "dolphin/ip.h"
#include "dolphin/ip/IPDhcp.h"
#include "dolphin/ip/IPUdp.h"
#include "dolphin/os.h"
#include "dolphin/os/OSContext.h"
#include <dolphin/private/ip.h>

#include <cstddef.h>
#include <stdio.h>
#include <string.h>

static SOAllocFunc Alloc = NULL;
static SOFreeFunc Free = NULL;
static u32 Allocated = 0;

#define SO_TABLE_NUM 256
static SONode SocketTable[SO_TABLE_NUM];
static IFQueue LingerQueue;
static SOSockAddrIn SockAnyIn = { 8, 2, 0, { 0 } };
static u8* TimeWaitBuf = NULL;
static s32 TimeWaitBufSize = 0;
static u8* ReassemblyBuffer = NULL;
static s32 ReassemblyBufferSize = 0;
static s32 State = 0;
static u32 Flag = 0;
static s32 Mtu = 0;
static s32 Rwin = 0;
static OSTime R2 = 0;

static OSThreadQueue CleaningQueue;
static OSThreadQueue PollingQueue;
static BOOL LowInitialized;
static BOOL Initialized;

static BOOL OnReset(BOOL);
static OSResetFunctionInfo ResetFunctionInfo = { &OnReset, 110, NULL, NULL };

static void LingerCallback(TCPInfo* info, s32);

void* SOAlloc(u32 name, s32 size) {
    void* ptr;
    BOOL enabled;

    ASSERT(Alloc);
    
    ptr = (*Alloc)(name, size);
    if (ptr != NULL) {
        enabled = OSDisableInterrupts();
        Allocated += size;
        OSRestoreInterrupts(enabled);
    }

    return ptr;
}

void SOFree(u32 name, void* ptr, s32 size) {
    BOOL enabled;

    ASSERT(Free);

    if (ptr != NULL) {
        (*Free)(name, ptr, size);
        enabled = OSDisableInterrupts();
        Allocated -= size;

        if (Allocated == 0 && State == 2) {
            OSWakeupThread(&CleaningQueue);
        }

        OSRestoreInterrupts(enabled);
    }
}

u32 SONtoHl(u32 netlong) {
    return netlong;
}

u16 SONtoHs(u16 netshort) {
    return netshort;
}

u32 SOHtoNl(u32 hostlong) {
    return hostlong;
}

u16 SOHtoNs(u16 hostshort) {
    return hostshort;
}

int SOInetAtoN(const char* cp, SOInAddr* inp) {
    u8 addr[4];

    if (IPAtoN(cp, inp != NULL ? (u8*)&inp->addr : addr) != NULL) {
        return TRUE;
    }

    return FALSE;
}

char* SOInetNtoA(SOInAddr in) {
    return IPNtoA((u8*)&in);
}

int SOInetPtoN(int af, const char* src, void* dst) {
    if (af == 2) {
        if (IPAtoN(src, (u8*)dst)) {
            return TRUE;
        }

        return FALSE;
    }

    return -5;
}

char* SOInetNtoP(int af, void* src, char* dst, u32 len) {
    const u8* addr;

    if (af == 2 && dst != NULL && len >= 16) {
        addr = (const u8*)src;
        sprintf(dst, "%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]);
        return dst;
    }

    return NULL;
}

static struct SONode* GetNode(int s, IPInfo** pinfo) {
    SONode* node;
    IPInfo* info;
    IPInfo* next;
    TCPInfo* tcp;
    BOOL enabled;
    IFQueue queue;

    queue.next = queue.prev = NULL;
    enabled = OSDisableInterrupts();
    
    /* Find any TCP packets which are unused */
    IFQueueIterator(IPInfo*, &LingerQueue, info, next) {
        tcp = (TCPInfo*)info;
        if (tcp->node == NULL || ((SONode*)tcp->node)->ref == 0) {
            IFQueueDequeueEntry(IPInfo*, &LingerQueue, info);
            IFQueueEnqueueTail(IPInfo*, &queue, info);
        }
    }

    OSRestoreInterrupts(enabled);

    /* Free all unused TCP packets */
    while (queue.next != NULL) {
        IFQueueDequeueHead(IPInfo*, &queue, info);

        tcp = (TCPInfo*)info;
        SOFree(2, tcp->recvData, tcp->recvBuff);
        SOFree(1, tcp->sendData, tcp->sendBuff);
        SOFree(0, tcp, sizeof(TCPInfo));
    }

    node = NULL;
    enabled = OSDisableInterrupts();
    if (s >= 0 && s < SO_TABLE_NUM) {
        node = &SocketTable[s];
        if (node->ref <= 0 || node->info == NULL) {
            node = NULL;
        } else {
            node->ref++;
            if (pinfo != NULL) {
                *pinfo = node->info;
            }
        }
    }
    OSRestoreInterrupts(enabled);
    return node;
}

static void PutNode(SONode* node) {
    BOOL enabled;
    IPInfo* info;
    TCPInfo* tcp;
    UDPInfo* udp;
    u8 proto;

    ASSERT(node);

    proto = 0;
    info = NULL;
    enabled = OSDisableInterrupts();
    ASSERT(0 < node->ref);
    if (--node->ref == 0 && node->info != NULL) {
        info = node->info;
        node->info = NULL;
        proto = node->proto;
        node->proto = 0;
    }
    OSRestoreInterrupts(enabled);

    if (info != NULL) {
        switch (proto) {
            case IP_PROTO_UDP:
                udp = (UDPInfo*)info;
                SOFree(5, udp->recvRing, udp->recvBuff);
                SOFree(4, udp->sendData, udp->sendBuff);
                SOFree(3, udp, sizeof(UDPInfo));
                break;
            case IP_PROTO_TCP:
                tcp = (TCPInfo*)info;
                SOFree(2, tcp->recvData, tcp->recvBuff);
                SOFree(1, tcp->sendData, tcp->sendBuff);
                SOFree(0, tcp, sizeof(TCPInfo));
                break;
            default:
                OSHaltLine(540, "PutNode: unknown proto");
                break;
        }
    }
}

static SOResolver Resolver;

int SOSetResolver(const SOInAddr* dns1, const SOInAddr* dns2) {
    if (State != 1) {
        return -39;
    }

    memmove(Resolver.dns1, dns1 ? dns1 : (const SOInAddr*)&IPAddrAny, sizeof(Resolver.dns1));
    memmove(Resolver.dns2, dns2 ? dns2 : (const SOInAddr*)&IPAddrAny, sizeof(Resolver.dns2));

    DNSClose(&Resolver.info);

    if (*(u32*)Resolver.dns1 != *(u32*)IPAddrAny) {
        DNSOpen(&Resolver.info, Resolver.dns1);
    }
    else if (*(u32*)Resolver.dns2 != *(u32*)IPAddrAny) {
        DNSOpen(&Resolver.info, Resolver.dns2);
    }

    Resolver.info.flag |= 0x4;
    return 0;
}

int SOGetResolver(SOInAddr* dns1, SOInAddr* dns2) {
    BOOL enabled;
    int rc;

    enabled = OSDisableInterrupts();
    if (State != 1) {
        rc = -39;
    } else {
        if (dns1 != NULL) {
            memcpy(dns1, Resolver.dns1, sizeof(Resolver.dns1));
        }

        if (dns2 != NULL) {
            memcpy(dns2, Resolver.dns2, sizeof(Resolver.dns2));
        }

        rc = 0;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

static void LcpHandler(PPPConf* conf) {
    if (conf->state == 0 && State == 2) {
        OSWakeupThread(&CleaningQueue);
    }
}

static void DhcpHandler(int state) {
    DHCPInfo info;
    u8 prev1[4];
    u8 prev2[4];
    
    switch (state) {
        case 3:
        DHCPGetStatus(&info);
            if (SOGetResolver((SOInAddr*)prev1, (SOInAddr*)prev2) == 0) {
                if (IPEQ(prev1, IPAddrAny) && IPEQ(prev2, IPAddrAny)) {
                    //DHCPGetOpt(DHCP_OPT_DNS, dns, sizeof(dns));
                    SOSetResolver((SOInAddr*)info.dns1, (SOInAddr*)info.dns2);
                } else {
                    SOSetResolver((SOInAddr*)prev1, (SOInAddr*)prev2);
                }
            }
            break;
        case 0:
            IPSetMtu(0, Mtu);
            if (State == 2) {
                OSWakeupThread(&CleaningQueue);
            }
            break;
    }
}

void SOInit(void) {
    if (!Initialized) {
        Initialized = TRUE;
        OSRegisterResetFunction(&ResetFunctionInfo);
    }

    IFInit(4);
    if (State == 0) {
        if (SOGetHostID() != 0u || DHCPGetStatus(0) != 0) {
            LowInitialized = TRUE;
        } else {
            IFMute(TRUE);
        }
    }
}

int SOStartup(const SOConfig* config) {
    SOHostEnt* ent = &Resolver.ent;
    s32 mtu;

    if (config->vendor == 0 && config->version == 0x0100) {
        if (!IFInit(4)) {
            return -28;
        }

        if (State  != 0) {
            return -28;
        }

        if (config->mtu > 0) {
            if (SO_GET_CONFIG_MTU(config) < 68) {
                mtu = 68;
            } else if (config->mtu >= SO_MTU_MAX) {
                mtu = SO_MTU_MAX;
            } else {
                mtu = config->mtu;
            }
        } else {
            mtu = SO_MTU_MAX;
        }

        Mtu = mtu;
        IPSetMtu(0, mtu);

        if (config->rwin > 0) {
            Rwin = config->rwin < 28 ? 28 : config->rwin;
        } else {
            Rwin = 0;
        }

        if (config->r2 > 0) {
            R2 = config->r2;
        } else {
            R2 = OSSecondsToTicks(100); // default timeout is 100 seconds
        }

        OSInitThreadQueue(&CleaningQueue);
        OSInitThreadQueue(&PollingQueue);

        Alloc = config->alloc;
        Free = config->free;
        Flag = config->flag;

        if (!LowInitialized) {
            if (config->timeWaitBuffer != 0) {
                TimeWaitBufSize = config->timeWaitBuffer;
                TimeWaitBuf = SOAlloc(6, TimeWaitBufSize);
                TCPSetTimeWaitBuffer(TimeWaitBuf, TimeWaitBufSize);
            }

            if (config->reassemblyBuffer != 0) {
               ReassemblyBufferSize = config->reassemblyBuffer;
                ReassemblyBuffer = SOAlloc(7, ReassemblyBufferSize);
                //IPSetReassemblyBuffer(ReassemblyBuffer, ReassemblyBufferSize, UdpSendBuff);
            }

            IPClearConfigError(0);
        }

        if (!LowInitialized) {
            if ((Flag & 2) != 0) {
                Flag &= ~0x8001;
                if (PPPInit(&__IFDefault, &PPPLcpConf, &PPPIpcpConf, config->peerid, config->passwd) == 0) {
                    goto fail;
                }
                PPPLcpConf.callback = &LcpHandler;
            } else if ((Flag & 1) != 0) {
                if (DHCPStartupEx(&DhcpHandler, config->rdhcp) == 0) {
                    LowInitialized = TRUE;
                }

                DHCPAuto(0);
            } else {
                if (config->addr.addr != 0) {
                    if (SOGetHostID() == 0u) {
                        IPInitRoute(&config->addr, &config->netmask, &config->router);
                    } else {
                        LowInitialized = TRUE;
                    }
                }
            }
        }

        if (!LowInitialized) {
            ARPRefresh();
        }

        if ((Flag & 0x8000) != 0) {
            IPAutoConfig();
        }

        LingerQueue.next = LingerQueue.prev = NULL;
        memset(&Resolver, 0, sizeof(Resolver));
        Resolver.zero = NULL;
        ent->name = Resolver.name;
        ent->aliases = &Resolver.zero;
        ent->addrType = 2;
        ent->length = 4;
        ent->addrList = Resolver.ptrList;
        SOSetResolver(&config->dns1, &config->dns2);
        State = 1;
        return 0;
    }

fail:
    if (TimeWaitBuf != NULL) {
        SOFree(6, TimeWaitBuf, TimeWaitBufSize);
    }

    if (ReassemblyBuffer != NULL) {
        SOFree(7, ReassemblyBuffer, ReassemblyBufferSize);
    }

    return -28;
}

int SOCleanup(void) {
    int s;
    SONode* node;
    IPInfo* info;
    IPInfo* next;
    SOLinger linger;
    int optlen;
    BOOL enabled;
    TCPInfo* tcp;

    if (State != 1) {
        return -27;
    }

    State = 2;
    __IPWakeupPollingThreads();

    for (s = 0; s < SO_TABLE_NUM; s++) {
        node = &SocketTable[s];

        if (node->ref != 0) {
            switch (node->proto) {
                case IP_PROTO_UDP:
                    __SOClose(s);
                    break;
                case IP_PROTO_TCP:
                    optlen = 8;
                    linger.onoff = 1;
                    linger.linger = 0;
                    __SOSetSockOpt(s, 0xFFFF, 0x80, &linger, optlen);
                    __SOClose(s);
                    break;
            }
        }

        IFQueueIterator(IPInfo*, &TCPInfoQueue, info, next) {
            tcp = (TCPInfo*)info;

            if (tcp->closeCallback == &LingerCallback) {
                TCPCancel(tcp);
            }
        }

        GetNode(-1, NULL);

        if ((Flag & 0x8000) != 0) {
            IPAutoStop();
        }

        DNSClose(&Resolver.info);

        if (!LowInitialized) {
            if ((Flag & 2) != 0) {
                PPPClose(&PPPIpcpConf);
                enabled = OSDisableInterrupts();
                while (PPPGetState(&PPPLcpConf) != 0) {
                    OSSleepThread(&CleaningQueue);
                }
                OSRestoreInterrupts(enabled);
            } else if ((Flag & 1) != 0) {
                enabled = OSDisableInterrupts();
                DHCPCleanup();
                while (DHCPGetStatus(0) != 0) {
                    OSSleepThread(&CleaningQueue);
                }
                OSRestoreInterrupts(enabled);
            } else {
                IPInitRoute(0, 0, 0);
                IPSetBroadcastAddr(&__IFDefault, 0);
            }
        }

        if (TimeWaitBuf != NULL) {
            SOFree(6, TimeWaitBuf, TimeWaitBufSize);
        }

        if (ReassemblyBuffer != NULL) {
            //IPSetReassemblyBuffer(NULL, 0, UdpSendBuff + 20);
            SOFree(7, ReassemblyBuffer, ReassemblyBufferSize);
        }

        enabled = OSDisableInterrupts();
        while (Allocated != 0) {
            OSSleepThread(&CleaningQueue);
        }
        OSRestoreInterrupts(enabled);
        ASSERT(Allocated == 0);

        if (!LowInitialized) {
            IFMute(TRUE);
            ARPRefresh();
        }

        State = 0;
        return 0;
    }
}

static s32 GetRwin(void) {
    s32 mtu;

    if (Rwin != 0) {
        return Rwin;
    }

    IPGetMtu(0, &mtu);
    return (mtu - 40) * 2;
}

int SOSocket(int af, int type, int protocol) {
    BOOL enabled;
    int socket;
    SONode* node;
    s32 rc;
    TCPInfo* tcp;
    UDPInfo* udp;
    void* sendbuf;
    void* recvbuf;
    s32 rwin;

    tcp = NULL;
    udp = NULL;

    if (State != 1) {
        return -39;
    }

    if (af != 2) {
        return -5;
    }

    if (protocol != 0) {
        return -68;
    }

    GetNode(-1, NULL);
    node = NULL;
    enabled = OSDisableInterrupts();
    for (socket = 0; socket < SO_TABLE_NUM; socket++) {
        node = &SocketTable[socket];
        if (node->ref == 0) {
            ASSERT(node->info == NULL);
            node->ref = 2;
            break;
        }
    }
    OSRestoreInterrupts(enabled);

    if (node == NULL) {
        return -33;
    }

    rwin = GetRwin();
    switch (type) {
        case 1:
            tcp = SOAlloc(0, sizeof(TCPInfo));
            sendbuf = SOAlloc(1, rwin);
            recvbuf = SOAlloc(2, rwin);
            rc = TCPOpen(tcp, sendbuf, rwin, recvbuf, rwin);
            if (rc >= 0) {
                TCPSetTimeout(tcp, R2);
            }
            break;
        case 2:
            udp = SOAlloc(3, sizeof(UDPInfo));
            sendbuf = SOAlloc(4, 0x5b2);
            recvbuf = SOAlloc(5, 0x1000);
            rc = UDPOpen(udp, recvbuf, 0x1000);
            if (rc >= 0) {
                rc = UDPSetSendBuff(udp, sendbuf, 1458);
            }
            break;
        default:
            PutNode(node);
            PutNode(node);
            return -69;
    }

    if (rc < 0) {
        switch (type) {
            case 1:
                SOFree(0, tcp, sizeof(TCPInfo));
                SOFree(1, sendbuf, rwin);
                SOFree(2, recvbuf, rwin);
                break;
            case 2:
                SOFree(3, udp, sizeof(UDPInfo));
                SOFree(4, sendbuf, 0x5b2);
                SOFree(5, recvbuf, 0x1000);
                break;
        }

        PutNode(node);
        PutNode(node);
        return -49;
    }

    node->flag = 0;
    OSInitMutex(&node->mutexRead);
    OSInitMutex(&node->mutexWrite);

    switch (type) {
        case 1:
            tcp->node = node;
            node->proto = IP_PROTO_TCP;
            node->info = &tcp->pair;
            break;
        case 2:
            node->proto = IP_PROTO_UDP;
            node->info = &udp->pair;
            break;
    }

    PutNode(node);
    return socket;
}

static void LingerCallback(TCPInfo* info, s32) {
    SONode* node;

    node = (SONode*)info->node;
    if (node != NULL) {
        ASSERT(0 < node->ref);
        node->ref--;
    }

    IFQueueEnqueueTail(IPInfo*, &LingerQueue, &info->pair);
}

static void LingerTimeout(OSAlarm* alarm, OSContext*) {
    TCPInfo* tcp;

    tcp = (TCPInfo*)(((u8*)alarm) - offsetof(TCPInfo, lingerAlarm));
    TCPCancel(tcp);
}

static int __SOClose(int s) {
    SONode* node;
    IPInfo* info;
    TCPInfo* tcp;
    TCPInfo* log;
    UDPInfo* udp;
    SOLinger linger;
    int optlen;
    BOOL enabled;
    s32 rc;
    IFQueue queue;

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    ASSERT(0 < node->ref);
    switch (node->proto) {
        case IP_PROTO_UDP:
            rc = UDPClose(info);
            ASSERT(0 <= rc);
            node->ref--;
            break;
        case IP_PROTO_TCP:
            tcp = (TCPInfo*)info;
            optlen = 8;
            rc = TCPGetSockOpt(tcp, 0xFFFF, 0x80, &linger, &optlen);
            ASSERT(0 <= rc);

            queue.next = queue.prev = NULL;
            enabled = OSDisableInterrupts();
            
            while (tcp->queueBacklog.next != NULL) {
                IFQueueDequeueHeadLINK(TCPInfo*, &tcp->queueBacklog, linkLog, log);
                log->logging = NULL;
                TCPCancel(log);
                IFQueueEnqueueTailLINK(TCPInfo*, &queue, linkLog, log);
            }

            while (tcp->queueCompleted.next != NULL) {
                IFQueueDequeueHeadLINK(TCPInfo*, &tcp->queueCompleted, linkLog, log);
                log->logging = NULL;
                TCPCancel(log);
                IFQueueEnqueueTailLINK(TCPInfo*, &queue, linkLog, log);
            }

            if (TCPGetStatus(tcp) == TCP_STATE_LISTEN) {
                rc = TCPCancel(tcp);
                ASSERT(TCPGetStatus(tcp) != TCP_STATE_LISTEN);
                if (tcp->accepting > 0) {
                    OSWakeupThread(&tcp->queueThread);
                }
                node->ref--;
            } else if ((node->flag & 0x4) != 0) {
                rc = TCPCancel(tcp);
                node->ref--;
            } else if (linger.onoff) {
                if (linger.linger <= 0) {
                    rc = TCPCancel(tcp);
                } else {
                    OSSetAlarm(&tcp->lingerAlarm, OSSecondsToTicks(linger.linger), &LingerTimeout);
                    rc = TCPClose(tcp);
                }

                node->ref--;
            } else {
                OSSetAlarm(&tcp->lingerAlarm, OSSecondsToTicks(15), &LingerTimeout);
                rc = TCPCloseAsync(tcp, &LingerCallback, 0);
                if (node->ref == 2) {
                    tcp->node = NULL;
                    node->ref--;
                }
                node->info = NULL;
            }

            OSRestoreInterrupts(enabled);

            while (queue.next != NULL) {
                IFQueueDequeueHeadLINK(TCPInfo*, &queue, linkLog, log);
                SOFree(2, log->recvData, log->recvBuff);
                SOFree(1, log->sendData, log->sendBuff);
                SOFree(0, log, sizeof(TCPInfo));
            }
            
            ASSERT(0 <= rc);
            break;
        default:
            rc = -8;
            break;
    }

    PutNode(node);

    if (rc < 0) {
        return -8;
    }

    return 0;
}

int SOClose(int s) {
    if (State != 1) {
        return -39;
    }

    return __SOClose(s);
}

static void AcceptCallback(TCPInfo* tcp, s32 result) {
    TCPInfo* logging;

    logging = tcp->logging;
    if (logging != NULL) {
        if (result >= 0) {
            IFQueueDequeueEntryLINK(TCPInfo*, &logging->queueBacklog, linkLog, tcp);
            IFQueueEnqueueTailLINK(TCPInfo*, &logging->queueCompleted, linkLog, tcp);
            OSWakeupThread(&logging->queueThread);
            if (logging->pair.poll > 0) {
                __IPWakeupPollingThreads();
            }
        } else {
            IFQueueDequeueEntryLINK(TCPInfo*, &logging->queueBacklog, linkLog, tcp);
            TCPCancel(tcp);
            TCPOpen(tcp, tcp->sendData, tcp->sendBuff, tcp->recvData, tcp->recvBuff);
            TCPSetTimeout(tcp, R2);
            tcp->logging = logging;
            IFQueueEnqueueTailLINK(TCPInfo*, &logging->queueBacklog, linkLog, tcp);
            TCPAcceptAsync(tcp, logging, &AcceptCallback, 0);
        }
    }
}

static TCPInfo* AddBackLog(TCPInfo* listening) {
    TCPInfo* tcp;
    void* sendbuf;
    s32 sendbufLen;
    void* recvbuf;
    s32 recvbufLen;
    s32 rc;
    BOOL enabled;
    
    ASSERT(listening);
    tcp = SOAlloc(0, sizeof(TCPInfo));
    sendbufLen = listening->sendBuff;
    recvbufLen = listening->recvBuff;
    sendbuf = SOAlloc(1, sendbufLen);
    recvbuf = SOAlloc(2, recvbufLen);
    rc = TCPOpen(tcp, sendbuf, sendbufLen, recvbuf, recvbufLen);
    if (rc >= 0) {
        TCPSetTimeout(tcp, R2);
        enabled = OSDisableInterrupts();

        if (TCPGetStatus(listening) == TCP_STATE_LISTEN) {
            tcp->logging = listening;
            IFQueueEnqueueTailLINK(TCPInfo*, &listening->queueBacklog, linkLog, tcp);
            OSRestoreInterrupts(enabled);
            TCPAcceptAsync(tcp, listening, &AcceptCallback, 0);
            OSRestoreInterrupts(enabled);
            return tcp;
        }

        OSRestoreInterrupts(enabled);
    }

    SOFree(2, recvbuf, recvbufLen);
    SOFree(1, sendbuf, sendbufLen);
    SOFree(0, tcp, sizeof(TCPInfo));
    return NULL;
}

int SOListen(int s, int backlog) {
    SONode* node;
    IPInfo* info;
    TCPInfo* listening;
    s32 rc;
    
    if (State != 1) {
        return -39;
    }

    if (backlog < 1) {
        backlog = 1;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            rc = -63;
            break;
        case IP_PROTO_TCP:
            listening = (TCPInfo*)info;
            rc = TCPListen(listening, NULL, NULL, NULL, 0);
            switch (rc) {
                case 0:
                     while (backlog-- > 0) {
                        if (AddBackLog(listening) == NULL) {
                            break;
                        }
                    }
                    break;
                case -7:
                    rc = -42;
                    break;
                case -5:
                default:
                    rc = -28;
                    break;
            }
            break;
        default:
            rc = -8;
            break;
    }

    PutNode(node);
    return rc;
}

int SOAccept(int s, void* sockAddr) {
    BOOL enabled;
    SONode* node;
    IPInfo* info;
    TCPInfo* listening;
    TCPInfo* tcp;
    int socket;
    s32 rc;
    s32 state;
    SONode* connected;

    if (State != 1) {
        return -39;
    }

    if (sockAddr && sizeof(SOSockAddrIn) > ((SOSockAddr*) sockAddr)->len) {
        return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    enabled = OSDisableInterrupts();
    switch (info->proto) {
        case IP_PROTO_UDP:
            rc = -63;
            break;
        case IP_PROTO_TCP:
            listening = (TCPInfo*)info;

        tcp_accept_loop:
            if (TCPGetStatus(listening) != TCP_STATE_LISTEN) {
                rc = -28;
                break;
            }

            listening->accepting++;
            while (TCPGetStatus(listening) == TCP_STATE_LISTEN && listening->queueCompleted.next == NULL) {
                if ((node->flag & 0x4) != 0) {
                    listening->accepting--;
                    rc = -6;
                    goto func_exit;
                }

                OSSleepThread(&listening->queueThread);
            }

            listening->accepting--;
            if (TCPGetStatus(listening) != TCP_STATE_LISTEN) {
                rc = -13;
                break;
            }

            for (socket = 0; socket < SO_TABLE_NUM; socket++) {
                connected = &SocketTable[socket];
                if (connected->ref == 0) {
                    IFQueueDequeueHeadLINK(TCPInfo*, &listening->queueCompleted, linkLog, tcp);
                    ASSERT(tcp);
                    rc = 0;
                    if (sockAddr != NULL) {
                        rc = TCPGetRemoteSocket(tcp, (IPSocket*)sockAddr);
                        
                    }

                    state = TCPGetStatus(tcp);
                    if ((state != 4 && state != 7) || rc < 0) {
                        TCPCancel(tcp);
                        TCPOpen(tcp, tcp->sendData, tcp->sendBuff, tcp->recvData, tcp->recvBuff);
                        TCPSetTimeout(tcp, R2);
                        tcp->logging = listening;
                        IFQueueEnqueueTailLINK(TCPInfo*, &listening->queueBacklog, linkLog, tcp);
                        TCPAcceptAsync(tcp, listening, &AcceptCallback, 0);
                        goto tcp_accept_loop;
                    } else {
                        connected->flag = node->flag;
                        connected->ref = 1;
                        OSInitMutex(&connected->mutexRead);
                        OSInitMutex(&connected->mutexWrite);
                        connected->proto = IP_PROTO_TCP;
                        connected->info = &tcp->pair;
                        rc = socket;
                        OSRestoreInterrupts(enabled);
                        AddBackLog(listening);
                        break;
                    }
                }
            }

            if (socket >= SO_TABLE_NUM) {
                rc = -33;
            }
            break;
        default:
            rc = -8;
            break;
    }

func_exit:
    OSRestoreInterrupts(enabled);
    PutNode(node);
    return rc;
}

int SOBind(int s, void* sockAddr) {
    SONode* node;
    IPInfo* info;
    TCPInfo* tcp;
    UDPInfo* udp;
    s32 rc;

    if (State != 1) {
        return -39;
    }

    ASSERT(sockAddr == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockAddr)->len);
    if (sockAddr == NULL || sizeof(SOSockAddrIn) > ((SOSockAddr*) sockAddr)->len) {
        return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            udp = (UDPInfo*)info;
            rc = UDPBind(udp, (IPSocket*)sockAddr);
            break;
        case IP_PROTO_TCP:
            tcp = (TCPInfo*)info;
            rc = TCPBind(tcp, (IPSocket*)sockAddr);
            break;
        default:
            PutNode(node);
            return -8;
    }

    PutNode(node);
    switch (rc) {
        case 0:
            return 0;
        case -13:
            return -5;
        case -5:
            return -3;
        case -12:
        default:
            return -28;
    }
}

int SOConnect(int s, void* sockAddr) {
    SONode* node;
    IPInfo* info;
    TCPInfo* tcp;
    UDPInfo* udp;
    s32 rc;

    if (State != 1) {
        return -39;
    }

    ASSERT(sockAddr == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockAddr)->len);
    if (sockAddr == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockAddr)->len) {
        return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            udp = (UDPInfo*)info;
            if (((SOSockAddr*) sockAddr)->family == 0) {
                sockAddr = &SockAnyIn;
            }
            rc = UDPConnect(udp, (IPSocket*)sockAddr);
            break;
        case IP_PROTO_TCP:
            tcp = (TCPInfo*)info;
            if ((node->flag & 0x4) == 0) {
                rc = TCPConnect(tcp, (IPSocket*)sockAddr);
            } else {
                rc = TCPConnectAsync(tcp, (IPSocket*)sockAddr, NULL, 0);
                if (rc == 0 && tcp->openCallback != NULL) {
                    rc = -1;
                }
            }
            break;
        default:
            PutNode(node);
            return -8;
    }

    PutNode(node);
    switch (rc) {
        case 0:
            return 0;
        case -1:
            return -26;
        case -13:
            return -5;
        case -5:
            return -30;
        case -3:
            return -15;
        case -11:
            return -14;
        case -10:
            return -76;
        case -12:
            return -28;
        case -7:
            return -42;
        case -19:
            return -38;
        default:
            return -40;
    }
}

int SOGetPeerName(int s, void* sockAddr) {
    SONode* node;
    IPInfo* info;
    TCPInfo* tcp;
    UDPInfo* udp;
    s32 rc;

    if (State != 1) {
        return -39;
    }

    ASSERT(sockAddr);
    ASSERT(sockAddr == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockAddr)->len);
    if (sockAddr == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockAddr)->len) {
        return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            udp = (UDPInfo*)info;
            rc = UDPGetRemoteSocket(udp, (IPSocket*)sockAddr);
            break;
        case IP_PROTO_TCP:
            tcp = (TCPInfo*)info;
            rc = TCPGetRemoteSocket(tcp, (IPSocket*)sockAddr);
            break;
        default:
            PutNode(node);
            return -8;
    }

    PutNode(node);
    if (rc < 0) {
        return -8;
    }

    if (((SOSockAddrIn*) sockAddr)->port == 0) {
        return -56;
    }

    return 0;
}

int SOGetSockName(int s, void* sockAddr) {
    SONode* node;
    IPInfo* info;
    TCPInfo* tcp;
    UDPInfo* udp;
    s32 rc;

    if (State != 1) {
        return -39;
    }

    ASSERT(sockAddr);
    ASSERT(sockAddr == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockAddr)->len);
    if (sockAddr == NULL || sizeof(SOSockAddrIn) <= ((SOSockAddr*) sockAddr)->len) {
        return -28;
    }

    node = GetNode(s, &info);
    if (node == NULL || info == NULL) {
        return -8;
    }

    switch (info->proto) {
        case IP_PROTO_UDP:
            udp = (UDPInfo*)info;
            rc = UDPGetLocalSocket(udp, (IPSocket*)sockAddr);
            break;
        case IP_PROTO_TCP:
            tcp = (TCPInfo*)info;
            rc = TCPGetLocalSocket(tcp, (IPSocket*)sockAddr);
            break;
        default:
            PutNode(node);
            return -8;
    }

    PutNode(node);
    if (rc < 0) {
        return -8;
    }

    return 0;
}
