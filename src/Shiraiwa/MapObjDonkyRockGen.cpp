#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Shiraiwa/Objects/MapObjDonkyRock.h"
#include "Shiraiwa/Objects/MapObjDonkyRockGen.h"
#include "types.h"

TMapObjDonkyRockGen::TMapObjDonkyRockGen(const CrsData::SObject &sObject) : GeographyObj(sObject) {
    mSpawnInterval = 0;
    mSpawnTimer = 0;
    mRocks = nullptr;

    #line 26
    JUT_ASSERT_MSG(mObjData->mPathID != 0xffff, "TMapObjDonkyRockGen: Illegal Path ID");

    int rockCount = sObject.mParam1;
    if (rockCount < 1) {
        rockCount = 1;
    }
    makeRockObject(rockCount);
}

TMapObjDonkyRockGen::~TMapObjDonkyRockGen() {
    delete []mRocks;
}

void TMapObjDonkyRockGen::reset() {
    resetObject();

    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();

    mSpawnInterval = mObjData->mParam2;

    if (mSpawnInterval < 1) {
        mSpawnInterval = 60;
    }

    mSpawnTimer = 0;
}

void TMapObjDonkyRockGen::calc() {
    if (mSpawnTimer > mSpawnInterval) {
        if (appearRock()) {
            mSpawnTimer = 0;
        }
    } else {
        mSpawnTimer++;
    }
}

bool TMapObjDonkyRockGen::appearRock() {
    bool appeared = false;

    for (int i = 0; i < mRockCount; i++) {
        TMapObjDonkyRock *rock = mRocks[i];

        if (rock && rock->getState() == 0) {
            rock->start();
            appeared = true;
            break;
        }
    }

    return appeared;
}

void TMapObjDonkyRockGen::makeRockObject(int count) {
    mRockCount = count;
    mRocks = new TMapObjDonkyRock *[mRockCount];
    for (int i = 0; i < mRockCount; i++) {
        mRocks[i] = (TMapObjDonkyRock *)GetGeoObjMgr()->createSubObj(0x1197);
        mRocks[i]->setObjData(mObjData);
    }
}

void TMapObjDonkyRockGen::createColModel(J3DModelData *) {}

#include "JSystem/JAudio/JASFakeMatch2.h"
