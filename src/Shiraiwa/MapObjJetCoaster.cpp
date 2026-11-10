#include "Shiraiwa/Objects/MapObjJetCoaster.h"
#include "JSystem/JGeometry/Util.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

f32 TMapObjJetCoaster::sFrameRate = 1.0f;
J3DAnmTransform *TMapObjJetCoaster::sJetCoasterBckAnmTrans;
J3DMtxCalc *TMapObjJetCoaster::sJetCoasterBckMtxCalc;

TMapObjJetCoaster::TMapObjJetCoaster(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
}

TMapObjJetCoaster::TMapObjJetCoaster(u32 id) : TMapObjHioNode(id) {
    NewAnmCtrl();
}

TMapObjJetCoaster::~TMapObjJetCoaster() {}

void TMapObjJetCoaster::reset() {
    GeographyObj::resetObject();
    mPos.zero();
    _158.zero();
    _14c.zero();

    clrObjFlagHidding();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();

    getAnmCtrl()->Reset();
    getAnmCtrl()->getFrameCtrl(0)->setRate(sFrameRate);
    _58 = 4;
    _164 = true;
}

f32 TMapObjJetCoaster::setAnimationFrame(f32 newFrame) {
    getAnmCtrl()->getFrameCtrl(0)->setFrame(newFrame);
}

void TMapObjJetCoaster::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjTrans::setupTransAnmData(&sJetCoasterBckAnmTrans, &sJetCoasterBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/Babyjet.bck"));
}

void TMapObjJetCoaster::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);
    AnmController *anmCtrl = getAnmCtrl();
    anmCtrl->mTrans = new AnmControlTrans;
    anmCtrl->mTrans->initAnm(1, &mModel);
    getAnmCtrl()->mTrans->registration(0, sJetCoasterBckAnmTrans, sJetCoasterBckMtxCalc);
    getAnmCtrl()->getFrameCtrl(0)->setAttribute(2);
    setJointNo();
}

void TMapObjJetCoaster::createColModel(J3DModelData *modelData) {
    createBoundsSphere(360.0f, mScale.y);
}

void TMapObjJetCoaster::getColPos(JGeometry::TVec3f *pos, s32 col) {
    JGeometry::TVec3f jointYDir;
    if (_164) {
        pos->zero();
        jointYDir.set(0.0f, 1.0f, 0.0f);
    } else {
        MtxPtr jointAnmMtx = mModel.getModel()->getAnmMtx(col);
        pos->set(jointAnmMtx[0][3], jointAnmMtx[1][3], jointAnmMtx[2][3]);
        jointYDir.set(jointAnmMtx[0][1], jointAnmMtx[1][1], jointAnmMtx[2][1]);
    }
    
    jointYDir.normalize();
    jointYDir.scale(200.0f);
    pos->add(jointYDir);
}

void TMapObjJetCoaster::calc() {
    JGeometry::TVec3f jointTrans;

    getColPos(&jointTrans, getJointNo());
    setColObjPos(jointTrans, 0);
    
    _14c.set(_158);
    _158.set(jointTrans);
    
    if (_164) {
        _164 = false;
    }
}

const s32 TMapObjJetCoasterHead::scBodyMax = 50;

TMapObjJetCoasterHead::TMapObjJetCoasterHead(const CrsData::SObject &sObject) : TMapObjJetCoaster(sObject) {
    u8 p1 = sObject.mParam1;
    mInitialFrame = sObject.mParam2;
    if (mInitialFrame < 0.0f) {
        mInitialFrame = 0.0f;
    }
    _158.zero();
    mSoundMgr = new GameAudio::CoasterSoundMgr(&_158, nullptr);
    createBody(p1);
}

void TMapObjJetCoasterHead::createBody(u8 num) {
    #line 217
    JUT_ASSERT(num < scBodyMax);

    mBodyCount = (num != 0) ? num : 1;

    mJetCoasterBodies = new TMapObjJetCoasterBody *[mBodyCount]();

    for (int i = 0; i < mBodyCount; i++) {
        mJetCoasterBodies[i] = (TMapObjJetCoasterBody *)GetGeoObjMgr()->createSubObj(0xce7);
    }
}

void TMapObjJetCoasterHead::reset() {
    TMapObjJetCoaster::reset();
    getAnmCtrl()->getFrameCtrl(0)->setFrame(mInitialFrame);
    
    for (int i = 0; i < mBodyCount; i++) {
        f32 offset = (i + 1) * 8.0f;
        int frameCount = getAnmCtrl()->getFrameCtrl(0)->getEnd();
        f32 newFrame = JGeometry::TUtilf::wrap(mInitialFrame - offset, 0.0f, frameCount);
        mJetCoasterBodies[i]->setAnimationFrame(newFrame);
    }
}

void TMapObjJetCoasterHead::calc() {
    JGeometry::TVec3f local_70;
    TMapObjJetCoaster::calc();

    local_70.sub(_158, _14c);

    f32 frameWorkSound = 0.0f;
    if (_14c.isZero() || _158.isZero()) {
        frameWorkSound = 0.0f;
    } else {
        frameWorkSound = local_70.length();
    }
    mSoundMgr->frameWork(frameWorkSound);
}

TMapObjJetCoasterHead::~TMapObjJetCoasterHead() {
    delete[] mJetCoasterBodies;
}

s32 TMapObjJetCoasterHead::sJointNo = -1;
void TMapObjJetCoasterHead::setJointNo() {
    sJointNo = mModel.getModelData()->getJointName()->getIndex("yoshi_jet_head1");
}

s32 TMapObjJetCoasterBody::sJointNo = -1;
void TMapObjJetCoasterBody::setJointNo() {
    sJointNo = mModel.getModelData()->getJointName()->getIndex("yoshi_jet_body1");
}

#include "JSystem/JAudio/JASFakeMatch2.h"
