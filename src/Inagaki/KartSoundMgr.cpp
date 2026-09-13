#include "Inagaki/GameSoundMgr.h"

#include "Inagaki/GameAudioAudience.h"
#include "Inagaki/GameAudioCommon.h"
#include "Inagaki/GameAudioCamera.h"
#include "Inagaki/GameAudioMain.h"
#include "Inagaki/GameSoundTable.h"

#include "JSystem/JAudio/Interface/JAIAudible.h"
#include "JSystem/JAudio/Interface/JAISound.h"
#include "JSystem/JAudio/JAUSoundObject.h"
#include "JSystem/JAudio/System/JASGadget.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/JAudio/JASFakeMatch2.h"

#include "Kaneshige/Course/CrsArea.h"
#include "Kaneshige/Course/CrsGround.h"
#include "kartEnums.h"

namespace GameAudio {

const s16 SignalUpPitchWidth[3] = { 0x0064, 0x0082, 0x0078 };
const s16 SignalUpPitchBase[3] = { 0x0032, 0x0046, 0x0050 };
const s16 SignalUpIntervalWidth[3] = { 0x0046, 0x001E, 0x000A };
const s16 SignalUpIntervalBase[3] = { 0x0014, 0x000A, 0x0005 };
const s16 SignalDownIntervalWidth[3] = { 0x0050, 0x0032, 0x0014 };
const s16 SignalDownIntervalBase[3] = { 0x0014, 0x000A, 0x0005 };

// TODO
static const u8 cKartRankClassTable0[7] = {0, 0, 1, 1, 1, 2, 2};
static const u8 cKartRankClassTable1[7] = {0, 0, 1, 1, 2, 2, 2};

u32 BoundSe[0x19];
u32 WheelSpinSe[0x19];
u32 SpinSe[0x19];
u32 SpinTurnSe[0x19];

f32 GA_ENEMY_VOLUME_DOWN_VALUE = 0.85f;


const f32 EngineKeisuuRaceUp[] = {
    0.006f, 0.006f, 0.005f, 0.003f,
    0.015f, 0.015f, 0.013f, 0.011f,
    0.02f,
};

const f32 EngineKeisuuRaceDown[] = {
    0.005f, 0.003f, 0.005f, 0.002f,
    0.012f, 0.012f, 0.012f, 0.009f,
    0.019f,
};

const f32 DashEngineAdjustInitialValue[] = {
    0.3f, 0.3f, 0.3f, 0.3f,
    0.3f, 0.3f, 0.3f, 0.3f,
    0.3f
};

const f32 DashEngineIncPerFrame[] = {
    0.025f, 0.025f, 0.025f, 0.095f,
    0.025f, 0.025f, 0.025f, 0.025f,
    0.025f
};

const f32 DashEngineDecPerFrame[] = {
    0.005f, 0.005f, 0.005f, 0.005f,
    0.005f, 0.005f, 0.005f, 0.005f,
    0.005f
};

u8 KartSoundMgr::smKartCount;
u8 KartSoundMgr::smEntryKartCount;
u8 KartSoundMgr::smGoalKartCount;

u8 KartSoundMgr::smKartRankClassMem[7] = {};

KartSoundMgr::KartSoundMgr(Vec *pos, JKRHeap *heap, u8 p3, u8 p4)
    : SoundMgr(pos, heap, 12) {
    _114 = 1.f;
    _110 = 1.f;
    _11c = 0;
    mCameraVolume = 1.f;
    mGoalVolume = 1.f;
    mGoalVolumeCounter = 0;
    _C = 0xff;

    _61 = p3;

    if(p4 == 0) {
        mKartCount = smKartCount++;
    }
    else {
        mKartCount = 4;
    }

    if(p4 != 2) {
        smEntryKartCount++;
    }

    _66 = p4;
    _130 = new CrsArea();
    u8 index = 0;

    if(_66 == 0) {
        _64 = 3;
    }
    else {
        u32 randomValue = GameAudio::Random::getSignalEngineRandomU32();
        u8 rankClassIndex = randomValue % 7;

        while(smKartRankClassMem[rankClassIndex] == 1 && index < 7)
        {
            index++;

            if(++rankClassIndex == 7)
            {
                rankClassIndex = 0;
            }
        }

        smKartRankClassMem[rankClassIndex] = 1;
        u8 playerMode = Parameters::getPlayerMode();
        if(playerMode != 1)
        {
            _64 = cKartRankClassTable0[rankClassIndex];
        }
        else {
            _64 = cKartRankClassTable1[rankClassIndex];
        }
    }

    init();

    _7c = 0;

    const u32 sceneMax = Main::getAudio()->getCamera()->getSceneMax();

    if(sceneMax > 1 && sceneMax > mKartCount)
    {
        _7c = (1 << mKartCount) ^ 0xf;
    }
}

KartSoundMgr::~KartSoundMgr() {
    setChibiFlag(false, false);
    for(u8 index = 0; index < 7; index++)
    {
        smKartRankClassMem[index] = 0;
    }

    if(_66 == 0)
    {
        const s32 kartCount = mKartCount;
        if(kartCount < 4)
        {
            CustomAudience<4>::smCameraVolume[kartCount] = 1.f;
        }
    }

    smKartCount = 0;
    smEntryKartCount = 0;
    smGoalKartCount = 0;
}

void KartSoundMgr::startSoundHandleNumber(u8 handleIndex, u32 soundID, u32 fadeCount) {
    if(mKillSw || _66 == 2)
    {
        return;
    }

    JAISoundStarter* soundStarter = JASGlobalInstance<JAISoundStarter>::getInstance();

    JAISoundHandle& handle = (*this)[handleIndex];

    soundStarter->startSound(soundID, &handle, NULL);

    JAISound* sound;

    if(!handle.isSoundAttached())
    {
        return;
    }

    if(handle->audible_ == NULL)
    {
        Main* main = Main::getAudio();
        u32 scene = 0;
        CameraMgr* camera = main->getCamera();
        u32 sceneMax = camera->getSceneMax();

        if(sceneMax > 1 && sceneMax > mKartCount)
        {
            scene = (1 << mKartCount) ^ 0xf;
        }

        sound = handle.operator->();

        JGeometry::TVec3f vec(*mSoundPos);
        sound->newAudible(vec, &_18, scene, NULL);
    }
    sound = handle.operator->();

    sound->fader_.fadeInFromOut2(fadeCount);

    setEcho(&handle, _6c);
}

void KartSoundMgr::dispose() {
    JAUSoundObject::dispose();

    clearInvincibleBgm(3);
}

void KartSoundMgr::startSoundEngine(u8, u32) {} // UNUSED

void KartSoundMgr::init() {
    _60 = 0;
    _5d = 0;
    _68 = 0.f;
    _63 = 0;
    _5c = 0;
    _6c = 0.f;
    _70 = 0.f;
    _80 = 0;
    _84 = 0.f;
    _88 = 0.f;
    _8c = 0;
    _8d = 0;
    _8e = 0;
    _90 = 0;
    _92 = 0;
    _94 = 0;
    _96 = 0;
    _98 = 0.f;
    _9c = 0;
    _a0 = 0.f;

    for(u8 index = 0; index < 4; index++)
    {
        _a4[index] = 0.f;
        _b4[index] = 0.f;
        _c4[index] = 0.f;
        _e4[index] = 0;
        mWaterDepths[index] = 0.f;
        _e8[index] = 0;
    }

    _ec = 0.f;
    _f0 = 0.f;
    _f4 = 0;
    _f8 = 0;
    _100 = 0;
    _101 = 0;
    _102 = 0;
    _103 = 0;
    _104 = 0;
    _105 = 1;
    _fc = 0.f;
    _5f = 0;
    _114 = 0.f;
    _110 = 0.f;
    _11c = 0;
    mCameraVolume = 1.f;
    mGoalVolume = 1.f;
    mGoalVolumeCounter = 0.f;
    _78 = 0;
    _5e = 0;
    _74 = 0;
    _65 = 0xff;

    if(_66 == 0)
    {
        const s32 kartCount = mKartCount;
        const f32 volume = mCameraVolume;
        if(kartCount < 4)
        {
            CustomAudience<4>::smCameraVolume[kartCount] = volume;
        }
    }

    Main* main = Main::getAudio();
    CustomSoundTable* soundTable = main->getSoundTable();
    for(s32 index = 0; index < _10; index++)
    {
        if(!(*this)[index].isSoundAttached())
        {
            continue;
        }
        JAISound* sound = (*this)[index].operator->();
        u32 swBit = soundTable->getSwBit(sound->getID().mId.mFullId);
        if(!(swBit & 0x00800000))
        {
            continue;
        }
        (*this)[index]->stop();
    }

    setChibiFlag(false, false);
    clearInvincibleBgm(3);
    smGoalKartCount = 0;
}

void KartSoundMgr::changeAttribute(u8) {} // UNUSED

void KartSoundMgr::changeDriver(bool) {}

void KartSoundMgr::checkCourseSound(u8) {} // UNUSED

void KartSoundMgr::frameWork(u8 p1) {
    if(_66 == 0) {
        _130->search(6, *mSoundPos);
        Course::Area* area = _130->getArea();
        if(area != NULL) {
            ECourseID courseID = Parameters::getRaceCourse();
            switch(courseID) {
            case 0x2a:
                if(p1 <= 3)
                {
                    JAISoundHandle& handle = (*this)[7];
                    if(!handle.isSoundAttached())
                    {
                        startSoundHandleNumber(7, 0x40024, 0);
                    }
                }
                break;

            case 0x29:
                _5c = 1;
                _6c = 0.5f;
                break;
            }
        }
        else {
            ECourseID courseID = Parameters::getRaceCourse();
            switch(courseID) {
            case 0x29:
                _5c = 0;
                break;
            }
        }
    }
    checkEcho();

    if(mKillSw || _66 == 2) {
        return;
    }

    crushRenzokuTaisaku();
    slipParamSet();
    if(_63 != 3) {
        _65 = p1;
    }
    checkAfterGoalVolume();
}

void KartSoundMgr::checkAfterGoalVolume() {
    if(_66 != 0) {
        return;
    }

    if(_5e != 1)
    {
        return;
    }

    if(_78 == 0) {
        JAISoundHandle& handle = (*this)[3];
        if(handle.isSoundAttached())
        {
            handle->stop();
        }

        Main* main = Main::getAudio();
        CameraMgr* camera = main->getCamera();

        if(camera->getSceneMax() == 2) {
            u8 mode = Parameters::getRaceMode();
            if(mode == 1) {
                _7c = 0xc;
                _80 = 0x78;
            }
        }
    }
    else if (_78 == 0x3c) {
        const f32 goalVolumeOn = 0.35f;
        if(mGoalVolumeCounter != 0 || !isCameraVolumeEqual(goalVolumeOn))
        {
            if(mGoalVolumeCounter == 0 || !isGoalVolumeEqual(goalVolumeOn)) {
                mGoalVolume = goalVolumeOn;
                mDeltaVolume = (mCameraVolume - mGoalVolume) / 61.f;
                mGoalVolumeCounter = 0x3d;
            }
        }
    }
    else if (_78 == 0x168) {
        const f32 goalVolumeOff = 0.0f;
        if(mGoalVolumeCounter != 0 || !isCameraVolumeEqual(goalVolumeOff))
        {
            if(mGoalVolumeCounter == 0 || !isGoalVolumeEqual(goalVolumeOff))
            {
                mGoalVolume = goalVolumeOff;
                mDeltaVolume = (mCameraVolume - mGoalVolume) / 301.f;
                mGoalVolumeCounter = 0x12d;
            }
        }
    }
    _78++;

    u8 camera;
    if(mGoalVolumeCounter == 0)
    {
        camera = 0;
    }
    else {
        if(--mGoalVolumeCounter){
            camera = 1;
            mCameraVolume -= mDeltaVolume;
        }
        else {
            camera = 1;
            mCameraVolume = mGoalVolume;
        }
    }

    if(camera == 0)
    {
        return;
    }

    s32 kartCount = mKartCount;
    f32 cameraVolume = mCameraVolume;

    if(kartCount >= 4){
        return;
    }

    CustomAudience<4>::smCameraVolume[kartCount] = cameraVolume;
}

void KartSoundMgr::setHandleVolume(JAISoundHandle&, f32) {} // UNUSED

void KartSoundMgr::setWaterDepth(f32) {} // UNUSED

void KartSoundMgr::setWaterDepth(u8 index, f32 depth) {
    mWaterDepths[index] = depth;
}

void KartSoundMgr::setSlip(u8 wheel, u8 r5, u8 r6, f32 slip) {
    #line 907
    JUT_ASSERT_MSG(wheel < 4, "KartSoundMgr::setSlip wheel ERROR!!!\n");

    _e8[wheel] = r5;
    if(mKillSw || _66 == 2)
    {
        return;
    }

    if(_66 != 0)
    {
        return;
    }

    f32 f28 = slip;
    u32 r27 = r5;

    f32 f30 = 0.f;
    f32 f31 = 1.f;
    f32 f29;

    u8 r6_2;
    switch(r5) {
    case 0x11:
        {
            if(mWaterDepths[wheel] <= f30)
            {
                switch(r6) {
                    case 0xa:
                        f29 = 0.1f;
                        r6_2 = 0x14;
                        break;
                    case 0x1c:
                        f29 = 0.1f;
                        r6_2 = 0x15;
                    break;
                    default:
                        f29 = 0.f;
                        r6_2 = 0xff;
                }
            }
            else {
                f29 = 0.f;
                r6_2 = 0xa;
            }
        }
        break;
        default:
        {
            switch(r6)
            {
                case 1:
                    if(r27 == 1)
                    {
                        f29 = 0.1f;
                    }
                    else {
                        f29 = -0.1f;
                    }
                    r6_2 = 0;
                    break;
                case 0:
                    f29 = 0.1f;
                    u8 type = Parameters::getCharacterType(_61);
                    if(type == 1){
                        r6_2 = 9;
                    }
                    else {
                        r6_2 = 1;
                    }
                    break;
                case 7:
                    if(r27 == 1)
                    {
                        f29 = -0.06f;
                    }
                    else {
                        f29 = -0.1f;
                    }
                    r6_2 = 2;
                    break;
                case 6:
                    if(r27 == 1)
                    {
                        f29 = f30;
                    }
                    else {
                        f29 = 0.1f;
                    }
                    r6_2 = 3;
                    break;
                case 2:
                    f29 = 0.1f;
                    r6_2 = 4;
                    break;
                case 8:
                    f29 = 0.1f;
                    r6_2 = 0x13;
                    break;
                case 3:
                    f29 = -0.1f;
                    r6_2 = 5;
                    break;
                case 5:
                    f29 = -0.1f;
                    r6_2 = 6;
                    break;
                case 14:
                    f29 = 0.1f;
                    r6_2 = 0xd;
                    break;
                case 20:
                    f29 = 0.1f;
                    r6_2 = 0xf;
                    break;
                case 22:
                    f29 = -0.06f;
                    r6_2 = 0xc;
                    break;
                case 13:
                    f29 = -0.06f;
                    r6_2 = 0xe;
                    break;
                case 10:
                    f29 = 0.1f;
                    r6_2 = 0x14;
                    break;
                case 28:
                    f29 = 0.1f;
                    r6_2 = 0x15;
                    break;
                case 12:
                    f29 = 0.1f;
                    r6_2 = 0xb;
                    break;
                case 21:
                    f29 = 0.1f;
                    r6_2 = 0x10;
                    break;
                case 11:
                    f29 = 0.1f;
                    r6_2 = 0x11;
                    break;
                case 23:
                    f29 = 0.1f;
                    r6_2 = 0x16;
                    break;
                case 24:
                    f29 = 0.1f;
                    r6_2 = 0x17;
                    break;
                case 4:
                case 9:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 25:
                case 26:
                case 27:
                default:
                    f29 = 0.f;
                    r6_2 = 0xff;
                    break;
            }
            // 294
        }
    }

    _105 = r6_2;

    if(r6_2 != 0xff && _8d != 0)
    {
        f32 f4 = _84;
        f32 f0 = 5.f;

        if(f4 < f0)
        {
            return;
        }
        f32 f1 = 0.7;
        f32 f2;
        f32 f5;
        f32 f3;
        u32 r0;

        switch(r5)
        {
            case 0:
            case 1:
            case 0x0c:
                switch(r6)
                {
                    default:
                    case 3:
                        f1 = 0.0026666666f;
                        f0 = 0.7f;
                        f31 = (f1 * f4) + f0;
                    case 11:
                    case 12:
                    case 20:
                    case 28:
                        f0 = f28 - f29;
                        f1 = 0.2f;
                        f30 = f1 + f0;
                        break;
                    case 0:
                    case 1:
                    case 4:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 15:
                    case 16:
                    case 17:
                    case 18:
                    case 19:
                    case 21:
                    case 23:
                    case 24:
                    case 25:
                    case 26:
                    case 27:
                        f1 = 0.0026666666f;
                        f0 = 0.7f;
                        f31 = (f1 * f4) + f0;
                    case 2:
                    case 5:
                    case 10:
                    case 13:
                    case 14:
                    case 22:
                        f1 = 70.f;
                        f0 = 1.f;
                        f2 = f4 / f1;
                        if(f2 > 1.f)
                        {
                            f2 = f0;
                        }
                        f30 = f2 * (f28 - f29 + 0.4f);
                        break;
                }
                break;

                break;
            case 0x11:
                switch(r6_2)
                {
                    case 0x14:
                    case 0x15:
                        f31 = (f4 * 0.0026666666f) + 0.7f;
                        f30 = (f28 - f29) + 0.2f;
                        break;
                    case 0xa:
                        f0 = 40.f;
                        f1 = mWaterDepths[wheel];
                        if(f1 > 40.f)
                        {
                            f1 = f0;
                        }

                        f5 = (40.f - f1) / 40.f;
                        if(f4 < 10.f)
                        {
                            f30 = 0.f;
                        }
                        else {
                            f1 = 70.f;
                            f0 = 1.f;
                            f4 /= f1;
                            if(f4 > f0)
                            {
                                f4 = f0;
                            }
                            f0 = 0.53f;
                            f3 = 1.2f;
                            f1 = f0 * f4;
                            f2 = 0.2f;
                            f0 = 0.79f;
                            f30 = (f3 * f4) + f2;
                            f31 = (f5 * f1) + f0;
                        }
                    break;
                }
                break;

            default:
                f1 = 70.f;
                f0 = 1.f;
                f2 = f4 / f1;
                if(f2 > f0)
                {
                    f2 = f0;
                }
                f30 = f2 * (f28 - f29 + 0.4f);
                break;
        } // 438 ?
        f0 = 0.1f;
        if(f30 <= f0){
            return;
        }
        f0 = 1.5f;
        if(f30 > f0){
            f30 = f0;
        }
        _e4[wheel] = 1;

        _b4[wheel] = f30;

        bool isRightWheel = wheel & 1;
        if(isRightWheel)
        {
            f0 = 0.47f;
            _a4[wheel] = f0;
        }
        else {
            f0 = 0.53f;
            _a4[wheel] = f0;
        }
        _c4[wheel] = f31;

        switch(wheel)
        {
            case 0:
                _104 = r6_2;
                break;
            case 1:
            case 2:
            case 3:
                r0 = _104;
                if(r0 != 6 && r0 != 5 && r0 != 10)
                {
                    _104 = r6_2;
                }
                break;
        }
    }
    //4d8
}

void KartSoundMgr::setConductStatus(f32 f1, f32 f2, bool r4, bool r5, bool r6, u8 r7, CrsArea *r8) {
    if(r8 != NULL && _5c == 0)
    {
        f32 rate = r8->getRate();
        f32 zeroValue = 0.f;
        if(rate != zeroValue)
        {
            _6c = 0.8f * r8->getEchoRate();
            if(_6c > 0.8f)
            {
                _6c = 0.8f;
            }
        }
        else {
            _6c = zeroValue;
        }
    }

    if(mKillSw || _66 == 2)
    {
        return;
    }

    _88 = f1;
    _84 = f2;

    if(r6)
    {
        if(f2 < 1.0f)
        {
            r6 = 0;
        }
    }

    if(r6)
    {
        _8c = r5;
    }
    else {
        _8c = r4;
    }
    _8d = r7;

    u8 characterType = Parameters::getCharacterType(_61);

    if(characterType != 9)
    {
        ECourseID id;
        switch(_8d)
        {
            case 0:
                setConductSignal();
                setWaterCutoffPort(0);
                break;
            case 1:
                setConductOutOfCourse(r7);
                setWaterCutoffPort(0);
                if(_66 != 0)
                {
                    break;
                }
                if(_63 == r7)
                {
                    break;
                }
                id = Parameters::getRaceCourse();
                if(id != 0x2f)
                {
                    break;
                }
                startSoundHandleNumber(7, 0x40074, 0);
                break;
            case 2:
                setConductTrouble(f1, r7);
                setWaterCutoffPort(0);

                if(_66 != 0)
                {
                    break;
                }
                if(_63 == r7)
                {
                    break;
                }

                id = Parameters::getRaceCourse();
                if(id == 0x2a)
                {
                    startSoundHandleNumber(7, 0x40057, 0);
                    break;
                }
                id = Parameters::getRaceCourse();
                if(id != 0x2f)
                {
                    break;
                }
                startSoundHandleNumber(7, 0x40074, 0);
                break;
            case 3:
                countGoalKart();
                _8c = 1;
                setConductAfterGoal(r6);
                _5e = 1;
                break;
            case 4:
                setConductRace(r6);
                // _66
                break;
            case 5:
                if(_66 == 0 && _63 != r7 && Parameters::getRaceCourse() == 0x2f)
                {
                    startSoundHandleNumber(7, 0x40074, 0);
                }
                setConductPressed();
                break;
            case 6:
            default:
                break;
        }
    }
    else {
        ECourseID id;
        switch(_8d)
        {
            case 0:
            case 2:
            case 4:
                if(_63 != r7)
                {
                    _8e = 0;
                    _92 = 0;
                }
                setConductLocomotiveAccel();
                if(_66 != 0)
                {
                    break;
                }
                if(_63 == r7)
                {
                    break;
                }
                id = Parameters::getRaceCourse();
                if(id == 0x2a)
                {
                    startSoundHandleNumber(7, 0x40057, 0);
                    break;
                }
                id = Parameters::getRaceCourse();
                if(id == 0x2f)
                {
                    startSoundHandleNumber(7, 0x40074, 0);
                }
                break;

            case 3:
                countGoalKart();
                _8c = 1;
                _5e = 1;

            case 1:
                if(_63 != r7)
                {
                    _8e = 0;
                    _92 = 0;
                }

                setConductLocomotiveSpeed(r6);
                break;
            case 5:
                if(_66 == 0 && _63 != r7)
                {
                    id = Parameters::getRaceCourse();
                    if(id == 0x2f)
                    {
                        startSoundHandleNumber(7, 0x40074, 0);
                    }
                }

                setConductPressed();
                break;
            case 6:
            default:
                break;
        }
    }

    _68 = f1;
    _63 = r7;
    _98 = _84;
}

void KartSoundMgr::setWaterCutoffPort(u16 port) {
    JAISoundHandle &handle = (*this)[3];
    if (handle.isSoundAttached()) {
        handle->getTrack()->writePort(0xa, port);
    }
}

void KartSoundMgr::getEngineIDOffsetAtt() {} // UNUSED

void KartSoundMgr::countGoalKart() {
    if(_5e != 0){
        return;
    }

    if(_63 == _8d)
    {
        return;
    }
    smGoalKartCount++;
}

// FABRICATED {
void KartSoundMgr::startSoundFromID(u32 id)
{
    u32 r6;
    JAISound* sound;

    JAISoundStarter* soundStarter = JASGlobalInstance<JAISoundStarter>::getInstance();

    JAISoundHandle& handle = (*this)[3];

    id += (_66 != 0 ? 0x14 : 0);

    soundStarter->startSound(
        id,
        &handle,
        NULL);

    if(handle.isSoundAttached())
    {
        JAIAudible* audible = handle->getAudible();
        if(audible == 0)
        {
            r6 = _7c;

            sound = handle.operator->();
            JGeometry::TVec3f vec(*mSoundPos);

            sound->newAudible(vec, &_18, r6, NULL);
            if(_80 != 0){

                handle->fader_.fadeInFromOut2(_80);
                _80 = 0;
            }
        }
        setEcho(&handle, _6c);
    }
}
// } FABRICATED

void KartSoundMgr::setConductLocomotiveAccel() {
    u32 r6;
    JAISound* sound;
    f32 r4;
    JAISoundStarter* soundStarter;
    f32 f31;

    bool changed = false;

    if(_8c != 0)
    {
        if(_8e == 0)
        {
            if(_92 == 0)
            {
                _92 = 9;
            }
            else if(_92 > 4) {
                _92--;
            }
            changed = true;
            f31 = 0.6f;
            _8e = _92;
        }
    }
    else if(_8e == 0)
    {
        if(_92 == 0)
        {
            _92 = 9;
        }
        if(_92 < 9){
            _92++;
            _8e = _92;
        }
        else {
            _8e = _92;
        }
        changed = true;
        f31 = 0.4f;
    }

    if(changed)
    {
        const u32 soundID = ((_96++ & 1) ? 0x11 : 0x12);

        startSoundFromID(soundID);
        JAISoundHandle& handle_2 = (*this)[3];
        if(!handle_2.isSoundAttached())
        {
            return;
        }
        handle_2->getAuxiliary().moveVolume(f31, 0);
        r4 = 0.6f + ((0.4f * (9 - _92)) / 5.f);
        handle_2->getAuxiliary().movePitch(r4, 0);
    }
    else {
        if(_8e != 0)
        {
            _8e--;
        }
    }
}


void KartSoundMgr::setConductLocomotiveSpeed(bool) {
    u32 r6;
    JAISound* sound;
    f32 r4;
    f32 f31;
    u32 soundID;

    f31 = _84;
    if(_8e == 0)
    {
        if(f31 < 1.f)
        {
            f31 = 1.f;
        }
        if(f31 > 130.f)
        {
            f31 = 130.f;
        }
        _8e = (u16) (3.f + ((130.f - f31) / 20.f));

        soundID = 0x12 + -(_96++ & 1);

        startSoundFromID(soundID);

        JAISoundHandle* handle = &(*this)[3];
        if(!handle->isSoundAttached())
        {
            return;
        }
        JAISound* sound = handle->operator->();
        sound->getAuxiliary().moveVolume((0.7f * (f31 / 130.f)) + 0.5f, 0);
        f32 pitch = 1.f;
        if(f31 < 100.f) {
            pitch = f31 / 100.f;
        }
        if(_5d != 0)
        {
            f32 chibiPitch = Parameters::getChibiPitch(soundID);
            pitch *= chibiPitch;
        }
        r4 = 0.6f + ((0.4f * (9 - _92)) / 5.f);
        (*handle)->getAuxiliary().movePitch((0.4f * pitch) + 0.6f, 0);
    }
    else {
        _8e--;
    }
}

void KartSoundMgr::setConductPressed() {
    startSoundFromID(0x13);

    f32 pitch = _84;
    if(pitch > 100.f)
    {
        pitch = 100.f;
    }

    pitch = 1.f + (pitch / 100.f);

    JAISoundHandle& handle = (*this)[3];
    if(!handle.isSoundAttached())
    {
        return;
    }

    handle->getAuxiliary().movePitch(pitch, 0);
}

static const f32 EngineKarabukashiLength[] = {
    90.f, 40.f, 90.f, 30.f,
    40.f, 35.f, 30.f, 95.f,
    90.f
};

static const f32 EnginePitchKeisuuSignal[] = {
    0.02f, 0.05f, 0.02f, 0.09f,
    0.05f, 0.06f, 0.06f, 0.02f,
    0.05f
};

static const f32 EnginePitchDownSignal[] = {
    0.9f, 0.9f, 0.6f, 0.9f,
    0.9f, 0.7f, 0.7f, 0.6f,
    0.8f
};

static const f32 EnginePitchKeisuuOutOfCourse[] = {
    0.01f, 0.01f, 0.01f, 0.01f,
    0.01f, 0.01f, 0.01f, 0.01f,
    0.01f
};

static const f32 EnginePitchKeisuuTrouble[] = {
    0.025f, 0.025f, 0.025f, 0.025f,
    0.025f, 0.025f, 0.025f, 0.025f,
    0.025f
};

static const f32 EngineAddKeisuuRaceUp[] = {
    0.0006f, 0.0006f, 0.0006f, 0.0005f,
    0.0012f, 0.0012f, 0.0012f, 0.0012f,
    0.0012f
};

static const f32 UpEngineLoopStart[] = {
    100639.f, 133568.f, 121677.f, 89762.f,
    0.f, 0.f, 0.f, 0.f,
    0.f
};

static const f32 UpEngineLoopEnd[] = {
    113023.f, 167266.f, 138431.f, 100738.f,
    0.f, 0.f, 0.f, 0.f,
    0.f
};

static const f32 EngineAddKeisuuRaceDown[] = {
    0.00015f, 0.00015f, 0.00015f, 0.00015f,
    0.0006f, 0.0006f, 0.0006f, 0.0006f,
    0.0006f
};

static const f32 DownEngineLoopStart[] = {
    42032.f, 57452.f, 54457.f, 33775.f,
    0.f, 0.f, 0.f, 0.f,
    0.f
};

static const f32 DownEngineLoopEnd[] = {
    93600.f, 104350.f, 79743.f, 63943.f,
    0.f, 0.f, 0.f, 0.f,
    0.f
};

void KartSoundMgr::setConductSignal() {
    if(_63 != _8d)
    {
        _fc = 0.f;
    }

    f32 pitch;
    f32 volume;
    if(_66 == 0 || Parameters::getNetworkCubes() != 1)
    {
        if(_8c == 1)
        {
            volume = 1.4f;
            f32 length = EngineKarabukashiLength[(u8)Parameters::getEngineType(_61)];
            if(_fc < length)
            {
                _fc = _fc + 1.f;
            }

            f32 signal = EnginePitchKeisuuSignal[(u8)Parameters::getEngineType(_61)];
            pitch = (_fc * signal) + 0.7f;
        }
        else {
            volume = 0.7f;
            if(_fc > 0.f){
                f32 signal = EnginePitchDownSignal[(u8)Parameters::getEngineType(_61)];
                _fc -= signal;
            }
            else {
                _fc = 0.f;
            }
            f32 signal = EnginePitchKeisuuSignal[(u8)Parameters::getEngineType(_61)];
            pitch = (_fc * signal) + 0.7f;
        }
        if(_fc < 20.f)
        {
            volume = ((-0.015000001f * _fc) + volume) + 0.3f;
        }
    }
    else {
        if(_11c == 0)
        {
            _5f ^= 1;
            if(_5f != 0)
            {
                u32 interval = SignalUpIntervalBase[_64] + (Random::getSignalEngineRandomU32() % SignalUpIntervalWidth[_64]);
                u32 pitchInt = SignalUpPitchBase[_64] + (Random::getSignalEngineRandomU32() % SignalUpPitchWidth[_64]);
                f32 currentPitch = f32(pitchInt) / 100.f;
                if((_11c != 0 || _114 != currentPitch) && (_11c == 0 || _110 != currentPitch))
                {
                    _110 = currentPitch;
                    if(interval == 0)
                    {
                        _114 = currentPitch;
                    }
                    else {
                        _118 = (_114 - _110) / (f32)(interval + 1);
                        _11c = interval + 1;
                    }
                }
            }
            else {
                u32 interval = SignalDownIntervalBase[_64] + (Random::getSignalEngineRandomU32() % SignalDownIntervalWidth[_64]);
                f32 volumeC = 0.7f;
                if ((_11c != 0 || _114 != volumeC) && (_11c == 0 || _110 != volumeC))
                {
                    _110 = 0.7f;
                    if (interval == 0)
                    {
                        _114 = 0.7f;
                    }
                    else
                    {
                        u32 ticks = interval + 1;
                        _118 = (_114 - _110) / (f32)ticks;
                        _11c = ticks;
                    }
                }
            }
        }

        if(_11c != 0)
        {
            if(--_11c != 0){
                _114 -= _118;
            }
            else {
                _114 = _110;
            }
        }
        if(_5f != 0)
        {
            volume = 1.4f;
        }
        else {
            volume = 0.7f;
        }

        pitch = _114;
        volume = volume * GA_ENEMY_VOLUME_DOWN_VALUE;
    }

    u8 engineType = Parameters::getEngineType(_61);
    startSoundFromID(engineType + 8);

    JAISoundHandle& handle = (*this)[3];
    if(handle.isSoundAttached())
    {
        handle->getAuxiliary().moveVolume(volume, 0);
        handle->getAuxiliary().movePitch(pitch, 0);
    }
}

void KartSoundMgr::setConductOutOfCourse(u8 r4) {
    JAISound* sound;
    f32 volume;
    u32 soundID;

    if(_63 != r4)
    {
        _94 = 0;
    }
    if(_8c == 1)
    {
        if(_94 < 0x64)
        {
            _94++;
            _94++;
        }
        volume = 1.2f;
    }
    else {
        if(_94 != 0)
        {
            _94--;
        }
        volume = 0.55f;
    }

    f32 pitch = (EnginePitchKeisuuOutOfCourse[(u8)Parameters::getEngineType(_61)] * _94) + 0.6f;

    if(_66 != 0){
        volume *= GA_ENEMY_VOLUME_DOWN_VALUE;
    }

    u8 engineType = Parameters::getEngineType(_61);
    startSoundFromID(engineType + 8);

    JAISoundHandle* handle = &(*this)[3];
    if(!handle->isSoundAttached())
    {
        return;
    }

    if(_5d != 0)
    {
        f32 chibiPitch = Parameters::getChibiPitch((*handle)->getID());
        pitch *= chibiPitch;
        volume *= 0.65f;
    }
    (*handle)->getAuxiliary().moveVolume(volume, 0);
    (*handle)->getAuxiliary().movePitch(pitch, 0);
}

void KartSoundMgr::setConductTrouble(f32 f1, u8 r4) {
    JAISound* sound;
    f32 volume;
    u32 soundID;

    if(_63 != r4)
    {
        _92 = (u16)(38.f * f1);
    }
    if(_8c == 1)
    {
        if(_92 < 0x26)
        {
            _92++;
            _92++;
        }
        volume = 1.2f;
    }
    else {
        if(_92 != 0)
        {
            _92--;
        }
        volume = 0.55f;
    }

    f32 pitch = (EnginePitchKeisuuTrouble[(u8)Parameters::getEngineType(_61)] * _92) + 0.6f;

    if(_66 != 0){
        volume *= GA_ENEMY_VOLUME_DOWN_VALUE;
    }

    u8 engineType = Parameters::getEngineType(_61);
    startSoundFromID(engineType + 8);

    JAISoundHandle* handle = &(*this)[3];
    if(!handle->isSoundAttached())
    {
        return;
    }

    if(_5d != 0)
    {
        f32 chibiPitch = Parameters::getChibiPitch((*handle)->getID());
        pitch *= chibiPitch;
        volume *= 0.65f;
    }
    (*handle)->getAuxiliary().moveVolume(volume, 0);
    (*handle)->getAuxiliary().movePitch(pitch, 0);
}

void KartSoundMgr::setConductRace(bool r4) {
    f32 f0;
    f32 f1;
    f32 f2;
    u16 r27 = 0;                 // u16: writePort gets a plain `mr r5, r27`
    u32 soundOffset;             // not u8: target truncates engineType, not this
    u32 engineType = Parameters::getEngineType((u8)_61);

    if (_e8[0] == 0x11 || _e8[1] == 0x11 || _e8[2] == 0x11 || _e8[3] == 0x11) {
        f2 = 215.f;
        for (u8 index = 0; index < 4U; index++) {
            f0 = mWaterDepths[index];
            if (f2 > f0) {
                f2 = f0;
            }
        }
        if (f2 < 15.f) {
            f2 = 0.f;
        } else {
            if (f2 > 215.f) {
                f2 = 215.f;
            }
            f2 = (f2 - 15.f) / 200.f;
        }
        if (f2 == 0.f) {
            r27 = 0;
        } else {
            r27 = -127.f -((126.f * f2));
        }
    }

    if ((1.f + _84) > _98) {
        if (_84 < 1.f) {
            _100 += 1;
            if (_100 == 0x14) {
                _8c = 0;
                _100 = 0;
            }
        }
        if (_84 > 50.f) {
            if (_96 < 0x320) {
                _96++;
            }
        }
    } else {
        if (_84 < 10.f) {
            _8c = 0;
        }
        if (_96 >= 0x28) {
            _96 -= 0x28;
        }
    }

    if (_8c == 1) {
        f1 = _fc;
        if (f1 > 0.f) {
            _fc = f1 - 1.f;
        }

        _90 = 0;
        if (_8e < 0x270fu) {
            _8e++;
        }
        f32 volume;
        f32 pitch;
        f32 temp_pitch = ((-0.0016666667f * _84) + 0.3f);

        if (_a0 != 0.f) {
            s16 r0 = (80.f * _a0);
            if (_96 > -0xC8) {
                _96 -= r0;
            }
        }
        adjustEngine();
        if (_f0 > 0.f) {
            _fc = 0.f;
        }
        u8 index = engineType;
        pitch = 0.02f * _fc
            + (_f0
            + (temp_pitch
            + (0.5f
            + (_96 * (EngineAddKeisuuRaceUp[index])
            + ((_88 * EngineKeisuuRaceUp[index]) + (_84 * EngineKeisuuRaceUp[index]))))));
        volume = 1.4f;

        if (_84 < 40.f) {
            volume = 0.3f + (-0.0075000003f * _84 + 1.4f);
        }

        if ((u8)engineType < 4 && !r4) {
            soundOffset = 0;
        } else {
            soundOffset = 8;
        }

        if (_66 != 0) {
            volume *= GA_ENEMY_VOLUME_DOWN_VALUE;
        }
        u16 temp = r27;

        if (temp != 0) {
            volume *= 0.5f + ((0.5f * temp) / 127.f);
        }
        startSoundFromID(soundOffset + (u8)engineType);

        JAISoundHandle* handle = &(*this)[3];
        if (handle->isSoundAttached()) {
            if (_5d != 0) {
                f32 chibiPitch = Parameters::getChibiPitch((*handle)->getID());
                pitch *= chibiPitch;
                volume *= 0.65f;
            }
            (*handle)->getAuxiliary().moveVolume(volume, 0);
            (*handle)->getAuxiliary().movePitch(pitch, 0);
            (*handle)->getTrack()->writePort(0xA, r27);
        }

        if ((u8)engineType < 4) {
            f1 = (f32)(u32)(_84 * (UpEngineLoopStart[(u8)engineType] / 150.f) - 10000.f);
            if (f1 < 0.f) {
                f1 = 0.f;
            }
            if (f1 > UpEngineLoopEnd[(u8)engineType]) {
                f1 = UpEngineLoopEnd[(u8)engineType];
            }

            // conversion happens before operator[] in the target
            u32 skip = (u32)f1;
            JAISoundHandle* handle = &(*this)[3];
            if (handle->isSoundAttached()) {
                (*handle)->getTrack()->setSkipSample(skip);
            }
        }
    } else {
        f32 volume;
        f32 pitch;
        _96 = 0;
        _8e = 0;
        if (_90 < 0x270fu) {
            _90++;
        }
        adjustEngine();

        u8 index = engineType;
        pitch = _f0 + (0.7f + (_96 * (EngineAddKeisuuRaceDown[index]) + ((_88 * EngineKeisuuRaceDown[index]) + (_84 * EngineKeisuuRaceDown[index]))));

        if ((u8)engineType < 4) {
            volume = 1.1f;
        } else {
            volume = 0.8f;
        }
        if (_84 < 40.f) {
            volume = 0.4f + ((-0.01f * _84) + volume);
        }

        if ((u8)engineType < 4 && !r4) {
            soundOffset = 4;
        } else {
            soundOffset = 8;
        }

        if (_66 != 0) {
            volume *= GA_ENEMY_VOLUME_DOWN_VALUE;
        }
        startSoundFromID(soundOffset + (u8)engineType);

        JAISoundHandle* handle = &(*this)[3];
        if (handle->isSoundAttached()) {
            if (_5d != 0) {
                f32 chibiPitch = Parameters::getChibiPitch((*handle)->getID());
                pitch *= chibiPitch;
                volume *= 0.65f;
            }
            (*handle)->getAuxiliary().moveVolume(volume, 0);
            (*handle)->getAuxiliary().movePitch(pitch, 0);
            (*handle)->getTrack()->writePort(0xA, r27);
        }

        if ((u8)engineType < 4) {
            const f32 loopStart = DownEngineLoopStart[(u8)engineType];

            f1 = (f32)(u32)(_84 * -(loopStart / 150.f) + loopStart - 5000.f);
            if (f1 < 0.f) {
                f1 = 0.f;
            }
            if (f1 > DownEngineLoopEnd[(u8)engineType]) {
                f1 = DownEngineLoopEnd[(u8)engineType];
            }

            u32 skip = (u32)f1;
            JAISoundHandle* handle = &(*this)[3];
            if (handle->isSoundAttached()) {
                (*handle)->getTrack()->setSkipSample(skip);
            }
        }
    }
}

void KartSoundMgr::setConductAfterGoal(bool enable) {
    setConductRace(enable);
}

void KartSoundMgr::setCrushSe(CrsGround::EMat mat, f32 f1) {
    switch (mat) {
    case CrsGround::Mat_5:
    case CrsGround::Mat_17:
        setCrushSe(0x1001DU, f1);
        return;
    case CrsGround::Mat_8:
        setCrushSe(0x10015U, f1);
        return;
    case CrsGround::Mat_7:
    case CrsGround::Mat_15:
        setCrushSe(0x10021U, f1);
        return;
    case CrsGround::Mat_16:
        setCrushSe(0x10023U, f1);
        return;
    case CrsGround::Mat_18:
        setCrushSe(0x10053U, f1);
        return;
    case CrsGround::Mat_19:
        setCrushSe(0x10056U, f1);
        return;
    case CrsGround::Mat_9:
        setCrushSe(0x10019U, f1);
        return;
    case CrsGround::Mat_25:
        setCrushSe(0x10067U, f1);
        return;
    case CrsGround::Mat_27:
        setCrushSe(0x1007DU, f1);
        return;
    case CrsGround::Mat_255:
        break;
    default:
        setCrushSe(0x10015U, f1);
        return;
    }
}

void KartSoundMgr::setCrushSe(u32 r4, f32 f1) {
    r4 = Common::changeRandomId(r4, 0);
    const u32 randomId2 = Random::getRandomU32();

    if(mKillSw || _66 == 2) {
        return;
    }

    if(_66 != 0) {
        return;
    }

    Main* main = Main::getAudio();

    if(main->get_80() - _74 < 0x3c) {
        return;
    }

    _74 = main->get_80();

    if(_9c != 0) {
        return;
    }

    startSoundHandleNumber(0, r4, 0);

    JAISoundHandle& handle = (*this)[0];
    f32 volume;
    const f32 f30 = f1 + 0.6f;
    if(handle.isSoundAttached()) {
        volume = 0.8f * f30;
        handle->getAuxiliary().moveVolume(volume, 0);
        _a0 = f30;
    }

    u8 r3 = (randomId2 & 0x7);

    _9c = r3 + 4;
}

void KartSoundMgr::setBrakeSe(u32 soundID) {
    f32 volume;
    f32 pitch;

    if(mKillSw || _66 == 2) {
        return;
    }

    if(_66 != 0)
    {
        return;
    }

    startSoundHandleNumber(6, soundID, 0);
    JAISoundHandle& handle = (*this)[6];
    if(handle.isSoundAttached())
    {
        volume = (0.003f * _84) + 0.6f;
        pitch = (0.008f * _84) + 0.25f;
        if(_5d != 0)
        {
            pitch = pitch * Parameters::getChibiPitch(soundID);
        }
        handle->getAuxiliary().moveVolume(0.8f * volume, 0);
        handle->getAuxiliary().movePitch(pitch, 0);
    }
    setChibiPitch(&handle);
}

void KartSoundMgr::setDashSe(u32 soundID) {
    if(mKillSw || _66 == 2) {
        return;
    }

    if(_66 == 1)
    {
        if((u8)Parameters::getRaceMode() != 2)
        {
            return;
        }   
    }

    this->setSe(soundID);

    u8 engineType = Parameters::getEngineType(_61);

    f32 adjustInitialValue = DashEngineAdjustInitialValue[engineType];

    _f0 = adjustInitialValue;
    _f4 = 0x10014;
    _f8 = 0x64;
}

void KartSoundMgr::setMiniturboSe(u32 soundID) {
    if(mKillSw || _66 == 2) {
        return;
    }

    if(_66 != 0)
    {
        return;
    }

    this->setSe(soundID);

    u8 engineType = Parameters::getEngineType(_61);

    f32 adjustInitialValue = DashEngineAdjustInitialValue[engineType];

    _f0 = adjustInitialValue;
    _f4 = 0x10044;
    _f8 = 0x28;
}

void KartSoundMgr::setJumpUpSe(u32 soundID) {
    if(mKillSw || _66 == 2) {
        return;
    }

    if(Parameters::getRaceCourse() != 0x22)
    {
        return;
    }

    this->setSe(soundID);

}

void KartSoundMgr::setBoundSe(f32 f1) {
    if(mKillSw || _66 == 2) {
        return;
    }

    if(_66 == 1 && (u8)Parameters::getRaceMode() != 2)
    {
        return;
    }

    if(_105 >= 0x19) 
    {
        return;
    }

    startSoundHandleNumber(4, BoundSe[_105], 0);

    
    JAISoundHandle& handle = (*this)[4];
    
    if(handle.isSoundAttached()) {
        handle->getAuxiliary().movePitch((0.5f * f1) + 0.5f, 0);
        if(handle.isSoundAttached()) {
            handle->getAuxiliary().moveVolume(f1 * mCameraVolume, 0);
        }
    }
    setChibiPitch(&handle);
}

void KartSoundMgr::setWheelSpinSe() {
    if(mKillSw || _66 == 2) {
        return;
    }

    if(_66 != 0)
    {
        return;
    }

    const u8 characterType = Parameters::getCharacterType(_61);

    if(characterType == 1)
    {
        startSoundHandleNumber(4, 0x10077, 0);
    }
    else {
        if(_105 < 0x19) 
        {
            startSoundHandleNumber(4, WheelSpinSe[_105], 0);
        }
    }

    JAISoundHandle& handle = (*this)[4];
    setChibiPitch(&handle);
}

void KartSoundMgr::setSpinSe() {
    if(mKillSw || _66 == 2) {
        return;
    }

    const u8 characterType = Parameters::getCharacterType(_61);

    if(characterType == 1)
    {
        startSoundHandleNumber(1, 0x100ae, 0);
    }
    else {
        if(_105 < 0x19) 
        {
            startSoundHandleNumber(1, SpinSe[_105], 0);
        }
    }

    JAISoundHandle& handle = (*this)[1];
    setChibiPitch(&handle);
}

void KartSoundMgr::setSpinTurnSe() {
    if(mKillSw || _66 == 2) {
        return;
    }

    if(_66 != 0)
    {
        return;
    }

    const u8 characterType = Parameters::getCharacterType(_61);

    if(characterType == 1)
    {
        startSoundHandleNumber(1, 0x100c6, 0);
    }
    else {
        if(_105 < 0x19)
        {
            startSoundHandleNumber(1, SpinTurnSe[_105], 0);
        }
    }

    JAISoundHandle& handle = (*this)[1];
    setChibiPitch(&handle);
}

void KartSoundMgr::setSe(u32) {}

void KartSoundMgr::setChibiPitch(JAISoundHandle *) {}

void KartSoundMgr::adjustEngine() {}

void KartSoundMgr::crushRenzokuTaisaku() {}

void KartSoundMgr::slipParamSet() {}

void KartSoundMgr::checkEcho() {}

void KartSoundMgr::setInvincibleBgm(u8) {}

void KartSoundMgr::clearInvincibleBgm(u8) {}

void KartSoundMgr::setChibiFlag(bool, bool) {}

}
