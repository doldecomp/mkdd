#include "Kaneshige/RaceMgr.h"
#include "Sato/ItemObjMgr.h"
#include "Sato/JPEffectPerformer.h"
#include "Yamamoto/KartGame.h"
#include "Yamamoto/kartBody.h"
#include "Yamamoto/kartCtrl.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

// comments inside functions are inline functions being called in that function

bool KartGame::Init(int kartNo) {
    mBody = GetKartCtrl()->getKartBody(kartNo);
    _8 = 0;
    _9 = 0;
    _38.zero();
    _4 = 0;
    _18[0] = 0.f;
    _18[1] = 0.f;
    _E = 0;
    _10 = 0;
    mCountDownDuration = 0;

    _A = 0;
    mTimeToChange = 0;
    _C[0] = 0;

    bool isRight = RaceMgr::getCurrentManager()->getStartPoint(&_20, &_2C, kartNo);

    _20.y += 300.f;

    return isRight;
}

void KartGame::GetGorundTireNum() {
    u8 num = mBody->mMynum;

    KartBody *body = mBody;
    body->mTouchNum = 0;

    KartSus *sus[4];
    int idx = num;
    sus[0] = GetKartCtrl()->getKartSus(idx * 4 + 0);
    sus[1] = GetKartCtrl()->getKartSus(idx * 4 + 1);
    sus[2] = GetKartCtrl()->getKartSus(idx * 4 + 2);
    sus[3] = GetKartCtrl()->getKartSus(idx * 4 + 3);

    if (sus[0]->_124 & 1)
        ++body->mTouchNum;

    if (sus[1]->_124 & 1)
        ++body->mTouchNum;

    if (sus[2]->_124 & 1)
        ++body->mTouchNum;

    if (sus[3]->_124 & 1)
        ++body->mTouchNum;

    GetKartCtrl()->getKartSound(num)->DoSlipSound(num);

    // TODO: what kind of vectors are this?
    JGeometry::TVec3f _vec0;
    _vec0.set(body->mPlayerPosMtx[0][3],
                            body->mPlayerPosMtx[1][3],
                            body->mPlayerPosMtx[2][3]);

    body->mShadowArea.searchShadow(_vec0);

    if(body->mTouchNum != 0 && body->_58c == 7) {
        // TODO: is getObject return type wrong or is the cast here ok? first one probably
        ExGeographyObj *exGeo = static_cast<ExGeographyObj*>(body->mBodyGround.getObject());

        JGeometry::TVec3f _vec2;
        _vec2.set(0.f, -3.5f, 0.f);

        exGeo->AddVel(_vec0, _vec2);
    }
}

void KartGame::WatchEffectAcceleration() {
    KartBody *body = mBody;
    int num = body->mMynum;
    
    const KartGamePad *gamePad = GetKartCtrl()->GetDriveCont(num);
    const KartPad *pad = GetKartCtrl()->getKartPad(num);

    if (gamePad->testButton(pad->mAccelBtn)) {
        body->mKartRPM = GetKartCtrl()->fcnvge(body->mKartRPM, 1.f, 0.050000001f , 0.050000001f);
    } else {
        body->mKartRPM = GetKartCtrl()->fcnvge(body->mKartRPM, 0.f, 0.050000001f , 0.050000001f);
    }
}

void KartGame::WatchAcceleration() {
    KartBody *body = mBody;
    int num = body->mMynum;

    const KartGamePad *gamePad = GetKartCtrl()->GetDriveCont(num);
    if ((body->mCarStatus & 0x400000) && (body->getRescue()->mState >= 3)) {
        KartPad *pad = GetKartCtrl()->getKartPad(num);
        if (gamePad->testButton(pad->mAccelBtn)) {
            body->_3c8 = GetKartCtrl()->fcnvge(body->_3c8, body->_3d0, 1.f , 1.f);
            _8 |= 2;
        } else {
            GetKartCtrl()->ChaseFnumber(&body->_3c8, 0.f, 1.f);
            _8 &= ~2;
        }
    }
}

void KartGame::DoItmCancel() {
    KartBody *body = mBody;

    body->mCarStatus |= 0x80000000;
    GetItemObjMgr()->abortItemShuffle(body->mMynum);
}

void KartGame::DoStopItm() {
    KartBody *body = mBody;
    u8 num = body->mMynum;

    body->mCarStatus |= 0x80000000;
    GetItemObjMgr()->abortItemShuffle(body->mMynum);

    ItemObjMgr *itemMgr = GetItemObjMgr();
    ItemObj *item = itemMgr->getKartEquipItem(num, 0);
    itemMgr->deleteHeartItem(num);

    if (item != nullptr) {
        item->IsSuccessionItem()
            ? item->setChildStateForceDisappear()
            : item->setStateForceDisappear();
    }

    item = itemMgr->getKartEquipItem(num, 1);
    if (item != nullptr) {
        item->IsSuccessionItem()
            ? item->setChildStateForceDisappear()
            : item->setStateForceDisappear();
    }
}

void KartGame::DoChange() {
    KartBody *body = mBody;
    int num = body->mMynum;

    const KartGamePad *gpDriver = GetKartCtrl()->GetDriveCont(num);
    const KartGamePad *gpCoDriv = GetKartCtrl()->GetCoDriveCont(num);
    
    bool change = false;
    const bool possible = GetKartCtrl()->MakeChangePossible(num);

    if (body->getChecker()->CheckCheange(num)) {
        mTimeToChange = 0;
        return;
    }

    if (mTimeToChange)
        mTimeToChange--;

    // single player kart
    if ((body->mGameStatus & KartBody::HasCoDriver) == false) {
        if (gpDriver->testTrigger(GetKartCtrl()->getKartPad(num)->mTrigZ))
            mTimeToChange = 20;

        if (body->mCarStatus & KartBody::InDriverChange) {
            if (!(body->mExModels[0])->IsChange() && !(body->mExModels[1])->IsChange())
                body->mCarStatus &= ~KartBody::InDriverChange;

        } else if ((gpDriver->testTrigger(GetKartCtrl()->getKartPad(num)->mTrigZ)
                    || possible || mTimeToChange != 0)) {
            change = true;
            mTimeToChange = 0;
        }
    // with co-driver, but already in change
    } else if (body->mCarStatus & KartBody::InDriverChange) {
        if (gpDriver->testButton(GetKartCtrl()->getKartPad(num)->mTrigZ) &&
            gpCoDriv->testButton(GetKartCtrl()->getKartPad(num)->mTrigZ)) {
            mTimeToChange = 20;
        }

        if (!(body->mExModels[0])->IsChange() &&
            !(body->mExModels[1])->IsChange()) {
            body->mCarStatus &= ~KartBody::InDriverChange;
        }
    // with co-driver and ready to change
    } else if ((gpDriver->testButton(GetKartCtrl()->getKartPad(num)->mTrigZ) &&
                gpCoDriv->testButton(GetKartCtrl()->getKartPad(num)->mTrigZ)) ||
                mTimeToChange != 0) {
        change = true;
        mTimeToChange = 0;
    }

    if (!change)
        return;

    body->mCarStatus &= ~(1ull<<38);
    body->mCarStatus |= KartBody::InDriverChange;;

    GetKartCtrl()->getKartAnime(num)->mFlags |= 1;
    GetKartCtrl()->getKartSound(num)->DoChangeVoice();
    GetKartCtrl()->getKartSound(num)->DoChangeStarSound();
}

void KartGame::DoSlide() {
    const int num = mBody->mMynum;
    KartBody *body = mBody;

    if (body->getChecker()->CheckPartsClearKey(num) || !(body->mGameStatus & KartBody::HasCoDriver)) {
        return;
    }

    if (body->mSlideTimer != 0) {
        body->mSlideTimer--;
    }

    if (body->mSlideTimer == 0) {
        body->mCarStatus &= ~KartBody::DoesSlide;
    }

    if ((body->mCarStatus & KartBody::DoesSlide)) {
        if (body->mSlideTimer >= 29) {
            body->_2cc.x += body->_2f0.x * body->_528;
            body->_2cc.y += body->_2f0.y * body->_528;
            body->_2cc.z += body->_2f0.z * body->_528;
        }

        return;
    }

    KartGamePad *gpCoDriv = GetKartCtrl()->GetCoDriveCont(num);
    if (body->getTouchNum() == 0 || (GetKartCtrl()->GetCarSpeed(num) <= 50.f)) {
        return;
    }

    if (!gpCoDriv->testTrigger(JUTGamePad::L) && !gpCoDriv->testTrigger(JUTGamePad::R)) {
        return;
    }

    body->mSlideTimer = 35;

    if (RaceMgr::getCurrentManager()->isMirror()) {
		if (gpCoDriv->testTrigger(JUTGamePad::L)) {
			body->_528 = -50.f * body->_3a4;
			GetKartCtrl()->getKartAnime(num)->mFlags |= 0x10;
			JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown1b, num, body->mPos, 1);
		} else if (gpCoDriv->testTrigger(JUTGamePad::R)) {
			body->_528 = 50.f * body->_3a4;
			GetKartCtrl()->getKartAnime(num)->mFlags |= 0x20;
            JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown1b, num, body->mPos, 0);
		}
	} else {
		if (gpCoDriv->testTrigger(JUTGamePad::R)) {
			body->_528 = -50.f * body->_3a4;
			GetKartCtrl()->getKartAnime(num)->mFlags |= 0x10;
			JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown1b, num, body->mPos, 1);
		} else if (gpCoDriv->testTrigger(JUTGamePad::L)) {
			body->_528 = 50.f * body->_3a4;
			GetKartCtrl()->getKartAnime(num)->mFlags |= 0x20;
			JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown1b, num, body->mPos, 0);
		}
	}

	body->_2cc.x += body->_2f0.x * body->_528;
	body->_2cc.y += body->_2f0.y * body->_528;
	body->_2cc.z += body->_2f0.z * body->_528;

	body->mCarStatus |= KartBody::DoesSlide;
	GetKartCtrl()->getKartSound(num)->DoStrikeSound();
	GetKartCtrl()->getKartSound(num)->DoTandemVoice(2);
}

void KartGame::DoDriftTurboSterr() {
    KartBody *body = mBody;

    if (body->mDriftSterr != 0 && body->mDriftSterr < 0x1E)
        body->mDriftSterr++;
}

void KartGame::SetDriftTurboSterr() {
    KartBody *body = mBody;
    const int num = mBody->mMynum;

    const int threshold = (body->mGameStatus & KartBody::HasCoDriver) ? 2 : 6;
    if ((body->mDriftSterr) < threshold)
        return;

    body->mMTState++;

    if (body->mMTState == 1)
            GetKartCtrl()->getKartSound(num)->DoKartsetSeSound(0x100C9);
    else if (body->mMTState == 2)
            GetKartCtrl()->getKartSound(num)->DoKartsetSeSound(0x100CA);

    if (body->mMTState >= 2)
        body->mMTState = 2;

    body->mDriftSterr = 0;
}

void KartGame::CheckDriftTurbo(void) {
	KartBody *body = mBody;
	int num = body->mMynum;

	if (GetKartCtrl()->GetCarSpeed(num) <= 50.f) {
		body->mCarStatus &= ~(1ull<<41);
        body->mMTBoost = 0;
		body->mDriftSterr = 0;
		body->mMTState = 0;
		return;
	}

	if (body->mGameStatus & KartBody::GsUnknown3)
		return;

	bool didDriftSterr = false;
	KartGamePad *gpCoDriv = GetKartCtrl()->GetCoDriveCont(num);

	if (body->mCarStatus & KartBody::CsUnknown0) {
		if (body->mGameStatus & KartBody::HasCoDriver) {
			if (gpCoDriv->getMainStickX() < 0.5f) {
				didDriftSterr = true;
				DoDriftTurboSterr();
			}
		} else if (body->mFrame >= -0.5f) {
            didDriftSterr = true;
            DoDriftTurboSterr();
		}

		if (!didDriftSterr) {
			SetDriftTurboSterr();
			body->mDriftSterr = 1;
		}
	} else {
		if ((body->mCarStatus & KartBody::CsUnknown1) != 0) {
			if (body->mGameStatus & KartBody::HasCoDriver) {
				if (gpCoDriv->getMainStickX() > -0.30000001f) {
					didDriftSterr = true;
					DoDriftTurboSterr();
				}
			} else {
				if (body->mFrame <= 0.5f) {
					didDriftSterr = true;
					DoDriftTurboSterr();
				}
			}

			if (!didDriftSterr) {
				SetDriftTurboSterr();
				body->mDriftSterr = 1;
			}
        } else {
            body->mMTState = 0;
            body->mDriftSterr = 0;
        }
    }

	body->mCarStatus &= ~(1ull<<41);
	body->mMTBoost = 0;
}

void KartGame::DoWarmUpRoll() {}

void KartGame::DoRollAnim() {}

void KartGame::DoDriftClear() {}

void KartGame::DoRoll() {}

void KartGame::DoTestPitch() {
    // void JUTGamePad::getMainStickY() const {}
}

void KartGame::DoLiftTurbo() {}

void KartGame::DoTurbo() {}

void KartGame::DoRollThrow() {}

void KartGame::DoRollOver() {}

void KartGame::DoWanWan() {
    // void ItemWanWanObj::getDifVel() const {}
    // void ItemWanWanObj::getPullVec(JGeometry::TVec3<float> *) {}
}

void KartGame::DoPushStart() {}

void KartGame::DoBalance(float *, float) {}

void KartGame::MakeClear() {}

void KartGame::MakeBoardDash() {}

void KartGame::MakeJumpDash() {}

void KartGame::MakeSpJumpDash() {}

void KartGame::MakeMashDash() {}

void KartGame::MakeGoldenMashDash() {}

void KartGame::MakeStartDash() {}

void KartGame::MakeCrashDash() {}

void KartGame::MakeWheelSpin() {}

void KartGame::MakeJump() {}

void KartGame::DoAirCheck() {}

void KartGame::DoRearSlidePower() {}

void KartGame::DoRearSlideBody() {
    // void JGeometry::TVec3<float>::div(float) {}
}

void KartGame::DoCorner() {}

void KartGame::FrameWork(float, KartSus *, KartSus *) {}

void KartGame::DoBodyAction() {}

void KartGame::DoElementForce() {}

bool KartGame::CheckBalloon() {}

void KartGame::SetRank() {}

void KartGame::RankWatchMan() {}

void KartGame::ItemWatchMan(ItemObj *) {}

void KartGame::AfterItemWatchMan() {}

void KartGame::DoFlagCtrl() {}

void KartGame::KeepWatch() {}

void KartGame::DoActionMgr() {
    // void KartBody::getStar() {}
    // void ItemObjMgr::getKartHitList(int) {}
}

void KartGame::DoActionCtrl() {}

void KartGame::DoStatus() {
    // void KartCtrl::DoAnime(int) {}
}
