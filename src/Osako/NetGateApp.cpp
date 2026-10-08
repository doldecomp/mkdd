#include "Osako/NetGateApp.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/JUtility/JUTDbg.h"
#include "Kameda/PrintMemoryCard.h"
#include "Kameda/Scene.h"
#include "Osako/AppMgr.h"
#include "Osako/CardAgent.h"
#include "Osako/GameApp.h"
#include "Osako/LANEntry.h"
#include "Osako/LANSelectMode.h"
#include "Osako/LANTitle.h"
#include "Osako/LANPlayInfo.h"
#include "Osako/MenuTitleLine.h"
#include "Osako/NetGameMgr.h"
#include "Osako/RaceApp.h"
#include "Osako/ResMgr.h"
#include "Osako/SequenceApp.h"
#include "Osako/system.h"
#include "kartLocale.h"
#include "types.h"
#include "mathHelper.h"
#include "JSystem/JAudio/JASFakeMatch2.h"

NetGateApp *NetGateApp::mspNetGateApp;
bool NetGateApp::msSkip;

NetGateApp *NetGateApp::create() {
    if (mspNetGateApp == nullptr) {
        mspNetGateApp = NEW_APP NetGateApp();
    }
    return mspNetGateApp;
}

void NetGateApp::call(bool skip) {
    msSkip = skip;
    AppMgr::setNextApp(AppMgr::mcNetGate);
}

NetGateApp::NetGateApp() : GameApp(0x400000, "Net Gate", nullptr) {
    mState = 0;
    mMenuType = 0;
    mHasActiveTask = false;
    mpPrintMc = CardAgent::getPrintMemoryCard();
    mpPrintMc->init(PrintMemoryCard::mcMsg40);
}

NetGateApp::~NetGateApp() {
    mspNetGateApp = nullptr;
    delete MenuTitleLine::ptr();
}

void NetGateApp::loadTask(void *) {
    char buf[64];
    snprintf(buf, sizeof(buf), "/SceneData/%s/titleline.arc", KartLocale::getLanguageName());
    NetGateApp::ptr()->mpTitleArc = JKRMountArchive(buf, JKRArchive::MOUNT_MEM, NetGateApp::ptr()->mHeap, JKRArchive::MOUNT_DIRECTION_HEAD);
    
    snprintf(buf, sizeof(buf), "/SceneData/%s/lanentry.arc", KartLocale::getLanguageName());
    NetGateApp::ptr()->mpEntryArc = JKRMountArchive(buf, JKRArchive::MOUNT_MEM, NetGateApp::ptr()->mHeap, JKRArchive::MOUNT_DIRECTION_HEAD);
    
    snprintf(buf, sizeof(buf), "/SceneData/%s/LANPlay.arc", KartLocale::getLanguageName());
    NetGateApp::ptr()->mpSceneArc = JKRMountArchive(buf, JKRArchive::MOUNT_MEM, NetGateApp::ptr()->mHeap, JKRArchive::MOUNT_DIRECTION_HEAD);

#line 101
    NetGateApp::ptr()->mHasActiveTask = false;
    JUT_ASSERT(NetGateApp::ptr()->mpSceneArc);
}

void NetGateApp::draw() {
    switch (mMenuType) {
    case 1:
        mpSelectMode->draw(System::getJ2DOrtho());
        break;
    case 2:
        mpTitle->draw(System::getJ2DOrtho());
        break;
    case 3:
        mpEntry->draw(System::getJ2DOrtho());
        break;
    }
    mpPrintMc->draw();
}

void NetGateApp::modeSelect() {
    switch (mpSelectMode->calc()) {
    case 1:
    case 2:
        mState = 4;
        mMenuType = 2;
        mpTitle->start();
        break;
    }
}

void NetGateApp::titleMenu() {
    switch (mpTitle->calc()) {
    case 1:
        mState = 5;
        mMenuType = 3;
        mpEntry->start();
        break;
    case 2:
        mState = 3;
        mMenuType = 1;
        mpSelectMode->start(true);
        break;
    case 3:
        mState = 8;
        mpPrintMc->setStuff(false, false, false);
        mpPrintMc->init(PrintMemoryCard::mcNoSpaceContinue);
        break;
    }
}

void NetGateApp::waitEntry() {
    switch (mpEntry->calc()) {
    case 1:
        NetGameMgr::ptr()->sleep();
        ResMgr::loadCourseData(gLANPlayInfo.getCrsID(), gLANPlayInfo.getCrsOrder());
        mState = 6;
        break;
    case 2:
        mState = 4;
        mMenuType = 2;
        mpTitle->start();
        break;            
    }
}

void NetGateApp::calc() {
    switch(mState) {
    case 0: {
        mState = 1;
        mpEntryArc = nullptr;
        mpSceneArc = nullptr;
        mHasActiveTask = true;

        if (!SYSTEM_RequestTask(loadTask, nullptr, nullptr)) {
#line 209
            JUT_PANIC("task full");
        }
    }

    case 1: {
        if (mpSceneArc) {
            MenuTitleLine::create(mpTitleArc, nullptr);
            mpSelectMode = new LANSelectMode(mpSceneArc);
            mpTitle = new LANTitle(mpSceneArc);
            mpEntry = new LANEntry(mpSceneArc, mpEntryArc);
    case 7:
            NetGameMgr::ptr()->awake();
            NetGameMgr::ptr()->initPadConv();
            mState = 2;
        }
        break;
    }
    case 2: {
        if (NetGameMgr::ptr()->getState() == 0xd) {
            if (msSkip) {
                mState = 4;
                mMenuType = 2;
                mpTitle->start();
            }
            else {
                mState = 3;
                mMenuType = 1;
                mpSelectMode->start(false);
            }
        }
        break;
    }
    case 3: {
        modeSelect();
        break;
    }
    case 4: {
        titleMenu();
        break;
    }
    case 5: {
        waitEntry();
        break;
    }
    case 6: {
        if ((NetGameMgr::ptr()->getState() == 0xf) && (ResMgr::isFinishLoadingArc(ResMgr::mcArcCourse))) {
            AppMgr::deleteCurrentApp();
            RaceApp::call();
        }
        break;
    }
    case 8:  {
        switch (mpPrintMc->get_14Thing() ) {
        case 0:
            mState = 9;
            mpPrintMc->init(PrintMemoryCard::mcMsg38);
            NetGameMgr::ptr()->end(false);
            break;
        case 1:
            mState = 4;
            break;
        }

        break;
    }
    case 9: {
        if (NetGameMgr::ptr()->getState() != 0x14) {
            SYSTEM_StartFadeOut(15);
            mState = 10;
            mpPrintMc->closeWindow();
        }
        break;
    }
    case 10: {
        if (SYSTEM_IsFadingOut()) {
            AppMgr::deleteCurrentApp();
            SequenceApp::call(Scene::SCENE_TITLE);
        }
        break;
    }
    }
    mpPrintMc->calc();
}

void NetGateApp::reset() {}
