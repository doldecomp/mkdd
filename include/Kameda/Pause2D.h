#ifndef PAUSE2D_H
#define PAUSE2D_H

#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DPane.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JORReflexible.h"
#include "Kaneshige/HioMgr.h"
#include "Osako/kartPad.h"

class Pause2DParam : public JORReflexible
{
public:
    Pause2DParam();
    virtual ~Pause2DParam() {}

    u8 mFlag;  // 0x04
};

class Pause2DHioNode : public HioNode
{
public:
    Pause2DHioNode() : HioNode("ポーズ表示", &mParam, 0, 0) {}
    virtual ~Pause2DHioNode() {}

    Pause2DParam mParam;
};

class Pause2D
{
public:
    enum PauseStatus
    {
        PAUSE_ZERO = 0,
        PAUSE_ONE = 1,
        PAUSE_TWO = 2,
        PAUSE_THREE = 3,
        PAUSE_FOUR = 4,
    };

    Pause2D(JKRHeap *heap);  // 0x8013554c
    ~Pause2D();

    void init();                                     // 0x80136308
    void draw();                                     // 0x801364d0
    void calc(KartGamePad *pad);                     // 0x8013653c
    void selectorGP(KartGamePad *pad);               // 0x8013672c
    void selectorVS(KartGamePad *pad);               // 0x80136854
    void selectorTA(KartGamePad *pad);               // 0x801369d4
    void selectorMG(KartGamePad *pad);               // 0x80136b54
    void selectorCommon();                           // 0x80136cec
    void setDrawGP();                                // 0x80136d34
    void setDrawVS();                                // 0x80136ee4
    void setDrawTA();                                // 0x80137408
    void setDrawMG();                                // 0x8013792c
    void setDrawCommon();                            // 0x80137ff4
    f32 getScale(int);                               // 0x801381b4
    static int getPauseSelector();                   // 0x801381fc
    static PauseStatus getPauseStatus();             // 0x80138204
    static void setPauseStatus(PauseStatus status);  // 0x8013820c
    void setPadText();                               // 0x80138214

private:
    static int mPauseSelector;   // 0x804162b8
    static PauseStatus mStatus;  // 0x804162bc

    Pause2DHioNode *mHioNode;          // 0x00
    J2DOrthoGraph *mOrthoGraph;        // 0x04
    J2DScreen *mScreen;                // 0x08
    J2DAnmTransform *mTransform;       // 0x0c
    J2DAnmColor *mColor;               // 0x10
    s32 _14;                           // 0x14
    s32 _18;                           // 0x18
    s32 mAnmFrame[2];                  // 0x1c
    f32 mScale[2];                     // 0x24
    J2DPicture *mMenuEntries[6];       // 0x2c
    J2DPane *mPaneStarsPosOffset;      // 0x44
    J2DPane *mPaneStarsPos[12];        // 0x48
    JGeometry::TVec2f mStarsPos[12];   // 0x78
    J2DPane *mStars[2];                // 0xd8
    s32 mCounter;                      // 0xe0
    JGeometry::TVec2f mInitialPos[2];  // 0xe4
    JGeometry::TVec2f mTargetPos[2];   // 0xf4
    JGeometry::TVec2f mDrawPos[2];     // 0x104

    struct LinePane
    {
        J2DAnmBase *mAnm;  // 0x00
        J2DPane *mPane;    // 0x04
        f32 mScale;        // 0x08
    } mPaneLines[6];       // 0x114

    void (Pause2D::*mSelector)(KartGamePad *);  // 0x15c
    void (Pause2D::*mSetDraw)();                // 0x168
};

#endif  // PAUSE2D_H
