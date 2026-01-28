#ifndef KARTBODY_H
#define KARTBODY_H

#include <JSystem/JGeometry.h>
#include "Kaneshige/KartInfo.h"
#include "Kaneshige/KartLoader.h"
#include "Kaneshige/Course/CrsArea.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kawano/driver.h"
#include "Osako/shadowModel.h"
#include "Sato/GeographyObj.h"
#include "Yamamoto/kartSus.h"
#include "Yamamoto/KartAnt.h"
#include "Yamamoto/KartCannon.h"
#include "Yamamoto/KartCheck.h"
#include "Yamamoto/KartCrash.h"
#include "Yamamoto/KartDamage.h"
#include "Yamamoto/KartDossin.h"
#include "Yamamoto/KartGame.h"
#include "Yamamoto/KartHandle.h"
#include "Yamamoto/KartItem.h"
#include "Yamamoto/KartPerCam.h"
#include "Yamamoto/KartPipe.h"
#include "Yamamoto/KartRescue.h"
#include "Yamamoto/KartScene.h"
#include "Yamamoto/KartStar.h"
#include "Yamamoto/KartStrat.h"
#include "Yamamoto/KartThunder.h"
#include "Yamamoto/KartTire.h"
#include "Yamamoto/KartTumble.h"

class KartBody
{
public:
    // TODO: move in KartGame
    enum GameStatus {
        HasCoDriver = 1<<0,
        GsUnknown3 = 1<<3,
    };

    enum CarStatus {
        CsUnknown0 = 1<<0,
        CsUnknown1 = 1<<1,
        DoesSlide = 1<<2,
        CsUnknown3 = 1<<3,
        CsUnknown5 = 1<<5,
        InDriverChange = 1<<7,
        CsUnknown8 = 1<<8,
        CsUnknown9 = 1<<9,
        CsUnknown10 = 1<<10,
        CsUnknown11 = 1<<11,
        CsUnknown12 = 1<<12,
        CsUnknown15 = 1<<15,
        CsUnknown18 = 1<<18,
        CsUnknown26 = 1<<26,
        CsUnknown27 = 1<<27,
    };

    KartBody() {}

    void DegubBody(u32);
    void MakeMassBody(f32, f32, f32, f32);
    void MakeRigidVertexBody(f32, f32, f32, f32);
    void SetUpRigidBodyPose(JGeometry::TVec3f *, JGeometry::TVec3f *);
    void SetUpRigidBodyStartGridPose();
    void DoForce(JGeometry::TVec3f *, JGeometry::TVec3f *);
    void DoTq(JGeometry::TVec3f *, JGeometry::TVec3f *);
    void DoForceOnly(KartBody *, JGeometry::TVec3f *, JGeometry::TVec3f *);
    void DoForceOnly(JGeometry::TVec3f *, JGeometry::TVec3f *);
    void DoGForce();
    void CalcMovement(JGeometry::TVec3f *, JGeometry::TVec3f *, JGeometry::TVec3f *, JGeometry::TVec3f *);
    int DoCalcWgVel();
    void DoPose();
    void DoCalcMassMatrix(Mtx44, f32, JGeometry::TVec3f *, JGeometry::TVec3f *);
    void WallFriction(JGeometry::TVec3f *, f32, JGeometry::TVec3f *, JGeometry::TVec3f *, JGeometry::TVec3f *, f32);
    void GroundReflection(JGeometry::TVec3f *, JGeometry::TVec3f *, f32, f32, f32);
    void RigidReflectionAnime(KartBody *, KartBody *, f32, f32);
    bool RigidReflectionPower(KartBody *, KartBody *, f32 *, f32 *);
    void RigidReflection(KartBody *, KartBody *, JGeometry::TVec3f *, JGeometry::TVec3f *, f32, f32, f32);
    void ObjectReflection(JGeometry::TVec3f *);
    void ObjectWallReflection(GeographyObj *);
    bool StarReact(GeographyObj *);
    void CheckObjectReflection();
    void CheckRoofReflection();
    void WallReflection(JGeometry::TVec3f *, f32, f32, f32);
    void GroundVertexReflection(int, CrsGround, JGeometry::TVec3f *, u8 *);
    void WallVertexReflection(CrsGround, JGeometry::TVec3f *, u8 *, u8 *);
    void CheckVertexReflection();
    bool CheckBodyHit(KartBody *, KartBody *);
    void InitBodySetting();
    void GetMiniGameCamData(int);
    void GetPakkunMiniGameCamData(int);
    void GetPakkunGameCamData(int);
    void InitCamSetting();
    void InitBodyCamParam();
    void InitBodyParam();
    void Init(int);

    // Inline
    KartCrash *getCrash() { return mKartCrash; }
    KartDossin *getDossin() { return mKartDossin; }
    KartScene *getScene() { return mKartScene; }
    KartCheck *getChecker() { return mKartCheck; }
    KartCannon *getCannon() { return mKartCannon; }
    KartStrat *getStrat() { return mKartStrat; }
    KartThunder *getThunder() { return mKartThunder; }
    KartDamage *getDamage() { return mKartDamage; }
    KartGame *getGame() { return mKartGame; }    
    KartItem *getItem() { return mKartItem; }
    KartTumble *getTumble() { return mKartTumble; }
    KartHandle *getHandle() { return mKartHandle; }
    KartLoader *getKartLoader() { return mKartLoader; }
    KartRescue *getRescue() { return mKartRescue; }
    KartPipe *getPipe() { return mKartPipe; }
    KartAnt *getAnt() { return mKartAnt; }
    KartSus *getSus(int index) { return mKartSus[index]; }


    int getTouchNum() { return mTouchNum; }

    // Unused
    void WallFallReflection(JGeometry::TVec3f *v1, JGeometry::TVec3f *v2);
    void GroundBoundReflection(CrsGround, JGeometry::TVec3f *);

    KartLoader *mKartLoader;
    KartSus *mKartSus[4];
    ExModel *mBodyModel;
    DriverModel *mDriverModels[2];
    DriverModel *mExModels[2];
    KartShadowModel *mShadowModel;
    CrsGround mBodyGround;
    CrsArea mShadowArea;
    CrsArea mRoofArea;
    KartRescue *mKartRescue;
    KartCannon *mKartCannon;
    KartDossin *mKartDossin;
    KartTumble *mKartTumble;
    KartScene *mKartScene;
    KartCheck *mKartCheck;
    KartDamage *mKartDamage;
    KartTire *mKartTire;
    KartStar *mKartStar;
    KartThunder *mKartThunder;
    KartCrash *mKartCrash;
    KartHandle *mKartHandle;
    KartPipe *mKartPipe;
    KartAnt *mKartAnt;
    KartStrat *mKartStrat;
    KartGame *mKartGame;
    KartItem *mKartItem;
    KartPerCam *mKartPerCam;
    u8 *mSettingPtrs[2];
    ECharID CharIDs[2];
    void *mUnkSub10c; 
    Mtx _110;
    Mtx mPlayerPosMtx;
    Mtx _170;
    Mtx _1a0;
    JGeometry::TVec3f _1d0[8];
    JGeometry::TVec3f _230;
    JGeometry::TVec3f mPos;
    JGeometry::TVec3f mPrevPos;
    JGeometry::TVec3f _254;
    JGeometry::TVec3f mVel;
    JGeometry::TVec3f mEffctVel;
    JGeometry::TVec3f mTireG;
    JGeometry::TVec3f _284;
    JGeometry::TVec3f _290;
    JGeometry::TVec3f _29c;
    JGeometry::TVec3f _2a8;
    JGeometry::TVec3f _2b4;
    JGeometry::TVec3f _2c0;
    JGeometry::TVec3f _2cc;
    JGeometry::TVec3f _2d8;
    JGeometry::TVec3f mWg;
    JGeometry::TVec3f _2f0;
    JGeometry::TVec3f _2fc;
    JGeometry::TVec3f _308;
    JGeometry::TVec3f _314;
    JGeometry::TVec3f _320;
    JGeometry::TVec3f _32c;
    JGeometry::TVec3f _338;
    JGeometry::TVec3f _344;
    JGeometry::TVec3f _350;
    JGeometry::TVec3f _35c;
    JGeometry::TVec3f _368;
    JGeometry::TVec3f _374;
    f32 mFrame;
    f32 _384;
    f32 _388;
    f32 _38c;
    f32 mLiftframe;
    f32 _394;
    f32 _398;
    f32 _39c;
    f32 _3a0;
    f32 _3a4;
    f32 _3a8;
    f32 _3ac;
    f32 _3b0;
    f32 _3b4;
    f32 _3b8;
    f32 _3bc;
    f32 _3c0;
    f32 _3c4;
    f32 _3c8;
    f32 _3cc;
    f32 _3d0;
    f32 _3d4;
    f32 _3d8;
    f32 _3dc;
    f32 _3e0;
    f32 _3e4;
    f32 _3e8;
    f32 _3ec;
    f32 _3f0;
    f32 _3f4;
    f32 _3f8;
    f32 _3fc;
    f32 _400;
    f32 _404;
    f32 _408;
    f32 _40c;
    f32 _410;
    f32 _414;
    f32 _418;
    f32 _41c;
    f32 _420;
    f32 _424;
    f32 _428;
    f32 _42c;
    f32 _430;
    f32 _434;
    f32 _438;
    f32 _43c;
    f32 _440;
    f32 mSpeed;
    f32 _448;
    f32 mCarRPM;
    f32 mKartRPM;
    f32 _454;
    f32 _458;
    f32 _45c;
    f32 _460;
    f32 _464;
    f32 _468;
    f32 _46c;
    f32 mSpeedScale;
    f32 _474;
    f32 mTireAngle;
    f32 _47c;
    f32 _480;
    f32 _484;
    f32 mSterrNorm;
    f32 _48c;
    f32 mSterrFrame;
    f32 mSterrFrameChase;
    f32 _498;
    f32 _49c;
    f32 _4a0;
    f32 _4a4;
    f32 _4a8;
    f32 _4ac;
    f32 _4b0;
    f32 _4b4;
    f32 _4b8;
    f32 _4bc;
    f32 _4c0;
    f32 _4c4;
    f32 _4c8;
    f32 _4cc;
    f32 _4d0;
    f32 _4d4;
    f32 _4d8;
    f32 _4dc;
    f32 _4e0;
    f32 _4e4;
    f32 _4e8;
    f32 _4ec;
    f32 _4f0;
    f32 _4f4;
    f32 _4f8;
    f32 mYawSterr;
    f32 mBtnMinusLiftSterr;
    f32 _504;
    f32 _508;
    f32 _50c;
    f32 _510;
    f32 _514;
    f32 _518;
    f32 _51c;
    f32 _520;
    f32 _524;
    f32 _528;
    f32 _52c;
    f32 _530;
    f32 _534;
    f32 _538;
    f32 _53c;
    f32 _540;
    f32 _544;
    f32 _548;
    f32 _54c;
    f32 _550;
    f32 _554;
    f32 _558;
    f32 _55c;
    f32 _560;
    f32 _564;
    f32 _568;
    u8 _56c[4]; // padding?
    u64 mCarStatus;  // 570, 574 | 0x400000: could be off track
    u32 mGameStatus; // 578
    u32 _57c;
    u32 _580;
    u32 _584; // 0: default, 4: crushed, 9: oob, 12: dkm cannon, 16: pb cannon
    u32 _588; // crash related, either 1 or 0
    u32 _58c;
    u16 _590;
    u16 _592;
    u16 _594; // Crash timer?
    u16 mBoostTimer;
    u16 _598;
    u16 _59a;
    u16 _59c;
    u16 mMTBoost;
    u16 mMTBoostMax;
    u8 _5a2[2];    // padding
    int mTouchNum; // amount of wheels that touch the ground?
    int mIdx;
    int mDriftSterr; // drift frame?
    u8 _5b0;
    u8 _5b1;
    u8 mDriver;
    u8 mMynum;
    u8 _5b4;
    u8 _5b5; // also some timer
    u8 mSlideTimer; // dash timer?
    u8 mCameraNum;
    u8 _5b8[8];
    u8 _5c0;
    u8 _5c1;
    u8 _5c2; // gets called in getChapter2, affects some bitfield
    u8 _5c3; // bitfield: 2: brake, 4: ???
    u8 mClass;
    u8 _5c5;
    u8 mMyRank;
    u8 mMTState;
};

#endif
