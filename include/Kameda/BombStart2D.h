#ifndef BOMBSTART2D_H
#define BOMBSTART2D_H

#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRHeap.h"

class BombStart2D
{
public:
    BombStart2D(JKRHeap *heap);  // 0x80198214
    ~BombStart2D();

    void init();   // 0x801984d4
    void draw();   // 0x80198528
    void calc();   // 0x801985c4
    void start();  // 0x8019862c

private:
    J2DAnmTransform *mTransform;  // 0x00
    f32 mTransformFrame;          // 0x04

    J2DAnmTevRegKey *mTevRegKey;  // 0x08
    f32 mTevRegKeyFrame;          // 0x0c

    J2DAnmColor *mColor;  // 0x10
    f32 mColorFrame;      // 0x14

    J2DGrafContext *mGrafContext;  // 0x18
    J2DScreen *mScreen;            // 0x1c

    bool mVisible;  // 0x20
};

#endif  // BOMBSTART2D_H
