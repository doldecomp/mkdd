#ifndef TESTSTRING_H
#define TESTSTRING_H

#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRDisposer.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JUtility/JUTTexture.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObj.h"
#include "Sato/Objects/GeoItemBox.h"
#include "Sato/StringObj.h"

// Forward Declaration.
class TTestString;

class TTestStrBarrel : public GeographyObj {
public:
    TTestStrBarrel() : GeographyObj(0xe0d) {}
    virtual ~TTestStrBarrel() {}
    virtual void reset();
    virtual void calc();
    virtual u32 getSoundID() const { return 0x10063; };
    virtual const char *getBmdFileName() { return "/Objects/TestHimo1Ukiwa.bmd"; }
    virtual const char *getShadowBmdFileName() { return "/Objects/TestHimo1Shadow.bmd"; }
    virtual void createColModel(J3DModelData *);
    virtual void doKartColCallBack(int);
    void moveShadowModel();
    void checkItemHitting();
    void vibrate(int);

    
    static f32 sItemVelScale;
    static f32 sKartVelScale;

    TTestString *mString;
};


class TTestStrItem : public GeographyObj {
public:
    TTestStrItem() : GeographyObj(0xe0b) {}
    virtual ~TTestStrItem() {}
    virtual void reset();
    virtual void calc();
    virtual void doKartColCallBack(int);
    virtual void createColModel(J3DModelData *);
    virtual const char *getBmdFileName() { return GeoItemBox::getBmdName(); }
    virtual const char *getShadowBmdFileName() { return GeoItemBox::getShadowBmdName(); }

    TTestString *_14c;          // 0x14c
    u16 _150;                   // 0x150 (+ 2 bytes padding)
    JPABaseEmitter *_154[2];    // 0x154
};


class TTestStrSupervisor : public GeoObjSupervisor {
public:
    TTestStrSupervisor() : GeoObjSupervisor() {
        _24.initiate();
        _30 = new JUTTexture();
    };
    virtual ~TTestStrSupervisor();
    virtual void reset() {}
    virtual void calc() {}
    void drawAll(u32);
    virtual void load();
    virtual void createModel(JKRSolidHeap *, u32);
    void entry(TTestString *);

    static JGeometry::TVec3f sStringOffset[6];
    static JGeometry::TVec3f sStringTexOffset[4];
    
    JSUList<TTestString> _24;
    JUTTexture *_30;
};

class TTestString : public GeographyObj, public JKRDisposer {
public:
    TTestString(const CrsData::SObject &sObject);   // 0x802abf28
    virtual ~TTestString();                         // 0x802ac110
    virtual void createModel(JKRSolidHeap *, u32);  // 0x802ad458
    virtual void reset();                           // 0x802ac1e4
    virtual void calc();                            // 0x802ac4f4
    virtual TTestStrSupervisor *getSupervisor() { return sSupervisor; } // 0x802ad45c 
    void load(J3DModelData *);                      // 0x802ac4f0
    void attack(const JGeometry::TVec3f &);         // 0x802ac5b4
    void attack(const JGeometry::TVec3f &, const JGeometry::TVec3f &); // 0x802ac5f4
    void draw(u32);                                 // 0x802ac66c
    void drawNode(f32, JGeometry::TVec3f &, JGeometry::TVec3f &, u32); // 0x802ac764
    void drawCrevicePyramid(JGeometry::TVec3f &, JGeometry::TVec3f &, JGeometry::TVec3f &); // 0x802ace90
    void drawPrism(JGeometry::TVec3f &, JGeometry::TVec3f &, JGeometry::TVec3f &, JGeometry::TVec3f &, JGeometry::TVec3f &); // 0x802ad034
    void getUpDir(JGeometry::TVec3f *);             // 0x802ad2c4
    void getPoint(JGeometry::TVec3f *);             // 0x802ad2e8
    void getPointDir(JGeometry::TVec3f *);          // 0x802ad318
    void setParameters();                           // 0x802ad430

    static f32 sAdjustRatio; // 0x80415278
    static f32 sAirFric; // 0x8041527c
    static f32 sGravity; // 0x80415280
    static f32 sWidth; // 0x80415284
    static f32 sDownVel; // 0x80415288
    static TTestStrSupervisor *sSupervisor;         // 0x80416f18
    
    // Inline/Unused
    //void JSUList<TTestString>::~JSUList();
    //void JSULink<TTestString>::~JSULink();
    //void TTestStringObj::calc();
    //void TTestStringObj::~TTestStringObj();
    //void TTestStrBarrel::scBombAttackSpeed;
    //void TTestStrBarrel::scCenterOffsetY;
    //void TTestStrBarrel::scVibrationLimit;
    //void TTestStrBarrel::scObjRadius;

    JSULink<TTestString> _164;              // 0x164
    ExStringNodeManager *mExStrNodeMgr;     // 0x174
    u8 _178[0x17c - 0x178];                 // 0x178 - Unused?
    TTestStrItem *_17c;                     // 0x17c
    u8 _180;                                // 0x180
    u8 _181[3];                             // 0x181
    f32 _184;                               // 0x184
    f32 *_188;                              // 0x188
    JGeometry::TVec3f _18c[3];              // 0x18c

}; // class TestString

#endif // TESTSTRING_H
