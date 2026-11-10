#ifndef RACEMGR_H
#define RACEMGR_H

#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JUtility/TColor.h"
#include "Kaneshige/Blurscreen.h"
#include "Kaneshige/KartInfo.h"
#include "Kaneshige/MaskScreen.h"
#include "kartEnums.h"
#include "JSystem/JKernel/JKRDisposer.h"
#include "JSystem/JUtility/JUTDbg.h"
#include "Kameda/PauseManager.h"
#include "Kaneshige/Course/Course.h"
#include "Kaneshige/RaceBGMPlayer.h"
#include "Kaneshige/RaceDirector.h"
#include "Kaneshige/RaceInfo.h"
#include "Kaneshige/RaceTime.h"
#include "Kaneshige/KartChecker.h"
#include "Kaneshige/KartLoader.h"
#include "Kaneshige/RaceDrawer.h"
#include "Kaneshige/SysDebug.h"
#include "Kawano/StaffRoll2D.h"
#include "Osako/Award2D.h"
#include "Yamamoto/kartCtrl.h"
#include "types.h"

class RaceMgr : public JKRDisposer
{
public:
    struct EventInfo
    {
        s16 id;
        const char *jpName;
        const char *engName;
    };

    RaceMgr(RaceInfo *);
    virtual ~RaceMgr();

    static const EventInfo sEventTable[];

    static s16 sMyStartPointID;

    static RaceMgr *sRaceManager;
    static s16 sForceTotalLapNum;
    static s16 sDispFrameCounter;

    static RaceMgr *getManager() { return sRaceManager; }
    static RaceMgr *getCurrentManager() { return getManager(); }

    class Console
    {
    public:
        Console();
        void changeTargetNo(int, bool);
        // Unused/Inline
        void clearZBuffer();
        bool isZoom();
        // Inline
        int getTargetNo() const { return mTargetNo; }
        void setDraw() { mFlags |= 1; }
        void setConsoleNo(int cnsNo) { mCnsNo = cnsNo; }
        void clrDraw() { mFlags &= ~1; }
        bool isDraw() const { return mFlags & 1; }
        void clrJugemZClr() { mFlags &= ~8; }
        void setJugemZClr() { mFlags |= 8; }
        bool isNoStat() const { return mFlags & 4; }
        bool isValid() const { return mCnsNo >= 0; }
        bool isDemo() const { return mIsDemo; }

    private:
        int mCnsNo;
        bool mIsDemo;
        int mTargetNo;
        u16 mFlags;
    };

    void updateBestTime();                                             // 0x801ad064
    void setRandomSeed();                                              // 0x801ad164
    void editRaceInfoForDebug();                                       // 0x801ad1bc
    void createConsole();                                              // 0x801ad210
    void resetConsole();                                               // 0x801ad2e8
    bool hideConsole(u32);                                             // 0x801ad368
    bool isHiddingConsole(u32 index);                                  // UNUSED
    void createModel();                                                // 0x801ad3c0
    void createKartModel();                                            // 0x801ad4ac
    void createCourseModel();                                          // 0x801ad578
    void createObjectModel();                                          // 0x801ad604
    void createItemModel();                                            // 0x801ad694
    void createEffectModel();                                          // 0x801ad724
    void createLight();                                                // 0x801ad7d0
    void resetRace();                                                  // 0x801ad9c4
    void resetRaceForResetEvent();                                     // 0x801ada58
    void resetRaceForRestartEvent();                                   // 0x801ada98
    void resetRaceForReplayEvent();                                    // 0x801adb04
    void resetRaceCommon();                                            // 0x801adb48
    int getCurrentConsoleNumber();                                     // 0x801add38
    void drawRace();                                                   // 0x801add84
    void checkKart();                                                  // 0x801ade64
    void checkRank();                                                  // 0x801ae00c
    void checkRankForBalloonBattle();                                  // 0x801ae0dc
    void checkRankForRobberyBattle();                                  // 0x801ae238
    void checkRankForBombBattle();                                     // 0x801ae324
    void checkRankForEscapeBattle();                                   // 0x801ae4b8
    void checkRankForAwardDemo();                                      // 0x801ae5a8
    void checkRankForRace();                                           // 0x801ae600
    void setRaceResult();                                              // 0x801ae774
    void calcRace();                                                   // 0x801ae870
    void frameWork();                                                  // 0x801ae9d0
    void updateRace();                                                 // 0x801aeac0
    bool isRaceModeMiniGame();                                         // 0x801aef9c
    ERacePhase getRacePhase();                                         // 0x801aefe4
    const RaceTime &getMiniGameTime();                                 // 0x801af008
    int searchNRankKart(int);                                          // 0x801af02c
    bool isAbleStart() const;                                          // 0x801af09c
    void setJugemZClr(u32, bool);                                      // 0x801af0dc
    u8 getStartID(int);                                                // 0x801af18c
    bool getStartPoint(JGeometry::TVec3f *, JGeometry::TVec3f *, int); // 0x801af1d8
    f32 getStartJugemOffsetY(int);                                     // 0x801af550
    int getProcLevel();                                                // 0x801af654
    bool isItemBoxValid();                                             // 0x801af6a4
    void beginProcTime(s16);                                           // 0x801af718
    void endProcTime(s16);                                             // 0x801af7cc
    const EventInfo *searchEventInfo(s16);                             // 0x801af864
    bool isJugemCountStart();                                          // 0x801af8a0
    bool isKartGoal(int);                                              // 0x801af8e0
    int getGoalKartNumber();                                           // 0x801af904
    u32 getPadRecorderFrame();                                         // 0x801af96c
    int getTotalLapNumberForDisplay() const;                           // 0x801af9c4
    bool robRivalOfBalloon(int, int);                                  // 0x801afa84
    bool robRivalOfRabbitMark(int, int);                               // 0x801afb48

    // Unused
    int getDrawingConsoleNumber();

    // Inline Functions
    // RaceInfo related
    bool isLANMode() const { return mRaceInfo->isLANMode(); }
    bool isMirror() const { return mRaceInfo->isMirror(); }
    bool isTinyProcess() const { return mRaceInfo->isTinyProcess(); }
    bool isWaitDemoMode() const { return mRaceInfo->isWaitDemo(); }
    bool isLastWaitDemo() const { return mRaceInfo->isLastWaitDemo(); } 
    int getKartNumber() const { return mRaceInfo->getKartNumber(); }
    int getAwardKartNo() const { return mRaceInfo->getAwardKartNo(); }
    int getConsoleNumber() const { return mRaceInfo->getConsoleNumber(); }
    int getItemSlotType() const { return mRaceInfo->getItemSlotType(); }
    int getStatusNumber() const { return mRaceInfo->getStatusNumber(); }
    int getGPStageNo() const { return mRaceInfo->getGpStageNo(); }
    ERacePhase getDemoNextPhase() const { return mRaceInfo->getDemoNextPhase(); }
    ERaceLevel getRaceLevel() const { return mRaceInfo->getRaceLevel(); }
    ERaceGpCup getRaceGpCup() const { return mRaceInfo->getGpCup(); }
    // why did this not use a getter?
    ERaceMode getRaceMode() const { return mRaceInfo->mRaceMode; }

    bool isDemoCamera(int camNo) const
    {
#line 162
        JUT_MINMAX_ASSERT(0, camNo, 4)
        return mRaceInfo->mIsDemoKart[camNo];
    }

    KartInfo *getKartInfo(int index)
    {
#line 170
        JUT_MINMAX_ASSERT(0, index, 8);
// TODO: in release it needs the other inline to prevent regswaps, debug grabs stuff from raceInfo without getters, why?
#ifdef DEBUG
        return &mRaceInfo->mKartInfo[index];
#else
        return mRaceInfo->getKartInfo(index);
#endif
    }

    bool isRaceEnd() const { return mRaceDirector->isRaceEnd(); };
    bool checkRaceEnd() { return mRaceDirector->checkRaceEnd(); }

    BlurScreen *getBlurScreen() { return mRaceDrawer->getBlurScreen(); }
    void setBlurColor(const JUTColor &color) { mRaceDrawer->setBlurColor(color); }
    void setBlurDecrease(u8 a) { mRaceDrawer->setBlurDecrease(a); }
    KartDrawer *getKartDrawer(int idx) { return mRaceDrawer->getKartDrawer(idx); };
    MaskScreen *getMaskScreen() { return mRaceDrawer->getMaskScreen(); }
    int getCameraNumber() const { return getConsoleNumber(); }

    bool isCrsDemoMode() { return getRacePhase() == PHASE_CRS_DEMO; }
    // RaceMgr get/sets
    u32 getRaceFrame() const { return mFrame; }
    int getTotalLapNumber() const { return mTotalLapNumber; }
    Course *getCourse() const { return mCourse; }
    const RaceTime &getBestLapTime() { return mBestLapTime; }
    bool isRaceModeGp() const { return getRaceMode() == GRAND_PRIX; }
    bool isRaceModeTA() const { return getRaceMode() == TIME_ATTACK; }
    bool isRaceModeVs() const { return getRaceMode() == VERSUS_RACE; }
    bool isStaffRoll() { return getRaceMode() == STAFF_ROLL; } // pls don't tell me it's stored as local variable
    bool isAwardDemoMode() { return getRaceMode() == AWARD_DEMO; }
    bool isReplayMode() const { return mReplayMode & 1; }
    bool isActiveAreaLight() const { return mAreaLight & 1; }
    void activeAreaLight() { mAreaLight |= 1; }
    bool isSubScrExist() const { return false; }
    Award2D *getAward2D() { return mAward2D; }
    StaffRoll2D *getStaffRoll2D() { return mStaffRoll2D; }

    KartLoader *getKartLoader(int index) const
    {
#line 257
        JUT_MINMAX_ASSERT(0, index, 8);
        return mKartLoader[index];
    }
    KartCam *getCamera(int cameraNo) const
    {
        JUT_MINMAX_ASSERT(0, cameraNo, 4);
        return KartCtrl::getKartCtrl()->getKartCam(cameraNo);
    }
    KartChecker *getKartChecker(int index) const
    {
        JUT_MINMAX_ASSERT(0, index, 8);
        return mKartChecker[index];
    }

    int getTargetKartNo(u32 viewNo) const
    {
#line 283
        JUT_MINMAX_ASSERT(0, viewNo, 4);
        return mRaceInfo->mTargetKarts[viewNo];
    }

    const RaceTime &getBestTotalTime(int recID)
    {
#line 328
        JUT_MINMAX_ASSERT(0, recID, 5);
        return mBestTotalTimes[recID];
    }

    JKRArchive *getAwardArc() {
        return (JKRArchive *)mAwardArc;
    }

public:
    RaceDirector *mRaceDirector;
    RaceDrawer *mRaceDrawer;
private:
    u16 mAreaLight;
    u8 _0x22; // raceNum?
    u8 _0x23; // probably padding
    u32 mFrame;
    JKRHeap *mRaceHeap;
    u16 mReplayMode;
    s16 mTotalLapNumber;
    s32 _0x30;
    bool mAbleStart;
public:
    RaceInfo *mRaceInfo;
    RaceBGMPlayer *mRaceBGMPlayer;
    Console *mConsole;
private:
    Course *mCourse;
    KartChecker *mKartChecker[8];
    KartLoader *mKartLoader[8];
    Award2D *mAward2D;
    StaffRoll2D *mStaffRoll2D;
    void *mAwardArc;        // Should this be declared a JKRArchive...?
    RaceTime mBestLapTime;
    RaceTime mBestTotalTimes[5];
    s16 mEvents;
    // These are monitor IDs, set to one of the IDs in sEventTable to monitor how long the CPU took to execute that part
    s16 mProctime1;
    s16 mProctime2;
    s16 mProctime3;
    s16 mProctime4;
};

inline RaceMgr *RCMGetManager() { return RaceMgr::getManager(); }
inline Course *RCMGetCourse() { return RaceMgr::getManager()->getCourse(); }
inline KartCam *RCMGetCamera(int index) { return RaceMgr::getManager()->getCamera(index); }
inline KartChecker *RCMGetKartChecker(int index) { return RaceMgr::getManager()->getKartChecker(index); }
inline KartLoader *RCMGetKartLoader(int index) { return RaceMgr::getManager()->getKartLoader(index); }

inline bool RCMIsPause() { return PauseManager::tstPause(); }

#endif // RACEMGR_H
