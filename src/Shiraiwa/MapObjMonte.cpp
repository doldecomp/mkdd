#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Osako/ResMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjMonte.h"
#include "Shiraiwa/Objects/MapObjDemoObj.h"
#include "Shiraiwa/SiUtil.h"
#include "dolphin/types.h"
#include "mathHelper.h"

// For `TMapObjMonteBase` and `TMaoObjUkleleMonte`, these classes
// are nearly identical to MapObjMare's classess, except for some 
// strings and static variables such as `sUkMonteBckAnmTrans` and
// `sUkMonteBckMtxCalc`.
//
// Might be able to condense these two classes somehow...?

TAnmInfo TMapObjMonteBase::sAnmInfos[2] = {
    { "/Objects/monL_a_bye1.bck", nullptr, nullptr, 2, 10, 0, 1, 0 },
    { "/Objects/monL_a_clap1.bck", nullptr, nullptr, 2, 10, 0, 0, 0 },
};

StateObserver::StateFuncSet<TMapObjMonteBase> TMapObjMonteBase::sTable[2] = {
    {0, &TMapObjMonteBase::initFunc_Banzai, &TMapObjMonteBase::doFunc_Banzai},
    {1, &TMapObjMonteBase::initFunc_Clapping, &TMapObjMonteBase::doFunc_Clapping},
};

const s8 TMapObjMonteBase::sAnmTable[2] = {
    0, 1
};


TMapObjMonteBase::TMapObjMonteBase(const CrsData::SObject &sObject) : TMapObjDemoObj(sObject) {
    NewAnmCtrl();
    mAnmPlayer.resetAnimations(sAnmInfos, 2);
}

TMapObjMonteBase::~TMapObjMonteBase() {}

void TMapObjMonteBase::reset() {
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

void TMapObjMonteBase::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmPlayer.loadAnimations(sAnmInfos, 2, modelData, ResMgr::mcArcCourse);

    for (u16 i = 0; i < modelData->getShapeNum(); i++) {
        modelData->getShapeNodePointer(i)->setTexMtxLoadType(0x2000);
    }
}

void TMapObjMonteBase::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x1000200, 1);
    mAnmPlayer.registAnimations(getAnmCtrl(), &mModel, sAnmInfos, 2);
}

void TMapObjMonteBase::InitExec() { Observer_FindAndInit(TMapObjMonteBase, 2); }

void TMapObjMonteBase::changeAllState(u16) {}

void TMapObjMonteBase::MoveExec() { Observer_FindAndExec(TMapObjMonteBase, 2); }

void TMapObjMonteBase::calc() {
    TMapObjDemoObj::calc();
    mAnmPlayer.update();
    ExecuteState();
}

void TMapObjMonteBase::initFunc_Clapping() {}

void TMapObjMonteBase::doFunc_Clapping() {
    if (getKartDistanceSq() > _190) {
        setState(0);
        if (sAnmTable[0] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[0];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}

void TMapObjMonteBase::initFunc_Banzai() {}

void TMapObjMonteBase::doFunc_Banzai() {
    if (getKartDistanceSq() < _190) {
        setState(1);
        if (sAnmTable[1] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[1];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}


J3DAnmTransform *TMapObjUkleleMonte::sUkMonteBckAnmTrans;
J3DMtxCalc *TMapObjUkleleMonte::sUkMonteBckMtxCalc;

TMapObjUkleleMonte::TMapObjUkleleMonte(const CrsData::SObject &sObject) : TMapObjDemoObj(sObject) {
    NewAnmCtrl();
}

TMapObjUkleleMonte::~TMapObjUkleleMonte() {}

void TMapObjUkleleMonte::reset() {
    TMapObjDemoObj::reset();
    getAnmCtrl()->Reset();
    SiUtil::setRandomStartFrame(getAnmCtrl()->getFrameCtrl(0), getGeoRnd());
}

void TMapObjUkleleMonte::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmObjTrans->setupTransAnmData(&sUkMonteBckAnmTrans, &sUkMonteBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/uklele_monte.bck"));
    for (u16 i = 0; i < modelData->getShapeNum(); i++) {
        modelData->getShapeNodePointer(i)->setTexMtxLoadType(0x2000);
    }
}

void TMapObjUkleleMonte::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x1000200, 1);

    AnmController *anmCtrl = getAnmCtrl();
    anmCtrl->mTrans = new AnmControlTrans();
    anmCtrl->mTrans->initAnm(1, &mModel);

    getAnmCtrl()->mTrans->registration(0, sUkMonteBckAnmTrans, sUkMonteBckMtxCalc);
    getAnmCtrl()->getFrameCtrl(0)->setAttribute(2);
}

void TMapObjUkleleMonte::calc() {
    TMapObjDemoObj::calc();
}



TAnmInfo TMapObjDanceMonte::sAnmInfos[2] = {
    { "/Objects/monF_a_dance.bck", nullptr, nullptr, 2, 0, 0, 0, 0 },
    { "/Objects/monF_a_fladance.bck", nullptr, nullptr, 2, 0, 0, 1, 0 },
};

TMapObjDanceMonte::TMapObjDanceMonte(const CrsData::SObject &sObject) : TMapObjDemoObj(sObject) {
    NewAnmCtrl();
    mAnmPlayer.resetAnimations(sAnmInfos, 2);
}

TMapObjDanceMonte::~TMapObjDanceMonte() {}

void TMapObjDanceMonte::reset() {
    TMapObjDemoObj::reset();
    getAnmCtrl()->Reset();
    mAnmPlayer.init(getAnmCtrl(), sAnmInfos, 2);
#line 305
    JUT_MINMAX_ASSERT(0, mObjData->mParam5, 2);
    s8 anmState = mObjData->mParam5;
    mAnmPlayer._11 = anmState;
    mAnmPlayer._10 = true;
    mAnmPlayer._e |= 1;
    if (mObjData->mParam6 != 0) {
        SiUtil::setRandomStartFrame(getAnmCtrl()->getFrameCtrl(anmState), getGeoRnd());
    }
}

void TMapObjDanceMonte::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmPlayer.loadAnimations(sAnmInfos, 2, modelData, ResMgr::mcArcCourse);
    for (u16 i = 0; i < modelData->getShapeNum(); i++) {
        modelData->getShapeNodePointer(i)->setTexMtxLoadType(0x2000);
    }
}

void TMapObjDanceMonte::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x1000200, 1);
    mAnmPlayer.registAnimations(getAnmCtrl(), &mModel, sAnmInfos, 2);
}

void TMapObjDanceMonte::calc() {
    TMapObjDemoObj::calc();
    mAnmPlayer.update();
}

#include "JSystem/JAudio/JASFakeMatch2.h"
