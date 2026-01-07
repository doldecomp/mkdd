#include "Osako/LANPlayInfo.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Osako/ResMgr.h"
#include "kartEnums.h"

LANPlayInfo gLANPlayInfo;

LANPlayInfo::LANPlayInfo() : mRnd(0) {

}

void LANPlayInfo::init() {}

ECourseID LANPlayInfo::getCrsID() {}

ResMgr::CourseOrder LANPlayInfo::getCrsOrder() {
    return ResMgr::mcOrder_1;
}

void LANPlayInfo::resumeInfo(LANSelectMode *) {}

void LANPlayInfo::saveInfo(LANSelectMode *) {}

u8 LANPlayInfo::getRandomMap(int max, u32 map) {
    if ((((mMapID) & (( 1 << max ) - 1) ) == (( 1 << max ) - 1))) {
        mMapID = map;
    }
    u8 ret = getMaskedRandom(max, mMapID);
    mMapID |= (1 << ret);
    return ret;
}

void LANPlayInfo::shuffleKart(int playerCount) {

}

u8 LANPlayInfo::getMaskedRandom(int max, u32 mask) {
    JUT_ASSERT(!(((mask) & (( 1 << max ) - 1) ) == (( 1 << max ) - 1)));
    
    u8 ret = 0;
    const int n = max * mRnd.get_ufloat_1() + 1;

    for (int i = 0; i < n; i++) {
        do {
            if (++ret >= max) {
                ret = 0;
            }
        } while (mask & (1 << ret));
    }
    return ret;
}

void LANPlayInfo::getRideKart(u8) {}

void LANPlayInfo::getFront(int) {}

void LANPlayInfo::getBack(int) {}

int LANPlayInfo::getDisplayNum() {
    switch (mDispIdx) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 4;
    default:
        return 0;
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
