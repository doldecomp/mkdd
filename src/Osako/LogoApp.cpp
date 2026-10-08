#include <JSystem/JUtility/JUTDbPrint.h>
#include <JSystem/JAudio/JASFakeMatch2.h>

#include "Inagaki/GameAudioMain.h"
#include "Kaneshige/Movie/MovieApp.h"
#include "Kaneshige/Movie/MoviePlayer.h"
#include "Osako/AppMgr.h"
#include "Osako/CardAgent.h"
#include "Osako/kartPad.h"
#include "Osako/LogoApp.h"
#include "Osako/ResMgr.h"
#include "Osako/SequenceApp.h"
#include "Osako/system.h"

// This file is a mess, it seems like it got changed after the PAL Release since the line numbers are different for whatever reason

extern const char _pBuildtimeDat[];

LogoApp *LogoApp::mspLogoApp;

LogoApp *LogoApp::create()
{
    if (mspLogoApp == nullptr)
        mspLogoApp = NEW_APP LogoApp();
    return mspLogoApp;
}

void LogoApp::call() { AppMgr::setNextApp(AppMgr::mcLogo); }

LogoApp::LogoApp() : GameApp(0x4800, "Logo", nullptr)
{
    _1c = 0;
    _20 = 0;
    _28 = 0;

    mPrintApp = new PRINTAPP_TYPE(mHeap);
    mPrintApp->init(PRINTAPP_TYPE::mcShow_Logo);

#ifndef REGION_EU
    mShowDialog = 0;
#endif
    mState = mcInitSystem;
    mLoadingState = 0;
#ifndef REGION_EU
    mFadeTimer = 0;
#endif
    mIsLoadingMovie = false;
    _2c = 0.0f;
    _30 = 0.0f;
}

LogoApp::~LogoApp() { mspLogoApp = nullptr; }

void LogoApp::draw()
{
#ifdef DEBUG
    JUTReport(30, 50, _pBuildtimeDat);
#endif
    mPrintApp->draw();
}

void LogoApp::calc()
{
    mPrintApp->calc(-1);
    switch (mState)
    {
#ifndef REGION_EU
    case mcInitSystem:
        if (++mFadeTimer > 10)
            mState = mcStartSystem;
        break;
#endif
    case mcStartSystem:
        if (!JUTGamePad::isPadOk())
        {
            break;
        }
        if (_28 != true)
        {
            _1c = Clock::getTimeDiff(_20);
            _28 = true;
        }
#ifndef REGION_EU
        if (VIGetDTVStatus() != 0)
        {
            if (OSGetProgressiveMode() == TRUE || gGamePad1P.testButton(PAD_BUTTON_B))
            {
                mShowDialog = 1;
            }
            else
            {
                mShowDialog = 0;
            }
        }
        else
        {
            OSSetProgressiveMode(0);
            mShowDialog = 0;
        }
#else
        mShowDialog = 1;
#endif
        CardAgent::ask(CardAgent::mcCommand1, 0);
        mState = mcLogoInitMemoryCard;
    case mcLogoInitMemoryCard:
        if (CardAgent::getState() != 0)
        {
            break;
        }
        CardAgent::getPrintMemoryCard()->setStuff(false, false, false);
        mState = mcLogoCheckFrameTime;
    case mcLogoCheckFrameTime:
    {
        _2c = getFrameTime();
        if (!GetGameAudioMain()->isWaveLoaded(GameAudioMain::STREAM0))
            break;
    }
    case mclogoStartAudio:
        SYSTEM_StartFadeIn(30);
        GetGameAudioMain()->startSystemSe(0x30334);
        mFadeTimer = 0;
        mState = mcLogoFadeIn;
        break;
    case mcLogoFadeIn:
        if (++mFadeTimer > 90)
        {
            SYSTEM_StartFadeOut(30);
            mState = mcLogoFadeOut;
        }

        if (mFadeTimer == 10)
        {
            LGWheels *wheels = JUTGamePad::getLGWheels();
            for (int i = 0; i < 4; i++)
            {
                wheels->PlayAutoCalibAndSpringForce(i);
            }
        }

        break;
    case mcLogoFadeOut:
        if (SYSTEM_IsFadingOut())
        {
            mPrintApp->init(PRINTAPP_TYPE::mcShow_Set);
            SYSTEM_StartFadeIn(30);
            mState = mcDolbyFadeIn;
            mFadeTimer = 0;
        }
        break;
    case mcDolbyFadeIn:
        if (++mFadeTimer > 90)
        {
            SYSTEM_StartFadeOut(30);

            OpeningState nextState = mcDialogApplyOption;
            if (mShowDialog == 1)
            {
                nextState = mcDialogFadeIn;
            }
            mState = nextState;
            LGWheels *wheels = JUTGamePad::getLGWheels();
            for (int i = 0; i < 4; i++)
            {
                if (wheels->IsConnected(i))
                    wheels->PlaySpringForce(i, 0, 40, 120);
            }
        }
        break;
    case mcDialogFadeIn:
        if (SYSTEM_IsFadingOut())
        {
            mFadeTimer = 0;
            SYSTEM_StartFadeIn(30);
            mState = mcDialogPickOption;
            mPrintApp->init(PRINTAPP_TYPE::mcShow_Dialog);
#ifdef REGION_EU
            if (OSGetEuRgb60Mode() == 1)
            {
                mPrintApp->setOption(0);
            }
            else
            {
                mPrintApp->setOption(1);
            }
#endif
        }
        break;
    case mcDialogPickOption:
        mFadeTimer++;
#ifndef REGION_EU
        if (mFadeTimer > 630 || mFadeTimer > 30 && mPrintApp->getState() != 2)
#else
        if (mFadeTimer > 30 && mPrintApp->getState() != 2)
#endif
        {
            mState = mcDialogApplyOption;
            SYSTEM_StartFadeOut(30);
        }
        break;
    case mcDialogApplyOption:
        if (SYSTEM_IsFadingOut())
        {
            mFadeTimer = 0;

            // mShowDialog never changes to 3, debugging purpose/arg?
            if (mPrintApp->getOption() == 0 || mShowDialog == 3)
            {
                SYSTEM_ChangeVideoMode();
                mState = mcRenderModeChangeFadeIn;
                break;
            }
            if (mPrintApp->getOption() == 1)
            {
                if (OSGetVideoMode() == 1)
                {
#ifdef REGION_EU
                    OSSetEuRgb60Mode(FALSE);
#endif
                    mState = mcDialogFadeOut;
                    SYSTEM_StartFadeIn(30);
                    mPrintApp->init(PRINTAPP_TYPE::mcSet_Option2);
#ifndef REGION_EU
                    OSSetProgressiveMode(FALSE);
#endif
                    break;
                }
                mState = mcLoadOpening;
                break;
            }

            mState = mcLoadOpening;
        }
        break;
    case mcRenderModeChangeFadeIn:
        if (++mFadeTimer > 100)
        {
            mFadeTimer = 0;
            if (OSGetVideoMode() == 0)
            {
                OSSetVideoMode(1);
                mState = mcDialogFadeOut;
                SYSTEM_StartFadeIn(30);
                mPrintApp->init(PRINTAPP_TYPE::mcSet_Option1);
            }
            else
            {
                mState = mcLoadOpening;
            }
        }
        break;
    case mcDialogFadeOut:
        if (++mFadeTimer > 120)
        {
            mState = mcLoadOpening;
            SYSTEM_StartFadeOut(30);
        }
        break;
    case mcLoadOpening:
        if (SYSTEM_IsFadingOut())
        {
            if (ResMgr::isFinishLoadingArc(ResMgr::mcArcOpening))
                MovieApp::call();
        }
        break;
    }

    controlLoading();
}

void LogoApp::createMoviePlayer(void *param)
{
    MoviePlayer::create((JKRHeap *)param);
    mspLogoApp->mIsLoadingMovie = false;
}

void LogoApp::reset() {}

#ifndef REGION_EU
#define linenum 466
#else
#define linenum 459
#endif

void LogoApp::restart()
{
    switch (mState)
    {
    case mcStartSystem:
    case mcLogoInitMemoryCard:
    case mcLogoCheckFrameTime:
    case mclogoStartAudio:
    case mcRenderModeChangeFadeIn:
    case mcDialogFadeOut:
    case mcLoadOpening:
        break;
    case mcLogoFadeIn:
    case mcLogoFadeOut:
    case mcDolbyFadeIn:
    case mcDialogFadeIn:
    case mcDialogPickOption:
    case mcDialogApplyOption:
        mState = mcStartSystem;
#ifdef REGION_EU
        System::changePal50();
#endif
        mPrintApp->init(PRINTAPP_TYPE::mcShow_Logo);
        break;

    default:
#line linenum
        JUT_PANIC("illegal state on restart");
        break;
    }
}

void LogoApp::controlLoading()
{
    switch (mLoadingState)
    {
    case 0:
        if (!GetGameAudioMain()->isWaveLoaded(GameAudioMain::STREAM1))
            break;

        _30 = getFrameTime();
        SEQUENCEAPP_isReady(Scene::SCENE_TITLE);
        SEQUENCEAPP_isReady(Scene::SCENE_MENU);
        mLoadingState = 1;
        break;
    case 1:
        if (mState > mcDialogApplyOption)
        {
            mIsLoadingMovie = true;
            SYSTEM_RequestTask(createMoviePlayer, MOVIE_GetHeap(), nullptr);
            MOVIE_loadOpeningData();
            ResMgr::loadKeepData();
            mLoadingState = 2;
        }
        break;
    }
}
