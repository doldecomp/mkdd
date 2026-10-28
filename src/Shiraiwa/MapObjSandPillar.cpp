#include "Shiraiwa/Objects/MapObjSandPillar.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JParticle/JPAMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/AnmController.h"
#include "Sato/GeographyObj.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/TKartThrower.h"
#include "dolphin/mtx.h"
#include "mathHelper.h"

f32 TMapObjSandPillar::sItemVel = 50.0f;
f32 TMapObjSandPillar::sItemUpVel = 0.08f;

J3DAnmTransform *TMapObjSandPillar::sSandPillarBckAnmTrans;
J3DMtxCalc *TMapObjSandPillar::sSandPillarBckMtxCalc;
J3DAnmTextureSRTKey *TMapObjSandPillar::sSandPillarBtkAnm;

StateObserver::StateFuncSet<TMapObjSandPillar> TMapObjSandPillar::sTable[1] = {
    { 0, &TMapObjSandPillar::initFunc_Walk, &TMapObjSandPillar::doFunc_Walk }
};

TMapObjSandPillar::TMapObjSandPillar(const CrsData::SObject &sObject) : TKartThrower(sObject), StateObserver() {
    NewAnmCtrl();
    mPathWalk = new TPathWalk(&sObject);
};

TMapObjSandPillar::~TMapObjSandPillar() {
    delete mPathWalk;
}

void TMapObjSandPillar::InitExec() { Observer_FindAndInit(TMapObjSandPillar, 1); }

void TMapObjSandPillar::MoveExec() { Observer_FindAndExec(TMapObjSandPillar, 1); }

void TMapObjSandPillar::getKartThrowDirPow(JGeometry::TVec3f *param_1, f32 *param_2, int param_3) {
    if (param_1 != nullptr) {
        param_1->set(mVel);
        if (param_1->isZero()) {
            param_1->set(mObjData->zNorm);
        }
        param_1->y = 0.0f;
        param_1->normalize();
    }

    if (param_2 != nullptr) {
        s32 uVar3 = mObjData->mParam2;
        if (uVar3 <= 0) {
            uVar3 = 0x32;
        }
        *param_2 = uVar3;
    }
}

void TMapObjSandPillar::getThrowDir(JGeometry::TVec3f *param_1, s16 param_2) {
    param_1->set(mVel);
    param_1->y = 0.0f;
    param_1->normalize();
}

s32 TMapObjSandPillar::getThrowPow() {
    s32 throwPow = mObjData->mParam2;

    if (throwPow <= 0) {
        throwPow = 0x32;
    }
    return throwPow;
}

const char *TMapObjSandPillar::getBmdFileName() {
    static const char *cBmdName = "/Objects/Tornado.bmd";
    return cBmdName;
}

void TMapObjSandPillar::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjTrans::setupTransAnmData(&sSandPillarBckAnmTrans, &sSandPillarBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/Tornado.bca"));
    mAnmObjMat.setupTexSRTAnmData(&sSandPillarBtkAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/Tornado.btk"));
}

void TMapObjSandPillar::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x200, 0);

    AnmController *anmCtrl = mAnmCtrl;
    anmCtrl->mTrans = new AnmControlTrans();
    anmCtrl->mTrans->initAnm(1, &mModel);

    mAnmCtrl->mTrans->registration(0, sSandPillarBckAnmTrans, sSandPillarBckMtxCalc);
    mAnmCtrl->mTrans->getFrameCtrl(0)->setAttribute(2);

    mAnmObjMat.setExModel(&mModel);
    mAnmObjMat.setAnmBase(sSandPillarBtkAnm);
    mAnmObjMat.initFrameCtrl(mAnmObjMat.getAnmBase());

    f32 scaleX = mScale.x > mScale.z
        ? mScale.x 
        : mScale.z;

    mCircleSoundMgr = new GameAudio::CircleSoundMgr(&mPos, (scaleX * 330.0f), 0.0f, mScale.y * 2000.0f, nullptr);
}

void TMapObjSandPillar::createColModel(J3DModelData *modelData) {
    f32 scaleXZ = mScale.x > mScale.z 
        ? mScale.x 
        : mScale.z;

    f32 max = scaleXZ > mScale.y 
        ? scaleXZ 
        : mScale.y;

    createBoundsCylinder(modelData, 330.0f * (scaleXZ / max),
        2000.0f * (mScale.y / max)
    );
}

void TMapObjSandPillar::reset() {
    GeographyObj::resetObject();
    _58 = 0x10;

    f32 dVar13 = mObjData->mParam1;
    if (dVar13 == 0.0f) {
        dVar13 = 12.0f;
    }

    mTornadoPos.set(mRotMtx);
    mPathWalk->init(&mPos, &mVel, &mTornadoPos);
    mPathWalk->setParameters(dVar13, 0.2f, 0.2f, 0.001f, 0.0f);
    mPathWalk->setCheckLength(600.0f);

    s16 sVar12 = mObjData->mParam3;
    if (sVar12 == 0) {
        sVar12 = 0x5a;
    }

    _17c = sVar12 * 182.04445f;

    mAnmCtrl->Reset();
    mAnmObjMat.resetFrame();
    mEmitter = nullptr;
    ResetState();
}

void TMapObjSandPillar::createEmitter() {}

void TMapObjSandPillar::moveEmitter() {}

void TMapObjSandPillar::calc() {
    JGeometry::TPos3f local_38;

    if (mEmitter == nullptr) {
        mEmitter = JPEffectMgr::getEffectMgr()->createEmt("mk_tatsumaki_a", mPos);
        #line 268
        JUT_ASSERT(mEmitter != 0);
    }
    mPathWalk->update();
    mAnmObjMat.getFrameCtrl()->update();

    PSMTXIdentity(local_38);
    
    // Not sure why scale needs to be copied here...?
    JGeometry::TVec3f scale;
    scale.set(mScale.x, mScale.y, mScale.z);

    local_38[0][0] = scale.x;
    local_38[0][1] = 0.0f;
    local_38[0][2] = 0.0f;

    local_38[1][0] = 0.0f;
    local_38[1][1] = scale.y;
    local_38[1][2] = 0.0f;

    local_38[2][0] = 0.0f;
    local_38[2][1] = 0.0f;
    local_38[2][2] = scale.z;

    PSMTXConcat(local_38, mRotMtx, local_38);
    local_38[0][3] = mPos.x;
    local_38[1][3] = mPos.y;
    local_38[2][3] = mPos.z;

    JPABaseEmitter *emitter = mEmitter;
    if (emitter != nullptr) {
        JPASetRMtxSTVecfromMtx(local_38, emitter->mGlobalRot, &emitter->mGlobalScl, &emitter->mGlobalTrs);
        emitter->setGlobalParticleScale(emitter->mGlobalScl);
    }
    ExecuteState();
    mCircleSoundMgr->setSe(0x40026);
    mCircleSoundMgr->frameWork();
}

void TMapObjSandPillar::update() {
    mAnmObjMat.anmFrameProc();
    GeographyObj::update();
}

void TMapObjSandPillar::getItemThrowDirPow(JGeometry::TVec3f *dir, f32 *power, const ItemObj &itemObj) {
    JGeometry::TVec3f itemPos;
    itemPos.set(itemObj.mPos);

    JGeometry::TVec3f throwDir;
    throwDir.sub(mPos, itemPos);
    throwDir.y = 0.0f;
    throwDir.normalize();

    throwDir.y = sItemUpVel;
    throwDir.normalize();

    dir->set(throwDir);
    *power = sItemVel;
}


void TMapObjSandPillar::initFunc_Walk() {
    mPathWalk->_94 = 1;
}

void TMapObjSandPillar::doFunc_Walk() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
