#include "Inagaki/GameAudioMain.h"
#include "Kaneshige/RaceMgr.h"
#include "Shiraiwa/JugemMain.h"

const Vec TJugem::scReversePoints0_1p[5] = {
    { -400.0f,  scJugemHeight, -1050.0f },
    { -300.0f,  50.0f,         -250.0f },
    {  -50.0f,  50.0f,         -150.0f },
    {  -25.0f,  50.0f,         -150.0f },
    {    0.0f,  50.0f,         -350.0f }
};

const Vec TJugem::scReversePoints0_multi[5] = {
    { -400.0f,   scJugemHeight, -1050.0f },
    { -300.0f, 100.0f,          -250.0f },
    {  -50.0f, 100.0f,          -150.0f },
    {  -25.0f, 100.0f,          -150.0f },
    {    0.0f,  90.0f,          -250.0f }
};

const Vec TJugem::scReversePoints1_multi[6] = {
    { 0.0f, 100.0f, -250.0f },
    { 0.0f,  50.0f, -450.0f },
    { 0.0f, 120.0f, -250.0f },
    { 0.0f,  70.0f, -150.0f },
    { 0.0f,  50.0f,  150.0f },
    { 0.0f,  70.0f,  -50.0f },
};

const Vec TJugem::scReversePoints2[4] = {
    { 0.0f, 0.0f,          0.0f },
    { 0.0f, 0.0f,          0.0f },
    { 0.0f, -450.0f,       0.0f },
    { 0.0f, scEraseHeight, 0.0f }
};

s16 TJugem::scReverseJudgeTime = 90;
s16 TJugem::scReviveJudgeTime  = 30;
f32 TJugem::sReverseEnterSpeed = 0.02f;
f32 TJugem::sReverseStaySpeed  = 0.0099999998f;
f32 TJugem::sReverseLeaveSpeed = 0.1f;

void TJugem::initFunc_Reverse() {
    wearCap(1);
    hold(1, 4);
    show(0);
    changeAnimation(3);
    switch (getScreenType()) {
        case 0:
            mSplineInterp.setPoint(5, &scReversePoints0_1p->x, sReverseEnterSpeed, false);
            break;
        case 1:
        case 2:
            mSplineInterp.setPoint(5, &scReversePoints0_multi->x, sReverseEnterSpeed, false);
            break;
        default:
            break;
    }
    _278 = 0;
    _276 = 0;
    resetJugemOriginNumber();
}

void TJugem::doFunc_Reverse() {
    move(mKartNum);
    GetGameAudioMain()->startKartSystemSe((u8)mKartNum, 0x1000a);
    if (!mSplineInterp.checkUnknownBool13()) {
        switch (getScreenType()) {
            case 0:
                break;
            case 1:
            case 2:
                mSplineInterp.setPoint(6, &scReversePoints1_multi->x, sReverseStaySpeed, true);
                break;
        }
    }
    if (checkReviveProc() != false) {
        setState(6);
    }
}

void TJugem::initFunc_RevEnd() {
    mSplineInterp.setPointOffset(4, &scReversePoints2->x, sReverseLeaveSpeed, false);
}

void TJugem::doFunc_RevEnd() {
    move(getKartNum());
    if (mSplineInterp.checkUnknownBool13() == false) {
        setState(1);
    }
}

bool TJugem::checkReverseProc() {
    if (RaceMgr::getCurrentManager()->getKartChecker(getKartNum())->isReverse()) {
        _276++;
    } else {
        _276 = 0;
    }
    return (_276 > scReverseJudgeTime);
}

bool TJugem::checkReviveProc() {
    if (!RaceMgr::getCurrentManager()->getKartChecker(getKartNum())->isReverse()) {
        _278++;
    } else {
        _278 = 0;
    }
    return (_278 > scReviveJudgeTime);
}


#include "JSystem/JAudio/JASFakeMatch2.h"
