#ifndef COORD3D_H
#define COORD3D_H


#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Quat.h"
#include "JSystem/JGeometry/Vec.h"
#include "Kaneshige/Course/CrsData.h"
#include "macros.h"

class TFreeMove {
public:
    TFreeMove();
    virtual ~TFreeMove() {}
    void init(JGeometry::TVec3f *, JGeometry::TVec3f *, f32);
    void reset();
    void setTargetPos(const JGeometry::TVec3f &, f32, f32);
    void setTargetOffset(const JGeometry::TVec3f &, f32, f32);
    void setTargetOffsetUniform(const JGeometry::TVec3f &, int);
    void update();
    void checkReachTarget();
    void velUpdate(JGeometry::TVec3f &, f32, f32);
    void TPathMove(const CrsData::SObject *);

    // Inline/Unused
    void initStart();
    void setTargetPosUniform(const JGeometry::TVec3f &, int);
    void fixCurPosition();

    bool hasTarget() const {
        return _18;
    }

    void releaseTarget() { // not sure what to name this
        _18 = false;
    }

private:
    JGeometry::TVec3f mTagret;
    JGeometry::TVec3f *mpPos;
    JGeometry::TVec3f *mpVel;
    bool _18;
    f32 _1c;
    f32 _20;
    f32 _24;
};

class TPathMove {
public:
    TPathMove(const CrsData::SObject *);
    virtual ~TPathMove() {}
    void update();
    void init(JGeometry::TVec3f *, JGeometry::TVec3f *);
    void reset();
    void setTargetNode();
    void setTargetNode(u16, f32, f32);
    void getNodePosition(JGeometry::TVec3f *, u16);
    void getNextNode();
    void updatePos();
    void checkReachTarget();

    // Inline/Unused
    u16 get_8() const { return _8; }
    void setTargetNode(f32, f32);
    void setTargetNode(u16);
    void getNodeDir(u16, JGeometry::TVec3f *);

protected:
    const CrsData::SObject *mpObj; // 04
    u16 _8;                      // 
    JGeometry::TVec3f *mpPos;      // 0c
    JGeometry::TVec3f *mpVel;      // 10
    f32 _14;
    f32 _18;
    bool _1c;
    PLACEHOLDER_BYTES(0x1d, 0x24);
}; // Size: 0x24

class TFreeRotate {
public:
    TFreeRotate();
    virtual ~TFreeRotate() {}
    void init(JGeometry::TPos3f *);
    void setTargetVec(const JGeometry::TVec3f &, const JGeometry::TVec3f &, f32, f32, f32);
    void setTargetVec(const JGeometry::TVec3f &, f32, f32, f32, u8);
    void setTargetQuat(const JGeometry::TQuat4f &, f32, f32, f32);
    bool update();
    void angleUpdate();
    void velUpdate();
    bool checkReachTarget();
    void setSpeed(f32);
    void restart();

    // Inline/Unused
    TFreeRotate(JGeometry::TPos3f *);
    void initStart();
    void setTargetQuat(const JGeometry::TQuat4f &, const JGeometry::TQuat4f &, f32, f32, f32);

    void stop() {
        _28 = false;
    }

    JGeometry::TPos3f *mpMatrix;
    JGeometry::TQuat4f _8;
    JGeometry::TQuat4f _18;
    bool _28;
    f32 mSpeedInc;
    f32 mSpeed;
    f32 mMaxSpeed;
    f32 mTarget;
    bool _3c;
};
 
#endif // COORD3D_H
