#include "Shiraiwa/Objects/MapObjPeachTree.h"
#include "Sato/GeographyObj.h"

GeoAnmTableEntry TMapObjPeachTree::sBckAnmTable = {
    0, "PeachTree1_wait.bck"
};

TMapObjPeachTree::TMapObjPeachTree(const CrsData::SObject &sObject) : GeoMarioTree1(sObject) {}

TMapObjPeachTree::~TMapObjPeachTree() {}

const char *TMapObjPeachTree::getBmdFileName() {
    static const char *cTreeBmdName = "PeachTree1.bmd";
    return cTreeBmdName;
}

GeoAnmTableEntry *TMapObjPeachTree::getAnmTbl() {
    if (availBckAnm()) {
        return &sBckAnmTable;
    }
    return nullptr;
}

void TMapObjPeachTree::createColModel(J3DModelData *modelData) {
    createBoundsCylinder(modelData, 210.0f, 500.0f);
}

u16 TMapObjPeachTree::getSizeAnmTbl() {
    return 8;
}

GeoAnmTableEntry TMapObjPeachTree2::sBckAnmTable = {
    0, "PeachTree2_wait.bck"
};

TMapObjPeachTree2::TMapObjPeachTree2(const CrsData::SObject &sObject) : GeoMarioTree1(sObject) {}

TMapObjPeachTree2::~TMapObjPeachTree2() {}

const char *TMapObjPeachTree2::getBmdFileName() {
    static const char* cTreeBmdName = "PeachTree2.bmd";
    return cTreeBmdName;
}

GeoAnmTableEntry *TMapObjPeachTree2::getAnmTbl() {
    if (availBckAnm()) {
        return &sBckAnmTable;
    }
    return nullptr;
}

u16 TMapObjPeachTree2::getSizeAnmTbl() {
    return 8;
}

void TMapObjPeachTree2::createColModel(J3DModelData *modelData) {
    createBoundsCylinder(modelData, 110.0f, 500.0f);
}


#include "JSystem/JAudio/JASFakeMatch2.h"
