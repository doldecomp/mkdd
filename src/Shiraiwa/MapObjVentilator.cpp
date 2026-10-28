#include "JSystem/JGeometry/Quat.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "dolphin/mtx.h"
#include "Shiraiwa/Objects/MapObjVentilator.h"


StateObserver::StateFuncSet<TMapObjVentilator> TMapObjVentilator::sTable[4] = {
    {0, &TMapObjVentilator::initFunc_Slow, &TMapObjVentilator::doFunc_Slow},
    {1, &TMapObjVentilator::initFunc_Slow2Fast, &TMapObjVentilator::doFunc_Slow2Fast},
    {2, &TMapObjVentilator::initFunc_Fast, &TMapObjVentilator::doFunc_Fast},
    {3, &TMapObjVentilator::initFunc_Fast2Slow, &TMapObjVentilator::doFunc_Fast2Slow},
};

TMapObjVentilator::TMapObjVentilator(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    setObjFlagSimpleDraw();
    createSoundMgr();
}

TMapObjVentilator::~TMapObjVentilator() {}

const char *TMapObjVentilator::getBmdFileName() {
    static const char *cBmdName = "/Objects/Fan1.bmd";
    return cBmdName;
}

void TMapObjVentilator::InitExec() { Observer_FindAndInit(TMapObjVentilator, 4); }

void TMapObjVentilator::MoveExec() { Observer_FindAndExec(TMapObjVentilator, 4); }

void TMapObjVentilator::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();

    _158.set(0.0f, 0.0f, 0.0f, 1.0f);

    mRotMtx.zeroTrans();
    mRotMtx.setQuat(_158);

    _168.set(0.0f, 0.0f, 0.0f, 1.0f);
    f32 s = sinf(0.05f);
    f32 c = cosf(0.05f);

    _168.x = s;
    _168.y = 0.0f;
    _168.z = 0.0f;
    _168.w = c;

    _178.set(0.0f, 0.0f, 0.0f, 1.0f);
    s = sinf(0.015f);
    c = cosf(0.015f);

    _178.x = s;
    _178.y = 0.0f;
    _178.z = 0.0f;
    _178.w = c;
}

void TMapObjVentilator::calc() {
    ExecuteState();

    f32 velocity = 0.3f;
    switch (getState()) {
        case 0:
            velocity = 0.3f;
            break;

        case 2:
            velocity = 1.0f;
            break;
            
        case 3:
            velocity = 0.7f * (1.0f - _188);
            break;
            
        case 1:
            velocity = 0.7f * _188;
            break;
    }

    mSoundMgr->setSeWithVelocityP(0x4007F, velocity);
}

void TMapObjVentilator::doRotate(const JGeometry::TQuat4f &rQuat) {
    PSQUATMultiply(&_158, &rQuat, &_158);
    _158.normalize(_158);
    mRotMtx.setQuat(_158);
}

void TMapObjVentilator::initFunc_Slow() {}

void TMapObjVentilator::doFunc_Slow() {
    doRotate(_178);
    if (getStateCount() > 300) {
        setState(1);
    }
}

void TMapObjVentilator::initFunc_Slow2Fast() {
    _188 = 0.0f;
}

void TMapObjVentilator::doFunc_Slow2Fast() {
    JGeometry::TQuat4f rQuat;    
    rQuat.x = _188 * (_168.x - _178.x) + _178.x;
    rQuat.y = _188 * (_168.y - _178.y) + _178.y;
    rQuat.z = _188 * (_168.z - _178.z) + _178.z;
    rQuat.w = _188 * (_168.w - _178.w) + _178.w;
    
    rQuat.normalize(rQuat);
    doRotate(rQuat);

    _188 += 0.01f;
    if (_188 >= 1.0f) {
        setState(2);
    }
}

void TMapObjVentilator::initFunc_Fast() {}

void TMapObjVentilator::doFunc_Fast() {
    doRotate(_168);
    if (getStateCount() > 900) {
        setState(3);
    }
}

void TMapObjVentilator::initFunc_Fast2Slow() {
    _188 = 0.0f;
}

void TMapObjVentilator::doFunc_Fast2Slow() {
    JGeometry::TQuat4f rQuat;    
    rQuat.x = _188 * (_178.x - _168.x) + _168.x;
    rQuat.y = _188 * (_178.y - _168.y) + _168.y;
    rQuat.z = _188 * (_178.z - _168.z) + _168.z;
    rQuat.w = _188 * (_178.w - _168.w) + _168.w;
    
    rQuat.normalize(rQuat);
    doRotate(rQuat);
    
    _188 += 0.01f;
    if (_188 >= 1.0f) {
        setState(0);
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
