#include "Shiraiwa/Interp.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Shiraiwa/Coord3D.h"

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
#line 41
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
                mpCurPos->set(getPointX(_5 - 1), getPointY(_5 - 1), getPointZ(_5 - 1));
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

void TBSplineInterp::updatePos() {
    pointUpdate(mRatio);

    u8 indices[4];
    for (int i = -1; i < 3; i++)
    {
        int index = _10 + i;

        if (index > _5 - 1) {
            if (_12) {
                index -= _5;
            }
            else {
                index = _5 - 1;
            }
        }
        else {
            if (index < 0) {
                if (_12) {
                    index += _5;
                }
                else {
                    index = 0;                    
                }      
            }
        }

        indices[i+1] = index;
    }

    if (_14) { // with offset
        mpCurPos->x = mOffset.x + (mPoint.x * getPointX(indices[0]) +
            mPoint.y * getPointX(indices[1]) +
            mPoint.z * getPointX(indices[2]) +
            mPoint.w * getPointX(indices[3]));
        mpCurPos->y = mOffset.y + (mPoint.x * getPointY(indices[0]) +
            mPoint.y * getPointY(indices[1]) +
            mPoint.z * getPointY(indices[2]) +
            mPoint.w * getPointY(indices[3]));
        mpCurPos->z = mOffset.z + (mPoint.x * getPointZ(indices[0]) +
            mPoint.y * getPointZ(indices[1]) +
            mPoint.z * getPointZ(indices[2]) +
            mPoint.w * getPointZ(indices[3]));
    }
    else { // without offset
        mpCurPos->x = (mPoint.x * getPointX(indices[0]) +
            mPoint.y * getPointX(indices[1]) +
            mPoint.z * getPointX(indices[2]) +
            mPoint.w * getPointX(indices[3]));
        mpCurPos->y = (mPoint.x * getPointY(indices[0]) +
            mPoint.y * getPointY(indices[1]) +
            mPoint.z * getPointY(indices[2]) +
            mPoint.w * getPointY(indices[3]));
        mpCurPos->z = (mPoint.x * getPointZ(indices[0]) +
            mPoint.y * getPointZ(indices[1]) +
            mPoint.z * getPointZ(indices[2]) +
            mPoint.w * getPointZ(indices[3]));
    }


}

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

void TBSplineInterp::setRotation(u8 p1) {
#line 242
    JUT_ASSERT(mRotate != 0);

    JGeometry::TVec3f v;
    if (p1 < 1) {
        v.set(0.0f, 0.0f, 0.0f);
    }
    else {
        v.set(getPointX(p1) - getPointX(p1-1), 0.0f, getPointZ(p1) - getPointZ(p1-1));
    }
    

    if (v.length() > 10.0f) {
        v.normalize();
        mRotate->setTargetVec(v, mSpeed, mSpeed, mRatio, 'z');
    }
}

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

#include "JSystem/JAudio/JASFakeMatch2.h"
