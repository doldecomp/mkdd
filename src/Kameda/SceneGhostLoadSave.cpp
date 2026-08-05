#include "Kameda/SceneGhostLoadSave.h"

#include "Kameda/Scene.h"
#include "kartEnums.h"
#include "mathHelper.h"

const char *scpaBaseFileName[] = {
    "GD_Title", "SelectMemoryCard", "GDIndexLayout", "GDIndexLine",

};

static const ECourseID caRaceCrsID[16] = {
    LUIGI_CIRCUIT, PEACH_BEACH, BABY_PARK, DRY_DRY_DESERT,
    MUSHROOM_BRIDGE, MARIO_CIRCUIT, DAISY_CRUISER, WALUIGI_STADIUM,
    SHERBET_LAND, MUSHROOM_CITY, YOSHI_CIRCUIT, DK_MOUNTAIN,
    WARIO_COLOSSEUM, DINO_DINO_JUNGLE, BOWSER_CASTLE, RAINBOW_ROAD
};

const u64 SceneGhostLoadSave::Card::mscaPaneTag[] = {
    'MeSlot_A', 'MemB_A', 'MemTxB_A', 'MemP_A', 
    'MmData_A', 'SlotWB_A', 'Slot_A', 'SlotW_A', 
    'NMmEf_A', 
};

void SceneGhostLoadSave::GhostFileInfoTable::append(s32, u32, TARecord &) {}

void SceneGhostLoadSave::GhostFileInfoTable::convCrsID(u8) {}

void SceneGhostLoadSave::Card::searchPane(u8, J2DScreen *) {}

void SceneGhostLoadSave::Card::setCardInfo() {}

void SceneGhostLoadSave::Card::process() {}

void SceneGhostLoadSave::Card::unselect() {}

void SceneGhostLoadSave::Card::insertCard() {}

void SceneGhostLoadSave::Card::error() {}

void SceneGhostLoadSave::Card::calcAnm() {}

const u64 SceneGhostLoadSave::QuitBtn::mscaPaneTag[] = {
    'NMemQuit', 'MemQuitB', 'MemQuitW', 'NMmEf_M', 
    'MemOK', 
};

void SceneGhostLoadSave::QuitBtn::calcAnm() {}

void SceneGhostLoadSave::SelectSlot::setup(JKRArchive *, JKRHeap *) {}

void SceneGhostLoadSave::SelectSlot::init() {}

void SceneGhostLoadSave::SelectSlot::processCard() {}

void SceneGhostLoadSave::SelectSlot::watchCardState() {}

void SceneGhostLoadSave::SelectSlot::selectSlot() {}

void SceneGhostLoadSave::SelectSlot::frameIn() {}

const u64 SceneGhostLoadSave::GDInexLineVC::mscaPaneTag[] = {
    'NGDline', 'GDNew', 
};

void SceneGhostLoadSave::GDInexLineVC::setup(JKRArchive *, JKRHeap *, bool) {}

void SceneGhostLoadSave::GDInexLineVC::updateInfo() {}

void SceneGhostLoadSave::GDIndexLayoutVC::setup(JKRArchive *, JKRHeap *) {}

void SceneGhostLoadSave::GDIndexLayoutVC::setCard(SceneGhostLoadSave::Card *) {}

void SceneGhostLoadSave::GDIndexLayoutVC::selectData() {}

void SceneGhostLoadSave::GDIndexLayoutVC::scrollUp() {}

void SceneGhostLoadSave::GDIndexLayoutVC::scrollDown() {}

void SceneGhostLoadSave::GDIndexLayoutVC::shiftUp() {}

void SceneGhostLoadSave::GDIndexLayoutVC::calcAnm() {}

SceneGhostLoadSave::SceneGhostLoadSave(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {
    // SceneGhostLoadSave::Card::Card() {}
    // SceneGhostLoadSave::GhostFileInfo::GhostFileInfo() {}
}

void SceneGhostLoadSave::init() {}

SceneGhostLoadSave::~SceneGhostLoadSave() {}

void SceneGhostLoadSave::draw() {}

void SceneGhostLoadSave::calc() {}

void SceneGhostLoadSave::startFadeIn() {}

void SceneGhostLoadSave::checkCard() {}

void SceneGhostLoadSave::fadeIn() {}

void SceneGhostLoadSave::frameInSlot() {}

void SceneGhostLoadSave::selectSlot() {}

void SceneGhostLoadSave::confirm() {}

void SceneGhostLoadSave::frameOutSlot() {}

void SceneGhostLoadSave::frameInData() {}

void SceneGhostLoadSave::selectData() {}

void SceneGhostLoadSave::ghostSaveLoad() {}

void SceneGhostLoadSave::frameOutData() {}

void SceneGhostLoadSave::nextScene() {}

void SceneGhostLoadSave::prevScene() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
