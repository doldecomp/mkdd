#include "Shiraiwa/Objects/MapObjIceBlock.h"
#include "Sato/AnmController.h"
#include "Sato/ObjUtility.h"

#include <JSystem/JAudio/JASFakeMatch2.h>

J3DAnmTransform* TMapObjIceBlock::sIceBlockBckAnmTrans;
J3DMtxCalc* TMapObjIceBlock::sIceBlockBckMtxCalc;

TMapObjIceBlock::TMapObjIceBlock(const CrsData::SObject&obj) : TMapObjHioNode(obj) {
    GeographyObj::NewAnmCtrl();
}

TMapObjIceBlock::~TMapObjIceBlock() { }


const char* TMapObjIceBlock::getBmdFileName() {
    static const char *cBmdName = "/Objects/IceBlock1.bmd";
    return cBmdName;
}

void TMapObjIceBlock::reset() {
    GeographyObj::reset();
    GeographyObj::clrObjFlagHidding();
    GeographyObj::clrObjFlagCheckItemHitting();
    GeographyObj::clrAllCheckKartHitFlag();
    GeographyObj::getAnmCtrl()->Reset();
}

void TMapObjIceBlock::loadAnimation() {
    J3DModelData *mdlData = mModel.getModelData();
    void* bck = ObjUtility::getPtrCourseArc("/Objects/IceBlock1.bck");
    J3DAnmObjTrans::setupTransAnmData(&sIceBlockBckAnmTrans, &sIceBlockBckMtxCalc, mdlData, bck);
}

void TMapObjIceBlock::createModel(JKRSolidHeap *heap, u32 viewNo) {
    mModel.createModel(heap, viewNo, 0);
    GeographyObj::getAnmCtrl()->InitRegistration(1, &mModel);
    GeographyObj::getAnmCtrl()->RegisterTrans(0, sIceBlockBckAnmTrans, sIceBlockBckMtxCalc);
    GeographyObj::getAnmCtrl()->getFrameCtrl(0)->setAttribute(2);
}

void TMapObjIceBlock::calc() {

}
