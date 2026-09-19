#include "JSystem/J3D/J3DAnmTextureSRTKey.h"
#include "JSystem/J3D/J3DAnmTransform.h"
#include "JSystem/J3D/J3DMtxCalc.h"
#include "Sato/AnmController.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/Objects/MapObjWluArrow.h"

J3DAnmTextureSRTKey *TMapObjWlArrow::sWlArrowBtkAnm;
J3DAnmTransform *TMapObjWlArrow::sWlArrowBckAnmTrans;
J3DMtxCalc *TMapObjWlArrow::sWlArrowBckMtxCalc;

J3DAnmTransform *TMapObjWluSearchLight::sWlScLightBckAnmTrans;
J3DMtxCalc *TMapObjWluSearchLight::sWlScLightBckMtxCalc;


TMapObjWlArrow::TMapObjWlArrow(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
}

TMapObjWlArrow::~TMapObjWlArrow() {}

const char *TMapObjWlArrow::getBmdFileName() {
    static const char *cBmdName = "/Objects/Wlarrow1.bmd";
    return cBmdName;
}

void TMapObjWlArrow::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mAnmObjMat.getFrameCtrl()->reset();
    getAnmCtrl()->Reset();
}

void TMapObjWlArrow::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x200, 0);

    mAnmObjMat.setExModel(&mModel);
    mAnmObjMat.setAnmBase(sWlArrowBtkAnm);
    mAnmObjMat.initFrameCtrl(mAnmObjMat.mAnmBase);

    AnmController *anmCtrl = mAnmCtrl;
    AnmControlTrans *anmCtrlTrans = new AnmControlTrans();

    anmCtrl->mTrans = anmCtrlTrans;
    anmCtrlTrans = anmCtrl->mTrans;
    anmCtrlTrans->initAnm(1, &mModel);

    mAnmCtrl->mTrans->registration(0, sWlArrowBckAnmTrans, sWlArrowBckMtxCalc);
    getAnmCtrl()->getFrameCtrl(0)->setAttribute(2);
}

void TMapObjWlArrow::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjTrans::setupTransAnmData(&sWlArrowBckAnmTrans, &sWlArrowBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/Wlarrow1.bck"));
    J3DAnmObjMaterial::setupTexSRTAnmData(&sWlArrowBtkAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/Wlarrow1.btk"));
}

void TMapObjWlArrow::calc() {
    mAnmObjMat.update();
}

void TMapObjWlArrow::update() {
    mAnmObjMat.anmFrameProc();
    GeographyObj::update();
}


TMapObjWluSearchLight::TMapObjWluSearchLight(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
}

TMapObjWluSearchLight::~TMapObjWluSearchLight() {}

const char *TMapObjWluSearchLight::getBmdFileName() {
    static const char *cBmdName = "/Objects/Wa_Search1.bmd";
    return cBmdName;
}

void TMapObjWluSearchLight::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    getAnmCtrl()->Reset();
}

void TMapObjWluSearchLight::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);

    AnmController *anmCtrl = mAnmCtrl;
    AnmControlTrans *anmCtrlTrans = new AnmControlTrans();

    anmCtrl->mTrans = anmCtrlTrans;
    anmCtrlTrans = anmCtrl->mTrans;
    anmCtrlTrans->initAnm(1, &mModel);

    mAnmCtrl->mTrans->registration(0, sWlScLightBckAnmTrans, sWlScLightBckMtxCalc);
    getAnmCtrl()->getFrameCtrl(0)->setAttribute(2);
}

void TMapObjWluSearchLight::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjTrans::setupTransAnmData(&sWlScLightBckAnmTrans, &sWlScLightBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/Wa_Search1.bck"));
}

#include "JSystem/JAudio/JASFakeMatch2.h"
