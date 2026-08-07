#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "Kaneshige/LightMgr.h"
#include "Kaneshige/RaceDirector.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/Objects/MapObjDemoObj.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "types.h"

// u32 array comprised of 4 char strings.
const u32 TMapObjDemoObj::scCameraID[11] = {
    'firs',     // firs
    'park',     // park
    'fron',     // fron
    'down',     // down
    'farr',     // farr
    'mont',     // mont
    'mare',     // mare
    'item',     // item
    'topp',     // topp
    'endd',     // endd
    'next'     // next
};

f32 TMapObjDemoObj::sTurnSpeed = 0.03f;

TMapObjDemoObj::TMapObjDemoObj(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    flagReset();
}

TMapObjDemoObj::TMapObjDemoObj(u32 id) : TMapObjHioNode(id) {
    flagReset();
}

TMapObjDemoObj::~TMapObjDemoObj() {}

void TMapObjDemoObj::reset() {
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    if ((mDemoFlags & 1) != 0) {
        mKartIndex = RaceMgr::getManager()->searchNRankKart(1);
        f32 zigZagRate = mObjData->mParam2;
        if (zigZagRate <= 0.0f) {
            zigZagRate = 1000.0f;
        }
        mTrackDistanceSq = zigZagRate * zigZagRate;
    } else {
        mKartIndex = -1;
        mTrackDistanceSq = 0.0f;
    }
    mKartDistanceSq = 1e11;
    mDistanceCheckTimer = 0;
}

void TMapObjDemoObj::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x01000200, true);
}

void TMapObjDemoObj::calc() {
    ShadowModel *shadowModel;
    
    if (mKartIndex != -1) {
        ObjUtility::getKartPos(mKartIndex, &mKartPos);
        if (checkKartLength()) {
            turnToKart();
        } else {
            mKartIndex = -1;
        }
    } else if ((mDemoFlags & 1U) != 0) {
        mKartIndex = RaceMgr::getManager()->searchNRankKart(1);
    }

    if (!checkVisible()) {
        setObjFlagHidding();
        shadowModel = mShadowMdl;
        if (shadowModel != nullptr) {
            shadowModel->clrVisibleAll();
        }
    } else {
        clrObjFlagHidding();
        shadowModel = mShadowMdl;
        if (shadowModel != nullptr) {
            shadowModel->setVisibleAll();
        }
    }

    if (mShadowMdl != nullptr) {
        GeographyObj::moveShadowModel();
    }
}

void TMapObjDemoObj::setCurrentViewNo(u32 viewNo) {
    mModel.setCurrentViewNo(viewNo);
    LightObj *lightObj = LightMgr::getManager()->searchLight(viewNo + 0x53434e30);
    MtxPtr effectMtx = lightObj->getEffectMtx();
    mModel.setEffectMtx(effectMtx, 1);
}

void TMapObjDemoObj::flagReset() {
    mDemoFlags = 0;
    mDemoFlags |= 1;
}

bool TMapObjDemoObj::checkKartLength() {
    if (mDistanceCheckTimer == 0) {
        JGeometry::TVec3f diff;
        diff.sub(mKartPos, mPos);
        diff.y = 0.0f;
        mKartDistanceSq = diff.squared();
    }

    mDistanceCheckTimer++;
    if (mDistanceCheckTimer > 30) {
        mDistanceCheckTimer = 0;
    }

    return mKartDistanceSq < mTrackDistanceSq;
}

bool TMapObjDemoObj::checkVisible() {
    u32 cameraTag = RaceMgr::getManager()->mRaceDirector->getDemoCameraTagName();
    s16 cameraMask = mObjData->mParam3;

    bool visible = true;
    for (int i = 0; i < 11; ++i) {
        if (cameraTag == scCameraID[i]) {
            if (cameraMask & (1 << i)) {
                visible = false;
            }
            break;
        }
    }

    return visible;
}

void TMapObjDemoObj::turnToKart() {
    JGeometry::TVec3f kartZXPos;
    JGeometry::TVec3f vecXDir;
    JGeometry::TVec3f vecYDir;
    JGeometry::TVec3f vecZDir;
    JGeometry::TVec3f kartZDiff;

    kartZXPos.sub(mKartPos, mPos);
    kartZXPos.y = 0.0f;
    kartZXPos.normalize();

    mRotMtx.getXDir(vecXDir);
    mRotMtx.getYDir(vecYDir);
    mRotMtx.getZDir(vecZDir);

    if (kartZXPos.isZero()) {
        kartZXPos.set(vecZDir);
    }

    kartZDiff.sub(kartZXPos, vecZDir);
    kartZDiff.y = 0.0f;

    f32 speed = kartZDiff.length();
    if (speed > sTurnSpeed) {
        kartZDiff.setLength(sTurnSpeed);
    }

    if (vecZDir.dot(kartZXPos) < 0.0f) {
        kartZDiff.scale(3.0f);
    }
    
    vecZDir.add(kartZDiff);
    vecZDir.normalize();
    PSVECCrossProduct(&vecYDir, &vecZDir, &vecXDir);
    vecXDir.normalize();

    mRotMtx.setXDir(vecXDir);
    mRotMtx.setYDir(vecYDir);
    mRotMtx.setZDir(vecZDir);
}

#include "JSystem/JAudio/JASFakeMatch2.h"
