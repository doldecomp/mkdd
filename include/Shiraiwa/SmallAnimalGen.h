#ifndef TSMALLANIMALGEN_H
#define TSMALLANIMALGEN_H

#include "JSystem/JGeometry/Vec.h"
#include "Kaneshige/Course/CrsData.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"


class TSmallAnimal : public TMapObjHioNode {
public:
    TSmallAnimal(u32 id);
    virtual ~TSmallAnimal();
    virtual void reset();
    virtual const char *getBmdFileName() { return NULL; }   // 0x802a5e40
    virtual void setOrigin(JGeometry::TVec3f *);            // 0x802a5e18

    JGeometry::TVec3f *mSmallAnimalPos;
};


class TSmallAnimalGen : public TMapObjHioNode {
public:
    TSmallAnimalGen(const CrsData::SObject &, u32); // 0x802a5c24
    virtual ~TSmallAnimalGen();                     // 0x802a5ca8
    virtual void reset();                           // 0x802a5d88
    virtual void calc();                            // 0x802a5e3c
    void makeObjects(u16);                          // 0x802a5d8c

    // Inline/Unused
    // void getNthObject(unsigned short);

    TSmallAnimal **_14c;    // GeographyObj **, maybe?
    u16 _150;
    u32 _154;
};


#endif // TSMALLANIMALGEN_H
