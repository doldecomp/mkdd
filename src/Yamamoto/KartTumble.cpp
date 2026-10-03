#include "Yamamoto/KartTumble.h"

#include "Yamamoto/kartCtrl.h"

#include "Sato/ItemObjMgr.h"
#include "Sato/JPEffectPerformer.h"

#include "Shiraiwa/TKartThrower.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

// comments inside functions are inline functions being called in that function

void KartTumble::Init(int index) {
    mBody = GetKartCtrl()->getKartBody(index);
    _4[0] = 0;
    _4[1] = 0;
    *(u16*)&_14[0] = 0;
    _8.zero();
}

void KartTumble::MakeWanWanTumble(ItemObj *itemObj) {
    KartBody* body = mBody;
    ItemObjMgr* mgr = GetItemObjMgr();
    ItemObjMgr::KartHitList* hitList = mgr->getKartHitList(body->mMynum);
    if (_4[0] & 1) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f v1;
                JGeometry::TVec3f v2;
                JGeometry::TVec3f v3;
                body->_584 = 4;
                body->_588 = 0;
                _4[1] = 0;
                hitList->mObjects[9]->getPos(&v3);
                body->getGame()->ItemWatchMan(itemObj);
                v3.y = body->mPos.y;
                v1.sub(v3, body->mPos);
                v1.length();
                body->_308.angle(v1);
                v2.set(v1);
                v2.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&_8, &v2, body->_110);
                _8.y = 0.0f;
                _8.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->getCrash()->NonRescue();
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                v2.normalize();
                v1.set(v2);
                v1.y = 0.0f;
                v1.scale(80.0f);
                v1.scale(body->_3a4);
                body->DoForce(&body->mPos, &v1);
                _4[0] |= 1;
                *(u16*)&_14[0] = 60;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::MakeKameTumble(ItemObj *itemObj) {
    KartBody* body = mBody;
    ItemObjMgr* mgr = GetItemObjMgr();
    ItemObjMgr::KartHitList* hitList = mgr->getKartHitList(body->mMynum);
    if (_4[0] & 1) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f v1;
                JGeometry::TVec3f v2;
                JGeometry::TVec3f v3;
                body->_584 = 4;
                body->_588 = 0;
                _4[1] = 0;
                hitList->mObjects[11]->getPos(&v3);
                v3.y = body->mPos.y;
                v1.sub(v3, body->mPos);
                v1.length();
                body->_308.angle(v1);
                v2.set(v1);
                v2.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&_8, &v2, body->_110);
                _8.y = 0.0f;
                _8.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                v2.normalize();
                v1.set(v2);
                v1.y = 0.0f;
                v1.scale(80.0f);
                v1.scale(body->_3a4);
                body->DoForce(&body->mPos, &v1);
                _4[0] |= 1;
                *(u16*)&_14[0] = 60;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getGame()->ItemWatchMan(itemObj);
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::MakeStarTumble() {
    KartBody* body = mBody;
    if (_4[0] & 1) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f v1;
                JGeometry::TVec3f v2;
                JGeometry::TVec3f v3;
                body->_584 = 4;
                body->_588 = 0;
                _4[1] = 0;
                v3.set(((KartBody*)body->mUnkSub10c)->mPos);
                v3.y = body->mPos.y;
                v1.sub(v3, body->mPos);
                v2.set(v1);
                v2.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&_8, &v2, body->_110);
                _8.y = 0.0f;
                _8.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->getCrash()->NonRescue();
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->ReleseWanWan();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                v2.normalize();
                v1.set(v2);
                v1.y = 0.0f;
                v1.scale(80.0f);
                v1.scale(body->_3a4);
                body->DoForce(&body->mPos, &v1);
                _4[0] |= 1;
                *(u16*)&_14[0] = 60;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::MakeDashTumble() {
    KartBody* body = mBody;
    JGeometry::TVec3f v1;
    JGeometry::TVec3f v2;
    JGeometry::TVec3f v3;
    v3.set(((KartBody*)body->mUnkSub10c)->mPrevPos);
    v1.sub(v3, body->mPrevPos);
    v2.sub(body->_308, ((KartBody*)body->mUnkSub10c)->_308);
    f32 ang = v2.angle(v1);
    if (!((180.0f * ang) / 3.141f < 90.0f)) {
        v1.normalize();
        v1.y = 0.0f;
        v1.scale(500.0f);
        v1.scale(body->_3a4);
        body->DoForce(&body->mPos, &v1);
    }
}

void KartTumble::DoTumble() {
    KartBody* body = mBody;
    GeographyObj* obj = GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[11];
    if (_4[0] & 1) return;
    if (obj == 0) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f v1;
                JGeometry::TVec3f v2;
                JGeometry::TVec3f v3;
                JGeometry::TVec3f v4;
                body->_584 = 4;
                body->_588 = 0;
                _4[1] = 0;
                obj->getPosition(&v4);
                obj->getColScaleRadius();
                GetGeoObjMgr()->getKartHitDepthNormalObj(body->mMynum);
                v3.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(body->mMynum));
                obj->getVelocity(&v2);
                v2.y = 0.0f;
                v4.y = body->mPos.y;
                v1.sub(v4, body->mPos);
                v2.set(v1);
                v2.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&_8, &v2, body->_110);
                _8.y = 0.0f;
                _8.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->ObjectReflection(&v3);
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                v2.normalize();
                v1.set(v2);
                v1.y = 0.0f;
                v1.scale(80.0f);
                v1.scale(body->_3a4);
                body->DoForce(&body->mPos, &v1);
                _4[0] |= 1;
                *(u16*)&_14[0] = 300;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::DoPakunTumble() {
    KartBody* body = mBody;
    GeographyObj* obj = GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[18];
    if (_4[0] & 1) return;
    if (obj == 0) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f v1;
                JGeometry::TVec3f v2;
                JGeometry::TVec3f v3;
                JGeometry::TVec3f v4;
                body->_584 = 4;
                body->_588 = 0;
                _4[1] = 0;
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown6, body->mMynum, body->mPos, false);
                obj->getPosition(&v4);
                obj->getColScaleRadius();
                GetGeoObjMgr()->getKartHitDepthNormalObj(body->mMynum);
                v3.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(body->mMynum));
                obj->getKartThrowDirPow(&v1, 0, body->mMynum);
                v2.set(v1);
                GetKartCtrl()->DevMatrixByVector(&_8, &v2, body->_110);
                _8.y = 0.0f;
                v2.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                v1.y = 0.0f;
                v1.normalize();
                v1.scale(body->_3a4);
                body->DoForce(&body->mPos, &v1);
                _4[0] |= 1;
                *(u16*)&_14[0] = 120;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::DoHanaTumble() {
    KartBody* body = mBody;
    GeographyObj* obj = GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[15];
    if (_4[0] & 1) return;
    if (obj == 0) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f v1;
                JGeometry::TVec3f v2;
                JGeometry::TVec3f v3;
                JGeometry::TVec3f v4;
                body->_584 = 4;
                body->_588 = 0;
                _4[1] = 0;
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown6, body->mMynum, body->mPos, false);
                obj->getPosition(&v4);
                obj->getColScaleRadius();
                GetGeoObjMgr()->getKartHitDepthNormalObj(body->mMynum);
                v3.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(body->mMynum));
                obj->getVelocity(&v2);
                v2.y = 0.0f;
                v4.y = body->mPos.y;
                v1.sub(v4, body->mPos);
                v2.set(v1);
                v2.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&_8, &v2, body->_110);
                _8.y = 0.0f;
                _8.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.680333f;
                body->mCarStatus |= 0x00102000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->ObjectReflection(&v3);
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                v2.normalize();
                v1.set(v2);
                v1.y = 0.0f;
                v1.scale(80.0f);
                v1.scale(body->_3a4);
                body->DoForce(&body->mPos, &v1);
                _4[0] |= 1;
                *(u16*)&_14[0] = 120;
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashStartSound();
                GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_11);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::MakePoiHanaTumble() {
    KartBody* body = mBody;
    TKartThrower* obj = (TKartThrower*)GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[14];
    if (_4[0] & 1) return;
    if (obj == 0) return;
    switch (body->getChecker()->CheckCrash()) {
        case 0:
            {
                JGeometry::TVec3f v1;
                JGeometry::TVec3f v2;
                JGeometry::TVec3f v3;
                JGeometry::TVec3f v4;
                body->_584 = 13;
                body->_588 = 0;
                _4[1] = 0;
                obj->getPosition(&v4);
                obj->getColScaleRadius();
                GetGeoObjMgr()->getKartHitDepthNormalObj(body->mMynum);
                v3.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(body->mMynum));
                obj->getVelocity(&v2);
                v2.y = 0.0f;
                v4.y = body->mPos.y;
                v1.sub(v4, body->mPos);
                v1.length();
                body->_308.angle(v1);
                v2.set(v1);
                v2.scale(-1.0f);
                GetKartCtrl()->DevMatrixByVector(&_8, &v2, body->_110);
                _8.y = 0.0f;
                _8.normalize();
                body->_594 = 0;
                body->_4b4 = 0.0f;
                body->_4bc = 0.191888f;
                body->mCarStatus |= 0x0000000800100000;
                body->getCrash()->SaveDir();
                body->getGame()->MakeClear();
                body->getDamage()->SetBigDamageAnime();
                body->getItem()->FallItem();
                body->mVel.zero();
                body->_2cc.zero();
                body->mWg.zero();
                JGeometry::TVec3f v5;
                f32 fVar = (f32)obj->getThrowPow();
                f32 five = 5.0f;
                fVar = fVar * five;
                fVar = fVar * body->_3a4;
                obj->getThrowDir(&v5, body->mMynum);
                body->_2cc.x += fVar * v5.x;
                body->_2cc.y += fVar * v5.y;
                body->_2cc.z += fVar * v5.z;
                _4[0] |= 1;
                *(u16*)&_14[0] = 120;
                if (obj->getKind() == 3402) {
                    GetKartCtrl()->getKartSound(body->mMynum)->DoShootSound();
                }
                GetKartCtrl()->getKartSound(body->mMynum)->DoShootVoice();
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, body->mMynum, body->mPos, true);
                body->getStrat()->DoMotor(MotorManager::MotorType_12);
                body->getCrash()->SetMatchlessTimer();
            }
            break;
        default:
            return;
    }
}

void KartTumble::DoShootCrashCrl() {
    KartBody* body = mBody;
    if (!(body->mCarStatus & 0x800000000)) {
        GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashSound();
    }
    switch (body->_588) {
        case 1:
            break;
        case 0:
            body->_594++;
            if (body->_594 < 80 && body->_594 > 20 && body->getTouchNum() >= 3) {
                body->_588 = 1;
            } else if (body->_594 >= 80) {
                body->_588 = 1;
            }
            GetKartCtrl()->ChaseFnumber(&body->_4b4, body->_4bc, 0.8f);
            if (!((s64)(s32)_4[0] & 0x400)) {
                if (body->_594 < 10) {
                    body->mVel.y += 10.0f;
                }
                if (body->_594 <= 20) {
                    body->mWg.scale(0.4f);
                    body->mWg.x += body->_4b4 * _8.z;
                    body->mWg.z += body->_4b4 * (-_8.x);
                }
            }
            break;
    }
}

void KartTumble::DoTumbleCrl() {
    if (*(u16*)&_14[0] != 0) {
        (*(u16*)&_14[0])--;
    } else {
        _4[0] &= 0xFE;
    }
}

void KartTumble::DoAfterTumbleCrl() {
    KartBody* body = mBody;
    if (!(body->mCarStatus & 0x800000000)) {
        GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashSound();
    }
    switch (body->_588) {
        case 1:
            break;
        case 0:
            body->_594++;
            if (body->_594 < 80 && body->_594 > 20 && body->getTouchNum() >= 3) {
                body->_588 = 1;
            } else if (body->_594 >= 80) {
                body->_588 = 1;
            }
            GetKartCtrl()->ChaseFnumber(&body->_4b4, body->_4bc, 0.8f);
            if (!((s64)(s32)_4[0] & 0x400)) {
                if (body->_594 <= 20) {
                    body->mWg.scale(0.4f);
                    body->mWg.x += body->_4b4 * _8.z;
                    body->mWg.z += body->_4b4 * (-_8.x);
                }
            }
            break;
    }
}
