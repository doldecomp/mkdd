#include "Shiraiwa/Objects/MapObjMare.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/SiUtil.h"

TAnmInfo TMapObjMareBase::sAnmInfos[2] = {
    { "/Objects/mareL_a_bye1.bck", nullptr, nullptr, 2, 10, 0, 1, 0 },
    { "/Objects/mareL_a_clap1.bck", nullptr, nullptr, 2, 10, 0, 0, 0 },
};

StateObserver::StateFuncSet<TMapObjMareBase> TMapObjMareBase::sTable[2] = {
    {0, &TMapObjMareBase::initFunc_Banzai, &TMapObjMareBase::doFunc_Banzai },
    {1, &TMapObjMareBase::initFunc_Clapping, &TMapObjMareBase::doFunc_Clapping },
};

const s8 TMapObjMareBase::sAnmTable[2] = {
    0, 1
};


TMapObjMareBase::TMapObjMareBase(const CrsData::SObject &sObject) : TMapObjDemoObj(sObject), StateObserver() {
    NewAnmCtrl();
    mAnmPlayer.resetAnimations(sAnmInfos, 2);
}

TMapObjMareBase::~TMapObjMareBase() {}

void TMapObjMareBase::reset() {
    TMapObjDemoObj::reset();
    mAnmCtrl->Reset();
    mAnmPlayer.init(mAnmCtrl, sAnmInfos, 2);
    f32 param1 = mObjData->mParam1;
    if (param1 <= 0.0f) {
        param1 = 700.0f;
    }
    _190 = param1 * param1;
    SiUtil::setRandomStartFrame(getAnmCtrl()->getFrameCtrl(0), getGeoRnd());
    ResetState();
}

void TMapObjMareBase::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmPlayer.loadAnimations(sAnmInfos, 2, modelData, ResMgr::mcArcCourse);

    for (u16 i = 0; i < modelData->getShapeNum(); i++) {
        modelData->getShapeNodePointer(i)->setTexMtxLoadType(0x2000);
    }
}

void TMapObjMareBase::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x1000200, 1);
    mAnmPlayer.registAnimations(getAnmCtrl(), &mModel, sAnmInfos, 2);
}

void TMapObjMareBase::InitExec() { Observer_FindAndInit(TMapObjMareBase, 2); }

void TMapObjMareBase::changeAllState(u16) {}

void TMapObjMareBase::MoveExec() { Observer_FindAndExec(TMapObjMareBase, 2); }

void TMapObjMareBase::calc() {
    TMapObjDemoObj::calc();
    mAnmPlayer.update();
    ExecuteState();
}

void TMapObjMareBase::initFunc_Clapping() {
}

void TMapObjMareBase::doFunc_Clapping() {
    if (getKartDistanceSq() > _190) {
        setState(0);
        if (sAnmTable[0] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[0];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}

void TMapObjMareBase::initFunc_Banzai() {}

void TMapObjMareBase::doFunc_Banzai() {
    if (getKartDistanceSq() < _190) {
        setState(1);
        if (sAnmTable[1] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[1];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}

// Inline/Unused
void changeAllState(u16);



J3DAnmTransform *TMapObjMareWBase::sMareWBckAnmTrans;
J3DMtxCalc *TMapObjMareWBase::sMareWBckMtxCalc;

TMapObjMareWBase::TMapObjMareWBase(const CrsData::SObject &sObject) : TMapObjDemoObj(sObject) {
    NewAnmCtrl();
}

TMapObjMareWBase::~TMapObjMareWBase() {}

void TMapObjMareWBase::reset() {
    TMapObjDemoObj::reset();
    getAnmCtrl()->Reset();
    SiUtil::setRandomStartFrame(getAnmCtrl()->getFrameCtrl(0), getGeoRnd());
}

void TMapObjMareWBase::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmObjTrans->setupTransAnmData(&sMareWBckAnmTrans, &sMareWBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/mareW_a_dance.bck"));
    for (u16 i = 0; i < modelData->getShapeNum(); i++) {
        modelData->getShapeNodePointer(i)->setTexMtxLoadType(0x2000);
    }
}

void TMapObjMareWBase::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x1000200, 1);

    AnmController *anmCtrl = getAnmCtrl();
    anmCtrl->mTrans = new AnmControlTrans();
    anmCtrl->mTrans->initAnm(1, &mModel);

    getAnmCtrl()->mTrans->registration(0, sMareWBckAnmTrans, sMareWBckMtxCalc);
    getAnmCtrl()->getFrameCtrl(0)->setAttribute(2);
}

void TMapObjMareWBase::calc() {
    TMapObjDemoObj::calc();
}


J3DAnmTransform *TMapObjMareM_A::sMareMBckAnmTrans;
J3DMtxCalc *TMapObjMareM_A::sMareMBckMtxCalc;

TMapObjMareM_A::TMapObjMareM_A(const CrsData::SObject &sObject) : TMapObjDemoObj(sObject) {
    NewAnmCtrl();
}

TMapObjMareM_A::~TMapObjMareM_A() {}

void TMapObjMareM_A::reset() {
    TMapObjDemoObj::reset();
    mAnmCtrl->Reset();
    SiUtil::setRandomStartFrame(getAnmCtrl()->getFrameCtrl(0), getGeoRnd());
}

void TMapObjMareM_A::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmObjTrans->setupTransAnmData(&sMareMBckAnmTrans, &sMareMBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/mareM_a.bck"));
    for (u16 i = 0; i < modelData->getShapeNum(); i++) {
        modelData->getShapeNodePointer(i)->setTexMtxLoadType(0x2000);
    }
}

void TMapObjMareM_A::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x1000200, 1);

    AnmController *anmCtrl = getAnmCtrl();
    anmCtrl->mTrans = new AnmControlTrans();
    anmCtrl->mTrans->initAnm(1, &mModel);

    getAnmCtrl()->mTrans->registration(0, sMareMBckAnmTrans, sMareMBckMtxCalc);
    getAnmCtrl()->getFrameCtrl(0)->setAttribute(2);
}

void TMapObjMareM_A::calc() {
    TMapObjDemoObj::calc();
}

#include "JSystem/JAudio/JASFakeMatch2.h"
