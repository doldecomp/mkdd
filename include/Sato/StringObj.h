#ifndef STRINGOBJ_H
#define STRINGOBJ_H

#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3DU/J3DUClipper.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JSupport/JSUList.h"
#include "Kaneshige/DarkAnmMgr.h"
#include "Kaneshige/ExModel.h"
#include "Sato/ObjCollision.h"

// Forward declarations
class ExStringNodeManager;
class StringNodeManager;

class StringObj {
    StringObj(u8, bool);
    virtual ~StringObj() { return; };
    void createModel(JKRSolidHeap *, u32);
    void loadmodel(J3DModelData *);
    void reset();
    virtual void calc();
    void update();
    void setCurrentViewNo(u32);
    void drawSimpleModel(u32, Mtx, J3DUClipper *, Mtx);

    // Inline/Unused
    void createShadowBrk(J3DAnmTevRegKey *);
    void viewCalc(u32);
    void viewCalcClipOn(u32);
    void viewCalcClipOff(u32);

public:
    // VTBL: 0x0
    StringNodeManager *mStringNodeMgr;          // 0x4
    ExModel *mExModel;                          // 0x8
    u32 _c;
    f32 mScale;                                 // 0x10
    MtxPtr _14;
    u8 _18;
    u8 _19[3];                                  // Padding?
    ItemDarkAnmPlayer **mItemDarkAnmPlayer;     // 0x1c
};

class StringNode {
public:
    StringNode() : _38(this) {
        mVel.zero();
        mPos.zero();
        _18.zero();
        _24.zero();
        _30 = true;
        _31 = false;
        _34 = 0.0f;
    }
    ~StringNode() {}
    void calc();

    JGeometry::TVec3f mVel;                     // 0x0
    JGeometry::TVec3f mPos;                     // 0xc
    JGeometry::TVec3f _18;                      // 0x18
    JGeometry::TVec3f _24;                      // 0x24
    u8 _30;                                     // 0x30
    u8 _31;                                     // 0x31
    u8 _32[2];                                  // Padding?
    f32 _34;                                    // 0x34
    JSULink<StringNode> _38;                    // 0x38
};

class StringNodeManager {
public:
    StringNodeManager(u8 count, f32 speed, bool makeSoundMgr, bool makeCrsGround, u8 someFlag);
    virtual ~StringNodeManager() {
        delete[] _34;
    }
    
    void setAllNodePos(const JGeometry::TVec3f &);
    void calc();
    virtual void calcBetweenNodePosAll(f32);
    void calcBetweenNodePos(StringNode *, StringNode *, f32, f32);
    void resetNodeAll(JGeometry::TVec3f *);
    void moveNodeAll();
    void doAirFricG(f32, f32);
    void doHeightCol();
    void getNodeVel(u32, JGeometry::TVec3f *);
    void getNodePos(u32, JGeometry::TVec3f *);
    void addNodeVel(u32, JGeometry::TVec3f);
    void setNodeVel(u32, JGeometry::TVec3f);
    void addNodePos(u32, JGeometry::TVec3f);
    void setNodePos(u32, JGeometry::TVec3f);

    // Inline/Unused
    void calcBetweenNodeVelAll(f32);
    void calcBetweenNodeVel(StringNode *, StringNode *, f32);
    void collideSphereAll(f32, ObjColSphere *, const JGeometry::TVec3f &);
    void collideCylinderAll(f32, ObjColCylinder *, const JGeometry::TVec3f &);
    void dispose();

    void set_3c(u32 val) {
        _3c = val;
    }

    
    void link_set_30(u32 num, bool val) {
#line 112
        JUT_MINMAX_ASSERT(0, num, mStrNodeList.getNumLinks());
        mStrNodeList.getNth(num)->getObject()->_30 = val;                            
    }
                            
    void setNodeLength(u32 num, f32 val) {
#line 202
        JUT_MINMAX_ASSERT(0, num, mStrNodeList.getNumLinks());
        mStrNodeList.getNth(num)->getObject()->_34 = val;                            
    }



    // VTBL: 0x0
    JSUList<StringNode> mStrNodeList;           // 0x4
    f32 _10;
    f32 _14;
    f32 _18;
    f32 _1c;
    f32 _20;                                    // Scaling multiplier of some sort.
    f32 _24;                                    // Something related to distance calculations...?
    JGeometry::TVec3f _28;
    StringNode *_34;
    GameAudio::ObjectSoundMgr *mObjSoundMgr;    // 0x38
    u32 _3c;
    u8 _40;
    u8 _41[3];                                  // Padding?
    f32 _44;
    CrsGround *mCrsGround;                      // 0x48
    u8 _4c;
    u8 _4d[3];                                  // Padding?
};

class ExStringNodeManager : public StringNodeManager {
public:
    ExStringNodeManager(u8 count, f32 speed, bool makeSoundMgr, bool makeCrsGround, u8 someFlag) : StringNodeManager(count, speed, makeSoundMgr, makeCrsGround, someFlag) {
        setNodeLengthAll(speed);
    }
    ~ExStringNodeManager() {}
    virtual void calcBetweenNodePosAll(f32);
    void setNodeLengthAll(f32);
};

#endif // STRINGOBJ_H
