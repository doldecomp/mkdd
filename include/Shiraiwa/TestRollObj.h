#ifndef TESTROLLOBJ_H
#define TESTROLLOBJ_H

#include "JSystem/J3D/J3DModel.h"
#include "Kaneshige/Course/CrsData.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "types.h"

class TTestRollObj : public TMapObjHioNode {
public:
    TTestRollObj(const CrsData::SObject &);         // 0x802b0ad4
    virtual ~TTestRollObj();                        // 0x802b0b5c
    virtual const char *getBmdFileName();           // 0x802b0c10
    virtual void reset();                           // 0x802b0c38
    virtual void calc();                            // 0x802b0cc4
    virtual void createColModel(J3DModelData *);    // 0x802b0cf4
    virtual void doKartColCallBack(int);            // 0x802b0d20

    static f32 sRotSpeed;                           // 0x804152c0

    // Inline/Unused
    static f32 sAirFriction;
    static f32 sGravitySize;
    static f32 sReflectRate;


    u8 test[0x154 - 0x14c];                         // 0x14c
    TFreeFallShakeSky *mFreeFallShakeSky;           // 0x154

}; // class TestRollObj
#endif // TESTROLLOBJ_H
