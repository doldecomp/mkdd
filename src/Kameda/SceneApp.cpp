#include "Kameda/SceneApp.h"
#include "Inagaki/GameAudioMain.h"
#include "Kameda/Scene.h"
#include "Osako/AppMgr.h"
#include "Osako/GameApp.h"
#include "Osako/MainMenuApp.h"
#include "Osako/SequenceApp.h"
#include "Osako/kartPad.h"
#include "Osako/system.h"


#include <JSystem/JAudio/JASFakeMatch2.h>

SceneApp* SceneApp::mspSceneApp;

SceneApp* SceneApp::create() {
    if (SceneApp::mspSceneApp == nullptr) {
        SceneApp::mspSceneApp = NEW_APP SceneApp();
    }

    return SceneApp::mspSceneApp;
}

void SceneApp::call() {
    AppMgr::setNextApp(AppMgr::mcScene);
}

SceneApp::SceneApp() : GameApp(0x0200000, "Scene", nullptr) {
    _5C = Scene::SCENE_MAX;
    _60 = 0;
}

void SceneApp::draw() {
    if (_5C < Scene::SCENE_MAX && _5C >= Scene::SCENE_TITLE) {
        return;
    }
    System::getJ2DOrtho()->setPort();
    System::getJ2DPrint()->initiate();
    System::getJ2DPrint()->print(80.0f, 50.0f, "SceneTest");

    const char* sceneNames[11] = {
        "TITLE",
        "MENU",
        "OPTION",
        "COURSE",
        "MAP_SELECT",
        "RECORD",
        "GP_NEXT",
        "GHOST_LOAD_SAVE",
        "LAN_ENTRY",
        "SECRET",
        "TEST",
    };

    System::getJ2DPrint()->print(80.0f, 100.0f, "%s", sceneNames[_60]);

}

//https://decomp.me/scratch/XESq4
void SceneApp::calc() {
    if (_5C < Scene::SCENE_MAX && (_5C >= Scene::SCENE_TITLE)) {
        AppMgr::deleteCurrentApp();

        SequenceApp::call(_5C);
        return;
    } else if (gGamePad1P.testTrigger(KartGamePad::A)) {
        GameAudio::Main::getAudio()->startSystemSe(0x20002);
        _5C = (Scene::SceneType)_60;
        return;
    } else if (gGamePad1P.testTrigger(KartGamePad::B)) {
        GameAudio::Main::getAudio()->startSystemSe(0x20004);
        AppMgr::deleteCurrentApp();
        MainMenuApp::call();
        return;
    } else if (gGamePad1P.testRepeat(KartGamePad::MAINSTICK_UP)) {
        GameAudio::Main::getAudio()->startSystemSe(0x20000);
        _60  = (_60 + 9) % 10;
        return;
    } else if (gGamePad1P.testRepeat(KartGamePad::MAINSTICK_DOWN)) {
        GameAudio::Main::getAudio()->startSystemSe(0x20000);
        _60  = ( _60 + 11) % 10;
    }

}

SceneApp::~SceneApp() {
    SceneApp::mspSceneApp = nullptr;
}
