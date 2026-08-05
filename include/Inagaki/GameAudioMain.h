#ifndef GAMEAUDIOMAIN_H
#define GAMEAUDIOMAIN_H

#include <JSystem/JKernel/JKRHeap.h>
#include <JSystem/JAudio/JAUSectionHeap.h>

#include "Inagaki/GameAudioAudience.h"
#include "JSystem/JAudio/Interface/JAISound.h"
#include "kartEnums.h"
#include "Inagaki/GameAudioFxConfig.h"
#include "Inagaki/GameSoundTable.h"
#include "types.h"

class CrsArea;

namespace GameAudio {

class CustomMgr;
class CameraMgr;

class Main
{
public:
    enum WS_ID // Data from GCKart.daa, might not be correct
    {
        STREAM0, // NINTENDO_LOGO_MARIO_WS_START?
        STREAM1, // SE_WS_START?
        STREAM2, // not used
        STREAM3  // TANAKA_BGM_WS_START?
    };

    enum PAUSE_STATUS
    {
        // TODO
    };

    Main();

    void init(JKRSolidHeap *, u32, void *, void *, u32);
    void bootDSP();
    bool isActive();
    void initRaceSound();
    void frameWork();
    void changeSection(u32);
    void checkSection();
    bool isWaveLoaded(WS_ID);
    void checkCourceOnRace();
    void setPause(PAUSE_STATUS);
    void startSequenceBgm(u32);
    void stopSequenceBgm(u32);
    u32 getPlayingSequenceID();
    void prepareStreamBgm(u32);
    void startStreamBgm(u32);
    void stopStreamBgm(u32);
    u32 getPlayingStreamID();
    void setInvincibleBgm(u8, u8);
    void clearInvincibleBgm(u8, u8);
    void startInvincibleBgm(u8, bool);
    void checkBgm();
    void startSystemSe(u32 id);
    void stopSystemSe(u32 id);
    void startRaceSystemSe(u32);
    void startKartSystemSe(u8, u32);
    void checkThunderMode();
    void setThunderMode(u8);
    void clearThunderMode(u8);
    void startBombPointSe(u8, u8);
    void startBombReachSe(u8, u8);
    void setMasterVolume(s8);
    f32 getMasterVolumeValue();
    void setBgmVolume(f32);
    void updateBgmVolume(u32);
    void fadeOutAll(u32);
    void setRaceSeVolume(f32, u32);
    void setOutputMode(u32);
    void resetAudio(u32);
    void resumeAudio();
    void checkResetInner();
    void setGoalProcess();
    void processSeqCallback(JASTrack *, u16);

    f32 getTHPOptionVolume() { return getMasterVolumeValue() / 2; };
    JAUSectionHeap *getSectionHeap() { return mSectionHeap; }
    CustomSoundTable *getSoundTable() { return mTable; }

    // Inline/Unused
    // Main();
    // void clearWork();
    // void changeSection(char *);
    // void changeSection(void *);
    // void muteStreamBgm(f32, u32);
    // void changeStreamPlayTrack(u8);
    // void setSeVolume(f32);
    // void checkReset();

    void setFxLine(u8 lineNum) { // fabricated
        mConfig->set(lineNum);
    }

    void set_50(int val) { // fabricated
        _50 = val;
    }

    CameraMgr *getCamera() {
#line 157
        JUT_ASSERT_MSG(mCamera, "GameAudioMain : カメラが初期化されていません。\n");
        return mCamera;
    }
    
    static Main *getAudio() { return msBasic; };

private:
    static Main sJAudio;
    static Main *msBasic;

    CustomMgr *mMgr;              // 00
    CustomAudience<4> *mAudience; // 04
    u8 _08[0xC - 0x8];            //
    CustomSoundTable *mTable;     // 0C
    u8 _10[4];                    //
    JAUSectionHeap *mSectionHeap; // 14
    u8 _18[0x24 - 0x18];          //
    JAISoundHandle _24[4];        //
    JAISoundHandle _34[4];        //
    FxLineConfig *mConfig;        // 44
    u8 _48[4];                    //
    PAUSE_STATUS mPauseStatus;    // 4C
    int _50;                      //
    u8 _54[0x64 - 0x54];          //
    CameraMgr *mCamera;           // 64
    JAISoundHandle _68;           //
    JAISoundHandle _6c;           //
    JAISoundHandle _70;           //
    u8 _74[0xa8 - 0x74];          //
    JAISoundHandle _84[4];        // 
    u8 _94[0xa8 - 0x94];          //

}; // Size: 0xa8

namespace Parameters
{
    void setPlayerMode(u8);
    void setRaceMode(u8);
    void setDemoMode(u8);
    void setKillSwAllObject(bool);
    void setRaceCourse(ECourseID);
    void setKartType(u8, EKartWeight);
    void setNetworkMode(u8, u8);
    void setChrType(u8, EKartID);
    void setChibiFlag(u8, bool);
    void setMirrorSwitch(bool);
    ERaceMode getRaceMode();
    u8 getPlayerMode();
    int getEngineType(u8);
    int getCharacterType(u8);
    ECourseID getRaceCourse();
    f32 getChibiPitch(u32);
    u8 getNetworkCubes();
    bool getChibiFlag(u8);
    u8 getDemoMode();
    bool getMirrorSwitch();
    extern f32 sChibiPitch[4];
    extern u8 sDemoMode;
    extern bool sMirrorSwitch;
    extern u8 sKartType[8];
    extern u8 sChrType[8];
    extern u8 sEngType[8];
    extern bool sKartChibiFlag[8];
    extern u8 sPlayerMode;
    extern u8 sRaceMode;
    extern u8 sNetworkCubes;
    extern u8 sNetworkNumber;
    extern ECourseID sRaceCourse;
}

extern const f32 GA_PLAY_SE_VOLUME_1P;
extern const f32 GA_PLAY_SE_VOLUME_2P;
extern const f32 GA_PLAY_SE_VOLUME_3P;
extern const f32 GA_PLAY_SE_VOLUME_4P;

extern int GoalFanfareDelayFrame;
extern int ParadeBgmMuteFrame;
extern f32 ParadeBgmMuteVolume;
}

#define GetGameAudioMain() GameAudio::Main::getAudio()
#define GameAudioMain GameAudio::Main
#define GameAudioParameters GameAudio::Parameters

#endif
