#ifndef CANNOTSAVEG2D_H
#define CANNOTSAVEG2D_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRHeap.h"

class CanNotSaveG2D
{
public:
    CanNotSaveG2D(JKRHeap *heap);  // 0x80194fe0

    void init();  // 0x801950d4
    void draw();  // 0x80195124
    void calc();  // 0x801951c0

    void setVisible(bool visible) { mVisible = visible; }

private:
    J2DAnmTransform *mTransform;  // 0x00
    u32 _4;                       // 0x04
    f32 mTransformFrame;          // 0x08

    J2DAnmTevRegKey *mTevRegKey;  // 0x0c
    u32 _10;                      // 0x10
    f32 mTevRegKeyFrame;          // 0x14

    J2DGrafContext *mGrafContext;  // 0x18
    J2DScreen *mScreen;            // 0x1c

    bool mVisible;  // 0x20
};

#endif  // CANNOTSAVEG2D_H
