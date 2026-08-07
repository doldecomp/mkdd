#ifndef MAPOBJNOSSIE_H
#define MAPOBJNOSSIE_H

#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObj.h"
#include "Sato/J3DAnmObject.h"
#include "Shiraiwa/AnmPlayer.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjNossieColBase : public TMapObjStamper {
public:
    TMapObjNossieColBase(u32 id) : TMapObjStamper(id) {}
    TMapObjNossieColBase(const CrsData::SObject &sObject, u32 id) : TMapObjStamper(id) {}
    virtual ~TMapObjNossieColBase() {}
    virtual void reset();
    virtual void calc() {}
    virtual int getReaction() { return 3; }
    void createColModel(J3DModelData *) = 0;
};

class TMapObjNossieColHead : public TMapObjNossieColBase {
public:
    TMapObjNossieColHead() : TMapObjNossieColBase(0x11a2) {
        createSoundMgr();
    }
    virtual ~TMapObjNossieColHead() {}
    virtual void createColModel(J3DModelData *);
    virtual void calc();
};

class TMapObjNossieColBody : public TMapObjNossieColBase {
public:
    TMapObjNossieColBody() : TMapObjNossieColBase(0x11a8) {}
    virtual ~TMapObjNossieColBody() {}
    virtual void createColModel(J3DModelData *);
    virtual int getReaction() { return 1; }
};

class TMapObjNossieColFoot : public TMapObjNossieColBase {
public:
    TMapObjNossieColFoot(u32 id) : TMapObjNossieColBase(id) {
        createSoundMgr();
    }
    virtual ~TMapObjNossieColFoot() {}
    virtual u32 getReleaseFrame() { return 0x1e; }
    virtual void reset();
    virtual void createColModel(J3DModelData *);
    bool isStepped(int);
    virtual void calc();
    void getHitFlag(u8 *);
    void moveShadowModel();
    const char *getShadowBmdFileName() = 0;
    virtual int getReaction() { return 10; }

    static f32 sShakeRadius;

    JGeometry::TVec3f mFootPos;
    f32 _180;
    u8 _184;
    u8 _185;
    u32 _188;
};

class TMapObjNossieColFootFront : public TMapObjNossieColFoot {
public:
    TMapObjNossieColFootFront() : TMapObjNossieColFoot(0x11a3) {}
    virtual ~TMapObjNossieColFootFront() {}
    virtual const char *getShadowBmdFileName() { return "/Objects/NossieFrontShadow.bmd"; };
};

class TMapObjNossieColFootRear : public TMapObjNossieColFoot {
public:
    TMapObjNossieColFootRear() : TMapObjNossieColFoot(0x11a7) {}
    virtual ~TMapObjNossieColFootRear() {}
    virtual const char *getShadowBmdFileName() { return "/Objects/NossieBackShadow.bmd"; };
};

class TMapObjSwimNossie : public TMapObjHioNode {
public:
    TMapObjSwimNossie(const CrsData::SObject &);
    ~TMapObjSwimNossie();
    virtual void createModel(JKRSolidHeap *, u32);
    virtual void loadAnimation();
    virtual const char *getBmdFileName() { return "/Objects/SwimNossie.bmd"; }
    virtual void reset();
    virtual void calc();
    virtual void update();
    f32 moveToNextNode();

    static f32 sMoveMaxSpeed;
    static f32 sMoveAccel;
    static f32 sReachLength;
    //void TMapObjSwimNossie::sTurnSpeed;

    static J3DAnmTexPattern *sSwimNossieBtpAnm;
    static J3DAnmTransform *sSwimNossieBckAnmTrans;
    static J3DMtxCalc *sSwimNossieBckMtxCalc;

    J3DAnmObjMaterial mAnmObjMaterial;
    TPathUtil *mPathUtil;
};

class TMapObjPtera : public TMapObjHioNode {
public:
    TMapObjPtera(const CrsData::SObject &);
    virtual ~TMapObjPtera();
    virtual void reset();
    virtual void calc();
    f32 moveToNextNode();
    virtual void loadAnimation();
    virtual void update();
    virtual void createModel(JKRSolidHeap *, u32);
    virtual const char *getBmdFileName() { return "/Objects/Ptera.bmd"; }

    static TAnmInfo sAnmInfos[2];
    static f32 sMoveMaxSpeed;
    static f32 sMoveAccel;
    static f32 sReachLength;
    //void TMapObjPtera::sTurnSpeed;
    //void TMapObjPtera::sPteraBtpAnm;
    //void TMapObjPtera::scWingSoundFrame;

    TPathUtil *mPathUtil;
    J3DAnmObjMaterial mAnmObjMaterial;
    TAnmPlayer mAnmPlayer;
};

class TMapObjNossie : public TMapObjHioNode {
public:
    TMapObjNossie(const CrsData::SObject &);                // 0x802c571c
    virtual ~TMapObjNossie() {}                             // 0x802c5884
    virtual const char *getBmdFileName();                   // 0x802c5924
    virtual void createModel(JKRSolidHeap *, u32);          // 0x802c594c
    virtual void loadAnimation();                           // 0x802c5a20
    virtual void reset();                                   // 0x802c5a8c
    bool isTimeAttack();                                    // 0x802c5c58
    virtual void calc();                                    // 0x802c5c74
    void makeStepEmitter(u32, const JGeometry::TVec3f &);   // 0x802c5ec0
    void reverse();                                         // 0x802c6038
    void getJointPosition(JGeometry::TVec3f *, s32);        // 0x802c608c

    static const f32 sOffsetY[4];                           // 0x80384de0
    static const u32 scNossieEffectAppearPos[16];           // 0x80384f88
    static const char *sNossieLegJointName[4];              // 0x803a8f50
    static s32 sNossieLegJointNo[4];                        // 0x803a8f60
    static TAnmInfo sAnmInfos[16];                          // 0x803a8f70
    static TAnmInfo sAnmInfosTA[1];                         // 0x803a90b0
    static const JGeometry::TVec3f sBodyOffset;             // 0x80413afc
    static const JGeometry::TVec3f sHeadOffset;             // 0x80413aa4
    static const char *sHeadJointName;                      // 0x80415448
    static s32 sHeadJointNo;                                // 0x804170c8

    // Inline/Unused
    //void TsAnmChangeVel;

public:
    TAnmPlayer mAnmPlayer;                      // 0x14c
    u8 _164[0x194 - 0x164];                     // 0x164 - TODO: Find this object's class type.
    GeographyObj *mNossieHead;                  // 0x194 - Head?
    TMapObjNossieColFoot *mNossieFoot[4];       // 0x198 - Feet
    GeographyObj *mNossieTail;                  // 0x1a8 - tail, maybe?
};  // 0x1ac


#endif // MAPOBJNOSSIE_H
