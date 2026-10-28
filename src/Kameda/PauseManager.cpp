#include "Kameda/PauseManager.h"

#include "Inagaki/GameAudioMain.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/Goal2D.h"
#include "Kameda/J2DManager.h"
#include "Kameda/Pause2D.h"
#include "Kameda/PrintMemoryCard.h"
#include "Kameda/Result2D.h"
#include "Kameda/SequenceInfo.h"
#include "Kameda/WipeManager.h"
#include "Kaneshige/RaceInfo.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/NetGameMgr.h"
#include "Osako/RaceApp.h"
#include "Osako/ResMgr.h"
#include "Osako/kartPad.h"
#include "Osako/system.h"
#include "mathHelper.h"  // For unused data

PauseManager *PauseManager::mThis;
bool PauseManager::mIsPause;
int PauseManager::mPauseSelector;
int PauseManager::mPauseChoice;
int PauseManager::mPauseNextChoice;
KartGamePad *PauseManager::mPausePad;

static int button;

PauseManager::PauseManager(JKRHeap *heap)
{
    if (!heap)
    {
        heap = JKRHeap::getCurrentHeap();
    }

    mArchive = ResMgr::getArchive(ResMgr::mcArcMRAMLoc);
    mOrthoGraph = System::getJ2DOrtho();
    mPrintMemoryCard = new (heap, 0) PrintMemoryCard(heap);

    mThis = this;

    mNetFlag = NetGameMgr::ptr()->getState() != 0;
    _12 = 0;
    mExec = false;

    mPause2D = new (heap, 0) Pause2D(heap);
    mResult2D = new (heap, 0) Result2D(heap);

    _11 = 1;
    for (int i = 0; i < RaceMgr::getManager()->getKartNumber(); ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            if (gSequenceInfo.getPad2Player(gRaceInfo.getKartInfo(i)->getPad(j)) != -1)
            {
                _11 = 0;
            }
        }
    }

    reset();
}

PauseManager::~PauseManager()
{
    // Unknown implementation
}

void PauseManager::reset()
{
    _12 = 1;
    mExec = true;
    GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);

    mIsPause = false;
    mPauseSelector = 0;
    mPauseChoice = 13;
    mPauseNextChoice = 13;
    mPausePad = 0;

    _c = 0;
    _d = 0;
    mResultStart = false;
    _f = 0;
    mGameOver = false;
    _14 = -1;
    mPauseEnd = false;
    mPause2D->init();
    mResult2D->init();
    _28 = 0;
    mWipeOutFrame = 0;
    mWipeOutDuration = 0;
    _34 = 0;
    _38 = 0;

    if (RaceMgr::getManager()->isReplayMode() && RaceMgr::getManager()->getRaceMode() == GRAND_PRIX)
    {
        Result2D::setGPClr();
    }

    mPrintMemoryCard->init(PrintMemoryCard::mcMsg40);
}

void PauseManager::setPause()
{
    // Unknown implementation
}

void PauseManager::clrPause()
{
    // Unknown implementation
}

void PauseManager::draw()
{
    if (!_12)
        return;

    if (!mNetFlag)
    {
        mResult2D->draw();
    }

    if (_11 || (!mResultStart && !RaceMgr::getManager()->isReplayMode()))
    {
        mPause2D->draw();
    }

    if (_28)
    {
        switch (mPauseNextChoice)
        {
        case 1:
        case 7:
            WipeManager::getManager()->getMario()->draw((f32)(_28 + 1) / 40.0f,
                                                        JUTColor(0x00, 0x00, 0x00, 0xFF));
            break;
        default:
            _28 = 1;
        }
    }

    if (mWipeOutFrame > 0 && mWipeOutDuration > 0)
    {
        f32 f = (f32)mWipeOutFrame / (f32)mWipeOutDuration;
        if (f > 1.0f)
        {
            f = 1.0f;
        }

        WipeManager::getManager()->getWipeCurtain()->calc(
            J2DManager::getManager()->getStatus2Kart(0x0),
            f,
            JUTColor(0x00, 0x00, 0x00, 0xFF));

        WipeManager::getManager()->getWipeCurtain()->draw(
            J2DManager::getManager()->getStatus2Kart(0x0),
            f,
            JUTColor(0x00, 0x00, 0x00, 0xFF));
    }

    if (_c)
    {
        mPrintMemoryCard->draw();
    }
}

void PauseManager::exec()
{
    if (!mExec)
        return;

    if (_14 >= 0)
    {
        _14 += 1;
    }

    if (_c)
    {
        mPrintMemoryCard->calc();

        switch (mPrintMemoryCard->get_14Thing())
        {
        case 0:
            _c = 0;
            if (_d)
            {
                goto labelA;
            }
            goto labelB;
        case 1:
            _c = 0;
            mPrintMemoryCard->init(PrintMemoryCard::mcMsg40);
            mPrintMemoryCard->setStuff(0, 0, 0);
            break;
        }
        return;
    }

    if (mWipeOutFrame > 0 && mWipeOutDuration > 0)
    {
        if (mWipeOutFrame == mWipeOutDuration)
        {
            JUTFader *fader = System::mspDisplay->getFader();
            if (fader)
            {
                fader->startFadeOut(1);
            }
        }
        mWipeOutFrame += 1;
    }

    int var_28 = _28;
    if (var_28)
    {
        if (var_28 < 40)
        {
            _28 = var_28 + 1;
            return;
        }

        mPauseChoice = mPauseNextChoice;
        mPausePad = 0;

        J2DManager::getManager()->startPreRace();

        _28 = 0;
        Result2D::setDrawFlag(_38);
        return;
    }

    int var_34 = _34;
    if (var_34)
    {
        if (var_34 < 11)
        {
            _34 = var_34 + 1;
        }
        else
        {
            mPauseChoice = mPauseNextChoice;
            mPausePad = 0;
            _34 = 0;
            Result2D::setDrawFlag(_38);

            switch (mPauseNextChoice)
            {
            case 1:
            case 7:
                mPauseChoice = 13;
                _28 = 1;
                GameAudio::Main::getAudio()->fadeOutAll(40);
                break;
            default:
            {
                switch (mPauseNextChoice)
                {
                case 13:
                    if (!_38)
                    {
                        mIsPause = 0;
                    }
                    break;
                default:
                    _34 = 1;
                    break;
                }
                break;
            }
            }
        }

        mResult2D->calc(gSequenceInfo.getDecidePad());
        return;
    }

    if (_14 < 120)
        return;

    if (!tstPause() && !RaceMgr::getManager()->isReplayMode())
    {
        if (_11)
        {
            mPausePad = &gGamePad1P;
            mPauseSelector = 0;
            mPauseChoice = 13;
        }
        else
        {
            KartGamePad *kartgamePad = gpaGamePad[0];
            if (kartgamePad && kartgamePad->testTrigger(JUTGamePad::START))
            {
                mPausePad = kartgamePad;
                mPauseSelector = 0;
                mPauseChoice = 13;
            }
        }
    }

    if (!mNetFlag &&
        ((mResultStart && Goal2D::mDrawEndFlag) || RaceMgr::getManager()->isReplayMode()))
    {
        mResult2D->calc(gSequenceInfo.getDecidePad());

        if (RaceMgr::getManager()->isReplayMode() &&
            RaceApp::ptr()->getPadRecorder()->getFrame() >= MAX_INPUT_FRAME_COUNT)
        {
            mIsPause = 1;
            mPause2D->init();
            Pause2D::setPauseStatus((Pause2D::PauseStatus)1);
            GameAudio::Main::getAudio()->fadeOutAll(40);
            mPauseNextChoice = 7;
            _34 = 1;
            _38 = 0;
        }
        else
        {
            KartGamePad *decidePad = gSequenceInfo.getDecidePad();
            if (decidePad->testTrigger(JUTGamePad::A) && Result2D::mResultState == 2 &&
                mResult2D->getAnmEnd())
            {
                if (Result2D::getDrawFlag())
                {
                    switch (Result2D::getResultSelector())
                    {
                    case 1:
                    case 7:
                        GameAudio::Main::getAudio()->startSystemSe(0x20002);
                        GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);
                        GameAudio::Main::getAudio()->fadeOutAll(40);

                        mPauseNextChoice = Result2D::getResultSelector();
                        _34 = 1;
                        _38 = 0;
                        mResult2D->_1980 = 1;
                        break;
                    case 8:
                        mPauseNextChoice = Result2D::getResultSelector();
                        if (RaceMgr::getManager()->isRaceModeGp())
                        {
                            _c = 1;
                            _d = 0;
                            mPrintMemoryCard->init(PrintMemoryCard::mcMsg39);
                            mPrintMemoryCard->calc();
                            break;
                        }
                        GameAudio::Main::getAudio()->startSystemSe(0x20023);
                    labelB:
                        _34 = 1;
                        _38 = 0;
                        mResult2D->_1980 = 1;
                        break;
                    default:
                        GameAudio::Main::getAudio()->startSystemSe(0x20002);
                        GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);
                        mPauseNextChoice = Result2D::getResultSelector();
                        _34 = 1;
                        _38 = 0;
                        mResult2D->_1980 = 1;
                    }
                }
            }
            else
            {
                if (gSequenceInfo.getDecidePad()->testTrigger(JUTGamePad::START))
                {
                    if (RaceMgr::getManager()->isReplayMode())
                    {
                        if (!Result2D::getDrawFlag())
                        {

                            mIsPause = 1;
                            mPause2D->init();
                            Pause2D::setPauseStatus((Pause2D::PauseStatus)1);
                            GameAudio::Main::getAudio()->startSystemSe(0x20008);
                            GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)1);
                            mPauseNextChoice = 13;
                            _34 = 1;
                            _38 = 1;
                            Result2D::setDrawFlag(true);
                            mResult2D->init();
                        }
                        else if (mResult2D->getAnmEnd())
                        {
                            GameAudio::Main::getAudio()->startSystemSe(0x20009);
                            GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)0);
                            mPauseNextChoice = 13;
                            _34 = 1;
                            _38 = 0;
                            mResult2D->_1980 = 1;
                        }
                    }
                }
                else
                {
                    if (gSequenceInfo.getDecidePad()->testTrigger(JUTGamePad::A))
                    {
                        if (RaceMgr::getManager()->isReplayMode() && !Result2D::getDrawFlag())
                        {
                            mIsPause = 1;
                            mPause2D->init();
                            Pause2D::setPauseStatus((Pause2D::PauseStatus)1);
                            GameAudio::Main::getAudio()->startSystemSe(0x20008);
                            GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)1);
                            mPauseNextChoice = 13;
                            _34 = 1;
                            _38 = 1;
                            Result2D::setDrawFlag(true);
                            mResult2D->init();
                        }
                    }
                    else if (gSequenceInfo.getDecidePad()->testTrigger(JUTGamePad::B) &&
                             RaceMgr::getManager()->isReplayMode() && Result2D::getDrawFlag())
                    {
                        GameAudio::Main::getAudio()->startSystemSe(0x20009);
                        GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)0);
                        mPauseNextChoice = 13;
                        _34 = 1;
                        _38 = 0;
                        mResult2D->_1980 = 1;
                    }
                }
            }
        }
    }

    if (!mPausePad)
        return;
    if (mNetFlag)
        return;
    if (mPauseEnd && _11 == 0x0)
        return;
    if (RaceMgr::getManager()->isReplayMode())
        return;

    mPause2D->calc(mPausePad);
    if (tstPause())
    {
        if (Pause2D::getPauseStatus() == (Pause2D::PauseStatus)3)
        {
            mPauseChoice = 13;
            if (mPausePad->testTrigger(JUTGamePad::A))
            {
                switch (Pause2D::getPauseSelector())
                {
                case 1:
                    GameAudio::Main::getAudio()->startSystemSe(0x20023);
                    _28 = 1;
                    GameAudio::Main::getAudio()->fadeOutAll(40);
                    GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);
                    mPauseNextChoice = Pause2D::getPauseSelector();
                    return;
                case 8:
                    if (RaceMgr::getManager()->isRaceModeGp())
                    {
                        _c = 1;
                        _d = 1;
                        mPrintMemoryCard->init(PrintMemoryCard::mcMsg39);
                        mPrintMemoryCard->calc();
                        return;
                    }
                    GameAudio::Main::getAudio()->startSystemSe(0x20023);
                labelA:
                    button = 0;
                    Pause2D::setPauseStatus((Pause2D::PauseStatus)4);
                    return;
                case 0:
                    GameAudio::Main::getAudio()->startSystemSe(0x20009);
                    button = 0;
                    Pause2D::setPauseStatus((Pause2D::PauseStatus)4);
                    return;
                default:
                    GameAudio::Main::getAudio()->startSystemSe(0x20023);
                    button = 0;
                    Pause2D::setPauseStatus((Pause2D::PauseStatus)4);
                }
            }
            else
            {
                if (!mPausePad->testTrigger(JUTGamePad::START))
                    return;

                button = 1;
                Pause2D::setPauseStatus((Pause2D::PauseStatus)4);
                GameAudio::Main::getAudio()->startSystemSe(0x20009);
                GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)0);
                return;
            }
        }
        else
        {
            if (Pause2D::getPauseStatus() != (Pause2D::PauseStatus)0)
                return;

            mPauseChoice = 13;
            switch (button)
            {
            case 0:
                switch (Pause2D::getPauseSelector())
                {
                case 0:
                    mIsPause = 0;
                    GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)0);
                default:
                    mPauseChoice = Pause2D::getPauseSelector();
                    mPausePad = nullptr;
                    GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);
                    break;
                }
                break;
            case 1:
                mIsPause = 0;
                mPauseSelector = 0;
                mPauseChoice = 0;
                mPausePad = nullptr;
                break;
            default:
#line 553
                JUT_ASSERT(0);
            }

            return;
        }
    }
    else
    {
        if (!mPausePad->testTrigger(JUTGamePad::START))
            return;

        mIsPause = 1;
        mPause2D->init();
        Pause2D::setPauseStatus((Pause2D::PauseStatus)1);
        GameAudio::Main::getAudio()->startSystemSe(0x20008);
        GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)1);
        return;
    }
}

bool Result2D::getDrawFlag()
{
    return mDrawFlag;
}

void Result2D::setDrawFlag(bool drawFlag)
{
    mDrawFlag = drawFlag;
}

bool PauseManager::tstPause()
{
    return mIsPause;
}

void PauseManager::wipeIn()
{
    // Unknown implementation
}

void PauseManager::wipeOut(int duration)
{
    mWipeOutFrame = 1;
    mWipeOutDuration = duration;

    WipeManager::getManager()->getWipeCurtain()->reset();
}

int PauseManager::getPauseChoice()
{
    if (mPauseChoice == 8)
    {
        GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);
    }

    if (mThis->_f)
    {
        mPauseChoice = 5;
        GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);
    }

    if (mThis->mGameOver)
    {
        mPauseChoice = 9;
        GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);
    }

    switch (mPauseChoice)
    {
    case 2:
    case 3:
    case 4:
    case 6:
        gSequenceInfo._28 = 1;
        break;
    }

    switch (mPauseChoice)
    {
    case 4:
        gSequenceInfo._2c = 0;
        break;
    case 6:
        gSequenceInfo._2c = 1;
        break;
    }

    if (mPauseChoice == 5)
    {
        GameAudio::Main::getAudio()->setPause((GameAudio::Main::PAUSE_STATUS)2);

        if (gSequenceInfo.isEndGP())
        {
            if (gSequenceInfo.isGameClr())
            {
                mPauseChoice = 11;
            }
            else
            {
                mPauseChoice = 12;
            }
        }
    }

    return mPauseChoice;
}

void PauseManager::getCharRankPos(int)
{
    // Unknown implementation
}

void PauseManager::setTA()
{
    mResult2D->setTA();
}

#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
