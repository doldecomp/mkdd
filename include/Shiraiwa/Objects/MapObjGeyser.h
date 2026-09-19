#ifndef MAPOBJGEYSER_H
#define MAPOBJGEYSER_H

#include "JSystem/J3D/J3DAnmTextureSRTKey.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/TKartThrower.h"


class TMapObjGeyser : public TKartThrower, public StateObserver {
public:
    TMapObjGeyser(const CrsData::SObject &);                // 0x802a8f6c
    virtual ~TMapObjGeyser();                               // 0x802a906c
    virtual void MoveExec();                                // 0x802a91d8
    virtual void InitExec();                                // 0x802a916c
    virtual const char *getBmdFileName();                   // 0x802a9244
    virtual void createColModel(J3DModelData *);            // 0x802a926c
    virtual void createModel(JKRSolidHeap *, u32);          // 0x802a92d4
    virtual void loadAnimation();                           // 0x802a9364
    virtual void update();                                  // 0x802a9700
    virtual void reset();                                   // 0x802a9744
    virtual void calc();                                    // 0x802a98bc
    virtual void viewCalc(u32);                             // 0x802a996c
    virtual void getThrowDir(JGeometry::TVec3f *, s16);     // 0x802a9998
    virtual s32 getThrowPow();                              // 0x802a9ba4
    virtual void getKartThrowDirPow(JGeometry::TVec3f *, f32 *, int); // 0x802a9bb0
    virtual void getItemThrowDirPow(JGeometry::TVec3f *, f32 *, const ItemObj &); // 0x802a9c34
    virtual bool checkChangeAct();                          // 0x802a9f70
    
    void playEmitter(u32, const JGeometry::TVec3f &);       // 0x802a93c4
    void moveEmitter(u32, const JGeometry::TVec3f &);       // 0x802a9564
    void initFunc_Start();                                  // 0x802a9d7c
    void doFunc_Start();                                    // 0x802a9dec
    void initFunc_Wait();                                   // 0x802a9e74
    void doFunc_Wait();                                     // 0x802a9ee4
    void initFunc_Act();                                    // 0x802aa03c
    void doFunc_Act();                                      // 0x802aa080
    void initFunc_JumpUp();                                 // 0x802aa1c8
    void doFunc_JumpUp();                                   // 0x802aa294
    void initFunc_JumpStay();                               // 0x802aa44c
    void doFunc_JumpStay();                                 // 0x802aa450
    void initFunc_JumpDown();                               // 0x802aa608
    void doFunc_JumpDown();                                 // 0x802aa61c

    static const char *scEffectName[6];                     // 0x803a6078
    static TAnmInfo sAnmInfos[4];                           // 0x803a6090
    static StateFuncSet<TMapObjGeyser> sTable[6];           // 0x803a6170
    static s16 sActTime;                                    // 0x80415258
    static f32 sJumpUpStartVel;                             // 0x8041525c
    static u16 sCheckCycle;                                 // 0x80415260
    static s16 sEfctABCDEndFrame;                           // 0x80415262
    static f32 sBoundHeight;                                // 0x80415264
    static s16 sEfctEFEndFrame;                             // 0x80416f00
    static s32 sJointNum;                                   // 0x80416f04
    static J3DAnmTextureSRTKey *sGeyserBtkAnm;              // 0x80416f08
    static const s8 sAnmTable[6];                           // 0x8041cbd8

    // MJB - Fabricated:
    inline void createEmitter(int);
    inline void setAnimation(int);

    // Inline/Unused
    JPABaseEmitter *getEmitter(u32 idx) { return _1a0[idx]; }
    bool isActiveEmitter(u32 idx) { return _1a0[idx]->checkStatus(1); };
    void stopEmitter(u32 idx) { _1a0[idx]->stopCreateParticle(); }
    // void hideEmitter(u32);
    void showEmitter(u32 idx) { _1a0[idx]->playCreateParticle(); }
    // void getEmitterPos(JGeometry::TVec3f *);
    // void changeAllState(u16);
    //void sTopStayTime;
    //void sLimitUpVel;
    //void sGravity;
    //void scHeightOffset;

    J3DAnmObjMaterial mAnmObjMaterial;      // 0x158
    TAnmPlayer mAnmPlayer;                  // 0x178
    s32 _190;                               // 0x190
    s16 _194;                               // 0x194
    f32 _198;                               // 0x198
    u16 _19c;                               // 0x19c
    JPABaseEmitter *_1a0[6];                // 0x1a0
    JGeometry::TVec3f _1b8;                 // 0x1b8
    f32 _1c4;                               // 0x1c4
};

class TMapObjGeyserSearch : public TMapObjGeyser {
public:
    TMapObjGeyserSearch(const CrsData::SObject &sObject) : TMapObjGeyser(sObject) {}
    virtual ~TMapObjGeyserSearch() {}
    virtual void reset();
    virtual bool checkChangeAct();

    static u16 sCheckCycle;
};

#endif // MAPOBJGEYSER_H
