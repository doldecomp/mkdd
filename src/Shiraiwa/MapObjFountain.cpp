#include "JSystem/J3D/J3DAnmTextureSRTKey.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/Objects/MapObjFountain.h"

J3DAnmTextureSRTKey *TMapObjFountain::sFountainBtkAnm;

TMapObjFountain::TMapObjFountain(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    setObjFlagSimpleDraw();
    createSoundMgr();
}

TMapObjFountain::~TMapObjFountain() {}

const char *TMapObjFountain::getBmdFileName() {
    static const char *cBmdName = "/Objects/PeachFountain.bmd";
    return cBmdName;
}

void TMapObjFountain::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mAnmObj.resetFrame();
}

void TMapObjFountain::createModel(JKRSolidHeap *jkrSolidHeap, u32 p2) {
    mAnmObj.mModel = &mModel;
    mAnmObj.mAnmBase = sFountainBtkAnm;
    mAnmObj.initFrameCtrl(mAnmObj.getAnmBase());
}

void TMapObjFountain::loadAnimation() {
    J3DModelData *modelData = *mModel.mModelData;
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/PeachFountain.btk");
    J3DAnmObjMaterial::setupTexSRTAnmData(&sFountainBtkAnm, modelData, ptrCourseArc);
}

void TMapObjFountain::calc() {
    mAnmObj.getFrameCtrl()->update();
    getSoundMgr()->setSe(0x40048);
}

void TMapObjFountain::update() {
    mAnmObj.anmFrameProc();
    GeographyObj::update();
}



J3DAnmTextureSRTKey *TMapObjDemoFountain::sDemoFountainBtkAnm;

TMapObjDemoFountain::TMapObjDemoFountain(const CrsData::SObject &sObject) : TMapObjFountain(sObject) {
    setObjFlagSimpleDraw();
}

TMapObjDemoFountain::~TMapObjDemoFountain() {}

void TMapObjDemoFountain::createModel(JKRSolidHeap *jkrSolidHeap, u32 p2) {
    mAnmObj.setExModel(&mModel);
    mAnmObj.setAnmBase(sDemoFountainBtkAnm);
    mAnmObj.initFrameCtrl(mAnmObj.getAnmBase());
}

void TMapObjDemoFountain::loadAnimation() {
    J3DModelData *modelData = *mModel.mModelData;
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/DPeachFountain.btk");
    J3DAnmObjMaterial::setupTexSRTAnmData(&sDemoFountainBtkAnm, modelData, ptrCourseArc);
}

void TMapObjDemoFountain::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mAnmObj.resetFrame();
    mIsMakeEmt = true;
}

void TMapObjDemoFountain::calc() {
    if (mIsMakeEmt != 0) {
        JPEffectMgr::getEffectMgr()->createEmt("mk_fountainWtr_a", mPos);
        mIsMakeEmt = 0;
    }
    mAnmObj.getFrameCtrl()->update();
    getSoundMgr()->setSe(0x40082);
}

#include "JSystem/JAudio/JASFakeMatch2.h"
