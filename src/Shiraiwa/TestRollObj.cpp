#include "Shiraiwa/TestRollObj.h"
#include "JSystem/JGeometry/Vec.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

f32 TTestRollObj::sRotSpeed = 0.95f;

// Inline/Unused
f32 TTestRollObj::sAirFriction;
f32 TTestRollObj::sGravitySize;
f32 TTestRollObj::sReflectRate;

TTestRollObj::TTestRollObj(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    mFreeFallShakeSky = new TFreeFallShakeSky(&mPos, &mVel, &mRotMtx);
}

TTestRollObj::~TTestRollObj() {
    delete mFreeFallShakeSky;
}

const char *TTestRollObj::getBmdFileName() {
    static const char *cBmdName = "/Objects/TestPaipo1.bmd";
    return cBmdName;
}

void TTestRollObj::reset() {
    resetObject();
    _58 = 4;
    mFreeFallShakeSky->reset();
    mFreeFallShakeSky->mGravity = 50.0f * mScale.y;
    mFreeFallShakeSky->mRotSpeed = sRotSpeed / mScale.y;
    mFreeFallShakeSky->mRnd = getGeoRnd();
    mFreeFallShakeSky->mEnabled = true;
}
void TTestRollObj::calc() {
    mFreeFallShakeSky->update();
}

void TTestRollObj::createColModel(J3DModelData *modelData) {
    createBoundsSphere(modelData);
}

void TTestRollObj::doKartColCallBack(int kartIdx) {
    JGeometry::TVec3f kartPos;
    JGeometry::TVec3f collisionVec;
    
    ObjUtility::getKartPos(kartIdx, &kartPos);
    collisionVec.sub(mPos, kartPos);
    collisionVec.normalize();
    collisionVec.y = 0.5f;
    collisionVec.scale(3.0f);
    mVel.add(collisionVec);
}

#include "JSystem/JAudio/JASFakeMatch2.h"
