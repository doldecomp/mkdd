#ifndef PAUSEMANAGER_H
#define PAUSEMANAGER_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kameda/Pause2D.h"
#include "Kameda/PrintMemoryCard.h"
#include "Kameda/Result2D.h"
#include "Osako/kartPad.h"
#include "types.h"

class PauseManager
{
public:
    PauseManager(JKRHeap *heap);  // 0x801343d0
    ~PauseManager();
    void reset();  // 0x801345fc
    void setPause();
    void clrPause();
    void draw();             // 0x801346d0
    void exec();             // 0x801348f8
    static bool tstPause();  // 0x80135288
    void wipeIn();
    void wipeOut(int);            // 0x80135290
    static int getPauseChoice();  // 0x801352d0
    void getCharRankPos(int);
    void setTA();  // 0x8013541c

    void setResultStartFlag(bool flag) { mResultStart = flag; }
    void setGameOverFlag(bool flag) { mGameOver = flag; }
    void setExecFlag(bool flag) { mExec = flag; }
    void setPauseEndFlag(bool flag) { mPauseEnd = flag; }

    void enablePause() { _14 = 0; }

    static PauseManager *getManager() { return mThis; }

    bool isResultStart() const { return mResultStart; }

private:
    static PauseManager *mThis;     // 0x80416298
    static bool mIsPause;           // 0x8041629c
    static int mPauseSelector;      // 0x804162a0
    static int mPauseChoice;        // 0x804162a4
    static int mPauseNextChoice;    // 0x804162a8
    static KartGamePad *mPausePad;  // 0x804162ac

    bool mNetFlag;                      // 0x00
    u8 _1[3];                           // 0x01
    JKRArchive *mArchive;               // 0x04
    PrintMemoryCard *mPrintMemoryCard;  // 0x08
    u8 _c;                              // 0x0c
    u8 _d;                              // 0x0d
    bool mResultStart;                  // 0x0e
    bool _f;                            // 0x0f
    bool mGameOver;                     // 0x10
    u8 _11;                             // 0x11
    u8 _12;                             // 0x12
    bool mExec;                         // 0x13
    s32 _14;                            // 0x14
    bool mPauseEnd;                     // 0x18
    u8 _19;                             // 0x19
    u8 _1a;                             // 0x1a
    u8 _1b;                             // 0x1b
    Pause2D *mPause2D;                  // 0x1c
    Result2D *mResult2D;                // 0x20
    J2DOrthoGraph *mOrthoGraph;         // 0x24
    s32 _28;                            // 0x28
    s32 mWipeOutFrame;                  // 0x2c
    s32 mWipeOutDuration;               // 0x30
    u32 _34;                            // 0x30
    bool _38;                           // 0x38
    u8 _39[3];                          // 0x39
};

inline PauseManager *GETPauseManager()
{
    return PauseManager::getManager();
}

#endif
