#include "Yamamoto/KartCrash.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

// comments inside functions are inline functions being called in that function

void KartCrash::Init(int) {}

void KartCrash::DoRabbitMark() {}

void KartCrash::DoMatchlessTimer() {
    // void KartDrawer::flashOff() {}
    // void KartDrawer::flashOn() {}
}

bool KartCrash::CheckMatchlessTimer() {}

void KartCrash::SetMatchlessTimer() {}

void KartCrash::ClearMatchlessTimer() {}

void KartCrash::DoDecBalloon() {}

void KartCrash::DoPointBomb(ItemObj *) {
    // void KartChecker::setBEHitBomb(ItemObj *) {}
}

void KartCrash::DoDecPointBomb() {
    // void KartChecker::setBEJugem() {}
}

void KartCrash::SaveDir() {}

void KartCrash::NonRescue() {
    // void CrsArea::searchInvValley(const JGeometry::TVec3<float> &) {}
}

void KartCrash::DoRollCrashCrl() {}

void KartCrash::DoPitchCrashCrl() {}

void KartCrash::MakeBombCrash(ItemObj *) {}

void KartCrash::DoBombCrashCrl() {}

void KartCrash::MakeKameCrash(ItemObj *) {}

void KartCrash::MakeTornado() {}

void KartCrash::DoTornadeCenter() {}

void KartCrash::DotornadeCrashCrl() {}

void KartCrash::MakeSpin(ItemObj *) {}

void KartCrash::DoSpinCrashCrl() {}

void KartCrash::MakeHalfSpin(ItemObj *) {}

void KartCrash::DoHalfSpinCrashCrl() {}

void KartCrash::MakeThunderSpin() {}

void KartCrash::MakeBurn(ItemObj *) { 
    // void ItemFireBall::IsEfctTypeRed() const {}
    // void ItemObj::getItemColorID() const {}
}

void KartCrash::MakeFreezeCrash() {}

void KartCrash::DoFreezeCrashCrl() {}
