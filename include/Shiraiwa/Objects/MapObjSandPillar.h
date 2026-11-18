#ifndef MAPOBJSANDPILLAR_H
#define MAPOBJSANDPILLAR_H

#include "JSystem/J3D/J3DAnmTextureSRTKey.h"
#include "JSystem/J3D/J3DAnmTransform.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/ItemObj.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/TKartThrower.h"

class TMapObjSandPillar : public TKartThrower, public StateObserver {
public:
    TMapObjSandPillar(const CrsData::SObject &);                            // 0x802bbe1c
    virtual ~TMapObjSandPillar();                                           // 0x802bbf10
    virtual void MoveExec();                                                // 0x802bc090
    virtual void InitExec();                                                // 0x802bc024
    void getKartThrowDirPow(JGeometry::TVec3f *, f32 *, int);               // 0x802bc0fc
    void getThrowDir(JGeometry::TVec3f *, s16);                             // 0x802bc27c
    virtual s32 getThrowPow();                                              // 0x802bc320
    virtual const char *getBmdFileName();                                   // 0x802bc338
    virtual void loadAnimation();                                           // 0x802bc360
    virtual void createModel(JKRSolidHeap *, u32);                          // 0x802bc3c8
    virtual void createColModel(J3DModelData *);                            // 0x802bc584
    virtual void reset();                                                   // 0x802bc5ec
    virtual void calc();                                                    // 0x802bc764
    virtual void update();                                                  // 0x802bc8c0
    
    void getItemThrowDirPow(JGeometry::TVec3f *, f32 *, const ItemObj &);   // 0x802bc904
    void initFunc_Walk();                                                   // 0x802bca7c
    void doFunc_Walk();                                                     // 0x802bca8c
    
    static StateFuncSet<TMapObjSandPillar> sTable[1];                       // 0x803a8098
    static f32 sItemVel;                                                    // 0x80415348
    static f32 sItemUpVel;                                                  // 0x8041534c
    static J3DAnmTransform *sSandPillarBckAnmTrans;                         // 0x80417048
    static J3DMtxCalc *sSandPillarBckMtxCalc;                               // 0x8041704c
    static J3DAnmTextureSRTKey *sSandPillarBtkAnm;                          // 0x80417050
    
    // Inline/Unused
    void createEmitter();
    void moveEmitter();
    // const cBoundRadius;
    // const cBoundHeight;

    J3DAnmObjMaterial mAnmObjMat;                   // 0x158
    TPathWalk *mPathWalk;                           // 0x178
    s16 _17c;                                       // 0x17c
    JGeometry::TPos3f mTornadoPos;                  // 0x180
    JPABaseEmitter *mEmitter;                       // 0x1b0
    GameAudio::CircleSoundMgr *mCircleSoundMgr;     // 0x1b4
}; // class MapObjSandPillar
#endif // MAPOBJSANDPILLAR_H
