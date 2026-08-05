#include "Shiraiwa/Interp.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"

TBSplineInterp::TBSplineInterp() {
    _5 = 0;
    mRatio = 0.0f;
    _10 = 0;
    _13 = false;
    mSpeed = 0.0f;
    _1c = 0;
}

TBSplineInterp::~TBSplineInterp() {}

void TBSplineInterp::init(JGeometry::TVec3f *pos) {
    mpCurPos = pos;
    JUT_ASSERT(mpCurPos != 0);
}

void TBSplineInterp::getVel(JGeometry::TVec3f *vel) {
    vel->set(mVel);
}

void TBSplineInterp::setPoint(u8 p1, const f32 *pPos, f32 spd, bool p4) {
    _5 = p1;
    mpPoint = pPos;
    _13 = true;
    mSpeed = spd;
    _10 = 0;
    _12 = p4;
    mRatio = 0.0f;
    _14 = false;
    mOffset.set(0.0f, 0.0f, 0.0f);
    mpCurPos->set(pPos[0], pPos[1], pPos[2]);
}

void TBSplineInterp::setPointOffset(u8 p1, const f32 *pPos, f32 spd, bool p4) {
    _5 = p1;
    mpPoint = pPos;
    _13 = true;
    mSpeed = spd;
    _10 = 0;
    _12 = p4;
    _14 = true;
    mOffset.set(*mpCurPos);
    mRatio = 0.0f;
    mpCurPos->x += pPos[0];
    mpCurPos->y += pPos[1];
    mpCurPos->z += pPos[2];
}

bool TBSplineInterp::update() {
    JGeometry::TVec3f startPos;
    startPos.set(*mpCurPos);
    if (_13) {
        if (checkReachTarget()) {
            if (_12) {
                _10 = 0;
            }
            else {
                const JGeometry::TVec3f &point = ((const JGeometry::TVec3f *)mpPoint)[(u8)(_5 - 1)];
                mpCurPos->set(point);
                _13 = false;
            }
        }
        else {
            updatePos();
            ratioUpdate();
        }
    }
    mVel.sub(*mpCurPos, startPos);

    return _13;
}

bool TBSplineInterp::checkReachTarget() {
    if (_12) {
        return _10 > _5 - 1;
    }
    return _10 > _5;
}

void TBSplineInterp::updatePos() {}

void TBSplineInterp::ratioUpdate() {
    mRatio += mSpeed;
    if (mRatio > 1.0f) {
        mRatio -= 1.0f;
        _10++;
        if (_1c & 1 && _10 < (u8)(_5 - 1) - 1) { // this has to be an inline
            setRotation(_10);
        }
    }
    
}

void TBSplineInterp::setRotation(u8) {}

void TBSplineInterp::setSpeed(f32 speed) {
    mSpeed = speed;
    if (_1c & 1) {
        mRotate->setSpeed(speed);
    }
}

void TBSplineInterp::pointUpdate(f32 t) {
    static f32 inv6 = 1.0f/6.0f;
    f32 negT = 1.0f - t;
    f32 t2 = t*t;
    f32 t3 = t2*t;
    f32 negT3 = -t3;
    
    mPoint.x = inv6 * negT * negT * negT;
    mPoint.y = inv6 * ((t3 * 3.0f - t2 * 6.0f) + 4.0f);
    mPoint.z = inv6 * ((negT3 + t2 + t) * 3.0f + 1.0f);
    mPoint.w = inv6 * t3;
}

void TBSplineInterp::stop() {
    _13 = false;
    if (_1c & 1) {
        mRotate->stop();
    }
}

void TBSplineInterp::restart() {
    _13 = true;
    if (_1c & 1) {
        mRotate->restart();
    }
}
