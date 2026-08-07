#include "Shiraiwa/Coord3DUtil.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/RaceMgr.h"
#include "Shiraiwa/Coord3D.h"
#include "Shiraiwa/SiUtil.h"
#include "dolphin/mtx.h"
#include "math.h"
#include "types.h"

s16 TPathWalk::sNearlyReachTargetLength = 300;
s16 TFreeFly::sNearlyReachTargetLength = 500;

TPathUtil::TPathUtil() {}

TPathUtil::TPathUtil(const CrsData::SObject *sObject) {
    setPointIndex(0);
    mDirection = 1;
    mObj = sObject;
}

TPathUtil::~TPathUtil() {}

void TPathUtil::reset() {
    mDistanceDirty = true;
    mDistance = 0.0f;
    setPointIndex(0);
    mDirection = 1;
}

u16 TPathUtil::getNextNode() {
    s16 nextNode = getPointIndex();
    nextNode += mDirection;

    CrsData::PathData *pathData = RCMGetCourse()->getCrsData()->getPathData(mObj->mPathID);
    if (nextNode >= pathData->getPointNumber()) {
        pathData = RCMGetCourse()->getCrsData()->getPathData(mObj->mPathID);
        if (pathData->isClosed()) {
            nextNode = 0;
        } else {
            nextNode = getPointIndex() - 1;
            mDirection = -1;
        }
    } else {
        if (nextNode < 0) {
            nextNode = getPointIndex() + 1;
            mDirection = 1;
        }
    }
    return nextNode;
}

void TPathUtil::getDirection(JGeometry::TVec3f &out) {
    int pointIndex = getPointIndex();

    CrsData::PointData* cur =
        RCMGetCourse()->getCrsData()->getPointData(mObj->mPathID, pointIndex);
    JGeometry::TVec3f current;
    current.set(cur->pos);

    s16 pointIdx = getPointIndex();
    pointIdx += mDirection;

    CrsData::PathData *path =
        RCMGetCourse()->getCrsData()->getPathData(mObj->mPathID);

    if (pointIdx >= path->getPointNumber()) {
        if (RCMGetCourse()->getCrsData()->getPathData(mObj->mPathID)->isClosed()) {
            pointIdx = 0;
        } else {
            pointIdx = getPointIndex() - 1;
            mDirection = -1;
        }
        
    } else if (pointIdx < 0) {
        pointIdx = getPointIndex() + 1;
        mDirection = 1;
    }

    CrsData::PointData *next = RCMGetCourse()->getCrsData()->getPointData(mObj->mPathID, pointIdx);

    JGeometry::TVec3f nextPos;
    nextPos.set(next->pos);

    JGeometry::TVec3f dir;
    dir.sub(nextPos, current);
    dir.normalize();
    out.set(dir);
}

f32 TPathUtil::getDistance() {
    if (mDistanceDirty) {
        int pointIndex = getPointIndex();

        CrsData::PointData* cur = RCMGetCourse()->getCrsData()->getPointData(mObj->mPathID, pointIndex);

        JGeometry::TVec3f current;
        current.set(cur->pos);

        s16 pointIdx = getPointIndex();
        pointIdx += mDirection;

        CrsData::PathData* path = RCMGetCourse()->getCrsData()->getPathData(mObj->mPathID);

        if (pointIdx >= path->getPointNumber()) {
            if (RCMGetCourse()->getCrsData()->getPathData(mObj->mPathID)->isClosed()) {
                pointIdx = 0;
            } else {
                pointIdx = getPointIndex() - 1;
                mDirection = -1;
            }
        } else if (pointIdx < 0) {
            pointIdx = getPointIndex() + 1;
            mDirection = 1;
        }

        CrsData::PointData* next = RCMGetCourse()->getCrsData()->getPointData(mObj->mPathID, pointIdx);

        JGeometry::TVec3f nextPos;
        nextPos.set(next->pos);

        JGeometry::TVec3f diff;
        diff.sub(nextPos, current);

        mDistance = PSVECMag(&diff);
        mDistanceDirty = false;
    }

    return mDistance;
}

void TPathUtilInterrupt::reset() {
    mDistanceDirty = true;
    mDistance = 0.0f;
    setPointIndex(0);
    mDirection = 1;
    _20 = false;
}

TPathWalk::TPathWalk(const CrsData::SObject *sObject) {
    _88 = new TPathUtilInterrupt(sObject);
    _4 = new TFreeRotate();
    _8 = new TFreeRotate();
    crsGround = new CrsGround(RCMGetCourse());
    _7c = sObject->position.x;
    _80 = sObject->position.y;
    _84 = sObject->position.z;
}

TPathWalk::~TPathWalk() {
    delete _88;
    delete _4;
    delete _8;
    delete crsGround;
}

bool TPathWalk::init(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, JGeometry::TPos3f *rot) {
    _10 = pos;
    _18 = rot;
    _14 = vel;
    PSMTXIdentity(_1c);
    PSMTXIdentity(_4c);
    _4->init(&_1c);
    _8->init(&_4c);
    return reset();
}

bool TPathWalk::reset() {
    bool didUpdate = false;
    JGeometry::TVec3f local_28;
    
    _88->reset();
    _94 = 0;
    PSMTXIdentity(_1c);
    PSMTXIdentity(_4c);
    _18->getYDir(local_28);
    _8->setTargetVec(local_28, 1.0f, 1.0f, 1.0f, 'y');
    _8->update();
    _18->getZDir(local_28);
    _4->setTargetVec(local_28, 1.0f, 1.0f, 1.0f, 'z');
    didUpdate = _4->update();
    _90 = 0.0f;
    _a0 = 0.0f;
    _98 = 0.0f;
    _a4 = 0.0f;
    _9c = 0.0f;
    mCheckLength = sNearlyReachTargetLength;
    return didUpdate;
}

void TPathWalk::setParameters(f32 param_1, f32 param_2, f32 param_3, f32 param_4, f32 param_5) {
    _90 = param_1;
    _98 = param_2;
    _9c = param_3;
    if (param_4 == 0.0f) {
        param_4 = _98;
    }
    _a0 = param_4;
    if (param_5 == 0.0f) {
        param_5 = _9c;
    }
    _a4 = param_5;
}

void TPathWalk::setCheckLength(f32 checkLength) {
    if (checkLength <= 0.0f) {
        checkLength = sNearlyReachTargetLength;
    }
    mCheckLength = checkLength;
}

s16 TPathWalk::update() {
    TPathUtilInterrupt *pathInt;
    s16 result = -1;

    if (_94) {
        dirUpdate();
        posUpdate();

        if (checkReachTarget()) {
            pathInt = _88;
            result = pathInt->getPointIndex();

            if (pathInt->_20) {
                pathInt->_20 = false;
            } else {
                pathInt->mDistanceDirty = true;

                s16 node = pathInt->getPointIndex();
                node += pathInt->mDirection;

                if (node >= RCMGetCourse()->getCrsData()->getPathData(pathInt->mObj->mPathID)->getPointNumber()) {
                    if (RCMGetCourse()->getCrsData()->getPathData(pathInt->mObj->mPathID)->isClosed()) {
                        node = 0;
                    } else {
                        node = pathInt->getPointIndex() - 1;
                        pathInt->mDirection = -1;
                    }
                } else if (node < 0) {
                    node = pathInt->getPointIndex() + 1;
                    pathInt->mDirection = 1;
                }
                
                pathInt->setPointIndex(node);
            }
        }
    }

    return result;
}

void TPathWalk::dirUpdate() {
    JGeometry::TVec3f crsPointNormal;
    
    _4->update();
    _8->update();
    PSMTXConcat(_4c, _1c, *_18);
    
    if (!_4->_28) {
        if (_88->_20) {
            crsPointNormal.set(_88->_14);
        } else {
            CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(_88->mObj->mPathID, _88->getPointIndex());
            crsPointNormal.set(pointData->pos);
        }
        crsPointNormal.sub(*_10);
        crsPointNormal.y = 0.0f;
        crsPointNormal.normalize();
        _4->setTargetVec(crsPointNormal, _a0, _98, 0.0f, 'z');
    }
    if (!_8->_28) {
        crsGround->search(*_10);
        JGeometry::TVec3f crsGroundNormal;
        crsGround->getNormal(&crsGroundNormal);
        crsGroundNormal.x = 0.0f;
        crsGroundNormal.normalize();
        _8->setTargetVec(crsGroundNormal, _a4, _9c, 0.0f, 'y');
    }
}

void TPathWalk::posUpdate() {
    JGeometry::TVec3f zDir;
    _18->getZDir(zDir);
    f32 initialHeight = _10->y;
    zDir.normalize();
    zDir.scale(_90);
    _14->set(zDir);
    _10->add(*_14);

    JGeometry::TVec3f crsGroundNormal;
    crsGround->search(*_10);
    crsGround->getNormal(&crsGroundNormal);
    if (crsGround->getAttribute() == CrsGround::Attr_255) {
        _10->y = initialHeight;
    } else {
        _10->y = crsGround->getHeight();
    }
    return;
}

bool TPathWalk::checkReachTarget() {
    bool reached = false;
    JGeometry::TVec3f target;

    if (_88->_20) {
        target.set(_88->_14);
    } else {
        CrsData::PointData *point = RCMGetCourse()->getCrsData()->getPointData(_88->mObj->mPathID, _88->getPointIndex());
        target.set(point->pos);
    }

    target.sub(*_10);
    target.y = 0.0f;

    if (target.squared() < mCheckLength * mCheckLength) {
        reached = true;
    }

    return reached;
}

void TPathWalk::forceTurn(const JGeometry::TVec3f &param_1) {
    _4->setTargetVec(param_1, 1.0f, 1.0f, 1.0f, 'z');
    dirUpdate();
}

void TPathWalk::forceTurn(const JGeometry::TVec3f &param_1, const JGeometry::TVec3f &param_2) {
    _8->setTargetVec(param_1, 1.0f, 1.0f, 1.0f, 'y');
    _4->setTargetVec(param_2, 1.0f, 1.0f, 1.0f, 'z');
    dirUpdate();
}

TFreeFly::TFreeFly() {
    mFreeRotate = nullptr;
    mPosition = nullptr;
    mVelocity = nullptr;
    _10 = nullptr;
    mFreeRotate = new TFreeRotate();
}

TFreeFly::~TFreeFly() {
    delete mFreeRotate;
}

void TFreeFly::init(JGeometry::TVec3f *param_1, JGeometry::TVec3f *param_2, JGeometry::TPos3f *param_3) {
    mPosition = param_1;
    _10 = param_3;
    mVelocity = param_2;
    PSMTXIdentity(_14);
    mFreeRotate->init(&_14);
}

void TFreeFly::reset() {
    _54 = 0;
}

void TFreeFly::setParameters(f32 param_1, f32 param_2, f32 param_3) {
    mSpeed = param_1;
    _58 = param_2;
    _5c = param_3;
}

u8 TFreeFly::update() {
    if (_54 != 0) {
        if (checkReachTarget()) {
            _54 = 0;
        } else {
            dirUpdate();
            posUpdate();
        }
    }
    return _54;
}

void TFreeFly::dirUpdate() {
    JGeometry::TVec3f local_38;
    mFreeRotate->update();
    PSMTXCopy(_14, *_10);
    if (mFreeRotate->_28 == false) {
        local_38.sub(_44, *mPosition);
        local_38.y = 0.0f;
        local_38.normalize();
        mFreeRotate->setTargetVec(local_38, _5c, _58, 0.0f, 'z');
    }
}

void TFreeFly::posUpdate() {
    JGeometry::TVec3f moveDir;
    JGeometry::TVec3f dir;

    dir.sub(_44, *mPosition);
    dir.normalize();

    _10->getZDir(moveDir);
    moveDir.normalize();

    moveDir.y = dir.y;

    moveDir.normalize();
    moveDir.scale(mSpeed);

    mVelocity->set(moveDir);
    mPosition->add(*mVelocity);
}

bool TFreeFly::checkReachTarget() {
    bool reached = false;
    JGeometry::TVec3f delta;
    delta.sub(_44, *mPosition);

    if (delta.squared() < sNearlyReachTargetLength * sNearlyReachTargetLength) {
        reached = true;
    }
    return reached;
}

TFreeFall::TFreeFall() {}

TFreeFall::TFreeFall(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, JGeometry::TPos3f *rot) {
    init(pos, vel, rot);
}

TFreeFall::~TFreeFall() {}

void TFreeFall::init(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, JGeometry::TPos3f *mtx) {
    #line 561
    JUT_ASSERT(pos != 0);
    #line 562
    JUT_ASSERT(vel != 0);
    #line 563
    JUT_ASSERT(mtx != 0);
    mPos = pos;
    mVel = vel;
    mRot = mtx;
}

void TFreeFall::reset() {
    _4 = 0.99f;
    _8 = 1.0f;
    _c = 0.3f;
    _10 = 1.0f;
    mRotSpeed = 0.95f;
    mGravity = 50.0f;
    mEnabled = false;
}

bool TFreeFall::update() {
    if (mEnabled) {
        roll();
    }
    return mEnabled;
}

void TFreeFall::roll() {
    JGeometry::TVec3f pos;
    JGeometry::TVec3f gravity;

    pos.set(*mPos);
    mVel->scale(_4);
    getGravity(&gravity);
    addAccel(gravity);
    mPos->add(*mVel);
    reflect(pos);
    rotate();
}

void TFreeFall::reflect(const JGeometry::TVec3f &param_1) {
    JGeometry::TVec3f local_94;
    JGeometry::TVec3f local_a0;
    JGeometry::TVec3f local_ac;
    JGeometry::TVec3f local_b8;

    CrsGround crsGround(RCMGetCourse());
    crsGround.search(*mPos, param_1);

    u32 attr = crsGround.getAttribute();
    if ((attr == CrsGround::Attr_2) || (attr != CrsGround::Attr_10 && (mPos->y < mGravity + crsGround.getHeight()))) {
        if (attr == CrsGround::Attr_2) {
            f32 wallNormal = crsGround.getWallNormal(&local_94, nullptr);
            local_a0.set(local_94);
            local_a0.normalize(wallNormal);
            mPos->add(local_a0);
        } else {
            crsGround.getNormal(&local_94);
            mPos->y = mGravity + crsGround.getHeight();
            mVel->scale(_8);
        }

        local_ac.scale(local_94.dot(*mVel), local_94);

        if (PSVECMag(&local_ac) > _c) {
            getReflect(local_94, &local_b8);
        } else {
            local_b8.zero();
        }

        local_ac.scale(-(_10 + 1.0f));
        local_ac.add(local_b8);
        mVel->add(local_ac);
    }
}

void TFreeFall::getReflect(const JGeometry::TVec3f &param_1, JGeometry::TVec3f *param_2) {
    JGeometry::TVec3f local_6c;
    JGeometry::TVec3f local_78;

    local_78.set(*mVel);
    local_78.normalize();

    local_6c.cross(param_1, local_78);
    local_6c.normalize();

    local_78.cross(local_6c, param_1);
    local_78.normalize();

    if (local_78.isZero()) {
        local_78.set(0.0f, 0.0f, 1.0f);
    }
    param_2->set(local_78);
}

void TFreeFall::rotate() {
    JGeometry::TPos3f local_78;
    JGeometry::TVec3f local_84;
    JGeometry::TVec3f local_90;
    JGeometry::TVec3f local_9c;
    JGeometry::TVec3f local_a8;
    
    local_84.set(mVel->x, 0.0f, mVel->z);
    if (!local_84.isZero()) {
        f32 dVar4 = mRotSpeed * SiUtil::getNormalRange(local_84.length(), 0.0f, 50.0f);
        local_a8.set(*mVel);
        local_a8.normalize();
        local_9c.set(0.0f, 1.0f, 0.0f);
        local_90.cross(local_9c, local_a8);
        local_90.normalize();
        local_78[2][3] = 0.0f;
        local_78[1][3] = 0.0f;
        local_78[0][3] = 0.0f;
        local_78[2][2] = 0.0f;
        local_78[1][2] = 0.0f;
        local_78[0][2] = 0.0f;
        local_78[2][1] = 0.0f;
        local_78[1][1] = 0.0f;
        local_78[0][1] = 0.0f;
        local_78[2][0] = 0.0f;
        local_78[1][0] = 0.0f;
        local_78[0][0] = 0.0f;
        local_78.setRotate(local_90, dVar4);
        PSMTXConcat(local_78, *mRot, *mRot);
    }
}

void TFreeFall::addAccel(const JGeometry::TVec3f &accel) {
    JGeometry::TVec3f newAccel;
    JGeometry::TVec3f crsNormal;
    
    CrsGround crsGround(RCMGetCourse());
    crsGround.search(*mPos);
    if (mPos->y - accel.y < mGravity + crsGround.getHeight()) {
        crsGround.getNormal(&crsNormal);
        f32 dp = crsNormal.dot(accel);
        JMAVECScaleAdd(&crsNormal, &accel, &newAccel, -dp);
        mVel->add(newAccel);
    } else {
        mVel->add(accel);
    }
}

void TFreeFall::getGravity(JGeometry::TVec3f *gravity) {
    gravity->set(0.0f, -1.0f, 0.0f);
    gravity->scale(_c);
}

void TFreeFallShakeSky::getGravity(JGeometry::TVec3f *gravity) {
    gravity->set(*RCMGetCourse()->getDirY());
    gravity->normalize();
    gravity->negate();
    gravity->scale(_c);
}

void TFreeFallShakeSky::reset() {
    TFreeFall::reset();
    _34 = 5.0f;
    _38 = 0;
    _30 = 0.0f;
    mRnd = nullptr;
}

void TFreeFallShakeSky::getReflect(const JGeometry::TVec3f &param_1, JGeometry::TVec3f *param_2) {
    JGeometry::TVec3f local_6c;
    JGeometry::TVec3f local_78;
    local_78.set(*mVel);
    local_78.normalize();
    local_6c.cross(param_1, local_78);
    local_6c.normalize();
    local_78.cross(local_6c, param_1);
    local_78.normalize();
    if (local_78.isZero()) {
        local_78.set(0.0f, 0.0f, 1.0f);
    }
    param_2->set(local_78);
}

// FIX: Register mismatch around `local_48.normalize();`.
void TFreeFallShakeSky::reflect(const JGeometry::TVec3f &param_1) {
    JGeometry::TVec3f local_48;
    JGeometry::TVec3f local_3c;
    JGeometry::TVec3f local_30;
    JGeometry::TVec3f local_24;
    
    CrsGround crsGround(RCMGetCourse());
    crsGround.search(*mPos, param_1);
    
    u32 attr = crsGround.getAttribute();
    f32 f25 = 0.0f;

    if (attr == CrsGround::Attr_2 || (attr != CrsGround::Attr_10 && mPos->y < mGravity + crsGround.getHeight())) {
        local_48.set(0.0f, 0.0f, 0.0f);
        if (attr == CrsGround::Attr_2) {
            f25 = crsGround.getWallNormal(&local_48, nullptr);
            crsGround.getWallNormal(&local_48, nullptr);
            
            if (_38 != 0) {
                _30 = 0.0f;
            }
        } else {
            crsGround.getNormal(&local_48);
            f32 height = crsGround.getHeight();

            f25 = (mGravity + height) - mPos->y;
            mVel->scale(_8);
            
            if (_38 != 0 && _30 > F_HALF_PI) {
                if (mRnd != nullptr) {

                    local_24.set(*mVel);
                    local_24.normalize();
                    
                    local_30.set(0.0f, 1.0f, 0.0f);
                    local_3c.cross(local_30, local_24);
                    local_3c.normalize();
                    
                    f32 velMag = mVel->length();
                    f32 minLen = (5.0f < velMag) ? 5.0f : velMag;

                    f32 scaledRand = (2.0f * mRnd->get_ufloat_1()) - 1.0f;
                    
                    f32 f24 = minLen * scaledRand;
                    local_3c.scale(f24);
                    mVel->add(local_3c);
                }
                
                mVel->y += (_34 * SiUtil::getNormalRange(mVel->length(), 5.0f, 20.0f) + 1.0f);
                _30 = 0.0f;
            }
        }

        mVel->length();
        local_48.normalize();

        JGeometry::TVec3f local_18;
        local_18.scale(local_48.dot(*mVel), local_48);
        
        JGeometry::TVec3f local_0c;
        local_0c.scale(-(1.0f + _10), local_18);
        mVel->add(local_0c);
        
        local_18.scale(f25, local_48);
        mPos->add(local_18);

    } else if (_38 != 0) {
        _30 = 0.0f;
    }
}

void TFreeFallShakeSky::rotate() {
    JGeometry::TPos3f local_78;
    JGeometry::TVec3f local_84;
    JGeometry::TVec3f local_90;
    JGeometry::TVec3f local_9c;
    JGeometry::TVec3f local_a8;

    local_84.set(mVel->x, 0.0f, mVel->z);
    if (!local_84.isZero()) {
        f32 dVar5 = mRotSpeed * SiUtil::getNormalRange(local_84.length(), 0.0f, 50.0f);
        local_a8.set(*mVel);
        local_a8.normalize();

        local_9c.set(0.0f, 1.0f, 0.0f);
        local_90.cross(local_9c, local_a8);
        local_90.normalize();

        // is there a zeroing function?
        local_78[2][3] = 0.0f;
        local_78[1][3] = 0.0f;
        local_78[0][3] = 0.0f;
        local_78[2][2] = 0.0f;
        local_78[1][2] = 0.0f;
        local_78[0][2] = 0.0f;
        local_78[2][1] = 0.0f;
        local_78[1][1] = 0.0f;
        local_78[0][1] = 0.0f;
        local_78[2][0] = 0.0f;
        local_78[1][0] = 0.0f;
        local_78[0][0] = 0.0f;

        local_78.setRotate(local_90, dVar5);
        PSMTXConcat(local_78.mMtx, mRot->mMtx, mRot->mMtx);
        _30 += dVar5;
    }
}

TFreeFallShakeSky::~TFreeFallShakeSky() {}


void TPathUtilInterrupt::getCurrentNodePos(JGeometry::TVec3f *) {}

void TPathUtilInterrupt::getNextNodePos(JGeometry::TVec3f *) {}

void TPathUtilInterrupt::updateNode() {}


TFreeFallDonkyRock::~TFreeFallDonkyRock() {}

void TFreeFallDonkyRock::reflect(const JGeometry::TVec3f &) {}

void TFreeFallDonkyRock::reset() {}

void TFreeFallDonkyRock::getReflect(const JGeometry::TVec3f &, JGeometry::TVec3f *) {}


#include "JSystem/JAudio/JASFakeMatch2.h"
