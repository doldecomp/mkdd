#include "Shiraiwa/Coord3D.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Quat.h"
#include "JSystem/JUtility/JUTAssert.h"

TFreeMove::TFreeMove() {}

void TFreeMove::init(JGeometry::TVec3f *, JGeometry::TVec3f *, f32) {}

void TFreeMove::reset() {}

void TFreeMove::setTargetPos(const JGeometry::TVec3f &, f32, f32) {}

void TFreeMove::setTargetOffset(const JGeometry::TVec3f &, f32, f32) {}

void TFreeMove::setTargetOffsetUniform(const JGeometry::TVec3f &, int) {}

void TFreeMove::update() {}

void TFreeMove::checkReachTarget() {}

void TFreeMove::velUpdate(JGeometry::TVec3f &, f32, f32) {}

TPathMove::TPathMove(const CrsData::SObject *) {}

u8 TPathMove::update() {}   // return type is some bool/integer type.

void TPathMove::init(JGeometry::TVec3f *, JGeometry::TVec3f *) {}

void TPathMove::reset() {}

void TPathMove::setTargetNode() {}

void TPathMove::setTargetNode(u16, f32, f32) {}

void TPathMove::getNodePosition(JGeometry::TVec3f *, u16) {}

u16 TPathMove::getNextNode() {}

void TPathMove::updatePos() {}

void TPathMove::checkReachTarget() {}

TFreeRotate::TFreeRotate() {
    mpMatrix = nullptr;
    _28 = false;
    _3c = false;
}

void TFreeRotate::init(JGeometry::TPos3f *matrix) {
    mpMatrix = matrix;
    _28 = false;
    JUT_ASSERT(mpMatrix !=0);
    mpMatrix->getQuat(_18);
    _8 = _18;
}

void TFreeRotate::setTargetVec(const JGeometry::TVec3f &, const JGeometry::TVec3f &, f32, f32, f32) {}

void TFreeRotate::setTargetVec(const JGeometry::TVec3f &, f32, f32, f32, unsigned char) {}

void TFreeRotate::setTargetQuat(const JGeometry::TQuat4f &q, f32 inc, f32 maxSpd, f32 target) {
    mpMatrix->getQuat(_18);
    if (!_18.equals(q)) {
        _8 = q;
        mSpeedInc = inc;
        mMaxSpeed = maxSpd;
        mSpeed = 0.0f;
        mTarget = target;
        _28 = true;
    }
}

bool TFreeRotate::update() {
    if (_28) {
        if (checkReachTarget()) {
            _28 = false;
        }
        else {
            angleUpdate();
            velUpdate();
        }
    }
    return _28;
}

void TFreeRotate::angleUpdate() {
    JGeometry::TQuat4f q;
    if (_3c & 1) {
        q.slerp(_18, _8, mTarget);
    }
    
    mTarget += mSpeed;
}

void TFreeRotate::velUpdate() {
    if (mSpeed < mMaxSpeed) {
        mSpeed += mSpeedInc;
        if (mSpeed > mMaxSpeed) {
            mSpeed = mMaxSpeed;
        }
    }   
}

bool TFreeRotate::checkReachTarget() {
    return mTarget > 1.0f;
}

void TFreeRotate::setSpeed(f32 speed) {
    if (speed > mMaxSpeed) {
        mMaxSpeed = speed;
    }
    mSpeed = speed;
}

void TFreeRotate::restart() {
    f32 f = mSpeed / (1.0f - mTarget);
    setTargetQuat(_8, f, f, 0.0f);
    _28 = true;
}
