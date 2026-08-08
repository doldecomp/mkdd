#ifndef _FADE_H
#define _FADE_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/TColor.h"
#include "types.h"

class Fade
{
public:
    Fade(JKRHeap *heap);  // 0x8013b318
    ~Fade();

    void init();                                   // 0x8013b76c
    void draw(int status, f32 f, JUTColor color);  // 0x8013b770

private:
    void *_0;  // 0x0

    J2DOrthoGraph *mOrthoGraph[4];  // 0x4

    struct
    {
        f32 mWidth;   // 0x14
        f32 mHeight;  // 0x18
    } mSizes[4];
};

#endif  // _FADE_H
