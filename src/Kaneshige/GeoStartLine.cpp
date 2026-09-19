#include "Kaneshige/Objects/GeoStartLine.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Vec.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

GeoStartLine::GeoStartLine() : GeographyObj(9) {
    reset();
    setObjFlagMainResource();
}

void GeoStartLine::reset() {
    JGeometry::TVec3f upDir(0.0f, 1.0f, 0.0f);
    JGeometry::TVec3f lfDir;
    JGeometry::TVec3f frDir(0.0f, 0.0f, 1.0f);

    resetObject();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();

    if (!RCMGetCourse()->getCrsData()->isDummyStartLineOn()) {
        setObjFlagHidding();
    }

    CrsData::StartPoint *startPoint = RCMGetCourse()->getCrsData()->getStartPoint(0xff);
    if (startPoint) {
        startPoint->getFrDirection(&frDir);
        startPoint->getLinePosition(&mPos);
    }
    else {
        mPos.zero();
    }

    lfDir.cross(upDir, frDir);

    CrsGround ground;
    ground.search(mPos);
    mPos.y = 2.0f + ground.getHeight();

    ground.getNormal(&upDir);
    frDir.cross(lfDir, upDir);
    frDir.normalize();
    lfDir.cross(upDir, frDir);
    lfDir.normalize();
    mRotMtx.setXYZDir(lfDir, upDir, frDir);
    _58 = 0;
}

void GeoStartLine::createColModel(J3DModelData *) { }

const char *GeoStartLine::getBmdFileName() {
    static const char *cTreeBmdName = "/Objects/StartLine.bmd";
    return cTreeBmdName;
}

void GeoStartLine::calc() {}
