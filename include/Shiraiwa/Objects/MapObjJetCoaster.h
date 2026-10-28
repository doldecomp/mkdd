#ifndef MAPOBJJETCOASTER_H
#define MAPOBJJETCOASTER_H

#include "Inagaki/GameMapSoundMgr.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "types.h"

class TMapObjJetCoaster : public TMapObjHioNode {
public:
    TMapObjJetCoaster(const CrsData::SObject &);
    TMapObjJetCoaster(u32);                         // 0x802c1000
    virtual ~TMapObjJetCoaster();                   // 0x802c1078
    virtual void loadAnimation();                   // 0x802c1208
    virtual void createModel(JKRSolidHeap *, u32);  // 0x802c1254
    virtual void reset();                           // 0x802c1108
    virtual void calc();                            // 0x802c13cc
    virtual void createColModel(J3DModelData *);    // 0x802c13a4
    virtual s32 getJointNo() { return -1; }         // 0x802c15b8
    virtual void setJointNo() {}                    // 0x802c13a0
    virtual const char *getBmdFileName() = 0;

    static f32 sFrameRate;                          // 0x804153d8
    static J3DAnmTransform *sJetCoasterBckAnmTrans; // 0x80417080
    static J3DMtxCalc *sJetCoasterBckMtxCalc;       // 0x80417084
    
    // Inline/Unused
    f32 setAnimationFrame(f32);
    void getColPos(JGeometry::TVec3f *, s32);
    //static const scHeightOffset;

public:
    JGeometry::TVec3f _14c;             // 0x14c
    JGeometry::TVec3f _158;             // 0x158
    bool _164;                          // 0x164
}; // class MapObjJetCoaster

class TMapObjJetCoasterBody : public TMapObjJetCoaster {
public:
    TMapObjJetCoasterBody() : TMapObjJetCoaster(0xce7) {}
    virtual ~TMapObjJetCoasterBody() {}             // 0x802c1e78
    virtual const char *getBmdFileName() { return "/Objects/BabyJet_body.bmd"; } // 0x802c1f18
    virtual s32 getJointNo() { return sJointNo; }   // 0x802c1f24
    virtual void setJointNo();
    
    static s32 sJointNo;
};

class TMapObjJetCoasterHead : public TMapObjJetCoaster {
public:
    TMapObjJetCoasterHead(const CrsData::SObject &);
    TMapObjJetCoasterHead();
    virtual ~TMapObjJetCoasterHead();
    virtual void reset();
    virtual void calc();
    virtual const char *getBmdFileName() { return "/Objects/BabyJet_head.bmd"; } // 0x802c1f2c
    virtual void setJointNo();
    void createBody(u8);
    virtual s32 getJointNo() { return sJointNo; } // 0x802c1f38
    
    static s32 sJointNo;
    static const s32 scBodyMax;
    // static scFrameDiff;

public:
    u8 mBodyCount;                              // 0x168 (+3 bytes padding)
    f32 mInitialFrame;                          // 0x16c
    TMapObjJetCoasterBody **mJetCoasterBodies;  // 0x170
    GameAudio::CoasterSoundMgr *mSoundMgr;      // 0x174
};

#endif // MAPOBJJETCOASTER_H
