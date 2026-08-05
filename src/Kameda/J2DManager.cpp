#include "Kameda/J2DManager.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/JUtility/TColor.h"
#include "Kameda/Demo2D.h"
#include "Kameda/Go2D.h"
#include "Kameda/Go3212D.h"
#include "Kameda/Goal2D.h"
#include "Kameda/LANNum2D.h"
#include "Kameda/LANResult2D.h"
#include "Kameda/PauseManager.h"
#include "Kameda/PreRace2D.h"
#include "Kameda/Race2D.h"
#include "Kameda/WipeManager.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/NetGameMgr.h"
#include "Osako/ResMgr.h"
#include "Osako/kartPad.h"
#include "Osako/system.h"
#include "kartEnums.h"
#include "mathHelper.h"
#include "types.h"

char *J2DManager::mKartNumberName[8] = {
    "MarioFont_128.1.bti",
    "MarioFont_128.2.bti",
    "MarioFont_128.3.bti",
    "MarioFont_128.4.bti",
    "MarioFont_128.5.bti",
    "MarioFont_128.6.bti",
    "MarioFont_128.7.bti",
    "MarioFont_128.8.bti",
};

int J2DManager::mKart2Status[8];
int J2DManager::mStatus2Kart[4];

J2DManager *J2DManager::mThis;
bool J2DManager::mNetFlag;

J2DManager::J2DManager(JKRHeap *heap) {
    if (heap == nullptr) {
        heap = JKRGetCurrentHeap();
    }
    mThis = this;

    mArchive = JKRMountArchive(ResMgr::getPtr(ResMgr::mcArcMRAM, "Race2D.arc"), heap, JKRArchive::MOUNT_DIRECTION_HEAD);
    mOrtho = System::getJ2DOrtho();
    mDrawFlag = true;
    mInit = true;
    _a = true;
    
    mKart2Status[0] = -1;
    mKart2Status[1] = -1;
    mKart2Status[2] = -1;
    mKart2Status[3] = -1;
    mKart2Status[4] = -1;
    mKart2Status[5] = -1;
    mKart2Status[6] = -1;
    mKart2Status[7] = -1;
    mStatus2Kart[0] = -1;
    mStatus2Kart[1] = -1;
    mStatus2Kart[2] = -1;
    mStatus2Kart[3] = -1;
    mNetFlag = NetGameMgr::ptr()->getState() != 0;

    for (int i = 0; i < 8; i++) {
        mWinnerAnmFlag[i] = false;
        mLoserAnmFlag[i] = false;
    }

    mDuration = 10000;
    mWipeMgr = new (heap, 0) WipeManager(heap);
    mDemo = new (heap, 0) Demo2D(heap);
    mPreRace = new (heap, 0) PreRace2D(heap);
    mGo321 = new (heap, 0) Go3212D(heap);
    mGo = new (heap, 0) Go2D(heap);
    mRace = new (heap, 0) Race2D(heap);
    mGoal = new (heap, 0) Goal2D(heap);
    mLANNumAnm = new (heap, 0) LANNum2D(heap);
    mLANResultAnm = new (heap, 0) LANResult2D(heap);
}

void J2DManager::reset() {
    mDemo->reset();
    mGo321->reset();
    mGo->reset();
    mGoal->reset();
    mRace->reset();
    mLANNumAnm->reset();
    mLANResultAnm->reset();
    mWipeMgr->reset();
    mDuration = 10000;

    if (_a) {
        if (RCMGetManager()->isAwardDemoMode()) {
            
            WipeManager::getManager()->setWipe(getStatus2Kart(0), 1, 0, 40, 0, JUTColor(0xc0, 0xc0, 0xc0, 0xff));
            mDuration = 40;
        }
        else if (RCMGetManager()->getStatusNumber() < 2) {
            WipeManager::getManager()->setWipe(getStatus2Kart(0), 7, 0, 45, 0, JUTColor(0, 0, 0, 0xff));
            mDuration = 45;
        }
        else {
            for (int i = 0; i < RCMGetManager()->getStatusNumber(); i++) {
                WipeManager::getManager()->setWipe(getStatus2Kart(i), 1, 0, 40, 0, JUTColor(0, 0, 0, 0xff));
                mDuration = 40;
            }
        }
        _a = false;
    }

    mAnmFrame = 0;
    for (int i = 0; i < 8; i++) {
        mWinnerAnmFlag[i] = false;
        mLoserAnmFlag[i] = false;
    }
}

void J2DManager::draw() {
    mDemo->draw();
    if (!mDrawFlag) {
        mWipeMgr->draw();
        return;
    }

    if (!RCMGetManager()->isAbleStart()) {
        mPreRace->draw();
    }

    if (!isLANDemo()) {
        mRace->drawPlayerMark();
        mRace->drawMask();
        mRace->draw();
        mWipeMgr->draw();
        mRace->drawCourse();
        mGo321->draw();
     
        if (RCMGetManager()->isAbleStart()) {
            mGo->draw();
        }
        mGoal->drawGoal();
        mGoal->drawRetire();
        mLANNumAnm->draw();
        mLANResultAnm->draw();
    }
}

void J2DManager::calc() {
    mDemo->calc();

    if (!mInit) return;

    if (!PauseManager::getManager()->isResultStart() && PauseManager::tstPause()) return;
    
    if (RCMGetManager()->isRaceEnd()) {
        mAnmFrame++;
        if (RCMGetManager()->isRaceModeGp()) {
            if ((mAnmFrame > 333 && mAnmFrame < 333) && gpaGamePad[0]->testTrigger(KartGamePad::A)) {
                mAnmFrame = 333;
            }
        }
        if (RCMGetManager()->isRaceModeTA()) {
            if (mAnmFrame > 333 && mAnmFrame < 333) {
                mAnmFrame = 333;
            }
        }
    }

    if (!RCMGetManager()->isAbleStart()) {
        mPreRace->calc();
    }

    mGoal->calc();
    mRace->calc();

    if (RCMGetManager()->isJugemCountStart()) {
        mGo321->start();
    }
    mGo321->calc();

    if (RCMGetManager()->isAbleStart()) {
        mGo->calc();
    }

    mLANNumAnm->calc();
    mLANResultAnm->calc();
    mWipeMgr->calc();

    if (--mDuration < 0) {
        mDuration = 0;
    }
}

void J2DManager::calcParticle() {
    if (!mInit) {
        return;
    }

    if (PauseManager::getManager()->isResultStart() || !PauseManager::tstPause()) {
        mRace->calcParticle();
    }
}

int J2DManager::getKart2Status(int kart) {
#line 303
    JUT_MINMAX_ASSERT(0, kart, 8);
    return mKart2Status[kart];
}

int J2DManager::getStatus2Kart(int status) {
#line 316
    JUT_MINMAX_ASSERT(0, status, 4);
    return mStatus2Kart[status];
}

void J2DManager::setStatus2Kart(int status, int kart) {
#line 330
    JUT_MINMAX_ASSERT(0, kart, 8);
    JUT_MINMAX_ASSERT(0, status, 4);
    mStatus2Kart[status] = kart;

    for (int i = 0; i < 8; i++) {
        mKart2Status[i] = -1;
    }

    for (int i = 3; i >= 0; i--) {
        if (mStatus2Kart[i] == -1)
            continue; 
        JUT_MINMAX_ASSERT(0, mStatus2Kart[i], 8);
        mKart2Status[mStatus2Kart[i]] = i;
    }

}

void J2DManager::setUsisi(int kart, ECharID character) {
#line 357
    JUT_MINMAX_ASSERT(0, kart, 8);
    JUT_MINMAX_ASSERT(1, character, cCharIDMax);
    mRace->setUsisi(kart, (ECharID)(character - 1));
}

void J2DManager::startPreRace() {
    mPreRace->start();
}

int J2DManager::getPreRaceStatus() {
    return mPreRace->getStatus();
}

int J2DManager::getPreRaceWipeType() {
    return mPreRace->getWipeType();
}

void J2DManager::setRace2DDrawFlag(bool enable) {
    mGo->setDrawFlag(enable);
    mRace->setDrawFlag(enable);
}

void J2DManager::hideRace2D() {
    mRace->hide();
}

int J2DManager::getHideFrameRace2D() {
    return mRace->getHideFrame();
}

bool J2DManager::isHideRace2D() {
    return getHideFrameRace2D() >= 20;
}

bool J2DManager::isRaceEnd() {
    switch (RCMGetManager()->getRaceMode()) {
    case TIME_ATTACK:
    case GRAND_PRIX:
    case VERSUS_RACE: {
        return mAnmFrame > 353;
    default:
        return mAnmFrame > 128;
    }
    }
}

bool J2DManager::isGoalAnmEnd(int status) {
    int goalAnmFrame = mGoal->getAnmFrame(status);
    bool ret = (goalAnmFrame >= 128);
    if (ret) {
        switch (RCMGetManager()->getRaceMode()) {
        case TIME_ATTACK:
        case GRAND_PRIX:
        case VERSUS_RACE: {
            ret = mAnmFrame > 353;
            break;
        default:
            ret = mAnmFrame > 128;
            break;
        }
        }
    }

    return ret;
}

bool J2DManager::isMiniGameAnmEnd(int status) {
    // UNUSED
}

int J2DManager::getGoalAnmFrame(int status) {
    return mGoal->getAnmFrame(status);
}

bool J2DManager::isAlarm(int kart) {
    return mRace->isAlarm(kart);
}

void J2DManager::startLANNumAnm() {
    mLANNumAnm->start();
}

void J2DManager::startLANNumResultAnm() {
    mLANNumAnm->start2();
}

void J2DManager::startLANResultAnm(int kart) {
    mLANResultAnm->start(kart);
}

void J2DManager::startBombDemo() {
    mRace->startBombDemo();
}

void J2DManager::endBombDemo() {
    mRace->endBombDemo();
}

bool J2DManager::isLANDemo() {
    return mNetFlag && RCMGetManager()->isDemoCamera(0);
}

bool J2DManager::isStartGo321() {
    return mGo321->isStart();
}

#include "JSystem/JAudio/JASFakeMatch2.h"
