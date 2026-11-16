#include "Inagaki/GameAudioMain.h"
#include "Kaneshige/RaceMgr.h"
#include "Kameda/J2DManager.h"
#include "Sato/GeographyObjMgr.h"
#include "Yamamoto/KartMat.h"
#include "Yamamoto/KartSound.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

void KartSound::Init(int idx)
{
    KartBody *body = GetKartCtrl()->getKartBody(idx);
    mBody = body;
    mWallSoundActive = false;
    mIsAlarmActive = false;
    u32 status = body->mGameStatus;
    if (status & 2 && !(status & 0x10) && status & 0x1000)
    {
        mSoundMgr = new GameAudio::KartSoundMgr((Vec *)&body->mPos, nullptr, idx, 0);
    }
    else if ((status & 0x10))
    {
        mSoundMgr = new GameAudio::KartSoundMgr((Vec *)&body->mPos, nullptr, idx, 2);
    }
    else
    {
        mSoundMgr = new GameAudio::KartSoundMgr((Vec *)&body->mPos, nullptr, idx, 1);
    }
}

void KartSound::DoInfo(int idx)
{
    KartBody *body = GetKartCtrl()->getKartBody(idx);
    TJugem *jugem = GetGeoObjMgr()->getJugem(0);
    KartGamePad *coPad = GetKartCtrl()->GetDriveCont(idx);
    
    bool holdA = false;
    if(coPad->testButton(GetKartCtrl()->getKartPad(idx)->mAccelBtn))
        holdA = true;

    bool holdB = false;
    if (coPad->testButton(GetKartCtrl()->getKartPad(idx)->mBtnB))
        holdB = true;
    
    bool isBrake = false;
    if(body->_5c3 & 2)
        isBrake = true;

    u8 signalthing = 0;
    if(jugem->getSignalState() == 5) {
        signalthing = 0;
    }
    else if(jugem->getSignalState() == 3) {
        signalthing = 0;
    }
    else if (jugem->getSignalState() == 1)
    {
        signalthing = 0;
    }
    else if (body->mGameStatus & 4)
    {
        if (body->mCarStatus & (0x100000000 | 0x400000))
        {
            signalthing = 2;
        }
        else if (body->mGameStatus & 0x20)
        {
            signalthing = 3;
        }
        else if (body->mCarStatus & 0x200000000)
        {
            signalthing = 5;
        }
        else if (body->mCarStatus & 0x100000)
            signalthing = 4;
        else
        {
            signalthing = 1;
        }
    }

    for (u8 i = 0; i < 4; i++)
    {
        KartSus *sus = GetKartCtrl()->getKartSus(i + idx * 4);
        if (sus->mCrsGnd.getAttribute() == 0x11)
        {
            mSoundMgr->setWaterDepth(i, sus->mCrsGnd.getWaterHeight() - sus->mForcePos.y);
        }
    }

    f32 rpm = GetKartCtrl()->GetCarRpm(idx);
    if (body->mGameStatus & 0x2000)
    {
        mSoundMgr->setConductStatus(rpm, GetKartCtrl()->GetCarSpeed(body->mMynum), holdA, holdB, isBrake, signalthing, &body->mRoofArea);
    }
}

void KartSound::DoFinishSound(int idx)
{
    mSoundMgr->frameWork(RCMGetKartChecker(GetKartCtrl()->getKartBody(idx)->mMynum)->getRank());
}

void KartSound::DoSlipSound(int idx)
{
    KartBody *body = GetKartCtrl()->getKartBody(idx);
    RaceMgr::getCurrentManager();

    body->mCarStatus &= ~0x2000000000;
    GetKartCtrl()->DevMatrixByVector((Vec *)&body->mTireG, (Vec *)&body->mVel, body->_110);

    if (!body->getChecker()->CheckPlayer())
        return;

    f32 x = body->mTireG.x;
    if (x < 0.0f)
    {
        x = -x;
    }

    f32 dir = (x - 5.0f) / 80.0f;
    if (dir > 1.0f)
    {
        dir = 1.0f;
    }
    if (dir >= 0.3f)
    {
        body->mCarStatus |= 0x2000000000;
    }

    for (u8 i = 0; i < 4; i++)
    {
        KartSus *sus = GetKartCtrl()->getKartSus(i + idx * 4);
        if (sus->mSusFlags & 1)
        {
            mSoundMgr->setSlip(i, sus->mCrsGnd.getAttribute(), sus->mCrsGnd.getMaterial(), dir);
        }
    }
}

void KartSound::DoWallHitSound(f32 vol, CrsGround::EMat mat)
{
    mSoundMgr->setCrushSe(mat, vol);
}

void KartSound::DoHorn()
{
    KartBody *body = mBody;
    KartChecker *kartChecker = RCMGetKartChecker(body->mMynum); // sure, let's call this and not use it

    u32 soundIDs[] = {
        0x10083,
        0x10084,
        0x10085,
        0x10086,
        0x10087,
        0x10088,
        0x10089,
        0x1008A,
        0x1008B,
        0x1008C,
        0x1008D,
        0x1008E,
        0x1008F,
        0x10090,
        0x10091,
        0x10092,
        0x10093,
        0x10094,
        0x10095,
        0x10097,
        0x10098,
        0x10099,
    };

    if (body->getGame()->mCountDownDuration != 0)
        return;

    KartGamePad *coPad = GetKartCtrl()->GetCoDriveCont(body->mMynum);

    u32 hornMask = GetKartCtrl()->getKartPad(body->mMynum)->mItemMask;
    if (body->mGameStatus & 1)
        hornMask = GetKartCtrl()->getKartPad(body->mMynum)->mFaceBtnsMask;

    if (!coPad->testTrigger(hornMask))
        return;

    if (body->getChecker()->CheckOnlyTandemPartsClearKey(body->mMynum))
        return;

    if (GetKartCtrl()->CheckTandemItmGet(body->mMynum))
        return;

    if (GetKartCtrl()->CheckTandemItmRollingGet(body->mMynum))
        return;

    if (GetKartCtrl()->getKartAnime(body->mMynum)->IsAppeal(body->mMynum))
        return;

    GetKartCtrl()->getKartAnime(body->mMynum)->mFlags |= 0x8000000000;
    mSoundMgr->setSe(soundIDs[body->mIdx]);
}

void KartSound::DoKartHitSound(f32 vol)
{
    mSoundMgr->setCrushSe(0x10017, vol);
}

void KartSound::DoMashDashSound()
{
    mSoundMgr->setDashSe(0x10014);
}

void KartSound::DoDashSound()
{
    mSoundMgr->setDashSe(0x10037);
}

void KartSound::DoRollCrashStartSound()
{
    mSoundMgr->setSe(0x1001c);
}

void KartSound::DoRollCrashSound()
{
    mSoundMgr->setSe(0x10011);
}

void KartSound::DoRollCrashEndSound()
{
    mSoundMgr->setSe(0x1001b);
}

void KartSound::DoBrakeSound()
{
    mSoundMgr->setBrakeSe(0x1000c);
}

void KartSound::DoStrikeSound()
{
    mSoundMgr->setSe(0x10028);
}

void KartSound::DoStarStartSound()
{
    mSoundMgr->setInvincibleBgm(1);
}

void KartSound::DoStarStopSound()
{
    mSoundMgr->clearInvincibleBgm(1);
}

void KartSound::DoWanWanStartSound()
{
    mSoundMgr->setInvincibleBgm(2);
}

void KartSound::DoWanWanStopSound()
{
    mSoundMgr->clearInvincibleBgm(2);
    DoTandemVoice(0x26);
}

void KartSound::DoChangeStarSound()
{
    mSoundMgr->changeDriver(false);
}

void KartSound::DoChibiSound(bool enable)
{
    mSoundMgr->setChibiFlag(enable, true);
}

void KartSound::DoGoalSound()
{
    mSoundMgr->setSe(0x10042);
}

void KartSound::DoLapSound()
{
    mSoundMgr->setSe(0x10041);
}

void KartSound::DoThunderSound()
{
    GetGameAudioMain()->startRaceSystemSe(0x40005);
}

void KartSound::DoShootSound()
{
    mSoundMgr->setJumpUpSe(0x10035);
}

void KartSound::DoFireBall()
{
    mSoundMgr->setSe(0x1000b);
}

void KartSound::DoHitFireBall()
{
    mSoundMgr->setSe(0x1003e);
}

void KartSound::DoKartDiveSound()
{
    mSoundMgr->setSe(0x10043);
}

void KartSound::DoKartMiniTuroboSound()
{
    mSoundMgr->setMiniturboSe(0x10044);
}

void KartSound::DoKartsetSeSound(u32 id)
{
    mSoundMgr->setSe(id);
}

void KartSound::DoLandingSound(f32 vol)
{
    mSoundMgr->setBoundSe(vol);
}

void KartSound::DoWheelSpin()
{
    mSoundMgr->setWheelSpinSe();
}

void KartSound::DoShineStartBGM()
{
    GetGameAudioMain()->startStreamBgm(0x2000023);
}

void KartSound::DoShineStopBGM()
{
    GetGameAudioMain()->stopStreamBgm(1);
    GetGameAudioMain()->startStreamBgm(0x2000025);
}

void KartSound::DoWallObjectSound(u32 id)
{
    f32 depth = GetGeoObjMgr()->getKartHitDepthNormalObj(mBody->mMynum);
    if (depth < 3.0f)
        return;
    if (mWallSoundActive == true)
        return;

    f32 vol = depth / 30.0f;
    if (vol > 1.0f)
    {
        vol = 1.0f;
    }

    mWallSoundActive = true;
    mSoundMgr->setCrushSe(id, vol);
}

void KartSound::DoItemAlarm()
{
    KartBody *body = mBody;
    if (!mIsAlarmActive)
    {
        if (GETJ2DManager()->isAlarm(body->mMynum))
        {
            mSoundMgr->setSe(0x1009c);
            mIsAlarmActive = true;
            return;
        }
    }
    mIsAlarmActive = false;
}

void KartSound::DoSpinSound()
{
    mSoundMgr->setSpinSe();
}

void KartSound::DoSpinTurnSound()
{
    mSoundMgr->setSpinTurnSe();
}

void KartSound::DoStrikeVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(2);
}

void KartSound::DoStaggerVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x1a);
}

void KartSound::DoCannonVoice()
{
    KartBody *body = mBody;
    body->mDriverModels[1]->getGameAudioCharacterSoundMgr();
    body->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x1b);
}

void KartSound::DoDragVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x19);
}

void KartSound::DoChampionVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x16);
}

void KartSound::DoPrizeVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x17);
}

void KartSound::DoOutsideVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x18);
}

void KartSound::DoGoalVoice()
{
    KartChecker *kartChecker = RCMGetKartChecker(mBody->mMynum);
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();
    int kartNo = GetKartCtrl()->GetKartNumber();
    if (raceMgr->isLANMode())
    {
        if (kartChecker->getRank() == 1)
        {
            DoChampionVoice();
        }
        else if (kartNo == kartChecker->getRank())
        {
            DoOutsideVoice();
        }
        else
        {
            DoPrizeVoice();
        }
    }
    else if (raceMgr->getRaceMode() == TIME_ATTACK)
    {
        if (kartChecker->isBestTotalTimeRenewal(0))
        {
            DoChampionVoice();
        }
        else if (kartChecker->isBestTotalTimeRenewal(4))
        {
            DoPrizeVoice();
        }
    }
    else if (raceMgr->getRaceMode() == VERSUS_RACE)
    {
        if (kartChecker->getRank() == 1)
        {
            DoChampionVoice();
        }
        else if (kartChecker->getRank() >= 2 && kartChecker->getRank() <= 3 && kartChecker->getRank() != kartNo)
        {
            DoPrizeVoice();
        }
    }
    else
    {
        if (kartChecker->getRank() == 1)
        {
            DoChampionVoice();
        }
        else if (kartChecker->getRank() >= 2 && kartChecker->getRank() <= 3)
        {
            DoPrizeVoice();
        }
        else
        {
            DoOutsideVoice();
        }
    }
}

void KartSound::DoDriverChampionVoice()
{
    mBody->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(0x16);
}

void KartSound::DoDriverPrizeVoice()
{
    mBody->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(0x17);
}

void KartSound::DoDriverOutsideVoice()
{
    mBody->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(0x18);
}

void KartSound::DoDriverGoalVoice()
{
    KartChecker *kartChecker = RCMGetKartChecker(mBody->mMynum);
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();
    int kartNo = GetKartCtrl()->GetKartNumber();
    if (raceMgr->isLANMode())
    {
        if (kartChecker->getRank() == 1)
        {
            DoDriverChampionVoice();
        }
        else if (kartNo == kartChecker->getRank())
        {
            DoDriverOutsideVoice();
        }
        else
        {
            DoDriverPrizeVoice();
        }
    }
    else if (raceMgr->getRaceMode() == TIME_ATTACK)
    {
        if (kartChecker->isBestTotalTimeRenewal(0))
        {
            DoDriverChampionVoice();
        }
        else if (kartChecker->isBestTotalTimeRenewal(4))
        {
            DoDriverPrizeVoice();
        }
    }
    else if (raceMgr->getRaceMode() == VERSUS_RACE)
    {
        if (kartChecker->getRank() == 1)
        {
            DoDriverChampionVoice();
        }
        else if (kartChecker->getRank() >= 2 && kartChecker->getRank() <= 3 && kartChecker->getRank() != kartNo)
        {
            DoDriverPrizeVoice();
        }
    }
    else
    {
        if (kartChecker->getRank() == 1)
        {
            DoDriverChampionVoice();
        }
        else if (kartChecker->getRank() >= 2 && kartChecker->getRank() <= 3)
        {
            DoDriverPrizeVoice();
        }
        else
        {
            DoDriverOutsideVoice();
        }
    }
}

void KartSound::DoSpinVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x4);
}

void KartSound::DoFallVoice()
{
    KartBody *body = mBody;
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(6);
    body->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(6);
}

void KartSound::DoDashVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(8);
}

void KartSound::DoGoldenDashVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x20);
}

void KartSound::DoThrowVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0xe);
}

void KartSound::DoPutVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x10);
}

void KartSound::DoChangeVoice()
{
    mBody->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(0xb);
}

void KartSound::DoWanwanVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x11);
}

void KartSound::DoItmHitVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x15);
}

void KartSound::DoPathVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x13);
}

void KartSound::DoShootVoice()
{
    KartBody *body = mBody;
    body->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(0x1c);
    body->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x1c);
}

void KartSound::DoRollCrashVoice()
{
    KartBody *body = mBody;
    body->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(0x1e);
    body->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x1e);
}

void KartSound::DoHeartVoice()
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(0x8);
}

void KartSound::DoTandemVoice(u8 id)
{
    mBody->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(id);
}

void KartSound::DoDriverVoice(u8 id)
{
    mBody->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(id);
}

void KartSound::DoAllVoice(u8 id)
{
    KartBody *body = mBody;
    body->mDriverModels[1]->getGameAudioCharacterSoundMgr()->setVoice(id);
    body->mDriverModels[0]->getGameAudioCharacterSoundMgr()->setVoice(id);
}
