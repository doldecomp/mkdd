#ifndef GOAL2D_H
#define GOAL2D_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JORReflexible.h"
#include "Kaneshige/HioMgr.h"

class Goal2DParam : public JORReflexible
{
public:
    Goal2DParam();
    virtual ~Goal2DParam() {}

    u16 mUnknown;
    bool mFlag;
};

class Goal2DHioNode : public HioNode
{
public:
    Goal2DHioNode() : HioNode("ゴール", &mParam, 0, 0) {}
    virtual ~Goal2DHioNode() {}

    Goal2DParam mParam;
};

class Goal2D
{
public:
    Goal2D(JKRHeap *heap);  // 0x80132758
    ~Goal2D();

    void init();         // 0x801330ec
    void drawGoal();     // 0x80133224
    void drawRetire();   // 0x801333a0
    void calc();         // 0x80133514
    void sequenceGP();   // 0x80133cc8
    void sequenceVS();   // 0x80133cf4
    void sequenceTA();   // 0x80133f58
    void sequenceMG();   // 0x80133f7c
    void setVSMGRank();  // 0x80134128

    void reset() { init(); }
    int getAnmFrame(int status) const { return mViewport[status].mAnmFrame; }

    static bool mDrawEndFlag;  // 0x80416290

private:
    struct TViewportData
    {
        f32 mX;           // 0x0
        f32 mY;           // 0x4
        bool mGoalAnm;    // 0x8
        bool mRetireAnm;  // 0x9
        s32 mAnmFrame;    // 0xc
    };

    Goal2DHioNode *mHioNode;               // 0x0
    J2DOrthoGraph *mOrthoGraph[4];         // 0x4
    J2DScreen *mScreen[4];                 // 0x14
    J2DAnmTransform *mTransform[4];        // 0x24
    J2DAnmTevRegKey *mTevRegKey[4];        // 0x34
    J2DPicture *mGoalPicture[4][2];        // 0x44
    J2DScreen *mRetireScreen[4];           // 0x64
    J2DAnmTransform *mRetireTransform[4];  // 0x74
    J2DAnmTevRegKey *mRetireTevRegKey[4];  // 0x84
    J2DPicture *mRetirePicture[4];         // 0x94
    TViewportData mViewport[4];            // 0xa4
    u32 _e4;                               // 0xe4
    bool mVisible;                         // 0xe8
    u8 _e9;                                // 0xe9
};

#endif  // GOAL2D_H
