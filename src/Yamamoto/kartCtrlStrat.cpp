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

void KartGame::GetGorundTireNum() {}

void KartGame::WatchEffectAcceleration() {}

void KartGame::WatchAcceleration() {}

void KartGame::DoItmCancel() {
    KartBody *body;

    body = mBody;

    body->mCarStatus |= 0x80000000;
    GetItemObjMgr()->abortItemShuffle(body->mMynum);
}

void KartGame::DoStopItm() {}

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
