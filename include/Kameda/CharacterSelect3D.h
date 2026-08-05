#ifndef CHARACTERSELECT3D_H
#define CHARACTERSELECT3D_H

#include "JSystem/JGeometry.h"
#include "JSystem/JKernel/JKRHeap.h"

#include "Kaneshige/RaceMgr.h"

class CharacterSelect3DParam // seems to inherit JORReflexible
{
public:
    CharacterSelect3DParam();
    virtual ~CharacterSelect3DParam() {}
};

class CharacterSelect3D // oh great, unnamed classes
{
public:
    enum CHARACTER_STATUS
    {
        STATUS_0,
        STATUS_1
        // TODO
    };

    CharacterSelect3D(JKRHeap *);                                                     // 0x80183a60
    void init();                                                                      // 0x80184910
    ~CharacterSelect3D();                                                             // 0x801849f0
    void draw(int, f32);                                                              // 0x80184a44
    void draw(int, f32, JGeometry::TVec3f, JGeometry::TVec3f);                        // 0x80184e60
    void calc();                                                                      // 0x801851d0
    void setCharacter(int, int, ECharID, JGeometry::TVec3f, JGeometry::TVec3f, f32);  // 0x80185b58
    void setCharacterStatus(int player, int position, CHARACTER_STATUS);              // 0x80185d20
    void setKart(int, EKartID, JGeometry::TVec3f, JGeometry::TVec3f, f32);            // 0x80185ef0
    void isCancel(int);                                                               // 0x80186054
    void isNext(int);                                                                 // 0x801860a8
    MtxPtr getCameraMatrix();                                                           // 0x801860fc
    f32 getFovy();                                                                    // 0x80186104
    void calcCharacter(int, int, ECharID, JGeometry::TVec3f, JGeometry::TVec3f, f32); // 0x80186114
    void drawKart(EKartID);                                                           // 0x80186938
    void calcRotTrans(Mtx, JGeometry::TVec3f, JGeometry::TVec3f);                     // 0x80186b90

    static CharacterSelect3D *ptr() { return mCharacterSelect3D; }

    static CharacterSelect3D *create(JKRHeap *heap) {
        if (mCharacterSelect3D == nullptr) {
            mCharacterSelect3D = new (heap, 0) CharacterSelect3D(heap);
        }
        return mCharacterSelect3D;
    }

    static const f32 mCharacterDefScale[20];      // 0x80372ec0
    static const f32 mKartDefScale[21];           // 0x80372f10
    static const f32 mMarioBody[9];               // 0x80372f64
    static const f32 mDkBody[9];                  // 0x80372f88
    static const f32 mYoshiBody[9];               // 0x80372fac
    static const f32 mNokoBody[9];                // 0x80372fd0
    static const f32 mPeachBody[9];               // 0x80372ff4
    static const f32 mBabyBody[9];                // 0x80373018
    static const f32 mWarioBody[9];               // 0x8037303c
    static const f32 mKoopaBody[9];               // 0x80373060
    static const f32 mDiddyBody[9];               // 0x80373084
    static const f32 mKoopaJrBody[9];             // 0x803730a8
    static const f32 mKinopioBody[9];             // 0x803730cc
    static const f32 mCatherineBody[9];           // 0x803730f0
    static const f32 mWaluigiBody[9];             // 0x80373114
    static const f32 mKingTeresaBody[9];          // 0x80373138
    static const f32 mBonusBody[9];               // 0x8037315c
    static const f32 mMarioArm[4][9];             // 0x80373180
    static const f32 mDkArm[4][9];                // 0x80373210
    static const f32 mYoshiArm[4][9];             // 0x803732a0
    static const f32 mNokoArm[4][9];              // 0x80373330
    static const f32 mPeachArm[4][9];             // 0x803733c0
    static const f32 mBabyArm[4][9];              // 0x80373450
    static const f32 mWarioArm[4][9];             // 0x803734e0
    static const f32 mKoopaArm[6][9];             // 0x80373570
    static const f32 mDiddyArm[6][9];             // 0x80373648
    static const f32 mKoopaJrArm[4][9];           // 0x80373720
    static const f32 mKinopioArm[4][9];           // 0x803737b0
    static const f32 mCatherineArm[4][9];         // 0x80373840
    static const f32 mWaluigiArm[4][9];           // 0x803738d0
    static const f32 mKingTeresaArm[4][9];        // 0x80373960
    static const f32 mBonusArm[4][9];             // 0x803739f0
    static const f32 mMarioDump[4][9];            // 0x80373a80
    static const f32 mDkDump[4][9];               // 0x80373b10
    static const f32 mYoshiDump[4][9];            // 0x80373ba0
    static const f32 mNokoDump[4][9];             // 0x80373c30
    static const f32 mPeachDump[4][9];            // 0x80373cc0
    static const f32 mBabyDump[4][9];             // 0x80373d50
    static const f32 mWarioDump[4][9];            // 0x80373de0
    static const f32 mKoopaDump[6][9];            // 0x80373e70
    static const f32 mKoopaJrDump[4][9];          // 0x80373f48
    static const f32 mCatherineDump[4][9];        // 0x80373fd8
    static const f32 mWaluigiDump[4][9];          // 0x80374068
    static const f32 mKingTeresaDump[4][9];       // 0x803740f8
    static const f32 mBonusDump[4][9];            // 0x80374188
    static const f32 mMarioTire[4][9];            // 0x80374218
    static const f32 mDkTire[4][9];               // 0x803742a8
    static const f32 mYoshiTire[4][9];            // 0x80374338
    static const f32 mNokoTire[4][9];             // 0x803743c8
    static const f32 mPeachTire[4][9];            // 0x80374458
    static const f32 mBabyTire[4][9];             // 0x803744e8
    static const f32 mWarioTire[4][9];            // 0x80374578
    static const f32 mKoopaTire[6][9];            // 0x80374608
    static const f32 mDiddyTire[6][9];            // 0x803746e0
    static const f32 mKoopaJrTire[4][9];          // 0x803747b8
    static const f32 mKinopioTire[4][9];          // 0x80374848
    static const f32 mCatherineTire[4][9];        // 0x803748d8
    static const f32 mWaluigiTire[4][9];          // 0x80374968
    static const f32 mKingTeresaTire[4][9];       // 0x803749f8
    static const f32 mBonusTire[4][9];            // 0x80374a88
    static void *mDiddyDump;                      // 0x80416358
    static void *mKinopioDump;                    // 0x8041635c
    static CharacterSelect3D *mCharacterSelect3D; // 0x80416360
private:
    u8 _0[0x1018];
}; // Size: 0x1018

#endif // CHARACTERSELECT3D_H
