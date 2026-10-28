#include "Yamamoto/KartDossin.h"

#include "JSystem/JAudio/JASFakeMatch2.h"
#include "Yamamoto/KartDossin.h"
#include "JSystem/JGeometry/Vec.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Yamamoto/KartSound.h"
#include "Yamamoto/kartBody.h"
#include "Yamamoto/kartCtrl.h"
#include <std/math.h>

#include "types.h"

void KartDossin::Init(int kartBodyIndex) {
    mBody = GetKartCtrl()->getKartBody(kartBodyIndex);
    _14 = 0;
    mKartFlattenedTimer = 0;
    _15 = 0;
    _16 = 0;
    _1c = 1.0f;
    _20 = 0.0f;
    mVelocity.zero();
}

void KartDossin::DoDossinTimer() {
    if (mKartFlattenedTimer == 0) {
        return;
    }

    mKartFlattenedTimer--;
}

void KartDossin::MakeDossin(GeographyObj *geographyObj) {
    KartBody *kartBody = mBody;
    kartBody->getGame()->MakeClear();
    kartBody->getThunder()->ClearThunder();
    kartBody->getItem()->FallItem();

    kartBody->mCarStatus = 0;
    kartBody->_584 = 0xe;

    kartBody->getDossin()->_15 = 0;
    kartBody->getDossin()->_16 = 0;

    kartBody->mCarStatus |= 0x200000000uLL; // 0x570
    kartBody->mCarStatus &= ~0x80B02000uLL; // 0x574
    kartBody->getDamage()->ClrAllDamage();

    if (GetKartCtrl()->getKartAnime(kartBody->mMynum)->IsDamage(kartBody->mMynum) == true) {
        GetKartCtrl()->getKartAnime(kartBody->mMynum)->mFlags |= 0x40000;
    }

    if (GetKartCtrl()->getKartAnime(kartBody->mMynum)->IsFloatAnime(kartBody->mMynum) != false) {
        GetKartCtrl()->getKartAnime(kartBody->mMynum)->mFlags |= 0x1000000;
    }

    if (GetKartCtrl()->getKartAnime(kartBody->mMynum)->IsBurnAnime(kartBody->mMynum) != false) {
        GetKartCtrl()->getKartAnime(kartBody->mMynum)->mFlags |= 0x80000000;
    }

    kartBody->getDamage()->SetDriverEndDamageAnime();
    kartBody->getDossin()->DoKeep();
    kartBody->getDossin()->_14 = 0;
    _14 |= 1;

    kartBody->mCarStatus |= 0x80000000;
    GetKartCtrl()->getKartSound(kartBody->mMynum)->DoAllVoice(0xc);
    mGeoObj = geographyObj;
}

bool KartDossin::DoDossin() {
    KartBody* kartBody = mBody;

    GeographyObj *geographyObj = GetGeoObjMgr()->getKartReactHitObjectList(kartBody->mMynum)[10];
    if (geographyObj == nullptr) {
        return false;
    }

    JGeometry::TVec3f velocityVec;
    JGeometry::TVec3f normalVec;
    JGeometry::TVec3f positionVec;

    geographyObj->getVelocity(&mVelocity);

    normalVec.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(kartBody->mMynum));

    geographyObj->getPosition(&positionVec);
    positionVec.y = kartBody->mPos.y;

    velocityVec.sub(positionVec, kartBody->mPos);
    f32 distance = velocityVec.length();

    f32 threshold = geographyObj->getColScaleRadius() * 0.65f;

    if (kartBody->mCarStatus & 0x40000) {
        mVelocity.zero();
        kartBody->ObjectReflection(&normalVec);
        kartBody->StarReact(geographyObj);

    } else if (kartBody->mCarStatus & 0x200000000uLL) {
        mVelocity.zero();
        if (distance > threshold) {
            kartBody->ObjectReflection(&normalVec);
        }
        
    } else {
        mVelocity.zero();

        if (distance < threshold) {
            MakeDossin(geographyObj);
        } else {
            f32 speed = GetKartCtrl()->GetCarSpeed(kartBody->mMynum);
            if (speed > 80.0f && mKartFlattenedTimer == 0) {
                kartBody->getCrash()->MakeHalfSpin(0);
            }
            kartBody->ObjectReflection(&normalVec);
        }
    }

    if (kartBody->mCarStatus & 0x01000000) {
        mKartFlattenedTimer = 180; // 0xB4
    }

    return true;
}

void KartDossin::DoKeep() {
    KartBody *kartBody = mBody;
    
    kartBody->mVel.zero();
    kartBody->_2cc.zero();
    kartBody->mWg.zero();
    kartBody->_2c0.zero();
    kartBody->_2d8.zero();

    kartBody->_4d8 = 0.0f;
    kartBody->_4d4 = 0.0f;
    kartBody->_4c4 = 0.0f;
    kartBody->_4c0 = 0.0f;
    kartBody->_3c8 = 0.0f;
    kartBody->_3cc = 0.0f;
    kartBody->mTireAngle = 0.0f;
    kartBody->_394 = 0.0f;
    kartBody->mLiftframe = 0.0f;

    for (int i = 0; i < 4; i++) {
        KartSus *kartSus = GetKartCtrl()->getKartSus(i + kartBody->mMynum * 4);
        kartSus->mWheelRPM = 0.0f;
        kartSus->mTireDispAngle = 0.0f;
        kartSus->mRPM = 0.0f;
    }
    _1c = 0.3f;
}

void KartDossin::DoClear() {
    KartBody *kartBody = mBody;
    
    _14 &= ~1;
    _14 |= 2;

    GetKartCtrl()->getKartSound(kartBody->mMynum)->DoKartsetSeSound(0x1007b);

    mVelocity.zero();
    _20 = 150.0f;
    
    kartBody->_584 = 0;
    return;
}

void KartDossin::DoCheck() {
    KartBody *kartBody = mBody;

    mVelocity.zero();
    if (kartBody->getTouchNum() != 0) {
        _14 &= ~0x2;
        _14 &= ~0x20;
    }
}

bool KartDossin::DoReturn() {
    _24 = 0.7f;
    _28 = 0.0f;
    _1c = GetKartCtrl()->fcnvge(_1c, 1.3f, 0.5f, 0.5f);
    return (u8)(_1c >= 1.3f);
}

// This is the popping up animation from AFTER the player gets squashed.
bool KartDossin::DoShake() {
    _24 = GetKartCtrl()->fcnvge(_24, 0.0f, 0.04f, 0.04f);
    _28 += 0.397;
    _1c = _24 * std::sinf(_28) + 1.0f;
    return (u8)(_24 == 0.0f);
}

bool KartDossin::DoCheckEnd() {
    mBody->mCarStatus &= ~0x280000000uLL;
    _1c = 1.0f;
    _15 = 0;
    mVelocity.zero();
    return true;
}

void KartDossin::DoDossinCrl() {
    KartBody *kartBody = mBody;

    switch (_15) {
        case 0:
            DoKeep();
            _20 = 0.0f;
            mVelocity.zero();
            _14 |= 0x20;
            _16 = 0;
            _15 = 1;
            break;

        case 1:
            DoKeep();
            mGeoObj->getVelocity(&mVelocity);

            if (mVelocity.y <= 0.0f) {
                mVelocity.zero();
                _20 = 0.0f;
            } else {
                _20 = 150.0f;
            }
            
            mVelocity.zero();
            _20 = 0.0f;

            if (mGeoObj->tstUserFlg1(kartBody->mMynum)) {
                DoClear();
                _16 = 0;
                _15 = 2;
            }
            break;

        case 2:
            _16++;
            DoCheck();
            if (_16 > 300) { // 0x12c
                _16 = 0;
                _15 = 3;
            }
            break;

        case 3:
            DoCheck();
            if (DoReturn() == true) {
                _16 = 0;
                _15 = 4;
                DoShake();
                GetKartCtrl()->getKartSound(kartBody->mMynum)->DoKartsetSeSound(0x1007A);
            }
            break;

        case 4:
            if (DoShake() == true) {
                _16 = 0;
                _15 = 5;
                DoCheckEnd();
            }
            break;

        case 5:
        case 6:
        default:
            break;
    }
}


void KartDossin::DoAfterDossinCrl() {
    switch (_15) {
        case 0:
            DoKeep();
            break;

        case 1:
            DoKeep();
            break;

        case 3:
            break;
    }
}
