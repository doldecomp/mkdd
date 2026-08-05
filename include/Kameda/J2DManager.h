#ifndef J2DMANAGER_H
#define J2DMANAGER_H

#include "JSystem/JKernel/JKRHeap.h"

#include "Kameda/Demo2D.h"
#include "Kameda/Race2D.h"
#include "Kameda/WipeManager.h"

#include "types.h"

// TODO: implement these
class PreRace2D;
class Go3212D;
class Go2D;
class Goal2D;
class LANNum2D;
class LANResult2D;

class J2DManager
{
public:
    J2DManager(JKRHeap *);          // 0x8013156c
    void reset();                   // 0x801317cc
    void draw();                    // 0x801319bc
    void calc();                    // 0x80131a94
    void calcParticle();            // 0x80131c10
    static int getKart2Status(int); // 0x80131c68
    static int getStatus2Kart(int); // 0x80131d00
    void setStatus2Kart(int, int);  // 0x80131d98
    void setUsisi(int, ECharID);    // 0x80131f40
    void startPreRace();            // 0x80132034
    int getPreRaceStatus();         // 0x80132058
    int getPreRaceWipeType();       // 0x80132064
    void setRace2DDrawFlag(bool);   // 0x80132070
    void hideRace2D();              // 0x80132084
    void getHideFrameRace2D();      // 0x80132094
    bool isHideRace2D();            // 0x801320a0
    bool isRaceEnd();               // 0x801320c0
    bool isGoalAnmEnd(int);         // 0x80132120
    void getGoalAnmFrame(int);      // 0x801321b0
    bool isAlarm(int);              // 0x801321c4
    void startLANNumAnm();          // 0x801321e8
    void startLANNumResultAnm();    // 0x8013220c
    void startLANResultAnm(int);    // 0x80132230
    void startBombDemo();           // 0x80132254
    void endBombDemo();             // 0x80132278
    bool isLANDemo();               // 0x80132288
    bool isStartGo321();            // 0x801322b4

    // Inline/Unused
    ~J2DManager();
    bool isMiniGameAnmEnd(int);

    void startWinnerAnm(int idx) { mWinnerAnmFlag[idx] = true; }
    void startLoserAnm(int idx) { mLoserAnmFlag[idx] = true; }
    void setDrawFlag(bool flag) { mDrawFlag = flag; }

    JKRArchive *getArchive() const { return mArchive; }

    static J2DManager *getManager() { return mThis; }

    static const char *mKartNumberName[8]; // 0x80394378
    static int mKart2Status[8];            // 0x803fb9a0
    static int mStatus2Kart[8];            // 0x803fba10
    static J2DManager *mThis;              // 0x80416288
    static bool mNetFlag;                  // 0x8041628c

    JKRArchive *mArchive;
    J2DOrthoGraph *mOrtho;
    bool mDrawFlag;
    bool mInit;
    int mAnmFrame;
    bool mWinnerAnmFlag[8];
    bool mLoserAnmFlag[8];
    int mDuration;
    Demo2D *mDemo2D;
    PreRace2D *mPreRace;
    Go3212D *mGo321;
    Go2D *mGo;
    Race2D *mRace;
    Goal2D *mGoal;
    LANNum2D *mLANNumAnm;
    LANResult2D *mLANResultAnm;
    WipeManager *mWipeMgr;
}; // Size 0x48;

inline J2DManager *GETJ2DManager() { return J2DManager::getManager(); }

#endif
