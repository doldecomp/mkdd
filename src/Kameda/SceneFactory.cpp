#include "Kameda/SceneFactory.h"

static const char *scpaArcName[15] = {
    "title", "menu", "option", "courseselect",
    "mapselect", "record", "ghostdata", "lanentry",
    "titleline", "back", "coursename", "battlename",
    "cup", "selectAnm", "secret"
};

SceneFactory *SceneFactory::mspSceneFactory;

SceneFactory::SceneFactory() {}

SceneFactory::~SceneFactory() {}

void SceneFactory::loadData(Scene::SceneType, JKRHeap *) {}

Scene *SceneFactory::createScene(Scene::SceneType, JKRHeap *) {}
