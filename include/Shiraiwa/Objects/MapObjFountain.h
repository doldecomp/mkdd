#ifndef MAPOBJFOUNTAIN_H
#define MAPOBJFOUNTAIN_H

#include "JSystem/J3D/J3DAnmTextureSRTKey.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/J3DAnmObject.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjFountain : public TMapObjHioNode {
public:
    TMapObjFountain(const CrsData::SObject &);          // 0x802b0f6c
    ~TMapObjFountain();                                 // 0x802b100c
    virtual const char *getBmdFileName();               // 0x802b10dc
    virtual void reset();                               // 0x802b1104
    virtual void createModel(JKRSolidHeap *, u32);      // 0x802b1180
    virtual void loadAnimation();                       // 0x802b11bc
    virtual void calc();                                // 0x802b1204
    virtual void update();                              // 0x802b1250

    static J3DAnmTextureSRTKey *sFountainBtkAnm; // 0x80416f58

    J3DAnmObjMaterial mAnmObj;
}; // class MapObjFountain

class TMapObjDemoFountain : public TMapObjFountain {
public: 
    TMapObjDemoFountain(const CrsData::SObject &);
    virtual ~TMapObjDemoFountain();
    virtual void createModel(JKRSolidHeap *, u32);
    virtual void loadAnimation();
    virtual void reset();
    virtual void calc();
    virtual const char *getBmdFileName() { return "/Objects/DPeachFountain.bmd"; }

    static J3DAnmTextureSRTKey *sDemoFountainBtkAnm;
    bool mIsMakeEmt;
};

#endif // MAPOBJFOUNTAIN_H
