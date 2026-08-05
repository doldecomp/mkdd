#ifndef TKART_THROWER_H
#define TKART_THROWER_H

#include <JSystem/JGeometry.h>
#include "JSystem/JGeometry/Vec.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TKartThrower : public TMapObjHioNode {
public:
    virtual ~TKartThrower() {}

    virtual void getThrowDir(JGeometry::TVec3f* dst, s16);
    virtual s32 getThrowPow();
};

#endif
