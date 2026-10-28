#include "Shiraiwa/Objects/MapObjSun.h"
#include "Sato/GeographyObj.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/ObjUtility.h"

J3DAnmTextureSRTKey *TMapObjSun::sSunBtkAnm;

TMapObjSun::TMapObjSun(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    
}

TMapObjSun::~TMapObjSun() {}

const char *TMapObjSun::getBmdFileName() {
    static const char *cBmdName = "/Objects/sun.bmd";
    return cBmdName;
}

void TMapObjSun::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mAnmObjMat.getFrameCtrl()->reset();
}

void TMapObjSun::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x200, 0);
    mAnmObjMat.setExModel(&mModel);
    mAnmObjMat.setAnmBase(sSunBtkAnm);
    mAnmObjMat.initFrameCtrl(mAnmObjMat.mAnmBase);
}

void TMapObjSun::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjMaterial::setupTexSRTAnmData(&sSunBtkAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/sun.btk"));
}

void TMapObjSun::calc() {
    mAnmObjMat.update();
}

void TMapObjSun::update() {
    mAnmObjMat.anmFrameProc();
    GeographyObj::update();
}

#include "JSystem/JAudio/JASFakeMatch2.h"
