#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3D/J3DAnmTexPattern.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JMath/JMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/KartChecker.h"
#include "Kaneshige/LightMgr.h"
#include "Kaneshige/RaceMgr.h"
#include "Kaneshige/SimpleDrawer.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/RivalSpeedCtrl.h"
#include "Shiraiwa/Objects/MapObjAward.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JSupport/JSUList.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjDemoObj.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"
#include "kartEnums.h"
#include "mathHelper.h"
#include "types.h"


StateObserver::StateFuncSet<TMapObjAwardItemBox> TMapObjAwardItemBox::sTable[4] = {
    { 0, &TMapObjAwardItemBox::initFunc_Appear, &TMapObjAwardItemBox::doFunc_Appear },
    { 1, &TMapObjAwardItemBox::initFunc_Roll, &TMapObjAwardItemBox::doFunc_Roll },
    { 2, &TMapObjAwardItemBox::initFunc_Disappear, &TMapObjAwardItemBox::doFunc_Disappear },
    { 3, &TMapObjAwardItemBox::initFunc_Hide, &TMapObjAwardItemBox::doFunc_Hide },
};

// .comm sVelocity0__15TMapObjAwardCup, 0x4C, 4
// FIX: Size doesn't match... what should this actually be?
JGeometry::TVec3f TMapObjAwardCup::sVelocity0(0.0f, 84.0f, 0.0f);

StateObserver::StateFuncSet<TMapObjAwardCup> TMapObjAwardCup::sTable[3] = {
    {0, &TMapObjAwardCup::initFunc_InBox, &TMapObjAwardCup::doFunc_InBox},
    {1, &TMapObjAwardCup::initFunc_FlyOut, &TMapObjAwardCup::doFunc_FlyOut},
    {2, &TMapObjAwardCup::initFunc_Roll, &TMapObjAwardCup::doFunc_Roll},
};

f32 TMapObjAwardItemBox::sRollSpeed = 0.001f;
s16 TMapObjAwardItemBox::sAppearFrame = 5;
s16 TMapObjAwardItemBox::sDisappearFrame = 5;

f32 TMapObjAwardCup::sAirFriction = 0.9999f;
f32 TMapObjAwardCup::sGravity = 0.8f;
f32 TMapObjAwardCup::sScaleVel = 0.05f;
s16 TMapObjAwardCup::sJumpFrame = 60;
f32 TMapObjAwardCup::sRollSpeed = 0.002f;
f32 TMapObjAwardCup::sRollSpeedMax = 0.03f;
s16 TMapObjAwardCup::sRotCycle = 0xf;
s16 TMapObjAwardCup::sAxisRotCycle = 0xccc;
s16 TMapObjAwardCup::sFlyOutRotCycle = 0x4000;
f32 TMapObjAwardCup::sRotVel0 = 0.4f;
f32 TMapObjAwardCup::sRotBrake = 0.003f;
s16 TMapObjAwardCup::sAxisCycleDecel = 30;

TAwardItemBoxSupervisor *TMapObjAwardItemBox::sSupervisor;
f32 TMapObjAwardCup::sOffsetHeight;
TAwardCupSupervisor *TMapObjAwardCup::sSupervisor;
J3DAnmTexPattern *TMapObjAwardCup::sAwardCupBtpAnm;

// Each of these are the GeographyObj ID for each cup,
// where every three of the same value correspond to the
// subsequent engine speed of that cup (50cc, 100cc, 150cc.)
const int TMapObjAwardItemBox::scIDTable[18] = {
    0xd67, 0xd67, 0xd67,      // Mushroom Cup;      50cc, 100cc, 150cc
    0xd68, 0xd68, 0xd68,      // Flower Cup;        50cc, 100cc, 150cc
    0xd69, 0xd69, 0xd69,      // Star Cup;          50cc, 100cc, 150cc
    0xd6a, 0xd6a, 0xd6a,    // Special Cup;       50cc, 100cc, 150cc
    0xd6b, 0xd6b, 0xd6b,   // Mirror Mode Cup;   50cc, 100cc, 150cc
    0xd6c, 0xd6c, 0xd6c,   // All Cup Tour Cup;  50cc, 100cc, 150cc
};


TMapObjAwardItemBox::TMapObjAwardItemBox(const CrsData::SObject &sObject) : TMapObjHioNode(sObject), mLinkAwardItemBox(this) {
    int cupId;
    switch (RCMGetManager()->getRaceGpCup()) {
    case MUSHROOM_CUP:
        cupId = MUSHROOM_CUP;
        break;

    case FLOWER_CUP:
        cupId = FLOWER_CUP;
        break;

    case STAR_CUP:
        cupId = STAR_CUP;
        break;

    case SPECIAL_CUP:
        cupId = SPECIAL_CUP;
        break;

    case ALL_CUP_TOUR:
        cupId = CUP_MAX;        // Why 5, and not 4? Skipping Mirror mode, maybe?
        break;

    default:
        #line 93
        JUT_ASSERT(false);
        break;
    }

    int type = mObjData->mParam1 + (cupId * 3) - 1;

    #line 99
    JUT_MINMAX_ASSERT(0, type, 18);

    mAwardCup = (TMapObjAwardCup *)GetGeoObjMgr()->createSubObj(scIDTable[type]);

    s16 newFrame = mObjData->mParam1 - 1;
    mAwardCup->mAnmObjMaterial.getFrameCtrl()->setFrame(newFrame);

    mAwardCup->_1c0 = (mObjData->mParam1 == 3) ? -1 : 1;

    if (getSupervisor() == nullptr) {
        sSupervisor = new TAwardItemBoxSupervisor;
    }
    
    sSupervisor->entry(this);
}

TMapObjAwardItemBox::~TMapObjAwardItemBox() {
    sSupervisor = nullptr;
}

void TMapObjAwardItemBox::createColModel(J3DModelData *modelData) {
    createBoundsSphere(150.0f, 1.0f);
}

// FIX: MJB - Register mismatch.
void TMapObjAwardItemBox::reset() {
    resetObject();
    ResetState();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    
    for (int i = 0; i < 2; i++) {
        _180[i] = nullptr;
    }
    
    JGeometry::TVec3f local_48;
    mRotMtx.getXDir(local_48);
    local_48.normalize();

    f32 jumpFrame = mAwardCup->sJumpFrame;
    f32 cupFrame = mObjData->mParam2;
    f32 scale = cupFrame / jumpFrame;

    mAwardCup->_228.scale(scale, local_48);
    mAwardCup->setOrgMtx(mRotMtx);
}

void TMapObjAwardItemBox::skip() {
    int rank = -1;
    int awardKartNo = RCMGetManager()->getAwardKartNo();
    if (awardKartNo != -1) {
        rank = RCMGetKartChecker(awardKartNo)->getRank();
    }
    if (rank == mObjData->mParam1) {
        mAwardCup->skip(mPos);
    }
    setState(3);
}

void TMapObjAwardItemBox::doKartColCallBack(int kartIdx) {
    int rank = RCMGetKartChecker(kartIdx)->getRank();

    if (!tstIsHitKartFlg(kartIdx)) {
        return;
    }

    if (rank != mObjData->mParam1) {
        return;
    }

    if (kartIdx == RCMGetManager()->getAwardKartNo()) {
        mAwardCup->flyOut(mPos);
    }

    JGeometry::TVec3f kartPos;
    ObjUtility::getKartPos(kartIdx, &kartPos);

    _180[0] = JPEffectMgr::getEffectMgr()->createEmt(JPEffectMgr::getEffectMgr()->getHashValue("mk_itemget_normal_a"), kartPos);
    _180[1] = JPEffectMgr::getEffectMgr()->createEmt(JPEffectMgr::getEffectMgr()->getHashValue("mk_itemget_normal_b"), kartPos);

    GetKartCtrl()->getKartEnemy(kartIdx)->getSpeedCtrl()->stop();
    setState(2);
}

void TMapObjAwardItemBox::InitExec() { Observer_FindAndInit(TMapObjAwardItemBox, 4); }

void TMapObjAwardItemBox::MoveExec() { Observer_FindAndExec(TMapObjAwardItemBox, 4); }

void TMapObjAwardItemBox::calc() {
    ExecuteState();
}

void TMapObjAwardItemBox::initFunc_Appear() {
    setAllCheckKartHitFlag();
    clrObjFlagHidding();
    if (getShadowModel() != nullptr) {
        getShadowModel()->setVisibleAll();
    }
    mScale.setAll(0.0f);
}

void TMapObjAwardItemBox::doFunc_Appear() {
    f32 scale = SiUtil::getNormalRange(getStateCount(), 0.0f, sAppearFrame) + 0.01f;
    if (scale >= 1.0f) {
        scale = 1.0f;
        setState(1);
    }
    mScale.setAll(scale);
}

void TMapObjAwardItemBox::initFunc_Roll() {
    _1b0 = 0.0f;
    _1b4 = 0.0f;
    mRotMtx.getXDir(mModelXDir);
    mRotMtx.getYDir(mModelYDir);
    mRotMtx.getZDir(mModelZDir);
    mRandDir = getGeoRnd()->getRandom();
}

void TMapObjAwardItemBox::doFunc_Roll() {
    rotAnimation();
}

void TMapObjAwardItemBox::rotAnimation() {
    JGeometry::TVec3f scaledY;
    _1b0 += JMASSin(mRandDir) * sRollSpeed;
    scaledY.scale(_1b0, mModelYDir);
    
    JGeometry::TVec3f scaledX;
    _1b4 += JMASCos(mRandDir) * -sRollSpeed;
    scaledX.scale(_1b4, mModelXDir);

    mModelZDir.add(scaledX);
    mModelZDir.add(scaledY);
    mModelZDir.normalize();
    mModelYDir.cross(mModelZDir, mModelXDir);
    mModelYDir.normalize();
    mModelXDir.cross(mModelYDir, mModelZDir);
    mRandDir += 0x100;
    stMakeRMtx(mRotMtx, mModelXDir, mModelYDir, mModelZDir);
}

void TMapObjAwardItemBox::initFunc_Disappear() {
    mScale.setAll(1.0f);
    clrAllCheckKartHitFlag();
}

void TMapObjAwardItemBox::doFunc_Disappear() {
    f32 scale = 1.0f - SiUtil::getNormalRange(getStateCount(), 0.0f, sDisappearFrame);
    if (scale <= 0.0f) {
        setObjFlagHidding();
        scale = 1.0f;
        setState(3);
    }
    mScale.setAll(scale);
}

void TMapObjAwardItemBox::initFunc_Hide() {
    if (getShadowModel() != nullptr) {
        getShadowModel()->clrVisibleAll();
    }
    setObjFlagHidding();
}

void TMapObjAwardItemBox::doFunc_Hide() {}


TMapObjAwardCup::TMapObjAwardCup(u32 id) : TMapObjDemoObj(id), _190(this) {
    clrDemoFlag(0x1);
    setObjFlagAwardCup();
    createSoundMgr();
    setObjFlagSimpleDraw();

    if (getSupervisor() == nullptr) {
        sSupervisor = new TAwardCupSupervisor;
    }
    sSupervisor->entry(this);
}

void TMapObjAwardCup::InitExec() { Observer_FindAndInit(TMapObjAwardCup, 3); }

void TMapObjAwardCup::MoveExec() { Observer_FindAndExec(TMapObjAwardCup, 3); }

void TMapObjAwardCup::createModel(JKRSolidHeap *heap, u32 p2) {
    mAnmObjMaterial.setExModel(&mModel);
    mAnmObjMaterial.setAnmBase(sAwardCupBtpAnm);
    mAnmObjMaterial.initFrameCtrl(mAnmObjMaterial.getAnmBase());
}

void TMapObjAwardCup::update() {
    mAnmObjMaterial.anmFrameProc();
    setModelMatrixAndScale();
    mModel.update(0);
}

void TMapObjAwardCup::reset() {
    GeographyObj::resetObject();
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
    setObjFlagHidding();
    mIsCupInBox = true;
    ResetState();
    PSMTXIdentity(_1e8);
    _224 = 0;
    _228.zero();
    if (sOffsetHeight == 0.0f) {
        sOffsetHeight = makeOffset();
    }
}

void TMapObjAwardCup::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    void *awardRes = RCMGetManager()->getAwardArc()->getResource(getBtpFileName());
    mAnmObjMaterial.setupTexPatternAnmData(&sAwardCupBtpAnm, modelData, awardRes);
}

void TMapObjAwardCup::setOrgMtx(const JGeometry::TPos3f &newOrg) {
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirZ;
    JGeometry::TVec3f vecDirY;

    newOrg.getXDir(vecDirX);
    newOrg.getYDir(vecDirY);
    newOrg.getZDir(vecDirZ);
    PSVECCrossProduct(&vecDirZ, &vecDirY, &vecDirX);
    vecDirX.normalize();
    _1e8[0][0] = vecDirX.x;
    _1e8[1][0] = vecDirX.y;
    _1e8[2][0] = vecDirX.z;
    _1e8[0][1] = vecDirZ.x;
    _1e8[1][1] = vecDirZ.y;
    _1e8[2][1] = vecDirZ.z;
    _1e8[0][2] = vecDirY.x;
    _1e8[1][2] = vecDirY.y;
    _1e8[2][2] = vecDirY.z;
}

f32 TMapObjAwardCup::makeOffset() {
    f32 offset = 0.0f;
    f32 vel = sVelocity0.y;
    for (int i = 0; i <= sJumpFrame + 1; i++) {
        vel *= sAirFriction;
        vel -= sGravity;
        offset += vel;
    }

    return offset;
}

void TMapObjAwardCup::flyOut(const JGeometry::TVec3f &newPos) {
    setState(1);
    mPos.set(newPos);
}

void TMapObjAwardCup::skip(const JGeometry::TVec3f &param_1) {
    mPos.scaleAdd(sJumpFrame, _228, param_1);
    mPos.y += sOffsetHeight;
    _222 = 0;
    _218 = sRollSpeedMax;
    _21c = _1c0 * sRollSpeedMax;
    _220 = (_1c0 > 0) ? 0 : 0x8000;

    mRotMtx.getXDir(mModelXDir);
    mRotMtx.getYDir(mModelYDir);
    mRotMtx.getZDir(mModelZDir);
    mIsCupInBox = false;
    setState(2);
}

void TMapObjAwardCup::initFunc_InBox() {
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
    mIsCupInBox = true;
}

void TMapObjAwardCup::doFunc_InBox() {}

void TMapObjAwardCup::initFunc_FlyOut() {
    mScale.setAll(0.0f);
    mVel.set(sVelocity0);
    mIsCupInBox = false;

    mRotMtx.getXDir(mModelXDir);
    mRotMtx.getYDir(mModelYDir);
    mRotMtx.getZDir(mModelZDir);

    _218 = sRotVel0;
    _21c = _1c0 * sRotVel0;

    _220 = (_1c0 > 0) ? 0 : 0x8000;

    _222 = 0;
    mSoundMgr->setSe(0x20044);
}

void TMapObjAwardCup::doFunc_FlyOut() {
    mVel.scale(sAirFriction);
    mVel.y -= sGravity;

    mPos.add(mVel);
    mPos.add(_228);

    f32 newScale = mScale.y;
    if (newScale < 1.0f) {
        newScale += sScaleVel;
        if (newScale > 1.0f) {
            newScale = 1.0f;
        }
        mScale.setAll(newScale);
    }

    rotAnimation(false);

    if (getStateCount() > sJumpFrame) {
        setState(2);
    }
}

void TMapObjAwardCup::initFunc_Roll() {}

void TMapObjAwardCup::doFunc_Roll() {
    rotAnimation(true);
}

void TMapObjAwardCup::draw(u32) {}  // UNUSED

void TMapObjAwardCup::rotAnimation(bool param_1) {
    JGeometry::TVec3f modelXDirScaled;
    JGeometry::TPos3f modelDir;
    JGeometry::TPos3f local_e4;
    JGeometry::TVec3f axisX;
    JGeometry::TVec3f axisY;

    f32 fVar2 = param_1
        ? sRollSpeedMax
        : JMAAbs(_21c);

    f32 newRollAngle = -sRollSpeed * JMASCos(_220);
    _21c += newRollAngle;

    if (JMAAbs(_21c) > fVar2) {
        f32 fVar1;
        if (_21c < 0.0f) {
            fVar1 = -1.0f;
        } else {
            fVar1 = 1.0f;
        }

        _21c -= sRotBrake * fVar1;

        if (JMAAbs(_21c) < fVar2) {
            if (_21c < 0.0f) {
                fVar1 = -1.0f;
            } else {
                fVar1 = 1.0f;
            }

            _21c = fVar2 * fVar1;
        }
    }

    modelXDirScaled.scale(_21c, mModelXDir);
    mModelZDir += modelXDirScaled;
    mModelZDir.normalize();

    mModelYDir.cross(mModelZDir, mModelXDir);
    mModelYDir.normalize();

    mModelXDir.cross(mModelYDir, mModelZDir);
    mModelXDir.normalize();

    PSMTXIdentity(modelDir);
    modelDir.setXYZDir(mModelXDir, mModelYDir, mModelZDir);

    PSMTXIdentity(local_e4);

    static JGeometry::TVec3f axisZ(0.0f, 0.0f, 1.0f);
    s16 flyOutRotCycle = sFlyOutRotCycle;
    if (param_1) {
        flyOutRotCycle = sAxisRotCycle;
    }
    if (_224 > flyOutRotCycle) {
        _224 -= sAxisCycleDecel;
    }
    if (_224 < flyOutRotCycle) {
        _224 = flyOutRotCycle;
    }

    s16 angle = _224 * JMASCos(_222);
    axisY.y = JMASCos(angle);
    axisY.x = JMASSin(angle);
    axisY.z = 0.0f;

    axisX.cross(axisY, axisZ);
    axisX.normalize();

    local_e4.setXYZDir(axisX, axisY, axisZ);

    PSMTXConcat(modelDir, local_e4, modelDir);
    PSMTXConcat(_1e8, modelDir, mRotMtx);
    
    _220 += sRotCycle;
    if (param_1) {
        _222 += sRotCycle;
    } else {
        _222 += sRotCycle * 0xf;
    }
}

void TMapObjAwardCup::calc() {
    ExecuteState();
}

void TAwardCupSupervisor::entry(TMapObjAwardCup *awardCup) {
    mList.append(&awardCup->_190);
}

void TAwardCupSupervisor::draw(u32 viewNo) {
    for (JSULink<TMapObjAwardCup> *awardCupsList = mList.getFirst(); awardCupsList != nullptr; awardCupsList = awardCupsList->getNext()) {
        TMapObjAwardCup *awardCup = awardCupsList->getObject();

        if (!awardCupsList->getObject()->mIsCupInBox) { // Why not use the obj you jusst created?!
            awardCup->mAnmObjMaterial.anmFrameProc();

            J3DModelData *modelData = awardCup->mModel.getModelData();
            SimpleDrawer simpleDrawer;
            simpleDrawer.drawInit(modelData);

            while (simpleDrawer.loadPreDrawSetting()) {
                LightObj *light = LightMgr::getManager()->searchLight(0x53434e30 + viewNo); // SCN0
                MtxPtr effectMtx = light->getEffectMtx();

                awardCup->mModel.setScale(awardCup->mScale);
                awardCup->mModel.clipAll(viewNo, true);
                awardCup->mModel.simpleDraw(viewNo, effectMtx, 1);
            }
        } else {
            awardCup->mModel.hide();
        }
    }
}

TAwardItemBoxSupervisor::TAwardItemBoxSupervisor() {
    mList.initiate();
}

void TAwardItemBoxSupervisor::entry(TMapObjAwardItemBox *awardItemBox) {
    mList.append(&awardItemBox->mLinkAwardItemBox);
}

void TAwardItemBoxSupervisor::skip() {
    for (JSULink<TMapObjAwardItemBox> *link = mList.getFirst(); link != nullptr; link = link->getNext()) {
        link->getObject()->skip();
    }
}

bool TAwardItemBoxSupervisor::isRollState() {
    bool result = false;

    for (JSULink<TMapObjAwardItemBox> *link = mList.getFirst(); link != nullptr; link = link->getNext()) {
        TMapObjAwardItemBox *itemBox = link->getObject();

        result |= (itemBox->mAwardCup)
            ? (itemBox->mAwardCup->getState() == 2)
            : false;
    }
    return result;
}

#include "JSystem/JAudio/JASFakeMatch2.h"
