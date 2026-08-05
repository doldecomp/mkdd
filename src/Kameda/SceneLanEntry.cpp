#include "Kameda/SceneLanEntry.h"
#include "Kameda/Scene.h"

#include "mathHelper.h"

SceneLanEntry::SceneLanEntry(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {
    
}

static const char *scpaMsgLabel[] = {
    "LA_CONNECTING",
    "LA_FIND",
    "LA_START",
    "LA_DISCONNECTING",
    "LA_DISCONNECTING_ERROR",
    "LA_NOT_CONNECT_00",
    "LA_NOT_CONNECT_03",
    "LA_NOT_CONNECT_01",
    "LA_ERROR_02"
};

void SceneLanEntry::init() {}

SceneLanEntry::~SceneLanEntry() {}

void SceneLanEntry::draw() {}

void SceneLanEntry::debugDraw() {}

void SceneLanEntry::calc() {}

void SceneLanEntry::start() {}

void SceneLanEntry::waitLink() {}

void SceneLanEntry::waitIPGet() {}

void SceneLanEntry::waitSearch() {}

void SceneLanEntry::waitSetup() {}

void SceneLanEntry::waitSleep() {}

void SceneLanEntry::waitEnd() {}

void SceneLanEntry::waitButton() {}

void SceneLanEntry::nextScene() {}

void SceneLanEntry::backScene() {}

void SceneLanEntry::error(SceneLanEntry::Window::Message) {}

void SceneLanEntry::debug() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
