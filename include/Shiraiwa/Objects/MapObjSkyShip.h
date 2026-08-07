#ifndef MAPOBJSKYSHIP_H
#define MAPOBJSKYSHIP_H

#include "JSystem/J3D/J3DAnmTextureSRTKey.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Osako/shadowModel.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3D.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"


class TMapObjSkyShip : public TMapObjHioNode {
public:
    TMapObjSkyShip(const CrsData::SObject &sObject);        // 0x80298b48
    virtual ~TMapObjSkyShip();                              // 0x80298c30
    virtual ShadowModel::ShadowKind getShadowKind() const { // 0x8029a3dc
        return ShadowModel::cShadowKind_SkyShip;
    }
    virtual void reset();                                   // 0x80298d04
    virtual void calc();                                    // 0x80298f60
    virtual const char *getBmdFileName();                   // 0x80298f10
    virtual const char *getShadowBmdFileName();             // 0x80298f38
    virtual void createColModel(J3DModelData *) {}          // 0x8029a3d8
    virtual f32 getAmplitude() { return sAmplitude; }       // 0x80299258
    virtual s16 getAmpTimer() { return sAmpTimer; }         // 0x80299260
    void moveShadowRot();                                   // 0x802990bc
    void getNodeDir(u16, JGeometry::TVec3f *);              // 0x80299170
    void localMove();                                       // 0x802991cc
    

    static f32 sAmplitude;                                  // 0x80415198
    static s16 sAmpTimer;                                   // 0x8041519c

public:
    u16 _14c;                           // 0x14c
    s16 _14e;                           // 0x14e - unused?
    f32 _150;                           // 0x150
    f32 _154;                           // 0x154
    TPathMove *mPathMove;               // 0x158
    TFreeRotate *mFreeRotate;           // 0x15c
}; // 0x160


class TMapObjPathYoshiHeli : public TMapObjSkyShip {
public:
    TMapObjPathYoshiHeli(const CrsData::SObject &sObject) : TMapObjSkyShip(sObject) {
        createSoundMgr();
    }
    virtual ~TMapObjPathYoshiHeli() {}                  // 0x8029a3e4
    virtual void loadAnimation();                       // 0x802999c4
    virtual void createModel(JKRSolidHeap *, u32);      // 0x80299a0c
    virtual void reset();                               // 0x80299268
    virtual void calc();                                // 0x802994b0
    virtual void update();                              // 0x80299980
    virtual const char *getBmdFileName() { return "/Objects/YoshiHeliB.bmd"; }  // 0x8029a508
    virtual const char *getShadowBmdFileName() { return NULL; }                   // 0x8029a514
    virtual f32 getAmplitude() { return sAmplitude; }   // 0x8029a51c
    virtual s16 getAmpTimer() { return sAmpTimer; }     // 0x8029a524
    void aimVelDir();                                   // 0x80299538

    static f32 sAmplitude;                              // 0x804151a0
    static s16 sAmpTimer;                               // 0x804151a4
    static f32 sReachDistance;              
    static J3DAnmTextureSRTKey *sPathYoshiHeliBtkAnm;

private:
    J3DAnmObjMaterial mAnmObjMaterial;      // 0x160
}; // 0x180


class TMapObjMashBalloon : public TMapObjSkyShip {
public:
    TMapObjMashBalloon(const CrsData::SObject &sObject) : TMapObjSkyShip(sObject) {}
    virtual ~TMapObjMashBalloon() {}                                                            // 0x8029a2cc
    virtual void reset();                                                                       // 0x80299a48
    virtual const char *getBmdFileName() { return "/Objects/mash_balloon.bmd"; }                // 0x8029a3b0
    virtual const char *getShadowBmdFileName() { return "/Objects/mash_balloon_shadow.bmd"; }   // 0x8029a3bc
    virtual f32 getAmplitude() { return sAmplitude; }                                                   // 0x8029a3c8
    virtual s16 getAmpTimer() { return sAmpTimer; }                                                     // 0x8029a3d0

    static f32 sAmplitude;                          // 0x804151ac
    static s16 sAmpTimer;                           // 0x804151b0
    
private:
    JGeometry::TPos3f _160;   // 0x160 - mRotMtx Rotation Matrix copy?
}; // 0x190


class TMapObjUFO : public TMapObjHioNode, StateObserver {
public:
    TMapObjUFO(const CrsData::SObject &sObject);
    virtual ~TMapObjUFO();                                              // 0x80299c9c
    virtual void reset();                                               // 0x80299dac
    virtual void calc();                                                // 0x80299f68
    virtual const char *getBmdFileName() { return "/Objects/ufo.bmd"; } // 0x8029a2c0
    virtual void createColModel(J3DModelData *) {}                      // 0x8029a2bc
    virtual void MoveExec();                                            // 0x80299efc
    virtual void InitExec();                                            // 0x80299e90
    void initFunc_Fly();                                                // 0x80299fc4
    void doFunc_Fly();                                                  // 0x8029a128
    void initFunc_Wait();                                               // 0x8029a224
    void doFunc_Wait();                                                 // 0x8029a284

    static f32 sRotRad;
    static f32 sFlySpeed;
    static s16 sWaitFrame;                     
    static StateFuncSet<TMapObjUFO> sTable[2];

private:
    TPathUtil *mPathUtil;               // 0x158
    JGeometry::TPos3f _15c;             // 0x15c
}; // 0x18c

#endif // MAPOBJSKYSHIP_H
