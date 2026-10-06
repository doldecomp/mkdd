#ifndef MAPOBJVENTILATOR_H
#define MAPOBJVENTILATOR_H

#include "Kaneshige/Course/CrsData.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjVentilator : public TMapObjHioNode, public StateObserver {
public:
    TMapObjVentilator(const CrsData::SObject &);        // 0x802d48e8
    virtual ~TMapObjVentilator();                       // 0x802d495c
    virtual void MoveExec();                            // 0x802d4a98
    virtual void InitExec();                            // 0x802d4a2c
    virtual const char *getBmdFileName();               // 0x802d4a04
    virtual void reset();                               // 0x802d4b04
    virtual void calc();                                // 0x802d4cb8
    
    void doRotate(const JGeometry::TQuat4f &);          // 0x802d4d58
    void initFunc_Slow();                               // 0x802d4ebc
    void doFunc_Slow();                                 // 0x802d4ec0
    void initFunc_Slow2Fast();                          // 0x802d4f08
    void doFunc_Slow2Fast();                            // 0x802d4f14
    void initFunc_Fast();                               // 0x802d5050
    void doFunc_Fast();                                 // 0x802d5054
    void initFunc_Fast2Slow();                          // 0x802d509c
    void doFunc_Fast2Slow();                            // 0x802d50a8

    static StateFuncSet<TMapObjVentilator> sTable[4];   // 0x803aab00

    // Inline/Unused
    //static const scFastRotRad;
    //static const scSlowRotRad;
    //static const scChangeRotSpeed;
    //static const scSlowFrame;
    //static const scFastFrame;
    //static const scMinSoundRate;
    //static const scMaxSoundRate;

    JGeometry::TQuat4f _158;
    JGeometry::TQuat4f _168;
    JGeometry::TQuat4f _178;
    f32 _188;
}; // class MapObjVentilator

#endif // MAPOBJVENTILATOR_H
