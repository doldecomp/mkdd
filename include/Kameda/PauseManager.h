#ifndef PAUSEMANAGER_H
#define PAUSEMANAGER_H

#include "JSystem/JKernel/JKRHeap.h"
#include "Osako/kartPad.h"
#include "types.h"

class PauseManager {
public:
    PauseManager(JKRHeap *);     // 0x801343d0
    void reset();                // 0x801345fc
    void draw();                 // 0x801346d0
    void exec();                 // 0x801348f8
    static bool tstPause();      // 0x80135288
    void wipeOut(int);           // 0x80135290
    static int getPauseChoice(); // 0x801352d0

    void setResultStartFlag(bool flag) {mResultStart = flag; }
    void setGameOverFlag(bool flag) { mGameOver = flag; }
    void setExecFlag(bool flag) { mExec = flag; }
    void setPauseEndFlag(bool flag) { mPauseEnd = flag; }
    
    void enablePause() { _14 = 0; }

    static PauseManager *getManager() {
        return mThis;
    }

private:
    static PauseManager *mThis;    // 0x80416298
    static bool mIsPause;          // 0x8041629c
    static int mPauseSelector;     // 0x804162a0
    static int mPauseChoice;       // 0x804162a4
    static int mPauseNextChoice;   // 0x804162a8
    static KartGamePad *mPausePad; // 0x804162ac

    u8 _0[0xe];
    bool mResultStart;
    bool _f;
    bool mGameOver;
    u8 _11[0x13 - 0x11];
    bool mExec;
    u32 _14;
    bool mPauseEnd;
    u8 _19[0x3c - 0x19];
};

inline PauseManager *GETPauseManager() { return PauseManager::getManager(); }

#endif
