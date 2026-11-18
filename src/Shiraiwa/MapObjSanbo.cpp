#include "Shiraiwa/Objects/MapObjSanbo.h"
#include "JSystem/J3D/J3DFrameCtrl.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRDisposer.h"
#include "JSystem/JMath/JMath.h"
#include "JSystem/JUtility/JUTNameTab.h"
#include "Osako/ResMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/ItemThunder.h"
#include "Sato/StateObserver.h"
#include "Sato/stMath.h"
#include "Shiraiwa/AnmPlayer.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"
#include "types.h"
#include "mathHelper.h"

TAnmInfo TMapObjSanbo::sAnmInfos[4] = {
    {"/Objects/Sanbo1_wait.bca", nullptr, nullptr, 2, 0, 0, 255, 0},
    {"/Objects/Sanbo1_fall_left.bca", nullptr, nullptr, 0, 0, 0, 255, 0},
    {"/Objects/Sanbo1_fall_right.bca", nullptr, nullptr, 0, 0, 0, 255, 0},
    {"/Objects/Sanbo1_revive.bca", nullptr, nullptr, 0, 15, 0, 0, 0},
};

StateObserver::StateFuncSet<TMapObjSanbo> TMapObjSanbo::sTable[10] = {
    {0, &TMapObjSanbo::initFunc_Start, &TMapObjSanbo::doFunc_Start},
    {1, &TMapObjSanbo::initFunc_Stand, &TMapObjSanbo::doFunc_Stand},
    {2, &TMapObjSanbo::initFunc_Prepare, &TMapObjSanbo::doFunc_Prepare},
    {3, &TMapObjSanbo::initFunc_Attack, &TMapObjSanbo::doFunc_Attack},
    {4, &TMapObjSanbo::initFunc_Attack, &TMapObjSanbo::doFunc_Attack},
    {5, &TMapObjSanbo::initFunc_Down, &TMapObjSanbo::doFunc_Down},
    {6, &TMapObjSanbo::initFunc_Recover, &TMapObjSanbo::doFunc_Recover},
    {7, &TMapObjSanbo::initFunc_Die, &TMapObjSanbo::doFunc_Die},
    {8, &TMapObjSanbo::initFunc_Hide, &TMapObjSanbo::doFunc_Hide},
    {9, &TMapObjSanbo::initFunc_Born, &TMapObjSanbo::doFunc_Born},
};

const char *TMapObjSanbo::scJointName[5] = {
    "joint5", "joint4", "joint3", "joint2", "joint1",
};
s32 TMapObjSanbo::sJointNum[5] = {-1, -1, -1, -1, -1};

StateObserver::StateFuncSet<TMapObjSanboParts> TMapObjSanboParts::sTable[2] = {
    {0, &TMapObjSanboParts::initFunc_Hide, &TMapObjSanboParts::doFunc_Hide},
    {1, &TMapObjSanboParts::initFunc_Burst, &TMapObjSanboParts::doFunc_Burst},
};

const s8 TMapObjSanbo::sAnmTable[10] = {0, 0, 0, 1, 2, 1, 1, 1, 1, 3};

const s16 TMapObjSanbo::scAppearSoundFrame[5] = {0x5, 0x13, 0x21, 0x2f, 0x3d};

TSanboSupervisor *TMapObjSanbo::sSupervisor;

f32 TMapObjSanbo::sTailOffset = -60.0f;
f32 TMapObjSanbo::sHeadOffset = 60.0f;
s16 TMapObjSanbo::sThunderDownBonus = 120;
f32 TMapObjSanboParts::sGravity = 1.5f;
f32 TMapObjSanboParts::sAirFric = 0.999f;
f32 TMapObjSanboParts::sBurstVelH = 25.0f;
f32 TMapObjSanboParts::sBurstVelV = 50.0f;
f32 TMapObjSanboParts::sRotRad = 0.3f;

TMapObjSanbo::TMapObjSanbo(const CrsData::SObject &sObject) : TMapObjHioNode(sObject), StateObserver(), JKRDisposer(), mLinkSanbo(this) {
    NewAnmCtrl();
    createSoundMgr();

    mAnmPlayer.resetAnimations(sAnmInfos, 4);

    // Create the Pokey's head.
    _1d4[0] = (TMapObjSanboHead *)GetGeoObjMgr()->createSubObj(0x138d);
    // ... and loop to create the other 4 body segments.
    for (int i = 1; i < 5; i++) {
        _1d4[i] = (TMapObjSanboBody *)GetGeoObjMgr()->createSubObj(0x138e);
    }

    if (getSupervisor() == nullptr) {
        sSupervisor = new TSanboSupervisor;
    }
    sSupervisor->entry(this);
}

TMapObjSanbo::~TMapObjSanbo() { sSupervisor = nullptr; }

void TMapObjSanbo::reset() {
    JGeometry::TPos3f mtxScale;

    GeographyObj::resetObject();
    StateObserver::ResetState();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = 4;

    _198 = true;
    _199 = true;

    mAnmPlayer.init(mAnmCtrl, sAnmInfos, 4);
    _19a = mObjData->mParam1;
    _19c = mObjData->mParam2;
    _19e = mObjData->mParam3;
    _1a0 = 0;
    PSMTXInverse(mRotMtx, _1a4);
    PSMTXScale(mtxScale, 1.0f / mScale.x, 1.0f / mScale.y, 1.0f / mScale.z);
    PSMTXConcat(_1a4, mtxScale, _1a4);
    for (int i = 0; i < 5; i++) {
        _1d4[i]->_188.set(mScale);
    }
}

void TMapObjSanbo::calc() {
    ExecuteState();
    checkItemHitting();
    moveColOffset();
    mAnmPlayer.update();
    _198 = false;
}

void TMapObjSanbo::loadAnimation() {
    mAnmPlayer.loadAnimations(sAnmInfos, 4, mModel.getModelData(), ResMgr::mcArcCourse);
}

void TMapObjSanbo::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);
    mAnmPlayer.registAnimations(mAnmCtrl, &mModel, sAnmInfos, 4);

    for (int i = 0; i < 5; i++) {
        sJointNum[i] = mModel.getModelData()->getJointName()->getIndex(scJointName[i]);
    }
}

void TMapObjSanbo::createColModel(J3DModelData *modelData) {
    f32 radius = (mScale.x > mScale.z) ? mScale.x : mScale.z;
    radius = (radius > mScale.y) ? radius : mScale.y;

    GeographyObj::createMultiBoundsPtrAll(2);

    // ...?! Yep, a decision was definitely made here...
    int idx;
    for (u8 i = 0; i < GeographyObj::getAllBoundsNum(); i++) {
        idx = i;
        GeographyObj::createMultiBoundsSphere(idx, 64.0f, radius);
        JGeometry::TVec3f pos(0.0f, 0.0f, 0.0f);
        GeographyObj::setColObjPos(pos, idx);
    }
}

void TMapObjSanbo::InitExec() { Observer_FindAndInit(TMapObjSanbo, 10); }

void TMapObjSanbo::MoveExec() { Observer_FindAndExec(TMapObjSanbo, 10); }

// Unused
void TMapObjSanbo::changeAllState(u16) {}

void TMapObjSanbo::checkItemHitting() {
    if (!tstItemHitting()) {
        return;
    }

    s32 itemKind = mColItemObj->mItemKind;
    if (itemKind < 7) {
        if (itemKind != 1) {
            return;
        }
    } else if (itemKind >= 10) {
        return;
    }
    _1a0 = mObjData->mParam4;
    setState(7);
}

void TMapObjSanbo::doKartColCallBack(int kartIdx) {
    if ((GetKartCtrl()->GetKartStatus(kartIdx) & 0x40000) != 0) {
        _1a0 = mObjData->mParam4;
        setState(7);
    }
}

void TMapObjSanbo::initFunc_Start() {
    clrObjFlagHidding();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
}

void TMapObjSanbo::doFunc_Start() {
    if (getStateCount() > _19c) {
        setAnmTableState(1);
    }
}

void TMapObjSanbo::initFunc_Stand() {
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
}

void TMapObjSanbo::doFunc_Stand() {
    if (getStateCount() > _19a) {
        setAnmTableState(2);
    }
}

void TMapObjSanbo::moveColOffset() {
    JGeometry::TVec3f tailPos;
    JGeometry::TVec3f headPos;
    JGeometry::TVec3f tailDir;
    JGeometry::TVec3f headDir;

    getJointPos(&tailPos, nullptr, sJointNum[3]);
    getJointPos(&headPos, &headDir, sJointNum[0]);

    tailDir.sub(tailPos, mPos);
    tailDir.normalize();

    tailDir.scale(sTailOffset * mScale.y);
    tailPos.add(tailDir);

    headDir.scale(sHeadOffset * mScale.y);
    headPos.add(headDir);

    tailPos.sub(mPos);
    headPos.sub(mPos);

    PSMTXMultVecSR(_1a4, &tailPos, &tailPos);
    PSMTXMultVecSR(_1a4, &headPos, &headPos);

    setColObjPos(tailPos, 0);
    setColObjPos(headPos, 1);
}

void TMapObjSanbo::initFunc_Prepare() {}

void TMapObjSanbo::doFunc_Prepare() {
    if (getAnmCtrl()->getFrameCtrl(0)->getFrame() <= 1.0f) {
        if (getGeoRnd()->get_ufloat_1() < 0.5f) {
            setAnmTableState(3);
        } else {
            setAnmTableState(4);
        }
    }
}

void TMapObjSanbo::initFunc_Attack() {
    if (getAnmCtrl()->mTrans != nullptr) {
        getAnmCtrl()->mTrans->resetFlag();
    }
}

void TMapObjSanbo::doFunc_Attack() {
    J3DFrameCtrl *frameCtrl = mAnmPlayer.mController->getFrameCtrl(mAnmPlayer.getCurAnmNumber());
    if (frameCtrl->getFrame() > 35.0f) {
        setState(5);
    }
}

void TMapObjSanbo::initFunc_Down() {
    if (getAnmCtrl()->mTrans != nullptr) {
        getAnmCtrl()->mTrans->setFlag();
    }
}

void TMapObjSanbo::doFunc_Down() {
    if (getStateCount() > _19e) {
        setState(6);
    }
}

void TMapObjSanbo::initFunc_Recover() {
    if (getAnmCtrl()->mTrans != nullptr) {
        getAnmCtrl()->mTrans->resetFlag();
    }
}

void TMapObjSanbo::doFunc_Recover() {
    J3DFrameCtrl *frameCtrl = mAnmPlayer.mController->getFrameCtrl(mAnmPlayer.getCurAnmNumber());
    if (frameCtrl->checkState(1)) {
        setAnmTableState(1);
    }
}

void TMapObjSanbo::initFunc_Die() {
    setBurst();
    setObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
}

void TMapObjSanbo::doFunc_Die() {
    for (int i = 0; i < 5; i++) {
        if (!_1d4[i]->tstObjFlagHidding()) {
            return;
        }
    }
    setAnmTableState(8);
}

void TMapObjSanbo::initFunc_Hide() {}

void TMapObjSanbo::doFunc_Hide() {
    if (getStateCount() > _1a0) {
        setAnmTableState(9);
    }
}

void TMapObjSanbo::initFunc_Born() {
    clrObjFlagHidding();
    if (getAnmCtrl()->mTrans != nullptr) {
        getAnmCtrl()->mTrans->resetFlag();
    }
}

void TMapObjSanbo::doFunc_Born() {
    AnmControlTrans *pAVar6;

    if (mAnmPlayer.isCurAnmReachEnd()) {
        setAnmTableState(1);
    }

    for (int i = 0; i < 5; i++) {
        J3DFrameCtrl *frameCtrl = mAnmPlayer.mController->getFrameCtrl(mAnmPlayer.getCurAnmNumber());

        if (SiUtil::checkPassFrame(scAppearSoundFrame[i], frameCtrl->getFrame(), frameCtrl->getRate())) {
            getSoundMgr()->setSe(0x4005b);
        }
    }
}

void TMapObjSanbo::setBurst() {
    JGeometry::TPos3f local_d8;
    JGeometry::TVec3f TStack_e4;
    JGeometry::TVec3f local_f0;
    JGeometry::TVec3f local_fc;
    JGeometry::TVec3f local_108;

    for (int i = 0; i < 5; i++) {
        // is there a zeroing function?
        local_d8[2][3] = 0.0f;
        local_d8[1][3] = 0.0f;
        local_d8[0][3] = 0.0f;
        local_d8[2][2] = 0.0f;
        local_d8[1][2] = 0.0f;
        local_d8[0][2] = 0.0f;
        local_d8[2][1] = 0.0f;
        local_d8[1][1] = 0.0f;
        local_d8[0][1] = 0.0f;
        local_d8[2][0] = 0.0f;
        local_d8[1][0] = 0.0f;
        local_d8[0][0] = 0.0f;

        getJointPos(&TStack_e4, &local_fc, sJointNum[i]);
        local_fc.normalize();

        JMAVECScaleAdd(&local_fc, &TStack_e4, &TStack_e4, 55.0f);
        mRotMtx.getXDir(local_f0);

        local_f0.normalize();
        PSVECCrossProduct(&local_f0, &local_fc, &local_108);

        local_108.normalize();
        PSVECCrossProduct(&local_fc, &local_108, &local_f0);

        local_f0.normalize();
        local_d8.setXYZDir(local_f0, local_fc, local_108);

        _1d4[i]->burst(TStack_e4, local_d8);
    }
}

void TMapObjSanbo::getJointPos(JGeometry::TVec3f *pos, JGeometry::TVec3f *dir, long jointIdx) {
    if (_198 != false) {
        pos->set(mPos);
        if (dir != nullptr) {
            dir->set(0.0f, 1.0f, 0.0f);
        }
        return;
    }

    MtxPtr anmMtx = mModel.getModel()->getAnmMtx(jointIdx);
    pos->set(anmMtx[0][3], anmMtx[1][3], anmMtx[2][3]);

    if (dir != nullptr) {
        pos->set(anmMtx[0][0], anmMtx[1][0], anmMtx[2][0]);
        dir->normalize();
    }
}

TSanboSupervisor::TSanboSupervisor() : GeoObjSupervisor() {
    mList.initiate();
    ItemThunder::entrySuperVisor(this);
}

void TSanboSupervisor::entry(TMapObjSanbo *sanbo) {
    mList.append(&sanbo->mLinkSanbo);
}

void TSanboSupervisor::thunderDownAll() {
    for (JSULink<TMapObjSanbo> *sanbo = mList.getFirst(); sanbo != nullptr; sanbo = sanbo->getNext()) {
        TMapObjSanbo *sanboObj = sanbo->getObject();
        sanboObj->_1a0 = sanboObj->mObjData->mParam4 + sanboObj->sThunderDownBonus;
        sanboObj->setState(7);
        
        const s8 anmItem = sanboObj->sAnmTable[7];
        if (anmItem != sanboObj->mAnmPlayer.getCurAnmNumber()) {
            sanboObj->mAnmPlayer._11 = anmItem;
            sanboObj->mAnmPlayer._10 = true;
            sanboObj->mAnmPlayer._e |= 1;
        }
    }
}

void TMapObjSanboParts::reset() {
    GeographyObj::resetObject();
    setObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    PSMTXIdentity(_158);
    _188.zero();
    ResetState();
}

void TMapObjSanboParts::calc() { ExecuteState(); }

void TMapObjSanboParts::initFunc_Hide() { setObjFlagHidding(); }

void TMapObjSanboParts::doFunc_Hide() {}

void TMapObjSanboParts::initFunc_Burst() { mScale.set(_188); }

void TMapObjSanboParts::doFunc_Burst() {
    mVel.scale(sAirFric);
    mVel.y -= sGravity;
    mPos.add(mVel);
    PSMTXConcat(mRotMtx, _158, mRotMtx);
    if (acceptHidding()) {
        setState(0);
    }
}

// FIX: this function needs a bit of work to get it fully matching.
void TMapObjSanboParts::burst(const JGeometry::TVec3f &pos, const JGeometry::TPos3f &mtx) {
    mPos.set(pos);    
    mRotMtx.set(mtx);

    s16 angleH = newRnd();
    
    mVel.set(
        sBurstVelH * JMASCos(angleH),
        sBurstVelV,
        sBurstVelH * JMASSin(angleH)
    );

    // here is where we start getting into trouble.
    JGeometry::TVec3f axis;
    u16 a1 = newRnd();
    axis.x = JMASCos(a1);
    axis.y = JMASSin(newRnd());
    axis.z = JMASCos(a1);
    axis.normalize();

    _158.zeroTrans();
    _158.setRotate(axis, sRotRad);

    clrObjFlagHidding();
    setState(1);
}

void TMapObjSanboParts::InitExec() { Observer_FindAndInit(TMapObjSanboParts, 2); }

void TMapObjSanboParts::MoveExec() { Observer_FindAndExec(TMapObjSanboParts, 2); }

const f32 TMapObjSanboHead::scHeight = 120.0f;
const f32 TMapObjSanboBody::scHeight = 80.0f;
bool TMapObjSanboHead::acceptHidding() {
    return (u8)((mPos.y + TMapObjSanboHead::scHeight) < SiUtil::getCourseHeight(mPos));
}

bool TMapObjSanboBody::acceptHidding() {
    return (u8)((mPos.y + TMapObjSanboBody::scHeight) < SiUtil::getCourseHeight(mPos));
}

#include "JSystem/JAudio/JASFakeMatch2.h"
