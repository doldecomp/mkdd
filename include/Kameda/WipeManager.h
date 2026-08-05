#ifndef WIPEMANAGER_H
#define WIPEMANAGER_H

#include <JSystem/JKernel/JKRHeap.h>
#include <JSystem/JUtility/TColor.h>

#include "Kameda/Fade.h"
#include "Kameda/Mario.h"
#include "Kameda/Task.h"
#include "Kaneshige/HioMgr.h"
#include "Shiraiwa/Wipe.h"

// Not sure if this is correct
class WipeParam : public JORReflexible
{
public:
    // Inline/Unused
    WipeParam();
    virtual ~WipeParam() {}

    // fabricated
    bool isDrawEnabled() const { return mDrawFlag & 1; }

    u8 _4;
    u8 mDrawFlag;
    u8 _6;
    u8 _7;
    u8 _8;
    u8 _9;
    u16 _a;
    u16 _c;
    u16 _e;
    u16 _10;
};

class WipeHioNode : public HioNode
{
public:
    virtual ~WipeHioNode() {}
    WipeHioNode() : HioNode("ワイプ表示", &mParam, 0, 0) {}

    WipeParam mParam;
};

class WipeManager
{
public:
    WipeManager(JKRHeap *);                                             // 0x80138720
    void reset();                                                       // 0x80138b38
    void draw();                                                        // 0x80138bc0
    void drawIn(int kart, int status, int);                             // 0x80138e34
    void drawOut(int kart, int status, int);                            // 0x80139534
    void calc();                                                        // 0x80139c28
    void calcIn(int kart, int status, int);                             // 0x80139d5c
    void calcOut(int kart, int status, int);                            // 0x8013a400
    int getWipeState(int kart);                                         // 0x8013aa98
    void setWipe(int kart, int, short, short, short, JUtility::TColor); // 0x8013ab30
    
    // The tasks
    static void initPre();                                              // 0x8013ad30
    static void preWipe();                                              // 0x8013ae04
    static void initMain();                                             // 0x8013aecc
    static void mainWipe();                                             // 0x8013afa0
    static void initAfter();                                            // 0x8013b068
    static void afterWipe();                                            // 0x8013b13c

    // Unused
    ~WipeManager();

    void killWipe(int idx) { mKillFlag[idx] = true; }
    void startWipe(int idx) { mMoveFlag[idx] = true; }
    void stopWipe(int idx) { mMoveFlag[idx] = false; }
    static WipeManager *getManager() { return mThis; }

private:
    static WipeManager *mThis; // 0x804162c0
    static Task *mTask;        // 0x804162c4
    static bool mMoveFlag[8];  // 0x804162c8
    static bool mKillFlag[8];  // 0x804162d0

    class
    {
    public:
        void reset()
        {
            mWipeType = 0;
            mWipeState = 6;
            _8 = 0;
            _a = 0;
            _c = 0;
            _e = 0;
            mColor.set(0, 0, 0, 0);
        }

        // fabricated inlines, only setColor should be needed?(fixed something in setWipe)
        void setWipeType(int type) { mWipeType = type; }
        void setColor(JUtility::TColor color)
        {
            mColor.r = color.r;
            mColor.g = color.g;
            mColor.b = color.b;
            mColor.a = color.a;
        }

        void setPreMainAfter(s16 pre, s16 main, s16 after)
        {
            _a = pre;
            _c = main;
            _e = after;
        }

        int mWipeType;
        int mWipeState;
        s16 _8;
        s16 _a; // pre
        s16 _c; // main
        s16 _e; // after
        JUtility::TColor mColor;
    } _0[8];
    int mScrnCount;
    WipeHioNode *mNode;
    Fade *mFade;
    Mario *mMario;
    TWipeSlide *mWipeSlide;
    TWipeCheck *mWipeCheck;
    TWipeShut *mWipeShut;
    TWipeLineA *mWipeLineA;
    TWipeLineB *mWipeLineB;
    TWipeWaveA *mWipeWaveA;
    TWipeCircle *mWipeCircle;
    TWipeCurtain *mWipeCurtain;
    int mKartNum;
}; // class WipeManager

inline WipeManager *GETWipeManager() { return WipeManager::getManager(); }

#endif // WIPEMANAGER_H
