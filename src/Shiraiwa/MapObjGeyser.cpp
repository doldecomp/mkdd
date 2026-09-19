#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JMath/JMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/ResMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/AnmPlayer.h"
#include "Shiraiwa/Objects/MapObjGeyser.h"
#include "Shiraiwa/SiUtil.h"
#include "dolphin/mtx.h"
#include "mathHelper.h"
#include "types.h"

s16 TMapObjGeyser::sActTime = 120;
s16 TMapObjGeyser::sEfctEFEndFrame = sActTime - 25;
f32 TMapObjGeyser::sJumpUpStartVel = 100.0f;
u16 TMapObjGeyser::sCheckCycle = 30;
s16 TMapObjGeyser::sEfctABCDEndFrame = 25;
f32 TMapObjGeyser::sBoundHeight = 2900.0f;
static const char *scJointName = "ef_set";

s32 TMapObjGeyser::sJointNum;
J3DAnmTextureSRTKey *TMapObjGeyser::sGeyserBtkAnm;

const char *TMapObjGeyser::scEffectName[6] = {
    "mk_kanketsusen_a",
    "mk_kanketsusen_b",
    "mk_kanketsusen_c",
    "mk_kanketsusen_d",
    "mk_kanketsusen_e",
    "mk_kanketsusen_f"
};

TAnmInfo TMapObjGeyser::sAnmInfos[4] = {
    {"/Objects/Geyser11.bca", nullptr, nullptr, 0, 0, 1, 255, 0},
    {"/Objects/Geyser12.bca", nullptr, nullptr, 0, 0, 1, 255, 0},
    {"/Objects/Geyser13.bca", nullptr, nullptr, 0, 0, 1, 255, 0},
    {"/Objects/Geyser14.bca", nullptr, nullptr, 0, 0, 1, 255, 0},
};


StateObserver::StateFuncSet<TMapObjGeyser> TMapObjGeyser::sTable[6] = {
    {0, &TMapObjGeyser::initFunc_Start, &TMapObjGeyser::doFunc_Start},
    {1, &TMapObjGeyser::initFunc_Wait, &TMapObjGeyser::doFunc_Wait},
    {2, &TMapObjGeyser::initFunc_Act, &TMapObjGeyser::doFunc_Act},
    {3, &TMapObjGeyser::initFunc_JumpUp, &TMapObjGeyser::doFunc_JumpUp},
    {4, &TMapObjGeyser::initFunc_JumpStay, &TMapObjGeyser::doFunc_JumpStay},
    {5, &TMapObjGeyser::initFunc_JumpDown, &TMapObjGeyser::doFunc_JumpDown},
};

const s8 TMapObjGeyser::sAnmTable[6] = {
    5, 5, 0, 1, 2, 3
};

inline void TMapObjGeyser::createEmitter(int idx) {
    if (getEmitter(idx) == nullptr) {
        JPABaseEmitter *emt = JPEffectMgr::getEffectMgr()->createEmt(scEffectName[idx], mPos);

        #line 169
        JUT_ASSERT(emt != 0);

        f32 scale = (mScale.x > mScale.z) ? mScale.x : mScale.z;

        emt->setGlobalScale(scale);
        emt->stopCreateParticle();

        _1a0[idx] = emt;
    }
}

inline void TMapObjGeyser::setAnimation(int idx) {
    setState(idx);
    // HACK: sAnmTable may have the wrong type based on these casts...
    if ((u32)sAnmTable[idx] != 5 && (u32)sAnmTable[idx] != mAnmPlayer.getCurAnmNumber()) {
        mAnmPlayer._11 = sAnmTable[idx];
        mAnmPlayer._10 = 1;
        mAnmPlayer._e |= 1;
    }
}


TMapObjGeyser::TMapObjGeyser(const CrsData::SObject &sObject) : TKartThrower(sObject) {
    NewAnmCtrl();
    createSoundMgr();
    mAnmPlayer.resetAnimations(sAnmInfos, 4);
    _1b8.zero();
}

TMapObjGeyser::~TMapObjGeyser() {}

void TMapObjGeyser::InitExec() { Observer_FindAndInit(TMapObjGeyser, 6); }

void TMapObjGeyser::MoveExec() { Observer_FindAndExec(TMapObjGeyser, 6); }

const char *TMapObjGeyser::getBmdFileName() {
    static const char *cBmdName = "/Objects/Geyser1.bmd";
    return cBmdName;
}

void TMapObjGeyser::createColModel(J3DModelData *modelData) {
    f32 scaleXZ = mScale.x > mScale.z 
        ? mScale.x 
        : mScale.z;

    f32 max = scaleXZ > mScale.y 
        ? scaleXZ 
        : mScale.y;

    createBoundsCylinder(modelData, 140.0f * (scaleXZ / max),
        sBoundHeight * (mScale.y / max)
    );
}

void TMapObjGeyser::createModel(JKRSolidHeap *jkrSolidHeap, u32 p2) {
    mModel.createDifferedModel(jkrSolidHeap, p2, 0x200, 0);
    mAnmPlayer.registAnimations(mAnmCtrl, &mModel, sAnmInfos, 4);

    sJointNum = mModel.getModelData()->getJointName()->getIndex(scJointName);

    mAnmObjMaterial.setExModel(&mModel);
    mAnmObjMaterial.setAnmBase(sGeyserBtkAnm);
    mAnmObjMaterial.initFrameCtrl(mAnmObjMaterial.getAnmBase());
}

void TMapObjGeyser::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    TAnmPlayer::loadAnimations(sAnmInfos, 4, modelData, ResMgr::mcArcCourse);
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/Geyser1.btk");
    J3DAnmObjMaterial::setupTexSRTAnmData(&sGeyserBtkAnm, modelData, ptrCourseArc);
}

void TMapObjGeyser::playEmitter(u32 id, const JGeometry::TVec3f &pos) {
    bool valid = true;

    switch (id) {
        case 1:
        case 3:
        case 5:
            valid = false;
            break;
    }

    if (!valid) {
        return;
    }

    #line 165
    JUT_MINMAX_ASSERT(0, id, 6);

    if (getEmitter(id) == nullptr) {
        valid = true;

        switch (id) {
            case 1:
            case 3:
            case 5:
                valid = false;
                break;
        }

        if (valid) {
            JPABaseEmitter* emt = JPEffectMgr::getEffectMgr()->createEmt(scEffectName[id], mPos);
    
            #line 169
            JUT_ASSERT(emt != 0);
    
            f32 scale = (mScale.x > mScale.z) ? mScale.x : mScale.z;
    
            emt->setGlobalScale(scale);
            emt->stopCreateParticle();
    
            _1a0[id] = emt;
        }
    }

    getEmitter(id)->playCreateParticle();
    moveEmitter(id, pos);
}

void TMapObjGeyser::moveEmitter(u32 id, const JGeometry::TVec3f &pos) {
    bool ok = true;

    switch (id) {
        case 1:
        case 3:
        case 5:
            ok = false;
            break;
    }

    if (!ok) {
        return;
    }

    #line 165
    JUT_MINMAX_ASSERT(0, id, 6);

    // Create emitter if missing
    if (getEmitter(id) == nullptr) {
        bool ok = true;

        switch (id) {
            case 1:
            case 3:
            case 5:
                ok = false;
                break;
        }

        if (ok) {
            JPABaseEmitter *emt = JPEffectMgr::getEffectMgr()->createEmt(scEffectName[id], mPos);

            #line 169
            JUT_ASSERT(emt != 0);

            f32 scale = (mScale.x > mScale.z) ? mScale.x : mScale.z;

            emt->setGlobalScale(scale);
            emt->stopCreateParticle();

            _1a0[id] = emt;
        }
    }

    getEmitter(id)->setGlobalTranslation(pos);
}

void TMapObjGeyser::update() {
    mAnmObjMaterial.anmFrameProc();
    GeographyObj::update();
}

void TMapObjGeyser::reset() {
    GeographyObj::resetObject();
    _58 = 0;
    mAnmPlayer.init(mAnmCtrl, sAnmInfos, 4);
    mAnmObjMaterial.getFrameCtrl()->reset();

    _190 = mObjData->mParam1;
    if (_190 == 0) {
        _190 = 300;
    }

    _194 = mObjData->mParam4 * 182.04445f;
    _198 = mScale.y * 1000.0f;
    _19c = 0;

    ResetState();

    if (RaceMgr::getManager()->getRaceMode() != 1 && mObjData->mParam8 != 0) {
        setState(1);
    } else {
        setState(0);
    }
    
    for (int i = 0; i != 6; i++) {
        _1a0[i] = 0;
    }

    _1c4 = -sBoundHeight;
    _1b8.zero();
}

void TMapObjGeyser::calc() {
    MtxPtr anmMtx = mModel.getModel()->getAnmMtx(sJointNum);
    _1b8.set(anmMtx[0][3], anmMtx[1][3], anmMtx[2][3]);
    
    ExecuteState();
    mAnmPlayer.update();
    mAnmObjMaterial.getFrameCtrl()->update();
    JGeometry::TVec3f local_18(0.0f, _1c4 + (_1b8.y - mPos.y) / mScale.y, 0.0f);
    GeographyObj::setColObjPos(local_18, 0);
}

void TMapObjGeyser::viewCalc(u32 viewNo) {
    mModel.viewCalc(viewNo);
}

void TMapObjGeyser::getThrowDir(JGeometry::TVec3f *throwDir, s16 kartIdx) {
    JGeometry::TVec3f tempThrowDir;
    
    if (kartIdx == -1) {
        tempThrowDir.set(0.0f, 1.0f, 0.0f);
    } else {
        ObjUtility::getKartVel(kartIdx, &tempThrowDir);
        tempThrowDir.y = 0.0f;
        if (tempThrowDir.isZero()) {
            ObjUtility::getKartZdir(kartIdx, &tempThrowDir);
            tempThrowDir.y = 0.0f;
            if (tempThrowDir.isZero()) {
                tempThrowDir.set(0.0f, 0.0f, 1.0f);
            }
        }
        tempThrowDir.normalize();

        tempThrowDir.scale(JMASCos(_194));
        tempThrowDir.y = JMASSin(_194);
    }
    throwDir->set(tempThrowDir);
}

s32 TMapObjGeyser::getThrowPow() { return mObjData->mParam3; }

void TMapObjGeyser::getKartThrowDirPow(JGeometry::TVec3f *throwDir, f32 *throwPower, int kartIdx) {
    if (throwDir != nullptr) {
        getThrowDir(throwDir, kartIdx);
    }
    if (throwPower != nullptr) {
        *throwPower = getThrowPow();
    }
}

void TMapObjGeyser::getItemThrowDirPow(JGeometry::TVec3f* dir, f32* power, const ItemObj& item) {
    JGeometry::TVec3f itemPos;
    itemPos.set(item.mPos);

    JGeometry::TVec3f horiz;
    horiz.sub(itemPos, mPos);
    horiz.y = 0.0f;
    horiz.normalize();

    horiz.scale(JMASCos(_194), horiz);
    horiz.y = JMASSin(_194);

    dir->set(horiz);
    *power = mObjData->mParam3 * 0.1f;
}

void TMapObjGeyser::initFunc_Start() {
    _58 = 0;
    mReaction.setFlg(0, 0);
    mReaction.setFlg(6, 0);
    mReaction.setFlg(5, 0);
    mReaction.setFlg(3, 0);
}

void TMapObjGeyser::doFunc_Start() {
    if (getStateCount() > mObjData->mParam2) {
        setAnimation(1);
    }
}

void TMapObjGeyser::initFunc_Wait() {
    _58 = 0;
    mReaction.setFlg(0, 0);
    mReaction.setFlg(6, 0);
    mReaction.setFlg(5, 0);
    mReaction.setFlg(3, 0);
}

void TMapObjGeyser::doFunc_Wait() {
    // FIX: What inline function/datatype actually exists at 0xa4?
    if (checkChangeAct() != 0) {
        setAnimation(2);
    }
}

bool TMapObjGeyser::checkChangeAct() {
    bool changeAct;
    
    if (RaceMgr::getManager()->getRaceMode() != 1 && mObjData->mParam8 != 0) {
        _19c++;
        if (_19c < sCheckCycle) {
            changeAct = false;
        } else {
            _19c = 0;
            if (SiUtil::searchNearKartCircle(mPos, mObjData->mParam5) != -1) {
                changeAct = true;
            } else {
                changeAct = false;
            }
        }
    } else {
        changeAct = getStateCount() > _190;
    }
    return changeAct;
}

void TMapObjGeyser::initFunc_Act() {
    playEmitter(4, mPos);
    playEmitter(5, mPos);
}

void TMapObjGeyser::doFunc_Act() {
    if (getStateCount() > sActTime) {
        setAnimation(3);
    }
    if (sEfctEFEndFrame == getStateCount()) {
        createEmitter(4);
        stopEmitter(4);
    }
}

void TMapObjGeyser::initFunc_JumpUp() {
    _58 = 0xe;
    mReaction.setFlg(0, 3);
    mReaction.setFlg(6, 3);
    mReaction.setFlg(5, 3);
    mReaction.setFlg(3, 3);
    mVel.set(0.0f, sJumpUpStartVel * mScale.y, 0.0f);
    playEmitter(0, _1b8);
    playEmitter(1, _1b8);
    playEmitter(2, mPos);
    playEmitter(3, mPos);
}

void TMapObjGeyser::doFunc_JumpUp() {
    u8 currentAnmNo = mAnmPlayer.getCurAnmNumber();
    if (mAnmPlayer.mController->getFrameCtrl(currentAnmNo)->checkState(1)) {
        setAnimation(4);
    }
    getSoundMgr()->setSe(0x40047);
    createEmitter(0);
    getEmitter(0)->setGlobalTranslation(_1b8);
}

void TMapObjGeyser::initFunc_JumpStay() {}

void TMapObjGeyser::doFunc_JumpStay() {
    u8 currentAnmNo = mAnmPlayer.getCurAnmNumber();
    if (mAnmPlayer.mController->getFrameCtrl(currentAnmNo)->checkState(1)) {
        setAnimation(5);
    }
    getSoundMgr()->setSe(0x40047);
    createEmitter(0);
    getEmitter(0)->setGlobalTranslation(_1b8);
}

void TMapObjGeyser::initFunc_JumpDown() { mVel.zero(); }



void TMapObjGeyser::doFunc_JumpDown() {
    u8 currentAnmNo = mAnmPlayer.getCurAnmNumber();
    if (mAnmPlayer.mController->getFrameCtrl(currentAnmNo)->checkState(1)) {
        setAnimation(1);
    }
    getSoundMgr()->setSe(0x40047);
    createEmitter(0);
    getEmitter(0)->setGlobalTranslation(_1b8);

    if (sEfctABCDEndFrame == getStateCount()) {
        createEmitter(0);
        stopEmitter(0);
        createEmitter(2);
        stopEmitter(2);
    }
}

void TMapObjGeyserSearch::reset() {
    TMapObjGeyser::reset();
    setState(1);
    _19c = 0;
}

u16 TMapObjGeyserSearch::sCheckCycle = 30;
bool TMapObjGeyserSearch::checkChangeAct() {
    _19c++;
    if (_19c < TMapObjGeyserSearch::sCheckCycle) {
        return false;
    }
    _19c = 0;
    f32 radius = mObjData->mParam1;
    s16 kartNo = SiUtil::searchNearKartCircle(mPos, radius);
    return kartNo != -1;
}

#include "JSystem/JAudio/JASFakeMatch2.h"
