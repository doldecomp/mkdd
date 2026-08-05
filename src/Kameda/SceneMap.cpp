#include "Kameda/SceneMap.h"
#include "Kameda/Scene.h"
#include "mathHelper.h"

SceneMap::SceneMap(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {
    init();
}

void SceneMap::init() {}

SceneMap::~SceneMap() {}

void SceneMap::draw() {}

void SceneMap::calc() {}

void SceneMap::map() {}

void SceneMap::nextScene() {}

void SceneMap::nextRace() {}

void SceneMap::nextAward() {}

void SceneMap::nextEnding() {}

void SceneMap::nextTrueEnding() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
