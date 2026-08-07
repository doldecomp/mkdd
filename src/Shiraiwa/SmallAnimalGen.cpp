#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObjMgr.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/SmallAnimalGen.h"


TSmallAnimalGen::TSmallAnimalGen(const CrsData::SObject &sObject, u32 id) : TMapObjHioNode(sObject) {
    u16 objectCount = sObject.mParam1;
    _14c = nullptr;
    _150 = 0;
    _154 = id;
    if (objectCount == 0) {
        objectCount = 1;
    }
    makeObjects((u16)objectCount);
}

TSmallAnimalGen::~TSmallAnimalGen() {
    for (int i = 0; i < _150; i++) {
        delete _14c[i];
    }
    delete[] _14c;
}

void TSmallAnimalGen::reset() {}

void TSmallAnimalGen::calc() {}

void TSmallAnimalGen::makeObjects(u16 objCount) {
    _150 = objCount;
    _14c = new TSmallAnimal *[_150];

    for (int i = 0; i < _150; i++) {
        _14c[i] = (TSmallAnimal *)GetGeoObjMgr()->createSubObj(_154);
        _14c[i]->setOrigin(&mPos);
    }
}


TSmallAnimal::TSmallAnimal(u32 id) : TMapObjHioNode(id) {
    mSmallAnimalPos = nullptr;
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
}

TSmallAnimal::~TSmallAnimal() {}

void TSmallAnimal::reset() {
    resetObject();
}

void TSmallAnimal::setOrigin(JGeometry::TVec3f *newOrigin) {
    mSmallAnimalPos = newOrigin;
    mPos.set(*mSmallAnimalPos);
}

#include "JSystem/JAudio/JASFakeMatch2.h"
