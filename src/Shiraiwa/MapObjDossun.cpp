#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/AnmController.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3D.h"
#include "Shiraiwa/Objects/MapObjDossun.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Yamamoto/kartCtrl.h"
#include "types.h"
#include "mathHelper.h"

StateObserver::StateFuncSet<TMapObjDossun> TMapObjDossun::sTable[6] = {
    { 0, &TMapObjDossun::initFunc_Bottom, &TMapObjDossun::doFunc_Bottom },
    { 1, &TMapObjDossun::initFunc_Up, &TMapObjDossun::doFunc_Up },
    { 2, &TMapObjDossun::initFunc_Die, &TMapObjDossun::doFunc_Die },
    { 3, &TMapObjDossun::initFunc_Start, &TMapObjDossun::doFunc_Start },
    { 4, &TMapObjDossun::initFunc_Top, &TMapObjDossun::doFunc_Top },
    { 5, &TMapObjDossun::initFunc_Down, &TMapObjDossun::doFunc_Down },
};

const JGeometry::TVec3f TMapObjDossun::scDownVelocity0(0.0f, 100.0f, 0.0f);
JGeometry::TVec3f TMapObjDossun::sDieVelocity0(0.0f, 200.0f, 0.0f);

J3DAnmTransform *TMapObjDossun::sDossunBckAnmTrans;
J3DMtxCalc *TMapObjDossun::sDossunBckMtxCalc;
J3DAnmTexPattern *TMapObjDossun::sDossunBtpAnm;

f32 TMapObjDossun::sBtpRevFrameRate = -0.6f;
f32 TMapObjDossun::sBtpPlyFrameRate = 0.6f;
f32 TMapObjDossun::sDieGravityY = 5.0f;


TMapObjDossun::TMapObjDossun(const CrsData::SObject &sObject) : TMapObjStamper(sObject), StateObserver(), mHeight(0x3e8), _1a2(0xfa), _1a4(0x1e) {
    NewAnmCtrl();
    createSoundMgr();
    mPathMove = new TPathMove(&sObject);
}

TMapObjDossun::~TMapObjDossun() {
    delete mPathMove;
}

void TMapObjDossun::reset() {
    GeographyObj::resetObject();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = 10;

    if (mObjData->mParam1 != 0) {
        _1a2 = mObjData->mParam1;
    }

    if (mObjData->mParam2 != 0) {
        _1a4 = mObjData->mParam2;
    }

    if (IsPathMove()) {
        mPathMove->getNodePosition(&mPos, 0);
        TPathMove *pathMove = mPathMove;
        pathMove->_14 = 10.0f;
        pathMove->_18 = 10.0f;
        pathMove->_20 = 25.0f;
    }

    _1d4 = mObjData->mParam3;
    if (_1d4 < 0) {
        _1d4 = 0;
    }

    setState(1);

    CrsGround crsGround(RCMGetCourse());
    crsGround.search(mPos);
    mHeight = mPos.y - crsGround.getHeight();

    #line 108
    JUT_ASSERT(mHeight >= 0);

    mFreeMove.init(&mPos, &mVel, 200.0f);
    mPathMove->init(&mPos, &mVel);
    f32 local_28 = mHeight;
    if (local_28 == 0.0f) {
        local_28 = _1a4;
        TPathMove *pathMove = mPathMove;
        pathMove->_14 = 1.4f;
        pathMove->_18 = local_28;
        pathMove->_20 = (local_28 * 0.5f) * (local_28 * 0.5f);
    }

    ResetState();
    mAnmCtrl->Reset();
    mAnmCtrl->ChangeTransAnm(0);    // FIX: Is this function causing register mismatches?
    if (mAnmCtrl->mTrans != nullptr) {
        mAnmCtrl->mTrans->setFlag();
    }

    _180.resetFrame();
    _180.getFrameCtrl()->setAttribute(0);
    resetFrame();
}

void TMapObjDossun::loadAnimation() {
    J3DModelData *modelData = *mModel.mModelData;
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/Dossun1.bck");
    J3DAnmObjTrans::setupTransAnmData(&sDossunBckAnmTrans, &sDossunBckMtxCalc, modelData, ptrCourseArc);
    ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/Dossun1.btp");
    J3DAnmObjMaterial::setupTexPatternAnmData(&sDossunBtpAnm, modelData, ptrCourseArc);
}

const char *TMapObjDossun::getShadowBmdFileName() {
    static const char *cShadowBmdName = "/Objects/Dossun1_kage.bmd";
    return cShadowBmdName;
}

void TMapObjDossun::createColModel(J3DModelData* modelData) {
    f32 scaleXZ = mScale.x > mScale.z 
        ? mScale.x 
        : mScale.z;

    f32 ratio = mScale.y / scaleXZ;
    static const f32 cBoundHeight = ratio * 800.0f;
    
    createBoundsCylinder(modelData, 500.0f, cBoundHeight);
}

void TMapObjDossun::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x40000, 0);

    AnmController *anmCtrl = mAnmCtrl;
    AnmControlTrans *anmTrans = new AnmControlTrans();
    anmCtrl->mTrans = anmTrans;
    anmTrans = anmCtrl->mTrans;

    anmTrans->initAnm(1, &mModel);
    mAnmCtrl->mTrans->registration(0, sDossunBckAnmTrans, sDossunBckMtxCalc);
    mAnmCtrl->mTrans->getFrameCtrl(0)->setAttribute(2);

    _180.setExModel(&mModel);
    _180.mAnmBase = sDossunBtpAnm;
    _180.initFrameCtrl(_180.getAnmBase());

}

const char *TMapObjDossun::getBmdFileName() {
    static const char *cBmdName = "/Objects/Dossun1.bmd";
    return cBmdName;
}

void TMapObjDossun::update() {
    _180.anmFrameProc();
    setModelMatrixAndScale();
    mModel.update(0);
}

void TMapObjDossun::doKartColCallBack(int kartIdx) {
    if ((GetKartCtrl()->getKartBody(kartIdx)->mCarStatus & 0x40000)) {
        setState(5);
    } else {
        startStamp(kartIdx);
    }
}

void TMapObjDossun::initFunc_Start() {}

void TMapObjDossun::doFunc_Start() {
    if (getStateCount() > _1d4) {
        setState(1);
    }
}

void TMapObjDossun::initFunc_Up() {
    JGeometry::TVec3f targetOffset;
    
    _180.setFrame(_180.getFrameCtrl()->getEnd());
    _180.setRate(sBtpRevFrameRate);
    targetOffset.set(0.0f, (f32)mHeight, 0.0f);
    mFreeMove.setTargetOffsetUniform(targetOffset, 120);
}

void TMapObjDossun::doFunc_Up() {
    if (!mFreeMove.hasTarget()) {
        setState(1);
    }
}

void TMapObjDossun::initFunc_Top() {
    _1a6 = 0;

    if (mAnmCtrl->mTrans != nullptr) {
        mAnmCtrl->mTrans->setFlag();
    }
    
    if (IsPathMove()) {
        mPathMove->setTargetNode();
        mVel.zero();
    }
}

void TMapObjDossun::doFunc_Top() {
    s16 frameEnd;
    AnmControlTrans *pAVar2;
    u32 uVar3;
    J3DAnmObjTrans *pJVar4;

    if (mPathMove->_1c != false) {
        moveShadowModel();
        return;
    }

    if (mHeight == 0) {
        if (_1a6 < _1a2) {
            if (_1a6 == 0) {
                shakeCamera();
            }
            _1a6++;
            return;
        }

        _1a6 = 0;

        if (mAnmCtrl->mTrans != nullptr) {
            mAnmCtrl->mTrans->setFlag();
        }

        if (IsPathMove()) {
            mPathMove->setTargetNode();
            mVel.zero();
        }
        return;
    }

    frameEnd = mAnmCtrl->mTrans->getObj(0)->getFrameCtrl()->getEnd();
    if (_1a6 < (u32)(_1a2 + frameEnd * 3)) {
        if (_1a6 == _1a2) {
            if (mAnmCtrl->mTrans != nullptr) {
                mAnmCtrl->mTrans->resetFlag();
            }
        }
        _1a6++;

    } else {
        if (mAnmCtrl->mTrans != nullptr) {
            mAnmCtrl->mTrans->setFlag();
        }
        setState(2);
    }
    
    if ((u32)(_1a2 + frameEnd * 3 - 6) == _1a6) {
        _180.setRate(sBtpPlyFrameRate);
    }
}

void TMapObjDossun::initFunc_Down() {
    JGeometry::TVec3f local_18;
    mVel.set(scDownVelocity0);
    local_18.set(0.0f, (f32)-mHeight, 0.0f);
    mFreeMove.setUnknown24(30.0f);
    mFreeMove.setTargetOffset(local_18, 10.0f, 200.0f);
}

void TMapObjDossun::doFunc_Down() {
    if (!mFreeMove.hasTarget()) {
        shakeCamera();
        makeGroundEmitter();
        getSoundMgr()->setSe(0x40033);
        setState(3);
    }
}

void TMapObjDossun::makeGroundEmitter() {
    JPEffectMgr::getEffectMgr()->createEmt("mk_dossun_a", mPos);
    JPEffectMgr::getEffectMgr()->createEmt("mk_dossun_b", mPos);
    JPEffectMgr::getEffectMgr()->createEmt("mk_dossun_c", mPos);
}

void TMapObjDossun::initFunc_Bottom() {
    if (mAnmCtrl->mTrans != nullptr) {
        mAnmCtrl->mTrans->resetFlag();
    }
    _1a6 = 0;
}

void TMapObjDossun::doFunc_Bottom() {
    s16 sVar1;
    u32 uVar2;
    J3DAnmObjTrans *pJVar3;
    AnmControlTrans *pAVar4;

    if (_1a6 < _1a4) {
        if (_1a6 == (u32)mAnmCtrl->mTrans->getObj(0)->getFrameCtrl()->getEnd()) {
            if (mAnmCtrl->mTrans != nullptr) {
                mAnmCtrl->mTrans->setFlag();
            }
        }
        _1a6++;

    } else {
        setState(4);
    }
}

void TMapObjDossun::initFunc_Die() {
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mFreeMove.releaseTarget();
    mVel.set(sDieVelocity0);
    _180.setFrame(_180.getFrameCtrl()->getEnd());
}

void TMapObjDossun::doFunc_Die() {
    mVel.y -= sDieGravityY;
    mPos.add(mVel);
    if (mPos.y > mObjData->position.y) {
        mPos.y = mObjData->position.y;
        mVel.zero();
        setObjFlagCheckItemHitting();
        setAllCheckKartHitFlag();
        setState(1);
    }
}

void TMapObjDossun::shakeCamera() {
    ObjUtility::executeShakeCamera(mPos, 10000.0f, 0.4f, 2000.0);
}

void TMapObjDossun::calc() {
    ExecuteState();
    mFreeMove.update();

    if (IsPathMove() && getState() == 1) {
        mPathMove->update();
    }

    checkItemHitting();
    _180.update();

    for (int kartIdx = 0; kartIdx < RCMGetManager()->getKartNumber(); kartIdx++) {
        if ((GetKartCtrl()->GetKartStatus(kartIdx) & 0x200000000) == 0) {
            endStamp(kartIdx);
        }
    }
    frameProc();
}

void TMapObjDossun::checkItemHitting() {
    if (tstItemHitting()) {
        if (mColItemObj->mItemKind == 7 || mColItemObj->mItemKind == 8) {
            mFreeMove.releaseTarget();
            setState(5);
        }
    }
}

void TMapObjDossun::InitExec() { Observer_FindAndInit(TMapObjDossun, 6); }

void TMapObjDossun::MoveExec() { Observer_FindAndExec(TMapObjDossun, 6); }

#include "JSystem/JAudio/JASFakeMatch2.h"
