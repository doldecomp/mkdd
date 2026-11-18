#ifndef LANNUM2D_H
#define LANNUM2D_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JORReflexible.h"
#include "Kaneshige/HioMgr.h"

class LANNum2DParam : public JORReflexible
{
public:
    LANNum2DParam();
    virtual ~LANNum2DParam() {}

    u16 _48;   // 0x48
    u8 mFlag;  // 0x4a
};

class LANNum2DHioNode : public HioNode
{
public:
    LANNum2DHioNode() : HioNode("ＬＡＮＮｕｍ２Ｄ", &mParam, 0, 0) {}
    virtual ~LANNum2DHioNode() {}

    LANNum2DParam mParam;
};

class LANNum2D
{
public:
    struct AnimatedPane
    {
        J2DAnmTransform *mAnmTransform;  // 0x0
        J2DPicture *mPane;               // 0x4
        u32 _8;                          // 0x8
    };

    struct PlayerPane
    {
        J2DPicture *mPane;        // 0x0
        J2DPicture *mBoxPane;     // 0x4
        J2DPicture *mNumberPane;  // 0x8
    };

    struct KartPane
    {
        J2DPicture *mPane;        // 0x0
        J2DPicture *mBoxPane;     // 0x4
        J2DPicture *mNumberPane;  // 0x8
    };

    LANNum2D(JKRHeap *heap);  // 0x80195a90
    ~LANNum2D();

    void setDraw1();  // 0x801961c8
    void setDraw2();  // 0x80196918
    void setDraw4();  // 0x80197090
    void init();      // 0x80197840
    void draw();      // 0x80197934
    void calc();      // 0x80197a58
    void start();     // 0x80197b4c
    void start2();    // 0x80197c88

    void reset() { init(); }

private:
    J2DAnmTransform *mAnimation;   // 0x00
    u32 _4;                        // 0x04
    f32 mTransformFrame;           // 0x08
    AnimatedPane mEndAnm[4];       // 0x0c
    AnimatedPane mNumAnm[4];       // 0x3c
    LANNum2DHioNode *mNode;        // 0x6c
    J2DOrthoGraph *mOrthoGraph;    // 0x70
    J2DScreen *mScreen;            // 0x74
    bool mDrawFlag;                // 0x78
    bool mTransformEnd;            // 0x79
    u8 _7a;                        // 0x7a
    u8 _7b;                        // 0x7b
    PlayerPane mPlayerPane[4][2];  // 0x7c
    KartPane mKartPane[4];         // 0xdc
};

#endif  // LANNUM2D_H
