#ifndef GO3212D_H
#define GO3212D_H

#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JORReflexible.h"
#include "Kaneshige/HioMgr.h"

class Go3212DParam : public JORReflexible
{
public:
    Go3212DParam();
    virtual ~Go3212DParam() {}

    u16 mUnknown;
    bool mFlag;
};

class Go3212DHioNode : public HioNode
{
public:
    Go3212DHioNode() : HioNode("Ｇｏ３２１２Ｄ", &mParam, 0, 0) {}
    virtual ~Go3212DHioNode() {}

    Go3212DParam mParam;
};

class Go3212D
{
public:
    Go3212D(JKRHeap *heap);  // 0x80183460
    ~Go3212D();              // UNUSED

    void init();  // 0x801835f8
    void draw();  // 0x80183680
    void calc();  // 0x8018372c

    void reset() { init(); }
    bool isStart() { return mIsStart; }
    void start() { mIsStart = true; }

private:
    J2DAnmTransform *mTransform;  // 0x0
    void *_4;                     // 0x4
    f32 mTransformFrame;          // 0x8

    J2DAnmTexPattern *mTexPattern;  // 0xc
    void *_10;                      // 0x10
    f32 mTexPatternFrame;           // 0x14

    Go3212DHioNode *mHioNode;  // 0x18

    J2DGrafContext *mGrafContext;  // 0x1c
    J2DScreen *mScreen;            // 0x20

    bool mIsStart;  // 0x24
};

#endif  // GO3212D_H
