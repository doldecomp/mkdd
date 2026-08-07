#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3D/J3DFrameCtrl.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Quat.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/ResMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/ObjCollision.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/AnmPlayer.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHeyho.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"
#include "dolphin/mtx.h"
#include "types.h"

TAnmInfo TMapObjHeyho::sAnmInfos[1] = {
    { "/Objects/Heyho1.bca", nullptr, nullptr, 2, 0, 0, 255, 0 }
};

StateObserver::StateFuncSet<TMapObjHeyho> TMapObjHeyho::sTable[7] = {
    0, &TMapObjHeyho::initFunc_Skate,   &TMapObjHeyho::doFunc_Skate,
    1, &TMapObjHeyho::initFunc_Jump,    &TMapObjHeyho::doFunc_Jump,
    2, &TMapObjHeyho::initFunc_Bomb,    &TMapObjHeyho::doFunc_Bomb,
    3, &TMapObjHeyho::initFunc_BombEnd, &TMapObjHeyho::doFunc_BombEnd,
    4, &TMapObjHeyho::initFunc_Sink,    &TMapObjHeyho::doFunc_Sink,
    5, &TMapObjHeyho::initFunc_Recover, &TMapObjHeyho::doFunc_Recover,
    6, &TMapObjHeyho::initFunc_Die,     &TMapObjHeyho::doFunc_Die,
};

StateObserver::StateFuncSet<TMapObjSubHeyho> TMapObjSubHeyho::sTable[3] = {
    { 0, &TMapObjSubHeyho::initFunc_Skate, &TMapObjSubHeyho::doFunc_Skate },
    { 1, &TMapObjSubHeyho::initFunc_Spin, &TMapObjSubHeyho::doFunc_Spin },
    { 2, &TMapObjSubHeyho::initFunc_Recover, &TMapObjSubHeyho::doFunc_Recover }
};

TAnmInfo TMapObjSubHeyho::sAnmInfos[1] = {
    { "/Objects/Heyho1.bca", nullptr, nullptr, 2, 0, 0, 255, 0 }
};


f32 TMapObjHeyho::sGravity = 4.0f;
f32 TMapObjHeyho::sAccel = 0.05f;
f32 TMapObjHeyho::sJumpVel = 30.0f;
f32 TMapObjHeyho::sJumpGrv = 1.0f;
f32 TMapObjHeyho::sRotSpeed = 0.7f;
f32 TMapObjHeyho::sFirstSinkVel = -0.05f;
f32 TMapObjHeyho::sSinkAccel = 0.01f;
f32 TMapObjHeyho::sBombVelH = 40.0f;
f32 TMapObjHeyho::sBombVelV = 70.0f;
f32 TMapObjHeyho::sSlideVelH = 100.0f;
f32 TMapObjHeyho::sSlideVelV = 30.0f;
f32 TMapObjHeyho::sBombRotSpeed = 0.3f;
f32 TMapObjHeyho::sBombEndRotDecel = 0.005f;
f32 TMapObjHeyho::sAirFriction = 0.999f;

THeyhoSupervisor *TMapObjHeyho::sSupervisor;

J3DAnmTexPattern *TMapObjHeyho::sHeyhoBtpAnm;

J3DAnmTexPattern *TMapObjSubHeyho::sHeyhoBtpAnm;

const s8 TMapObjHeyho::sAnmTable[7] = {
    0, 0, 0, 0, 0, 0, 0 
};


void THeyhoSupervisor::calc() {
    for (JSULink<TMapObjHeyho> *linkA = _24.getFirst(); linkA != nullptr; linkA = linkA->getNext()) {
        for (JSULink<TMapObjHeyho> *linkB = linkA; linkB != nullptr; linkB = linkB->getNext()) {
            if (linkA != linkB) {
                linkA->getObject()->hitCheck(linkB->getObject());
            }
        }
    }
}

TMapObjHeyho::TMapObjHeyho(const CrsData::SObject &sObject) : TMapObjHioNode(sObject), StateObserver(), mLinkHeyho(this) {
    NewAnmCtrl();
    createSoundMgr();

    if (sObject.mParam2 == 1) {
        _1bc = (TMapObjSubHeyho *)GetGeoObjMgr()->createSubObj(0x13ef);
        _1bc->mSubHeyhoPos = &mPos;
        _1bc->mSubHeyhoVel = &mVel;
        _1bc->mSubHeyhoRot = &mRotMtx;
        _1bc->mScale.set(mScale);
    } else {
        _1bc = nullptr;
    }
    mAnmPlayer.resetAnimations(sAnmInfos, 1);

    mPathWalk = new TPathWalk(&sObject);

    if (getSupervisor() == nullptr) {
        sSupervisor = new THeyhoSupervisor;
    }

    sSupervisor->_24.append(&mLinkHeyho);
}

TMapObjHeyho::~TMapObjHeyho() {
    sSupervisor = nullptr;
    delete mPathWalk;
}

void TMapObjHeyho::reset() {
    f32 fVar1;

    resetObject();
    ResetState();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = 4;

    mAnmPlayer.init(mAnmCtrl, sAnmInfos, 1);
    
    mAnmObj.resetFrame();
    mSinkVel = 0.0f;
    _238 = 0.0f;
    mJumpVel = 0.0f;
    _24c = 0.0f;
    _250 = 0.0f;

    PSMTXIdentity(_208);
    mHeyhoPos.set(mPos);
    mHeyhoRot.set(mRotMtx);

    mHeyhoVel.zero();
    _248 = 0.0f;
    mPathWalk->init(&mHeyhoPos, &mVel, &mHeyhoRot);
    mPathWalk->reset();

    fVar1 = mObjData->mParam1;
    if (fVar1 == 0.0f) {
        fVar1 = 12.0f;
    }
    _244 = fVar1;

    const s16 frameLimit = 3;
    u16 newFrame = mObjData->mParam3;
    if (newFrame >= frameLimit) {
        newFrame = 0;
    }

    mAnmObj.setFrame(newFrame);
    
    if (_1bc != nullptr) {
        _1bc->mAnmObj.setFrame(newFrame);
    }
    mPathWalk->setParameters(fVar1, 0.2f, 0.06f, 0.001f, 0.01f);
}

void TMapObjHeyho::calc() {
    s16 updateResult = mPathWalk->update();
    if (updateResult != -1) {
        checkJump(updateResult);
    }

    if (checkItemHitting()) {
        changeAllState(2);
    }
    ExecuteState();
    moveShadowModel();
    callWalkSound();
}

void TMapObjHeyho::callWalkSound() {
    J3DFrameCtrl *anim = mAnmPlayer.mController->mTrans->getFrameCtrl(mAnmPlayer.getCurAnmNumber());

    if (SiUtil::checkPassFrame(65.0f, anim->getFrame(), anim->getRate())) {
        getSoundMgr()->setSe(0x4004B);
    }

    if (SiUtil::checkPassFrame( 0.0f, anim->getFrame(), anim->getRate())) {
        getSoundMgr()->setSe(0x4004A);
    }
}

bool TMapObjHeyho::checkItemHitting() {
    bool hit = false;

    if (tstItemHitting()) {
        ItemObj* item = mColItemObj;

        switch (item->mItemKind) {
        case 1:
        case 7:
        case 8:
            _24c = sBombVelH;
            _250 = sBombVelV;
            mHeyhoVel.set(item->mPos);
            hit = true;
            break;

        case 0:
        case 2:
        case 9:
            _24c = sSlideVelH;
            _250 = sSlideVelV;
            mHeyhoVel.set(item->mPos);
            hit = true;
            break;
        }
    }

    if (!hit && _1bc != nullptr) {
        if (_1bc->checkItemHitting()) {
            hit = true;
        }
    }

    return hit;
}

void TMapObjHeyho::checkJump(u16 param_1) {
    if (RCMGetCourse()->getCrsData()->getPointData(mObjData->mPathID, param_1)->linkPoint) {
        setState(1);
    }
}

const char *TMapObjHeyho::getBmdFileName() {
    static const char *cBmdName = "/Objects/Heyho1.bmd";
    return cBmdName;
}

const char *TMapObjHeyho::getShadowBmdFileName() {
    static const char *cShadowBmdName = "/Objects/Heyho1Shadow.bmd";
    return cShadowBmdName;
}

void TMapObjHeyho::loadAnimation() {
    J3DModelData *modelData = *mModel.mModelData;
    TAnmPlayer::loadAnimations(sAnmInfos, 1, modelData, ResMgr::mcArcCourse);
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/Heyho1.btp");
    mAnmObj.setupTexPatternAnmData(&sHeyhoBtpAnm, modelData, ptrCourseArc);
}

void TMapObjHeyho::createModel(JKRSolidHeap *jkrSolidHeap, u32 p2) {
    mModel.createDifferedModel(jkrSolidHeap, p2, 0x20000, 0);
    TAnmPlayer::registAnimations(mAnmCtrl, &mModel, sAnmInfos, 1);
    mAnmObj.setExModel(&mModel);
    mAnmObj.setAnmBase(sHeyhoBtpAnm);
    mAnmObj.initFrameCtrl(mAnmObj.getAnmBase());
}

void TMapObjHeyho::createColModel(J3DModelData *modelData) {
    f32 scaleXZ = mScale.x > mScale.z 
        ? mScale.x 
        : mScale.z;

    f32 max = scaleXZ > mScale.y 
        ? scaleXZ 
        : mScale.y;

    createBoundsCylinder(modelData, 140.0f * (scaleXZ / max),
        320.0f * (mScale.y / max)
    );
}

void TMapObjHeyho::InitExec() { Observer_FindAndInit(TMapObjHeyho, 7); }

void TMapObjHeyho::MoveExec() { Observer_FindAndExec(TMapObjHeyho, 7);  }

void TMapObjHeyho::update() {
    mAnmObj.anmFrameProc();
    setModelMatrixAndScale();
    mModel.update(0);
}

void TMapObjHeyho::changeAllState(u16 state) {
    #line 386
    JUT_MINMAX_ASSERT(0, state, 7);
    setState((u16)state);
    if (mAnmPlayer.getCurAnmNumber() != sAnmTable[state]) {
        mAnmPlayer._11 = sAnmTable[state];
        mAnmPlayer._10 = true;
        mAnmPlayer._e |= 1;
    }
}

void TMapObjHeyho::doKartColCallBack(int kartNo) {
    KartBody *kart = GetKartCtrl()->getKartBody(kartNo);

    if (kart->mCarStatus & 0x40000) {
        ObjUtility::getKartPos(kartNo, &mHeyhoVel);
        setState(2);

        if (mAnmPlayer.getCurAnmNumber() != sAnmTable[2]) {
            mAnmPlayer._11 = sAnmTable[2];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}

void TMapObjHeyho::initFunc_Skate() {
    mPathWalk->_94 = 1;
    if (!mAnmCtrl->IsAvailableTrans()) {
        return;
    }
    mAnmCtrl->StopTrans();
}

void TMapObjHeyho::doFunc_Skate() {
    mPos.set(mHeyhoPos);
    mRotMtx.set(mHeyhoRot);
}

void TMapObjHeyho::initFunc_Jump() {
    _238 = 0.0f;
    mJumpVel = sJumpVel;

    mRotMtx.set(mHeyhoRot);
    PSMTXIdentity(_208);

    const f32 angle = sRotSpeed;
    const f32 s = sinf(angle);
    const f32 c = cosf(angle);
    _208[1][1] = 1.0f;
    _208[0][0] = c;
    _208[0][2] = s;
    _208[2][0] = -s;
    _208[2][2] = c;
    _208[2][1] = 0.0f;
    _208[1][2] = 0.0f;
    _208[1][0] = 0.0f;
    _208[0][1] = 0.0f;

    if (mAnmCtrl->IsAvailableTrans()) {
        mAnmCtrl->mTrans->setFlag();
    }
    if (_1bc != nullptr) {
        _1bc->setState(1);
    }

    getSoundMgr()->setSe(0x4004c);
}

void TMapObjHeyho::doFunc_Jump() {
    mJumpVel -= sJumpGrv;
    _238 += mJumpVel;
    if (_238 < 0.0f) {
        _238 = 0.0f;
        mHeyhoRot.set(mRotMtx);
        if (mAnmCtrl->mTrans != nullptr) {
            mAnmCtrl->mTrans->resetFlag();
        }
        mPathWalk->_90 = _244 * 0.5f;
        if (_1bc != nullptr) {
            _1bc->setState(2);
        }
        setState(4);
        if (mAnmPlayer.getCurAnmNumber() != sAnmTable[4]) {
            mAnmPlayer._11 = sAnmTable[4];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }

    mPos.set(mHeyhoPos.x, mHeyhoPos.y + _238, mHeyhoPos.z);
    PSMTXConcat(mRotMtx, _208, mRotMtx);
}

void TMapObjHeyho::initFunc_Bomb() {    
    mPathWalk->_94 = 0;

    JGeometry::TVec3f local_38;
    local_38.sub(mPos, mHeyhoVel);
    local_38.normalize();

    local_38.y = 0.0f;
    local_38.setLength(_24c);

    local_38.y = _250;
    mVel.set(local_38);

    mRotMtx.set(mHeyhoRot);
    PSMTXIdentity(_208);

    const f32 angle = sBombRotSpeed;
    const f32 s = sinf(angle);
    const f32 c = cosf(angle);
    _208[1][1] = 1.0f;
    _208[0][0] = c;
    _208[0][2] = s;
    _208[2][0] = -s;
    _208[2][2] = c;
    _208[2][1] = 0.0f;
    _208[1][2] = 0.0f;
    _208[1][0] = 0.0f;
    _208[0][1] = 0.0f;

    if (mAnmCtrl->IsAvailableTrans()) {
        mAnmCtrl->mTrans->setFlag();
    }
    if (_1bc != nullptr) {
        _1bc->setState(1);
    }
}

void TMapObjHeyho::doFunc_Bomb() {
    PSMTXConcat(mRotMtx, _208, mRotMtx);

    mVel.scale(sAirFriction);
    mVel.y -= sGravity;

    JGeometry::TVec3f oldPos(mPos);
    mPos.add(mVel);

    CrsGround ground(RCMGetCourse());
    ground.search(mPos, oldPos);

    JGeometry::TVec3f wallNormal;
    f32 normalisedWall = ground.getWallNormal(&wallNormal, nullptr);
    if (normalisedWall > -1.0f) {
        mPos.scaleAdd(normalisedWall, wallNormal, mPos);
    }

    f32 groundHeight = ground.getHeight();

    if (mPos.y < groundHeight) {
        ground.search(oldPos);

        switch (ground.getAttribute()) {
            case 0x05:
            case 0x0A:
            case 0x0E:
            case 0x0F:
            case 0xFF: {
                setState(6);
                if (mAnmPlayer.getCurAnmNumber() != sAnmTable[6]) {
                    mAnmPlayer._11 = sAnmTable[6];
                    mAnmPlayer._10 = true;
                    mAnmPlayer._e |= 1;
                }
                break;
            }

            default: {
                mPos.y = groundHeight;
                mHeyhoPos.set(mPos);
                mHeyhoRot.set(mRotMtx);

                mVel.zero();
                mPathWalk->_90 = 0.0f;

                setState(3);

                if (mAnmPlayer.getCurAnmNumber() != sAnmTable[3]) {
                    mAnmPlayer._11 = sAnmTable[3];
                    mAnmPlayer._10 = true;
                    mAnmPlayer._e |= 1;
                }

                break;
            }
        }
    }
}

void TMapObjHeyho::initFunc_BombEnd() {
    _248 = sBombRotSpeed;
}

void TMapObjHeyho::doFunc_BombEnd() {
    JGeometry::TVec3f local_4c;
    JGeometry::TVec3f local_58;
    JGeometry::TPos3f auStack_40;
    
    _248 -= sBombEndRotDecel;
    if (_248 <= 0.0f) {
        _248 = 0.0f;
        mRotMtx.getZDir(local_4c);
        local_4c.y = 0.0f;
        local_4c.normalize();
        mRotMtx.getYDir(local_58);
        local_58.x = 0.0f;
        local_58.normalize();
        mHeyhoRot.set(mRotMtx);
        mPathWalk->_94 = 1;
        mPathWalk->forceTurn(local_58, local_4c);
        if (_1bc != nullptr) {
            _1bc->setState(2);
        }
        setState(5);
        if (mAnmPlayer.getCurAnmNumber() != sAnmTable[5]) {
            mAnmPlayer._11 = sAnmTable[5];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    } else {
        PSMTXIdentity(auStack_40);
        PSMTXRotRad(auStack_40, 'y', _248);
        PSMTXConcat(mRotMtx, auStack_40, mRotMtx);
    }
}

void TMapObjHeyho::initFunc_Sink() {
    mSinkVel = sFirstSinkVel;
    getSoundMgr()->setSe(0x4005a);
}

void TMapObjHeyho::doFunc_Sink() {
    mSinkVel += sSinkAccel;
    mRotMtx[3][1] += mSinkVel;

    if (mRotMtx[3][1] > 1.0f) {
        mRotMtx[3][1] = 1.0f;
    }

    f32 walkPos = mPathWalk->_90;
    walkPos += sAccel;

    if (walkPos > _244) {
        walkPos = _244;

        if (mRotMtx[3][1] >= 1.0f) {
            setState(0);

            if (mAnmPlayer.getCurAnmNumber() != sAnmTable[0]) {
                mAnmPlayer._11 = sAnmTable[0];
                mAnmPlayer._10 = true;
                mAnmPlayer._e |= 1;
            }
        }
    }

    mPathWalk->_90 = walkPos;

    mPos.set(mHeyhoPos);
    mRotMtx.set(mHeyhoRot);
}

void TMapObjHeyho::initFunc_Recover() {
    mPathWalk->_94 = 1;
    if (!mAnmCtrl->IsAvailableTrans()) {
        return;
    }
    mAnmCtrl->StopTrans();
}

void TMapObjHeyho::doFunc_Recover() {
    f32 walkPos = mPathWalk->_90;
    walkPos += sAccel;

    if (walkPos > _244) {
        walkPos = _244;

        if (mRotMtx[3][1] >= 1.0f) {
            setState(0);

            if (mAnmPlayer.getCurAnmNumber() != sAnmTable[0]) {
                mAnmPlayer._11 = sAnmTable[0];
                mAnmPlayer._10 = true;
                mAnmPlayer._e |= 1;
            }
        }
    }
    mPathWalk->_90 = walkPos;
    mPos.set(mHeyhoPos);
    mRotMtx.set(mHeyhoRot);
}

void TMapObjHeyho::initFunc_Die() {
    mPathWalk->_94 = 0;
    if (mAnmCtrl->IsAvailableTrans()) {
        mAnmCtrl->mTrans->setFlag();
    }
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    setObjFlagHidding();
}

void TMapObjHeyho::doFunc_Die() {}

// FIX: Register mismatch.
void TMapObjHeyho::hitCheck(TMapObjHeyho* heyho) {
    JGeometry::TVec3f heyhoPos(heyho->mPos);
    JGeometry::TVec3f delta;

    if (getBounds(0)->IsHitCylinder(mPos, heyhoPos, *(ObjColCylinder *)heyho->getBounds(0))) {
        delta.sub(mPos, heyhoPos);
        delta.normalize();
        delta.scale(getBounds(0)->getBoundDepth());

        mPos.add(delta);
        mHeyhoPos.set(mPos);
    }
}

TMapObjSubHeyho::TMapObjSubHeyho() : GeographyObj(0x13ef) {
    NewAnmCtrl();
    TAnmPlayer::resetAnimations(sAnmInfos, 1);
}

void TMapObjSubHeyho::reset() {
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = 4;
    mAnmCtrl->Reset();
    mAnmObj.resetFrame();
    _188.set(250.0f, 0.0f, 0.0f);
    ResetState();
}

bool TMapObjSubHeyho::checkItemHitting() {
    bool isItemHitting = false;
    if (tstItemHitting()) {
        switch (mColItemObj->mItemKind) {
            case 0:
            case 1:
            case 2:
            case 7:
            case 8:
            case 9:
                isItemHitting = true;
                break;
        }
    }
    return isItemHitting;
}

void TMapObjSubHeyho::InitExec() { Observer_FindAndInit(TMapObjSubHeyho, 3); }

void TMapObjSubHeyho::MoveExec() { Observer_FindAndExec(TMapObjSubHeyho, 3); }

void TMapObjSubHeyho::calc() {
    mRotMtx.set(*mSubHeyhoRot);
    ExecuteState();
    moveShadowModel();
}

void TMapObjSubHeyho::initFunc_Skate() {
    setAllCheckKartHitFlag();
    setObjFlagCheckItemHitting();
}

void TMapObjSubHeyho::doFunc_Skate() {
    JGeometry::TVec3f newPos;
    JGeometry::TPos3f mtxFront;
    
    makeFrontMtx(&mtxFront);
    PSMTXMultVecSR(mtxFront, &_188, &newPos);
    mPos.add(*mSubHeyhoPos, newPos);
}


void TMapObjSubHeyho::makeFrontMtx(JGeometry::TPos3f *mtx) {
    JGeometry::TVec3f dirX;
    JGeometry::TVec3f dirY;
    JGeometry::TVec3f dirZ;
    
    if (mSubHeyhoVel->isZero()) {
        mRotMtx.getZDir(dirZ);
        if (dirZ.isZero()) {
            dirZ.set(0.0f, 0.0f, 1.0f);
        }
    } else {
        dirZ.set(*mSubHeyhoVel);
    }
    dirZ.normalize();
    dirY.set(0.0f, 1.0f, 0.0f);
    dirX.cross(dirY, dirZ);
    dirX.normalize();
    mtx->setXYZDir(dirX, dirY, dirZ);
}

void TMapObjSubHeyho::initFunc_Spin() {
    _194.set(*mSubHeyhoRot);
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
}

void TMapObjSubHeyho::doFunc_Spin() {
    JGeometry::TVec3f newPos;
    
    PSMTXMultVecSR(_194, &_188, &newPos);
    mPos.add(*mSubHeyhoPos, newPos);
}

void TMapObjSubHeyho::initFunc_Recover() {
    _1c4 = 0.0f;
    _194.zeroTrans();
}

// FIX: Register mismatch.
void TMapObjSubHeyho::doFunc_Recover() {
    JGeometry::TQuat4f local_4c(0.0f, 0.0f, 0.0f, 1.0f);
    _194.getQuat(local_4c);
    local_4c.normalize(local_4c);
    JGeometry::TPos3f local_3c;
    makeFrontMtx(&local_3c);
    JGeometry::TQuat4f local_5c(0.0f, 0.0f, 0.0f, 1.0f);
    local_3c.getQuat(local_5c);
    local_5c.normalize(local_5c);

    // FIX: Should this be an inlined slerp/nlerp?
    local_4c.x = _1c4 * local_4c.x + (local_5c.x - local_4c.x);
    local_4c.y = _1c4 * local_4c.y + (local_5c.y - local_4c.y);
    local_4c.z = _1c4 * local_4c.z + (local_5c.z - local_4c.z);
    local_4c.w = _1c4 * local_4c.w + (local_5c.w - local_4c.w);
    
    local_4c.normalize(local_4c);
    local_3c.setQuat(local_4c);
    JGeometry::TVec3f local_68;
    PSMTXMultVecSR(local_3c, &_188, &local_68);
    mPos.add(*mSubHeyhoPos, local_68);
    _1c4 += 0.01f;
    if (_1c4 >= 1.0f) {
        setState(0);
    }
}

const char *TMapObjSubHeyho::getBmdFileName() {
    static const char *cBmdName = "/Objects/Heyho1.bmd";
    return cBmdName;
}

const char *TMapObjSubHeyho::getShadowBmdFileName() {
    static const char *cShadowBmdName = "/Objects/Heyho1Shadow.bmd";
    return cShadowBmdName;
}

void TMapObjSubHeyho::update() {
    mAnmObj.anmFrameProc();
    setModelMatrixAndScale();
    mModel.update(0);
}

void TMapObjSubHeyho::loadAnimation() {
    J3DModelData *modelData = *mModel.mModelData;
    TAnmPlayer::loadAnimations(sAnmInfos, 1, modelData, ResMgr::mcArcCourse);
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/Heyho1.btp");
    mAnmObj.setupTexPatternAnmData(&sHeyhoBtpAnm, modelData, ptrCourseArc);
}

void TMapObjSubHeyho::createModel(JKRSolidHeap *jkrSolidHeap, u32 p2) {
    mModel.createDifferedModel(jkrSolidHeap, p2, 0x20000, 0);
    TAnmPlayer::registAnimations(mAnmCtrl, &mModel, sAnmInfos, 1);
    mAnmObj.setExModel(&mModel);
    mAnmObj.setAnmBase(sHeyhoBtpAnm);
    mAnmObj.initFrameCtrl(mAnmObj.getAnmBase());
}

void TMapObjSubHeyho::createColModel(J3DModelData *modelData) {
    f32 scaleXZ = mScale.x > mScale.z 
        ? mScale.x 
        : mScale.z;

    f32 max = scaleXZ > mScale.y 
        ? scaleXZ 
        : mScale.y;

    createBoundsCylinder(modelData, 140.0f * (scaleXZ / max),
        320.0f * (mScale.y / max)
    );
}

#include "JSystem/JAudio/JASFakeMatch2.h"
