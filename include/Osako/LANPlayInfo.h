#ifndef LANPLAYINFO_H
#define LANPLAYINFO_H

#include "JSystem/JMath/JMath.h"
#include "Osako/LANSelectMode.h"
#include "Osako/ResMgr.h"
#include "kartEnums.h"
#include "macros.h"
#include "types.h"

class LANPlayInfo {
public:
    void init();                       // 0x80218580
    ECourseID getCrsID();              // 0x8021865c
    ResMgr::CourseOrder getCrsOrder(); // 0x80218904
    void resumeInfo(LANSelectMode *);  // 0x8021890c
    void saveInfo(LANSelectMode *);    // 0x802189a4
    u8 getRandomMap(int, u32);       // 0x80218a48
    void shuffleKart(int);             // 0x80218ab4
    u8 getMaskedRandom(int, u32);     // 0x80218be4
    void getRideKart(u8);              // 0x80218d10
    void getFront(int);                // 0x80218ec4
    void getBack(int);                 // 0x80218ee4
    int getDisplayNum();              // 0x80218f04
    // Inline/Unused
    LANPlayInfo();

private:
    PLACEHOLDER_BYTES(0, 7);
    u8 mDispIdx;
    u8 mValueMode;
    PLACEHOLDER_BYTES(9, 0xc);
    u32 mMapID;
    PLACEHOLDER_BYTES(0x10, 0x14);
    u32 _14;
    JMARandFast mRnd;
    struct {
        u8 back;
        u8 front;
        u8 pos;
    } _1c[8];
    PLACEHOLDER_BYTES(0x34, 0x3c);
    u8 _3c[8];
};
extern LANPlayInfo gLANPlayInfo;

#endif // LANPLAYINFO_H
