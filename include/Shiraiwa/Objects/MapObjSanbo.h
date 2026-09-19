#ifndef MAPOBJSANBO_H
#define MAPOBJSANBO_H

#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRDisposer.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObj.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjSanbo;

class TSanboSupervisor : public GeoObjSupervisor {
public:
    TSanboSupervisor();
    virtual ~TSanboSupervisor() {}                  // 0x802b60d4
    virtual void reset() {}                         // 0x802b6174
    virtual void calc() {}                          // 0x802b6178
    virtual void thunderDownAll();

    void entry(TMapObjSanbo *);

    JSUList<TMapObjSanbo> mList;
};

class TMapObjSanboParts : public TMapObjHioNode, public StateObserver {
public:
    TMapObjSanboParts(u32 id) : TMapObjHioNode(id) {}
    virtual ~TMapObjSanboParts() {}
    virtual void reset();
    virtual void calc();
    virtual const char *getBmdFileName() = 0;
    virtual bool acceptHidding() = 0;
    virtual void MoveExec();                            // 0x802b5e40
    virtual void InitExec();                            // 0x802b5dd4
    void initFunc_Hide();
    void doFunc_Hide();
    void initFunc_Burst();
    void doFunc_Burst();
    void burst(const JGeometry::TVec3f &, const JGeometry::TPos3f &);

    f32 newRnd() { return ((getGeoRnd()->get_ufloat_1() * 2.0f) - 1.0f) * F_PI * (0x8000 / F_PI); }
    
    static StateFuncSet<TMapObjSanboParts> sTable[2];
    static f32 sGravity;
    static f32 sAirFric;
    static f32 sBurstVelH;
    static f32 sBurstVelV;
    static f32 sRotRad;
    
    // static TMapObjSanboParts::sBornFrame;

    JGeometry::TPos3f _158;             // 0x158
    JGeometry::TVec3f _188;             // 0x188
};

class TMapObjSanboHead : public TMapObjSanboParts {
public:
    TMapObjSanboHead() : TMapObjSanboParts(0x138d) {}
    virtual ~TMapObjSanboHead() {}            // 0x802b6014
    virtual const char *getBmdFileName() { return "/Objects/Sanbo1_break_head.bmd"; } // 0x802b6008
    virtual bool acceptHidding();

    static const f32 scHeight;
};

class TMapObjSanboBody : public TMapObjSanboParts {
public:
    TMapObjSanboBody() : TMapObjSanboParts(0x138e) {}
    virtual ~TMapObjSanboBody() {}            // 0x802b5f48
    virtual const char *getBmdFileName() { return "/Objects/Sanbo1_break_body.bmd"; } // 0x802b5f3c
    virtual bool acceptHidding();

    static const f32 scHeight;
};

class TMapObjSanbo : public TMapObjHioNode, public StateObserver, public JKRDisposer {
public:
    TMapObjSanbo(const CrsData::SObject &);         // 0x802b42cc
    virtual ~TMapObjSanbo();                        // 0x802b4418
    virtual void loadAnimation();                   // 0x802b4660
    virtual void createModel(JKRSolidHeap *, u32);  // 0x802b4698
    virtual void reset();                           // 0x802b44fc
    virtual void calc();                            // 0x802b4610
    virtual const char *getBmdFileName() { return "/Objects/Sanbo1.bmd"; } // 0x802b617c
    virtual void createColModel(J3DModelData *);    // 0x802b472c
    virtual TSanboSupervisor *getSupervisor() { return sSupervisor; } // 0x802b4410
    virtual void doKartColCallBack(int);            // 0x802b4940
    virtual void MoveExec();                        // 0x802b486c
    virtual void InitExec();                        // 0x802b4800

    void checkItemHitting();                        // 0x802b48d8
    void initFunc_Start();                          // 0x802b49b0
    void doFunc_Start();                            // 0x802b49d4
    void initFunc_Stand();                          // 0x802b4a54
    void doFunc_Stand();                            // 0x802b4a6c
    void moveColOffset();                           // 0x802b4aec
    void initFunc_Prepare();                        // 0x802b4d04
    void doFunc_Prepare();                          // 0x802b4d08
    void initFunc_Attack();                         // 0x802b4e74
    void doFunc_Attack();                           // 0x802b4e94
    void initFunc_Down();                           // 0x802b4f44
    void doFunc_Down();                             // 0x802b4f64
    void initFunc_Recover();                        // 0x802b4f9c
    void doFunc_Recover();                          // 0x802b4fbc
    void initFunc_Die();                            // 0x802b509c
    void doFunc_Die();                              // 0x802b50e8
    void initFunc_Hide();                           // 0x802b5180
    void doFunc_Hide();                             // 0x802b5184
    void initFunc_Born();                           // 0x802b5204
    void doFunc_Born();                             // 0x802b5230
    void setBurst();                                // 0x802b5394
    void getJointPos(JGeometry::TVec3f *, JGeometry::TVec3f *, long); // 0x802b5754

    static const s8 sAnmTable[10];                  // 0x80384280
    static const s16 scAppearSoundFrame[5];         // 0x8038428c
    static TAnmInfo sAnmInfos[4];                   // 0x803a71b8
    static StateFuncSet<TMapObjSanbo> sTable[10];   // 0x803a72f8
    static const char *scJointName[5];              // 0x803a7410
    static s32 sJointNum[5];                        // 0x803a7424
    static f32 sTailOffset;                         // 0x80415300
    static f32 sHeadOffset;                         // 0x80415304
    static s16 sThunderDownBonus;                   // 0x80415308
    static TSanboSupervisor *sSupervisor;           // 0x80416f98

    // Inline/Unused
    //void JSULink<TMapObjSanbo>::~JSULink();
    void changeAllState(u16);
    //void JSUList<TMapObjSanbo>::~JSUList();
    //static sBornInterval;

    // Fabricated:
    void setAnmTableState(u8 newState) {
        setState(newState);
        const s8 anmNo = sAnmTable[newState];
        if (anmNo != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = anmNo;
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }

    //static const scAttackFrame;
    //static const scOrgCenterY;

    JSULink<TMapObjSanbo> mLinkSanbo;   // 0x170
    TAnmPlayer mAnmPlayer;              // 0x180
    bool _198;                          // 0x198
    bool _199;                          // 0x199
    s16 _19a;                           // 0x19a
    s16 _19c;                           // 0x19c
    s16 _19e;                           // 0x19e
    s16 _1a0;                           // 0x1a0
    u8 _1a2;                            // 0x1a2
    u8 _1a3;                            // 0x1a3
    JGeometry::TPos3f _1a4;             // 0x1a4
    TMapObjSanboParts *_1d4[5];         // 0x1d4
}; // class MapObjSanbo

#endif // MAPOBJSANBO_H
