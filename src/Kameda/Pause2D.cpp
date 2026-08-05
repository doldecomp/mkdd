#include "Kameda/Pause2D.h"
#include "mathHelper.h"

int Pause2D::mPauseSelector;
Pause2D::PauseStatus Pause2D::mStatus;

Pause2D::Pause2D(JKRHeap *) {}

void Pause2D::init() {}

void Pause2D::draw() {}

void Pause2D::calc(KartGamePad *) {}

void Pause2D::selectorGP(KartGamePad *) {}

void Pause2D::selectorVS(KartGamePad *) {}

void Pause2D::selectorTA(KartGamePad *) {}

void Pause2D::selectorMG(KartGamePad *) {}

void Pause2D::selectorCommon() {}

void Pause2D::setDrawGP() {}

void Pause2D::setDrawVS() {}

void Pause2D::setDrawTA() {}

void Pause2D::setDrawMG() {}

void Pause2D::setDrawCommon() {}

void Pause2D::getScale(int) {}

int Pause2D::getPauseSelector() {
    return mPauseSelector;
}

Pause2D::PauseStatus Pause2D::getPauseStatus() {
    return mStatus;
}

void Pause2D::setPauseStatus(Pause2D::PauseStatus status) {
    mStatus = status;
}

void Pause2D::setPadText() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
