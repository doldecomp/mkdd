#ifndef MAPOBJYOSHIHELI_H
#define MAPOBJYOSHIHELI_H

#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjYoshiHeli : public TMapObjHioNode, public StateObserver {
public:
    TMapObjYoshiHeli(const CrsData::SObject &);         // 0x802d53d4
    virtual ~TMapObjYoshiHeli();                        // 0x802d549c
    virtual void reset();                               // 0x802d55a0
    virtual void calc();                                // 0x802d56f4
    virtual void update();                              // 0x802d5794
    virtual void loadAnimation();                       // 0x802d57d8
    virtual void createModel(JKRSolidHeap *, u32);      // 0x802d5820
    virtual void MoveExec();                            // 0x802d58ec
    virtual void InitExec();                            // 0x802d5880
    
    void initFunc_Fly();                                // 0x802d5958
    void doFunc_Fly();                                  // 0x802d5974
    s16 findNearPlayerKart(f32);                        // 0x802d5a7c
    void initFunc_Search();                             // 0x802d5bb4
    void doFunc_Search();                               // 0x802d5bcc
    void initFunc_ForceBack();                          // 0x802d5d54
    void doFunc_ForceBack();                            // 0x802d5d58
    void move(const JGeometry::TVec3f &, bool);         // 0x802d5eb8
    void turn(const JGeometry::TVec3f &);               // 0x802d5f68
    void setFront(const JGeometry::TVec3f &);           // 0x802d62a4
    virtual const char *getBmdFileName() { return "/Objects/YoshiHeli.bmd"; }           // 0x802d670c

    static StateFuncSet<TMapObjYoshiHeli> sTable[3];    // 0x803aacd8
    static f32 sAirFriction;                            // 0x80415538
    static f32 sAccel;                                  // 0x8041553c
    static f32 sTurnMaxSpeed;                           // 0x80415540
    static f32 sTurnMinSpeed;                           // 0x80415544
    static f32 sMinMoveDistance;                        // 0x80415548
    static f32 sLeanVelY;                               // 0x8041554c
    static f32 sFrontTurnMaxSpeed;                      // 0x80415550
    static f32 sFloatHeight;                            // 0x80415554
    static s16 sFloatSpeed;                             // 0x80415558
    static J3DAnmTextureSRTKey *sYoshiHeliBtkAnm;       // 0x80417170

    // Inline/Unused
    void getTargetPos(JGeometry::TVec3f *, u8);
    void getKartPos(u8);
    bool isWall(const JGeometry::TVec3f &);
    
    
    // static sTargetDistance;
    // static sDecelRadius;
    // static sTargetHeight;
    // static sUpPow;
    // static sLeaveUpVel;
    // static sForceBackReachDistance;
    // static sBackUpVel;
    // static sWallBackFrame;
    // static sRotFriction;
    // static const scBoundsRadius;
    // static const scBackHeight;
    // static const scNodeReachRadius;

    J3DAnmObjMaterial mAnmObjMat;           // 0x158
    JGeometry::TVec3f _178;                 // 0x178
    JGeometry::TVec3f mPosHeli;             // 0x184
    JGeometry::TVec3f _190;                 // 0x190
    JGeometry::TVec3f mTurnDir;             // 0x19c
    f32 mSearchRadius;                      // 0x1a8
    f32 _1ac;                               // 0x1ac
    f32 _1b0;                               // 0x1b0
    s16 floatSpeed;                         // 0x1b4
    u8 mTargetKartNo;                       // 0x1b6
    s8 mKartToTrack;                        // 0x1b7
    s16 _1b8;                               // 0x1b8 (+2 bytes padding)
    TPathUtil *mPathUtil;                   // 0x1bc
}; // class MapObjYoshiHeli
#endif // MAPOBJYOSHIHELI_H
