#include "dolphin/os.h"
#include <dolphin/private/ip.h>

// Range: 0x0 -> 0x10C
u8* IFRingIn(u8* buf /* r23 */, s32 size /* r24 */, u8* head /* r30 */, s32 used /* r22 */, const u8* data /* r26 */, s32 len /* r28 */) {
    // Local variables
    u8* end; // r25
    u8* tail; // r31
    s32 free; // r27

    ASSERT(used + len <= size);
    end = buf + size;
    ASSERT(buf <= head && head < end);
    tail = head + used;
    if (end <= tail) {
        tail -= size;
    }

    if (head <= tail) {
        free = (s32)end - (s32)tail;
        if (len <= free) {
            memmove(tail, data, len);
            return head;
        } else {
            memmove(tail, data, free);
            data += free;
            len -= free;
            memmove(buf, data, len);
            return head;
        }
    } else {
        memmove(tail, data, len);
        return head;
    }
}

// Range: 0x10C -> 0x238
u8* IFRingOut(u8* buf /* r25 */, s32 size /* r1+0xC */, u8* head /* r31 */, s32 used /* r1+0x14 */, u8* data /* r26 */, s32 len /* r29 */) {
    // Local variables
    u8* end; // r28
    s32 front; // r27

    ASSERT(len <= used);
    end = buf + size;
    ASSERT(buf <= head && head < end);

    if (head + len < end) {
        memmove(data, head, len);
        head += len;
    } else {
        front = (s32)end - (s32)head;
        ASSERT(front <= len);
        memmove(data, head, front);
        data += front;
        len -= front;
        head = buf;
        memmove(data, head, len);
        head += len;
    }

    ASSERT(buf <= head && head < end);
    return head;
}

// Range: 0x238 -> 0x328
int IFRingGet(u8* buf /* r25 */, s32 size /* r24 */, u8* head /* r31 */, s32 used /* r1+0x14 */, IFVec* vec /* r30 */, s32 len /* r27 */) {
    // Local variables
    u8* end; // r28
    s32 front; // r26

    ASSERT(len <= used);
    end = buf + size;
    if (end <= head) {
        head -= size;
    }
    ASSERT(buf <= head && head < end);

    if (head + len <= end) {
        vec->data = head;
        vec->len = len;
        return 1; // one entry
    } else {
        front = (s32)end - (s32)head;
        ASSERT(front < len);
        vec->data = head;
        vec->len = front;
        vec++;
        vec->data = buf;
        vec->len = len - front;
        return 2; // two entries
    }
}

// Range: 0x328 -> 0x3FC
u8* IFRingPut(u8* buf /* r27 */, s32 size /* r1+0xC */, u8* head /* r31 */, s32 used /* r1+0x14 */, s32 len /* r28 */) {
    // Local variables
    u8* end; // r29

    ASSERT(len <= used);
    end = buf + size;
    ASSERT(buf <= head && head < end);
    if (head + len < end) {
        head += len;
    } else {
        head = (buf + len) - (end - head);
    }

    ASSERT(buf <= head && head < end);
    return head;
}

// Range: 0x3FC -> 0x650
static s32 MargeBlock(u8* ptr /* r23 */, s32 len /* r24 */, IFBlock* blockTable /* r25 */, s32 maxblock /* r20 */, s32 size /* r21 */, u8* tail /* r27 */) {
    // Local variables
    IFBlock* block; // r31
    IFBlock* end; // r28
    s32 pl; // r26
    s32 pr; // r30
    s32 pb; // r29

    ASSERT(1 < maxblock && blockTable);
    ASSERT(0 <= len);

    pl = tail <= ptr ? (s32)ptr - (s32)tail : (s32)ptr + size - (s32)tail;
    pr = pl + len;
    end = blockTable + maxblock;
    if (tail == ptr) {
        block = blockTable;

        while (block < end && block->ptr) {
            pb = tail <= block->ptr ? (s32)block->ptr - (s32)tail : (s32)block->ptr + size - (s32)tail;
            if (pb <= pr) {
                pr = pb + block->len > pr ? pb + block->len : pr;
                len = pr - pl;
                memmove(block, block + 1, (s32)end - (s32)(block + 1));
                memset(end - 1, 0, sizeof(IFBlock));
            } else {
                block++;
            }
        }

        return len;
    } else {
        block = blockTable;
        while (block < end && block->ptr) {
            pb = tail <= block->ptr ? (s32)block->ptr - (s32)tail : (s32)block->ptr + size - (s32)tail;
            if (pl <= pb + block->len && pb <= pr) {
                pr = pb + block->len > pr ? pb + block->len : pr;

                if (pb < pl) {
                    pl = pb;
                    ptr = block->ptr;
                }

                len = pr - pl;
                memmove(block, block + 1, (s32)end - (s32)(block + 1));
                memset(end - 1, 0, sizeof(IFBlock));
            } else {
                block++;
            }
        }

        if (block < end) {
            ASSERT(block->ptr == NULL);
            block->ptr = ptr;
            block->len = len;
        } else {
            memmove(blockTable, blockTable + 1, (s32)end - (s32)(blockTable + 1));
            block = end - 1;
            block->ptr = ptr;
            block->len = len;
        }

        return 0;
    }
}

// Range: 0x650 -> 0x7D4
u8* IFRingInEx(u8* buf /* r21 */, s32 size /* r27 */, u8* head /* r28 */, s32 used /* r19 */, s32 offset /* r22 */, const u8* data /* r23 */, s32 * adv /* r20 */, IFBlock* blockTable /* r1+0x24 */, s32 maxblock /* r1+0x68 */) {
    // Local variables
    u8* end; // r26
    u8* tail; // r25
    u8* ptr; // r31
    s32 len; // r29
    s32 free; // r24

    len = *adv;
    if (len == 0) {
        return head;
    }

    ASSERT(0 <= offset);
    ASSERT(used + offset + len <= size);
    end = buf + size;
    ASSERT(buf <= head && head < end);
    tail = head + used;
    if (end <= tail) {
        tail -= size;
    }

    ASSERT(offset < size);
    ptr = tail + offset;
    if (end <= ptr) {
        ptr -= size;
    }

    if (head <= ptr) {
        free = (s32)end - (s32)ptr;
        if (len <= free) {
            memmove(ptr, data, len);
        } else {
            memmove(ptr, data, free);
            memmove(buf, data + free, len - free);
        }
    } else {
        memmove(ptr, data, len);
    }

    *adv = MargeBlock(ptr, len, blockTable, maxblock, size, tail);
    return head;
}
