#ifndef MAPOBJSUN_H
#define MAPOBJSUN_H

#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjSun : public TMapObjHioNode {
public:
    TMapObjSun(const CrsData::SObject &);               // 0x802b9a40
    virtual ~TMapObjSun();                              // 0x802b9acc
    virtual const char *getBmdFileName();               // 0x802b9b9c
    virtual void reset();                               // 0x802b9bc4
    virtual void createModel(JKRSolidHeap *, u32);      // 0x802b9c40
    virtual void loadAnimation();                       // 0x802b9ca0
    virtual void calc();                                // 0x802b9ce8
    virtual void update();                              // 0x802b9d0c

    static J3DAnmTextureSRTKey *sSunBtkAnm;             // 0x80416ff8

    J3DAnmObjMaterial mAnmObjMat;
}; // class TMapObjSun
#endif // MAPOBJSUN_H
