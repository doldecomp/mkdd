#ifndef __DOLPHIN_OS_IF_QUEUE_H__
#define __DOLPHIN_OS_IF_QUEUE_H__

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IFQueue IFQueue;
typedef struct IFQueue IFLink;

struct IFQueue {
    // total size: 0x8
    IFQueue* next; // offset 0x0, size 0x4
    IFQueue* prev; // offset 0x4, size 0x4
};

#define IFQueueInit(queue) ((queue)->next = (queue)->prev = 0)
#define IFIsEmptyQueue(queue) ((queue)->next == 0)

#define IFQueueIterator(type, queue, iter, n) \
    for ((iter) = (type)((queue)->next), (n) = ((iter) == 0) ? 0 : (type)(iter)->link.next; \
         (iter) != 0; \
         (iter) = (n), (n) = ((iter) == 0) ? 0 : (type)(iter)->link.next)

#define IFQueueReverseIterator(type, queue, iter, p) \
    for ((iter) = (type)((queue)->prev), (p) = ((iter) == 0) ? 0 : (type)(iter)->link.prev; \
         (iter) != 0; \
         (iter) = (p), (p) = ((iter) == 0) ? 0 : (type)(iter)->link.prev)

#define IFQueueDequeueEntry(type, queue, entry) \
do {                                            \
    register IFQueue* ___next;                   \
    register IFQueue* ___prev;                   \
                                                \
    ___next = (entry)->link.next;                \
    ___prev = (entry)->link.prev;                \
                                                \
    if (___next == 0) {                       \
        (queue)->prev = ___prev;                 \
    } else {                                    \
        ((type)___next)->link.prev = ___prev;     \
    }                                           \
    if (___prev == 0){                        \
        (queue)->next = ___next;                 \
    } else {                                    \
        ((type)___prev)->link.next = ___next;     \
    }                                           \
} while (0)

#define IFQueueDequeueHead(type, queue, entry)  \
do {                                            \
    register IFQueue* ___next;                   \
                                                \
    (entry) = (type)(queue)->next;              \
    ___next = ((type)(entry))->link.next;        \
                                                \
    if (___next == 0) {                       \
        (queue)->prev = 0;                   \
    } else {                                    \
        ((type)___next)->link.prev = 0;       \
    }                                           \
    (queue)->next = ___next;                     \
} while (0)

#define IFQueueDequeueTail(type, queue, entry)  \
do {                                            \
    register IFQueue* ___prev;                   \
                                                \
    (entry) = (type)(queue)->prev;              \
    ___prev = ((type)(entry))->link.prev;        \
                                                \
    if (___prev == 0) {                       \
        (queue)->next = 0;                   \
    } else {                                    \
        ((type)___prev)->link.next = 0;       \
    }                                           \
    (queue)->prev = ___prev;                     \
} while (0)

#define IFQueueEnqueueHead(type, queue, entry)          \
do {                                                    \
    register IFQueue* ___next;                           \
                                                        \
    ___next = (queue)->next;                             \
                                                        \
    if (___next == 0) {                               \
        (queue)->prev = (IFQueue*)(entry);              \
    } else {                                            \
        ((type)___next)->link.prev = (IFQueue*)(entry);  \
    }                                                   \
    (entry)->link.next = ___next;                        \
    (entry)->link.prev = 0;                          \
    (queue)->next = (IFQueue*)(entry);                  \
} while (0)

#define IFQueueEnqueueTail(type, queue, entry)          \
do {                                                    \
    register IFQueue* ___prev;                           \
                                                        \
    ___prev = (queue)->prev;                             \
                                                        \
    if (___prev == 0) {                               \
        (queue)->next = (IFQueue*)(entry);              \
    } else {                                            \
        ((type)___prev)->link.next = (IFQueue*)(entry);  \
    }                                                   \
    (entry)->link.prev = ___prev;                        \
    (entry)->link.next = 0;                          \
    (queue)->prev = (IFQueue*)(entry);                  \
} while (0)

/* Macros where you can set the link member name */

#define IFQueueDequeueEntryLINK(type, queue, link, entry) \
do {                                            \
    register IFQueue* ___next;                   \
    register IFQueue* ___prev;                   \
                                                \
    ___next = (entry)->link.next;                \
    ___prev = (entry)->link.prev;                \
                                                \
    if (___next == 0) {                       \
        (queue)->prev = ___prev;                 \
    } else {                                    \
        ((type)___next)->link.prev = ___prev;     \
    }                                           \
    if (___prev == 0){                        \
        (queue)->next = ___next;                 \
    } else {                                    \
        ((type)___prev)->link.next = ___next;     \
    }                                           \
} while (0)

#define IFQueueDequeueHeadLINK(type, queue, link, entry)  \
do {                                            \
    register IFQueue* ___next;                   \
                                                \
    (entry) = (type)(queue)->next;              \
    ___next = ((type)(entry))->link.next;        \
                                                \
    if (___next == 0) {                       \
        (queue)->prev = 0;                   \
    } else {                                    \
        ((type)___next)->link.prev = 0;       \
    }                                           \
    (queue)->next = ___next;                     \
} while (0)

#define IFQueueDequeueTailLINK(type, queue, link, entry)  \
do {                                            \
    register IFQueue* ___prev;                   \
                                                \
    (entry) = (type)(queue)->prev;              \
    ___prev = ((type)(entry))->link.prev;        \
                                                \
    if (___prev == 0) {                       \
        (queue)->next = 0;                   \
    } else {                                    \
        ((type)___prev)->link.next = 0;       \
    }                                           \
    (queue)->prev = ___prev;                     \
} while (0)

#define IFQueueEnqueueHeadLINK(type, queue, link, entry)          \
do {                                                    \
    register IFQueue* ___next;                           \
                                                        \
    ___next = (queue)->next;                             \
                                                        \
    if (___next == 0) {                               \
        (queue)->prev = (IFQueue*)(entry);              \
    } else {                                            \
        ((type)___next)->link.prev = (IFQueue*)(entry);  \
    }                                                   \
    (entry)->link.next = ___next;                        \
    (entry)->link.prev = 0;                          \
    (queue)->next = (IFQueue*)(entry);                  \
} while (0)

#define IFQueueEnqueueTailLINK(type, queue, link, entry)          \
do {                                                    \
    register IFQueue* ___prev;                           \
                                                        \
    ___prev = (queue)->prev;                             \
                                                        \
    if (___prev == 0) {                               \
        (queue)->next = (IFQueue*)(entry);              \
    } else {                                            \
        ((type)___prev)->link.next = (IFQueue*)(entry);  \
    }                                                   \
    (entry)->link.prev = ___prev;                        \
    (entry)->link.next = 0;                          \
    (queue)->prev = (IFQueue*)(entry);                  \
} while (0)

#ifdef __cplusplus
}
#endif

#endif
