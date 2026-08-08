#ifndef _MARIO_H
#define _MARIO_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/TColor.h"
#include "types.h"

class Mario
{
public:
    Mario(JKRHeap *heap);  // 0x8016eed0
    ~Mario();              // Unused

    void init();                                   // 0x8016f4cc
    void draw(int status, f32 f, JUTColor color);  // Unused
    void draw(f32 f, JUTColor color);              // 0x8016f4d0

private:
    J2DOrthoGraph *mOrthoGraph[4];  // 0x0

    J2DPicture *mPicture;  // 0x10

    s32 mPictureWidth;   // 0x14
    s32 mPictureHeight;  // 0x18

    J2DOrthoGraph *mOrthoGraph2D;  // 0x1c

    f32 mWidth;   // 0x20
    f32 mHeight;  // 0x24

    f32 mScaledWidth;   // 0x28
    f32 mScaledHeight;  // 0x2c
};

#endif  // _MARIO_H
