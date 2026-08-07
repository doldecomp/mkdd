#ifndef MAPOBJDOSSUN_H
#define MAPOBJDOSSUN_H

#include "JSystem/J3D/J3DAnmTexPattern.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3D.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjDossun : public TMapObjStamper, StateObserver {
public:
    TMapObjDossun(const CrsData::SObject &);
    virtual ~TMapObjDossun();
    virtual void loadAnimation();
    virtual void createModel(JKRSolidHeap *, u32);
    virtual void reset();
    virtual void calc();
    virtual void update();
    virtual const char *getBmdFileName();
    virtual const char *getShadowBmdFileName();
    virtual void createColModel(J3DModelData *);
    virtual void doKartColCallBack(int param_1);
    virtual u32 getReleaseFrame() { return 100; };
    virtual void MoveExec();
    virtual void InitExec();
    
    void initFunc_Start();
    void doFunc_Start();
    void initFunc_Up();
    void doFunc_Up();
    void initFunc_Top();
    void doFunc_Top();
    void initFunc_Down();
    void doFunc_Down();
    void makeGroundEmitter();
    void initFunc_Bottom();
    void doFunc_Bottom();
    void initFunc_Die();
    void doFunc_Die();
    void shakeCamera();
    void checkItemHitting();
    
    static JGeometry::TVec3f sDieVelocity0;
    static const JGeometry::TVec3f scDownVelocity0;
    static StateFuncSet<TMapObjDossun> sTable[6];
    static J3DAnmTransform *sDossunBckAnmTrans;
    static J3DMtxCalc *sDossunBckMtxCalc;
    static J3DAnmTexPattern *sDossunBtpAnm;
    static f32 sBtpPlyFrameRate;
    static f32 sBtpRevFrameRate;
    static f32 sDieGravityY;
    

private:
    J3DAnmObjMaterial _180;             // 0x180
    s16 mHeight;                        // 0x1a0
    u16 _1a2;                           // 0x1a2
    u8 _1a4;                            // 0x1a4
    u8 _1a5;                            // 0x1a5 - Padding...?
    u16 _1a6;                           // 0x1a6
    TFreeMove mFreeMove;                // 0x1a8
    TPathMove *mPathMove;               // 0x1d0
    s16 _1d4;                           // 0x1d4
}; // 0x1d8

#endif // MAPOBJDOSSUN_H
