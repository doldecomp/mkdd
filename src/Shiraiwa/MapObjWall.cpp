#include "Shiraiwa/Objects/MapObjWall.h"

J3DAnmTransform *TMapObjWall::sWlWallBckAnmTrans;
J3DMtxCalc *TMapObjWall::sWlWallBckMtxCalc;

bool TMapObjWall::sUseBca = true;

StateObserver::StateFuncSet<TMapObjWall> TMapObjWall::sTable[2] = {
    {0, &TMapObjWall::initFunc_Move, &TMapObjWall::doFunc_Move},
    {1, &TMapObjWall::initFunc_Rest, &TMapObjWall::doFunc_Rest},
};

TMapObjWall::TMapObjWall(const CrsData::SObject &obj) : TMapObjHioNode(obj) {
    NewAnmCtrl();
    createSoundMgr();
    mPathMove = new TPathMove(&obj);
    reset();
}

const char *TMapObjWall::getBmdFileName() {
    static const char *cBmdName = "/Objects/wl_wall1.bmd";
    return cBmdName;
}
