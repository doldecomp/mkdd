#include "JSystem/JSupport/JSUList.h"
#include "Kameda/MotorManager.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/JPEffectMgr.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/Objects/MapObjHanabi.h"
#include "types.h"
#include "mathHelper.h"

THanabiSupervisor *TMapObjHanabi::sSupervisor;

TMapObjHanabi::TMapObjHanabi(const CrsData::SObject &sObject) : TMapObjHioNode(sObject), mLinkHanabi(this) {
    if (sSupervisor == nullptr) {
        sSupervisor = new THanabiSupervisor();
    }
    sSupervisor->entry(this);
    createSoundMgr();
}

void TMapObjHanabi::reset() {
    GeographyObj::resetObject();
    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _174 = false;
}

void TMapObjHanabi::calc() {
    if (!_174) {
        return;
    }

    JPEffectMgr::getEffectMgr()->createEmt("mk_hanabi_a", mPos);
    JPEffectMgr::getEffectMgr()->createEmt("mk_hanabi_b", mPos);
    JPEffectMgr::getEffectMgr()->createEmt("mk_hanabi_c", mPos);

    if (RCMGetManager()->isAwardDemoMode()) {
        mSoundMgr->setSe(0x40077);
        MotorManager::getManager()->setAwardMotor(MotorManager::MotorType_24);
    }
    _174 = false;
}


THanabiSupervisor::THanabiSupervisor() {
}

THanabiSupervisor::~THanabiSupervisor() {}

void THanabiSupervisor::entry(TMapObjHanabi *hanabi) {
    mEntryList.append(&hanabi->mLinkHanabi);
}

void THanabiSupervisor::fire(s16 fireId) {
    for (JSULink<TMapObjHanabi> *link = mEntryList.getFirst(); link != nullptr; link = link->getNext()) {
        TMapObjHanabi *hanabi = link->getObject();
        hanabi->_174 = (hanabi->mObjData->mParam1 == fireId);
    }
}

s16 THanabiSupervisor::getFireFrame(s16 fireId) {
    s16 result = -1;
    for (JSULink<TMapObjHanabi> *link = mEntryList.getFirst(); link != nullptr; link = link->getNext()) {
        if (link->getObject()->mObjData->mParam1 == fireId) {
            result = link->getObject()->mObjData->mParam2;
            break;
        }
    }
    return result;
}

#include "JSystem/JAudio/JASFakeMatch2.h"
