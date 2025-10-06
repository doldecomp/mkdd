#include "Kaneshige/RaceMgr.h"
#include "Sato/ItemObjMgr.h"
#include "Yamamoto/KartGame.h"
#include "Yamamoto/kartCtrl.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

// comments inside functions are inline functions being called in that function

bool KartGame::Init(int kartNo) {
    bool isRight;

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

    _A[0] = 0;
    _A[1] = 0;
    _A[2] = 0;

    isRight = RaceMgr::getCurrentManager()->getStartPoint(&_20, &_2C, kartNo);

    _20.y += 300.f;

    return isRight;
}

void KartGame::GetGorundTireNum() {
    u8 num;
    int idx;
    KartBody *body;
    KartSus *sus[4];
    ExGeographyObj *exGeo;

    // lgtm
    num = mBody->mMynum;

    body = mBody;
    body->mTouchNum = 0;


    // the compiler will optimize all of these redundant calls out, right?... right????
    idx = num;
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

    JGeometry::TVec3f _geo0;
    _geo0.set(body->mPlayerPosMtx[0][3],
                            body->mPlayerPosMtx[1][3],
                            body->mPlayerPosMtx[2][3]);

    body->mShadowArea.searchShadow(_geo0);

    if(body->mTouchNum != 0 && body->_58c == 7) {
        exGeo = (ExGeographyObj*)body->mBodyGround.getObject();

        JGeometry::TVec3f _geo2;
        _geo2.set(0.f, -3.5f, 0.f);

        exGeo->AddVel(_geo0, _geo2);
    }
}

void KartGame::WatchEffectAcceleration() {
    int num;
    KartPad *pad;
    KartBody *body;
    KartGamePad *gamePad;

    num = mBody->mMynum;
    body = mBody;

    gamePad = GetKartCtrl()->GetDriveCont(num);
    pad = GetKartCtrl()->getKartPad(num);
    if (gamePad->testButton(pad->mAccelBtn)) {
        body->mKartRPM = GetKartCtrl()->fcnvge(body->mKartRPM, 1.f, 0.050000001f , 0.050000001f);
    } else {
        body->mKartRPM = GetKartCtrl()->fcnvge(body->mKartRPM, 0.f, 0.050000001f , 0.050000001f);
    }
}

void KartGame::WatchAcceleration() {
    int num;
    KartPad *pad;
    KartBody *body;
    KartGamePad *gamePad;

    num = mBody->mMynum;
    body = mBody;

    gamePad = GetKartCtrl()->GetDriveCont(num);
    if ((body->mCarStatus & 0x400000) && (body->getRescue()->_6c[10] >= 3)) {
        pad = GetKartCtrl()->getKartPad(num);
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
    KartBody *body;

    body = mBody;

    body->mCarStatus |= 0x80000000;
    GetItemObjMgr()->abortItemShuffle(body->mMynum);
}

void KartGame::DoStopItm() {
    u8 num;
    KartBody *body;
    ItemObjMgr *itemMgr;
    ItemObj *item;

    num = mBody->mMynum;
    body = mBody;

    /* probably copy pasted this from DoItmCancel lmao */
    body->mCarStatus |= 0x80000000;
    GetItemObjMgr()->abortItemShuffle(body->mMynum);

    itemMgr = GetItemObjMgr();
    item = itemMgr->getKartEquipItem(num, 0);
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

void KartGame::DoChange() {}

void KartGame::DoSlide() {}

void KartGame::DoDriftTurboSterr() {}

void KartGame::SetDriftTurboSterr() {}

void KartGame::CheckDriftTurbo() {
    // void JUTGamePad::getMainStickX() const {}
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
