#include "dolphin/os.h"
#include <dolphin/private/ip.h>

#ifdef DEBUG
#define IFFifoMemset(p, c, n) memset(p, c, n)
#else
#define IFFifoMemset(p, c, n)
#endif

// Range: 0x0 -> 0x98
void IFFifoInit(IFFifo* fifo /* r29 */, void* buff /* r30 */, s32 size /* r31 */) {
    ASSERT(size == 0 || (buff && 0 < size));
    fifo->size = size;
    fifo->buff = buff;
    fifo->head = buff;
    fifo->used = 0;

#ifdef DEBUG
    if (buff && size > 0) {
        IFFifoMemset(buff, 0xFF, size);
    }
#endif
}

// Range: 0x98 -> 0x24C
static void* FifoAlloc(IFFifo* fifo /* r31 */, s32 len /* r28 */) {
    // Local variables
    s32 free; // r26
    u8* tail; // r29
    u8* end; // r27

    end = fifo->buff + fifo->size;
    ASSERT(0 < len);
    ASSERT(0 <= fifo->used && fifo->used <= fifo->size);
    ASSERT(fifo->buff <= fifo->head && fifo->head <= end);

    if (fifo->size - fifo->used < len) {
        return NULL;
    }

    tail = fifo->head + fifo->used;
    if (end <= tail) {
        tail -= fifo->size;
    }

    if (fifo->head == tail) {
        fifo->head = fifo->buff;
        fifo->used = len;
        return fifo->buff;
    } else if (fifo->head < tail) {
        free = (s32)(end - tail);
        if (len <= free) {
            fifo->used += len;
            return tail;
        } else if (len <= (s32)(fifo->head - fifo->buff)) {
            fifo->used += free + len;
            return fifo->buff;
        }
    } else {
        ASSERT(fifo->head - tail == fifo->size - fifo->used);
        fifo->used += len;
        return tail;
    }

    return NULL;
}

// Range: 0x24C -> 0x2D0
void * IFFifoAlloc(IFFifo* fifo /* r30 */, s32 len /* r29 */) {
    // Local variables
    void* ptr; // r31

    ptr = FifoAlloc(fifo, len);
    if (ptr) {
        IFFifoMemset(ptr, 0xA3, len);
    }

    ASSERT(fifo->used <= fifo->size);
    return ptr;
}

// Range: 0x2D0 -> 0x648
BOOL IFFifoFree(IFFifo* fifo /* r31 */, void* ptr /* r1+0xC */, s32 len /* r26 */) {
    // Local variables
    u8* p; // r29
    u8* end; // r27
    u8* head; // r30
    u8* tail; // r28

    p = (u8*)ptr;
    end = fifo->buff + fifo->size;
    ASSERT(0 <= fifo->used && fifo->used <= fifo->size);
    ASSERT(fifo->buff <= fifo->head && fifo->head < end);

    if (len <= 0 || fifo->used < len || p == NULL || p < fifo->buff || end <= p) {
        return FALSE;
    }

    tail = fifo->head + fifo->used;
    if (end <= tail) {
        tail -= fifo->size;
    }

    head = p + len;
    if (end <= head) {
        head -= fifo->size;
    }

    if (head < fifo->buff || end <= head) {
        return FALSE;
    }

    if (fifo->head == tail) {
        if (fifo->head < head) {
            if (head <= p || p < fifo->head) {
                return FALSE;
            }

            IFFifoMemset(fifo->head, -0xA4, (size_t)(head - fifo->head));
        } else {
            if (head <= p && p < fifo->head) {
                return FALSE;
            }

            if (head == tail) {
                IFFifoMemset(p, -0xA4, len);
                fifo->used -= len;
                return TRUE;
            }

            IFFifoMemset(fifo->head, -0xA4, (size_t)(end - fifo->head));
            IFFifoMemset(fifo->buff, -0xA4, (size_t)(head - fifo->buff));
        }
    } else if (fifo->head < tail) {
        if (head <= p || p < fifo->head || tail < head) {
            return FALSE;
        }

        if (head == tail) {
            IFFifoMemset(p, -0xA4, len);
            fifo->used -= len;
            return TRUE;
        }

        IFFifoMemset(fifo->head, -0xA4, (size_t)(head - fifo->head));
    } else if (fifo->head < head) {
        if (head <= p || p < fifo->head) {
            return FALSE;
        }

        IFFifoMemset(fifo->head, -0xA4, (size_t)(head - fifo->head));
    } else {
        if (tail < head) {
            return FALSE;
        }

        if (head <= p && p < fifo->head) {
            return FALSE;
        }

        if (head == tail) {
            IFFifoMemset(p, -0xA4, len);
            fifo->used -= len;
            return TRUE;
        }

        IFFifoMemset(fifo->head, -0xA4, (size_t)(end - fifo->head));
        IFFifoMemset(fifo->buff, -0xA4, (size_t)(head - fifo->buff));
    }

    if (head <= tail) {
        fifo->used = (s32)tail - (s32)head;
    } else {
        fifo->used = ((s32)end - (s32)head) + ((s32)tail - (s32)fifo->buff);
    }

    ASSERT(fifo->used < fifo->size);
    fifo->head = head;
    return TRUE;
}

// Range: 0x648 -> 0x748
void IFDump(void* ptr /* r28 */, s32 len /* r27 */) {
    // Local variables
    int j; // r30
    int i; // r31
    int n; // r29

    for (j = 0; j < len; j += 16) {
        n = len - j;
        if (n > 16) {
            n = 16;
        }

        OSReport("%08x: ", j);
        for (i = 0; i < n; i++) {
            OSReport("%02x ", ((u8*)ptr)[j + i]);
        }

        for (i; i < 16; i++) {
            OSReport("   ");
        }

        OSReport("  ");
        for (i = 0; i < n; i++) {
            OSReport("%c", isprint(((u8*)ptr)[j + i]) ? ((u8*)ptr)[j + i] : '.');
        }

        OSReport("\n");
    }
}
