#ifndef KARTGAME_H
#define KARTGAME_H

#include "Sato/ItemObj.h"
#include "Yamamoto/kartSus.h"
#include "types.h"

class KartBody;

class KartGame
{
public:
    // In kartCtrlStrat.cpp
    bool Init(int kartNo);
    void GetGorundTireNum();
    void WatchEffectAcceleration();
    void WatchAcceleration();
    void DoItmCancel();
    void DoStopItm();
    void DoChange();
    void DoSlide();
    void DoDriftTurboSterr();
    void SetDriftTurboSterr();
    void CheckDriftTurbo();
    void DoWarmUpRoll();
    void DoRollAnim();
    void DoDriftClear();
    void DoRoll();
    void DoTestPitch();
    void DoLiftTurbo();
    void DoTurbo();
    void DoRollThrow();
    void DoRollOver();
    void DoWanWan();
    void DoPushStart();
    void DoBalance(f32 *, f32);
    void MakeClear();
    void MakeBoardDash();
    void MakeJumpDash();
    void MakeSpJumpDash();
    void MakeMashDash();
    void MakeGoldenMashDash();
    void MakeStartDash();
    void MakeCrashDash();
    void MakeWheelSpin();
    void MakeJump();
    void DoAirCheck();
    void DoRearSlidePower();
    void DoRearSlideBody();
    void DoCorner();
    void FrameWork(f32, KartSus *, KartSus *);
    void DoBodyAction();
    void DoElementForce();
    bool CheckBalloon();
    void SetRank();
    void RankWatchMan();
    void ItemWatchMan(ItemObj *);
    void AfterItemWatchMan();
    void DoFlagCtrl();
    void KeepWatch();
    void DoActionMgr();
    void DoActionCtrl();
    void DoStatus();

    // In kartChapter.cpp
    void DoVisible();
    void DoWinCamera();
    void DoChapterOne();
    void DoChapterTwo();
    void DoChapterThree();
    void DoChapterFour();
    void DoChapterFive();
    void DoChapterSix();
    void DoChapterSeven();
    void DoChapterBalloon();
    void DoChapterBomEsc();
    void DoStaffRoll();
    void DoWaitBattleWin();
    void DoStartGoalCtl();

    // Inline/Unused
    // void DoParamChange();
    // void DoJump();
    // void CheckTeamDriftTurbo();
    // void DoTeamWarmUpRoll();
    // void DoTeamRollAnim();
    // void DoPushBody();
    // void FrameWorkR(f32, f32, KartSus *);
    // void FrameWorkL(f32, f32, KartSus *);
    // void DoTurboPower();
    // void CheckBalloonPlayer();
    KartBody *mBody; // 00
    u32 _4;
    u8 _8;
    u8 _9;
    u8 _A[0x4];
    u16 _E;
    u16 _10;
    u16 mCountDownDuration; // 12
    u8 _14[0x4];
    float _18[2];
    JGeometry::TVec3f _20;
    JGeometry::TVec3f _2C;
    JGeometry::TVec3f _38;
};

#endif KARTGAME_H
