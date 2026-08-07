#ifndef MAPOBJBUBBLE_H
#define MAPOBJBUBBLE_H

#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjBubble : public TMapObjHioNode, public StateObserver {
public:
    TMapObjBubble(const CrsData::SObject &);        // 0x802b9e5c
    virtual ~TMapObjBubble();                       // 0x802b9f00
    virtual void createColModel(J3DModelData *);    // 0x802b9fe4
    virtual const char *getBmdFileName();           // 0x802ba010
    virtual void createModel(JKRSolidHeap *, u32);  // 0x802ba038
    virtual void loadAnimation();                   // 0x802ba098
    virtual void update();                          // 0x802ba1b8
    virtual void reset();                           // 0x802ba1fc
    virtual void calc();                            // 0x802ba438
    void initFunc_Start();                          // 0x802ba470
    void stopParticle();                            // 0x802ba490
    void doFunc_Start();                            // 0x802ba4ac
    void initFunc_Dive();                           // 0x802ba4e4
    void doFunc_Dive();                             // 0x802ba500
    void initFunc_Symptom();                        // 0x802ba538
    void doFunc_Symptom();                          // 0x802ba53c
    void initFunc_Jump();                           // 0x802ba574
    void doFunc_Jump();                             // 0x802ba62c
    static StateFuncSet<TMapObjBubble> sTable[4];   // 0x803a7c00
    static f32 sGravity;                            // 0x80415330
    static J3DAnmTextureSRTKey *sBubbleBtkAnm;      // 0x80417008

    // Inline/Unused
    //void startParticle();
    //void moveEmitter();

    // Inline
    virtual void MoveExec();                        // 0x802ba14c
    virtual void InitExec();                        // 0x802ba0e0


    J3DAnmObjMaterial mAnmObjMat;
    JGeometry::TVec3f _178;
    u16 mCountStateStart;
    u16 mCountStateDive;
    u16 mCountStateSymptom;
    f32 _18c;
    f32 _190;
    f32 _194;
    JPABaseEmitter *mEmitter;
}; // class MapObjBubble
#endif // MAPOBJBUBBLE_H
