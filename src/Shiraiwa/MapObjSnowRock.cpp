#include "Shiraiwa/Objects/MapObjSnowRock.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/ItemObj.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"
#include "types.h"
#include "mathHelper.h"

StateObserver::StateFuncSet<TMapObjSnowRock> TMapObjSnowRock::sTable[3] = {
    0, &TMapObjSnowRock::initFunc_Break, &TMapObjSnowRock::doFunc_Break,
    1, &TMapObjSnowRock::initFunc_Die, &TMapObjSnowRock::doFunc_Die,
    2, &TMapObjSnowRock::initFunc_Stand, &TMapObjSnowRock::doFunc_Stand
};

f32 TMapObjSnowRock::sGravity = 0.01f;
f32 TMapObjSnowRock::sStartVel = 70.0f;
TSnowRockSupervisor *TMapObjSnowRock::sSupervisor;

void TSnowRockSupervisor::calc() {
    for (JSULink<TMapObjSnowRock> *linkA = _24.getFirst(); linkA != nullptr; linkA = linkA->getNext()) {
        for (JSULink<TMapObjSnowRock> *linkB = linkA; linkB != nullptr; linkB = linkB->getNext()) {
            if (linkA != linkB) {
                linkA->getObject()->hitCheck(linkB->getObject());
            }
        }
    }
}

TMapObjSnowRock::TMapObjSnowRock(const CrsData::SObject &sObject) : TMapObjHioNode(sObject), JKRDisposer(), _170(this) {
    setObjFlagSimpleDraw();
    createSoundMgr();
    _180 = new CrsGround(RCMGetCourse());
    if (getSupervisor() == nullptr) {
        sSupervisor = new TSnowRockSupervisor();
    }
    sSupervisor->_24.append(&_170);
};

TMapObjSnowRock::~TMapObjSnowRock() {
    delete _180;
    sSupervisor = nullptr;
}

void TMapObjSnowRock::InitExec() {
    Observer_FindAndInit(TMapObjSnowRock, 3);
}

void TMapObjSnowRock::MoveExec() {
    Observer_FindAndExec(TMapObjSnowRock, 3);
}

const char *TMapObjSnowRock::getBmdFileName() {
    static const char *cBmdName = "/Objects/SnowRock1.bmd";
    return cBmdName;
}

void TMapObjSnowRock::createColModel(J3DModelData *modelData) {
    f32 scaleXZ = mScale.x > mScale.z 
        ? mScale.x 
        : mScale.z;

    f32 max = scaleXZ > mScale.y 
        ? scaleXZ 
        : mScale.y;

    createBoundsCylinder(modelData, 500.0f * (scaleXZ / max),
        950.0f * (mScale.y / max)
    );
}

void TMapObjSnowRock::doKartColCallBack(int kartIdx) {
    JGeometry::TVec3f kartPos;
    JGeometry::TVec3f kartVel;
    JGeometry::TVec3f colVel;
    
    mSoundMgr->setSe(0x1005b);
    if ((GetKartCtrl()->getKartBody(kartIdx)->mCarStatus & 0x40000) != 0) {
        setState(1);
    } else {
        static const JGeometry::TVec3f axisY(0.0f, 1.0f, 0.0f);
        ObjUtility::getKartPos(kartIdx, &kartPos);
        setAttacked(kartPos, getBounds(0)->getBoundDepth());
        ObjUtility::getKartVel(kartIdx, &kartVel);
        kartVel.normalize();
        kartPos.set(mVel);
        if (kartPos.dot(kartVel) > 0.8f) {
            colVel.scaleAdd(-kartVel.dot(kartPos), kartVel, kartPos);
            if (colVel.isZero()) {
                colVel.cross(kartVel, axisY);
                colVel.normalize();
            }
            colVel.y = 0.0f;
            colVel.setLength(30.0f);
            mVel.add(colVel);
        }
    }
}

void TMapObjSnowRock::setAttacked(const JGeometry::TVec3f &objPos, f32 param_2) {
    JGeometry::TVec3f local_28;
    
    if (RCMGetManager()->getRaceMode() != TIME_ATTACK) {
        local_28.sub(mPos, objPos);
        local_28.normalize();
        local_28.scale(sStartVel * SiUtil::getNormalRange(param_2, 0.0f, 30.0f));
        mVel.add(local_28);
    }
}

void TMapObjSnowRock::reset() {
    resetObject();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    mReaction.setFlg(8, 0);
    mReaction.setFlg(7, 0);
    _58 = 19;
    _184 = mObjData->mParam1 * 0.001f;
    if (_184 == 0.0f) {
        _184 = 0.99f;
    }
    ResetState();
}

void TMapObjSnowRock::calc() {
    ExecuteState();
}

void TMapObjSnowRock::initFunc_Stand() {
    mVel.zero();
}

void TMapObjSnowRock::doFunc_Stand() {
    JGeometry::TVec3f newPos;
    newPos.set(mPos);
    mVel.scale(_184);
    mVel.y = mVel.y - sGravity;
    mPos.add(mVel);
    _180->search(mPos, newPos);
    f32 crsHeight = _180->getHeight();
    if (mPos.y < crsHeight) {
        mPos.y = crsHeight;
        mVel.y = 0.0f;
    }
    if (_180->getAttribute() != 4) {
        setState(1);
    }
    checkItemHitting();
    if (!mVel.isZero()) {
        mSoundMgr->setSeWithVelocityV(0x40031, SiUtil::getNormalRange(mVel.length(), 0.0f, 30.0f), 0.3f);
    }
}

void TMapObjSnowRock::checkItemHitting() {
    if (!tstItemHitting()) {
        return;
    }
    JGeometry::TVec3f pos;
    switch (getColItemObj()->getKind()) {
        case 7: {
            pos = getColItemObj()->getPos();
            f32 depth = getBounds(0)->getBoundDepth();
            setAttacked(pos, depth);
            break;
        }
        case 1: {
            pos = getColItemObj()->getPos();
            setAttacked(pos, getColItemObj()->getVel().length());
            break;
        }
        case 8:
            setState(1);
            break;
    }
}

void TMapObjSnowRock::initFunc_Break() {
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
    makeBreakEmitter();
    mSoundMgr->setSe(0x40030);
    _58 = 0;
}

void TMapObjSnowRock::makeBreakEmitter() {
    JPABaseEmitter *baseEmitter;
    baseEmitter = JPEffectMgr::getEffectMgr()->createEmt("mk_icerockCrash_a", mPos);
    baseEmitter->setGlobalScale(mScale);
    baseEmitter = JPEffectMgr::getEffectMgr()->createEmt("mk_icerockCrash_b", mPos);
    baseEmitter->setGlobalScale(mScale);
    baseEmitter = JPEffectMgr::getEffectMgr()->createEmt("mk_icerockCrash_c", mPos);
    baseEmitter->setGlobalScale(mScale);
}

void TMapObjSnowRock::doFunc_Break() {
    setState(2);
}

void TMapObjSnowRock::initFunc_Die() {
    setObjFlagHidding();
}

void TMapObjSnowRock::doFunc_Die() {}

// FIX: Register mismatch.
void TMapObjSnowRock::hitCheck(TMapObjSnowRock *objSnowRock) {
    JGeometry::TVec3f rockPos = objSnowRock->mPos;
    if (getBounds(0)->IsHitCylinder(mPos, rockPos, *(ObjColCylinder *)objSnowRock->getBounds(0))) {
        JGeometry::TVec3f diff;
        diff.sub(mPos, rockPos);
        diff.normalize();
        diff.scale(sStartVel * SiUtil::getNormalRange(getBounds(0)->getBoundDepth(), 0.0f, 30.0f));
        mVel.add(diff);
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
