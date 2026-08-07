#ifndef MAPOBJHEYHO_H
#define MAPOBJHEYHO_H

#include "JSystem/J3D/J3DAnmTextureSRTKey.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRDisposer.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObj.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/AnmPlayer.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"


class TMapObjHeyho;

class THeyhoSupervisor : public GeoObjSupervisor {
public:
    THeyhoSupervisor() : GeoObjSupervisor() {
        _24.initiate();
    }
    virtual ~THeyhoSupervisor() {}
    virtual void calc();
    virtual void reset() {};

    // void THeyhoSupervisor::entry(TMapObjHeyho *);

    JSUList<TMapObjHeyho> _24;
};


class TMapObjSubHeyho : public GeographyObj, public StateObserver {
    public:
    TMapObjSubHeyho();
    virtual ~TMapObjSubHeyho() {}; // 0x802b3d98
    virtual void loadAnimation();
    virtual void createModel(JKRSolidHeap *, u32);
    virtual void reset();
    virtual void calc();
    virtual void update();
    virtual const char *getBmdFileName();
    virtual const char *getShadowBmdFileName();
    virtual void createColModel(J3DModelData *);
    virtual void MoveExec(); // 0x802b33dc
    virtual void InitExec(); // 0x802b3370
    bool checkItemHitting();
    void initFunc_Skate();
    void doFunc_Skate();
    void makeFrontMtx(JGeometry::TPos3f *);
    void initFunc_Spin();
    void doFunc_Spin();
    void initFunc_Recover();
    void doFunc_Recover();

    static StateFuncSet<TMapObjSubHeyho> sTable[3];
    static TAnmInfo sAnmInfos[1];
    static J3DAnmTexPattern *sHeyhoBtpAnm;
    // void TMapObjSubHeyho::scRecoverSpeed;

    J3DAnmObjMaterial mAnmObj;          // 0x158
    JGeometry::TVec3f *mSubHeyhoPos;    // 0x178
    JGeometry::TVec3f *mSubHeyhoVel;    // 0x17c
    int _180;                           // 0x180 - Unknown
    JGeometry::TPos3f *mSubHeyhoRot;    // 0x184
    JGeometry::TVec3f _188;             // 0x188 - scale?
    JGeometry::TPos3f _194;             // 0x194
    f32 _1c4;                           // 0x1c4
};



class TMapObjHeyho : public TMapObjHioNode, public StateObserver, JKRDisposer {
public:
    TMapObjHeyho(const CrsData::SObject &);             // 0x802b1718
    virtual ~TMapObjHeyho();                            // 0x802b1930
    virtual void MoveExec();                            // 0x802b2118
    virtual void InitExec();                            // 0x802b20ac
    virtual void reset();                               // 0x802b1a70
    virtual void calc();                                // 0x802b1c4c
    void callWalkSound();                               // 0x802b1ccc
    bool checkItemHitting();                            // 0x802b1dcc
    void checkJump(u16);                                // 0x802b1ec0
    virtual const char *getBmdFileName();               // 0x802b1f1c
    virtual const char *getShadowBmdFileName();         // 0x802b1f44
    virtual void loadAnimation();                       // 0x802b1f6c
    virtual void createModel(JKRSolidHeap *, u32);      // 0x802b1fcc
    virtual void createColModel(J3DModelData *);        // 0x802b2044
    virtual void update();                              // 0x802b2184
    void changeAllState(u16);                           // 0x802b21ec
    virtual void doKartColCallBack(int);                // 0x802b22b0
    void initFunc_Skate();                              // 0x802b2360
    void doFunc_Skate();                                // 0x802b238c
    void initFunc_Jump();                               // 0x802b23d8
    void doFunc_Jump();                                 // 0x802b2504
    void initFunc_Bomb();                               // 0x802b264c
    void doFunc_Bomb();                                 // 0x802b289c
    void initFunc_BombEnd();                            // 0x802b2ad4
    void doFunc_BombEnd();                              // 0x802b2ae0
    void initFunc_Sink();                               // 0x802b2d38
    void doFunc_Sink();                                 // 0x802b2d78
    void initFunc_Recover();                            // 0x802b2ea8
    void doFunc_Recover();                              // 0x802b2ed4
    void initFunc_Die();                                // 0x802b2fd0
    void doFunc_Die();                                  // 0x802b301c
    void hitCheck(TMapObjHeyho *);                      // 0x802b3020

    static TAnmInfo sAnmInfos[1];                       // 0x803a6d78
    static StateFuncSet<TMapObjHeyho> sTable[7];        // 0x803a6e34
    static f32 sGravity;                                // 0x804152c8
    static f32 sAccel;                                  // 0x804152cc
    static f32 sJumpVel;                                // 0x804152d0
    static f32 sJumpGrv;                                // 0x804152d4
    static f32 sRotSpeed;                               // 0x804152d8
    static f32 sFirstSinkVel;                           // 0x804152dc
    static f32 sSinkAccel;                              // 0x804152e0
    static f32 sBombVelH;                               // 0x804152e4
    static f32 sBombVelV;                               // 0x804152e8
    static f32 sSlideVelH;                              // 0x804152ec
    static f32 sSlideVelV;                              // 0x804152f0
    static f32 sBombRotSpeed;                           // 0x804152f4
    static f32 sBombEndRotDecel;                        // 0x804152f8
    static f32 sAirFriction;                            // 0x804152fc
    static THeyhoSupervisor *sSupervisor;               // 0x80416f68
    static J3DAnmTexPattern *sHeyhoBtpAnm;              // 0x80416f6c
    static const s8 sAnmTable[7];                       // 0x8041cd48

    // Inline/Unused
    //void JSUList<TMapObjHeyho>::~JSUList();
    //void JSULink<TMapObjHeyho>::~JSULink();
    //void scColorTypeNum;
    //void scLeftDownFrame;
    //void scRightDownFrame;

    // Inline
    virtual THeyhoSupervisor *getSupervisor() { return sSupervisor; } // 0x802b1928

    JSULink<TMapObjHeyho> mLinkHeyho;   // 0x170
    J3DAnmObjMaterial mAnmObj;          // 0x180
    TAnmPlayer mAnmPlayer;              // 0x1a0
    TPathWalk *mPathWalk;               // 0x1b8
    TMapObjSubHeyho *_1bc;              // 0x1bc
    JGeometry::TVec3f mHeyhoPos;        // 0x1c0
    JGeometry::TVec3f mHeyhoVel;        // 0x1cc
    JGeometry::TPos3f mHeyhoRot;        // 0x1d8
    JGeometry::TPos3f _208;             // 0x208
    f32 _238;                           // 0x238
    f32 mJumpVel;                       // 0x23c
    f32 mSinkVel;                       // 0x240
    f32 _244;                           // 0x244
    f32 _248;                           // 0x248
    f32 _24c;                           // 0x24c
    f32 _250;                           // 0x250

};

#endif // MAPOBJHEYHO_H
