#include "Sato/Objects/GeoObjPathMove.h"

#include "JSystem/JAudio/JASFakeMatch2.h"
#include "JSystem/JGeometry/Vec.h"
#include "Kaneshige/Objects/GeoCar.h"
#include "Kaneshige/Objects/GeoItemCar.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/stMath.h"

namespace {
    f32 sAchieveLength = 300.0f;
}

void GeoObjPathMove::stdInit(JGeometry::TVec3f *pos, const f32 &p2) {
    _4 = 0;
    _6 = 1;
    getPathPos(pos, 0);
    mPathPosY = pos->y;
    _8 = p2;
}

void GeoObjPathMove::initPath(stRandom *rand, JGeometry::TVec3f *pos, const f32 &p3) {
    stdInit(pos, p3);
}

void GeoObjPathMove::stdPathMove(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, f32 acc, f32 maxVel) {
    JGeometry::TVec3f pathPos;
    getPathPos(&pathPos, _4);
    pos->y = mPathPosY;

    JGeometry::TVec3f diff;
    diff.sub(pathPos, *pos);
    if (diff.length() < sAchieveLength) {
        nodeProc();
    }
    stVecNormalize(diff);

    diff.scale(acc);
    vel->add(diff);
    
    if (vel->length() > maxVel) {
        stVecNormalize(*vel);
        vel->scale(maxVel);
    }

    pos->add(*vel);
    mPathPosY = pos->y;
    pos->y += _8;
}

void GeoObjPathMove::movePath(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, f32 acc, f32 maxVel) {
    stdPathMove(pos, vel, acc, maxVel);
}

void GeoObjPathMove::nodeProc() {
    _4 += _6;
    if (_4 >= (getMaxPathNodeNum() - 1)) {
        if (!getPathData()->isClosed()) {
            _6 = -1;
        }
        else {
            _4 = 0;
        }
    }
    else if (_4 == 0) {
        _6 = 1;
    }
}

CrsData::PathData *GeoObjPathMove::getPathData() {
    return RCMGetCourse()->getCrsData()->getPathData(mObj.mPathID);
}

u16 GeoObjPathMove::getMaxPathNodeNum() {
    return getPathData()->getPointNumber();
}

void GeoObjPathMove::getPathPos(JGeometry::TVec3f *pos, u16 pointID) {
    *pos = RCMGetCourse()->getCrsData()->getPointData(mObj.mPathID, pointID)->pos;
}

void GeoObjPathMoveWaveY::initPath(stRandom *rand, JGeometry::TVec3f *pos, const f32 &p3) {
    stdInit(pos, p3);
    initSwingVel(rand, 1.0f);
}

void GeoObjPathMoveWaveY::movePath(JGeometry::TVec3f *pos, JGeometry::TVec3f *vel, f32 acc, f32 maxVel) {
    stdPathMove(pos, vel, acc, maxVel);
    mSwingVel += GeoItemCar::getSwingVel();
    pos->y += GeoItemCar::getSwingHeight(mSwingVel);
}
