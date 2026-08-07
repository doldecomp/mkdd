#ifndef MAPOBJKPGEAR_H
#define MAPOBJKPGEAR_H

#include "JSystem/J3D/J3DAnmTransform.h"
#include "JSystem/J3D/J3DMtxCalc.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjKpGear : public TMapObjHioNode {
public:
    TMapObjKpGear(const CrsData::SObject &);            // 0x802d0794
    virtual ~TMapObjKpGear();                           // 0x802d080c
    virtual const char *getBmdFileName();               // 0x802d089c
    virtual void reset();                               // 0x802d08c4
    virtual void loadAnimation();                       // 0x802d0918
    virtual void createModel(JKRSolidHeap *, u32);      // 0x802d0964
    virtual void calc();                                // 0x802d0a9c
    static J3DAnmTransform *sKpGearBckAnmTrans;         // 0x80417138
    static J3DMtxCalc *sKpGearBckMtxCalc;               // 0x8041713c
}; // class MapObjKpGear

#endif // MAPOBJKPGEAR_H
