#ifndef KARTSUS_H
#define KARTSUS_H

#include <JSystem/JGeometry.h>
#include "Kaneshige/KartLoader.h"
#include "Kaneshige/Course/CrsGround.h"

class KartBody;

class KartSus
{
public:
    KartSus() { }

    void InitTirePose(int);
    void InitSettingParam(int);
    void InitParam(int);
    void Init(int);
    void NormalInit(int);
    void ResetInit(int);
    void CircleFriction(JGeometry::TVec3f *, f32);
    f32 GetCornerForce();
    f32 GetCircleFric();
    void DoSusAction(int); // amogus
    void DoTireAction();
    // Inlines
    KartBody *getKartBody() { return mBody; }

    // Unused
    void InitTestTirePose(int);
    void WallReflection(JGeometry::TVec3<float> *, JGeometry::TVec3<float> *, float, float);

    // TODO
    ExModel *mWheel;
    ExModel *mArm;
    ExModel *mShock;
    KartBody *mBody; // 0xc
    KartLoader *mLoader;
    CrsGround mCrsGnd;
    u8 _74[4];
    JGeometry::TVec3f _78[4];
    u8 _a8[0xb0 - 0xa8];
    f32 _b0;
    f32 _b4;
    f32 _b8;
    u8 _bc[4];
    JGeometry::TVec3f _c0;
    JGeometry::TVec3f _cc;
    JGeometry::TVec3f _d8;
    JGeometry::TVec3f _e4;
    JGeometry::TVec3f _f0;
    f32 mTireRadius;
    u8 _100[0x10c - 0x100];
    f32 _10c;
    f32 _110;
    u8 _114[0x124 - 0x114];
    u32 _124; // bitmask? 0: on ground?
    Mtx _128;
    Mtx _158;
    Mtx _188;
    Mtx _1b8;
    Mtx _1e8;
    Mtx _218;
    Mtx _248;
    f32 _278;
    u8 _27c[0x284 - 0x27c];
    JGeometry::TVec3f _284;
    u8 _290[0x2cc - 0x290];
}; // _2cc

#endif
