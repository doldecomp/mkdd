#ifndef MAPOBJKPFIRE_H
#define MAPOBJKPFIRE_H

#include "JSystem/J3D/J3DAnmTransform.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/J3D/J3DMtxCalc.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjKpFireBall;

class TMapObjKpFire : public TMapObjHioNode, public StateObserver {
public:
    TMapObjKpFire(const CrsData::SObject &);            // 0x802c2c20
    virtual ~TMapObjKpFire();                           // 0x802c2d00
    virtual void MoveExec();                            // 0x802c3c44
    virtual void InitExec();                            // 0x802c3bd8
    virtual void loadAnimation();                       // 0x802c2dc8
    virtual void createModel(JKRSolidHeap *, u32);      // 0x802c2e14
    virtual void reset();                               // 0x802c2f94
    virtual void calc();                                // 0x802c3170
    virtual const char *getBmdFileName() { return "/Objects/KPFire1.bmd"; } // 0x802c4afc

    void initFunc_Start();                              // 0x802c3194
    void doFunc_Start();                                // 0x802c3198
    void initFunc_Walk();                               // 0x802c31d4
    void doFunc_Walk();                                 // 0x802c3210
    void initFunc_Stop();                               // 0x802c34a0
    void doFunc_Stop();                                 // 0x802c34d8
    bool checkFire(u16);                                // 0x802c3778
    void initFunc_FireOpen();                           // 0x802c37bc
    void doFunc_FireOpen();                             // 0x802c384c
    void initFunc_Fire();                               // 0x802c38ec
    void doFunc_Fire();                                 // 0x802c38f0
    void makeFireEmitter(const JGeometry::TVec3f &);    // 0x802c3a08
    const void getFirePos(JGeometry::TVec3f *);              // 0x802c3b00
    void initFunc_FireClose();                          // 0x802c3b38
    void doFunc_FireClose();                            // 0x802c3b3c

    static StateFuncSet<TMapObjKpFire> sTable[6];       // 0x803a8b18
    static f32 sAccel;                                  // 0x804153e8
    static f32 sMaxSpeed;                               // 0x804153ec
    static f32 sShakeAccel;                             // 0x804153f0
    static f32 sShakeMaxSpeed;                          // 0x804153f4
    static s16 sShakeFrame;                             // 0x804153f8
    static s16 sWaitFrame;                              // 0x804153fa
    static s16 sFireInterval;                           // 0x804153fc
    static u8 sFireLineNum;                             // 0x804153fe
    static f32 sShakeMaxLength;                         // 0x80415400
    static const char *scFireJointName;                 // 0x80415404
    static u32 sFireJointNo;                            // 0x80417098
    static J3DAnmTransform *sKpFireBckAnmTrans;         // 0x8041709c
    static J3DMtxCalc *sKpFireBckMtxCalc;               // 0x804170a0

    // Inline/Unused
    //static sAccel_One;
    //static sAccel_Div;
    //static sMaxSpeed_One;
    //static sMaxSpeed_Div;
    //static const scFireFrame;

public:
    TPathUtil *mPathUtil;                       // 0x158
    TMapObjKpFireBall *mMapObjKpFireBall[18];   // 0x15c
}; // class MapObjKpFire


class TMapObjKpFireBall : public TMapObjHioNode, public StateObserver {
public:
    // Outside class members
    TMapObjKpFireBall();
    virtual ~TMapObjKpFireBall(); // 0x802c3d94
    virtual void MoveExec(); // 0x802c4004
    virtual void InitExec(); // 0x802c3f98
    virtual void loadAnimation();
    virtual void createModel(JKRSolidHeap *, u32);
    virtual void reset();
    virtual void calc();
    virtual void update();
    virtual const char *getBmdFileName() { return "/Objects/KPFire1Fire.bmd"; } // 0x802c4af0
    virtual void createColModel(J3DModelData *);
    virtual void doKartColCallBack(int);

    void setVelDir(const JGeometry::TVec3f &);
    void checkItemHitting();
    void fire(const JGeometry::TVec3f &);
    void initFunc_Hide();
    void doFunc_Hide();
    void initFunc_Appear();
    void doFunc_Appear();
    void initFunc_Fly();
    void doFunc_Fly();
    void initFunc_Disappear();
    void makeBreakEmitter(int);
    void doFunc_Disappear();

    static StateFuncSet<TMapObjKpFireBall> sTable[4];
    static s16 sMaxLife;
    static s16 sAppearFrame;
    static f32 sMaxSpeed;
    static f32 sVelocity0;
    static f32 sDecelSpeed;
    static f32 sOrgScale;
    static f32 sRotRadRange;
    static f32 sRotRadMin;
    static J3DAnmTextureSRTKey *sKpFireBallBtkAnm;
    static J3DAnmTevRegKey *sKpFireBallTevRegKey;

public:
    J3DAnmObjMaterial mAnmObjMaterialOne;   // 0x158 - array?
    J3DAnmObjMaterial mAnmObjMaterialTwo;   // 0x178
    JGeometry::TVec3f _198;                 // 0x198
    JPABaseEmitter *mEmitter;               // 0x1a4
};
#endif // MAPOBJKPFIRE_H
