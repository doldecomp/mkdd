#include "Shiraiwa/Objects/MapObjDesun.h"
#include "JSystem/JMath/JMath.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/AnmController.h"
#include "Sato/GeographyObj.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/ObjUtility.h"
#include "dolphin/mtx.h"
#include "math.h"


J3DAnmTransform *TMapObjDesun::sDesunBckAnmTrans;
J3DMtxCalc *TMapObjDesun::sDesunBckMtxCalc;
f32 TMapObjDeBalloon::sFloatHeight = 2000.0f;
s16 TMapObjDeBalloon::sFloatSpeed = 100;



TMapObjDesun::TMapObjDesun(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    setObjFlagSimpleDraw();
    NewAnmCtrl();
}

TMapObjDesun::~TMapObjDesun() {}

const char *TMapObjDesun::getBmdFileName() {
    static const char *cBmdName = "/Objects/desun1.bmd";
    return cBmdName;
}

void TMapObjDesun::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mAnmCtrl->Reset();
}

void TMapObjDesun::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjTrans::setupTransAnmData(&sDesunBckAnmTrans, &sDesunBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/desun1.bck"));
}

void TMapObjDesun::createModel(JKRSolidHeap *heap, u32 p2) {
    AnmController *anmCtrl = mAnmCtrl;
    anmCtrl->mTrans = new AnmControlTrans();
    anmCtrl->mTrans->initAnm(1, &mModel);
    mAnmCtrl->mTrans->registration(0, sDesunBckAnmTrans, sDesunBckMtxCalc);
    mAnmCtrl->mTrans->getFrameCtrl(0)->setAttribute(2);
}

void TMapObjDesun::calc() {}


J3DAnmTexPattern *TMapObjDeBalloon::sDeBalloonBtpAnm;

// HACK: Forces the value 0xc000 (-0x4000).
static const s16 calcStartDeg() {
    return MTXDegToRad(0xc000 / (F_PI / 180));
}
s16 TMapObjDeBalloon::scFloatStartDeg = calcStartDeg();

TMapObjDeBalloon::TMapObjDeBalloon(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
}

TMapObjDeBalloon::~TMapObjDeBalloon() {}

void TMapObjDeBalloon::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x20000, 0);
    mAnmObjMat.setExModel(&mModel);
    mAnmObjMat.setAnmBase(sDeBalloonBtpAnm);
    mAnmObjMat.initFrameCtrl(mAnmObjMat.getAnmBase());
}

void TMapObjDeBalloon::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmObjMat.setupTexPatternAnmData(&sDeBalloonBtpAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/DeBalloon1.btp"));
}

void TMapObjDeBalloon::update() {
    mAnmObjMat.anmFrameProc();
    setModelMatrixAndScale();
    mModel.update(0);
}



void TMapObjDeBalloon::reset() {
    GeographyObj::resetObject();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mAnmObjMat.resetFrame();

    u16 frame = mObjData->mParam1;
    if (frame > mAnmObjMat.getFrameCtrl()->getEnd()) {
        frame = 0;
    }
    
    const s16 startDeg = scFloatStartDeg;
    mAnmObjMat.getFrameCtrl()->setFrame(frame);
    _16c = startDeg;
}

void TMapObjDeBalloon::calc() {
    mPos.set(mObjData->position);
    mPos.y += sFloatHeight * (0.5f * (JMASSin(_16c) + 1.0f));
    _16c += sFloatSpeed;
}

#include "JSystem/JAudio/JASFakeMatch2.h"
