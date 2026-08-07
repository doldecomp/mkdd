#include "Shiraiwa/Objects/MapObjBubble.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/SiUtil.h"
#include "dolphin/mtx.h"
#include "mathHelper.h"

StateObserver::StateFuncSet<TMapObjBubble> TMapObjBubble::sTable[4] = {
    { 0, &TMapObjBubble::initFunc_Start, &TMapObjBubble::doFunc_Start },
    { 1, &TMapObjBubble::initFunc_Dive, &TMapObjBubble::doFunc_Dive },
    { 2, &TMapObjBubble::initFunc_Symptom, &TMapObjBubble::doFunc_Symptom },
    { 3, &TMapObjBubble::initFunc_Jump, &TMapObjBubble::doFunc_Jump },
};

f32 TMapObjBubble::sGravity = 1.0f;
J3DAnmTextureSRTKey *TMapObjBubble::sBubbleBtkAnm;

TMapObjBubble::TMapObjBubble(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    createSoundMgr();
}

TMapObjBubble::~TMapObjBubble() {}

void TMapObjBubble::createColModel(J3DModelData *modelData) {
    createBoundsSphere(modelData);
}

const char *TMapObjBubble::getBmdFileName() {
    static const char *cBmdName = "/Objects/Bubble1.bmd";
    return cBmdName;
}

void TMapObjBubble::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x200, 0);
    mAnmObjMat.setExModel(&mModel);
    mAnmObjMat.setAnmBase(sBubbleBtkAnm);
    mAnmObjMat.initFrameCtrl(mAnmObjMat.mAnmBase);
}

void TMapObjBubble::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjMaterial::setupTexSRTAnmData(&sBubbleBtkAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/Bubble1.btk"));
}

void TMapObjBubble::InitExec() { Observer_FindAndInit(TMapObjBubble, 4); }

void TMapObjBubble::MoveExec() { Observer_FindAndExec(TMapObjBubble, 4); }

void TMapObjBubble::update() {
    mAnmObjMat.anmFrameProc();
    GeographyObj::update();
}

void TMapObjBubble::reset() {
    stRandom *stRndm;
    
    GeographyObj::resetObject();
    ResetState();
    mCountStateDive = mObjData->mParam1;
    if (mCountStateDive == 0) {
        mCountStateDive = 0xb4;
    }
    mCountStateStart = mObjData->mParam2;
    mCountStateSymptom = mObjData->mParam3;
    if (mCountStateSymptom == 0) {
        mCountStateSymptom = 0x3c;
    }

    // Why is this cast to a u32 when this is a float?
    _18c = (u32)(mObjData->mParam4 * 0.01f);
    if (_18c == 0.0f) {
        _18c = 80.0f;
    }

    _190 = mObjData->mParam5 * 0.01f;
    mAnmObjMat.resetFrame();
    SiUtil::setRandomStartFrame(mAnmObjMat.getFrameCtrl(), GeographyObj::getGeoRnd());
    clrObjFlagHidding();
    setAllCheckKartHitFlag();
    _58 = 0xd;
    mRotMtx.getXDir(_178);
    _178.normalize();
    mEmitter = nullptr;
    _194 = 0.0f;
}

void TMapObjBubble::calc() {
    ExecuteState();
    mAnmObjMat.update();
}

void TMapObjBubble::initFunc_Start() {
    stopParticle();
}

void TMapObjBubble::stopParticle() {
    if (mEmitter == nullptr) {
        return;
    }
    mEmitter->setFlag(1);
}

void TMapObjBubble::doFunc_Start() {
    if (getStateCount() > mCountStateStart) {
        setState(2);
    }
}

void TMapObjBubble::initFunc_Dive() {
    if (mEmitter == nullptr) {
        return;
    }
    mEmitter->setFlag(1);
}

void TMapObjBubble::doFunc_Dive() {
    if (getStateCount() > mCountStateDive) {
        setState(2);
    }
}

void TMapObjBubble::initFunc_Symptom() {}

void TMapObjBubble::doFunc_Symptom() {
    if (getStateCount() > mCountStateSymptom) {
        setState(3);
    }
}

void TMapObjBubble::initFunc_Jump() {
    mVel.set(0.0f, _18c, 0.0f);
    if (mEmitter == nullptr) {
        mEmitter = JPEffectMgr::getEffectMgr()->createEmt("mk_bubble_a", mPos);
        #line 199
        JUT_ASSERT(mEmitter != 0);
    }
    if (mEmitter != nullptr) {
        mEmitter->clearStatus(1);
    }
}

void TMapObjBubble::doFunc_Jump() {
    JGeometry::TPos3f local_118;
    JGeometry::TVec3f local_114;
    JGeometry::TVec3f local_124;
    JGeometry::TVec3f local_130;
    JGeometry::TVec3f local_13c;
    JGeometry::TVec3f local_148;

    mVel.y -= sGravity;

    local_124.set(_178);
    local_124.normalize();
    local_124.scale(_190);

    mVel.add(local_124);
    mPos.add(mVel);

    if (mPos.y < mObjData->position.y) {
        mPos.set(mObjData->position);
        setState(1);
    }

    local_13c.set(mVel);
    local_13c.normalize();

    local_148.set(mObjData->zNorm);
    local_148.normalize();

    local_130.cross(local_13c, local_148);
    local_130.normalize();

    local_148.cross(local_130, local_13c);
    local_148.normalize();

    mRotMtx.setXYZDir(local_130, local_13c, local_148);

    CrsGround CStack_e8(RCMGetCourse());
    CStack_e8.search(mPos);

    f32 dVar7 = mPos.y - CStack_e8.getSplashHeight();
    if (dVar7 * _194 < 0.0f) {
        JPEffectMgr::getEffectMgr()->createEmt("mk_bubbleLand_a", mPos)->setGlobalScale(mScale);
        JPEffectMgr::getEffectMgr()->createEmt("mk_bubbleLand_b", mPos)->setGlobalScale(mScale);

        if (mVel.y > 0.0f) {
            getSoundMgr()->setSe(0x4003b);
        } else {
            getSoundMgr()->setSe(0x4003c);
        }
    }
    _194 = dVar7;

    PSMTXIdentity(local_118);

    // Not sure why scale needs to be copied here...?
    JGeometry::TVec3f scale;
    scale.set(mScale.x, mScale.y, mScale.z);

    local_118[0][0] = scale.x;
    local_118[0][1] = 0.0f;
    local_118[0][2] = 0.0f;

    local_118[1][0] = 0.0f;
    local_118[1][1] = scale.y;
    local_118[1][2] = 0.0f;

    local_118[2][0] = 0.0f;
    local_118[2][1] = 0.0f;
    local_118[2][2] = scale.z;
    
    PSMTXConcat(local_118, mRotMtx, local_118);
    local_118[0][3] = mPos.x;
    local_118[1][3] = mPos.y;
    local_118[2][3] = mPos.z;
    JPABaseEmitter *emt = mEmitter;
    if (emt != nullptr) {
        JPASetRMtxSTVecfromMtx(local_118, emt->mGlobalRot, &emt->mGlobalScl, &emt->mGlobalTrs);
        emt->setGlobalParticleScale(emt->mGlobalScl);
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
