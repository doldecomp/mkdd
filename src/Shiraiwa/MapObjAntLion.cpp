#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3D/J3DJoint.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "Kaneshige/Course/CrsData.h"
#include "Shiraiwa/Objects/MapObjAntLion.h"
#include "Sato/AnmController.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "types.h"

StateObserver::StateFuncSet<TMapObjAntLion> TMapObjAntLion::sTable[3] = {
    { 0, &TMapObjAntLion::initFunc_Start, &TMapObjAntLion::doFunc_Start },
    { 1, &TMapObjAntLion::initFunc_Wait, &TMapObjAntLion::doFunc_Wait },
    { 2, &TMapObjAntLion::initFunc_Eat, &TMapObjAntLion::doFunc_Eat }
};

J3DAnmTransform *TMapObjAntLion::sAntLionBckAnmTrans;
J3DMtxCalc *TMapObjAntLion::sAntLionBckMtxCalc;
const char *TMapObjAntLion::scJointName = "skl_loot";
s32 TMapObjAntLion::sJointNo;

TMapObjAntLion::TMapObjAntLion(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
    createSoundMgr();
}

TMapObjAntLion::~TMapObjAntLion() {
    delete mCircleSoundMgr;
}

const char *TMapObjAntLion::getBmdFileName() {
    static const char *cBmdName = "/Objects/AntLion.bmd";
    return cBmdName;
}

void TMapObjAntLion::createColModel(J3DModelData *modelData) {
    createBoundsCylinder(1500.0f, 600.0f, 1.0f);
    JGeometry::TVec3f newColObjPos(0.0f, mObjData->mParam1 / mScale.y, 0.0f);
    setColObjPos(newColObjPos, 0);
}

void TMapObjAntLion::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmObjTrans->setupTransAnmData(&sAntLionBckAnmTrans, &sAntLionBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/AntLion_eat.bck"));
}

// FIX: Register mismatch.
void TMapObjAntLion::createModel(JKRSolidHeap *heap, u32 p2) {
    J3DJoint *joint;
    mModel.createModel(heap, p2, 0);
    
    AnmController *anmCtrl = mAnmCtrl;
    anmCtrl->mTrans = new AnmControlTrans();
    anmCtrl->mTrans->initAnm(1, &mModel);
    mAnmCtrl->mTrans->registration(0, sAntLionBckAnmTrans, sAntLionBckMtxCalc);

    joint = mModel.getModelData()->getJointTree().getJointNodePointer(0);
    mCircleSoundMgr = new GameAudio::CircleSoundMgr(&mPos, mScale.y * joint->getRadius(), 0.0f, 0.0f, nullptr);

    sJointNo = mModel.getModelData()->getJointName()->getIndex(scJointName);
}

void TMapObjAntLion::InitExec() { Observer_FindAndInit(TMapObjAntLion, 3); }

void TMapObjAntLion::MoveExec() { Observer_FindAndExec(TMapObjAntLion, 3); }

void TMapObjAntLion::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = 17;
    mAnmCtrl->Reset();
    ResetState();
}

void TMapObjAntLion::calc() {
    ExecuteState();
    mCircleSoundMgr->setSe(0x40025);
    mCircleSoundMgr->frameWork();
}

void TMapObjAntLion::initFunc_Start() {
    JGeometry::TVec3f local_18;
    local_18.zero();
    GetJPAMgr()->createEmt("mk_arijigoku_a", local_18);
}

void TMapObjAntLion::doFunc_Start() {
    setState(1);
}

void TMapObjAntLion::initFunc_Wait() {
    mAnmCtrl->mTrans->getFrameCtrl(0)->setFrame(0.0f);
    if (mAnmCtrl->mTrans != nullptr) {
        mAnmCtrl->mTrans->setFlag();
    }
}

void TMapObjAntLion::doFunc_Wait() {}

void TMapObjAntLion::initFunc_Eat() {
    JGeometry::TPos3f *anmMtx;
    JGeometry::TVec3f emtVec;
    JGeometry::TPos3f newRotMtx;

    static const char *emtName[2] = {
        "mk_arijigoku_paku_a",
        "mk_arijigoku_paku_b",
    };

    mAnmCtrl->mTrans->getFrameCtrl(0)->reset();

    if (mAnmCtrl->mTrans != nullptr) {
        mAnmCtrl->mTrans->resetFlag();
    }

    getSoundMgr()->setSe(0x4002e);
    newRotMtx.set(mRotMtx);
    newRotMtx.zeroTrans();

    anmMtx = (JGeometry::TPos3f *)mModel.getModel()->mMtxBuffer->getAnmMtx(0);
    emtVec.set(anmMtx[sJointNo][0][3], anmMtx[sJointNo][1][3], anmMtx[sJointNo][2][3]);

    for (int i = 0; i < 2; i++) {
        JPABaseEmitter *emt = GetJPAMgr()->createEmt(emtName[i], emtVec);
        JPASetRMtxfromMtx(newRotMtx.mMtx, emt->mGlobalRot);
    }
}

void TMapObjAntLion::doFunc_Eat() {
    if (mAnmCtrl->mTrans->getFrameCtrl(0)->checkState(1)) {
        setState(1);
    }
}

void TMapObjAntLion::getZDir(JGeometry::TVec3f *vecDirZ) {
    mRotMtx.getZDir(*vecDirZ);
    vecDirZ->normalize();
}

void TMapObjAntLion::doKartColCallBack(int) {}

#include "JSystem/JAudio/JASFakeMatch2.h"
