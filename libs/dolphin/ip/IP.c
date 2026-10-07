#include <dolphin/private/ip.h>
#include "types.h"
#include "stdio.h"

static u16 Id = 1;
const u8 IPAddrAny[4] = { 0, 0, 0, 0 }; // 0.0.0.0
const u8 IPLoopbackAddr[4] = { 127, 0, 0, 1 }; // 127.0.0.1
const u8 IPLimited[4] = { 255, 255, 255, 255 }; // 255.255.255.255

void IPPrintAddr(u8* addr) {
    OSReport("%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
}

char* IPAtoN(const char* dotted, u8* addr) {
    int c;
    u32 part[4];
    int n;
    u32 val;
    int base;

    n = 0;
    c = *dotted;
    while (TRUE) {
        if (isdigit(c) == FALSE) {
            return NULL;
        }

        base = 10;
        if (c == '0') {
            c = *(++dotted);
            
            if (c == 'x' || c == 'X') {
                base = 16;
                c = *(++dotted);
            } else {
                base = 8;
            }
        }

        val = 0;
        while (TRUE) {
            if (isdigit(c)) {
                val *= base;
                val = c + val;
                val -= '0';
            } else {
                if (base != 16 || isxdigit(c) == FALSE) {
                    break;
                }

                val *= 16;
                val += ((c + 10) - (islower(c) ? 'a' : 'A'));
            }

            c = *(++dotted);
        }

        part[n++] = val;
        if (c == '.') {
            if (n >= 4 || val > 255) {
                return NULL; /* bad IP string */
            }

            c = *(++dotted);
        } else if (c > '\x7F') {
            return NULL; /* character out of standard ASCII range */
        } else {
            switch (n) {
                case 1:
                    addr[0] = (u8)(val >> 24);
                    addr[1] = (u8)(val >> 16);
                    addr[2] = (u8)(val >>  8);
                    addr[3] = (u8)(val >>  0);
                    break;
                case 2:
                    if (0x00FFFFFF < val) {
                        return NULL;
                    }

                    addr[0] = (u8)part[0];
                    addr[1] = (u8)(val >> 16);
                    addr[2] = (u8)(val >>  8);
                    addr[3] = (u8)(val >>  0);
                    break;
                case 3:
                    if (val > 0x0000FFFF) {
                        return NULL;
                    }

                    addr[0] = (u8)part[0];
                    addr[1] = (u8)part[1];
                    addr[2] = (u8)(val >>  8);
                    addr[3] = (u8)(val >>  0);
                    break;
                case 4:
                    addr[0] = (u8)part[0];
                    addr[1] = (u8)part[1];
                    addr[2] = (u8)part[2];
                    addr[3] = (u8)val;
                break;
                default:
                    return NULL;
            }

            return (char*)dotted;
        }
    }
}

char* IPNtoA(const u8* addr) {
    static char ascii[16];

    sprintf(ascii, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
    return ascii;
}

IPInfo* IPLookupInfo(IFQueue* queue, u8* srcAddr, u8* dstAddr, u16 src, u16 dst, u32 flag) {
    IPInfo* info;
    IPInfo* next;
    int wildcard;
    int minimum;
    IPInfo* match;
    s32 mcast;

    minimum = 3;
    match = NULL;

    if (IP_CLASSD(dstAddr)) {
        mcast = IPMulticastLookup(dstAddr, IPAddrAny);

        if (mcast < 0) {
            return NULL;
        }
    }

    IFQueueIterator(IPInfo*, queue, info, next) {
        if (
            (info->local.port != 0 && info->local.port == dst) && (!IP_CLASSD(dstAddr) ||
            ((info->flag & (1 << mcast)) != 0 && (((flag & 0x4) == 0) || ((info->flag & 0x8000) != 0))))
        ) {
            wildcard = 0;
            if (IPNEQ(dstAddr, IPAddrAny)) {
                if (IPEQ(info->local.addr, IPAddrAny)) {
                    wildcard++;
                } else if (IPNEQ(info->local.addr, dstAddr)) {
                    continue;
                }
            } else if (IPNEQ(info->local.addr, IPAddrAny)) {
                wildcard++;
            }
            
            if (IPNEQ(srcAddr, IPAddrAny)) {
                if (IPEQ(info->remote.addr, IPAddrAny)) {
                    wildcard++;
                } else if (info->remote.port != src || IPNEQ(info->remote.addr, srcAddr)) {
                    continue;
                }
            } else if (IPNEQ(info->remote.addr, IPAddrAny)) {
                wildcard++;
            }
            
            if (wildcard < minimum) {
                match = info;
                minimum = wildcard;

                if (minimum == 0) {
                    break;
                }
            }
        }
    }

    return match;
}

BOOL __IPIsMember(IFQueue* queue, IPInfo* info) {
    IPInfo* iter;
    IPInfo* next;

    IFQueueIterator(IPInfo*, queue, iter, next) {
        if (iter == info) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL IPBind(IFQueue* queue, IPInfo* info, const IPSocket* socket, BOOL reuse) {
    IPInfo* iter;
    IPInfo* next;

    if (socket->len != 8 || socket->family != 2 || socket->port == 0 || IP_CLASSE(socket->addr)) {
        return -12;
    }

    if (socket->port == 0 || (IPNEQ(socket->addr, IPAddrAny) && IPNEQ(socket->addr, IPLoopbackAddr) &&
        IPNEQ(socket->addr, __IFDefault.addr) && IPNEQ(socket->addr, __IFDefault.alias) && !IP_CLASSD(socket->addr))) {
        return -13;
    }

    IFQueueIterator(IPInfo*, queue, iter, next) {
        if (iter != info && iter->local.port == socket->port && IPEQ(iter->local.addr, socket->addr) &&
            (reuse == FALSE || (iter->remote.port == info->remote.port && IPEQ(iter->remote.addr, info->remote.addr)))
        ) {
            return -5;
        }
    }

    memmove(&info->local, socket, sizeof(info->local));
    return 0;
}

u16 IPGetAnonPort(IFQueue* queue, u16* last) {
    u16 port;
    IPInfo* info;
    IPInfo* next;
    int skip;

    skip = 0;
    if (*last < 0x400 || *last > 0x1388) {
        *last = 0x400;
    }

loop:
    port = *last;
    *last = port + 1;
    if (*last > 0x1388) {
        *last = 0x400;
    }

    IFQueueIterator(IPInfo*, queue, info, next) {
        if (info->local.port == port) {
            if (++skip <= 0xf88) {
                goto loop;
            }

            return 0;
        }
    }

    return port;
}

s32 IPConnect(IFQueue* queue, IPInfo* info, const IPSocket* socket, u16* last) {
    IPInfo* iter;
    IPInfo* next;
    IPInterface* interface;
    const u8* localAddr;

    if (socket == NULL || socket->len != 8 || socket->family != 2 || socket->port == 0 || IP_CLASSE(socket->addr)) {
        return -12;
    }

    if (IPEQ(socket->addr, IPAddrAny)) {
        return -13;
    }

    interface = IPGetRoute(socket->addr, NULL);
    if (interface == NULL) {
        return -2;
    }

    if (info->proto == 6 && (IP_CLASSD(socket->addr) || IPIsBroadcastAddr(interface, socket->addr))) {
        return -13;
    }


    if (IPEQ(info->local.addr, IPAddrAny)) {
        if (socket->addr[0] == 127) {
            localAddr = IPLoopbackAddr;
        } else if (memcmp(socket->addr, interface->alias, 2) == 0 || IPEQ(interface->addr, IPAddrAny)) {
            localAddr = interface->alias;
        } else {
            localAddr = interface->addr;
        }
    } else {
        localAddr = info->local.addr;
    }

    if (info->local.port == 0) {
        do {
            info->local.port = IPGetAnonPort(queue, last);
            if (info->local.port == 0) {
                return -7;
            }
        } while (info->proto == IP_PROTO_TCP && TCPLookupTimeWaitInfo(socket->addr, socket->port, localAddr, info->local.port));
    } else {
        IFQueueIterator(IPInfo*, queue, iter, next) {
            if (iter != info && iter->local.port == info->local.port && iter->remote.port == info->remote.port &&
                IPEQ(iter->local.addr, localAddr) && IPEQ(iter->remote.addr, info->remote.addr)) {
                return -5;
            }
        }

        if (info->proto == IP_PROTO_TCP && TCPLookupTimeWaitInfo(socket->addr, socket->port, localAddr, info->local.port)) {
            return -5;
        }
    }

    memmove(&info->remote, socket, sizeof(info->remote));
    if (IPEQ(info->local.addr, IPAddrAny)) {
        memmove(info->local.addr, localAddr, sizeof(info->local.addr));
    }

    return 0;
}

s32 IPGetRemoteSocket(IPInfo* info, IPSocket* socket) {
    ASSERT(info->remote.len == IP_SOCKLEN);
    ASSERT(info->remote.family == IP_INET);
    memcpy(socket, &info->remote, sizeof(info->remote));
    return 0;
}

s32 IPGetLocalSocket(IPInfo* info, IPSocket* socket) {
    ASSERT(info->local.len == IP_SOCKLEN);
    ASSERT(info->local.family == IP_INET);
    memcpy(socket, &info->local, sizeof(info->local));
    return 0;
}

s32 IPGetSockOpt(IPInfo* info, int level, int optname, void* optval, int* optlen) {
    BOOL enabled;
    s32 rc;

    rc = -14;
    enabled = OSDisableInterrupts();
    if (level == 0) {
        switch (optname) {
            case IP_OPT_TOS:
                if (*(u32*)optlen >= sizeof(u32)) {
                    *(u32*)optval = info->tos;
                    *optlen = sizeof(u32);
                    rc = 0;
                } else {
                    rc = -12;
                }
                break;
            case IP_OPT_TTL:
                if (*(u32*)optlen >= sizeof(u32)) {
                    *(u32*)optval = info->ttl;
                    *optlen = sizeof(u32);
                    rc = 0;
                } else {
                    rc = -12;
                }
                break;
            case IP_OPT_9:
                if (*(u32*)optlen >= sizeof(u8)) {
                    *(u8*)optval = (info->flag & 0x8000) ? TRUE : FALSE;
                    *optlen = sizeof(u8);
                    rc = 0;
                } else {
                    rc = -12;
                }
                break;
            case IP_OPT_MTTL:
                if (*(u32*)optlen >= sizeof(info->mttl)) {
                    *(u8*)optval = info->mttl;
                    *optlen = sizeof(info->mttl);
                    rc = 0;
                } else {
                    rc = -12;
                }
                break;
        }
    }

    OSRestoreInterrupts(enabled);
    return rc;
}

s32 IPSetSockOpt(IPInfo* info, int level, int optname, void* optval, int optlen) {
    BOOL enabled;
    s32 rc;

    rc = -14;
    enabled = OSDisableInterrupts();

    if (level == 0) {
        switch (optname) {
            case IP_OPT_TOS:
                if (optlen >= sizeof(u32)) {
                    info->tos = *(s32*)optval < 255 ? *(u32*)optval : 255;
                    rc = 0;
                } else {
                    rc = -12;
                }
                break;
            case IP_OPT_TTL:
                if (optlen >= sizeof(u32)) {
                    info->ttl = *(s32*)optval < 255 ? *(u32*)optval : 255;
                    rc = 0;
                } else {
                    rc = -12;
                }
                break;
            case IP_OPT_9:
                if (optlen >= sizeof(u8)) {
                    if (*(u8*)optval) {
                        info->flag |= 0x8000;
                    } else {
                        info->flag &= ~0x8000;
                    }

                    rc = 0;
                } else {
                    rc = -12;
                }
                break;
            case IP_OPT_MTTL:
                if (optlen >= sizeof(info->mttl)) {
                    info->mttl = *(u8*)optval;
                    rc = 0;
                } else {
                    rc = -12;
                }
                break;
            case IP_OPT_JOIN_MCAST:
                if (optlen >= sizeof(SOIpMreq)) {
                    const SOIpMreq* mreq = (const SOIpMreq*)optval;

                    rc = IPMulticastLookup((u8*)&mreq->multiaddr.addr, (u8*)&mreq->interface.addr);
                    if (0 <= rc) {
                        if ((info->flag & (1 << rc)) == 0) {
                            rc = IPMulticastJoin((u8*)&mreq->multiaddr.addr, (u8*)&mreq->interface.addr);
                            ASSERT(0 <= rc);
                            info->flag |= 1 << rc;
                        }

                        rc = 0;
                    } else {
                        rc = IPMulticastJoin((u8*)&mreq->multiaddr.addr,(u8*)&mreq->interface.addr);
                        if (0 <= rc) {
                            info->flag |= 1 << rc;
                            rc = 0;
                        }
                    }
                } else {
                    rc = -12;
                }
                break;
            case IP_OPT_LEAVE_MCAST:
                if (optlen >= sizeof(SOIpMreq)) {
                    const SOIpMreq* mreq = (const SOIpMreq*)optval;

                    rc = IPMulticastLookup((u8*)&mreq->multiaddr.addr, (u8*)&mreq->interface.addr);
                    if (0 <= rc) {
                        if ((info->flag & (1 << rc)) != 0) {
                            rc = IPMulticastLeave((u8*)&mreq->multiaddr.addr, (u8*)&mreq->interface.addr);
                            ASSERT(0 <= rc);
                            info->flag &= ~(1 << rc);
                            rc = 0;
                        } else {
                            rc = -4;
                        }
                    }
                } else {
                    rc = -12;
                }
                break;
        }
    }

    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL IPSetOption(IPInfo* info, u8 ttl, u8 tos) {
    info->ttl = ttl;
    info->tos = tos;
    return TRUE;
}

u16 IPCheckSum(IPHeader* ip) {
    int len;
    u32 sum;
    u16* p;

    sum = 0;
    len = IP_HLEN(ip);
    p = (u16*)ip;
    
    for (; len > 0; len -= sizeof(u16)) {
        sum += *p++;
    }

    /* Add the 16-bit carry values twice and return the inverse */
    sum = (sum & 0xFFFF) + ((sum >> 16) & 0xFFFF);
    sum = (sum & 0xFFFF) + ((sum >> 16) & 0xFFFF);
    return sum ^ 0xFFFF;
}

void IPIn(IPInterface* interface, IPHeader* ip, s32 len, u32 flag) {
    BOOL bcast;

    bcast = IPIsBroadcastAddr(interface, ip->dst);
    if (len < 20 || len < ip->len) {
        return;
    }

    if ((flag & 1) != 0 && !bcast) {
        return;
    }

    if ((flag & 2) != 0 && !IP_CLASSD(ip->dst)) {
        return;
    }

    if ((ip->verlen >> 4) != 4 || ip->len < IP_HLEN(ip)) {
        return;
    }

    if (IPCheckSum(ip) != 0) {
        return;
    }

    if (IP_CLASSD(ip->src) || (IPNEQ(ip->src, IPAddrAny) && IPIsBroadcastAddr(interface, ip->src))) {
        return;
    }

    if (IP_CLASSE(ip->dst) || IP_CLASSE(ip->src)) {
        return;
    }

    if (IPProcessSourceRoute(ip) < 0) {
        return;
    }

    if (IPNEQ(interface->addr, IPAddrAny) && !IP_CLASSD(ip->dst) && !bcast && ip->dst[0] != 127 &&
        IPNEQ(ip->dst, interface->addr) && IPNEQ(ip->dst, interface->alias)) {
        return;
    }

    if ((ip->frag & IP_HAS_FRAG) != 0 || IP_FRAG(ip) != 0) {
        ip = IPReassemble(interface, ip, flag);
        if (ip == NULL) {
            return;
        }
    }
    
    switch (ip->proto) {
        case IP_PROTO_ICMP:
            ICMPIn(interface, ip, flag);
            break;
        case IP_PROTO_IGMP:
            IGMPIn(interface, ip, flag);
            break;
        case IP_PROTO_UDP:
            UDPIn(interface, ip, flag);
            break;
        case IP_PROTO_TCP:
            if ((flag & 0x3) == 0) {
                TCPIn(interface, ip, flag);
            }
            break;
    }

    ip->verlen = 0;
}

s32 IPOut(IFDatagram* datagram) {
    IPHeader* ip;
    IPInterface* interface;
    TCPHeader* tcp;
    UDPHeader* udp;
    IGMP* igmp;

    ASSERT(0 < datagram->nVec && datagram->nVec <= IF_MAX_VEC);
    ip = (IPHeader*)datagram->vec[0].data;
    ASSERT(IP_HLEN(ip) <= datagram->vec[0].len);

    ip->id = Id++;
    if (IP_CLASSD(ip->dst)) {
        interface = &__IFDefault;
        memmove(datagram->dst, ip->dst, sizeof(datagram->dst));
    } else {
        interface = (IPInterface*)IPGetRoute(ip->dst, datagram->dst);
        if (interface == NULL) {
            return -2;
        }
    }

    if (interface->mtu < ip->len) {
        return -17;
    }

    if (ip->src[0] != 127 && IPNEQ(interface->addr, ip->src) && IPNEQ(interface->alias, ip->src)) {
        return -19;
    }


    ip->sum = 0;
    ip->sum = IPCheckSum(ip);

    switch (ip->proto) {
        case IP_PROTO_IGMP:
            igmp = (IGMP*)(((u8*)ip) + IP_HLEN(ip));
            igmp->sum = 0;
            igmp->sum = IGMPCheckSum(igmp);
            break;
        case IP_PROTO_UDP:
            udp = (UDPHeader*)(((u8*)ip) + IP_HLEN(ip));
            udp->sum = 0;
            udp->sum = UDPCheckSum(datagram->vec, datagram->nVec);
            if (udp->sum == 0) {
                udp->sum = 0xFFFF;
            }
            break;
        case IP_PROTO_TCP:
            tcp = (TCPHeader*)(((u8*)ip) + IP_HLEN(ip));
            tcp->sum = 0;
            tcp->sum = TCPCheckSum(datagram->vec, datagram->nVec);
            ASSERT((tcp->flag & (TCP_FLAG_SYN | TCP_FLAG_FIN)) != (TCP_FLAG_SYN | TCP_FLAG_FIN));
            break;
    }

    datagram->type = ETH_IP;
    (*interface->out)(interface, datagram);
    return 0;
}

void IPCancel(IFDatagram* datagram) {
    IPInterface* interface;

    interface = datagram->interface;
    if (interface) {
        (*interface->cancel)(interface, datagram);
        ASSERT(datagram->interface == NULL);
    }
}

void IFInitDatagram(IFDatagram* datagram, u16 type, int nVec) {
    datagram->interface = NULL;
    datagram->queue = NULL;
    datagram->type = type;
    //datagram->offset = 0;
    datagram->prefixLen = 0;
    datagram->flag = 0;
    datagram->callback = NULL;
    datagram->param = NULL;
    datagram->nVec = nVec;
    memset(datagram->vec, 0, nVec * sizeof(IFVec));
}
