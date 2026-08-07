#ifndef MAPOBJDEMOOBJ_H
#define MAPOBJDEMOOBJ_H

#include "JSystem/JGeometry/Vec.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

class TMapObjDemoObj : public TMapObjHioNode {
public:
    TMapObjDemoObj(const CrsData::SObject &);       // 0x802cad44
    TMapObjDemoObj(u32);                            // 0x802cad9c
    virtual ~TMapObjDemoObj();                      // 0x802cadf4

    virtual void createModel(JKRSolidHeap *, u32);  // 0x802caf68
    virtual void reset();                           // 0x802cae9c
    virtual void calc();                            // 0x802cafa0
    virtual void setCurrentViewNo(u32);             // 0x802cb11c
    u32 getJ3DModelDataTevStageNum() const { return 0x20020; }

    void flagReset();                               // 0x802cae84
    bool checkKartLength();                         // 0x802cb090
    bool checkVisible();                            // 0x802cb190
    void turnToKart();                              // 0x802cb220

    static const u32 scCameraID[11];                // 0x80385410, firsparkfrondownfarrmontmareitemtoppenddnext
    static f32 sTurnSpeed;                          // 0x80415478

    // Inline/Unused
    //void scTargetKartRank;
    //void scCheckInterval;
    
private:
    f32 mTrackDistanceSq;           // 0x14c
    f32 mKartDistanceSq;            // 0x150
    u8  mDistanceCheckTimer;        // 0x154
    u32 mDemoFlags;                 // 0x158
    s16 mKartIndex;                 // 0x15c
    JGeometry::TVec3f mKartPos;     // 0x160
};

#endif // MAPOBJDEMOOBJ_H
