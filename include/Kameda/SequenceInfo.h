#ifndef SEQUENCEINFO_H
#define SEQUENCEINFO_H

#include "JSystem/JMath/JMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/RaceTime.h"
#include "Osako/SystemRecord.h"
#include "Osako/kartPad.h"
#include "kartEnums.h"
#include "types.h"

class SequenceInfo
{
public:
    void init(); // 0x801515f0
    void setRandomSeed(); // 0x80151760
    u32 getRandom(); // 0x80151790
    KartGamePad *getDecidePad(); // 0x801517b4
    void setClrGPCourse(); // 0x801517d0
    bool isEndGP(); // 0x80151df0
    bool isGameClr(); // 0x80151e34
    int getPlayerRankGP(); // 0x80151f7c
    int getPad2Player(KartGamePad *); // 0x801520c8
    void setSecretFlag(); // 0x80152110
    void setSecretGameAppear(SystemRecord::GameFlag); // 0x80152890
    void setSecretKartAppear(SystemRecord::SecretKartID); // 0x801528bc
    void rndDemo(u32); // 0x801528e8
    EKartID getDemoKart(ECharID); // 0x80152a4c
    void rndAllCupCourse(); // 0x80152b10
    static const int RANKPOINT[8]; // 0x8036f410
    // Inline/Unused
    SequenceInfo();
    void setGPRank(int, int);

    void setStartNo(int kartNo, int startNo) {
#line 167
        JUT_MINMAX_ASSERT(0, kartNo, 8);
        JUT_MINMAX_ASSERT(0, startNo, 8);
        mStartNo[kartNo] = startNo;
    }

    void set_318_charId(int kart, int driverNo, ECharID charId) {
#line 278
        JUT_MINMAX_ASSERT(0, kart, 2);        
        //JUT_MINMAX_ASSERT(0, kart, 8);
        JUT_MINMAX_ASSERT(0, charId, cCharIDMax);
        _318[kart]._0[driverNo] = charId;
    }

    void set_318_kartId(int kart, EKartID kartId) {
#line 288
        JUT_MINMAX_ASSERT(0, kart, 2);        
        JUT_MINMAX_ASSERT(0, kartId, cKartIDMax);
        _318[kart]._8 = kartId;
    }

    RaceTime get_318_RaceTime(int kart) {
#line 301
        JUT_MINMAX_ASSERT(0, kart, 2);
        return _318[kart].mTime;
    }

    void set_318_RaceTime(int kart, RaceTime time) {
#line 305
        JUT_MINMAX_ASSERT(0, kart, 2);
        _318[kart].mTime = time;
    }

    void set_318_14(int kart, int val) {
#line 313
        JUT_MINMAX_ASSERT(0, kart, 2);
        JUT_MINMAX_ASSERT(0, kart, 8); // compared against the wrong value?
        _318[kart]._14 = val;
    }

    void set_318_Pad(int kart, KartGamePad *pad) {
        _318[kart]._18 = pad;
    }

    void getDemoKart(int no, ECharID &charID1, ECharID &charID2, EKartID &kartID)
    {
#line 413
        JUT_MINMAX_ASSERT(0, no, 8);

        charID1 = mDemoKart[no].char1;
        charID2 = mDemoKart[no].char2;
        kartID = mDemoKart[no].kart;
    }

    void set_354(int val) { _354 =  val; }
    void setRaceLevel(ERaceLevel level) { mLevel = level; }
    void setGhostFlag(u8 flags) { mGhostFlags = flags; }

    void lockGame(int i) { // fabricated
        mGameFlag &= ~(1 << i);
    }

    void lockKart(int i) { // fabricated
        mSecretKart &= ~(1 << i);
    }

    bool tstGameUnlocked(int i) {
        return (mGameFlag & (1 << i)) != 0;
    }

    bool tstSecretKartUnlocked(int i) {
        return (mSecretKart & (1 << i)) != 0;
    }

    bool checkAnyGameUnlocked() {
        return mGameFlag != 0;
    }

    bool checkAnySecretKartUnlocked() {
        return mSecretKart != 0;
    }

    bool checkAnyUnlocked() { // fabricated
        return mGameFlag != 0 || mSecretKart != 0;
    }

private:
    JMARandFast mRnd;          // 00
    int mStartNo[8];           // 004
    KartGamePad *mDecidePad;   // 024
    u8 _28;                    // 028
    int _2c;                   // 02C
    int _30;                   // 030
    u8 mGhostFlags;            // 034
    u8 _35[0x3C - 0x35];       // 035
    int mMapIndex;             // 03C
    int _40[4][4];             // 040
    int _80[4];                // 080
    struct {    
        int cup;               // 090
        int course;            // 094
    } mMapInfo;
    int _98[8][16];            // 098
    int mRankPoint[8];         // 298
    int _2b8[8];               // 2B8
    int _2d8[8];               // 2D8
    int _2f8[8];               // 2F8
    struct {
        ECharID _0[2];     // 00
        EKartID _8;        // 08
        u8 _C[0x10 - 0xc]; // 0C
        RaceTime mTime;    // 10
        int _14;           // 14
        KartGamePad *_18;  // 18
        // Size: 0x1c
    } _318[2];                 // 318
    int _350;                  //
    int _354;                  //
    ERaceLevel mLevel;         //
    u8 _35c;                   //
    u8 _35d;                   //
    u8 _35e;                   //
    u8 _35f;                   //
    u8 _360;                   //
    int _364;                  //
    u16 mGameFlag;             // 368
    u16 mSecretKart;           // 36a
    struct {
        ECharID char1;
        ECharID char2;
        EKartID kart;
        // Size: 0xc
    } mDemoKart[8];            // 36C
    int _3cc[16];              // 3CC
};

extern SequenceInfo gSequenceInfo;

#endif
