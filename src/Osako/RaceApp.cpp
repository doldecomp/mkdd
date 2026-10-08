#include "Inagaki/GameAudioMain.h"

#include "Kameda/PauseManager.h"
#include "Kameda/SequenceInfo.h"

#include "Kaneshige/SysDebug.h"

#include "Osako/AppMgr.h"
#include "Osako/AwardApp.h"
#include "Osako/CardAgent.h"

#include "Osako/NetGameMgr.h"
#include "Osako/NetGateApp.h"
#include "Osako/ForceEffectMgr.h"
#include "Osako/RaceApp.h"
#include "Osako/SequenceApp.h"
#include "Osako/shadowMgr.h"
#include "Osako/system.h"

#include "Sato/RivalSpeedCtrl.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

RaceApp *RaceApp::mspRaceApp;

volatile u16 RaceApp::msStaffGhostFlag;

#include "mathHelper.h"

RaceApp *RaceApp::create()
{
    if (mspRaceApp == nullptr)
        mspRaceApp = NEW_APP RaceApp();
    return mspRaceApp;
}

void RaceApp::call() { AppMgr::setNextApp(AppMgr::mcRace); }

RaceApp::RaceApp() : GameApp(0x600000, "Race", nullptr)
{
    int i;
    switch (NetGameMgr::ptr()->getState())
    {
    case 0:
    case 6:
        mState = 2;
        mDrawFlags = 1;
        _20 = 0;
        break;
    case 15:
        mState = 0;
        mDrawFlags = 0;
        _20 = 1;
        break;
    default:
#line 94
        JUT_PANIC("illegal net state!")
    }

    setAudioRaceMode();
    if (gRaceInfo.isDemoConsole(0))
        GameAudio::Parameters::setPlayerMode(4);
    else
        GameAudio::Parameters::setPlayerMode(gRaceInfo.getStatusNumber() - 1);
    // GameAudio::Parameters::setPlayerMode(gRaceInfo.isDemoConsole(0) ? 4 : (gRaceInfo.getStatusNumber() - 1));

    for (u8 i = 0; i < gRaceInfo.getKartNumber(); i++)
    {
        KartInfo *kartInfo = gRaceInfo.getKartInfo(i);
        GameAudio::Parameters::setChrType(i, kartInfo->getKartID());
        GameAudio::Parameters::setKartType(i, kartInfo->getKartWeight());
    }

    GameAudio::Parameters::setRaceCourse(ResMgr::msCourseID);
    GameAudio::Parameters::setNetworkMode(NetGameMgr::ptr()->getNetworkCubes(), NetGameMgr::ptr()->getNetworkNum());
    SYSDBG_SetHeapGroup("Shadow Mgr", nullptr);
    if (gRaceInfo.getStatusNumber() > 2)
        ShadowManager::ptr()->setDepthMode(0);
    else
        ShadowManager::ptr()->setDepthMode(1);
    // ShadowManager::ptr()->setDepthMode(gRaceInfo.getStatusNumber() > 2 ? 0 : 1);

    if (!gRaceInfo.isWaitDemo())
    {
        ForceEffectMgr::create();
        ForceEffectMgr::ptr()->start();
    }

    mRecorder = nullptr;

    for (int i = 0; i < 4; i++)
    {
        mPadRecord[i] = nullptr;
    }

    switch (gRaceInfo.getRaceMode())
    {
    case TIME_ATTACK:
    case GRAND_PRIX:
    case VERSUS_RACE:
    case BALLOON_BATTLE:
    case BOMB_BATTLE:
    case ESCAPE_BATTLE:
    {
        if (!_20)
        {
            // No idea why this is defined outside of the loop but sure
            u8 charID1, charID2, kartID, courseID;
            int port;
            SYSDBG_SetHeapGroup("PadRecord", getHeap());
            mRecorder = new PadRecorder();

            for (int i = 0; i < gRaceInfo.getStatusNumber() ; i++)
            {
                KartInfo *kartInfo = gRaceInfo.getKartInfo(i);
                if (!kartInfo->getPad(0))
                    continue;

                charID1 = kartInfo->getDriverCharID(0);
                charID2 = kartInfo->getDriverCharID(1);
                kartID = kartInfo->getKartID();
                courseID = ResMgr::msCourseID;
                port = kartInfo->getPad(0)->getPadPort();

                if (port >= 0 && port < 4)
                {
                    mPadRecord[port] = new KartPadRecord(charID1, charID2, kartID, courseID);
                    mRecorder->setRecord(port, mPadRecord[port]);
                }

                if (!kartInfo->getPad(1))
                    continue;

                port = kartInfo->getPad(1)->getPadPort();
                if (port >= 0 && port < 4)
                {
                    mPadRecord[port] = new KartPadRecord(charID1, charID2, kartID, courseID);
                    mRecorder->setRecord(port, mPadRecord[port]);
                }
            }
            mRecorder->setFrame(0);
            mRecorder->standby();
            PadMgr::mspRecorder = mRecorder;

            _18 = 1;
            _1C = 0;
            _50 = 0;
            break;
        }
    }

    default:
        _18 = 0;
        break;
    }

    mGhostRecord = nullptr;

    if (gRaceInfo.getRaceMode() == TIME_ATTACK)
    {
        gRaceInfo.setRandomSeed(0x12D5ECB); // this value was used in more places i believe?

        for (int i = 0; i < 3; i++)
        {
            _44[i] = new KartGamePad(JUTGamePad::Port_unknown, KartGamePad::PORT_INV, KartGamePad::RECORD, KartGamePad::STATE_1);
        }

        if (!(gSystemRecord.mOptions & 8))
        {
            mGhostRecord = new KartPadRecord(gGhostFile.mFileData.mPadRecord);

            KartInfo *kartInfo = gRaceInfo.getKartInfo(0);
            EKartID kartID = (EKartID)kartInfo->getKartID(); // cast is needed here as well to generate an extra temp
            ECharID charID1 = kartInfo->getDriverCharID(0);
            ECharID charID2 = kartInfo->getDriverCharID(1);

            gRaceInfo.setKart(1, kartID, charID1, _44[0], charID2, nullptr);
            gRaceInfo.getKartInfo(1)->setGhostKind(KIND_NORMAL_GHOST);
            mRecorder->setGhostPad(0, _44[0]);

            if (gGhostFile.available(ResMgr::msCourseID))
            {
                KartPadRecord *rec = &gGhostFile.mFileData.mPadRecord;
                // These casts are actually needed to prevent instruction swaps
                // Changing the return type of the function back to int or u8 actually makes it worse
                // so maybe this is actually what was done?
                charID1 = (ECharID)rec->getCharID(0);
                charID2 = (ECharID)rec->getCharID(1);
                kartID = (EKartID)rec->getKartID();

                gRaceInfo.setKart(2, kartID, charID1, _44[1], charID2, nullptr);
                _44[1]->setPadState(KartGamePad::STATE_0);
                mRecorder->setGhostRecord(1, mGhostRecord);
                mRecorder->setGhostPad(1, _44[1]);
            }
            else
            {
                gRaceInfo.setKart(2, kartID, charID1, _44[1], charID2, nullptr);
            }

            gRaceInfo.getKartInfo(2)->setGhostKind(KIND_NORMAL_GHOST);
            mStaffGhostRecord = (KartPadRecord *)ResMgr::getPtr(ResMgr::mcCourseDataGhost);

            if (mStaffGhostRecord)
            {
                gRaceInfo.setKart(3, mStaffGhostRecord->getKartID(), mStaffGhostRecord->getCharID(0), _44[2], mStaffGhostRecord->getCharID(1), nullptr);

                if (msStaffGhostFlag & 1 << SystemRecord::convCourseID(ResMgr::msCourseID))
                {
                    _44[2]->setPadState(KartGamePad::STATE_0);
                }
                else
                {
                }
                mRecorder->setGhostRecord(2, mStaffGhostRecord);
                mRecorder->setGhostPad(2, _44[2]);
            }
            else
            {
                gRaceInfo.setKart(3, kartID, charID1, _44[2], charID2, nullptr);
            }
            gRaceInfo.getKartInfo(3)->setGhostKind(KIND_STAFF_GHOST);
        }
        else
        {
            gRaceInfo.setKart(1, RED_FIRE, MARIO, _44[0], LUIGI, nullptr);
            gRaceInfo.setKart(2, RED_FIRE, MARIO, _44[1], LUIGI, nullptr);
            gRaceInfo.setKart(3, RED_FIRE, MARIO, _44[2], LUIGI, nullptr);
        }
    }
    DriverData::create();
    SYSDBG_SetHeapGroup("RACE MGR", nullptr);
    JUTColor clearColor(0, 0, 0, ShadowManager::ptr()->getDstAlpha());
    System::getDisplay()->setClearColor(clearColor);

    mRaceMgr = new RaceMgr(&gRaceInfo);
}

RaceApp::~RaceApp()
{
    ForceEffectMgr::destroy();
    delete mRecorder;
    PadMgr::destroy();
    mspRaceApp = nullptr;
    delete mRaceMgr;
}

void RaceApp::draw()
{
    switch (mDrawFlags)
    {
    case 1:
        if (System::getDisplay()->isWaiting())
            mRaceMgr->drawRace();
        break;
    }
}

void RaceApp::calc()
{
    switch (mState)
    {
    case 0:
        NetGameMgr::ptr()->awake();
        mState = 1;
        break;
    case 1:
        if (NetGameMgr::ptr()->getState() == 0xd)
        {
            mState = 2;
            if (NetGameMgr::ptr()->get12ee())
            {
                System::getDisplay()->set_38(2);
            }
        }

        break;
    case 2:
#ifdef DEBUG
        if (gGamePad1P.testButton(0x20) && gGamePad1P.testButton(4))
        {
            RivalSpeedCtrl::sConfusedFightModeFlg = 1;
        }
#endif
        mRaceMgr->calcRace();
        ctrlRecorder();
        mRaceMgr->updateRace();
        ctrlRace();
        if (_20)
        {
            for (u8 i = 0; i < gRaceInfo.getPlayerKartNumber(); i++)
            {
                if (gpaKartPad[i]->testButton(0x200) && gpaKartPad[i]->testButton(0x400) && gpaKartPad[i]->testButton(0x800) && gpaKartPad[i]->testButton(0x10))
                {
#ifndef DEBUG
                    System::haltRumble();
#endif
                    GetGameAudioMain()->stopStreamBgm(0);
                    mState = 3;
                    break;
                }
            }
        }
        mDrawFlags = 1;
        break;
    case 3:
        if (_20)
        {
            NetGameMgr::ptr()->sleep();
            mState = 4;
        }
        else
        {
            if (gSystemFile._6024)
            {
                CardAgent::ask(CardAgent::mcCommand3, 0);
                gSystemFile._6024 = 0;
                System::getDisplay()->getFader()->setStatus(JUTFader::Status_In, 0);
                mDrawFlags = 0;
                mState = 5;
            }
            else
            {
                mState = 6;
            }
        }
        break;
    case 4:
        if (NetGameMgr::ptr()->getState() == 0xf)
        {
            mState = 6;
        }
        break;
    case 5:
        if (CardAgent::msState == 0)
        {
            CardAgent::mspPrintMemoryCard->setStuff(false, false, false);
            SYSTEM_GetFader()->setStatus(JUTFader::Status_Out, 0);
            mState = 6;
        }
        break;
    case 6:
#ifdef DEBUG
        System::haltRumble();
#endif
        AppMgr::deleteCurrentApp();
        AwardApp::destroy();
        if (_20)
        {
            if (NetGameMgr::ptr()->get12ee())
            {
                NetGameMgr::ptr()->set12ee(0);
                System::getDisplay()->set_38(1);
            }
            NetGateApp::call(true);
        }
        else
        {

            SequenceApp::call(mNextScene);
        }
    }
    return;
}

void RaceApp::ctrlRace()
{
    switch (mRaceMgr->getRacePhase())
    {
    case PHASE_3:
        mRaceMgr->resetRace();
        break;
    case PHASE_4:
    {
        if (mRecorder)
        {
            if (gRaceInfo.getRaceMode() == TIME_ATTACK && !(gSystemRecord.mOptions & 8))
            {
                GhostFile *ghostFile = &gGhostFile;
                switch (ghostFile->mGhostType)
                {
                case 3:
                    *mGhostRecord = ghostFile->mFileData.mPadRecord;
                    mRecorder->setGhostRecord(0, mGhostRecord);
                    _44[0]->setPadState(KartGamePad::STATE_0);
                    _44[1]->setPadState(KartGamePad::STATE_1);
                    break;
                }
                if (msStaffGhostFlag & 1 << SystemRecord::convCourseID(ResMgr::msCourseID))
                {
                    _44[2]->setPadState(KartGamePad::STATE_0);
                }
            }
            for (int i = 0; i < 4; i++)
            {
                mRecorder->setRecord(i, mPadRecord[i]);
            }
            mRecorder->stop();
            mRecorder->setFrame(0);
            mRecorder->standby();
            mRecorder->initRecord();
            _50 = 0;
            _18 = 1;
            _1C = 0;
        }
        setAudioRaceMode();
        mRaceMgr->resetRace();
        ForceEffectMgr::ptr()->start();

        break;
    }
    case PHASE_5:
    {
        for (int i = 0; i < 4; i++)
        {
            mRecorder->setRecord((u8)i, mPadRecord[i]);
        }
        mRecorder->stop();
        mRecorder->setFrame(0);
        mRecorder->standby();
        _18 = 2;
        _1C = 0;
        GameAudio::Parameters::setRaceMode(2);
        mRaceMgr->resetRace();
        ForceEffectMgr::ptr()->end();
        break;
    }
    case 6:
        mNextScene = Scene::SCENE_TITLE;
        mState = 3;
        break;
    case 7:
    case 0xf:
        mNextScene = Scene::SCENE_MENU;
        mState = 3;
        break;
    case 0xe:
        mNextScene = Scene::SCENE_MAP_SELECT;
        mState = 3;
        break;
    case 8:
        mNextScene = Scene::SCENE_COURSE;
        mState = 3;
        break;
    case 10:
        mState = 3;
        break;
    case 0xb:
        mNextScene = Scene::SCENE_GP_NEXT;
        mState = 3;
        break;
    case 0xd:
        gSequenceInfo.setGhostFlag(2);
        mNextScene = Scene::SCENE_GHOST_LOAD_SAVE;
        mState = 3;
        break;
    }
}

void RaceApp::ctrlRecorder()
{

    switch (_1C)
    {
    case 0:
    {
        if (mRaceMgr->isJugemCountStart())
            _1C = 1;
        break;
    }

    case 1:
        _1C = 2;
        break;
    }

    switch (_18)
    {
    case 0:
        break;
    case 1:
    {
        switch (_1C)
        {
        case 1:
            mRecorder->rec();
            break;
        case 2:
        {
            KartChecker *kartChecker;
            if (!PauseManager::tstPause())
            {
                mRecorder->nextFrame();
                for (int i = 0; i, i < gRaceInfo.getStatusNumber(); i++)
                {
                    if (!(_50 & 1 << i) && mRaceMgr->isKartGoal(i))
                    {
                        const RaceTime &rTime = mRaceMgr->getKartChecker(i)->getTotalTime();
                        KartInfo *kartInfo = mRaceMgr->getKartInfo(i);
                        KartGamePad *pad = kartInfo->getPad(0);
                        int port;
                        if (pad)
                        {
                            port = kartInfo->getPad(0)->getPadPort();
                            mRecorder->finalizeRecord(port, rTime);
                            mRecorder->setRecord(port, nullptr);
                        }
                        pad = kartInfo->getPad(1);
                        if (pad)
                        {
                            port = pad->getPadPort();
                            mRecorder->finalizeRecord(port, rTime);
                            mRecorder->setRecord(port, nullptr);
                        }
                        _50 = _50 | (1 << i);
                    }
                }

                int temp = (1 << gRaceInfo.getStatusNumber()) - 1;
                if (temp == (temp & _50))
                {
                    mRecorder->stop();
                    _18 = 0;

                    if (gRaceInfo.getRaceMode() == TIME_ATTACK && !(gSystemRecord.mOptions & 8) && mPadRecord[0])
                    {
                        kartChecker = mRaceMgr->getKartChecker(0);
                        for (int i = 0; i < mRaceMgr->getTotalLapNumber() - 1; i++)
                        {
                            mPadRecord[0]->setLapSlit(i, kartChecker->getLapSplit(i));
                        }
                        mPadRecord[0]->setName(gSystemRecord.mDefaultName);
                        gGhostFile.updateKartPadRecord(*mPadRecord[0]);
                        int courseID = SystemRecord::convCourseID(ResMgr::msCourseID);
                        if (!(msStaffGhostFlag & 1 << courseID) && mStaffGhostRecord)
                        {
                            RaceTime time = mStaffGhostRecord->getRaceTime();
                            time.set(time.get() + 3000);
                            time.round();
                            if (!time.isLittle(kartChecker->getTotalTime()))
                            {
                                msStaffGhostFlag = msStaffGhostFlag | (1 << courseID);
                            }
                        }
                    }
                }
            }
            break;
        }

        default:
            break;
        }
        break;
    }
    case 2:
    {
        switch (_1C)
        {
        case 1:
            mRecorder->play();
            break;

        case 2:
            if (!PauseManager::tstPause())
                mRecorder->nextFrame();
            break;
        }
    }
    }
}

void RaceApp::setAudioRaceMode()
{
    switch (gRaceInfo.getRaceMode())
    {
    case GRAND_PRIX:
        GameAudio::Parameters::setRaceMode(1);
        break;
    case TIME_ATTACK:
        GameAudio::Parameters::setRaceMode(0);
        break;
    case AWARD_DEMO:
        GameAudio::Parameters::setRaceMode(3);
        break;
    case VERSUS_RACE:
        GameAudio::Parameters::setRaceMode(4);
        break;
    case BALLOON_BATTLE:
    case ROBBERY_BATTLE:
    case BOMB_BATTLE:
    case ESCAPE_BATTLE:
        GameAudio::Parameters::setRaceMode(5);
        break;
    }
}
