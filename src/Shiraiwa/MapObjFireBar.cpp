#include "Shiraiwa/Objects/MapObjFireBar.h"
#include "Inagaki/GameAudioMain.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JMath/JMath.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/AnmController.h"
#include "Sato/GeographyObj.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjFireObj.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "math.h"
#include "types.h"

TMapObjFireBar::TMapObjFireBar(const CrsData::SObject &sObject, u32 id) : TMapObjFireObj(sObject, id) {
    _18c = 0;
    
    int objDataParam4 = mObjData->mParam4;
    if (objDataParam4 == 0) {
        objDataParam4 = 2;
    }

    for (int i = 0; i < fireBallCount; i++) {
        if (i + objDataParam4 >= fireBallCount) {
            fireBalls[i]->createSoundMgr();
        }
    }
}

TMapObjFireBar::~TMapObjFireBar() {}

void TMapObjFireBar::reset() {
    GeographyObj::resetObject();
    setObjFlagCheckGeoHitting();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = 1;
    StateObserver::ResetState();

    _190 = mObjData->mParam2 * 0.001f;
    if (_190 == 0.0f) {
        _190 = 0.03f;
    }

    _18c = mObjData->mParam3;
    if (_18c == 0) {
        _18c = 200;
    }

    _188 = mObjData->mParam4;
    if (_188 == 0) {
        _188 = 2;
    }

    _180 = 0.0f;
    resetFire();
}

void TMapObjFireBar::resetFire() {
    int ring = 0;
    JGeometry::TVec3f axis;
    JGeometry::TVec3f zAxis;

    for (int i = 0; i < fireBallCount; i++) {
        int segment = i % _188;

        if (segment == 0) {
            ring++;
        }

        int angle = (segment << 16) / _188;
        int radius = _18c * ring;

        axis.set(
            JMASCos(angle) * radius,
            JMASSin(angle) * radius,
            0.0f
        );
        zAxis.set(axis);

        fireBalls[i]->_178.set(axis);

        fireBalls[i]->setRMtx(mRotMtx);
        fireBalls[i]->_184.set(mPos);
        fireBalls[i]->mScale.set(mScale);

        fireBalls[i]->_171 = (i + _188 >= fireBallCount);

        if (mObjData->mParam5 == 0 && (fireBallCount - i) > _188) {
            TMapObjFireBallBase *ball = fireBalls[i];
            ball->clrAllCheckKartHitFlag();
            ball->clrObjFlagCheckItemHitting();
        }
    }

    if (_190 != 0.0f) {
        zAxis.set(0.0f, 0.0f, 1.0f);
        _160.setRotate(zAxis, _190);
        _170.setRotate(zAxis, _190 * 3.0f);
    } else {
        _160.set(0.0f, 0.0f, 0.0f, 1.0f);
        _170.set(0.0f, 0.0f, 0.0f, 1.0f);
    }
}



void TMapObjFireBar::createColModel(J3DModelData *modelData) {
    createBoundsSphere(modelData);
}

const char *TMapObjFireBar::getBmdFileName() {
    static const char *cBmdName = "/Objects/FireBar1.bmd";
    return cBmdName;
}

const char *TMapObjFireBar::getShadowBmdFileName() { return NULL; }

void TMapObjFireBar::calc() {
    TMapObjFireObj::calc();
}

J3DAnmTransform *TMapObjKpFireBar::sKpFireBarBckAnmTrans;
J3DMtxCalc *TMapObjKpFireBar::sKpFireBarBckMtxCalc;
bool TMapObjKpFireBar::sUseBck = false;

TMapObjKpFireBar::TMapObjKpFireBar(const CrsData::SObject &sObject) : TMapObjFireBar(sObject, 0x1263) {
    NewAnmCtrl();
}

void TMapObjKpFireBar::reset() {
    JGeometry::TPos3f rot;
    resetObject();
    setObjFlagCheckGeoHitting();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = 1;
    ResetState();

    _190 = mObjData->mParam2 * 0.001f;
    if (_190 == 0.0f) {
        _190 = 0.03f;
    }

    _18c = mObjData->mParam3;
    if (_18c == 0) {
        _18c = 200;
    }

    _188 = mObjData->mParam4;
    if (_188 == 0) {
        _188 = 2;
    }

    _180 = 0.0f;

    resetFire();
    mScale.set(1.0f, 1.0f, 1.0f);

    if (sUseBck) {
        getAnmCtrl()->Reset();
    }

    // Does this exist as a function elsewhere?
    f32 s = sinf(-F_HALF_PI);
    f32 c = cosf(-F_HALF_PI);
    rot[0][0] = 1.0f;
    rot[2][1] = s;
    rot[1][1] = c;
    rot[1][2] = -s;
    rot[2][2] = c;
    rot[2][0] = 0.0f;
    rot[0][2] = 0.0f;
    rot[1][0] = 0.0f;
    rot[0][1] = 0.0f;
    mRotMtx.zeroTrans();
    PSMTXConcat(mRotMtx, rot, mRotMtx);

    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
}

void TMapObjKpFireBar::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/KpFireBar1.bck");
    if (ptrCourseArc != nullptr) {
        sUseBck = true;
        J3DAnmObjTrans::setupTransAnmData(&sKpFireBarBckAnmTrans, &sKpFireBarBckMtxCalc, modelData, ptrCourseArc);
    } else {
        sUseBck = false;
    }
}

void TMapObjKpFireBar::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);
    if (sUseBck != false) {
        AnmController *anmCtrl = getAnmCtrl();
        AnmControlTrans *anmCtrlTrans = new AnmControlTrans();
        anmCtrl->mTrans = anmCtrlTrans;
        anmCtrlTrans = anmCtrl->mTrans;
        anmCtrlTrans->initAnm(1, &mModel);
        getAnmCtrl()->RegisterTrans(0, sKpFireBarBckAnmTrans, sKpFireBarBckMtxCalc);
    }
}

void TMapObjKpFireBar::createColModel(J3DModelData *) {}



TMapObjFireCircle::TMapObjFireCircle(const CrsData::SObject &sObject) : TMapObjFireObj(sObject, 0x106a) {
    _188 = 0.0f;
    createSoundMgr();
}

TMapObjFireCircle::~TMapObjFireCircle() {}

void TMapObjFireCircle::reset() {
    GeographyObj::resetObject();
    StateObserver::ResetState();
    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _58 = 0;

    _190 = mObjData->mParam2 * 0.001f;
    if (_190 == 0.0f) {
        _190 = 0.001f;
    }

    if (mObjData->mParam3 == 10 || mObjData->mParam3 == 0) {
        _194 = 1.0f;
    } else {
        _194 = mObjData->mParam3 * 0.1f;
    }

    _188 = mObjData-> mParam4;
    if (_188 == 0.0f) {
        _188 = 400.0f;
    }

    _18c = 0;
    _18e = 0x111;
    _180 = 0.0f;
    resetFire();
}

void TMapObjFireCircle::resetFire() {
    s16 step = 0x10000 / fireBallCount;
    s16 angle = 0;
    
    for (s16 i = 0; i < fireBallCount; i++) {
        fireBalls[i]->_178.set(
            _188 * JMASCos(angle),
            _188 * JMASSin(angle),
            0.0f
        );
        fireBalls[i]->_184.set(mPos);
        fireBalls[i]->setRMtx(mRotMtx);

        angle += step;

        TMapObjFireBallBase *fbb = fireBalls[i];
        fbb->mScale.set(mScale);

        TMapObjFireBallBase *fbb2 = fireBalls[i];
        fbb2->clrAllCheckKartHitFlag();
        fbb2->clrObjFlagCheckItemHitting();
    }

    if (_190 != 0.0f) {
        const static JGeometry::TVec3f zAxis(0.0f, 0.0f, 1.0f);
        _160.setRotate(zAxis, _190);
        _170.setRotate(zAxis, _190 * 3.0f);
    } else {
        _160.set(0.0f, 0.0f, 0.0f, 1.0f);
        _170.set(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void TMapObjFireCircle::calc() {
    u8 uVar7 = 0;
    f32 fVar1 = _188 * _194 + 1000.0f;
    f32 dVar9 = SQUARE(fVar1);
    for (int kartId = 0; kartId < RaceMgr::getManager()->getKartNumber(); kartId++) {
        JGeometry::TVec3f kartPos;
        ObjUtility::getKartPos(kartId, &kartPos);

        kartPos.sub(mPos);
        if (kartPos.squared() < dVar9) {
            uVar7 |= 1 << kartId;
        }
    }

    for (int i = 0; i < fireBallCount; i++) {
        for (int kartId = 0; kartId < RCMGetManager()->getKartNumber(); kartId++) {
            if ((uVar7 & (1 << kartId)) != 0) {
                fireBalls[i]->setCheckKartHitFlag(kartId);
            } else {
                fireBalls[i]->clrCheckKartHitFlag(kartId);
            }
        }
    }

    TMapObjFireObj::calc();
    if (_194 != 1.0f) {
        f32 fVar2 = _194;
        f32 c = JMASCos(_18c);

        for (int i = 0; i < fireBallCount; i++) {
            fireBalls[i]->_174 = (1.0f + (fVar2 - 1.0f) * c + fVar2) * 0.5f;
        }
        _18c += _18e;
        getSoundMgr()->setSe(0x40032);
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
