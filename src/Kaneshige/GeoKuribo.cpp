#include "Kaneshige/Objects/GeoKuribo.h"
#include "JSystem/J3D/J3DAnmLoader.h"
#include "JSystem/J3D/J3DFrameCtrl.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/ItemObjMgr.h"
#include "Sato/ItemThunder.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Yamamoto/kartCtrl.h"
#include <std/math.h>

GeoKuriboSupervisor *GeoKuribo::sSupervisor;
s16 GeoKuribo::sSupervisorCreateNum;
J3DAnmTransform *GeoKuribo::sAnmTransWalkL;
J3DAnmTransform *GeoKuribo::sAnmTransWalkR;
J3DMtxCalc *GeoKuribo::sMtxCalcWalkL;
J3DMtxCalc *GeoKuribo::sMtxCalcWalkR;

f32 GeoKuribo::sWalkAccl = 0.1f;
f32 GeoKuribo::sWalkMaxVel = 10.0f;
f32 GeoKuribo::sWalkTurnRate = 0.02f;
f32 GeoKuribo::sPointR = 100.0f;
f32 GeoKuribo::sOccurVel0 = 160.0f;
f32 GeoKuribo::sShadowScale = 0.88f;
f32 GeoKuribo::sWalkAnmFrame = 25.0f;
s16 GeoKuribo::sCrushAnmFrame = 5;
s16 GeoKuribo::sBlendAnmFrame = 30;

StateObserver::StateFuncSet<GeoKuribo> GeoKuribo::sTable[4] = {
    {0, &GeoKuribo::initFuncWait, &GeoKuribo::doFuncWait},
    {1, &GeoKuribo::initFuncWalk, &GeoKuribo::doFuncWalk},
    {2, &GeoKuribo::initFuncCrush, &GeoKuribo::doFuncCrush},
    {3, &GeoKuribo::initFuncRecover, &GeoKuribo::doFuncRecover}
};

const u8 GeoKuribo::cNazoTable[8] = {
    0x46, 0x45, 0x45, 0x4C,
    0x20, 0x4D, 0x49, 0x45
};

#include "JSystem/JAudio/JASFakeMatch2.h"

GeoKuriboSupervisor::GeoKuriboSupervisor() {
    mList.initiate();
    ItemThunder::entrySuperVisor(this);
}

GeoKuriboSupervisor::~GeoKuriboSupervisor() {
    GeoKuribo::sSupervisor = nullptr;
}

void GeoKuriboSupervisor::reset() {}

void GeoKuriboSupervisor::calc() {}

void GeoKuriboSupervisor::entry(GeoKuribo *kuribo) {
    mList.append(&kuribo->mLink);
}

void GeoKuriboSupervisor::thunderDownAll() {
    for (JSUListIterator<GeoKuribo> it(mList.getFirst()); it != mList.getEnd(); it++) {
        GeoKuribo *kuribo = it.getObject();
        kuribo->thunderDown();
    }
}

GeoKuribo::GeoKuribo(const CrsData::SObject &obj) : GeographyObj(obj), mLink(this) {
    if (sSupervisor == nullptr) {
        sSupervisor = new GeoKuriboSupervisor();
    }
    sSupervisor->entry(this);
    GeographyObj::NewAnmCtrl();
    GeographyObj::createSoundMgr();
    mNazo = cNazoTable[(u32)this / 16 & 7];
    reset();
}

void GeoKuribo::reset() {
    GeographyObj::resetObject();
    _58 = 3;
    GeographyObj::clrObjFlagCheckGeoHitting();
    GeographyObj::setAllCheckKartHitFlag();
    GeographyObj::setObjFlagCheckItemHitting();
    StateObserver::ResetState();
    StateObserver::setState(1);
    
    mFrDir.set(mObjData->zNorm);
    mFrDir.scale(0.0001f);
    mUpDir.set(mObjData->yNorm);
    mUpDir.scale(0.0001f);
    mLfDir.cross(mUpDir, mFrDir);
    mLfDir.normalize();
    
    mYScale = 1.0f;
    mPointFlags = 0;
    mPointID = 0;
    mGround.reset();
    mMaxWalkAccl = sWalkAccl;
    if (mObjData->mParam2 != 0) {
        mMaxWalkAccl = mObjData->mParam2 * 0.01f;
    }
    mWalkAccl = mMaxWalkAccl;
    mWalkDir = 1.0f;
    mWalkVel = 0.0f;
    mMaxWalkVel = sWalkMaxVel;
    mRate = 1.0;
    if (mObjData->mParam3 != 0) {
        mRate = mObjData->mParam3 * 0.01f;
    }
    
    mCrushTimer = 0;
    mMaxCrushTime = 300;
    if (mObjData->mParam1 != 0) {
        mMaxCrushTime = mObjData->mParam1;
    }

    mRecoverState = 0;
    _218 = 0.0f;
    _21c = 0.0;
    resetAnimation();
#line 326
    JUT_ASSERT_MSG(mObjData->mPathID != 0xffff, "UNLINK PATH");

    CrsData *crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    mPos.set(crsData->getPointData(mObjData->mPathID, 0)->pos);
    mGround.search(mPos);
    mPos.y = mGround.getHeight();
    mGround.getNormal(&mUpDir);
    mLfDir.cross(mUpDir, mFrDir);
    mLfDir.normalize();
    resetWalkDirection(0);
}

void GeoKuribo::resetAnimation() {
    if (GeographyObj::getAnmCtrl()->IsAvailableTrans()) {
        for (u8 i = 0; i < 2; i++) {
            GeographyObj::getAnmCtrl()->getFrameCtrl(i)->setAttribute(2);
            GeographyObj::getAnmCtrl()->getFrameCtrl(i)->setRate(mRate);
        }
        GeographyObj::getAnmCtrl()->Reset();
    }
}

void GeoKuribo::resetWalkDirection(s16 pointID) {
    CrsData *crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    s16 startID = pointID; // fakematch?

    CrsData::PathData *pathData = crsData->getPathData(mObjData->mPathID);
    u16 pointCount = (u16)pathData->pointCount;

    if (tstKuriboFlagDecPoint()) {
        if (mPointID > 0) {
            startID--;
        }
    }
    else if (mPointID < (pointCount - 1)) {
        startID++;
    }
    mPointID = startID;
    CrsData::PointData *pointData = crsData->getPointData(mObjData->mPathID, mPointID);

    JGeometry::TVec3f vel;
    vel.sub(pointData->pos, mPos);

    if (vel.dot(mLfDir) >= 0.0f) { // nooo not the inclusive comparison!
        mWalkDir = 1.0f;
        GeographyObj::getAnmCtrl()->ChangeBlendTransAnm(0, sBlendAnmFrame, 1);
        if (GeographyObj::getAnmCtrl()->IsAvailableTrans()) {
          J3DFrameCtrl *frameCtrl = GeographyObj::getAnmCtrl()->getFrameCtrl(0);
          frameCtrl->setRate(mRate);
        }
    }
    else {
        mWalkDir = -1.0f;
        GeographyObj::getAnmCtrl()->ChangeBlendTransAnm(1, sBlendAnmFrame, 1);
        if (GeographyObj::getAnmCtrl()->IsAvailableTrans()) {
          J3DFrameCtrl *frameCtrl = GeographyObj::getAnmCtrl()->getFrameCtrl(1);
          frameCtrl->setRate(mRate);
        }
    }

    mWalkAccl = mMaxWalkAccl;
    mWalkVel = 0.0f;
}

void GeoKuribo::loadAnimation() {
    J3DModelData *mdlData = mModel.getModelData();
    void *bca_r = ObjUtility::getPtrCourseArc("/Objects/Kuribo1_r.bca");
    void *bca_l = ObjUtility::getPtrCourseArc("/Objects/Kuribo1_l.bca");
    J3DAnmObjTrans::loadTransAnm(&sAnmTransWalkL, bca_r);
    J3DAnmObjTrans::loadTransAnm(&sAnmTransWalkR, bca_l);

    sMtxCalcWalkL = J3DUNewMtxCalcAnm(mdlData->getMtxCalcType(), sAnmTransWalkL, sAnmTransWalkR, nullptr, nullptr, MTXCalc_Blend);
    sMtxCalcWalkR = J3DUNewMtxCalcAnm(mdlData->getMtxCalcType(), sAnmTransWalkR, sAnmTransWalkL, nullptr, nullptr, MTXCalc_Blend);
}

void GeoKuribo::createModel(JKRSolidHeap *slid, u32 viewNo) {
    mModel.createModel(slid, viewNo, 0);
    GeographyObj::getAnmCtrl()->InitRegistration(2, &mModel);
    GeographyObj::getAnmCtrl()->RegisterTransBlend(0, sAnmTransWalkL, sMtxCalcWalkL);
    GeographyObj::getAnmCtrl()->RegisterTransBlend(1, sAnmTransWalkR, sMtxCalcWalkR);
    resetAnimation();
}

void GeoKuribo::createColModel(J3DModelData *mdlData) {
    GeographyObj::createBoundsCylinder(mdlData, 80.0f, 170.0f);
}

void GeoKuribo::doKartColCallBack(int kartNo) {
    if (GeographyObj::tstIsHitKartFlg(kartNo)) {
        changeStateCrush(kartNo);
    }
}

void GeoKuribo::changeStateCrush(int kartNo) {
    bool crushed = false;
    if (kartNo >= 0) {
        if (KartCtrl::getKartCtrl()->GetKartStatus(kartNo) & 0x40000) {
            crushed = true;
        }
    }
    else {
        crushed = true;
    }

    if (crushed && StateObserver::getState() != 2) {
        StateObserver::setState(2);
        GeographyObj::clrAllCheckKartHitFlag();
        GeographyObj::clrObjFlagCheckItemHitting();
        occurKinoko(-1);
    }
}

void GeoKuribo::thunderDown() {
    changeStateCrush(-1);
}

void GeoKuribo::calc() {
    checkItemHitting();
    StateObserver::ExecuteState();
    mScale.y = mObjData->scale.y * mYScale;
    mRotMtx.setXYZDir(mLfDir, mUpDir, mFrDir);
    GeographyObj::moveShadowModel();

    JGeometry::TVec3f shadowScale;
    shadowScale.setAll(sShadowScale);
    GeographyObj::getShadowModel()->getModel()->setBaseScale(shadowScale);
}

void GeoKuribo::checkItemHitting() {
    if (GeographyObj::tstItemHitting()) {
        bool crushed = false;
        ItemObj *itemObj = GeographyObj::getColItemObj();

        switch (itemObj->getState()) {
        case 2:
        case 3:
        case 8:
        case 10: {
            switch (itemObj->getKind()) {
            case 0:
            case 1:
            case 2:
            case 7:
            case 8:
            case 9:
            case 0xb:
            case 0xd: {
                crushed = true;
                break;
            }
            break;
            }
        }        
        }
        if (crushed) {
            changeStateCrush(-1);
        }
    }
}

void GeoKuribo::occurKinoko(int p1) {
    bool positive = true;
    if (p1 >= 0) {
        JGeometry::TVec3f dir;
        ObjUtility::getKartZdir(p1, &dir);
        if (dir.dot(mFrDir) < 0.0f) {
            positive = false;
        }
    }
    else {
        positive = false;
    }

    JGeometry::TVec3f vel;
    JGeometry::TVec3f frDir(mFrDir);
    if (!positive) {
        frDir.scale(-1.0f);
    }
    vel.scaleAdd(0.3f, mUpDir, frDir);
    vel.normalize();
    vel.scale(sOccurVel0);

    JGeometry::TVec3f pos;
    if (positive) {
        pos.scaleAdd(10.0f, mFrDir, mPos);
    }
    else {
        pos.scaleAdd(-10.0f, mFrDir, mPos);
    }

    GetItemObjMgr()->occurItem(5, pos, vel);
}

void GeoKuribo::InitExec() { Observer_FindAndInit(GeoKuribo, 4); }

void GeoKuribo::MoveExec() { Observer_FindAndExec(GeoKuribo, 4); }

void GeoKuribo::initFuncWait() {}

void GeoKuribo::doFuncWait() {}

void GeoKuribo::initFuncWalk() {
    mWalkAccl = mMaxWalkAccl;
    mWalkVel = 0.0f;
}

void GeoKuribo::doFuncWalk() {
    CrsData::PointData *pointData = RaceMgr::getManager()->getCourse()->getCrsData()->getPointData(mObjData->mPathID, mPointID);

    u8 anm_no = GeographyObj::getAnmCtrl()->AnmController::getNowTransNo();
    J3DFrameCtrl *ctrl = GeographyObj::getAnmCtrl()->AnmController::getFrameCtrl(anm_no);
    f32 frame = ctrl->getFrame();
    if (frame >= 0.0f && frame < sWalkAnmFrame) {
        mWalkAccl = -mMaxWalkAccl;
    }
    else {
        mWalkAccl = mMaxWalkAccl;
    }

    mWalkVel += mWalkAccl;
    if (mWalkVel > mMaxWalkVel) {
        mWalkVel = mMaxWalkVel;
    }
    else if (mWalkVel < 0.0f) {
        mWalkVel = 0.0f;
    }

    mVel.set(mLfDir);
    mVel.scale(mWalkVel * mWalkDir);
    mPos.add(mVel);
    mGround.search(mPos);
    mPos.y = mGround.getHeight();

    JGeometry::TVec3f vel1;
    vel1.sub(pointData->pos, mPos);
    vel1.normalize();

    if (mWalkDir >= 0.0f) {
        mLfDir.scaleAdd(sWalkTurnRate, vel1, mLfDir);
    }
    else {
        mLfDir.scaleAdd(-sWalkTurnRate, vel1, mLfDir);
    }
    mLfDir.normalize();

    JGeometry::TVec3f normal;
    mGround.getNormal(&normal);

    mUpDir.scaleAdd(0.1f, normal, mUpDir);
    mUpDir.normalize();
    mFrDir.cross(mLfDir, mUpDir);
    mFrDir.normalize();
    mLfDir.cross(mUpDir, mFrDir);
    mLfDir.normalize();

    JGeometry::TVec3f vel2;
    vel2.sub(pointData->pos, mPos);

    if (vel2.squared() < (sPointR*sPointR)) {
        CrsData::PathData *pathData = RCMGetCourse()->getCrsData()->getPathData(mObjData->mPathID);
        s16 pointId = pathData->pointCount;
        u8 anm_no = GeographyObj::getAnmCtrl()->AnmController::getNowTransNo();
        J3DFrameCtrl *ctrl = GeographyObj::getAnmCtrl()->AnmController::getFrameCtrl(anm_no);
        BOOL anmState = ctrl->checkState(2);

        if (tstKuriboFlagDecPoint()) {
            if (mPointID <= 0) {
                mWalkAccl = -mMaxWalkAccl;
                if (anmState) {
                    clrKuriboFlagDecPoint();
                    resetWalkDirection(0);
                }
            }
            else {
                mPointID--;
            }
        }
        else if (mPointID >= (pointId -1)) {
            mWalkAccl = -mMaxWalkAccl;
            if (anmState) {
                setKuriboFlagDecPoint();
                resetWalkDirection(pointId - 1);
            }
        }
        else {
            mPointID++;
        }
    }

}

void GeoKuribo::initFuncCrush() {
    mCrushTimer = 0;
    u8 trans_no = GeographyObj::getAnmCtrl()->getNowTransNo();
    GeographyObj::getAnmCtrl()->getFrameCtrl(trans_no)->setFrame(sCrushAnmFrame);
    GeographyObj::getAnmCtrl()->getFrameCtrl(trans_no)->stop();
    GeographyObj::getSoundMgr()->setSe(0x4007c); 
}

void GeoKuribo::doFuncCrush() {
    mCrushTimer++;
    if (mCrushTimer >= mMaxCrushTime) {
        StateObserver::setState(3);
    }
    f32 crushFrame = mCrushTimer;
    if (crushFrame > 5.0f) {
        crushFrame = 5.0f;
    }

    mYScale = (1.0f - crushFrame / 5.0f) * 0.9f + 0.1f;
}

void GeoKuribo::initFuncRecover() {
    mCrushTimer = 0;
    mRecoverState = 0;
    _218 = 0.7f;
    _21c = 0.0f;
    GeographyObj::getSoundMgr()->setSe(0x4007d); 
}

void GeoKuribo::doFuncRecover() {
    switch (mRecoverState) {
    case 0: {
        mYScale += 0.5f;
        if (mYScale >= 1.3f) {
            mYScale = 1.3f;
            mRecoverState = 1;
        }
        break;
    }
    case 1: {
        _218 -= 0.04f;
        if (_218 < 0.0f) {
            _218 = 0.0f;
        }
        _21c += 0.397f;
        mYScale = _218 * std::sinf(_21c) + 1.0f;
        if (mYScale == 1.0f) {
            StateObserver::setState(1);
            GeographyObj::setAllCheckKartHitFlag();
            GeographyObj::setObjFlagCheckItemHitting();

            u8 anm_no = GeographyObj::getAnmCtrl()->AnmController::getNowTransNo();
            J3DFrameCtrl *ctrl = GeographyObj::getAnmCtrl()->AnmController::getFrameCtrl(anm_no);
            ctrl->setRate(mRate);
        }
        break;
    }
    }
}
