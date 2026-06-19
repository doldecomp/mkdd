#ifndef KARTCTRL_H
#define KARTCTRL_H

#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/KartLoader.h"
#include "types.h"
#include "Inagaki/GameSoundMgr.h"
#include "Kaneshige/CenterLine.h"
#include "Sato/RivalKart.h"

#include "Yamamoto/kartPad.h"
#include "Yamamoto/kartSus.h"
#include "Yamamoto/KartAnime.h"
#include "Yamamoto/KartAppendix.h"
#include "Yamamoto/KartDisp.h"
#include "Yamamoto/KartSound.h"
#include "Yamamoto/KartTarget.h"

#include "Yamamoto/KartMat.h"
#include "Yamamoto/kartBody.h"

class KartCam;

class KartCtrl : public KartMat // WHAT THE HELL ARE YOU DOING
{
public:
    KartCtrl() {}

    void DoBodyHit();
    static KartCtrl *makeKartCtrl();
    void Init(KartBody *, KartSus **, KartAppendix **, KartSound *, KartTarget *, KartDisp *, int);
    void DynamicsInit(bool);
    void DynamicsReset();
    void DynamicsStage1();
    void DynamicsStage2();
    void DrawKartInit(int);
    void SetGrafPort();
    void DrwaBody(int, u32, Mtx44);
    void DrwaParts(int, u32, Mtx44);

    // KartCtrlInfo
    void SetPadClear(KartGamePad *);
    void SetWanWanPadClear(KartGamePad *);
    void SetGhostPadClear(KartGamePad *);
    void PadAllClear(int);
    void DoContCtl(int);
    void DoContCopy(int);
    void DoContPaste(int);
    f32 GetItemStickY(int);
    f32 GetItemStickX(int);
    u32 GetItemButton(int);
    void DoLod();
    int GetPortPtr(int);
    void GetCamFovy(int);
    void GetCamAspect(int);
    // TODO: do these return references or pointers?
    CrsGround *GetBodyGround(int);
    CrsGround *GetRRTireGround(int);
    CrsGround *GetRLTireGround(int);
    void GetBodyPos(int idx, JGeometry::TVec3f *ret);
    void GetBodyVel(int, JGeometry::TVec3f *);
    bool GetLeftFTirePos(int, Vec *);
    bool GetRightFTirePos(int, Vec *);
    void GetLeftTirePos(int, Vec *);
    void GetRightTirePos(int, Vec *);
    int GetTirePos(int, int, Vec *);
    f32 GeTireG(int);
    f32 GetCarSpeed(int);
    f32 GetCarRpm(int);
    f32 GetKartRpm(int);
    KartGamePad *GetDriveCont(int);
    KartGamePad *GetCoDriveCont(int);
    u64 GetCarStatus(int);
    u32 GetGameStatus(int);
    void SetTireDispRound(KartBody *, KartSus *, f32);
    void SetKartRpm(KartBody *, f32, f32);
    bool WhichDriver(int);
    u8 WhichNowDriver(int);
    bool CheckCamera(int);
    int GetCameraNum(int);
    bool CheckItem(const int); // TODO: maybe not const
    f32 GetMaxSpeed(int);
    f32 GetDownSlopeAcc(int);
    f32 GetDownSlopeSpeed(int);
    f32 GetTireAngle(int);
    bool GetTandemDir(int); // unsure of return type
    f32 GetWaterHeight(int);
    bool CheckJugemuSignal();
    GameAudio::KartSoundMgr *GetKartSoundMgr(int idx); /*{ return getKartSound(idx)->mKartSoundMgr; }*/
    GameAudio::CharacterSoundMgr *GetCharacterSoundMgr(int);
    f32 GetKartBodyOffset(int);
    bool MakeChangePossible(int);
    bool CheckTandemItmGet(int);
    bool CheckTandemItmRollingGet(int);
    void SetObjectPos(int, JGeometry::TVec3f);
    bool CheckThunderBolt(int);
    double GetTireRadius(int);
    bool IsBurn(int);
    bool IsWallReact(int);
    bool HaveBalloon(int);
    int GetDriftCnt(int);
    bool IsMiniGame();
    bool IsMiniGameEnd();
    u8 CheckWinner();
    void GetKartEffctVel(int, JGeometry::TVec3f *);
    bool CheckChange(int);
    bool CheckMatchless(int);
    bool CheckReverse(int);
    f32 GetKartScale(int);
    RivalKart *getKartEnemy(int);
    CLPoint *GetKartCenterPtr(int);
    void EnemyInit(int);
    void EnemyResetInit(int);
    bool CheckZoomWinConsole();
    void SetUpMiniTurbo(int, u8);
    // Inline/Unused
    // void SetPadKeyClear(KartGamePad *);
    // void DiscriminationCont(int);
    // void IsHandleCont(int);
    // void GetCamNear(int);
    // void GetCamFar(int);
    // void GetFRTireGround(int);
    // void GetFLTireGround(int);
    // void GetFRTireStatus(int);
    // void GetFLTireStatus(int);
    // void GetRRTireStatus(int);
    // void GetRLTireStatus(int);
    // void GetBodyAngle(int);
    // void GetBodyMove(int);
    // void GetCarType(int);
    // void DispSpeed(J2DPrint *);
    // void MakeRnd();
    // void CheckDamage(int);
    // void GetTireMtx(int, Mtx44);
    // void GetCharID(int);
    // void GetCharacterSoundMgr(int);
    // void GetKartTopPos(int);
    // void CheckPushStart(int);
    // void GetCenterLineNum(int);
    // void RivalKart::GetTargetcnt();

    // Inlines for selecting GamePad for driver/passenger.
    inline KartGamePad* getPad(int gamePadIndex) { return mGamePads[gamePadIndex][0]; }     // Driver
    inline KartGamePad* getCoPad(int gamePadIndex) { return mGamePads[gamePadIndex][1]; }   // Passenger

    void HaveRabbit();

    void AnimeInit(int idx) { getKartAnime(idx)->Init(); }
    void ApeendixInit(int idx)
    {
        for (int i = 0; i < 2; i++)
            getKartAppendix(i + idx * 2)->Init(i);
    }
    void BodyInit(int idx) { getKartBody(idx)->Init(idx); }
    void BodyPoseInit(int idx) { getKartBody(idx)->SetUpRigidBodyStartGridPose(); }
    void TargetInit(int idx) { getKartTarget(idx)->Init(); }
    void SusInit(int idx)
    {
        for (int i = 0; i < 4; i++)
            getKartSus(i + idx * 4)->NormalInit(i);
    }

    void ResetSusInit(int idx)
    {
        for (int i = 0; i < 4; i++)
            getKartSus(i + idx * 4)->ResetInit(i);
    }

    void DoSusAction(int idx) { getKartSus(idx)->DoSusAction(idx); }
    void DoTireAction(int idx) { getKartSus(idx)->DoTireAction(); }

    void DoGForce(int idx) { getKartBody(idx)->DoGForce(); }
    void DoPose(int idx) { getKartBody(idx)->DoPose(); }    
    void CheckObjectReflection(int idx) { getKartBody(idx)->CheckObjectReflection(); }
    void CheckRoofReflection(int idx) { getKartBody(idx)->CheckRoofReflection(); }
    void CheckVertexReflection(int idx) { getKartBody(idx)->CheckVertexReflection(); }
    void SetDriverCurrentViewNo(int idx, u32 viewNo) { getKartBody(idx)->mExModels[0]->setCurrentViewNo(viewNo);  }
    void SetCoDriverCurrentViewNo(int idx, u32 viewNo) { getKartBody(idx)->mExModels[1]->setCurrentViewNo(viewNo);  }
    void DrwaDriver(int idx, u16 level) { getKartBody(idx)->mExModels[0]->update(level);  }
    void DrwaCoDriver(int idx, u16 level) { getKartBody(idx)->mExModels[1]->update(level);  }

    int GetKartNumber() { return mKartCount; }
    KartBody *getKartBody(int idx){return mKartBodies[idx]; } 
    KartCam *getKartCam(int idx) { return mKartCams[idx]; }
    KartSound *getKartSound(int idx) { return mKartSounds[idx]; }
    KartSus *getKartSus(int idx) { return mKartSus[0][idx]; }
    KartAppendix *getKartAppendix(int idx) { return mKartAppendix[0][idx]; }
    KartPad *getKartPad(int idx) { return mKartPads[idx]; }
    KartAnime *getKartAnime(int idx) { return mKartAnimes[idx]; }
    KartTarget *getKartTarget(int idx) { return mKartTargets[idx]; }
    KartDisp *getKartDisp(int idx) { return mKartDisps[idx]; }
    KartLoader *getKartLoader(int idx) { return mKartLoaders[idx]; }

    void setKartPad(KartPad *pad, int idx) { mKartPads[idx] = pad; }
    void setKartCam(KartCam *cam, int idx) { mKartCams[idx] = cam; }
    void setKartAnime(KartAnime *anime, int idx) { mKartAnimes[idx] = anime; }

    u64 GetKartStatus(int idx) { return getKartBody(idx)->mCarStatus; }

    static KartCtrl *getKartCtrl() { return mCtrl; }

private:
    static KartCtrl *mCtrl;
    KartLoader *mKartLoaders[8];  // 0
    KartGamePad *mGamePads[8][2]; // 20 might not be a double sided array
    KartPad *mKartPads[8]; // 60
    KartAnime *mKartAnimes[8]; // 80
    KartBody *mKartBodies[8]; // a0
    KartSus *mKartSus[8][4]; // c0
    KartAppendix *mKartAppendix[8][2]; // 140
    RivalKart *mRivalKarts[8]; // 180
    KartSound *mKartSounds[8]; // 1a0    
    KartTarget *mKartTargets[8]; // 1c0
    KartDisp *mKartDisps[8]; // 1e0
    KartCam *mKartCams[8]; // 200

public:
    u8 _220[4]; // unused
    u32 mBitfield;
    u32 _228;
    int mKartCount;
}; // size: 0x230

inline KartCtrl *GetKartCtrl() {
    return KartCtrl::getKartCtrl();
}

#endif
