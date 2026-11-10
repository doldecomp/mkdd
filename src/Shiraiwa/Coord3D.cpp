#include "Shiraiwa/Coord3D.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Quat.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "dolphin/mtx.h"

TFreeMove::TFreeMove() {
    _18 = false;
    _1c = -1.0f;
}

void TFreeMove::init(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, f32 param_3) {
    mpPos = pos;
    mpVel = vel;
    setUnknown24(param_3);
    _1c = -1.0f;
    reset();

#line 84
    JUT_ASSERT(pos != 0 && vel != 0);
}

void TFreeMove::reset() {
    releaseTarget();
    mTarget.zero();
    _20 = 0.0f;
}

void TFreeMove::initStart() {}

void TFreeMove::setTargetPos(const JGeometry::TVec3f &pos, f32 param_2, f32 param_3) {
    mTarget.set(pos);
    _20 = param_2;
    _1c = param_3;
    _18 = true;
}

void TFreeMove::setTargetOffset(const JGeometry::TVec3f &offset, f32 param_2, f32 param_3) {
    mTarget.add(offset, *mpPos);
    _20 = param_2;
    _1c = param_3;
    _18 = true;
}

void TFreeMove::setTargetPosUniform(const JGeometry::TVec3f &, int) {}

void TFreeMove::setTargetOffsetUniform(const JGeometry::TVec3f &param_1, int param_2) {
    _20 = 0.0f;
    mpVel->scale(1.0f / param_2, param_1);
    _1c = mpVel->length();
    mTarget.add(*mpPos, param_1);
    _18 = true;
}

bool TFreeMove::update() {
    if (hasTarget()) {
        if (checkReachTarget()) {
            mpPos->set(mTarget);
            releaseTarget();
        } else {
            mpPos->add(*mpVel);
            velUpdate(*mpVel, _20, _1c);
        }
    }
    return hasTarget();
}

bool TFreeMove::checkReachTarget() {
    JGeometry::TVec3f relativePos;
    relativePos.sub(mTarget, *mpPos);
    return relativePos.length() <= _1c;
}

bool TFreeMove::velUpdate(JGeometry::TVec3f &vel, f32 param_2, f32 param_3) {
    bool isUpdate = false;
    
    if (param_2 == 0.0f) {
        isUpdate = false;
        return isUpdate;
    }

    isUpdate = false;
    if (param_3 <= 0.0f || vel.length() < param_3) {
        JGeometry::TVec3f local_80;
        local_80.sub(mTarget, *mpPos);
        local_80.normalize();
        local_80.scale(param_2);

        vel.add(local_80);

        if (param_3 > 0.0f && vel.length() > param_3) {
            vel.normalize();
            vel.scale(param_3);
        }
        isUpdate = true;
    }
    return isUpdate;
}

void TFreeMove::fixCurPosition() {}

TPathMove::TPathMove(const CrsData::SObject *sObject) {
    _8 = 0;
    _a = 1;
    _1c = false;
    _1d = false;
    mpObj = sObject;
    return;
}

u8 TPathMove::update() {
    if (_1c) {
        updatePos();
        if (checkReachTarget()) {
            if ((_1d & 1) != 0) {
                setTargetNode();
            } else {
                _1c = false;
            }
        }
    }
    return _1c;
}

void TPathMove::init(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel) {
    mpPos = pos;
    mpVel = vel;
    reset();

#line 448
    JUT_ASSERT(pos != 0 && vel != 0)
}

void TPathMove::reset() {
    _1d = false;
    _1c = false;
    _a = 1;
    _8 = 0;
}

void TPathMove::setTargetNode() {
    setTargetNode(getNextNode(), _14, _18);
}

void TPathMove::setTargetNode(f32, f32) {}

void TPathMove::setTargetNode(u16) {}

void TPathMove::setTargetNode(u16 param_1, f32 param_2, f32 param_3) {
    _8 = param_1;
    _14 = param_2;
    _18 = param_3;
    _1c = true;
}

void TPathMove::getNodePosition(JGeometry::TVec3f *param_1, u16 param_2) {
    param_1->set(RCMGetCourse()->getCrsData()->getPointData(mpObj->mPathID, param_2)->pos);
}

u16 TPathMove::getNextNode() {
    u16 nextNode = _8;
    nextNode += _a;

    if (nextNode >= RCMGetCourse()->getCrsData()->getPathData(mpObj->mPathID)->getPointNumber() - 1) {
        if (!RCMGetCourse()->getCrsData()->getPathData(mpObj->mPathID)->isClosed()) {
            _a = -1;
        } else {
            nextNode = 0;
        }
    } else if (nextNode == 0) {
        _a = 1;
    }

    return nextNode;
}

void TPathMove::getNodeDir(u16, JGeometry::TVec3f *) {}

void TPathMove::updatePos() {
    JGeometry::TVec3f crsPointPos;
    crsPointPos.set(RCMGetCourse()->getCrsData()->getPointData(mpObj->mPathID, _8)->pos);

    JGeometry::TVec3f posDelta;
    posDelta.sub(crsPointPos, *mpPos);
    posDelta.normalize();
    posDelta.scale(_14);

    mpVel->add(posDelta);
    if (mpVel->length() > _18) {
        mpVel->normalize();
        mpVel->scale(_18);
    }

    mpPos->add(*mpVel);
}

bool TPathMove::checkReachTarget() {
    JGeometry::TVec3f pointPos;
    pointPos.set(RCMGetCourse()->getCrsData()->getPointData(mpObj->mPathID, _8)->pos);
    pointPos.sub(*mpPos);
    return pointPos.squared() < _20 ? true : false;
}

TFreeRotate::TFreeRotate() {
    mpMatrix = nullptr;
    _28 = false;
    _3c = false;
}

TFreeRotate::TFreeRotate(JGeometry::TPos3f *) {}

void TFreeRotate::init(JGeometry::TPos3f *matrix) {
    mpMatrix = matrix;
    _28 = false;
#line 712
    JUT_ASSERT(mpMatrix != 0);
    mpMatrix->getQuat(_18);
    _8 = _18;
}

void TFreeRotate::initStart() {}

void TFreeRotate::setTargetVec(const JGeometry::TVec3f &from, const JGeometry::TVec3f &to, f32 param_3, f32 param_4, f32 param_5) {
    mpMatrix->getQuat(_18);
    
    JGeometry::TQuat4f rotation;
    rotation.setRotate(from, to);
    PSQUATMultiply(&rotation, &_18, &_8);

    mSpeedInc = param_3;
    mMaxSpeed = param_4;
    mSpeed = 0.0f;
    mTarget = param_5;
    _28 = true;
}

void TFreeRotate::setTargetVec(const JGeometry::TVec3f &param_1, f32 param_2, f32 param_3, f32 param_4, u8 axis) {
    JGeometry::TVec3f target;

    switch (axis) {
        case 'X':
        case 'x':
            mpMatrix->getXDir(target);
            break;

        case 'Y':
        case 'y':
            mpMatrix->getYDir(target);
            break;

        case 'Z':
        case 'z':
        default:
            mpMatrix->getZDir(target);
            break;
    }

    target.normalize();
    setTargetVec(target, param_1, param_2, param_3, param_4);
}

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

void TFreeRotate::setTargetQuat(const JGeometry::TQuat4<float> &, const JGeometry::TQuat4<float> &, float, float, float) {}

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
    } else {
        q.x = mTarget * (_8.x - _18.x) + _18.x;
        q.y = mTarget * (_8.y - _18.y) + _18.y;
        q.z = mTarget * (_8.z - _18.z) + _18.z;
        q.w = mTarget * (_8.w - _18.w) + _18.w;
    }

    q.normalize(q);
    mpMatrix->setQuat(q);
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

#include "JSystem/JAudio/JASFakeMatch2.h"
