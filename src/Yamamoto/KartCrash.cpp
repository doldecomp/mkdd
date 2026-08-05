#include "Yamamoto/KartCrash.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

#include "Sato/ItemObjMgr.h"
#include "Sato/ItemTurtle.h"
#include "Sato/JPEffectPerformer.h"
#include "Sato/stEffectMgr.h"

// comments inside functions are inline functions being called in that function

void KartCrash::Init(int bodyIndex) {
    mBody = GetKartCtrl()->getKartBody(bodyIndex);
    _C = 0;
    _D = 0;
    _E = 0;
    mMatchlessTimer = 0;
}

void KartCrash::DoRabbitMark() {
    KartChecker* checker = RCMGetKartChecker(mBody->mMynum);

    if (RaceMgr::getCurrentManager()->getRaceMode() == ESCAPE_BATTLE && checker->releaseRabbitMark()) {
        mMatchlessTimer = 200;
    }
}

void KartCrash::DoMatchlessTimer() {
    KartBody* body  = mBody;
    RaceMgr* mgr = RaceMgr::getCurrentManager();
    KartChecker* kartChecker = RCMGetKartChecker(body->mMynum);

    if (mgr->getRaceMode() == BALLOON_BATTLE) {
        if (mMatchlessTimer != 0 && (body->mCarStatus & 0x100000) == 0) {
            mMatchlessTimer--;
            if (mMatchlessTimer == 199) {
                if (kartChecker->getBalloonNumber() != 0) {
                    if (!mgr->getKartDrawer(body->mMynum)->isFlashing()) {
                        mgr->getKartDrawer(body->mMynum)->flashOn();
                        JPEffectPerformer::setEffect(JPEffectPerformer::Effect_UnknownD, body->mMynum, body->mPos, 1);
                    }

                }
            }
        }
        else {
            mgr->getKartDrawer(body->mMynum)->flashOff();
        }
    }
    else {
        if (mMatchlessTimer != 0 && (body->mCarStatus & 0x100000) == 0) {
            mMatchlessTimer--;
            if (mMatchlessTimer == 199) {
                if (!mgr->getKartDrawer(body->mMynum)->isFlashing()) {
                    mgr->getKartDrawer(body->mMynum)->flashOn();
                    JPEffectPerformer::setEffect(JPEffectPerformer::Effect_UnknownD, body->mMynum, body->mPos, 1);
                }
            }
        } else {
            mgr->getKartDrawer(body->mMynum)->flashOff();
        }
    }
}

bool KartCrash::CheckMatchlessTimer() {
    if (mMatchlessTimer != 0) return true;
    return false;
}

void KartCrash::SetMatchlessTimer() {
    RaceMgr* mgr = RaceMgr::getCurrentManager();
    if (mgr->getRaceMode() == BALLOON_BATTLE || mgr->getRaceMode() == BOMB_BATTLE) {
        mMatchlessTimer = 200;
    }
}

void KartCrash::ClearMatchlessTimer() {
    KartBody* body = mBody;
    RaceMgr* mgr = RaceMgr::getCurrentManager();
    mMatchlessTimer = 0;
    mgr->getKartDrawer(body->mMynum)->flashOff();
}

void KartCrash::DoDecBalloon() {
    KartBody* body = mBody;
    KartChecker* kartChecker = RCMGetKartChecker(body->mMynum);
    RaceMgr* mgr = RaceMgr::getCurrentManager();

    if (mgr->getRaceMode() == BALLOON_BATTLE && kartChecker->decBalloon()) {
        GetKartCtrl()->getKartSound(body->mMynum)->DoKartsetSeSound(0x40052);
        SetMatchlessTimer();
    }
}

void KartCrash::DoPointBomb(ItemObj *itemObj) {
    KartBody* body = mBody;
    if (RaceMgr::getCurrentManager()->getRaceMode() != BOMB_BATTLE || itemObj->getKind() != 8) {
        return;
    }
    RCMGetKartChecker(body->mMynum)->setBEHitBomb(itemObj);
}

void KartCrash::DoDecPointBomb() {
    KartBody* body = mBody;
    RaceMgr* mgr = RaceMgr::getCurrentManager();
    KartChecker* kartChecker = RCMGetKartChecker(body->mMynum);

    if (mgr->getRaceMode() == BOMB_BATTLE) {
        kartChecker->setBEJugem();
    }
}

void KartCrash::SaveDir() {
    KartBody* body = mBody;
    JGeometry::TVec3f  stack_38;
    JGeometry::TVec3f  stack_2C;
    JGeometry::TVec3f  stack_20;
    JGeometry::TVec3f  stack_14;
    JGeometry::TVec3f  stack_8;

    stack_2C.set(body->_308);

    if (body->_2fc.y < 0.0f) {
        stack_2C.scale(-1.0f);
    }

    stack_38.set(0.0f, 1.0f, 0.0f);
    stack_20.cross(stack_38, stack_2C);
    stack_20.normalize();
    stack_14.cross(stack_2C, stack_20);
    stack_14.normalize();
    stack_2C.cross(stack_20, stack_14);
    stack_2C.normalize();
    body->_338.set(stack_2C);

}

void KartCrash::NonRescue() {
    KartBody* body = mBody;
    CrsArea area;
    area.searchInvValley(body->mPos);

    if (area.isInside()) {
        body->mCarStatus |= 0x40000000;
    }
}

void KartCrash::MakeRollCrash() {
    // UNUSED, size somehow matches lol
    // Needed for the 180.0f, this probably wasn't what was originally written
    // There's no way of knowing what this actually did
    KartBody* body = mBody;
    u32 num = body->mMynum;

    if (body->getChecker()->CheckCrash() == false) {
        body->getGame()->DoRollThrow();
        body->_584 = 5;
        body->_588 = 0;
        body->_4b8 = (body->mVel.dot(body->_308) / 180.0f);
        body->_594 = 0;

        NonRescue();
        body->mCarStatus |= 0x300000;
        SaveDir();
        body->getGame()->MakeClear();
        body->getDamage()->SetDamageAnime();
        body->getItem()->FallItem();
        GetKartCtrl()->getKartSound(num)->DoRollCrashStartSound();
        body->getStrat()->DoMotor(MotorManager::MotorType_10);
        GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
        JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, num, body->mPos, 1);
        SetMatchlessTimer();
    }
}

void KartCrash::DoRollCrashCrl() {
    KartBody* body = mBody;
    GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashSound();

    switch (body->_588) {
        case 1:
            break;
        case 0:
            body->_594++;

            if (body->_594 < 80 && body->_594 > 20 && body->getTouchNum() >= 3) {
                body->_588 = 1;
            } else if (body->_594 >= 80) {
                _D = 1;
            }
            GetKartCtrl()->ChaseFnumber(&body->_4b4, body->_4bc, 0.1f);
            if (body->_584 == 3) {
                if (body->_594 <= 20) {
                    body->mWg.scale(0.65f);
                    body->mWg.z += body->_4b4;
                    body->mWg.x  += body->_4b4 * body->_4b8;
                }
            }
            else if (body->_594 <= 20) {
                    body->mWg.scale(0.65f);
                    body->mWg.z -= body->_4b4;
                    body->mWg.x  += body->_4b4 * body->_4b8;
            }
        break;
    }
}

void KartCrash::DoPitchCrashCrl() {
    KartBody* body = mBody;
    GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashSound();

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
            GetKartCtrl()->ChaseFnumber(&body->_4b4, body->_4bc, 0.1f);
            if (body->_594 <= 20) {
                    body->mWg.scale(0.4f);
                    body->mWg.x += body->_4b4;
                    body->mWg.z  += body->_4b4 * body->_4b8;
            }
        break;
    }
}

void KartCrash::MakeBombCrash(ItemObj *itemObj) {
    KartBody* body = mBody;
    u32 num = body->mMynum;
    if ((body->mCarStatus & 0x2280100c00000) == 0 && (body->getDossin()->_14 & 3) == 0) {
        body->_584 = 6;
        body->_588 = 0;
        body->mCarStatus &= 0xfffffff7fed7ffff;
        body->getTumble()->_0[4] = 0;
        body->mVel.zero();
        body->_594 = 0;
        f32 val = 600.0f * body->_3a4;
        body->_2cc.y += val;
        body->_4bc = 0.174444f;
        NonRescue();
        body->mCarStatus |= 0x900000;
        SaveDir();
        body->getGame()->MakeClear();
        body->getDamage()->SetBigDamageAnime();
        body->getItem()->FallItem();
        body->getStrat()->DoMotor(MotorManager::MotorType_12);
        body->getGame()->ItemWatchMan(itemObj);
        GetKartCtrl()->getKartSound(num)->DoShootVoice();
        GetKartCtrl()->getKartSound(num)->DoRollCrashStartSound();
        SetMatchlessTimer();
        DoPointBomb(itemObj);
        JPEffectPerformer::setEffect(JPEffectPerformer::Effect_UnknownF, num, body->mPos, 1);
    }
}

void KartCrash::DoBombCrashCrl() {
    KartBody* body = mBody;
    GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashSound();
    switch (body->_588) {
        case 1:
            break;
        case 0:
            body->_594++;
            if (body->_594 < 6) {
                body->mWg.x  += 0.314f;
                GetKartCtrl()->ChaseFnumber(&body->_4bc, -0.209333f, 0.098f);
                body->mWg.z += body->_4bc;
            }
            if (body->_594 > 100) {
                body->_588 = 1;
            }
        break;
    }
}

void KartCrash::MakeKameCrash(ItemObj *itemObj) {
    KartBody* body = mBody;
    u32 num = body->mMynum;

    if (body->getChecker()->CheckCrash() == false) {
        body->getGame()->DoRollThrow();
        body->_584 = 5;
        body->_588 = 0;
        body->_4b8 = 0.2f * (body->mVel.dot(body->_308) / 180.0f);
        body->_594 = 0;
        body->_4b4 = 0.0f;
        body->_4bc = 0.610555f;
        NonRescue();
        body->mCarStatus |= 0x300000;
        SaveDir();
        body->getGame()->MakeClear();
        body->getDamage()->SetDamageAnime();
        body->getItem()->FallItem();
        GetKartCtrl()->getKartSound(num)->DoRollCrashStartSound();
        body->getStrat()->DoMotor(MotorManager::MotorType_10);
        body->getGame()->ItemWatchMan(itemObj);
        GetKartCtrl()->getKartSound(body->mMynum)->DoRollCrashVoice();
        JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown0, num, body->mPos, 1);
        SetMatchlessTimer();
    }
}

void KartCrash::MakeTornado() {
    KartBody* body = mBody;
    u32 num = body->mMynum;

    if (body->getChecker()->CheckCrash() == false) {
        _4 = (TKartThrower*)GetGeoObjMgr()->getKartReactHitObjectList(body->mMynum)[16];

        //required to get loading order right lol
        TKartThrower* temp_4 = _4;

        JGeometry::TVec3f stack_8;
        f32 fVar2 = 200.0f * temp_4->getThrowPow() * body->_3a4;

        //Why do you need a whole vector if you are gonna multiply by 1.0f?
        stack_8.set(0.0f, 1.0f, 0.0f);

        body->_2cc.y += fVar2 * stack_8.y;

        body->_4a8 = -0.0174444f;
        body->_4bc = body->_4b0 = body->_4ac = 0.0f;
        body->_584 = 15;
        body->_588 = 0;
        body->_594 = 0;
        SaveDir();
        body->mCarStatus |= (0x80000100000ul);
        if ((body->getThunder()->mFlags & 1) == 0) {
            if (body->_4a8 < 0.0f) {
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown1, num, body->mPos, 1);
            } else {
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown1, num, body->mPos, 0);
            }
        }
        body->getGame()->MakeClear();
        body->getDamage()->SetDamageAnime();
        body->getItem()->FallItem();
        GetKartCtrl()->getKartSound(num)->DoSpinVoice();
        body->getStrat()->DoMotor(MotorManager::MotorType_9);
        GetKartCtrl()->getKartSound(body->mMynum)->DoKartsetSeSound(0x10080);
        SetMatchlessTimer();
    }
}

void KartCrash::DoTornadeCenter() {
    KartBody* body = mBody;
    JGeometry::TVec3f newVel;

    if (body->mVel.y > 0.0f) {
        _4->getPosition(&newVel);
        body->_4bc += 0.174444f;
        newVel.y = body->mPos.y;
        newVel.sub(body->mPos);
        f32 length_8 = newVel.length();

        newVel.normalize();

        body->mVel.x = newVel.x * length_8;
        body->mVel.z = newVel.z * length_8;
    }
}

void KartCrash::DotornadeCrashCrl() {
    KartBody* body = mBody;
    JGeometry::TVec3f stack_8;
    switch (body->_588) {
        case 0:
            DoTornadeCenter();
            body->_594 = 0;
            body->mVel.y = 14.0f * body->getCrash()->_4->getThrowPow();

            f32 cnvge_4ac = body->_4ac;

            if (cnvge_4ac > 10.0f) {
                body->_4b0 = GetKartCtrl()->fcnvge(body->_4b0, 0.069777697f, 0.0069777598f, 0.0069777598f);
            } else {
                body->_4b0 = GetKartCtrl()->fcnvge(body->_4b0, 0.33f, 0.0348888f, 0.0348888f);
            }
            cnvge_4ac = GetKartCtrl()->fcnvge(cnvge_4ac, 18.84f, body->_4b0, body->_4b0);
            if (body->_4a8 < 0.0f) {
                body->mWg.y = -(cnvge_4ac - body->_4ac) / body->mSpeedScale;
            } else {
                body->mWg.y = (cnvge_4ac - body->_4ac) / body->mSpeedScale;
            }
            body->_4ac = cnvge_4ac;
            cnvge_4ac = 0.0f;
            if (body->_4ac > 17.6f) {
                for (s32 i = 0; i < 4; i++) {
                    body->getSus(i)->_10c = cnvge_4ac;
                    body->getSus(i)->_110 = cnvge_4ac;
                }
                body->_588 = 1;
            }
            return;
        case 1:
            GetKartCtrl()->ChaseFnumber(&body->mWg.y, 0.0f, 0.05f);
            if (body->_594 < 1) {
                DoTornadeCenter();
                body->_594++;
                body->mVel.y = body->getCrash()->_4->getThrowPow();
                return;
            }
            body->mVel.zero();
            body->_2cc.zero();
            _4->getVelocity(&stack_8);
            body->mVel.x = -5.0f * stack_8.x;
            body->mVel.y = 5.0f * stack_8.y;
            body->mVel.z = -5.0f * stack_8.z;
            body->_588 = 2;
            return;
        case 2:
            GetKartCtrl()->ChaseFnumber(&body->mWg.y, 0.0f, 0.029999999f);
            if (body->getTouchNum() != 0) {
                body->getStrat()->MovingTornadeClear();
                body->mVel.x *= 0.0099999998f;
                body->mVel.y = 80.0f;
                body->mVel.z *= 0.0099999998f;
            }
            return;
    }
    body->getStrat()->MovingTornadeClear();
}

void KartCrash::MakeSpin(ItemObj *itemObj) {
    KartBody* body = mBody;
    u32 num = body->mMynum;

    if (body->getChecker()->CheckCrash() != true) {
        if (body->mTireAngle == 0.0f) {
            JGeometry::TVec3f stack_8;
            GetKartCtrl()->DevMatrixByVector(&stack_8, &body->_2cc, body->_110);
            if (stack_8.x < 0.0f) {
                body->_4a8 = -0.0174444f;
            } else {
                body->_4a8 = 0.0174444f;
            }
        } else if (body->mTireAngle < 0.0f) {
            body->_4a8 = -0.0174444f;
        } else {
            body->_4a8 = 0.0174444f;
        }
        body->_4b0 = body->_4ac = 0.0f;
        body->_584 = 1;
        body->_588 = 0;
        body->mCarStatus |= 0x180000;
        if ((body->getThunder()->mFlags & 1) == 0) {
            if (body->_4a8 < 0.0f) {
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown1, num, body->mPos, 1);
            } else {
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown1, num, body->mPos, 0);
            }
        }
        body->getGame()->MakeClear();
        body->getDamage()->SetDamageAnime();
        body->getItem()->FallItem();
        GetKartCtrl()->getKartSound(num)->DoSpinVoice();
        body->getStrat()->DoMotor(MotorManager::MotorType_9);
        body->getGame()->ItemWatchMan(itemObj);
        SetMatchlessTimer();
        DoPointBomb(itemObj);
    }
}

void KartCrash::DoSpinCrashCrl() {
    KartBody* body = mBody;
    switch (body->_588) {
        case 0:
            f32 cnvge_4ac = body->_4ac;

            if (cnvge_4ac > 10.0f) {
                body->_4b0 = GetKartCtrl()->fcnvge(body->_4b0, 0.0697777f, 0.00348888f, 0.00348888f);
            } else {
                body->_4b0 = GetKartCtrl()->fcnvge(body->_4b0, 0.174444f, 0.0174444f, 0.0174444f);
            }
            cnvge_4ac = GetKartCtrl()->fcnvge(cnvge_4ac, 12.56f, body->_4b0, body->_4b0);
            if (body->_4a8 < 0.0f) {
                body->mWg.y = -(cnvge_4ac - body->_4ac) / body->mSpeedScale;
            } else {
                body->mWg.y = (cnvge_4ac - body->_4ac) / body->mSpeedScale;
            }
            body->_4ac = cnvge_4ac;
            GetKartCtrl()->getKartSound(body->mMynum)->DoSpinSound();
            cnvge_4ac = 0.0f;
            if (body->_4ac > 12.55f) {
                for (s32 i = 0; i < 4; i++) {
                    body->getSus(i)->_10c = cnvge_4ac;
                    body->getSus(i)->_110 = cnvge_4ac;
                }
                body->getStrat()->MovingSpinClear();
            }
            return;
        default:
            body->getStrat()->MovingSpinClear();
    }
}

void KartCrash::MakeHalfSpin(ItemObj *itemObj) {
    KartBody* body = mBody;
    u32 num = body->mMynum;

    if (body->getChecker()->CheckCrash() != true) {
        if (body->mTireAngle == 0.0f) {
            JGeometry::TVec3f stack_8;
            GetKartCtrl()->DevMatrixByVector(&stack_8, &body->_2cc, body->_110);
            if (stack_8.x < 0.0f) {
                body->_4a8 = -0.0174444f;
            } else {
                body->_4a8 = 0.0174444f;
            }
        } else if (body->mTireAngle < 0.0f) {
            body->_4a8 = -0.0174444f;
        } else {
            body->_4a8 = 0.0174444f;
        }
        body->_4b0 = body->_4ac = 0.0f;
        body->_584 = 7;
        body->_588 = 0;
        body->mCarStatus |= 0x1100000;
        if ((body->getThunder()->mFlags & 1) == 0) {
            if (body->_4a8 < 0.0f) {
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown2, num, body->mPos, 1);
            } else {
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown2, num, body->mPos, 0);
            }
        }
        body->getGame()->MakeClear();
        body->getDamage()->SetDamageAnime();
        body->getItem()->FallItem();
        GetKartCtrl()->getKartSound(num)->DoSpinVoice();
        body->getStrat()->DoMotor(MotorManager::MotorType_9);
        body->getGame()->ItemWatchMan(itemObj);
        SetMatchlessTimer();
    }
}

void KartCrash::DoHalfSpinCrashCrl() {
    KartBody* body = mBody;
    switch (body->_588) {
        case 0:
            f32 cnvge_4ac = body->_4ac;

            if (cnvge_4ac > 5.0f) {
                body->_4b0 = GetKartCtrl()->fcnvge(body->_4b0, 0.1046666f, 0.0174444f, 0.0174444f);
            } else {
                body->_4b0 = GetKartCtrl()->fcnvge(body->_4b0, 0.174444f, 0.0174444f, 0.0174444f);
            }
            cnvge_4ac = GetKartCtrl()->fcnvge(cnvge_4ac, 6.28f, body->_4b0, body->_4b0);
            if (body->_4a8 < 0.0f) {
                body->mWg.y = -(cnvge_4ac - body->_4ac) / body->mSpeedScale;
            } else {
                body->mWg.y = (cnvge_4ac - body->_4ac) / body->mSpeedScale;
            }
            body->_4ac = cnvge_4ac;
            GetKartCtrl()->getKartSound(body->mMynum)->DoSpinSound();
            cnvge_4ac = 0.0f;
            if (body->_4ac > 6.17f) {
                for (s32 i = 0; i < 4; i++) {
                    body->getSus(i)->_10c = cnvge_4ac;
                    body->getSus(i)->_110 = cnvge_4ac;
                }
                body->getStrat()->MovingHalfSpinClear();
                body->mTireAngle = 0;
            }
            return;
        default:
            body->getStrat()->MovingHalfSpinClear();
    }
}

void KartCrash::MakeThunderSpin() {
    KartBody* body = mBody;
    if (body->getChecker()->CheckCrash() != true) {
        MakeSpin(nullptr);
        body->getDamage()->mFlags |= 0x80;
        body->getDamage()->SetBurnAnime();
    }
}

void KartCrash::MakeBurn(ItemObj *itemObj) {
    KartBody* body = mBody;
    u32 num = body->mMynum;
    ItemObjMgr* mgr = GetItemObjMgr();

    ItemObjMgr::KartHitList* list = mgr->getKartHitList(num);
    if (body->getChecker()->CheckCrash() != true) {
        ItemObj* fireBallObj = list->mObjects[10];
        if (fireBallObj != nullptr) {
            JGeometry::TVec3f stack_8;
            stack_8.set(body->mPlayerPosMtx[0][3], body->mPlayerPosMtx[1][3], body->mPlayerPosMtx[2][3]);
            GetKartCtrl()->getKartSound(body->mMynum)->DoHitFireBall();
            ItemFireBall* fireBall = (ItemFireBall*)mgr->getItemFireBall(fireBallObj);

            JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Fireball, num, stack_8, (!fireBall->IsEfctTypeRed() ? (u8)1 : (u8)0)); // TODO: perhaps these are some constants
        }
        MakeSpin(nullptr);
        body->getDamage()->mFlags |= 0x80;
        body->getDamage()->SetBurnAnime();
        body->getGame()->ItemWatchMan(itemObj);
    }
}

void KartCrash::MakeFreezeCrash() {
    KartBody* body = mBody;

    if (body->getChecker()->CheckCrash() != true) {
        body->_584 = 18;
        body->_588 = 0;
        body->_594 = 0;
        body->mCarStatus |=  (0x2000000100000);
        u16 oldDamageFlags = body->getDamage()->mFlags;
        body->getGame()->MakeClear();
        body->getDamage()->mFlags = oldDamageFlags;
        GetKartCtrl()->getKartAnime(body->mMynum)->DoStopAnime(body->mMynum);
        body->getItem()->FallAllItem();
        _8 = nullptr;
        _8 = GetJ3DEfctMgr()->setEffectKart(body->mMynum, 3, body->_110);
        //RCMGetManager doesn't inline here
        RaceMgr::getManager()->getKartDrawer(body->mMynum)->playTevAnm(1);
        GetKartCtrl()->getKartSound(body->mMynum)->DoKartsetSeSound(0x10060);
        body->getStrat()->DoMotor(MotorManager::MotorType_16);
        SetMatchlessTimer();
        body->mCarStatus |= 0x80000000;
        GetItemObjMgr()->abortItemShuffle(body->mMynum);
        GetStEfctMgr()->setExhaustStopFlg(body->mMynum, true);
    }
}

void KartCrash::DoFreezeCrashCrl() {
    KartBody* body = mBody;

    switch (body->_588) {
        case 1:
        break;
        case 0:
        body->mCarStatus |=  0x80000000;
        GetItemObjMgr()->abortItemShuffle(body->mMynum);
        body->_594++;
        if (body->_594 == 90) {
            if (_8 != nullptr) {
                _8->setTrigger();
                RaceMgr::getManager()->getKartDrawer(body->mMynum)->stopTevAnm();
                GetKartCtrl()->getKartSound(body->mMynum)->DoKartsetSeSound(0x1004C);

            }
            GetKartCtrl()->getKartAnime(body->mMynum)->DoReStopAnime(body->mMynum);
            body->getStrat()->FreezeClear();
            GetStEfctMgr()->setExhaustStopFlg(body->mMynum, false);
            body->getStrat()->DoMotor(MotorManager::MotorType_17);
            body->_5c3 &= ~2;
        }
        break;
    }
}
