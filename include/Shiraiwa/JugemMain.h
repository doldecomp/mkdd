#ifndef JUGEMMAIN_H
#define JUGEMMAIN_H

#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/DarkAnmMgr.h"
#include "Sato/AnmController.h"
#include "Sato/GeographyObj.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Interp.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/JugemItem.h"
#include "Shiraiwa/JugemHeadItem.h"
#include "Shiraiwa/JugemRodItem.h"
#include "Shiraiwa/AnmPlayer.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"


class TJugem : public TMapObjHioNode, public StateObserver
{
public:
    TJugem();
    virtual ~TJugem();
    virtual void loadAnimation();
    virtual void createModel(JKRSolidHeap *, u32);
    virtual void createShadowModel(JKRSolidHeap *, u32);
    virtual void reset();
    virtual void calc();
    virtual void update();
    virtual void viewCalc(u32);
    virtual void setCurrentViewNo(u32);
    virtual const char *getBmdFileName();
    virtual const char *getShadowBmdFileName();
    virtual const char *getJ3DModelDataTevStageNum();
    virtual void createColModel(J3DModelData *);

    virtual void InitExec();
    virtual void MoveExec();

    void makeAnmCtrl(int num);
    void resetStaticData();
    void hold(u32, u32);
    void setJugemItem(TJugemItem *, u32);
    void setJugemHeadItem(TJugemHeadItem *item, u32 id);
    
    bool checkVisible(u32);
    void initFunc_Erase();
    void doFunc_Erase();
    void initFunc_Wait();
    void doFunc_Wait();
    
    void resetJugemOriginCourse();
    void resetJugemOriginNumber();
    
    void getZDir(int, JGeometry::TVec3f *);
    void resetJugemOrigin(const JGeometry::TVec3f &, const JGeometry::TVec3f &);
    void getJugemOrigin(const JGeometry::TVec3f &, JGeometry::TVec3f *);
    
    void move(int);
    bool checkKartCrash();
    
    void setPosition(const JGeometry::TVec3f &);
    void setRMtx(const JGeometry::TPos3f &);
    void resetAllPosAndMtx();
    void localMove(JGeometry::TVec3f *);
    void globalMove(JGeometry::TVec3f *);
    int getGlobalState();
    void fixPosition(JGeometry::TVec3f *, f32);
    f32 fixGround(CrsGround &, const JGeometry::TVec3f &);
    void fixWall(CrsGround &, JGeometry::TVec3f *);
    void fixNear(JGeometry::TVec3f *);
    void setLimitation(JGeometry::TVec3f *, f32, f32);
    void chase(int, const JGeometry::TVec3f &, const JGeometry::TVec3f &, JGeometry::TVec3f &);
    void hide();
    void hideAll();
    void show(u8);
    
    void wearCap(u32);
    void changeAnimation(u32);
    void cutFishLine();
    static bool nodeCallBack(J3DJoint *, int);

    void setKartNum(u8);
    static u32 getScreenType();
    u32 getJ3DModelDataTevStageNum() const {
        return 0x20020; // TODO: Identify what this value really represents.
    }
    
    void setCameraNum(u8 cam);


    // The following declarations exist across multiple files:
    
    // JugemRescue
    void initFunc_RescueWait();
    void doFunc_RescueWait();
    void initFunc_Rescue();
    void doFunc_Rescue();
    bool checkRescueInProc();
    bool checkRescueOutProc();


    // JugemReverse
    void initFunc_Reverse();
    void doFunc_Reverse();
    void initFunc_RevEnd();
    void doFunc_RevEnd();
    bool checkReverseProc();
    bool checkReviveProc();


    // JugemLap
    void initFunc_Lap();
    void doFunc_Lap();
    void initFunc_LapStay();
    void doFunc_LapStay();
    void initFunc_LapEnd();
    void doFunc_LapEnd();
    bool checkLapProc();


    // JugemDemo
    static bool isDemoMode();
    void initFunc_DemoHide();
    void doFunc_DemoHide();
    void initFunc_DemoIn();
    void doFunc_DemoIn();
    void initFunc_DemoWait();
    void doFunc_DemoWait();
    void initFunc_WaitDemoOut();
    void doFunc_WaitDemoOut();
    void initFunc_DemoOut();
    void doFunc_DemoOut();
    void initFunc_Ending();
    void doFunc_Ending();


    // JugemGoal
    void initFunc_Goal();
    void doFunc_Goal();
    bool checkJugemAppearRank();
    static Vec scGoalPoints0[4];


    // JugemStart
    void resetStartPosition();
    void initFunc_StartWaitPermission();
    void doFunc_StartWaitPermission();
    void initFunc_StartWaitCamera();
    void doFunc_StartWaitCamera();
    void initFunc_StartDown();
    void doFunc_StartDown();
    void initFunc_StartWaitBlend();
    void doFunc_StartWaitBlend();
    void initFunc_StartCountDown();
    void doFunc_StartCountDown();
    void initFunc_StartUp();
    void doFunc_StartUp();
    void initFunc_StartWaitHide();
    void doFunc_StartWaitHide();
    void startNextAnime();

    // End of external references.

    void signalGo()
    {
        mIsAbleStart = true;
        setState(0);
    };
    bool isAllShapePacketHidding(u32 viewNo) { return mModel.isAllShapePacketHidding(viewNo); }
    bool isAbleStart(); /* { return mIsAbleStart; }*/
    bool isCallThree() { return !mSignal ? false : mSignal->isCallThree(); }
    bool isDemoOut() { return isDemoMode() && mAnmPlayer.getCurAnmNumber() == 2; }

    const JGeometry::TVec3f &getPosition() const { return mPos; }
    u8 getKartNum() { return mKartNum; }

    void setHearPtr(TJugemHeadHear *hear) { setJugemHeadItem(hear, 0); }
    void setSignalPtr(TJugemRodSignal *signal) { mSignal = signal; }
    void setVoidRodPtr(TJugemVoidRod *rod) {
        setJugemItem(rod, 1);
        setSignalPtr(rod->getSignal());
    }

    void setDemoPattern(u32 pattern) {
        mDemoPattern = pattern;
    }

    void startDemo() { setState(0x12); }
    void endDemo() { setState(0x14); }

    int getSignalState();

    void setEndingState() { setState(0x16); }

private:
    static StateFuncSet<TJugem> sTable[23];
    static f32 scJugemDistance;
    static const f32 scJugemHeight;
    static const f32 scEraseHeight;
    static s16 sChaseDistance;
    static f32 sChaseAccel;
    static f32 sChaseDecel;
    static f32 sChaseEndSpeed;
    static s16 sShadowEraseFrame;
    static TAnmInfo sAnmInfos_Start[5];
    static TAnmInfo sAnmInfos_Lap[4];
    static TAnmInfo sAnmInfos_Rescue[3];
    static TAnmInfo sAnmInfos_Reverse[2];
    static TAnmInfo sAnmInfos_Goal[1];
    static TAnmInfo sAnmInfos_Demo1[3];
    static TAnmInfo sAnmInfos_Demo2[3];
    static TAnmInfo sAnmInfos_Demo3[3];
    static TJugemAnmTableEntry sAnmStateTable[5];
    static TJugemAnmTableEntry sDemoAnmStateTable[3];

private:
    // Private static declarations for other files.
    // JugemLap
    static f32 sLapMoveSpeed1;
    static f32 sLapMoveSpeed2;
    static f32 sAnimeStartFrame;
    static f32 sLapPauseRatio;
    static s16 sLastStayFrame;
    static Vec scLapPointsIn_1p[10];
    static Vec scLapPointsIn_multi[10];

    // JugemReverse
    static Vec scReversePoints2[4];
    static Vec scReversePoints0_1p[5];
    static Vec scReversePoints0_multi[5];
    static const Vec scReversePoints1_1p[6]; // unused
    static const Vec scReversePoints1_multi[6];
    static s16 scReverseJudgeTime;
    static s16 scReviveJudgeTime;
    static f32 sReverseEnterSpeed;
    static f32 sReverseStaySpeed;
    static f32 sReverseLeaveSpeed;

    // JugemStart
    static Vec scStartPoints0_1p[6];
    static Vec scStartPoints0_2p[5];
    static s16 sStartWaitCameraFrame;
    static s16 sStartWaitBlendFrame;
    static s16 sStartUpFrame;
    static f32 sStartUpSpeed;
    static f32 sStartDownSpeed;


public:
    AnmController *mAnmController[5];       // 158
    AnmController *mDemoAnmController[3];   // 16c
    TAnmPlayer mAnmPlayer;                  // 178
    J3DAnmObjMaterial _190;                 // 190
    TJugemItem *mJugemItem;                 // 1b0
    TJugemHeadItem *mJugemHeadItem;         // 1b4
    TBSplineInterp mSplineInterp;           // 1b8
    JGeometry::TVec3f _208;                 // 208
    JGeometry::TVec3f _214;                 // 214
    JGeometry::TVec3f _220;                 // 220
    JGeometry::TVec3f _22c;                 // 22c
    JGeometry::TVec3f _238;                 // 238
    u32 mDemoPattern;                       // 244
    u8 mCameraNum;                          // 248
    u8 mKartNum;                            // 249
    u8 _24a[0x24c - 0x24a];                 // 24a
    s32 _24c;                               // 24c - Some sort of lap number...?
    bool mIsAbleStart;                      // 250
    bool mIsStartPoint;                     // 251
    u8 _252[0x254 - 0x252];                 // 252
    DarkAnmPlayer *mDarkAnmPlayer;          // 254
    TJugemItem *mItem[3];                   // 258 - Items on Lakitu's fishing rod at startup?
    TJugemHeadItem *mHeadItem[2];           // 264 - Lakitu's caps.
    TJugemRodSignal *mSignal;               // 26c - Lap Number, Final Lap, U-Turn sign etc.
    u8 _270[0x274 - 0x270];                 // 270
    u8 _274;                                // 274
    u8 _275;                                // 275 // Unused?
    u16 _276;                               // 276
    u16 _278;                               // 278
    u8 _27a[0x27c - 0x27a];                 // 27a
}; // Size: 0x27c

#endif
