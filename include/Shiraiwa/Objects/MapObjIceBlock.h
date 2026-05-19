#ifndef MAPOBJICEBLOCK_H
#define MAPOBJICEBLOCK_H

#include "Sato/GeographyObj.h"

class AnmController;
class AnmControlTrans;

class TMapObjIceBlock : public TMapObjHioNode {
public:
    TMapObjIceBlock(const CrsData::SObject &); // 0x802b7944
    ~TMapObjIceBlock(); // 0x802b79bc

    virtual void loadAnimation();
    virtual void createModel(JKRSolidHeap *, u32); // 0x802caf68
    virtual void reset(); // 0x802cae9c
    virtual void calc(); // 0x802cafa0
    virtual const char *getBmdFileName(); // 0x802b7a4c

    static J3DAnmTransform* sIceBlockBckAnmTrans;
    static J3DMtxCalc* sIceBlockBckMtxCalc;
}; // class TMapObjIceBlock
#endif // MAPOBJICEBLOCK_H
