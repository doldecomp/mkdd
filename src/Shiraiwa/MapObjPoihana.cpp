#include "Shiraiwa/Objects/MapObjPoihana.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3D/J3DAnmTexPattern.h"
#include "JSystem/J3D/J3DFrameCtrl.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Util.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JMath/JMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/ResMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/ItemThunder.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/AnmPlayer.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"
#include "math.h"
#include "types.h"


TAnmInfo TMapObjPoihana::sAnmInfos[3] = {
    {"/Objects/Poihana1_wait.bck", nullptr, nullptr, 2, 0, 0, 255, 0},
    {"/Objects/Poihana1_damage.bck", nullptr, nullptr, 2, 0, 0, 255, 0},
    {"/Objects/Poihana1_throw.bck", nullptr, nullptr, 2, 0, 1, 255, 0},
};

StateObserver::StateFuncSet<TMapObjPoihana> TMapObjPoihana::sTable[7] = {
    {0, &TMapObjPoihana::initFunc_Wake, &TMapObjPoihana::doFunc_Wake},
    {1, &TMapObjPoihana::initFunc_Throw, &TMapObjPoihana::doFunc_Throw},
    {2, &TMapObjPoihana::initFunc_Run, &TMapObjPoihana::doFunc_Run},
    {3, &TMapObjPoihana::initFunc_Rest, &TMapObjPoihana::doFunc_Rest},
    {4, &TMapObjPoihana::initFunc_Walk, &TMapObjPoihana::doFunc_Walk},
    {5, &TMapObjPoihana::initFunc_Down, &TMapObjPoihana::doFunc_Down},
    {6, &TMapObjPoihana::initFunc_Buried, &TMapObjPoihana::doFunc_Buried},
};

s16 TMapObjPoihana::sDownRotFrame = 30;
s16 TMapObjPoihana::sDownFrame = 200;
f32 TMapObjPoihana::sFirstDownVel = 60.0f;
f32 TMapObjPoihana::sFirstDownVelBanana = 40.0f;
f32 TMapObjPoihana::sDownGravity = 4.0f;
s16 TMapObjPoihana::sWakeRotFrame = 30;
f32 TMapObjPoihana::sFirstWakeVel = 70.0f;
f32 TMapObjPoihana::sWakeGravity = 4.0f;
f32 TMapObjPoihana::sWalkWaterRatio = 0.6f;
f32 TMapObjPoihana::sDefaultSearchRadius = 6000.0f;
f32 TMapObjPoihana::sRunSpeedRatio = 2.0f;
f32 TMapObjPoihana::sRunRotRatio =  4.0f;
s16 TMapObjPoihana::sKartIgnoreFrame = 240;
s16 TMapObjPoihana::sWaterDecelBand = 200;
s16 TMapObjPoihana::sReSearchFrame = 30;
f32 TMapObjPoihana::sRunVelScale = 30.0f;
f32 TMapObjPoihana::sWalkSpeed = 9.0f;

J3DAnmTexPattern *TMapObjPoihana::sPoihanaBtpAnm;

f32 TMapObjPoihana::sSinkOffset;

TPoihanaSupervisor *TMapObjPoihana::sSupervisor;

const s8 TMapObjPoihana::sAnmTable[7] = {
    0, 1, 1, 1, 2, 0, 0
};



void TMapObjPoihana::checkItemHitting() {
    if (tstItemHitting()) {
        switch (mColItemObj->getKind()) {
        case 0:
        case 1:
        case 2:
        case 7:
        case 8:
        case 9:
            mColPos = mColItemObj->mPos;
            mFirstDownVel = sFirstDownVel;
            setAnmTableState(1);
            break;

        case 3:
        case 4:
            if (mColItemObj->tstTransfer2()) {
                mColPos = mColItemObj->mPos;
                mFirstDownVel = sFirstDownVelBanana;
                setAnmTableState(1);
            }
            break;
        }
    }
}



TMapObjPoihana::TMapObjPoihana(const CrsData::SObject &sObject) : TKartThrower(sObject), mLinkPoihana(this) {
    _1e4 = 0.015f;
    _1e8 = 0.3f;
    _250 = 50;

    #line 85
    JUT_ASSERT_MSG(mObjData->mPathID != 0xffff, "TMapObjPoihana: Illegal Path ID");

    NewAnmCtrl();
    createSoundMgr();

    mCrsGround = new CrsGround(RCMGetCourse());
    mAnmPlayer.resetAnimations(sAnmInfos, 3);

    mPathWalk = new TPathWalk(&sObject);

    _260 = mObjData->mParam4;
    if (_260 <= 0) {
        _260 = 4000;
    }

    if (getSupervisor() == nullptr) {
        sSupervisor = new TPoihanaSupervisor();
    }
    sSupervisor->entry(this);
}

TMapObjPoihana::~TMapObjPoihana() {
    delete mPathWalk;
    sSupervisor = nullptr;
}

const char *TMapObjPoihana::getBmdFileName() {
    static const char *cBmdName = "/Objects/Poihana1.bmd";
    return cBmdName;
}

const char *TMapObjPoihana::getShadowBmdFileName() {
    static const char *cShadowBmdName = "/Objects/Poihana1Shadow.bmd";
    return cShadowBmdName;
}

void TMapObjPoihana::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    TAnmPlayer::loadAnimations(sAnmInfos, 3, modelData, ResMgr::mcArcCourse);
    J3DAnmObjMaterial::setupTexPatternAnmData(&sPoihanaBtpAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/Poihana1.btp"));
}

void TMapObjPoihana::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x80000, 0);
    mAnmPlayer.registAnimations(mAnmCtrl, &mModel, sAnmInfos, 3);
    mAnmObjMaterial.setExModel(&mModel);
    mAnmObjMaterial.setAnmBase(sPoihanaBtpAnm);
    mAnmObjMaterial.initFrameCtrl(mAnmObjMaterial.mAnmBase);
}

void TMapObjPoihana::createColModel(J3DModelData *modelData) {
    static const f32 cBoundHeight = (mScale.y * 270.0f) / mScale.x;
    createBoundsCylinder(modelData, 200.0f, cBoundHeight);

    f32 yVal = 230.0f;
    JGeometry::TVec3f newColObjPos(0.0f, -yVal, 0.0f);
    setColObjPos(newColObjPos, 0);
}

void TMapObjPoihana::InitExec() { Observer_FindAndInit(TMapObjPoihana, 7); }

void TMapObjPoihana::MoveExec() { Observer_FindAndExec(TMapObjPoihana, 7); }

void TMapObjPoihana::update() {
    mAnmObjMaterial.anmFrameProc();
    setModelMatrixAndScale();
    mModel.update(0);
}

void TMapObjPoihana::reset() {
    GeographyObj::resetObject();
    _58 = 0xe;
    setAllCheckKartHitFlag();
    setObjFlagCheckItemHitting();
    clrObjFlagCheckGeoHitting();
    ResetState();

    mAnmPlayer.init(mAnmCtrl, sAnmInfos, 3);
    mAnmObjMaterial.resetFrame();

    u16 frame = mObjData->mParam1;
    if (frame >= 4) {
        frame = 0;
    }
    mAnmObjMaterial.setFrame(frame);

    _250 = mObjData->mParam2;
    if (_250 < 0) {
        _250 = 0;
    }

    u8 p3Clamp = mObjData->mParam3;
    if (p3Clamp > 0x5a) {
        p3Clamp = 0x5a;
    }

    s32 unknownTwo = p3Clamp;
    _25e = unknownTwo * 182.04445f;

    _24c = sDefaultSearchRadius;
    if (_24c < _260) {
        _24c = _260;
    }

    clearAllIgnore();
    mTargetKartIdx = -1;
    mKartSearchCounter = 0;
    mFirstDownVel = 0.0f;
    _25c = false;
    _254 = mScale.y * 230.0f;

    PSMTXIdentity(_1ec);
    PSMTXIdentity(_21c);
    mPos.y += _254;
    _1e0 = sWalkSpeed * mScale.z;

    mPathWalk->init(&mPoihanaPos, &mVel, &mRotMtx);
    mPathWalk->setParameters(_1e0, _1e4, _1e8, 0.0f, 0.0f);
    mPathWalk->reset();

    mColPos.zero();
    _1d0.zero();
    mPoihanaPos.zero();

    PSMTXIdentity(mRotMtx);

    mReaction.setFlg(3, 2);
    JGeometry::TVec3f newColObjPos(0.0f, -(_254 / mScale.y), 0.0f);
    setColObjPos(newColObjPos, 0);
}

void TMapObjPoihana::calc() {
    mPathWalk->update();
    ExecuteState();
    mAnmPlayer.update();
    moveShadowModel();
}


void TMapObjPoihana::doKartColCallBack(int kartNo) {
    switch (getState()) {
        case 0:
        case 4:
        case 5:
        case 6: {
            _58 = 0xe;

            if ((GetKartCtrl()->GetKartStatus(kartNo) & 0x40000) != 0) {
                ObjUtility::getKartPos(kartNo, &mColPos);
                mFirstDownVel = sFirstDownVel;
                setAnmTableState(1);
                break;
            }

            if (isIgnore(kartNo)) {
                break;
            }

            JGeometry::TVec3f dir;
            ObjUtility::getKartPos(kartNo, &dir);

            dir.sub(mPos);
            dir.y = 0.0f;
            dir.normalize();

            mPathWalk->forceTurn(dir);

            ignoreKart(kartNo);
            mPathWalk->_88->_20 = 0;

            setAnmTableState(4);
            break;
        }
        
        case 1:
        case 3:
            _58 = 0;
            break;

        case 2:
        default:
            _58 = 1;
            break;
    }
}

bool TMapObjPoihana::isIgnore(int idx) {
    return mTargetableKarts[idx] != 0;
}

void TMapObjPoihana::ignoreKart(int idx) {
    mTargetableKarts[idx] = sKartIgnoreFrame;
}

void TMapObjPoihana::clearAllIgnore() {
    for (int i = 0; i < 8; i++) {
        mTargetableKarts[i] = 0;
    }
}

void TMapObjPoihana::initFunc_Walk() {
    changeWalkSpeed(1.0f, 1.0f);
    mPathWalk->_94 = 1;
    _25c = false;
    mPoihanaPos.set(mPos.x, mPos.y - _254, mPos.z);
}

void TMapObjPoihana::doFunc_Walk() {
    updatePosition();
    checkItemHitting();
    changeDownState(1);
    changeSpeedForWater(1.0f, 1.0f);

    int nearestKart = searchKart();
    if (nearestKart != -1) {
        mTargetKartIdx = nearestKart;
        setAnmTableState(5);
    }
}

void TMapObjPoihana::updatePosition() {
    JGeometry::TVec3f posDiff;
    posDiff.sub(mPoihanaPos, mObjData->position);

    if (posDiff.squared() > SQUARE((_260 - 500.0f))) {
        setAnmTableState(6);
    }
    mPos.set(mPoihanaPos.x, mPoihanaPos.y + _254, mPoihanaPos.z);
}

void TMapObjPoihana::changeSpeedForWater(f32 xSpeed, f32 ySpeed) {
    mCrsGround->search(mPos);

    f32 waterDepth = mCrsGround->getWaterHeight() - mPoihanaPos.y;
    if (waterDepth < 0.0f) {
        if (_25c) {
            changeWalkSpeed(xSpeed, ySpeed);
            _25c = false;
        }
    } else {
        f32 gradient = 1.0f - SiUtil::getNormalRange(waterDepth, 0.0f, sWaterDecelBand);
        f32 invRatio = 1.0f - sWalkWaterRatio;

        f32 speedScale = invRatio * gradient + sWalkWaterRatio;
        changeWalkSpeed(xSpeed * speedScale, ySpeed * speedScale);
        _25c = true;
    }
}

int TMapObjPoihana::searchKart() {
    int kartIndex = -1;
    changeDownState(1);

    if (mKartSearchCounter < sReSearchFrame) {
        mKartSearchCounter++;
        return kartIndex;
    }

    mKartSearchCounter = 0;
    f32 closestKart = SQUARE(_24c);

    for (int i = 0; i < RCMGetManager()->getKartNumber(); i++) {
        if (mTargetableKarts[i] != 0) {
            continue;
        }

        JGeometry::TVec3f kartPosVec;
        ObjUtility::getKartPos(i, &kartPosVec);
        kartPosVec.sub(mPos);

        f32 newClosestKart = kartPosVec.squared();
        if (newClosestKart < closestKart) {
            closestKart = newClosestKart;
            kartIndex = i;
        }
    }

    return kartIndex;
}

void TMapObjPoihana::changeWalkSpeed(f32 xVel, f32 yVel) {
    mPathWalk->setParameters(_1e0 * xVel, _1e4 * yVel, _1e8, 0.0f, 0.0f);
    mAnmPlayer.mController->getFrameCtrl(mAnmPlayer.getCurAnmNumber())->setRate(xVel);
}

void TMapObjPoihana::initFunc_Down() {
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
    
    JGeometry::TVec3f yDiff;
    JGeometry::TVec3f posDiff;
    static JGeometry::TVec3f yAxis(0.0f, 1.0f, 0.0f);
    
    posDiff.sub(mColPos, mPos);
    posDiff.y = 0.0f;
    posDiff.normalize();

    yDiff.cross(posDiff, yAxis);
    yDiff.normalize();

    _1ec.setRotate(yDiff,  JGeometry::TUtilf::PI() / sDownRotFrame);
    _21c.setRotate(yDiff, -JGeometry::TUtilf::PI() / sWakeRotFrame);

    mPathWalk->_94 = 0;
    
    posDiff.negate();
    posDiff.y = 1.0f;
    posDiff.normalize();

    posDiff.scale(mFirstDownVel);
    mVel.set(posDiff);
}

void TMapObjPoihana::doFunc_Down() {
    bool updateAnmState = true;
    if (getStateCount() < sDownRotFrame) {
        updateAnmState = false;
        PSMTXConcat(_1ec, mRotMtx, mRotMtx);
    }

    JGeometry::TVec3f pos = mPos;
    mPos.add(mVel);

    mVel.y -= sDownGravity;

    fixWall(mPos, pos);
    mCrsGround->search(mPos);

    if (mPos.y < mCrsGround->getHeight() - sSinkOffset) {
        mPos.y = mCrsGround->getHeight() - sSinkOffset;

        if (updateAnmState) {
            setAnmTableState(2);
        }
    }
}

void TMapObjPoihana::fixWall(const JGeometry::TVec3f &param_1, const JGeometry::TVec3f &param_2) {
    CrsGround crsGround(RCMGetCourse());
    crsGround.search(param_1, param_2);
    
    JGeometry::TVec3f wallNormal;
    f32 wallScalar = crsGround.getWallNormal(&wallNormal, nullptr);

    if (wallScalar > -1.0f) {
        wallNormal.normalize();
        mPos.scaleAdd(wallScalar, wallNormal, param_1);
    }
}

void TMapObjPoihana::initFunc_Buried() {
    setAllCheckKartHitFlag();
    setObjFlagCheckItemHitting();

    mReaction.setFlg(8, 2);
    mReaction.setFlg(9, 2);
    mReaction.setFlg(7, 0);
    mReaction.setFlg(2, 2);
    mReaction.setFlg(0, 1);
    mReaction.setFlg(1, 1);

    JGeometry::TVec3f colObjPos;
    colObjPos.set(0.0f, (0.5f * _254) / mScale.y, 0.0f);
    setColObjPos(colObjPos, 0);
}

void TMapObjPoihana::doFunc_Buried() {
    if (getStateCount() > sDownFrame) {
        setAnmTableState(3);
    }
}

void TMapObjPoihana::initFunc_Wake() {
    mVel.set(0.0f, sFirstWakeVel, 0.0f);
}

void TMapObjPoihana::doFunc_Wake() {
    bool updateAnmState = true;
    if (getStateCount() < sWakeRotFrame) {
        updateAnmState = false;
        PSMTXConcat(_21c, mRotMtx, mRotMtx);
    }

    mPos.add(mVel);
    mVel.y -= sWakeGravity;
    mCrsGround->search(mPos);

    JGeometry::TVec3f wallNormal;
    f32 wallScalar = mCrsGround->getWallNormal(&wallNormal, nullptr);

    if (wallScalar > -1.0f) {
        wallNormal.normalize();
        mPos.scaleAdd(wallScalar, wallNormal, mPos);
    }

    if (mVel.y < 0.0f && mPos.y < _254 + mCrsGround->getHeight()) {
        mPos.y = _254 + mCrsGround->getHeight();

        if (updateAnmState) {
            setAnmTableState(6);
        }
    }
}

void TMapObjPoihana::initFunc_Throw() {
    mPathWalk->_94 = 0;
}

void TMapObjPoihana::doFunc_Throw() {
    if (mAnmPlayer._10 == false) {
        setAnmTableState(0);
    }
}

void TMapObjPoihana::initFunc_Run() {
    _25c = 0;

    f32 runSpeedRatio = sRunSpeedRatio;
    mPathWalk->setParameters(_1e0 * runSpeedRatio, _1e4 * sRunRotRatio, _1e8, 0.0f, 0.0f);
    mAnmPlayer.mController->getFrameCtrl(mAnmPlayer.getCurAnmNumber())->setRate(runSpeedRatio);

    JGeometry::TVec3f pos;
    ObjUtility::getKartPos(mTargetKartIdx, &pos);

    TPathUtilInterrupt *pathUtilInterrupt = mPathWalk->_88;
    pathUtilInterrupt->_14.set(pos);

    TPathWalk *pathWalk = mPathWalk;
    pathWalk->_88->_20 = true;
    pathWalk->_4->_28 = false;
    pathWalk->_8->_28 = false;
}

void TMapObjPoihana::doFunc_Run() {
    JGeometry::TVec3f target;               // 0x44
    JGeometry::TVec3f vel;                  // 0x38
    JGeometry::TVec3f pos;                  // 0x2c
    JGeometry::TVec3f diff;                 // 0x20
    JGeometry::TVec3f poihanaPosDiff;       // 0x14

    ObjUtility::getKartPos(mTargetKartIdx, &pos);
    ObjUtility::getKartVel(mTargetKartIdx, &vel);

    vel.scale(sRunVelScale);
    target.add(pos, vel);
    diff.sub(target, mPos);
    if (diff.dot(vel) > 0.0f) {
        target.sub(vel);
    }

    mPathWalk->_88->_14.set(target);

    changeSpeedForWater(sRunSpeedRatio, sRunRotRatio);
    changeDownState(1);
    updatePosition();
    checkItemHitting();

    poihanaPosDiff.sub(mPoihanaPos, pos);
    if (poihanaPosDiff.squared() > SQUARE(_24c)) {
        setAnmTableState(6);
    }
    callWalkSound();
}

void TMapObjPoihana::callWalkSound() {
    J3DFrameCtrl *currentFrame = mAnmPlayer.mController->getFrameCtrl(mAnmPlayer.getCurAnmNumber());

    if ((SiUtil::checkPassFrame(16.0f, currentFrame->getFrame(), currentFrame->getRate()) & 0xff) != 0) {
        if (_25c) {
            getSoundMgr()->setSe(0x4003d);
        } else {
            getSoundMgr()->setSe(0x40039);
        }
    }

    if ((SiUtil::checkPassFrame(46.0f, currentFrame->getFrame(), currentFrame->getRate()) & 0xff) != 0) {
        if (_25c) {
            getSoundMgr()->setSe(0x4003e);
        } else {
            getSoundMgr()->setSe(0x4003a);
        }
    }
}

void TMapObjPoihana::initFunc_Rest() {
    f32 scale = 1.0f;
    mPathWalk->setParameters(_1e0 * scale, _1e4 * scale, _1e8, 0.0f, 0.0f);
    mAnmPlayer.mController->getFrameCtrl(mAnmPlayer.getCurAnmNumber())->setRate(1.0f);

    mReaction.setFlg(8, 2);
    mReaction.setFlg(9, 0);
    mReaction.setFlg(7, 0);
    mReaction.setFlg(2, 2);
    mReaction.setFlg(0, 2);
    mReaction.setFlg(1, 0);

    mPathWalk->_94 = 1;
    
    const CrsData::SObject *objData = mObjData;
    TPathUtilInterrupt *pathUtilInterrupt = mPathWalk->_88;
    pathUtilInterrupt->_14.set(objData->position);

    TPathWalk *pathWalk = mPathWalk;
    pathWalk->_88->_20 = true;
    pathWalk->_4->_28 = false;
    pathWalk->_8->_28 = false;

    _25c = 0;
    mTargetKartIdx = -1;

    mPoihanaPos.set(mPos.x, mPos.y - _254, mPos.z);
    JGeometry::TVec3f newColObjPos(0.0f, -(_254 / mScale.y), 0.0f);
    setColObjPos(newColObjPos, 0);
}

void TMapObjPoihana::doFunc_Rest() {
    mPos.set(mPoihanaPos.x, mPoihanaPos.y + _254, mPoihanaPos.z);

    checkItemHitting();
    changeSpeedForWater(1.0f, 1.0f);

    if (getStateCount() > sKartIgnoreFrame) {
        setAnmTableState(0);
        changeAllState(0);
    }
}

void TMapObjPoihana::getKartThrowDirPow(JGeometry::TVec3f *throwDir, f32 *throwPow, int idx) {
    if (throwDir != nullptr) {
        getThrowDir(throwDir, idx);
    }
    if (throwPow != 0) {
        *throwPow = getThrowPow();
    }
}

void TMapObjPoihana::getThrowDir(JGeometry::TVec3f *throwDir, s16 param_2) {
    JGeometry::TVec3f newThrowDir;

    mRotMtx.getZDir(newThrowDir);
    newThrowDir.y = 0.0f;
    if (newThrowDir.isZero()) {
        newThrowDir.set(0.0f, 0.0f, -1.0f);
    } else {
        newThrowDir.normalize();
        newThrowDir.negate();
        newThrowDir.scale(JMASCos(_25e));
        newThrowDir.y = JMASSin(_25e);
    }
    throwDir->set(newThrowDir);
}



TPoihanaSupervisor::TPoihanaSupervisor() {
    mList.initiate();
    ItemThunder::entrySuperVisor(this);
}

void TPoihanaSupervisor::entry(TMapObjPoihana *poihana) {
    mList.append(&poihana->mLinkPoihana);
}

void TPoihanaSupervisor::thunderDownAll() {
    for (JSULink<TMapObjPoihana> *ptrLink = mList.getFirst(); ptrLink != nullptr; ptrLink = ptrLink->getNext()) {
        TMapObjPoihana *poihana = ptrLink->getObject();
        poihana->mTargetKartIdx = -1;

        JGeometry::TVec3f vecDirZ;
        poihana->mRotMtx.getZDir(vecDirZ);
        vecDirZ.y = 0.0f;
        vecDirZ.normalize();

        poihana->mColPos.add(poihana->mPos, vecDirZ);
        poihana->mFirstDownVel = TMapObjPoihana::sFirstDownVel;

        poihana->setState(1);
        const s8 anmNo = TMapObjPoihana::sAnmTable[1];
        if (anmNo != poihana->mAnmPlayer.getCurAnmNumber()) {
            poihana->mAnmPlayer._11 = anmNo;
            poihana->mAnmPlayer._10 = true;
            poihana->mAnmPlayer._e |= 1;
        }
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
