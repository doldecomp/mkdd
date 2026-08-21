#include "Inagaki/GameSoundMgr.h"

#include "Inagaki/GameAudioAudience.h"
#include "Inagaki/GameAudioCamera.h"
#include "Inagaki/GameAudioCommon.h"
#include "Inagaki/GameAudioMain.h"
#include "Inagaki/GameSoundTable.h"

#include "JSystem/JAudio/Interface/JAIAudible.h"
#include "JSystem/JAudio/Interface/JAISound.h"
#include "JSystem/JAudio/JAUSoundObject.h"
#include "JSystem/JAudio/System/JASGadget.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"

#include "Kaneshige/Course/CrsArea.h"
#include "Kaneshige/Course/CrsGround.h"
#include "kartEnums.h"

namespace GameAudio
{

const s16 SignalUpPitchWidth[3] = {0x0064, 0x0082, 0x0078};
const s16 SignalUpPitchBase[3] = {0x0032, 0x0046, 0x0050};
const s16 SignalUpIntervalWidth[3] = {0x0046, 0x001E, 0x000A};
const s16 SignalUpIntervalBase[3] = {0x0014, 0x000A, 0x0005};
const s16 SignalDownIntervalWidth[3] = {0x0050, 0x0032, 0x0014};
const s16 SignalDownIntervalBase[3] = {0x0014, 0x000A, 0x0005};

static const u8 cKartRankClassTable0[7] = {0, 0, 1, 1, 1, 2, 2};
static const u8 cKartRankClassTable1[7] = {0, 0, 1, 1, 2, 2, 2};

u32 BoundSe[25];
u32 WheelSpinSe[25];
u32 SpinSe[25];
u32 SpinTurnSe[25];

namespace
{
struct InitBoundSe
{
    InitBoundSe()
    {
        BoundSe[0] = 0x10046;
        BoundSe[1] = 0x10045;
        BoundSe[2] = 0x10049;
        BoundSe[3] = 0x10048;
        BoundSe[4] = 0x10045;
        BoundSe[5] = 0x10045;
        BoundSe[6] = 0x10045;
        BoundSe[7] = 0x10045;
        BoundSe[8] = 0x10045;
        BoundSe[9] = 0x10045;
        BoundSe[10] = 0x10045;
        BoundSe[11] = 0x1004a;
        BoundSe[12] = 0x10047;
        BoundSe[13] = 0x10045;
        BoundSe[14] = 0x10045;
        BoundSe[15] = 0x1004a;
        BoundSe[16] = 0x10045;
        BoundSe[17] = 0x10045;
        BoundSe[18] = 0x10045;
        BoundSe[19] = 0x10045;
        BoundSe[20] = 0x10045;
        BoundSe[21] = 0x10045;
        BoundSe[22] = 0x10045;
        BoundSe[23] = 0x10045;
    }
};
struct InitWheelSpinSe
{
    InitWheelSpinSe()
    {
        WheelSpinSe[0] = 0x1006f;
        WheelSpinSe[1] = 0x1006e;
        WheelSpinSe[2] = 0x1006e;
        WheelSpinSe[3] = 0x10072;
        WheelSpinSe[4] = 0x10071;
        WheelSpinSe[5] = 0x1006e;
        WheelSpinSe[6] = 0x1006e;
        WheelSpinSe[7] = 0x1006e;
        WheelSpinSe[8] = 0x1006e;
        WheelSpinSe[9] = 0x1006e;
        WheelSpinSe[10] = 0x1006e;
        WheelSpinSe[11] = 0x10074;
        WheelSpinSe[12] = 0x1006e;
        WheelSpinSe[13] = 0x1006e;
        WheelSpinSe[14] = 0x1006e;
        WheelSpinSe[15] = 0x10074;
        WheelSpinSe[16] = 0x1006e;
        WheelSpinSe[17] = 0x10075;
        WheelSpinSe[18] = 0x1006e;
        WheelSpinSe[19] = 0x1006e;
        WheelSpinSe[20] = 0x10070;
        WheelSpinSe[21] = 0x10070;
        WheelSpinSe[22] = 0x10073;
        WheelSpinSe[23] = 0x10076;
        WheelSpinSe[24] = 0x1006e;
    }
};
struct InitSpinSe
{
    InitSpinSe()
    {
        SpinSe[0] = 0x1009e;
        SpinSe[1] = 0x1009d;
        SpinSe[2] = 0x100a6;
        SpinSe[3] = 0x100a1;
        SpinSe[4] = 0x100a0;
        SpinSe[5] = 0x100b0;
        SpinSe[6] = 0x100af;
        SpinSe[7] = 0x100af;
        SpinSe[8] = 0x100af;
        SpinSe[9] = 0x100af;
        SpinSe[10] = 0x100a7;
        SpinSe[11] = 0x100a9;
        SpinSe[12] = 0x100ad;
        SpinSe[13] = 0x100ab;
        SpinSe[14] = 0x100aa;
        SpinSe[15] = 0x100a3;
        SpinSe[16] = 0x100ac;
        SpinSe[17] = 0x100a4;
        SpinSe[18] = 0x1009e;
        SpinSe[19] = 0x100a8;
        SpinSe[20] = 0x1009f;
        SpinSe[21] = 0x1009f;
        SpinSe[22] = 0x100a2;
        SpinSe[23] = 0x100a5;
        SpinSe[24] = 0x1009d;
    }
};
struct InitSpinTurnSe
{
    InitSpinTurnSe()
    {
        SpinTurnSe[0] = 0x100b6;
        SpinTurnSe[1] = 0x100b5;
        SpinTurnSe[2] = 0x100be;
        SpinTurnSe[3] = 0x100b9;
        SpinTurnSe[4] = 0x100b8;
        SpinTurnSe[5] = 0x100c8;
        SpinTurnSe[6] = 0x100c7;
        SpinTurnSe[7] = 0x100c7;
        SpinTurnSe[8] = 0x100c7;
        SpinTurnSe[9] = 0x100c7;
        SpinTurnSe[10] = 0x100bf;
        SpinTurnSe[11] = 0x100c1;
        SpinTurnSe[12] = 0x100c5;
        SpinTurnSe[13] = 0x100c3;
        SpinTurnSe[14] = 0x100c2;
        SpinTurnSe[15] = 0x100bb;
        SpinTurnSe[16] = 0x100c4;
        SpinTurnSe[17] = 0x100bc;
        SpinTurnSe[18] = 0x100b6;
        SpinTurnSe[19] = 0x100c0;
        SpinTurnSe[20] = 0x100b7;
        SpinTurnSe[21] = 0x100b7;
        SpinTurnSe[22] = 0x100ba;
        SpinTurnSe[23] = 0x100bd;
        SpinTurnSe[24] = 0x100b5;
    }
};

InitBoundSe sInitBoundSe;
InitWheelSpinSe sInitWheelSpinSe;
InitSpinSe sInitSpinSe;
InitSpinTurnSe sInitSpinTurnSe;
}  // namespace

f32 GA_ENEMY_VOLUME_DOWN_VALUE = 0.85f;
}  // namespace GameAudio

#include "JSystem/JAudio/JASFakeMatch14.h"

namespace GameAudio
{

const f32 EngineKeisuuRaceUp[] = {
    0.006f,
    0.006f,
    0.005f,
    0.003f,
    0.015f,
    0.015f,
    0.013f,
    0.011f,
    0.02f,
};

const f32 EngineKeisuuRaceDown[] = {
    0.005f,
    0.003f,
    0.005f,
    0.002f,
    0.012f,
    0.012f,
    0.012f,
    0.009f,
    0.019f,
};

const f32 DashEngineAdjustInitialValue[] = {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f};

const f32 DashEngineIncPerFrame[] =
    {0.025f, 0.025f, 0.025f, 0.095f, 0.025f, 0.025f, 0.025f, 0.025f, 0.025f};

const f32 DashEngineDecPerFrame[] =
    {0.005f, 0.005f, 0.005f, 0.005f, 0.005f, 0.005f, 0.005f, 0.005f, 0.005f};

u8 KartSoundMgr::smKartCount;
u8 KartSoundMgr::smEntryKartCount;
u8 KartSoundMgr::smGoalKartCount;

u8 KartSoundMgr::smKartRankClassMem[7] = {};

KartSoundMgr::KartSoundMgr(Vec *pos, JKRHeap *heap, u8 kartNo, u8 kartType)
    : SoundMgr(pos, heap, 12)
{
    mSignalPitch = 1.f;
    mSignalTargetPitch = 1.f;
    mSignalPitchCounter = 0;
    mCameraVolume = 1.f;
    mGoalVolume = 1.f;
    mGoalVolumeCounter = 0;
    _C = 0xff;

    mKartIndex = kartNo;

    if (kartType == 0)
    {
        mPlayerIndex = smKartCount++;
    }
    else
    {
        mPlayerIndex = 4;
    }

    if (kartType != 2)
    {
        smEntryKartCount++;
    }

    mKartType = kartType;
    mCourseArea = new CrsArea();
    u8 index = 0;

    if (mKartType == 0)
    {
        mRankClass = 3;
    }
    else
    {
        u32 randomValue = GameAudio::Random::getSignalEngineRandomU32();
        u8 rankClassIndex = randomValue % 7;

        while (smKartRankClassMem[rankClassIndex] == 1 && index < 7)
        {
            index++;

            if (++rankClassIndex == 7)
            {
                rankClassIndex = 0;
            }
        }

        smKartRankClassMem[rankClassIndex] = 1;
        u8 playerMode = Parameters::getPlayerMode();
        if (playerMode != 1)
        {
            mRankClass = cKartRankClassTable0[rankClassIndex];
        }
        else
        {
            mRankClass = cKartRankClassTable1[rankClassIndex];
        }
    }

    init();

    mSceneMask = 0;

    const u32 sceneMax = Main::getAudio()->getCamera()->getSceneMax();

    if (sceneMax > 1 && sceneMax > mPlayerIndex)
    {
        mSceneMask = (1 << mPlayerIndex) ^ 0xf;
    }
}

KartSoundMgr::~KartSoundMgr()
{
    setChibiFlag(false, false);
    for (u8 index = 0; index < 7; index++)
    {
        smKartRankClassMem[index] = 0;
    }

    if (mKartType == 0)
    {
        const s32 kartCount = mPlayerIndex;
        if (kartCount < 4)
        {
            CustomAudience<4>::smCameraVolume[kartCount] = 1.f;
        }
    }

    smKartCount = 0;
    smEntryKartCount = 0;
    smGoalKartCount = 0;
}

void KartSoundMgr::startSoundHandleNumber(u8 handleIndex, u32 soundID, u32 fadeCount)
{
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    JAISoundStarter *soundStarter = JASGlobalInstance<JAISoundStarter>::getInstance();

    JAISoundHandle &handle = (*this)[handleIndex];

    soundStarter->startSound(soundID, &handle, NULL);

    JAISound *sound;

    if (!handle.isSoundAttached())
    {
        return;
    }

    if (handle->audible_ == NULL)
    {
        Main *main = Main::getAudio();
        u32 scene = 0;
        CameraMgr *camera = main->getCamera();
        u32 sceneMax = camera->getSceneMax();

        if (sceneMax > 1 && sceneMax > mPlayerIndex)
        {
            scene = (1 << mPlayerIndex) ^ 0xf;
        }

        sound = handle.operator->();

        JGeometry::TVec3f vec(*mSoundPos);
        sound->newAudible(vec, &_18, scene, NULL);
    }
    sound = handle.operator->();

    sound->fader_.fadeInFromOut2(fadeCount);

    setEcho(&handle, mEchoMix);
}

void KartSoundMgr::dispose()
{
    JAUSoundObject::dispose();

    clearInvincibleBgm(3);
}

void KartSoundMgr::startSoundEngine(u8, u32) {}  // UNUSED

void KartSoundMgr::init()
{
    const u8 cWheelCount = 4;
    mInvincibleBgmBits = 0;
    mChibiFlag = 0;
    mPrevRpm = 0.f;
    mPrevConductStatus = 0;
    mEchoFixed = 0;
    mEchoMix = 0.f;
    mPrevEchoMix = 0.f;
    mEngineFadeCount = 0;
    mSpeed = 0.f;
    mRpm = 0.f;
    mAccelFlag = 0;
    mConductStatus = 0;
    mAccelTimer = 0;
    mDecelTimer = 0;
    mEngineLevel = 0;
    mOutOfCourseLevel = 0;
    mEngineRevCount = 0;
    mPrevSpeed = 0.f;
    mCrushWait = 0;
    mCrushDegree = 0.f;

    for (u8 index = 0; index < cWheelCount; index++)
    {
        mSlipPans[index] = 0.f;
        mSlipVolumes[index] = 0.f;
        mSlipPitches[index] = 0.f;
        mSlipFlags[index] = 0;
        mWaterDepths[index] = 0.f;
        mGroundAttrs[index] = 0;
    }

    _ec = 0.f;
    mEngineAdjust = 0.f;
    mAdjustSoundID = 0;
    mAdjustCounter = 0;
    mLowSpeedCount = 0;
    mSlipSeCount = 0;
    mSlipSeInterval = 0;
    mSlipSeAlternator = 0;
    mSlipSeIndex = 0;
    mGroundSeIndex = 1;
    mKarabukashiCount = 0.f;
    mSignalRevUp = 0;
    mSignalPitch = 0.f;
    mSignalTargetPitch = 0.f;
    mSignalPitchCounter = 0;
    mCameraVolume = 1.f;
    mGoalVolume = 1.f;
    mGoalVolumeCounter = 0.f;
    mAfterGoalCounter = 0;
    mGoalFlag = 0;
    mLastCrushFrame = 0;
    mRank = 0xff;

    if (mKartType == 0)
    {
        const s32 kartCount = mPlayerIndex;
        const f32 volume = mCameraVolume;
        if (kartCount < 4)
        {
            CustomAudience<4>::smCameraVolume[kartCount] = volume;
        }
    }

    Main *main = Main::getAudio();
    CustomSoundTable *soundTable = main->getSoundTable();
    for (s32 index = 0; index < _10; index++)
    {
        if (!(*this)[index].isSoundAttached())
        {
            continue;
        }
        JAISound *sound = (*this)[index].operator->();
        const u32 swBit = soundTable->getSwBit(sound->getID().mId.mFullId);
        if (!(swBit & 0x00800000))
        {
            continue;
        }
        (*this)[index]->stop();
    }

    setChibiFlag(false, false);
    clearInvincibleBgm(3);
    smGoalKartCount = 0;
}

void KartSoundMgr::changeAttribute(u8) {}  // UNUSED

void KartSoundMgr::changeDriver(bool) {}

void KartSoundMgr::checkCourseSound(u8) {}  // UNUSED

void KartSoundMgr::frameWork(u8 rank)
{
    const u8 cHandleCourse = 7;
    if (mKartType == 0)
    {
        mCourseArea->search(6, *mSoundPos);
        Course::Area *area = mCourseArea->getArea();
        if (area != NULL)
        {
            ECourseID courseID = Parameters::getRaceCourse();
            switch (courseID)
            {
            case 0x2a:
                if (rank <= 3)
                {
                    JAISoundHandle &handle = (*this)[cHandleCourse];
                    if (!handle.isSoundAttached())
                    {
                        startSoundHandleNumber(cHandleCourse, 0x40024, 0);
                    }
                }
                break;

            case 0x29:
                mEchoFixed = 1;
                mEchoMix = 0.5f;
                break;
            }
        }
        else
        {
            ECourseID courseID = Parameters::getRaceCourse();
            switch (courseID)
            {
            case 0x29:
                mEchoFixed = 0;
                break;
            }
        }
    }
    checkEcho();

    if (mKillSw || mKartType == 2)
    {
        return;
    }

    crushRenzokuTaisaku();
    slipParamSet();
    if (mPrevConductStatus != 3)
    {
        mRank = rank;
    }
    checkAfterGoalVolume();
}

void KartSoundMgr::checkAfterGoalVolume()
{
    const u8 cHandleEngine = 3;
    if (mKartType != 0)
    {
        return;
    }

    if (mGoalFlag != 1)
    {
        return;
    }

    if (mAfterGoalCounter == 0)
    {
        JAISoundHandle &handle = (*this)[cHandleEngine];
        if (handle.isSoundAttached())
        {
            handle->stop();
        }

        Main *main = Main::getAudio();
        CameraMgr *camera = main->getCamera();

        if (camera->getSceneMax() == 2)
        {
            const u8 mode = Parameters::getRaceMode();
            if (mode == 1)
            {
                mSceneMask = 0xc;
                mEngineFadeCount = 0x78;
            }
        }
    }
    else if (mAfterGoalCounter == 0x3c)
    {
        const f32 goalVolumeOn = 0.35f;
        // Forced to use those fabricated "equal" methods for the registers to be loaded
        // in the right order.
        if (mGoalVolumeCounter != 0 || !isCameraVolumeEqual(goalVolumeOn))
        {
            if (mGoalVolumeCounter == 0 || !isGoalVolumeEqual(goalVolumeOn))
            {
                mGoalVolume = goalVolumeOn;
                mDeltaVolume = (mCameraVolume - mGoalVolume) / 61.f;
                mGoalVolumeCounter = 0x3d;
            }
        }
    }
    else if (mAfterGoalCounter == 0x168)
    {
        const f32 goalVolumeOff = 0.0f;
        if (mGoalVolumeCounter != 0 || !isCameraVolumeEqual(goalVolumeOff))
        {
            if (mGoalVolumeCounter == 0 || !isGoalVolumeEqual(goalVolumeOff))
            {
                mGoalVolume = goalVolumeOff;
                mDeltaVolume = (mCameraVolume - mGoalVolume) / 301.f;
                mGoalVolumeCounter = 0x12d;
            }
        }
    }
    mAfterGoalCounter++;

    u8 camera;
    if (mGoalVolumeCounter == 0)
    {
        camera = 0;
    }
    else
    {
        if (--mGoalVolumeCounter)
        {
            camera = 1;
            mCameraVolume -= mDeltaVolume;
        }
        else
        {
            camera = 1;
            mCameraVolume = mGoalVolume;
        }
    }

    if (camera == 0)
    {
        return;
    }

    const s32 playerNo = mPlayerIndex;
    const f32 cameraVolume = mCameraVolume;

    if (playerNo >= 4)
    {
        return;
    }

    CustomAudience<4>::smCameraVolume[playerNo] = cameraVolume;
}

void KartSoundMgr::setHandleVolume(JAISoundHandle &, f32) {}  // UNUSED

void KartSoundMgr::setWaterDepth(f32) {}  // UNUSED

void KartSoundMgr::setWaterDepth(u8 wheel, f32 depth)
{
    mWaterDepths[wheel] = depth;
}

void KartSoundMgr::setSlip(u8 wheel, u8 attribute, u8 material, f32 slipDegree)
{
    const u8 cNoGroundSe = 0xff;
#line 907
    JUT_ASSERT_MSG(wheel < 4, "KartSoundMgr::setSlip wheel ERROR!!!\n");

    mGroundAttrs[wheel] = attribute;
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        return;
    }

    f32 volume = 0.f;
    f32 pitch = 1.f;
    f32 slipBias;

    u8 seIndex;
    switch (attribute)
    {
    case 0x11:
    {
        if (mWaterDepths[wheel] <= 0.f)
        {
            switch (material)
            {
            case 0xa:
                slipBias = 0.1f;
                seIndex = 0x14;
                break;
            case 0x1c:
                slipBias = 0.1f;
                seIndex = 0x15;
                break;
            default:
                slipBias = 0.f;
                seIndex = cNoGroundSe;
            }
        }
        else
        {
            slipBias = 0.f;
            seIndex = 0xa;
        }
    }
    break;
    default:
    {
        switch (material)
        {
        case 1:
            if (attribute == 1)
            {
                slipBias = 0.1f;
            }
            else
            {
                slipBias = -0.1f;
            }
            seIndex = 0;
            break;
        case 0:
            slipBias = 0.1f;
            const u8 type = Parameters::getCharacterType(mKartIndex);
            if (type == 1)
            {
                seIndex = 9;
            }
            else
            {
                seIndex = 1;
            }
            break;
        case 7:
            if (attribute == 1)
            {
                slipBias = -0.06f;
            }
            else
            {
                slipBias = -0.1f;
            }
            seIndex = 2;
            break;
        case 6:
            if (attribute == 1)
            {
                slipBias = 0.f;
            }
            else
            {
                slipBias = -0.1f;
            }
            seIndex = 3;
            break;
        case 2:
            slipBias = 0.1f;
            seIndex = 4;
            break;
        case 8:
            slipBias = 0.1f;
            seIndex = 0x13;
            break;
        case 3:
            slipBias = -0.1f;
            seIndex = 5;
            break;
        case 5:
            slipBias = -0.1f;
            seIndex = 6;
            break;
        case 14:
            slipBias = 0.1f;
            seIndex = 0xd;
            break;
        case 20:
            slipBias = 0.1f;
            seIndex = 0xf;
            break;
        case 22:
            slipBias = -0.06f;
            seIndex = 0xc;
            break;
        case 13:
            slipBias = -0.06f;
            seIndex = 0xe;
            break;
        case 10:
            slipBias = 0.1f;
            seIndex = 0x14;
            break;
        case 28:
            slipBias = 0.1f;
            seIndex = 0x15;
            break;
        case 12:
            slipBias = 0.1f;
            seIndex = 0xb;
            break;
        case 21:
            slipBias = 0.1f;
            seIndex = 0x10;
            break;
        case 11:
            slipBias = 0.1f;
            seIndex = 0x11;
            break;
        case 23:
            slipBias = 0.1f;
            seIndex = 0x16;
            break;
        case 24:
            slipBias = 0.1f;
            seIndex = 0x17;
            break;
        default:
            slipBias = 0.f;
            seIndex = cNoGroundSe;
            break;
        }
    }
    }

    mGroundSeIndex = seIndex;

    if (seIndex != cNoGroundSe && mConductStatus != 0)
    {
        if (mSpeed < 5.f)
        {
            return;
        }
        f32 waterDepth;
        f32 speedRatio;
        f32 shallowRatio;
        u32 frontSlipSe;
        const f32 pitchCoefficient = 0.0026666666f;
        switch (attribute)
        {
        case 0:
        case 1:
        case 0x0c:
            switch (material)
            {
            default:
                pitch = (pitchCoefficient * mSpeed) + 0.7f;
            case 11:
            case 12:
            case 20:
            case 28:
                volume = 0.2f + (slipDegree - slipBias);
                break;
            case 3:
                pitch = (pitchCoefficient * mSpeed) + 0.7f;
            case 2:
            case 5:
            case 10:
            case 13:
            case 14:
            case 22:
                speedRatio = mSpeed / 70.f;
                if (speedRatio > 1.f)
                {
                    speedRatio = 1.f;
                }
                volume = speedRatio * (slipDegree - slipBias + 0.4f);
                break;
            }
            break;

            break;
        case 0x11:
            switch (seIndex)
            {
            case 0x14:
            case 0x15:
                pitch = (mSpeed * pitchCoefficient) + 0.7f;
                volume = (slipDegree - slipBias) + 0.2f;
                break;
            case 0xa:
                waterDepth = mWaterDepths[wheel];
                if (waterDepth > 40.f)
                {
                    waterDepth = 40.f;
                }

                shallowRatio = (40.f - waterDepth) / 40.f;
                if (mSpeed < 10.f)
                {
                    volume = 0.f;
                }
                else
                {
                    speedRatio = mSpeed / 70.f;
                    if (speedRatio > 1.f)
                    {
                        speedRatio = 1.f;
                    }
                    volume = (1.2f * speedRatio) + 0.2f;
                    pitch = (shallowRatio * (0.53f * speedRatio)) + 0.79f;
                }
                break;
            }
            break;

        default:
            speedRatio = mSpeed / 70.f;
            if (speedRatio > 1.f)
            {
                speedRatio = 1.f;
            }
            volume = speedRatio * (slipDegree - slipBias + 0.4f);
            break;
        }
        if (volume <= 0.1f)
        {
            return;
        }
        if (volume > 1.5f)
        {
            volume = 1.5f;
        }
        mSlipFlags[wheel] = 1;

        mSlipVolumes[wheel] = volume;

        if (wheel & 1)
        {
            mSlipPans[wheel] = 0.47f;
        }
        else
        {
            mSlipPans[wheel] = 0.53f;
        }
        mSlipPitches[wheel] = pitch;

        switch (wheel)
        {
        case 0:
            mSlipSeIndex = seIndex;
            break;
        case 1:
        case 2:
        case 3:
            frontSlipSe = mSlipSeIndex;
            if (frontSlipSe != 6 && frontSlipSe != 5 && frontSlipSe != 10)
            {
                mSlipSeIndex = seIndex;
            }
            break;
        }
    }
}

void KartSoundMgr::setConductStatus(f32 rpm,
                                    f32 speed,
                                    bool accelBtn,
                                    bool brakeBtn,
                                    bool isBrake,
                                    u8 conductStatus,
                                    CrsArea *area)
{
    const u8 cHandleCourse = 7;
    if (area != NULL && mEchoFixed == 0)
    {
        f32 rate = area->getRate();
        f32 zeroValue = 0.f;
        if (rate != zeroValue)
        {
            mEchoMix = 0.8f * area->getEchoRate();
            if (mEchoMix > 0.8f)
            {
                mEchoMix = 0.8f;
            }
        }
        else
        {
            mEchoMix = zeroValue;
        }
    }

    if (mKillSw || mKartType == 2)
    {
        return;
    }

    mRpm = rpm;
    mSpeed = speed;

    if (isBrake)
    {
        if (speed < 1.0f)
        {
            isBrake = 0;
        }
    }

    if (isBrake)
    {
        mAccelFlag = brakeBtn;
    }
    else
    {
        mAccelFlag = accelBtn;
    }
    mConductStatus = conductStatus;

    u8 characterType = Parameters::getCharacterType(mKartIndex);

    if (characterType != 9)
    {
        switch (mConductStatus)
        {
        case 0:
            setConductSignal();
            setWaterCutoffPort(0);
            break;
        case 2:
            setConductOutOfCourse(conductStatus);
            setWaterCutoffPort(0);
            if (mKartType != 0)
            {
                break;
            }
            if (mPrevConductStatus == conductStatus)
            {
                break;
            }
            if (Parameters::getRaceCourse() != 0x2f)
            {
                break;
            }
            startSoundHandleNumber(cHandleCourse, 0x40074, 0);
            break;
        case 4:
            setConductTrouble(rpm, conductStatus);
            setWaterCutoffPort(0);

            if (mKartType != 0)
            {
                break;
            }
            if (mPrevConductStatus == conductStatus)
            {
                break;
            }

            if (Parameters::getRaceCourse() == 0x2a)
            {
                startSoundHandleNumber(cHandleCourse, 0x40057, 0);
                break;
            }
            if (Parameters::getRaceCourse() != 0x2f)
            {
                break;
            }
            startSoundHandleNumber(cHandleCourse, 0x40074, 0);
            break;
        case 3:
            countGoalKart();
            mAccelFlag = 1;
            setConductAfterGoal(isBrake);
            mGoalFlag = 1;
            break;
        case 1:
            setConductRace(isBrake);
            break;
        case 5:
            if (mKartType == 0 && mPrevConductStatus != conductStatus &&
                Parameters::getRaceCourse() == 0x2f)
            {
                startSoundHandleNumber(cHandleCourse, 0x40074, 0);
            }
            setConductPressed();
            break;
        case 6:
        default:
            break;
        }
    }
    else
    {
        switch (mConductStatus)
        {
        case 0:
        case 2:
        case 4:
            if (mPrevConductStatus != conductStatus)
            {
                mAccelTimer = 0;
                mEngineLevel = 0;
            }
            setConductLocomotiveAccel();
            if (mKartType != 0)
            {
                break;
            }
            if (mPrevConductStatus == conductStatus)
            {
                break;
            }
            if (Parameters::getRaceCourse() == 0x2a)
            {
                startSoundHandleNumber(cHandleCourse, 0x40057, 0);
                break;
            }
            if (Parameters::getRaceCourse() == 0x2f)
            {
                startSoundHandleNumber(cHandleCourse, 0x40074, 0);
            }
            break;

        case 3:
            countGoalKart();
            mAccelFlag = 1;
            mGoalFlag = 1;

        case 1:
            if (mPrevConductStatus != conductStatus)
            {
                mAccelTimer = 0;
                mEngineLevel = 0;
            }

            setConductLocomotiveSpeed(isBrake);
            break;
        case 5:
            if (mKartType == 0 && mPrevConductStatus != conductStatus)
            {
                if (Parameters::getRaceCourse() == 0x2f)
                {
                    startSoundHandleNumber(cHandleCourse, 0x40074, 0);
                }
            }

            setConductPressed();
            break;
        case 6:
        default:
            break;
        }
    }

    mPrevRpm = rpm;
    mPrevConductStatus = conductStatus;
    mPrevSpeed = mSpeed;
}

void KartSoundMgr::setWaterCutoffPort(u16 port)
{
    const u8 cHandleEngine = 3;
    JAISoundHandle &handle = (*this)[cHandleEngine];
    if (handle.isSoundAttached())
    {
        handle->getTrack()->writePort(0xa, port);
    }
}

void KartSoundMgr::getEngineIDOffsetAtt() {}  // UNUSED

void KartSoundMgr::countGoalKart()
{
    if (mGoalFlag != 0)
    {
        return;
    }

    if (mPrevConductStatus == mConductStatus)
    {
        return;
    }
    smGoalKartCount++;
}

// FABRICATED {
void KartSoundMgr::startSoundFromID(u32 id)
{
    const u8 cHandleEngine = 3;
    u32 scene;
    JAISound *sound;

    JAISoundStarter *soundStarter = JASGlobalInstance<JAISoundStarter>::getInstance();

    JAISoundHandle &handle = (*this)[cHandleEngine];

    id += (mKartType != 0 ? 0x14 : 0);

    soundStarter->startSound(id, &handle, NULL);

    if (handle.isSoundAttached())
    {
        JAIAudible *audible = handle->getAudible();
        if (audible == 0)
        {
            scene = mSceneMask;

            sound = handle.operator->();
            JGeometry::TVec3f vec(*mSoundPos);

            sound->newAudible(vec, &_18, scene, NULL);
            if (mEngineFadeCount != 0)
            {

                handle->fader_.fadeInFromOut2(mEngineFadeCount);
                mEngineFadeCount = 0;
            }
        }
        setEcho(&handle, mEchoMix);
    }
}
// } FABRICATED

void KartSoundMgr::setConductLocomotiveAccel()
{
    const u8 cHandleEngine = 3;
    f32 pitch;
    f32 volume;

    bool changed = false;

    if (mAccelFlag != 0)
    {
        if (mAccelTimer == 0)
        {
            if (mEngineLevel == 0)
            {
                mEngineLevel = 9;
            }
            else if (mEngineLevel > 4)
            {
                mEngineLevel--;
            }
            changed = true;
            volume = 0.6f;
            mAccelTimer = mEngineLevel;
        }
    }
    else if (mAccelTimer == 0)
    {
        if (mEngineLevel == 0)
        {
            mEngineLevel = 9;
        }
        if (mEngineLevel < 9)
        {
            mEngineLevel++;
            mAccelTimer = mEngineLevel;
        }
        else
        {
            mAccelTimer = mEngineLevel;
        }
        changed = true;
        volume = 0.4f;
    }

    if (changed)
    {
        const u32 soundID = ((mEngineRevCount++ & 1) ? 0x11 : 0x12);

        startSoundFromID(soundID);
        JAISoundHandle &handle = (*this)[cHandleEngine];
        if (!handle.isSoundAttached())
        {
            return;
        }
        handle->getAuxiliary().moveVolume(volume, 0);
        pitch = 0.6f + ((0.4f * (9 - mEngineLevel)) / 5.f);
        handle->getAuxiliary().movePitch(pitch, 0);
    }
    else
    {
        if (mAccelTimer != 0)
        {
            mAccelTimer--;
        }
    }
}

void KartSoundMgr::setConductLocomotiveSpeed(bool)
{
    const u8 cHandleEngine = 3;
    f32 speed;
    u32 soundID;

    speed = mSpeed;
    if (mAccelTimer == 0)
    {
        if (speed < 1.f)
        {
            speed = 1.f;
        }
        if (speed > 130.f)
        {
            speed = 130.f;
        }
        mAccelTimer = (u16)(3.f + ((130.f - speed) / 20.f));

        soundID = 0x12 + -(mEngineRevCount++ & 1);

        startSoundFromID(soundID);

        JAISoundHandle &handle = (*this)[cHandleEngine];
        if (!handle.isSoundAttached())
        {
            return;
        }
        JAISound *sound = handle.operator->();
        sound->getAuxiliary().moveVolume((0.7f * (speed / 130.f)) + 0.5f, 0);
        f32 pitch = 1.f;
        if (speed < 100.f)
        {
            pitch = speed / 100.f;
        }
        if (mChibiFlag != 0)
        {
            const f32 chibiPitch = Parameters::getChibiPitch(soundID);
            pitch *= chibiPitch;
        }
        handle->getAuxiliary().movePitch((0.4f * pitch) + 0.6f, 0);
    }
    else
    {
        mAccelTimer--;
    }
}

void KartSoundMgr::setConductPressed()
{
    const u8 cHandleEngine = 3;
    startSoundFromID(0x13);

    f32 pitch = mSpeed;
    if (pitch > 100.f)
    {
        pitch = 100.f;
    }

    pitch = 1.f + (pitch / 100.f);

    JAISoundHandle &handle = (*this)[cHandleEngine];
    if (!handle.isSoundAttached())
    {
        return;
    }

    handle->getAuxiliary().movePitch(pitch, 0);
}

}  // namespace GameAudio

static const f32 EngineKarabukashiLength[] = {90.f, 40.f, 90.f, 30.f, 40.f, 35.f, 30.f, 95.f, 90.f};

static const f32 EnginePitchKeisuuSignal[] =
    {0.02f, 0.05f, 0.02f, 0.09f, 0.05f, 0.06f, 0.06f, 0.02f, 0.05f};

static const f32 EnginePitchDownSignal[] = {0.9f, 0.9f, 0.6f, 0.9f, 0.9f, 0.7f, 0.7f, 0.6f, 0.8f};

static const f32 EnginePitchKeisuuOutOfCourse[] =
    {0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f};

static const f32 EnginePitchKeisuuTrouble[] =
    {0.025f, 0.025f, 0.025f, 0.025f, 0.025f, 0.025f, 0.025f, 0.025f, 0.025f};

static const f32 EngineAddKeisuuRaceUp[] =
    {0.0006f, 0.0006f, 0.0006f, 0.0005f, 0.0012f, 0.0012f, 0.0012f, 0.0012f, 0.0012f};

static const f32 UpEngineLoopStart[] =
    {100639.f, 133568.f, 121677.f, 89762.f, 0.f, 0.f, 0.f, 0.f, 0.f};

static const f32 UpEngineLoopEnd[] =
    {113023.f, 167266.f, 138431.f, 100738.f, 0.f, 0.f, 0.f, 0.f, 0.f};

static const f32 EngineAddKeisuuRaceDown[] =
    {0.00015f, 0.00015f, 0.00015f, 0.00015f, 0.0006f, 0.0006f, 0.0006f, 0.0006f, 0.0006f};

static const f32 DownEngineLoopStart[] =
    {42032.f, 57452.f, 54457.f, 33775.f, 0.f, 0.f, 0.f, 0.f, 0.f};

static const f32 DownEngineLoopEnd[] =
    {93600.f, 104350.f, 79743.f, 63943.f, 0.f, 0.f, 0.f, 0.f, 0.f};

namespace GameAudio
{

void KartSoundMgr::setConductSignal()
{
    const u8 cHandleEngine = 3;
    if (mPrevConductStatus != mConductStatus)
    {
        mKarabukashiCount = 0.f;
    }

    f32 pitch;
    f32 volume;
    if (mKartType == 0 || Parameters::getNetworkCubes() != 1)
    {
        if (mAccelFlag == 1)
        {
            volume = 1.4f;
            f32 length = EngineKarabukashiLength[(u8)Parameters::getEngineType(mKartIndex)];
            if (mKarabukashiCount < length)
            {
                mKarabukashiCount = mKarabukashiCount + 1.f;
            }

            f32 signal = EnginePitchKeisuuSignal[(u8)Parameters::getEngineType(mKartIndex)];
            pitch = (mKarabukashiCount * signal) + 0.7f;
        }
        else
        {
            volume = 0.7f;
            if (mKarabukashiCount > 0.f)
            {
                f32 signal = EnginePitchDownSignal[(u8)Parameters::getEngineType(mKartIndex)];
                mKarabukashiCount -= signal;
            }
            else
            {
                mKarabukashiCount = 0.f;
            }
            f32 signal = EnginePitchKeisuuSignal[(u8)Parameters::getEngineType(mKartIndex)];
            pitch = (mKarabukashiCount * signal) + 0.7f;
        }
        if (mKarabukashiCount < 20.f)
        {
            volume = ((-0.015000001f * mKarabukashiCount) + volume) + 0.3f;
        }
    }
    else
    {
        if (mSignalPitchCounter == 0)
        {
            mSignalRevUp ^= 1;
            if (mSignalRevUp != 0)
            {
                u32 interval =
                    SignalUpIntervalBase[mRankClass] +
                    (Random::getSignalEngineRandomU32() % SignalUpIntervalWidth[mRankClass]);
                u32 pitchInt = SignalUpPitchBase[mRankClass] + (Random::getSignalEngineRandomU32() %
                                                                SignalUpPitchWidth[mRankClass]);
                f32 currentPitch = f32(pitchInt) / 100.f;
                if ((mSignalPitchCounter != 0 || mSignalPitch != currentPitch) &&
                    (mSignalPitchCounter == 0 || mSignalTargetPitch != currentPitch))
                {
                    mSignalTargetPitch = currentPitch;
                    if (interval == 0)
                    {
                        mSignalPitch = currentPitch;
                    }
                    else
                    {
                        mSignalPitchStep =
                            (mSignalPitch - mSignalTargetPitch) / (f32)(interval + 1);
                        mSignalPitchCounter = interval + 1;
                    }
                }
            }
            else
            {
                u32 interval =
                    SignalDownIntervalBase[mRankClass] +
                    (Random::getSignalEngineRandomU32() % SignalDownIntervalWidth[mRankClass]);
                f32 volumeC = 0.7f;
                if ((mSignalPitchCounter != 0 || mSignalPitch != volumeC) &&
                    (mSignalPitchCounter == 0 || mSignalTargetPitch != volumeC))
                {
                    mSignalTargetPitch = 0.7f;
                    if (interval == 0)
                    {
                        mSignalPitch = 0.7f;
                    }
                    else
                    {
                        u32 ticks = interval + 1;
                        mSignalPitchStep = (mSignalPitch - mSignalTargetPitch) / (f32)ticks;
                        mSignalPitchCounter = ticks;
                    }
                }
            }
        }

        if (mSignalPitchCounter != 0)
        {
            if (--mSignalPitchCounter != 0)
            {
                mSignalPitch -= mSignalPitchStep;
            }
            else
            {
                mSignalPitch = mSignalTargetPitch;
            }
        }
        if (mSignalRevUp != 0)
        {
            volume = 1.4f;
        }
        else
        {
            volume = 0.7f;
        }

        pitch = mSignalPitch;
        volume = volume * GA_ENEMY_VOLUME_DOWN_VALUE;
    }

    u8 engineType = Parameters::getEngineType(mKartIndex);
    startSoundFromID(engineType + 8);

    JAISoundHandle &handle = (*this)[cHandleEngine];
    if (handle.isSoundAttached())
    {
        handle->getAuxiliary().moveVolume(volume, 0);
        handle->getAuxiliary().movePitch(pitch, 0);
    }
}

void KartSoundMgr::setConductOutOfCourse(u8 conductStatus)
{
    const f32 cEnemyVolumeScale = 0.65f;
    const u8 cHandleEngine = 3;
    f32 volume;

    if (mPrevConductStatus != conductStatus)
    {
        mOutOfCourseLevel = 0;
    }
    if (mAccelFlag == 1)
    {
        if (mOutOfCourseLevel < 0x64)
        {
            mOutOfCourseLevel++;
            mOutOfCourseLevel++;
        }
        volume = 1.2f;
    }
    else
    {
        if (mOutOfCourseLevel != 0)
        {
            mOutOfCourseLevel--;
        }
        volume = 0.55f;
    }

    f32 pitch = (EnginePitchKeisuuOutOfCourse[(u8)Parameters::getEngineType(mKartIndex)] *
                 mOutOfCourseLevel) +
                0.6f;

    if (mKartType != 0)
    {
        volume *= GA_ENEMY_VOLUME_DOWN_VALUE;
    }

    const u8 engineType = Parameters::getEngineType(mKartIndex);
    startSoundFromID(engineType + 8);

    JAISoundHandle &handle = (*this)[cHandleEngine];
    if (!handle.isSoundAttached())
    {
        return;
    }

    if (mChibiFlag != 0)
    {
        const f32 chibiPitch = Parameters::getChibiPitch(handle->getID());
        pitch *= chibiPitch;
        volume *= cEnemyVolumeScale;
    }
    handle->getAuxiliary().moveVolume(volume, 0);
    handle->getAuxiliary().movePitch(pitch, 0);
}

void KartSoundMgr::setConductTrouble(f32 rpm, u8 conductStatus)
{
    const f32 cEnemyVolumeScale = 0.65f;
    const u8 cHandleEngine = 3;
    f32 volume;

    if (mPrevConductStatus != conductStatus)
    {
        mEngineLevel = (u16)(38.f * rpm);
    }
    if (mAccelFlag == 1)
    {
        if (mEngineLevel < 0x26)
        {
            mEngineLevel++;
            mEngineLevel++;
        }
        volume = 1.2f;
    }
    else
    {
        if (mEngineLevel != 0)
        {
            mEngineLevel--;
        }
        volume = 0.55f;
    }

    f32 pitch =
        (EnginePitchKeisuuTrouble[(u8)Parameters::getEngineType(mKartIndex)] * mEngineLevel) + 0.5f;

    if (mKartType != 0)
    {
        volume *= GA_ENEMY_VOLUME_DOWN_VALUE;
    }

    const u8 engineType = Parameters::getEngineType(mKartIndex);
    startSoundFromID(engineType + 8);

    JAISoundHandle &handle = (*this)[cHandleEngine];
    if (!handle.isSoundAttached())
    {
        return;
    }

    if (mChibiFlag != 0)
    {
        const f32 chibiPitch = Parameters::getChibiPitch(handle->getID());
        pitch *= chibiPitch;
        volume *= cEnemyVolumeScale;
    }
    handle->getAuxiliary().moveVolume(volume, 0);
    handle->getAuxiliary().movePitch(pitch, 0);
}

void KartSoundMgr::setConductRace(bool isBrake)
{
    const f32 cEnemyVolumeScale = 0.65f;
    const u8 cHandleEngine = 3;
    const u8 cWheelCount = 4;
    const u16 cEngineTimerMax = 0x270f;
    f32 depth;
    f32 skipSample;
    f32 minWaterDepth;
    u16 waterPort = 0;
    u32 soundOffset;
    const u32 engineType = Parameters::getEngineType((u8)mKartIndex);

    if (mGroundAttrs[0] == 0x11 || mGroundAttrs[1] == 0x11 || mGroundAttrs[2] == 0x11 ||
        mGroundAttrs[3] == 0x11)
    {
        minWaterDepth = 215.f;
        for (u8 index = 0; index < cWheelCount; index++)
        {
            depth = mWaterDepths[index];
            if (minWaterDepth > depth)
            {
                minWaterDepth = depth;
            }
        }
        f32 waterRatio;
        if (minWaterDepth < 15.f)
        {
            waterRatio = 0.f;
        }
        else
        {
            if (minWaterDepth > 215.f)
            {
                minWaterDepth = 215.f;
            }
            waterRatio = (minWaterDepth - 15.f) / 200.f;
        }
        if (waterRatio == 0.f)
        {
            waterPort = 0;
        }
        else
        {
            waterPort = 127.f - ((126.f * waterRatio));
        }
    }

    if ((1.f + mSpeed) > mPrevSpeed)
    {
        if (mSpeed < 1.f)
        {
            mLowSpeedCount += 1;
            if (mLowSpeedCount == 0x14)
            {
                mAccelFlag = 0;
                mLowSpeedCount = 0;
            }
        }
        if (mSpeed > 50.f)
        {
            if (mEngineRevCount < 0x320)
            {
                mEngineRevCount++;
            }
        }
    }
    else
    {
        if (mSpeed < 10.f)
        {
            mAccelFlag = 0;
        }
        if (mEngineRevCount >= 0x28)
        {
            mEngineRevCount -= 0x28;
        }
    }

    if (mAccelFlag == 1)
    {
        if (mKarabukashiCount > 0.f)
        {
            mKarabukashiCount = mKarabukashiCount - 1.f;
        }

        mDecelTimer = 0;
        if (mAccelTimer < cEngineTimerMax)
        {
            mAccelTimer++;
        }
        f32 volume;
        f32 pitch;
        const f32 temp_pitch = ((-0.0016666667f * mSpeed) + 0.3f);

        if (mCrushDegree != 0.f)
        {
            const s16 revDrop = (80.f * mCrushDegree);
            if (mEngineRevCount > -0xC8)
            {
                mEngineRevCount -= revDrop;
            }
        }
        adjustEngine();
        if (mEngineAdjust > 0.f)
        {
            mKarabukashiCount = 0.f;
        }
        u8 index = engineType;
        pitch = 0.02f * mKarabukashiCount +
                (mEngineAdjust +
                 (temp_pitch + (0.5f + (mEngineRevCount * (EngineAddKeisuuRaceUp[index]) +
                                        ((mRpm * EngineKeisuuRaceUp[index]) +
                                         (mSpeed * EngineKeisuuRaceUp[index]))))));
        volume = 1.4f;

        if (mSpeed < 40.f)
        {
            volume = 0.3f + (-0.0075000003f * mSpeed + 1.4f);
        }

        if ((u8)engineType < 4 && !isBrake)
        {
            soundOffset = 0;
        }
        else
        {
            soundOffset = 8;
        }

        if (mKartType != 0)
        {
            volume *= GA_ENEMY_VOLUME_DOWN_VALUE;
        }
        if (waterPort != 0)
        {
            volume *= 0.5f + ((0.5f * waterPort) / 127.f);
        }
        startSoundFromID(soundOffset + (u8)engineType);

        JAISoundHandle &handle = (*this)[cHandleEngine];
        if (handle.isSoundAttached())
        {
            if (mChibiFlag != 0)
            {
                pitch *= Parameters::getChibiPitch(handle->getID());
                volume *= cEnemyVolumeScale;
            }
            handle->getAuxiliary().moveVolume(volume, 0);
            handle->getAuxiliary().movePitch(pitch, 0);
            handle->getTrack()->writePort(0xA, waterPort);
        }

        if ((u8)engineType < 4)
        {
            skipSample = (f32)(u32)(mSpeed * (UpEngineLoopStart[(u8)engineType] / 150.f) - 10000.f);
            if (skipSample < 0.f)
            {
                skipSample = 0.f;
            }
            if (skipSample > UpEngineLoopEnd[(u8)engineType])
            {
                skipSample = UpEngineLoopEnd[(u8)engineType];
            }

            // conversion happens before operator[] in the target
            u32 skip = (u32)skipSample;
            JAISoundHandle &handle = (*this)[cHandleEngine];
            if (handle.isSoundAttached())
            {
                handle->getTrack()->setSkipSample(skip);
            }
        }
    }
    else
    {
        f32 volume;
        f32 pitch;
        mEngineRevCount = 0;
        mAccelTimer = 0;
        if (mDecelTimer < cEngineTimerMax)
        {
            mDecelTimer++;
        }
        adjustEngine();

        u8 index = engineType;
        pitch = mEngineAdjust + (0.7f + (mEngineRevCount * (EngineAddKeisuuRaceDown[index]) +
                                         ((mRpm * EngineKeisuuRaceDown[index]) +
                                          (mSpeed * EngineKeisuuRaceDown[index]))));

        if ((u8)engineType < 4)
        {
            volume = 1.1f;
        }
        else
        {
            volume = 0.8f;
        }
        if (mSpeed < 40.f)
        {
            volume = 0.4f + ((-0.01f * mSpeed) + volume);
        }

        if ((u8)engineType < 4 && !isBrake)
        {
            soundOffset = 4;
        }
        else
        {
            soundOffset = 8;
        }

        if (mKartType != 0)
        {
            volume *= GA_ENEMY_VOLUME_DOWN_VALUE;
        }
        startSoundFromID(soundOffset + (u8)engineType);

        JAISoundHandle &handle = (*this)[cHandleEngine];
        if (handle.isSoundAttached())
        {
            if (mChibiFlag != 0)
            {
                pitch *= Parameters::getChibiPitch(handle->getID());
                volume *= cEnemyVolumeScale;
            }
            handle->getAuxiliary().moveVolume(volume, 0);
            handle->getAuxiliary().movePitch(pitch, 0);
            handle->getTrack()->writePort(0xA, waterPort);
        }

        if ((u8)engineType < 4)
        {
            const f32 loopStart = DownEngineLoopStart[(u8)engineType];

            skipSample = (f32)(u32)(mSpeed * -(loopStart / 150.f) + loopStart - 5000.f);
            if (skipSample < 0.f)
            {
                skipSample = 0.f;
            }
            if (skipSample > DownEngineLoopEnd[(u8)engineType])
            {
                skipSample = DownEngineLoopEnd[(u8)engineType];
            }

            u32 skip = skipSample;
            JAISoundHandle &handle = (*this)[cHandleEngine];
            if (handle.isSoundAttached())
            {
                handle->getTrack()->setSkipSample(skip);
            }
        }
    }
}

void KartSoundMgr::setConductAfterGoal(bool isBrake)
{
    setConductRace(isBrake);
}

void KartSoundMgr::setCrushSe(CrsGround::EMat mat, f32 degree)
{
    switch (mat)
    {
    case CrsGround::Mat_5:
    case CrsGround::Mat_17:
        setCrushSe(0x1001DU, degree);
        return;
    case CrsGround::Mat_8:
        setCrushSe(0x10015U, degree);
        return;
    case CrsGround::Mat_7:
    case CrsGround::Mat_15:
        setCrushSe(0x10021U, degree);
        return;
    case CrsGround::Mat_16:
        setCrushSe(0x10023U, degree);
        return;
    case CrsGround::Mat_18:
        setCrushSe(0x10053U, degree);
        return;
    case CrsGround::Mat_19:
        setCrushSe(0x10056U, degree);
        return;
    case CrsGround::Mat_9:
        setCrushSe(0x10019U, degree);
        return;
    case CrsGround::Mat_25:
        setCrushSe(0x10067U, degree);
        return;
    case CrsGround::Mat_27:
        setCrushSe(0x1007DU, degree);
        return;
    case CrsGround::Mat_255:
        break;
    default:
        setCrushSe(0x10015U, degree);
        return;
    }
}

void KartSoundMgr::setCrushSe(u32 soundID, f32 degree)
{
    const u32 cCrushSeIntervalFrames = 60;
    const u8 cHandleCrush = 0;
    soundID = Common::changeRandomId(soundID, 0);
    const u32 randomId2 = Random::getRandomU32();

    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        return;
    }

    const Main *main = Main::getAudio();

    if (main->getFrameCount() - mLastCrushFrame < cCrushSeIntervalFrames)
    {
        return;
    }

    mLastCrushFrame = main->getFrameCount();

    if (mCrushWait != 0)
    {
        return;
    }

    startSoundHandleNumber(cHandleCrush, soundID, 0);

    JAISoundHandle &handle = (*this)[cHandleCrush];
    f32 volume;
    const f32 f30 = degree + 0.6f;
    if (handle.isSoundAttached())
    {
        volume = 0.8f * f30;
        handle->getAuxiliary().moveVolume(volume, 0);
        mCrushDegree = f30;
    }

    const u8 crushRand = (randomId2 & 0x7);

    mCrushWait = crushRand + 4;
}

void KartSoundMgr::setBrakeSe(u32 soundID)
{
    const u8 cHandleBrake = 6;
    f32 volume;
    f32 pitch;

    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        return;
    }

    startSoundHandleNumber(cHandleBrake, soundID, 0);
    JAISoundHandle &handle = (*this)[cHandleBrake];
    if (handle.isSoundAttached())
    {
        volume = (0.008f * mSpeed) + 0.25f;
        pitch = (0.003f * mSpeed) + 0.6f;
        if (mChibiFlag != 0)
        {
            pitch = pitch * Parameters::getChibiPitch(soundID);
        }
        handle->getAuxiliary().moveVolume(0.8f * volume, 0);
        handle->getAuxiliary().movePitch(pitch, 0);
    }
    setChibiPitch(&handle);
}

void KartSoundMgr::setDashSe(u32 soundID)
{
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType == 1)
    {
        if ((u8)Parameters::getRaceMode() != 2)
        {
            return;
        }
    }

    this->setSe(soundID);

    const u8 engineType = Parameters::getEngineType(mKartIndex);
    const f32 adjustInitialValue = DashEngineAdjustInitialValue[engineType];

    mEngineAdjust = adjustInitialValue;
    mAdjustSoundID = 0x10014;
    mAdjustCounter = 0x64;
}

void KartSoundMgr::setMiniturboSe(u32 soundID)
{
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        return;
    }

    this->setSe(soundID);

    const u8 engineType = Parameters::getEngineType(mKartIndex);

    const f32 adjustInitialValue = DashEngineAdjustInitialValue[engineType];

    mEngineAdjust = adjustInitialValue;
    mAdjustSoundID = 0x10044;
    mAdjustCounter = 0x28;
}

void KartSoundMgr::setJumpUpSe(u32 soundID)
{
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (Parameters::getRaceCourse() != 0x22)
    {
        return;
    }

    this->setSe(soundID);
}

void KartSoundMgr::setBoundSe(f32 degree)
{
    const u8 cGroundSeCount = 25;
    const u8 cHandleBound = 4;
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType == 1 && (u8)Parameters::getRaceMode() != 2)
    {
        return;
    }

    if (mGroundSeIndex >= cGroundSeCount)
    {
        return;
    }

    startSoundHandleNumber(cHandleBound, BoundSe[mGroundSeIndex], 0);

    JAISoundHandle &handle = (*this)[cHandleBound];

    if (handle.isSoundAttached())
    {
        handle->getAuxiliary().movePitch((0.5f * degree) + 0.5f, 0);
        if (handle.isSoundAttached())
        {
            handle->getAuxiliary().moveVolume(degree * mCameraVolume, 0);
        }
    }
    setChibiPitch(&handle);
}

void KartSoundMgr::setWheelSpinSe()
{
    const u8 cGroundSeCount = 25;
    const u8 cHandleBound = 4;
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        return;
    }

    const u8 characterType = Parameters::getCharacterType(mKartIndex);

    if (characterType == 1)
    {
        startSoundHandleNumber(cHandleBound, 0x10077, 0);
    }
    else
    {
        if (mGroundSeIndex < cGroundSeCount)
        {
            startSoundHandleNumber(cHandleBound, WheelSpinSe[mGroundSeIndex], 0);
        }
    }

    JAISoundHandle &handle = (*this)[cHandleBound];
    setChibiPitch(&handle);
}

void KartSoundMgr::setSpinSe()
{
    const u8 cGroundSeCount = 25;
    const u8 cHandleSpin = 1;
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    const u8 characterType = Parameters::getCharacterType(mKartIndex);

    if (characterType == 1)
    {
        startSoundHandleNumber(cHandleSpin, 0x100ae, 0);
    }
    else
    {
        if (mGroundSeIndex < cGroundSeCount)
        {
            startSoundHandleNumber(cHandleSpin, SpinSe[mGroundSeIndex], 0);
        }
    }

    JAISoundHandle &handle = (*this)[cHandleSpin];
    setChibiPitch(&handle);
}

void KartSoundMgr::setSpinTurnSe()
{
    const u8 cGroundSeCount = 25;
    const u8 cHandleSpin = 1;
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        return;
    }

    const u8 characterType = Parameters::getCharacterType(mKartIndex);

    if (characterType == 1)
    {
        startSoundHandleNumber(cHandleSpin, 0x100c6, 0);
    }
    else
    {
        if (mGroundSeIndex < cGroundSeCount)
        {
            startSoundHandleNumber(cHandleSpin, SpinTurnSe[mGroundSeIndex], 0);
        }
    }

    JAISoundHandle &handle = (*this)[cHandleSpin];
    setChibiPitch(&handle);
}

void KartSoundMgr::setSe(u32 soundID)
{
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    Main *const main = Main::getAudio();
    u32 swBit;
    CustomSoundTable *const soundTable = main->getSoundTable();
    if (mConductStatus == 3)
    {
        swBit = soundTable->getSwBit(soundID);
        if (swBit & 0x8000000)
        {
            return;
        }
    }

    if (mKartType != 0)
    {
        swBit = soundTable->getSwBit(soundID);
        if (swBit & 0x80000000)
        {
            return;
        }
    }

    if (soundID - 0x10000 == 0x41)
    {
        if (0.f == Main::getAudio()->getBgmVolume())
        {
            return;
        }
    }
    u32 scene = 0;
    CameraMgr *camera = Main::getAudio()->getCamera();

    if (camera->getSceneMax() > 1 && camera->getSceneMax() > mPlayerIndex)
    {
        scene = (1 << mPlayerIndex) ^ 0xf;
    }

    JAISoundHandle *soundCustomHandle = startSoundCustom(soundID, scene);

    // Double check needed for isSoundAttached..
    if (mKartType != 0 && soundCustomHandle != NULL && soundCustomHandle->isSoundAttached() &&
        soundCustomHandle->isSoundAttached())
    {
        (*soundCustomHandle)->getAuxiliary().moveVolume(0.8f * mCameraVolume, 0);
    }

    if (soundID - 0x10000 != 0x38 && soundID - 0x10000 != 0x39)
    {
        setChibiPitch(soundCustomHandle);
    }

    setEcho(soundCustomHandle, mEchoMix);
}

void KartSoundMgr::setChibiPitch(JAISoundHandle *handle)
{
    if (mChibiFlag == 0)
    {
        return;
    }

    if (handle == NULL)
    {
        return;
    }

    if (handle->isSoundAttached())
    {
        (*handle)->getAuxiliary().movePitch(Parameters::getChibiPitch((*handle)->getID()), 0);
    }
}

void KartSoundMgr::adjustEngine()
{
    const u32 engineType = Parameters::getEngineType((u8)mKartIndex);
    if (mAdjustCounter != 0)
    {
        switch (mAdjustSoundID)
        {
        case 0x10014:
            if (mAdjustCounter > 0x78)
            {
                mEngineAdjust += DashEngineIncPerFrame[(u8)engineType];
                mAdjustCounter--;
                return;
            }
            if (mAdjustCounter > 0x6e)
            {
                mAdjustCounter--;
                return;
            }
            if (mAdjustCounter == 0 || mEngineAdjust < 0.0f)
            {
                mAdjustCounter = 0;
                mEngineAdjust = 0.f;
                mAdjustSoundID = 0;
                return;
            }
            mEngineAdjust -= DashEngineDecPerFrame[(u8)engineType];
            mAdjustCounter -= 1;
            return;

        case 0x10044:
            if (mAdjustCounter > 0x30)
            {
                mEngineAdjust = 0.05f + mEngineAdjust;
                mAdjustCounter--;
                return;
            }
            if (mAdjustCounter > 0x2c)
            {
                mAdjustCounter--;
                return;
            }
            if (mAdjustCounter == 0 || mEngineAdjust < 0.0f)
            {
                mAdjustCounter = 0;
                mEngineAdjust = 0.f;
                mAdjustSoundID = 0;
                return;
            }
            mEngineAdjust -= 0.02f;
            mAdjustCounter -= 1;
            return;
        }
    }
}

void KartSoundMgr::crushRenzokuTaisaku()
{
    if (mCrushWait != 0)
    {
        mCrushWait--;
        if (mCrushWait == 0)
        {
            mCrushDegree = 0.f;
        }
    }
}

void KartSoundMgr::slipParamSet()
{
    const u8 cNoGroundSe = 0xff;
    const f32 cMaxSlipSpeed = 150.f;
    const u8 cWheelCount = 4;
    const u8 cHandleSlip = 2;
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        return;
    }

    const u8 playerMode = Parameters::getPlayerMode();

    if (mSlipFlags[0] != 1 && mSlipFlags[1] != 1 && mSlipFlags[2] != 1 && mSlipFlags[3] != 1)
    {
        return;
    }

    f32 clampedSpeed = MIN(cMaxSlipSpeed, mSpeed);
    switch (mSlipSeIndex)
    {
    case 6:
        mSlipSeCount += 1;
        mSlipSeInterval = 30.f + ((-25.f * clampedSpeed) / cMaxSlipSpeed);
        if (mSlipSeCount > mSlipSeInterval)
        {
            mSlipSeCount = 0;
            if (mSlipSeAlternator == 0)
            {
                startSoundHandleNumber(cHandleSlip, 0x10007, 0);
                mSlipSeAlternator = 1;
            }
            else
            {
                startSoundHandleNumber(cHandleSlip, 0x10008, 0);
                mSlipSeAlternator = 0;
            }
        }
        break;
    case 13:
        mSlipSeCount += 1;
        mSlipSeInterval = 30.f + ((-25.f * clampedSpeed) / cMaxSlipSpeed);
        if (mSlipSeCount > mSlipSeInterval)
        {
            startSoundHandleNumber(cHandleSlip, 0x1005c, 0);
        }
        break;
    case 5:
        mSlipSeCount += 1;
        mSlipSeInterval = 15.f + ((-13.f * clampedSpeed) / cMaxSlipSpeed);
        if (mSlipSeCount > mSlipSeInterval)
        {
            mSlipSeCount = 0;
            if (mSlipSeAlternator == 0)
            {
                startSoundHandleNumber(cHandleSlip, 0x10005, 0);
                mSlipSeAlternator = 1;
            }
            else
            {
                startSoundHandleNumber(cHandleSlip, 0x10006, 0);
                mSlipSeAlternator = 0;
            }
        }
        break;
    case 10:
        startSoundHandleNumber(cHandleSlip, 0x10036, 0);
        break;
    case 11:
        startSoundHandleNumber(cHandleSlip, 0x1004e, 0);
        break;
    case 12:
        startSoundHandleNumber(cHandleSlip, 0x10059, 0);
        break;
    case 14:
        startSoundHandleNumber(cHandleSlip, 0x1004f, 0);
        break;
    case 15:
        startSoundHandleNumber(cHandleSlip, 0x10057, 0);
        break;
    case 16:
        startSoundHandleNumber(cHandleSlip, 0x10058, 0);
        break;
    case 17:
        startSoundHandleNumber(cHandleSlip, 0x1004D, 0);
        break;
    case 19:
        startSoundHandleNumber(cHandleSlip, 0x1003B, 0);
        break;
    case 20:
        startSoundHandleNumber(cHandleSlip, 0x1003A, 0);
        break;
    case 21:
        startSoundHandleNumber(cHandleSlip, 0x10082, 0);
        break;
    case 22:
        startSoundHandleNumber(cHandleSlip, 0x1005f, 0);
        break;
    case 23:
        startSoundHandleNumber(cHandleSlip, 0x10069, 0);
        break;
    case 7:
    case 8:
    case 9:
    case 18:
    default:
        if (mSlipSeIndex <= 9)
        {
            startSoundHandleNumber(cHandleSlip, 0x10000 + mSlipSeIndex, 0);
        }
        break;
    }
    f32 pan;
    f32 volume;
    f32 pitch;
    f32 panSum = 0.f;

    volume = 0.f;
    pitch = 0.f;
    mSlipSeIndex = cNoGroundSe;
    u8 count = 0;
    for (u8 index = 0; index < cWheelCount; index++)
    {
        if (mSlipFlags[index] == 1)
        {
            count++;

            if (playerMode == 0)
            {
                panSum += mSlipPans[index];
            }

            volume += mSlipVolumes[index];
            pitch += mSlipPitches[index];
            mSlipFlags[index] = 0;
        }
    }

    if (playerMode == 0)
    {
        panSum = panSum / count;
        pan = Common::panDeform(panSum, 3.f);
    }
    else
    {
        pan = Common::setMultiPlayModePan(mPlayerIndex);
    }

    volume = volume / count;
    pitch = pitch / count;

    JAISoundHandle &handle = (*this)[cHandleSlip];
    if (!handle.isSoundAttached())
    {
        return;
    }
    if (Parameters::getMirrorSwitch())
    {
        pan = 1.f - pan;
    }

    handle->getAuxiliary().movePan(pan, 3);

    if (handle.isSoundAttached())
    {
        handle->getAuxiliary().moveVolume(volume * mCameraVolume, 0);
    }

    if (mChibiFlag != 0)
    {
        pitch *= Parameters::getChibiPitch(handle->getID());
    }
    handle->getAuxiliary().movePitch(pitch, 0);
}

void KartSoundMgr::checkEcho()
{
    if (mPrevEchoMix == mEchoMix)
    {
        return;
    }

    for (s32 i = 0; i < _10; i++)
    {
        if ((*this)[i].isSoundAttached())
        {
            (*this)[i]->getAuxiliary().moveFxMix(mEchoMix, 0);
        }
    }

    mPrevEchoMix = mEchoMix;
}

void KartSoundMgr::setInvincibleBgm(u8 bit)
{
    const u8 cHandleInvincible = 5;
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        if (Main::getAudio()->getBgmVolume() == 0.f)
        {
            return;
        }
        switch (bit)
        {
        case 1:
            mInvincibleBgmBits |= 1;
            startSoundHandleNumber(cHandleInvincible, 0x1003F, 0);
            return;
        case 2:
            startSoundHandleNumber(cHandleInvincible, 0x10040, 0);
            mInvincibleBgmBits |= 2;
            return;
        default:
#line 0xb5b
            JUT_WARNING_F(false, "%s", "KartSoundMgr::startInvincibleBgm : 不明なBITです。\n");
            return;
        }
    }
    else
    {
        switch (bit)
        {
        case 1:
            Main::getAudio()->setInvincibleBgm(mKartIndex, bit);
            mInvincibleBgmBits |= 1;
            return;
        case 2:
            Main::getAudio()->setInvincibleBgm(mKartIndex, bit);
            mInvincibleBgmBits |= 2;
            return;
        default:
#line 0xb6b
            JUT_WARNING_F(false, "%s", "KartSoundMgr::startInvincibleBgm : 不明なBITです。\n");
            return;
        }
    }
}

void KartSoundMgr::clearInvincibleBgm(u8 bit)
{
    const u8 cHandleInvincible = 5;
    if (mKillSw || mKartType == 2)
    {
        return;
    }

    if (mKartType != 0)
    {
        if (Main::getAudio()->getBgmVolume() == 0.f)
        {
            return;
        }
        switch (bit)
        {
        case 1:
            mInvincibleBgmBits ^= 1;
            break;
        case 2:
            mInvincibleBgmBits ^= 2;
            break;
        case 3:
            mInvincibleBgmBits = 0;
            break;
        default:
#line 0xb87
            JUT_WARNING_F(false, "%s", "KartSoundMgr::startInvincibleBgm : 不明なBITです。\n");
            break;
        }
        if (mInvincibleBgmBits == 0)
        {
            JAISoundHandle &handle = (*this)[cHandleInvincible];
            if (handle.isSoundAttached())
            {
                handle->stop();
            }
        }
        else
        {
            switch (mInvincibleBgmBits)
            {
            case 1:
                startSoundHandleNumber(cHandleInvincible, 0x1003F, 0);
                break;
            case 2:
                startSoundHandleNumber(cHandleInvincible, 0x10040, 0);
                break;
                ;
            }
        }
    }
    else
    {
        switch (bit)
        {
        case 1:
            Main::getAudio()->clearInvincibleBgm(mKartIndex, bit);
            mInvincibleBgmBits ^= 1;
            return;
        case 2:
            Main::getAudio()->clearInvincibleBgm(mKartIndex, bit);
            mInvincibleBgmBits ^= 2;
            return;
        case 3:
            mInvincibleBgmBits = 0;
            return;
        default:
#line 0xba8
            JUT_WARNING_F(false, "%s", "KartSoundMgr::startInvincibleBgm : 不明なBITです。\n");
            return;
        }
    }
}

void KartSoundMgr::setChibiFlag(bool chibi, bool playSe)
{
    Parameters::setChibiFlag(mKartIndex, chibi);

    if (chibi)
    {
        if (mChibiFlag != chibi && playSe)
        {
            setSe(0x10038);
        }
        GameAudioMain::getAudio()->setThunderMode(mKartIndex);
    }
    else
    {
        if (mChibiFlag != chibi && playSe)
        {
            setSe(0x10039);
        }
        GameAudioMain::getAudio()->clearThunderMode(mKartIndex);

        for (s32 index = 0; index < _10; index++)
        {
            if ((*this)[index].isSoundAttached())
            {
                (*this)[index]->getAuxiliary().movePitch(1.f, 0);
            }
        }
    }
    mChibiFlag = chibi;
}

}  // namespace GameAudio
