#include "Inagaki/GameAudioMain.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/JAudio/System/JASKernel.h"
#include "JSystem/JGeometry/Quat.h"
#include "JSystem/JGeometry/Util.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/ItemThunder.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjCollision.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjDonkyRock.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "dolphin/mtx.h"
#include "mathHelper.h"
#include "types.h"

StateObserver::StateFuncSet<TMapObjDonkyRock> TMapObjDonkyRock::sTable[3] = {
    { 0, &TMapObjDonkyRock::initFunc_Wait, &TMapObjDonkyRock::doFunc_Wait },
    { 1, &TMapObjDonkyRock::initFunc_Fall, &TMapObjDonkyRock::doFunc_Fall },
    { 2, &TMapObjDonkyRock::initFunc_Break, &TMapObjDonkyRock::doFunc_Break },
};
TDonkyRockSupervisor *TMapObjDonkyRock::sSupervisor;
f32 TMapObjDonkyRock::sReflectRate = 0.55f;
f32 TMapObjDonkyRock::sFricRate = 0.99f;
f32 TMapObjDonkyRock::sGravity = -1.0f;
f32 TMapObjDonkyRock::sMaxSpeed = 80.0f;
f32 TMapObjDonkyRock::sRotSpeed = 0.0031f;
f32 TMapObjDonkyRock::sKartDownScale = 0.3f;

TMapObjDonkyRock::TMapObjDonkyRock() : TMapObjHioNode(0x1197), StateObserver(), _170(this) {
    mGenObjData = nullptr;
    setObjFlagSimpleDraw();
    mCrsGround = new CrsGround(RaceMgr::getManager()->getCourse());
    if (getSupervisor() == nullptr) {
        sSupervisor = new TDonkyRockSupervisor();
    }
    sSupervisor->entry(this);
}

TMapObjDonkyRock::~TMapObjDonkyRock() {
    delete mCrsGround;
    sSupervisor = nullptr;
}

void TMapObjDonkyRock::reset() {
    const CrsData::SObject *genObjData;
    ShadowModel *shadowModel;
    
    GeographyObj::resetObject();
    genObjData = mGenObjData;
    mScale.set(genObjData->scale);
    mBounds[0]->setScale(mScale);
    _58 = 0xb;
    _184.zero();
    _190 = 0.0f;
    _194 = 0;
    StateObserver::ResetState();
    clrObjFlagCheckGeoHitting();
    setObjFlagHidding();
    shadowModel = mShadowMdl;
    if (shadowModel != nullptr) {
        shadowModel->clrVisibleAll();
    }
    return;
}

void TMapObjDonkyRock::createColModel(J3DModelData *modelData) {
    createBoundsSphere(modelData);
}

const char *TMapObjDonkyRock::getBmdFileName() {
    static const char *cBmdName = "/Objects/DonkyRock1.bmd";
    return cBmdName;
}

const char *TMapObjDonkyRock::getShadowBmdFileName() {
    static const char *cShadowBmdName = "/Objects/DonkyRock1Shadow.bmd";
    return cShadowBmdName;
}

// FIX: Register mismatch for `mGenObjData->position.y + _180`
void TMapObjDonkyRock::start() {
    #line 166
    JUT_ASSERT(mGenObjData != 0);
    #line 168
    JUT_ASSERT(mGenObjData->mPathID != 0xffff);

    mPos.set(mGenObjData->position.x, mGenObjData->position.y + _180, mGenObjData->position.z);

    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    clrObjFlagHidding();
    if (mShadowMdl != nullptr) {
        mShadowMdl->setVisibleAll();
    }
    moveShadowModel();
    _58 = 0xb;
    setState(1);
}

void TMapObjDonkyRock::loadAnimation() {}

void TMapObjDonkyRock::createModel(JKRSolidHeap *heap, u32 param_2) {
    mModel.createModel(heap, param_2, 0);
}

void TMapObjDonkyRock::setObjData(const CrsData::SObject *sObject) {
    mGenObjData = sObject;
    _180 = mGenObjData->scale.y * 300.0f;
    _196 = RaceMgr::getManager()->getCourse()->getCrsData()->
            getPathData(mGenObjData->mPathID)->pointCount;
    mRockSoundMgr = new GameAudio::RockSoundMgr(&mPos, sObject->scale.y, nullptr);
    setStartVel();
}

void TMapObjDonkyRock::calc() {
    ExecuteState();
    mRockSoundMgr->frameWork();
}

void TMapObjDonkyRock::initFunc_Wait() {
    setObjFlagHidding();
    if (mShadowMdl == nullptr) {
        return;
    }
    mShadowMdl->clrVisibleAll();
}

void TMapObjDonkyRock::doFunc_Wait() {}

void TMapObjDonkyRock::initFunc_Fall() {
    mVel.set(_198);
    _184.zero();
    _190 = 0.0f;
    _194 = 1;
    clrObjFlagHidding();
    setAllCheckKartHitFlag();
    if (mShadowMdl == nullptr) {
        return;
    }
    mShadowMdl->setVisibleAll();
}

// FIX: Register mismatch and missing `fmr` call around `setRotate` call.
void TMapObjDonkyRock::setStartVel() {
    static JGeometry::TVec3f axisY(0.0f, 1.0f, 0.0f);

    f32 speed = mGenObjData->mParam3;

    if (speed > 0.0f) {
        JGeometry::TVec3f nodePos;
        getNodePosition(&nodePos, 1);

        _198.sub(nodePos, mGenObjData->position);
        _198.y = 0.0f;
        _198.normalize();

        JGeometry::TVec3f axis;
        axis.cross(_198, axisY);
        axis.normalize();

        JGeometry::TQuat4f rot;
        rot.setRotate(axis, MTXDegToRad((f32)mGenObjData->mParam4));
        rot.transform(_198);

        _198.scale(speed);
    } else {
        _198.zero();
    }
}

void TMapObjDonkyRock::doFunc_Fall() {
    u16 nodeIdx = _194;
    if (nodeIdx >= _196) {
        nodeIdx = _196 - 1;
    }

    JGeometry::TVec3f pos;
    getNodePosition(&pos, nodeIdx);
    move_FallDown(pos);

    GeographyObj::moveShadowModel();
    pos.sub(mPos);

    f32 sq = pos.squaredZX();
    if (pos.y > 0.0f || sq < (_180 * _180)) {
        _194++;
    }

    if (_194 >= _196) {
        setState(2);
    }
    if (tstItemHitting() != 0) {
        // What is 7 in this context?
        if (getColItemObj()->getKind() - 7 <= 2 || (getColItemObj()->getKind() == 1)) {
            setState(2);
        }
    }
}

void TMapObjDonkyRock::move_FallDown(JGeometry::TVec3f &param_1) {
    JGeometry::TVec3f *mPosCopy;
    JGeometry::TVec3f local_a4;
    JGeometry::TVec3f local_b0;
    JGeometry::TVec3f local_bc;
    JGeometry::TVec3f local_c8;
    
    mVel.add(_184);
    if (mVel.squared() > sMaxSpeed * sMaxSpeed) {
        mVel.normalize();
        mVel.scale(sMaxSpeed);
    }
    mPosCopy = &mPos;
    mPosCopy->add(mVel);
    _184.set(0.0f, sGravity, 0.0f);
    mCrsGround->search(*mPosCopy);

    u32 attr = mCrsGround->getAttribute();
    static JGeometry::TVec3f axisY(0.0f, 1.0f, 0.0f);

    if (attr != CrsGround::Attr_10 && mPos.y < _180 + mCrsGround->getHeight()) {
        mCrsGround->getNormal(&local_c8);
        local_a4.normalize(local_c8);
        local_bc.sub(param_1, mPos);
        local_bc.normalize();
        PSVECCrossProduct(&local_bc, &local_a4, &local_b0);
        local_b0.normalize();
        mPos.y = _180 + mCrsGround->getHeight();
        getReflect(local_a4, local_bc);
    }
    
    local_bc.set(mVel.x, 0.0f, mVel.z);

    if (!local_bc.isZero()) {
        local_a4.set(axisY);
        local_bc.normalize();

        PSVECCrossProduct(&local_bc, &local_a4, &local_b0);
        local_b0.normalize();
        _190 += (-sRotSpeed * mVel.length()) / mScale.y;

        f32 fVar1;
        for (fVar1 = _190; fVar1 < -JGeometry::TUtilf::PI();) {
            fVar1 += 2.0f * JGeometry::TUtilf::PI();
        }

        while (fVar1 >= JGeometry::TUtilf::PI()) {
            fVar1 -= 2.0f * JGeometry::TUtilf::PI();
        }

        _190 = fVar1;
        mRotMtx.setRotate(local_b0, _190);
    }
}
// FIX: Register mismatch - probably from squared() function.
void TMapObjDonkyRock::getReflect(JGeometry::TVec3f &normal, JGeometry::TVec3f &direction) {
    JGeometry::TVec3f proj;
    proj.scale(normal.dot(mVel), normal);
    
    if (proj.squared() <= (100.0f + sGravity * sGravity)) {
        proj.scale(-0.9f);
        mVel.add(proj);
        mVel.scale(sFricRate);

        mRockSoundMgr->setRollSe(mVel.length());
    } else {
        proj.scale(-(1.0f + sReflectRate));
        mVel.add(proj);

        f32 speed = mVel.length();

        mVel.normalize();
        mVel.x = direction.x;
        mVel.z = direction.z;

        mVel.scale(speed);
        mRockSoundMgr->setBoundSe(speed);
    }
}

void TMapObjDonkyRock::getNodePosition(JGeometry::TVec3f *param_1, u16 param_2) {
    CrsData::PointData *pPVar1 = RCMGetCourse()->getCrsData()->getPointData(mGenObjData->mPathID, param_2);
    param_1->set(pPVar1->pos);
    param_1->y += _180;
}

void TMapObjDonkyRock::initFunc_Break() {
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    if (mShadowMdl != nullptr) {
        mShadowMdl->clrVisibleAll();
    }
    setObjFlagHidding();
    JPABaseEmitter *baseEmitter = JPEffectMgr::getEffectMgr()->createEmt("mk_donkyRock_a", mPos);
    baseEmitter->setGlobalScale(mScale);
    baseEmitter = JPEffectMgr::getEffectMgr()->createEmt("mk_donkyRock_b", mPos);
    baseEmitter->setGlobalScale(mScale);
    mRockSoundMgr->setBrokenSe();
}

void TMapObjDonkyRock::doFunc_Break() {
    setState(0);
}

void TMapObjDonkyRock::doKartColCallBack(int kartIdx) {
    bool changeState = false;

    if (GetKartCtrl()->getKartBody(kartIdx)->mCarStatus & 0x100040004ULL) {
        _58 = 11;
        changeState = true;
    } else if (GetKartCtrl()->getKartBody(kartIdx)->mCarStatus & 0x4000ULL) {
        _58 = 11;

        if (RCMGetManager()->getRaceMode() != 1) {
            changeState = true;
        }
    } else if (mScale.y < sKartDownScale) {
        _58 = 0;
        changeState = true;
    } else {
        _58 = 11;
    }

    if (changeState) {
        setState(2);
    }
}

void TMapObjDonkyRock::MoveExec() {
    Observer_FindAndExec(TMapObjDonkyRock, 3);
}

void TMapObjDonkyRock::InitExec() {
    Observer_FindAndInit(TMapObjDonkyRock, 3);
}    


TDonkyRockSupervisor::TDonkyRockSupervisor() {
    mList.initiate();
    ItemThunder::entrySuperVisor(this);
}

TDonkyRockSupervisor::~TDonkyRockSupervisor() {}

void TDonkyRockSupervisor::entry(TMapObjDonkyRock *tMapObjDonkyRock) {
    mList.append(&tMapObjDonkyRock->_170);
}

void TDonkyRockSupervisor::thunderDownAll() {
    for (JSULink<TMapObjDonkyRock> *donkyRockItem = mList.getFirst(); donkyRockItem != nullptr; donkyRockItem = donkyRockItem->getNext()) {
        donkyRockItem->getObject()->setState(2);
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
