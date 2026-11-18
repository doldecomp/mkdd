#include "Shiraiwa/Objects/MapObjWall.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "Sato/AnmController.h"
#include "Sato/GeographyObj.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3D.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

StateObserver::StateFuncSet<TMapObjWall> TMapObjWall::sTable[2] = {
    {0, &TMapObjWall::initFunc_Move, &TMapObjWall::doFunc_Move},
    {1, &TMapObjWall::initFunc_Rest, &TMapObjWall::doFunc_Rest},
};

J3DAnmTransform *TMapObjWall::sWlWallBckAnmTrans;
J3DMtxCalc *TMapObjWall::sWlWallBckMtxCalc;
bool TMapObjWall::sUseBca = true;

TMapObjWall::TMapObjWall(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
    createSoundMgr();
    mPathMove = new TPathMove(&sObject);
    reset();
}

TMapObjWall::~TMapObjWall() {}

void TMapObjWall::reset() {
    resetObject();
    setAllCheckKartHitFlag();
    setObjFlagCheckItemHitting();

    _15c = mObjData->mParam1;

    const CrsData::SObject *sObject = mObjData;
    f32 p2 = sObject->mParam2;

    if (p2 <= 0.0f) {
        p2 = 10.0f;
    }

    if (sObject->mPathID != 0xffff) {
        mPathMove->init(&mPos, &mVel);
        TPathMove *pathMove = mPathMove;
        pathMove->_14 = 1.0f;
        pathMove->_18 = p2;
        pathMove->_20 = SQUARE(p2 * 0.5f);
        ResetState();
    } else {
        setState(1);
    }

    if (sUseBca != 0) {
        mAnmCtrl->Reset();
    }

    mReaction.setFlg(7, 2);
    _58 = 1;
}

const char *TMapObjWall::getBmdFileName() {
    static const char *cBmdName = "/Objects/wl_wall1.bmd";
    return cBmdName;
}

void TMapObjWall::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/wl_wall1.bca");
    if (ptrCourseArc != nullptr) {
        sUseBca = true;
        J3DAnmObjTrans::setupTransAnmData(&sWlWallBckAnmTrans, &sWlWallBckMtxCalc, modelData, ptrCourseArc);
    } else {
        sUseBca = false;
    }
}

void TMapObjWall::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);
    if (sUseBca != false) {
        AnmController *anmCtrl = mAnmCtrl;
        anmCtrl->mTrans = new AnmControlTrans();
        anmCtrl->mTrans->initAnm(1, &mModel);
        mAnmCtrl->mTrans->registration(0, sWlWallBckAnmTrans, sWlWallBckMtxCalc);
        mAnmCtrl->mTrans->getFrameCtrl(0)->setAttribute(2);
    }
}

void TMapObjWall::calc() {
    ExecuteState();
    if (mObjData->mPathID != 0xffff) {
        mPathMove->update();
    }
    
    JGeometry::TPos3f rotMtx;
    PSMTXCopy(mRotMtx, rotMtx);
    rotMtx[0][3] = mPos.x;
    rotMtx[1][3] = mPos.y;
    rotMtx[2][3] = mPos.z;

    ((ObjColCube *)getBounds(0))->updateParameter(rotMtx);
}

void TMapObjWall::InitExec() { Observer_FindAndInit(TMapObjWall, 2); }

void TMapObjWall::MoveExec() { Observer_FindAndExec(TMapObjWall, 2); }

void TMapObjWall::initFunc_Move() {
    switch (mPathMove->_8) {
        case 0:
            getSoundMgr()->setSe(0x4004e);
            break;
        case 1:
            getSoundMgr()->setSe(0x4004f);
            break;
    }
    mPathMove->setTargetNode();
}

void TMapObjWall::doFunc_Move() {
    if (mPathMove->_1c == false) {
        mVel.zero();
        setState(1);
    }
}

void TMapObjWall::initFunc_Rest() {}

void TMapObjWall::doFunc_Rest() {
    if (mObjData->mPathID != 0xffff) {
        if (mPathMove->_8 == 0 || getStateCount() > _15c) {
            setState(0);
        }
    }
}

void TMapObjWall::createColModel(J3DModelData *modelData) {
    createBoundsCube(modelData);
}

#include "JSystem/JAudio/JASFakeMatch2.h"
