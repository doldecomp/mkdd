#ifndef RIVALCTRLBASE_H
#define RIVALCTRLBASE_H

#include "Sato/stMath.h"
#include "macros.h"
#include "types.h"

class RivalKart;

class RivalCtrlBase
{
public:
    class RankParam
    {
    public:
        RankParam(int);
        void reset();

        int getFirstRank() {
            return _4;
        }
    private:
        int _0;
        int _4;
        int _8;
        f32 _c;
    };

    RivalCtrlBase(RivalKart *);
    void getMyID();
    void getBodyCtrl();
    void getBody();
    stRandom *getRandom();
    void getRankParamID();
    void tstEndRace();
    bool isRival() const;
    void getMyRank();
    // Inline/Unused
    // void getSpeedCtrl();
    // void getItemCtrl();

    virtual ~RivalCtrlBase() {} // 0x8027ea9c

    RivalKart *mRival;
    PLACEHOLDER_BYTES(0x8, 0xc);
};

#endif
