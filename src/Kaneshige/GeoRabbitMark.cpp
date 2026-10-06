#include "Kaneshige/Objects/GeoRabbitMark.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/Objects/GeoShine.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Yamamoto/kartCtrl.h"
#include "kartEnums.h"
#include "types.h"

StateObserver::StateFuncSet<GeoRabbitMark> GeoRabbitMark::sTable[3] = {
    {0, &GeoRabbitMark::initFuncWait, &GeoRabbitMark::doFuncWait, },
    {1, &GeoRabbitMark::initFuncEquip, &GeoRabbitMark::doFuncEquip},
    {2, &GeoRabbitMark::initFuncRelease, &GeoRabbitMark::doFuncRelease}
};

u16 GeoRabbitMark::sSupervisorCreateNum;
GeoRabbitMarkSupervisor *GeoRabbitMark::sSupervisor;

GeoRabbitMarkSupervisor::GeoRabbitMarkSupervisor() {
    reset();
}

GeoRabbitMarkSupervisor::~GeoRabbitMarkSupervisor() {
    GeoRabbitMark::sSupervisor = nullptr;
}

f32 GeoRabbitMark::sFootLength = 200.0f;
f32 GeoRabbitMark::sEquipOfsY1 = 480.0f;
f32 GeoRabbitMark::sEquipOfsYPakkun1 = 550.0f;
f32 GeoRabbitMark::sEquipOfsY2 = 430.0f;
f32 GeoRabbitMark::sEquipOfsYPakkun2 = 530.0f;
f32 GeoRabbitMark::sEquipOfsY34 = 460.0f;
f32 GeoRabbitMark::sEquipOfsYPakkun34 = 550.0f;
f32 GeoRabbitMark::sGravity = 0.3f;
f32 GeoRabbitMark::sReflectK = 0.5f;
f32 GeoRabbitMark::sNormalScale = 0.6f;
f32 GeoRabbitMark::sEquipScale = 0.45f;
f32 GeoRabbitMark::sEqpSprMinLength = 250.0f;
f32 GeoRabbitMark::sEqpSpringK = 0.25f;
f32 GeoRabbitMark::sEqpAirRegist = 0.15f;
f32 GeoRabbitMark::sEqpChaseRate = 0.3f;
f32 GeoRabbitMark::sRelV0 = 30.0f;
f32 GeoRabbitMark::sRelGravity = 2.0f;
f32 GeoRabbitMark::sRelReflectK = 0.55f;

void GeoRabbitMarkSupervisor::reset() {
    CrsData *crsData = RCMGetCourse()->getCrsData();
    CrsData::MGParam *param = crsData->getMGParam(RaceMgr::getManager()->getKartNumber());
    mWinTime = param->getRabbitWinSec();
    mMinTime = param->getRabbitMinSec();
    mDecTime = param->getRabbitDecSec();
}

void GeoRabbitMarkSupervisor::calc() {}

void GeoRabbitMarkSupervisor::entry(GeoRabbitMark *mark) {
    mMark = mark;
}

int GeoRabbitMarkSupervisor::getRabbitKartNo() { 
    return mMark->mOwnerNo;
}

void GeoRabbitMarkSupervisor::release() {
    mMark->release();

    mWinTime -= mDecTime;
    if (mWinTime < mMinTime)
        mWinTime = mMinTime;
}

void GeoRabbitMarkSupervisor::getMarkPosition(JGeometry::TVec3f *out) {
    out->set(mMark->mPos);
}

void GeoRabbitMarkSupervisor::changeOwner(int ownerNum) {
    mMark->equip(ownerNum);
}

void GeoRabbitMarkSupervisor::startWinnerEffect() {
    mMark->startWinnerEffect();
}

void GeoRabbitMarkSupervisor::setWinnerPosition() {
    mMark->setWinnerPosition();
}

GeoRabbitMark::GeoRabbitMark(const CrsData::SObject &object) : GeographyObj(object) {
    if (sSupervisor == nullptr) {
        sSupervisor = new GeoRabbitMarkSupervisor();
        sSupervisorCreateNum = 1;
    }

    createSoundMgr();

    for (u32 i = 0; i < 4; i++) {
        if (i < RaceMgr::getManager()->getConsoleNumber()) {
            mShines[i] = (GeoShine*)GetGeoObjMgr()->createSubObj(0x26b4);
            mShines[i]->setMark(this);
            mShines[i]->setConsoleNo(i);
        }
        else {
            mShines[i] = nullptr;
        }
    }

    mState = 1;
    reset();
    mState = 0;
    sSupervisor->entry(this);
}

void GeoRabbitMark::reset() {
    resetObject();
    _58 = 0;
    setObjFlagMainResource();
    setAllCheckKartHitFlag();
    setObjFlagCheckItemHitting();
    setObjFlagSimpleDraw();
    clrObjFlagHidding();
    ResetState();

    mIsDemoMode = false;
    if (RaceMgr::getManager()->isCrsDemoMode()) {
        mIsDemoMode = true;
    }
    mEqpSpr.zero();
    mEqpSprLength = 0.0f;
    _1a0 = 0.0f;
    mMarkScale = sNormalScale;
    _1aa = 0;
    mPrevOwnerNo = cNonOwner;
    mOwnerNo = cNonOwner;
    _1b0 = 0;
    mWinnerPos.zero();
    _1c0 = 0;

#line 476
    JUT_ASSERT_MSG(mObjData->mPathID != 0xffff, "UNLINK PATH")
    bool initialized = false;
    if (mState == 0)
        initialized = true;
    resetPose(initialized);
}

void GeoRabbitMark::resetPose(bool initialized) {
    mVel.zero();
    if (initialized) {
        CrsData::PathData *pathData = RaceMgr::getManager()->getCourse()->getCrsData()->getPathData(mObjData->mPathID);
        u16 pointId = getGeoRnd()->getRandom() % pathData->pointCount;
        CrsData::PointData *pointData = RaceMgr::getManager()->getCourse()->getCrsData()->getPointData(mObjData->mPathID, pointId);
        mPos.set(pointData->pos);
    }
    else {
        mPos.set(mObjData->position);
    }

    mLfDir.set(mObjData->zNorm);
    mLfDir.scale(0.0001f);
    mUpDir.set(mObjData->yNorm);
    mUpDir.scale(0.0001f);
    mFrDir.cross(mUpDir, mLfDir);
    mFrDir.normalize();
    
}

void GeoRabbitMark::createColModel(J3DModelData *mdlData) {
    createBoundsSphere(225.0f, 1.0f);
}

void GeoRabbitMark::doKartColCallBack(const int kartNo) {
    if(!tstIsHitKartFlg(kartNo))
        return;

    bool canEquip = false;
    if (isNonOwner() && !GetKartCtrl()->CheckMatchless(kartNo)) {
        canEquip = true;
    }

    if (canEquip) {
        equip(kartNo);
    }
}

void GeoRabbitMark::equip(int kartNo) {
    mOwnerNo = kartNo;
    mPrevOwnerNo = cNonOwner;
    _1aa = 0;
    setState(1);
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
}

void GeoRabbitMark::calc() {
    if (_1aa > 0) {
        _1aa--;
    }
    else {
        mPrevOwnerNo = cNonOwner;
    }

    ExecuteState();

    if (_1c0 != 0) {
        mMarkScale = 0.35f;
    }
    else if (getState() == 1) {
        mMarkScale = sEquipScale;
    }
    else {
        mMarkScale = sNormalScale;
    }


    mFrDir.cross(mUpDir, mLfDir);
    mFrDir.normalize();
    mUpDir.cross(mLfDir, mFrDir);
    mRotMtx.setXYZDir(mFrDir, mUpDir, mLfDir);

    for (u32 i = 0; i < 4; i++) {
        if (mShines[i]) {
            mShines[i]->move();
        }
    }
}

void GeoRabbitMark::release() {
    if (getState() == 1) {
        setState(2);
    }
}

void GeoRabbitMark::startFastRot() {
    for(u32 i = 0; i < 4; i++) {
        if (mShines[i]) {
            mShines[i]->startFastRot();
        }
    }
}

void GeoRabbitMark::startWinnerEffect() {
    for(u32 i = 0; i < 4; i++) {
        if (mShines[i]) {
            mShines[i]->enableWinEffect();
        }
    }    
}

void GeoRabbitMark::setWinnerPosition() {
    if (RaceMgr::getManager()->getConsoleNumber() <= 1) {
        if (isRidingBossPakkun()) {
            mWinnerPos.set(-100.0f, -250.0f, 60.0f);
        }
        else {            
            mWinnerPos.set(-100.0f, -160.0f, 60.0f);
        }
    }
    else if (RaceMgr::getManager()->getConsoleNumber() <= 2) {
        if (isRidingBossPakkun()) {
            mWinnerPos.set(-100.0f, -230.0f, 60.0f);
        }
        else {            
            mWinnerPos.set(-100.0f, -110.0f, 60.0f);
        }
    }
    else {
        if (isRidingBossPakkun()) {
            mWinnerPos.set(-100.0f, -250.0f, 60.0f);
        }
        else {            
            mWinnerPos.set(-100.0f, -140.0f, 60.0f);
        }
    }

    startFastRot();
    _1c0 = 1;
}

void GeoRabbitMark::InitExec() { 
    Observer_FindAndInit(GeoRabbitMark, 3);  
}

void GeoRabbitMark::MoveExec() { 
    Observer_FindAndExec(GeoRabbitMark, 3);  
}

void GeoRabbitMark::initFuncWait() {
    _1b0 = 0;
}

void GeoRabbitMark::doFuncWait() {
    JGeometry::TVec3f v;
    v.scaleAdd(-sFootLength, mUpDir, mPos);

    if (!mIsDemoMode) {
        mVel.y -= sGravity;
    }

    v.add(mVel);

    CrsGround ground;
    ground.search(v);
    if (v.y < ground.getHeight()) {
        v.y = ground.getHeight();
        mVel.y *= -sReflectK;
        if (_1b0 < 5) {
            _1b0++;
        }
        setBoundSE();
    }

    mPos.scaleAdd(sFootLength, mUpDir, v);

    if(isResetGndAttr(ground.getAttribute())) {
        resetPose(true);
    }
    else if (ground.getWaterHeight() >= mPos.y) {
        resetPose(true);
    }
}

bool GeoRabbitMark::isRidingBossPakkun() {
    bool ret = false;
    if (mOwnerNo != cNonOwner) {
        KartInfo *kartInfo = RaceMgr::getManager()->getKartInfo(mOwnerNo);
        for (int i = 0; i < 2; i++) {
            if (kartInfo->getDriverCharID(i) == PETEY) {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

f32 GeoRabbitMark::getEquipOfsY() {
    if (RaceMgr::getManager()->getConsoleNumber() <= 1) {
        if (isRidingBossPakkun()) {
            return sEquipOfsYPakkun1;
        }
        else {            
            return sEquipOfsY1;
        }
    }
    else if (RaceMgr::getManager()->getConsoleNumber() <= 2) {
        if (isRidingBossPakkun()) {
            return sEquipOfsYPakkun2;
        }
        else {            
            return sEquipOfsY2;
        }
    }
    else if (isRidingBossPakkun()) {
        return sEquipOfsYPakkun34;
    }
    else {            
        return sEquipOfsY34;
    }
}

void GeoRabbitMark::initFuncEquip() {
#line 996
    JUT_ASSERT(mOwnerNo != cNonOwner);

    JGeometry::TVec3f kartPos;
    ObjUtility::getKartPos(mOwnerNo, &kartPos);
    ObjUtility::getKartYdir(mOwnerNo, &mUpDir);

    kartPos.scaleAdd(getEquipOfsY(), mUpDir, kartPos);
    mEqpSpr.sub(mPos, kartPos);
    mEqpSprLength = mEqpSpr.length();

    if (mEqpSprLength < sEqpSprMinLength) {
        mEqpSprLength = sEqpSprMinLength;
    }

    mEqpSpr.normalize();

    JGeometry::TVec3f upDir(0.0f, 1.0f, 0.0f);
    if (upDir.dot(mEqpSpr) < 0.3f) {
        if (mEqpSpr.y > 0.0f){
            mEqpSpr.set(0.0f,1.0f, 0.0f);
        } 
        else {
            mEqpSpr.set(0.0f,-1.0f, 0.0f);
        }
    }
    mEqpSpr.normalize();

    _1a0 = 0.0f;
    startFastRot();
    getSoundMgr()->setSe(0x40042);
}

void GeoRabbitMark::doFuncEquip() {
    JGeometry::TVec3f kartPos;
    ObjUtility::getKartPos(mOwnerNo, &kartPos);

    JGeometry::TVec3f kartYDir;
    ObjUtility::getKartYdir(mOwnerNo, &kartYDir);

    mUpDir.scaleAdd(sEqpChaseRate, kartYDir, mUpDir);
    mUpDir.normalize();
    kartPos.scaleAdd(getEquipOfsY(), mUpDir, kartPos);

    JGeometry::TVec3f kartZDir;
    ObjUtility::getKartZdir(mOwnerNo, &kartZDir);

    JGeometry::TVec3f kartXDir;
    ObjUtility::getKartXdir(mOwnerNo, &kartXDir);

    kartPos.scaleAdd(mWinnerPos.x, kartXDir, kartPos);
    kartPos.scaleAdd(mWinnerPos.y, mUpDir, kartPos);
    kartPos.scaleAdd(mWinnerPos.z, kartZDir, kartPos);

    _1a0 *= (1.0f - sEqpAirRegist);
    f32 springK = -sEqpSpringK * mEqpSprLength;
    _1a0 += springK;
    mEqpSprLength += _1a0;
    mPos.scaleAdd(mEqpSprLength, mEqpSpr, kartPos);
}

void GeoRabbitMark::initFuncRelease() {
    mPrevOwnerNo = mOwnerNo;
    _1aa = 120;
    mOwnerNo = -1;
    _1b0 = 0;

    mUpDir.set(0.0f, 1.0f, 0.0f);
    mVel.set(0.0f, sRelV0, 0.0f);
    getSoundMgr()->setSe(0x40043);
}

void GeoRabbitMark::doFuncRelease() {
    JGeometry::TVec3f v;
    v.scaleAdd(-sFootLength, mUpDir, mPos);

    CrsArea area;
    area.searchRoof(v);
    mVel.y -= sRelGravity;

    v.add(mVel);
    if (area.isInside()) {
        JGeometry::TVec3f v2;
        v2.scaleAdd(sFootLength, mUpDir, v);
    
        JGeometry::TVec3f roofPos;
        area.getRoofPosition(&roofPos);

        if (v2.y >= roofPos.y) {
            v2.y = roofPos.y;
            if (mVel.y > 0.0f) {
                mVel.y = 0.0f;
            }
            v.scaleAdd(-sFootLength, mUpDir, v2);
        }
    }

    CrsGround ground;
    ground.search(v);
    if (v.y <= ground.getHeight()) {
        v.y = ground.getHeight();
        mVel.y *= -sRelReflectK;
        _1b0++;

        setAllCheckKartHitFlag();
        setObjFlagCheckItemHitting();
        setBoundSE();
        
        if(isResetGndAttr(ground.getAttribute())) {
            resetPose(true);
            v.scaleAdd(-sFootLength, mUpDir, mPos);
        }
        if (mVel.y <= 1.0f || _1b0 >= 5) {
            mVel.y = 0.0f;
            setState(0);
        }
    }

    mPos.scaleAdd(sFootLength, mUpDir, v);

}

bool GeoRabbitMark::isResetGndAttr(CrsGround::EAttr attr) {
    bool ret = false;
    switch (attr) {
    case CrsGround::Attr_5:
    case CrsGround::Attr_10:
    case CrsGround::Attr_14:
    case CrsGround::Attr_15:
    case CrsGround::Attr_255:
        ret = true;
        break;
    }
    return ret;
}

void GeoRabbitMark::setBoundSE() {
    if (mVel.y >= 1.0f) {
        switch (_1b0) {
        case 1: {
            getSoundMgr()->setSe(0x40070);
            break;
        }
        case 2: {
            getSoundMgr()->setSe(0x40071);
            break;
        }
        case 3: {
            getSoundMgr()->setSe(0x40072);
            break;
        }
        }
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
