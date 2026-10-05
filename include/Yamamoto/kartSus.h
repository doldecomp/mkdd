#ifndef KARTSUS_H
#define KARTSUS_H

#include <JSystem/JGeometry.h>
#include "Kaneshige/KartLoader.h"
#include "Kaneshige/Course/CrsGround.h"

class KartBody;
class Spline;

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
    KartBody *getKartBody();

    // Unused
    void InitTestTirePose(int);
    void WallReflection(JGeometry::TVec3<float> *, JGeometry::TVec3<float> *, float, float);

    // TODO
    ExModel *mWheel;        // 0x0
    ExModel *mArm;          // 0x4
    ExModel *mShock;        // 0x8
    KartBody *mBody;        // 0xc
    KartLoader *mLoader;    // 0x10
    CrsGround mCrsGnd;      // 0x14
    Spline *mSpline;        // 0x74
    JGeometry::TVec3f mSplinePts[4]; // 0x78
    f32 mSpring;            // 0xa8
    f32 mDamp;              // 0xac
    f32 mSusBase;           // 0xb0
    f32 mSusCur;            // 0xb4
    f32 mSusPrev;           // 0xb8
    f32 mSusForce;          // 0xbc
    JGeometry::TVec3f mContactPos; // 0xc0
    JGeometry::TVec3f mGndPoint;   // 0xcc
    JGeometry::TVec3f mForcePos;   // 0xd8
    JGeometry::TVec3f mGndNormal;  // 0xe4
    JGeometry::TVec3f mScratchVec; // 0xf0
    f32 mTireRadius;        // 0xfc
    f32 mTireDim[3];        // 0x100
    f32 mTireDispAngle;     // 0x10c
    f32 mWheelRPM;          // 0x110
    f32 mSusFactor;         // 0x114
    f32 mDampExt;           // 0x118
    f32 mDampComp;          // 0x11c
    f32 mGndHeight;         // 0x120
    u32 mSusFlags;          // 0x124
    Mtx mSusMtx;            // 0x128
    Mtx mTireMtx;           // 0x158
    Mtx mArmMtx;            // 0x188
    Mtx mSuspArmMtx;        // 0x1b8
    Mtx mSuspMtx;           // 0x1e8
    Mtx mDumpMtx;           // 0x218
    Mtx mDumpArmMtx;        // 0x248
    f32 mRPM;               // 0x278
    f32 mTireGrip;          // 0x27c
    f32 mGripScale;         // 0x280
    JGeometry::TVec3f mSusPos; // 0x284
    f32 mSusPower;          // 0x290
    f32 mSusPowerMul;       // 0x294
    f32 mTireSusA;          // 0x298
    f32 mTireSusB;          // 0x29c
    f32 mSusParam0;         // 0x2a0
    u8 mSusState;           // 0x2a4
    f32 mSpringBase;        // 0x2a8
    f32 mSpringMax;         // 0x2ac
    f32 mSpringMin;         // 0x2b0
    f32 mSpringK;           // 0x2b4
    f32 mSpringDiv;         // 0x2b8
    f32 mSuspRearY;         // 0x2bc
    f32 mSuspFrontY;        // 0x2c0
    f32 mSusParam2;         // 0x2c4
    u32 mObjKind;           // 0x2c8
}; // 0x2cc

#endif
