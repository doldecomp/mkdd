#include "Shiraiwa/Objects/MapObjDonkyTree.h"
#include "JSystem/J3D/J3DFrameCtrl.h"
#include "Sato/GeographyObj.h"
#include "Sato/Objects/GeoTree.h"
#include "types.h"

GeoAnmTableEntry TMapObjDonkyTree::sBckAnmTable = {
    0, "DonkyTree1_wait.bck"
};

TMapObjDonkyTree::TMapObjDonkyTree(const CrsData::SObject &sObject) : GeoMarioTree1(sObject) {}

TMapObjDonkyTree::~TMapObjDonkyTree() {}

const char *TMapObjDonkyTree::getBmdFileName() {
    static const char *cTreeBmdName = "DonkyTree1.bmd";
    return cTreeBmdName;
}

GeoAnmTableEntry *TMapObjDonkyTree::getAnmTbl() {
    if (availBckAnm()) {
        return &sBckAnmTable;
    }
    return nullptr;
}

GeoAnmTableEntry TMapObjDinoTree::sBckAnmTable = {
    0, "DinoTree1_wait.bck"
};

TMapObjDinoTree::TMapObjDinoTree(const CrsData::SObject &sObject) : GeoMarioTree1(sObject) {}

TMapObjDinoTree::~TMapObjDinoTree() {}

void TMapObjDinoTree::reset() {
    GeographyObj::resetObject();
    _58 = 0;
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
    AnmController *anmCtrl = getAnmCtrl();
    if (anmCtrl) {
        J3DFrameCtrl *ctrl = getAnmCtrl()->getFrameCtrl(0);
        ctrl->setAttribute(2);
        ctrl->setRate(((getGeoRnd()->get_ufloat_1() * 2 - 1) * 0.3f + 1.0f));
    }
}

const char *TMapObjDinoTree::getBmdFileName() {
    static const char *cTreeBmdName = "DinoTree1.bmd";
    return cTreeBmdName;
}

GeoAnmTableEntry *TMapObjDinoTree::getAnmTbl() {
    if (availBckAnm()) {
        return &sBckAnmTable;
    }
    return nullptr;
}

TMapObjDesertTree::TMapObjDesertTree(const CrsData::SObject &sObject) : GeoMarioTree1(sObject) {
    clrObjFlagLODBias();
}

TMapObjDesertTree::~TMapObjDesertTree() {}

void TMapObjDesertTree::reset() {
    GeographyObj::resetObject();
    _58 = 1;
    setAllCheckKartHitFlag();
    setObjFlagCheckItemHitting();
}

const char *TMapObjDesertTree::getBmdFileName() {
    static const char *cTreeBmdName = "DesertTree1.bmd";
    return cTreeBmdName;
}

void TMapObjDesertTree::createColModel(J3DModelData *modelData) {
    createBoundsCylinder(modelData, 210.0f, 700.0f);
}

#include "JSystem/JAudio/JASFakeMatch2.h"
