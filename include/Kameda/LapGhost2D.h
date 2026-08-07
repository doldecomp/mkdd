#ifndef LAPGHOST2D_H
#define LAPGHOST2D_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRHeap.h"

#include "Kaneshige/RaceTime.h"

class LapGhost2D
{
public:
    LapGhost2D(JKRHeap *heap);  // 0x801949c4
    ~LapGhost2D();              // Unused

    void init();                             // 0x80194b58
    void draw();                             // 0x80194bac
    void calc();                             // 0x80194c38
    void start(RaceTime raceTime, int lap);  // 0x80194d08

private:
    J2DAnmTransform *mTransform;  // 0x00
    u32 _4;                       // 0x04
    f32 mTransformFrame;          // 0x08

    J2DAnmTevRegKey *mTevRegKey;  // 0x0c
    u32 _10;                      // 0x10
    f32 mTevRegKeyFrame;          // 0x14

    J2DGrafContext *mGrafContext;  // 0x18
    J2DScreen *mScreen;            // 0x1c

    J2DPicture *mPictures[8];  // 0x20

    bool mVisible[2];  // 0x40
};

#endif  // LAPGHOST2D_H
