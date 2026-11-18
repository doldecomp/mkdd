#include "Shiraiwa/Objects/MapObjSnowMan.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

J3DDeformData *TMapObjSnowMan::sSnowManBlsAnmData;
J3DAnmCluster *TMapObjSnowMan::sSnowManBlkAnmData;


TMapObjSnowMan::TMapObjSnowMan(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {}

TMapObjSnowMan::~TMapObjSnowMan() {}

const char *TMapObjSnowMan::getBmdFileName() {
    static const char *cBmdName = "/Objects/Snowman1.bmd";
    return cBmdName;
}

void TMapObjSnowMan::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _14c.resetFrame();
}


void TMapObjSnowMan::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, false);
    J3DAnmObjCluster::setDeformData(&mModel, sSnowManBlsAnmData, false);
    _14c.setExModel(&mModel, sSnowManBlsAnmData);
    _14c.attach(sSnowManBlkAnmData);
    _14c.getFrameCtrl()->setAttribute(2);
}

void TMapObjSnowMan::loadAnimation() {
    J3DDeformData *snowmanDeformData = (J3DDeformData *)ObjUtility::getPtrCourseArc("/Objects/Snowman1.bls");
    J3DAnmObjCluster::loadClusterData(&sSnowManBlsAnmData, snowmanDeformData);
    J3DAnmCluster *snowmanClusterData = (J3DAnmCluster *)ObjUtility::getPtrCourseArc("/Objects/Snowman1.blk");
    J3DAnmObjCluster::loadClusterAnmData(&sSnowManBlkAnmData, snowmanClusterData);
}

void TMapObjSnowMan::calc() {}

void TMapObjSnowMan::update() {
    _14c.update();
    GeographyObj::update();
}

#include "JSystem/JAudio/JASFakeMatch2.h"
