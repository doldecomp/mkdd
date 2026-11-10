#ifndef MAPOBJPEACHTREE_H
#define MAPOBJPEACHTREE_H

#include "Sato/Objects/GeoTree.h"


class TMapObjPeachTree : public GeoMarioTree1 {
public:
    TMapObjPeachTree(const CrsData::SObject&);
    virtual ~TMapObjPeachTree();

    virtual const char* getBmdFileName();
    virtual void createColModel(J3DModelData*);
    virtual GeoAnmTableEntry *getAnmTbl();
    virtual u16 getSizeAnmTbl();

    static GeoAnmTableEntry sBckAnmTable;
};

class TMapObjPeachTree2 : public GeoMarioTree1 {
public:

    TMapObjPeachTree2(const CrsData::SObject&);
    virtual ~TMapObjPeachTree2();

    virtual const char* getBmdFileName();
    virtual void createColModel(J3DModelData*);
    virtual GeoAnmTableEntry *getAnmTbl();
    virtual u16 getSizeAnmTbl();

    static GeoAnmTableEntry sBckAnmTable;
};

#endif // MAPOBJPEACHTREE_H
