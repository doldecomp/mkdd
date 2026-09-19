#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JMath/JMath.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "math.h"
#include "Shiraiwa/Objects/MapObjYoshiHeli.h"

StateObserver::StateFuncSet<TMapObjYoshiHeli> TMapObjYoshiHeli::sTable[3] = {
    {0, &TMapObjYoshiHeli::initFunc_Fly, &TMapObjYoshiHeli::doFunc_Fly},
    {1, &TMapObjYoshiHeli::initFunc_Search, &TMapObjYoshiHeli::doFunc_Search},
    {2, &TMapObjYoshiHeli::initFunc_ForceBack, &TMapObjYoshiHeli::doFunc_ForceBack},
};

f32 TMapObjYoshiHeli::sAirFriction = 0.98f;
f32 TMapObjYoshiHeli::sAccel = 2.0f;
f32 TMapObjYoshiHeli::sTurnMaxSpeed = 0.5f;
f32 TMapObjYoshiHeli::sTurnMinSpeed = 0.05f;
f32 TMapObjYoshiHeli::sMinMoveDistance = 10.0f;
f32 TMapObjYoshiHeli::sLeanVelY = 500.0f;
f32 TMapObjYoshiHeli::sFrontTurnMaxSpeed = 0.02f;
f32 TMapObjYoshiHeli::sFloatHeight = 500.0f;
s16 TMapObjYoshiHeli::sFloatSpeed = 250;

J3DAnmTextureSRTKey *TMapObjYoshiHeli::sYoshiHeliBtkAnm;

TMapObjYoshiHeli::TMapObjYoshiHeli(const CrsData::SObject &sObject) : TMapObjHioNode(sObject), StateObserver() {
    createSoundMgr();
    mPathUtil = new TPathUtil(&sObject);
}

TMapObjYoshiHeli::~TMapObjYoshiHeli() {
    delete mPathUtil;
}

void TMapObjYoshiHeli::reset() {
    GeographyObj::resetObject();
    ResetState();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _58 = 0;
    mAnmObjMat.resetFrame();
    mSearchRadius = mObjData->mParam1;
    if (mSearchRadius <= 0.0f) {
        mSearchRadius = 10000.0f;
    }
    // Not sure why double assignment happens here...?
    _1ac = mObjData->mParam2;
    _1ac = 20000.0f;
    _178.set(mPos);
    mPosHeli.set(mPos);
    _190.zero();
    mTurnDir.zero();
    _1b0 = 0.0f;
    floatSpeed = 0;
    mPathUtil->reset();
}

void TMapObjYoshiHeli::calc() {
    ExecuteState();
    mPos.set(mPosHeli);
    mPos.y += sFloatHeight * JMASSin(floatSpeed);
    floatSpeed += sFloatSpeed;
    mAnmObjMat.getFrameCtrl()->update();
    getSoundMgr()->setSe(0x40062);
}

void TMapObjYoshiHeli::update() {
    mAnmObjMat.anmFrameProc();
    GeographyObj::update();
}

void TMapObjYoshiHeli::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjMaterial::setupTexSRTAnmData(&sYoshiHeliBtkAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/YoshiHeli.btk"));
}

void TMapObjYoshiHeli::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x200, 0);
    mAnmObjMat.setExModel(&mModel);
    mAnmObjMat.setAnmBase(sYoshiHeliBtkAnm);
    mAnmObjMat.initFrameCtrl(mAnmObjMat.getAnmBase());
}

void TMapObjYoshiHeli::InitExec() { Observer_FindAndInit(TMapObjYoshiHeli, 3); }

void TMapObjYoshiHeli::MoveExec() { Observer_FindAndExec(TMapObjYoshiHeli, 3); }

void TMapObjYoshiHeli::initFunc_Fly() {
    _1b8 = 0;
    mVel.zero();
}

void TMapObjYoshiHeli::doFunc_Fly() {
    turn(mTurnDir);
    s16 nearPlayerKart = findNearPlayerKart(mSearchRadius);
    if (nearPlayerKart != -1) {
        mTargetKartNo = nearPlayerKart;
        nearPlayerKart = -1;
        for (u32 i = 0; nearPlayerKart == -1 && i < RCMGetManager()->getConsoleNumber(); i++) {
            if (mTargetKartNo == RCMGetManager()->getTargetKartNo(i)) {
                nearPlayerKart = i;
            }
        }
        if (nearPlayerKart != -1) {
            mKartToTrack = nearPlayerKart;
            setState(1);
        }
    }
}

s16 TMapObjYoshiHeli::findNearPlayerKart(f32 radius) {
    s16 nearestPlayerKart = -1;
    f32 bestDistSquared = SQUARE(radius);

    for (s16 i = 0; i < RCMGetManager()->getKartNumber(); i++) {
        if (RCMGetManager()->getKartInfo(i)->getPad(0) != nullptr) {
            JGeometry::TVec3f kartPos;
            ObjUtility::getKartPos(i, &kartPos);
            kartPos -= mObjData->position;

            f32 dist = kartPos.squaredZX();

            if (dist < bestDistSquared) {
                nearestPlayerKart = i;
                bestDistSquared = dist;
            }
        }
    }

    return nearestPlayerKart;
}

void TMapObjYoshiHeli::initFunc_Search() {
    _1b0 = 0;
    _190.zero();
}

void TMapObjYoshiHeli::doFunc_Search() {
    const CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(mPathUtil->mObj->mPathID, mPathUtil->getPointIndex());

    JGeometry::TVec3f target;
    target.x = pointData->pos.x;
    target.y = pointData->pos.y;
    target.z = pointData->pos.z;

    JGeometry::TVec3f dir;
    dir.sub(target, mPosHeli);

    f32 distSq = dir.squared();
    dir.normalize();

    move(target, true);
    turn(dir);

    if (distSq < 9000000.0f) {
        TPathUtil *path = mPathUtil;

        path->mDistanceDirty = true;
        path->mPointIndex = path->getNextNode();

        if (mPathUtil->getNextNode() == 0) {
            setState(2);
        }
    }
}

void TMapObjYoshiHeli::initFunc_ForceBack() {}

void TMapObjYoshiHeli::doFunc_ForceBack() {
    JGeometry::TVec3f posDiff;
    posDiff.sub(mObjData->position, mPosHeli);

    f32 len = posDiff.normalize();

    move(mObjData->position, true);
    turn(posDiff);

    if (len < 3000.0f) {
        mVel.scale(0.89f);
        if (mVel.length() <= 15.0f) {
            setState(0);
        }
    }
}

void TMapObjYoshiHeli::getTargetPos(JGeometry::TVec3f *, u8) {}
void TMapObjYoshiHeli::getKartPos(u8) {}

void TMapObjYoshiHeli::move(const JGeometry::TVec3f &newPos, bool unused) {
    // Is this not used?
    JGeometry::TVec3f vec;
    vec.scaleAdd(100.0f, mTurnDir, mPosHeli);

    mVel.scale(sAirFriction);
    if (mVel.length() > sMinMoveDistance) {
        mPosHeli.add(mVel);
    }
    mVel.scaleAdd(sAccel, mTurnDir, mVel);
}

bool TMapObjYoshiHeli::isWall(const JGeometry::TVec3f &) {}

void TMapObjYoshiHeli::turn(const JGeometry::TVec3f &dir) {
    JGeometry::TVec3f front;
    front.set(mTurnDir);
    front.normalize();

    JGeometry::TVec3f delta;
    delta.sub(dir, front);

    JGeometry::TVec3f work(delta);
    f32 len = work.normalize();
    if (len > sTurnMaxSpeed) {
        delta.setLength(sTurnMaxSpeed);
    } else if (len < sTurnMinSpeed) {
        delta.zero();
    }

    if (dir.dot(front) < 0.0f) {
        delta.scale(10.0f);
    }

    delta.scale(0.1f);
    front.add(delta);
    front.normalize();

    mTurnDir.set(front);
    mTurnDir.y *= 0.01f;
    setFront(front);
}


void TMapObjYoshiHeli::setFront(const JGeometry::TVec3f &param_1) {
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirY;
    JGeometry::TVec3f vecDirZ;
    JGeometry::TVec3f frontDiff;
    
    mRotMtx.getZDir(vecDirZ);
    vecDirZ.normalize();

    frontDiff.sub(param_1, vecDirZ);
    frontDiff.y = 0.0f;

    f32 len = frontDiff.length();
    if (len > sFrontTurnMaxSpeed) {
        frontDiff.scale((sFrontTurnMaxSpeed / len));
    }

    vecDirZ.add(frontDiff);
    vecDirZ.normalize();

    vecDirY.set(0.0f, sLeanVelY, 0.0f);
    vecDirY.add(mVel);
    vecDirY.normalize();

    vecDirX.cross(vecDirY, vecDirZ);
    vecDirX.normalize();

    vecDirZ.cross(vecDirX, vecDirY);
    vecDirZ.normalize();

    mRotMtx.setXYZDir(vecDirX, vecDirY, vecDirZ);
}


#include "JSystem/JAudio/JASFakeMatch2.h"
