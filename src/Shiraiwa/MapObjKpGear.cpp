#include "JSystem/J3D/J3DAnmTransform.h"
#include "JSystem/J3D/J3DMtxCalc.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/AnmController.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/Objects/MapObjKpGear.h"

J3DAnmTransform *TMapObjKpGear::sKpGearBckAnmTrans;
J3DMtxCalc *TMapObjKpGear::sKpGearBckMtxCalc;

TMapObjKpGear::TMapObjKpGear(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
}

TMapObjKpGear::~TMapObjKpGear() {}

const char *TMapObjKpGear::getBmdFileName() {
    static const char *cBmdName = "/Objects/KpGear1.bmd";
    return cBmdName;
}

void TMapObjKpGear::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mAnmCtrl->Reset();
}

void TMapObjKpGear::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjTrans::setupTransAnmData(&sKpGearBckAnmTrans, &sKpGearBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/KpGear1.bck"));
}

void TMapObjKpGear::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);

    AnmController *anmCtrl = mAnmCtrl;
    anmCtrl->mTrans = new AnmControlTrans();

    AnmControlTrans *anmCtrlTrans = anmCtrl->mTrans;
    anmCtrlTrans->initAnm(1, &mModel);
    mAnmCtrl->mTrans->registration(0, sKpGearBckAnmTrans, sKpGearBckMtxCalc);

    J3DFrameCtrl *ctrl = getAnmCtrl()->getFrameCtrl(0);
    ctrl->setAttribute(2);
}

void TMapObjKpGear::calc() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
