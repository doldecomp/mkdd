#ifndef MAPOBJFERRISWHEEL_H
#define MAPOBJFERRISWHEEL_H

#include "JSystem/JGeometry/Matrix.h"
#include "Kaneshige/Course/CrsData.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"

// HACK: J3DJointCallBack is a `bool` by default, but for some reason
//       it's used as an `int` here. Declare this so we can use a
//       typecast to force the right type for now.
typedef int (*J3DJointCallBack2)(J3DJoint*, int);

class TMapObjFerriswheel : public TMapObjHioNode {
public:
    TMapObjFerriswheel(const CrsData::SObject &);                       // 0x802bcbf0
    virtual ~TMapObjFerriswheel();                                      // 0x802bcc58
    virtual const char *getBmdFileName();                               // 0x802bcce8
    virtual void createModel(JKRSolidHeap *, u32);                      // 0x802bcd10
    void setCallBack(const char *, J3DJointCallBack2);                  // 0x802bcd9c
    virtual void reset();                                               // 0x802bcdf4
    virtual void calc();                                                // 0x802bd1d8
    static int centerCallBack(J3DJoint *, int);                         // 0x802bd1dc
    static int gondolaCallBack(J3DJoint *, int);                        // 0x802bd290
    
    static const char *scGondolaJointName[16];                          // 0x803a82d8
    static s32 sRotFrame;                                               // 0x80415350
    static const char *scRotJointName;                                  // 0x80415354
    static s32 sRotJointNo;                                             // 0x80417060
    
    // Inline/Unused
    void rotate(MtxPtr);
    void setGondolaMtx(int, MtxPtr);

    JGeometry::TPos3f _14c;                             // 0x14c
    JGeometry::TPos3f _17c;                             // 0x17c
    JGeometry::TPos3f _1ac;                             // 0x1ac
}; // class MapObjFerriswheel
#endif // MAPOBJFERRISWHEEL_H
