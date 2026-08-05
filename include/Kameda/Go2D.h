#ifndef GO2D_H
#define GO2D_H

#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JORReflexible.h"
#include "Kaneshige/HioMgr.h"

class Go2DParam : public JORReflexible
{
public:
    Go2DParam();
    virtual ~Go2DParam() {}

    u16 mUnknown;
    bool mFlag;
};

class Go2DHioNode : public HioNode
{
public:
    Go2DHioNode() : HioNode("Ｇｏ２Ｄ", &mParam, 0, 0) {}
    virtual ~Go2DHioNode() {}

    Go2DParam mParam;
};

class Go2D
{
public:
    Go2D(JKRHeap *heap);  // 0x8016ea58
    ~Go2D();              // UNUSED

    void init();  // 0x8016ebec
    void draw();  // 0x8016ecac
    void calc();  // 0x8016ed58

    void reset() { init(); }
    void setDrawFlag(bool enable) { mDrawFlag = enable; }

private:
    J2DAnmTransform *mTransform;  // 0x0
    void *_4;                     // 0x4
    f32 mTransformFrame;          // 0x8

    J2DAnmTevRegKey *mTevRegKey;  // 0xc
    void *_10;                    // 0x10
    f32 mTevRegKeyFrame;          // 0x14

    u32 _18;  // 0x18
    u32 _1c;  // 0x1c
    u32 _20;  // 0x20

    Go2DHioNode *mHioNode;  // 0x24

    J2DGrafContext *mGrafContext;  // 0x28
    J2DScreen *mScreen;            // 0x2c

    bool mDrawFlag;  // 0x30
};

#endif  // GO2D_H
