#ifndef CRSAREA_H
#define CRSAREA_H

#include "types.h"

#include "JSystem/JGeometry.h"

#include "Kaneshige/Course/Course.h"

class CrsArea
{
public:
    CrsArea();

    bool enableLightParam();
    void getRoofPosition(JGeometry::TVec3f *out);
    int getCameraID() const;
    f32 getEchoRate();
    s16 getLightParamID();
    s16 getShadowID() const;    

    void reset();

    void search(u8 id, const JGeometry::TVec3f &p2);
    void searchShadow(const JGeometry::TVec3f &p1) { search(0, p1); }
    void searchCamera(const JGeometry::TVec3f &p1) { search(1, p1); }
    void searchRoof(const JGeometry::TVec3f &p1) { search(2, p1); }
    void searchLight(const JGeometry::TVec3f &p1) { search(7, p1); }

    bool isInside() const { return mArea != nullptr; }
    Course::Area *getArea() const { return mArea; }
    f32 getRate() const { return mRate; }
    f32 getShadowRate() const { return getRate(); }

    bool isKind2() const { // fabricated
        bool ret = false;
        if (isInside()) {
            ret = mArea->getKind() == 2;
        }
        return ret;

    }

private:
    JGeometry::TVec3f mPos;
    Course::Area *mArea;
    f32 mRate;
};

#endif
