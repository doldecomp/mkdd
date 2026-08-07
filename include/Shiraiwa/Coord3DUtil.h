#ifndef COORD3DUTIL_H
#define COORD3DUTIL_H

#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Sato/stMath.h"
#include "Shiraiwa/Coord3D.h"

class TPathUtil {
public:
    TPathUtil();
    TPathUtil(const CrsData::SObject *);
    virtual ~TPathUtil();

    virtual void reset();
    void getDirection(JGeometry::TVec3f& out);
    float getDistance();
    u16 getNextNode();

    u16 getPointIndex() const { return mPointIndex; }
    void setPointIndex(u16 pointIndex) { mPointIndex = pointIndex; }

public:
    const CrsData::SObject *mObj;   // 0x4
    u16 mPointIndex;                // 0x8
    s8 mDirection;                  // 0xA
    f32 mDistance;                  // 0xC
    bool mDistanceDirty;            // 0x10 - 3 bytes padding.

}; // 0x14 - confirmed via TMapObjUFO Constructor.

class TPathUtilInterrupt : public TPathUtil {
public:
    TPathUtilInterrupt(const CrsData::SObject *sObject) : TPathUtil(sObject) {
        _14.set(0.0f, 0.0f ,0.0f);
        _20 = 0;
    }
    virtual void reset();
    void getCurrentNodePos(JGeometry::TVec3f *);
    void getNextNodePos(JGeometry::TVec3f *);
    void updateNode();

    JGeometry::TVec3f _14;
    bool _20;
};

class TPathWalk {
public:
    TPathWalk(const CrsData::SObject *);
    virtual ~TPathWalk();
    bool init(JGeometry::TVec3f *, JGeometry::TVec3f *, JGeometry::TPos3f *);
    bool reset();
    void setParameters(f32, f32, f32, f32, f32);
    void setCheckLength(f32);
    s16 update();
    void dirUpdate();
    void posUpdate();
    bool checkReachTarget();
    void forceTurn(const JGeometry::TVec3f &);
    void forceTurn(const JGeometry::TVec3f &, const JGeometry::TVec3f &);

    static s16 sNearlyReachTargetLength;

    TFreeRotate *_4;                        // 0x4
    TFreeRotate *_8;                        // 0x8
    CrsGround *crsGround;                   // 0xc
    JGeometry::TVec3f *_10;                 // 0x10
    JGeometry::TVec3f *_14;                 // 0x14
    JGeometry::TPos3f *_18;                 // 0x18
    JGeometry::TPos3f _1c;                  // 0x1c
    JGeometry::TPos3f _4c;                  // 0x4c
    f32 _7c;                                // 0x7c - TVec3f?
    f32 _80;                                // 0x80
    f32 _84;                                // 0x84
    TPathUtilInterrupt *_88;                // 0x88
    f32 mCheckLength;                       // 0x8c
    f32 _90;                                // 0x90
    u8 _94;                                 // 0x94
    f32 _98;                                // 0x98
    f32 _9c;                                // 0x9c
    f32 _a0;                                // 0xa0
    f32 _a4;                                // 0xa4
};


class TFreeFly {
public:
    TFreeFly();
    virtual ~TFreeFly();
    virtual void reset();
    void init(JGeometry::TVec3f *, JGeometry::TVec3f *, JGeometry::TPos3f *);
    void setParameters(f32, f32, f32);
    u8 update();
    void dirUpdate();
    void posUpdate();
    bool checkReachTarget();

    void forceTurn(JGeometry::TVec3f &);

    static s16 sNearlyReachTargetLength;

    TFreeRotate *mFreeRotate;       // 0x4
    JGeometry::TVec3f *mPosition;   // 0x8
    JGeometry::TVec3f *mVelocity;   // 0xc
    JGeometry::TPos3f *_10;         // 0x10
    JGeometry::TPos3f _14;          // 0x14
    JGeometry::TVec3f _44;          // 0x44
    f32 mSpeed;                     // 0x50
    u8 _54;                         // 0x54
    f32 _58;                        // 0x58
    f32 _5c;                        // 0x5c
};

class TFreeFall {
public:
    TFreeFall();
    TFreeFall(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, JGeometry::TPos3f *rot);
    virtual ~TFreeFall();
    virtual bool update();      // 0xc
    virtual void reset();       // 0x10
    virtual void roll();        // 0x14
    virtual void reflect(const JGeometry::TVec3f &);
    virtual void rotate();
    virtual void getGravity(JGeometry::TVec3f *);
    virtual void getReflect(const JGeometry::TVec3f &, JGeometry::TVec3f *);
    virtual void addAccel(const JGeometry::TVec3f &);
    void init(JGeometry::TVec3f *, JGeometry::TVec3f *, JGeometry::TPos3f *);

    f32 _4;
    f32 _8;
    f32 _c;
    f32 _10;
    f32 mRotSpeed;
    f32 mGravity;
    bool mEnabled;
    JGeometry::TVec3f *mPos;    // 0x20
    JGeometry::TVec3f *mVel;    // 0x24
    JGeometry::TPos3f *mRot;    // 0x28
};

class TFreeFallShakeSky : public TFreeFall {
public:
    TFreeFallShakeSky(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, JGeometry::TPos3f *rot) : TFreeFall(pos, vel, rot) {
        mRnd = nullptr;
    }
    virtual ~TFreeFallShakeSky();
    virtual void reset();
    virtual void reflect(const JGeometry::TVec3f &);
    virtual void rotate();
    virtual void getGravity(JGeometry::TVec3f *);
    virtual void getReflect(const JGeometry::TVec3f &, JGeometry::TVec3f *);

    // static scPowMax;
    // static scSmallJumpMin;
    // static scSmallJumpMax;

    stRandom *mRnd;
    f32 _30;
    f32 _34;
    u8 _38;
};

class TFreeFallDonkyRock {
public:
    ~TFreeFallDonkyRock();
    void reflect(const JGeometry::TVec3f &);
    void reset();
    void getReflect(const JGeometry::TVec3f &, JGeometry::TVec3f *);
};

#endif // COORD3DUTIL_H
