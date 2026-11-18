#include "Shiraiwa/Objects/MapObjNoMove.h"
#include "Sato/GeographyObj.h"

GeoAnmTableEntry TMapObjNoMove::sObjList[4] = {
    { 4505, "/Objects/DonkyWood.bmd" },
    { 4208, "/Objects/Wl_Dokan1.bmd" },
    { 5107, "/Objects/Lights1.bmd" },
    { 5004, "/Objects/DeBalloon1.bmd" }
};

TMapObjNoMove::TMapObjNoMove(const CrsData::SObject &sObject) : GeographyObj(sObject) {
    setObjFlagSimpleDraw();
}

TMapObjNoMove::~TMapObjNoMove() {}

void TMapObjNoMove::reset() {
    GeographyObj::resetObject();
    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
}

const char *TMapObjNoMove::getBmdFileName() {
    for (int i = 0; i < 4; i++) {
        if (mObjData->objID == sObjList[i]._0) {
            return sObjList[i].fileName;
        }
    }
    return NULL;
};

void TMapObjNoMove::calc() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
