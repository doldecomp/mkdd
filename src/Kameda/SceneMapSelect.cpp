#include "Kameda/SceneMapSelect.h"
#include "Kameda/Scene.h"
#include "kartEnums.h"

#include "mathHelper.h"

const SceneMapSelect::MapData SceneMapSelect::mMapData[] = {
    MINI_7, "BattleMapSnap1.bti", "Mozi_Map1.bti",
    MINI_2, "BattleMapSnap3.bti", "Mozi_Map3.bti",
    MINI_3, "BattleMapSnap2.bti", "Mozi_Map2.bti",
    MINI_8, "BattleMapSnap4.bti", "Mozi_Map4.bti",
    MINI_1, "BattleMapSnap6.bti", "Mozi_Map6.bti",
    MINI_5, "BattleMapSnap5.bti", "Mozi_Map5.bti",
};

const u64 SceneMapSelect::mMapSelectBckTag[3] = {
    'NSlMap', 'OK_wb11', 'NRandom', 
};

const u64 SceneMapSelect::mMapBckTag[3] = {
    'NMap', 'Name', 'HighL', 
};

const u64 SceneMapSelect::mMapBrkTag[2] = {
    'MapPict', 'Name',
};

const u64 SceneMapSelect::mMapBpkTag = 'HighL';

SceneMapSelect::SceneMapSelect(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {
    
}

void SceneMapSelect::reset() {}

void SceneMapSelect::init() {}

SceneMapSelect::~SceneMapSelect() {}

void SceneMapSelect::draw() {}

void SceneMapSelect::calc() {}

void SceneMapSelect::map_init() {}

void SceneMapSelect::map() {}

void SceneMapSelect::buttonA() {}

void SceneMapSelect::buttonB() {}

void SceneMapSelect::map_end() {}

void SceneMapSelect::nextScene() {}

void SceneMapSelect::nextBattle() {}

void SceneMapSelect::calcAnm() {}

void SceneMapSelect::rndRoulette() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
