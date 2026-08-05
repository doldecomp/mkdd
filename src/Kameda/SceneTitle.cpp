#include "Kameda/SceneTitle.h"
#include "Kameda/Scene.h"
#include "mathHelper.h"

u8 SceneTitleParam::mDemoType;
int SceneTitle::mDemoFlag = 1;

const u64 SceneTitle::mTitleBckBrkTag[] = {
    'TtlL1', 'TtlL2', 'TtlL3', 'TtlL4', 
    'TtlR1', 'TtlR2', 'TtlR3', 'TtlR4', 
    'TtlM1', 'TtlM2', 'TtlM3', 'TtlM4', 
    'TM2', 
};

SceneTitle::SceneTitle(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {

}

void SceneTitle::init() {}

SceneTitle::~SceneTitle() {}

void SceneTitle::draw() {}

void SceneTitle::calc() {}

void SceneTitle::fadeIn() {}

void SceneTitle::title() {}

void SceneTitle::card() {}

void SceneTitle::titleMenu() {}

void SceneTitle::nextScene() {}

void SceneTitle::demo() {}

void SceneTitle::titleMenuAnm() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
