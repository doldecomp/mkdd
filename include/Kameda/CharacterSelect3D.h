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

    static f32 mCharacterDefScale[20];            // 0x80372ec0
    static f32 mKartDefScale[21];                 // 0x80372f10
    static f32 mMarioBody[9];                     // 0x80372f64
    static f32 mDkBody[9];                        // 0x80372f88
    static f32 mYoshiBody[9];                     // 0x80372fac
    static f32 mNokoBody[9];                      // 0x80372fd0
    static f32 mPeachBody[9];                     // 0x80372ff4
    static f32 mBabyBody[9];                      // 0x80373018
    static f32 mWarioBody[9];                     // 0x8037303c
    static f32 mKoopaBody[9];                     // 0x80373060
    static f32 mDiddyBody[9];                     // 0x80373084
    static f32 mKoopaJrBody[9];                   // 0x803730a8
    static f32 mKinopioBody[9];                   // 0x803730cc
    static f32 mCatherineBody[9];                 // 0x803730f0
    static f32 mWaluigiBody[9];                   // 0x80373114
    static f32 mKingTeresaBody[9];                // 0x80373138
    static f32 mBonusBody[9];                     // 0x8037315c
    static f32 mMarioArm[36];                     // 0x80373180
    static f32 mDkArm[36];                        // 0x80373210
    static f32 mYoshiArm[36];                     // 0x803732a0
    static f32 mNokoArm[36];                      // 0x80373330
    static f32 mPeachArm[36];                     // 0x803733c0
    static f32 mBabyArm[36];                      // 0x80373450
    static f32 mWarioArm[36];                     // 0x803734e0
    static f32 mKoopaArm[36];                     // 0x80373570
    static f32 mDiddyArm[36];                     // 0x80373648
    static f32 mKoopaJrArm[36];                   // 0x80373720
    static f32 mKinopioArm[36];                   // 0x803737b0
    static f32 mCatherineArm[36];                 // 0x80373840
    static f32 mWaluigiArm[36];                   // 0x803738d0
    static f32 mKingTeresaArm[36];                // 0x80373960
    static f32 mBonusArm[36];                     // 0x803739f0
    static f32 mMarioDump[36];                    // 0x80373a80
    static f32 mDkDump[36];                       // 0x80373b10
    static f32 mYoshiDump[36];                    // 0x80373ba0
    static f32 mNokoDump[36];                     // 0x80373c30
    static f32 mPeachDump[36];                    // 0x80373cc0
    static f32 mBabyDump[36];                     // 0x80373d50
    static f32 mWarioDump[36];                    // 0x80373de0
    static f32 mKoopaDump[36];                    // 0x80373e70
    static f32 mKoopaJrDump[36];                  // 0x80373f48
    static f32 mCatherineDump[36];                // 0x80373fd8
    static f32 mWaluigiDump[36];                  // 0x80374068
    static f32 mKingTeresaDump[36];               // 0x803740f8
    static f32 mBonusDump[36];                    // 0x80374188
    static f32 mMarioTire[36];                    // 0x80374218
    static f32 mDkTire[36];                       // 0x803742a8
    static f32 mYoshiTire[36];                    // 0x80374338
    static f32 mNokoTire[36];                     // 0x803743c8
    static f32 mPeachTire[36];                    // 0x80374458
    static f32 mBabyTire[36];                     // 0x803744e8
    static f32 mWarioTire[36];                    // 0x80374578
    static f32 mKoopaTire[36];                    // 0x80374608
    static f32 mDiddyTire[36];                    // 0x803746e0
    static f32 mKoopaJrTire[36];                  // 0x803747b8
    static f32 mKinopioTire[36];                  // 0x80374848
    static f32 mCatherineTire[36];                // 0x803748d8
    static f32 mWaluigiTire[36];                  // 0x80374968
    static f32 mKingTeresaTire[36];               // 0x803749f8
    static f32 mBonusTire[36];                    // 0x80374a88
    static f32 mDiddyDump[36];                    // 0x80416358
    static f32 mKinopioDump[36];                  // 0x8041635c
    static CharacterSelect3D *mCharacterSelect3D; // 0x80416360
private:
    u8 _0[0x1018];
}; // Size: 0x1018

#endif // CHARACTERSELECT3D_H
