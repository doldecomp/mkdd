#ifndef JKRASSERTHEAP_H
#define JKRASSERTHEAP_H

#include <JSystem/JKernel/JKRHeap.h>
#include <JSystem/JUtility/JUTDbg.h>
#include "types.h"

class JKRAssertHeap : public JKRHeap
{
protected:
    JKRAssertHeap(void *, unsigned long, JKRHeap *, bool);
    
    // Thank you Nintendo for ordering this differently than the base class
public:
    virtual ~JKRAssertHeap();                                         // _08
    virtual u32 getHeapType() { return 'ASTH'; }                      // _10
    virtual bool check() { return true; };                            // _14
    virtual bool dump() { return true; }                              // _1C
    virtual bool dump_sort() { return true; }                         // _18
    virtual void do_destroy();                                        // _20
    virtual u8 do_changeGroupID(u8)                                   // _4C
    {
#line 41
        JUT_ASSERT_MSG(0, "0&&\"illegal changeGroupID()\"");
        return 0;
    }
    virtual u8 do_getCurrentGroupId() { return 0; } // _50
    virtual void *do_alloc(u32, int)                // _24
    {
        JUT_ASSERT_MSG(0, "0&&\"illegal alloc\"");
        return nullptr;
    };
                                                                                       
    virtual void do_free(void *) { JUT_ASSERT_MSG(0, "0&&\"illegal free\""); }          // _28
    
    virtual void do_freeAll() { JUT_ASSERT_MSG(0, "0&&\"illegal freeAll()\""); }        // _2C
    
    virtual void do_freeTail() { JUT_ASSERT_MSG(0, "0&&\"illegal freeTail()\""); };     // _30
    virtual void do_fillFreeArea(){};                                                   // _34

#line 61
    virtual int do_resize(void *, u32) { JUT_ASSERT_MSG(0, "0&&\"illegal resize\"") return 0; }; // _38
    virtual int do_getSize(void *) { return 0; };                                                // _3C
    virtual u32 do_getFreeSize() { return 0; };                                                  // _40
    virtual void *do_getMaxFreeBlock() { return 0; };                                            // _44
    virtual u32 do_getTotalFreeSize() { return 0; };                                             // _48

public:
    static JKRAssertHeap *create(JKRHeap *);
};

#endif