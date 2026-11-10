#ifndef MAPOBJFIREBAR_H
#define MAPOBJFIREBAR_H

#include "JSystem/J3D/J3DModel.h"
#include "JSystem/J3D/J3DMtxCalc.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Shiraiwa/Objects/MapObjFireObj.h"
#include "dolphin/types.h"


class TMapObjFireBar : public TMapObjFireObj {
public:
    TMapObjFireBar(const CrsData::SObject &sObject, u32 id = 0x106a);
    virtual ~TMapObjFireBar();                      // 0x8029ca6c
    virtual void reset();                           // 0x8029cacc
    virtual void calc();                            // 0x8029cf18
    virtual const char *getBmdFileName();           // 0x8029cee8
    virtual const char *getShadowBmdFileName();
    virtual void createColModel(J3DModelData *);    // 0x8029cebc
    void resetFire();                               // 0x8029cbcc

    // Inline/Unused
    //void TMapObjFireCircle::sKartMax;
    // Inline

    u8 _184[0x188 - 0x184];
    u8 _188;
    u8 _18a[0x18c - 0x18a];     // Padding?
    s16 _18c;
    f32 _190;
}; // class TMapObjFireBar

class TMapObjKpFireBar : public TMapObjFireBar {
public:
    TMapObjKpFireBar(const CrsData::SObject &sObject);
    virtual ~TMapObjKpFireBar() {}
    virtual void loadAnimation();
    virtual void createModel(JKRSolidHeap *, u32);
    virtual void reset();
    virtual const char *getBmdFileName() { return "/Objects/KpFireBar1.bmd"; }
    virtual void createColModel(J3DModelData *);

    static J3DAnmTransform *sKpFireBarBckAnmTrans;
    static J3DMtxCalc *sKpFireBarBckMtxCalc;
    static bool sUseBck;
};

class TMapObjFireCircle : public TMapObjFireObj {
public:
    TMapObjFireCircle(const CrsData::SObject &);
    virtual ~TMapObjFireCircle(); // 0x8029d368
    virtual void reset();
    virtual void calc();
    void resetFire();

    u8 _184[4]; // TODO: Unknown Type - unused?
    f32 _188;
    s16 _18c;
    s16 _18e;
    f32 _190;
    f32 _194;
};

#endif // MAPOBJFIREBAR_H
