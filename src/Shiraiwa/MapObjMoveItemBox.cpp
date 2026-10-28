#include "Shiraiwa/Objects/MapObjMoveItemBox.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/Objects/GeoItemCar.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/Objects/GeoItemBox.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/SiUtil.h"
#include "kartEnums.h"
#include "types.h"

StateObserver::StateFuncSet<TMapObjMoveItemBox> TMapObjMoveItemBox::sTable[5] = {
    { 0, &TMapObjMoveItemBox::initFunc_Appear, &TMapObjMoveItemBox::doFunc_Appear },
    { 1, &TMapObjMoveItemBox::initFunc_Roll, &TMapObjMoveItemBox::doFunc_Roll },
    { 2, &TMapObjMoveItemBox::initFunc_Disappear, &TMapObjMoveItemBox::doFunc_Disappear },
    { 3, &TMapObjMoveItemBox::initFunc_Hide, &TMapObjMoveItemBox::doFunc_Hide },
    { 4, &TMapObjMoveItemBox::initFunc_RaceEnd, &TMapObjMoveItemBox::doFunc_RaceEnd },
};

f32 TMapObjMoveItemBox::sHeight = 115.0f;
f32 TMapObjMoveItemBox::sRollSpeed = 0.43f;
s16 TMapObjMoveItemBox::sAppearFrame = 5;
s16 TMapObjMoveItemBox::sDisappearFrame = 5;
f32 TMapObjMoveItemBox::sGravitySize = 1.0f;
f32 TMapObjMoveItemBox::sReflectRate = 0.45f;
f32 TMapObjMoveItemBox::sAirFriction = 0.998f;
f32 TMapObjMoveItemBox::sFriction = 0.989f;
f32 TMapObjMoveItemBox::sJumpPow = 10.0f;

f32 TMapObjMoveItemBoxLimit::sGravitySize = 1.5f;

TMapObjMoveItemBox::TMapObjMoveItemBox(const CrsData::SObject &sObject) : GeoItemBox(sObject), StateObserver() {
    mFreeFallShakeSky = new TFreeFallShakeSky(&mPos, &mVel, &mRotMtx);
}

TMapObjMoveItemBox::~TMapObjMoveItemBox() {
    delete mFreeFallShakeSky;
}

void TMapObjMoveItemBox::reset() {
    GeoItemBox::reset();
    resetObject();
    ResetState();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();

    for (int i = 0; i < 2; i++) {
        mEmitter[i] = nullptr;
    }

    mFreeFallShakeSky->reset();
    mFreeFallShakeSky->mGravity = sHeight * mScale.y;
    mFreeFallShakeSky->mRotSpeed = sRollSpeed / mScale.y;
    mFreeFallShakeSky->mGravitySize = sGravitySize;
    mFreeFallShakeSky->mReflectRate = sReflectRate;
    mFreeFallShakeSky->mAirFriction = sAirFriction;
    mFreeFallShakeSky->mFriction = sFriction;

    mFreeFallShakeSky->_38 = 1;
    mFreeFallShakeSky->mRnd = getGeoRnd();
    mFreeFallShakeSky->mJumpPow = sJumpPow;
    mFreeFallShakeSky->mEnabled = true;

    mVel.set(mObjData->zNorm);
    mVel.scale(10.0f);
}

void TMapObjMoveItemBox::InitExec() { Observer_FindAndInit(TMapObjMoveItemBox, 5); }

void TMapObjMoveItemBox::MoveExec() { Observer_FindAndExec(TMapObjMoveItemBox, 5); }

void TMapObjMoveItemBox::calc() {
    doMode();
    ExecuteState();
}

void TMapObjMoveItemBox::move() {
    JGeometry::TVec3f itemBoxPos;
    itemBoxPos.set(mPos);

    if (RCMGetManager()->getRacePhase() != PHASE_CRS_DEMO) {
        mFreeFallShakeSky->update();
    }
    moveShadowModel();

    bool shouldSetState = false;
    if (mPos.y <= sHeight * mScale.y) {
        shouldSetState = true;
    } else {
        CrsGround crsGround(RCMGetCourse());
        crsGround.search(itemBoxPos);

        if (crsGround.getAttribute() == 10) {
            f32 itemBoxHeight = sHeight + crsGround.getHeight();

            if (itemBoxPos.y - itemBoxHeight > 0.0f) {
                if (mPos.y - itemBoxHeight < 0.0f) {
                    shouldSetState = true;
                }
            }
        }
    }

    if (shouldSetState) {
        setState(2);
    }
}

void TMapObjMoveItemBox::initFunc_Appear() {
    PSMTXIdentity(mRotMtx);
    setAllCheckKartHitFlag();
    clrObjFlagHidding();
    showShadow();
    mVel.set(0.0f, 0.0f, 0.0f);
    mScale.set(0.0f, 0.0f, 0.0f);
    resetPosition();
}

void TMapObjMoveItemBox::resetPosition() {
    mPos.set(mObjData->position);
}

void TMapObjMoveItemBox::doFunc_Appear() {
    f32 normalRange = SiUtil::getNormalRange(getStateCount(), 0.0f, sAppearFrame) + 0.01f;
    
    if (normalRange >= 1.0f) {
        normalRange = 1.0f;
        setState(1);
    }

    mScale.setAll(mObjData->scale.y * normalRange);
    move();
}

void TMapObjMoveItemBox::initFunc_Roll() {}

void TMapObjMoveItemBox::doFunc_Roll() {
    move();
    if (RCMGetManager()->isRaceEnd()) {
        setState(4);
    }
}

void TMapObjMoveItemBox::setOpeningPos() {
    mPos.set(mObjData->position);
    PSMTXIdentity(mRotMtx);
}

void TMapObjMoveItemBox::initFunc_Disappear() {
    mScale.setAll(mObjData->scale.y);
    clrAllCheckKartHitFlag();
    JPEffectMgr::getEffectMgr()->createEmt("mk_itemget_normal_a", mPos);
}

void TMapObjMoveItemBox::doFunc_Disappear() {
    f32 normalRange = 1.0f - SiUtil::getNormalRange(getStateCount(), 0.0f, sDisappearFrame);

    if (normalRange <= 0.0f) {
        setObjFlagHidding();
        normalRange = 1.0f;
        setState(3);
    }

    mScale.setAll(mObjData->scale.y * normalRange);
}

void TMapObjMoveItemBox::initFunc_Hide() {
    if (mShadowMdl != nullptr) {
        mShadowMdl->clrVisibleAll();
    }
    setObjFlagHidding();
}

void TMapObjMoveItemBox::doFunc_Hide() {
    if (!RCMGetManager()->isRaceEnd()) {
        if (getStateCount() > getInactiveTime()) {
            setState(0);
        }
    }
}

void TMapObjMoveItemBox::initFunc_RaceEnd() {
    mScale.setAll(mObjData->scale.y);
    clrAllCheckKartHitFlag();
}

void TMapObjMoveItemBox::doFunc_RaceEnd() {
    doFunc_Disappear();
}


void TMapObjMoveItemBoxLimit::reset() {
    TMapObjMoveItemBox::reset();

    #line 330
    JUT_ASSERT_MSG(mObjData->mPathID != 0xffff, "MoveItemBoxLimit: Illegal Path ID");

    CrsData::PointData *crsPointData = RCMGetCourse()->getCrsData()->getPointData(mObjData->mPathID, 0);
    JGeometry::TVec3f crsPos;
    crsPos.set(crsPointData->pos);
    crsPointData = RCMGetCourse()->getCrsData()->getPointData(mObjData->mPathID, 1);

    JGeometry::TVec3f newCrsPos;
    newCrsPos.set(crsPointData->pos);
    pos.set(crsPos);

    if (pos.x >= newCrsPos.x) {
        pos.x = newCrsPos.x;
    }
    if (pos.y >= newCrsPos.y) {
        pos.y = newCrsPos.y;
    }
    if (pos.z >= newCrsPos.z) {
        pos.z = newCrsPos.z;
    }

    vel.set(crsPos);
    if (vel.x <= newCrsPos.x) {
        vel.x = newCrsPos.x;
    }
    if (vel.y <= newCrsPos.y) {
        vel.y = newCrsPos.y;
    }
    if (vel.z <= newCrsPos.z) {
        vel.z = newCrsPos.z;
    }

    mFreeFallShakeSky->mGravitySize = sGravitySize;
    mFreeFallShakeSky->mFriction = 1.0f;
    mPos.set(crsPos);
    mPos.y += sHeight;
}

void TMapObjMoveItemBoxLimit::move() {
    mFreeFallShakeSky->update();
    fixMoveSphere();
    moveShadowModel();
}

void TMapObjMoveItemBoxLimit::fixMoveSphere() {
    f32 mainPos = mPos.x;
    f32 itemPos = pos.x;

    if (mainPos < itemPos) {
        mPos.x = itemPos;
        mVel.x = 0.0f;
    } else {
        itemPos = vel.x;
        if (mainPos > itemPos) {
            mPos.x = itemPos;
            mVel.x = 0.0f;
        }
    }

    itemPos = mPos.z;
    mainPos = pos.z;

    if (itemPos < mainPos) {
        mPos.z = mainPos;
        mVel.z = 0.0f;
    } else {
        mainPos = vel.z;
        if (itemPos > mainPos) {
            mPos.z = mainPos;
            mVel.z = 0.0f;
        }
    }
}

void TMapObjMoveItemBoxLimit::doKartColCallBack(int kartIdx) {
    if (tstIsHitKartFlg(kartIdx)) {
        GeoItemCar::hitItemBoxProc(kartIdx, mEmitter);
        setState(2);
    }
}

void TMapObjMoveItemBoxLimit::calc() {
    ExecuteState();
}

#include "JSystem/JAudio/JASFakeMatch2.h"
