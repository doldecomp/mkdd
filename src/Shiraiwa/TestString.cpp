#include "Shiraiwa/TestString.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3D/J3DModelLoader.h"
#include "JSystem/J3D/J3DTransform.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/ResTIMG.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/Objects/GeoItemCar.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StringObj.h"
#include "Yamamoto/kartCtrl.h"
#include "dolphin/gx/GXEnum.h"
#include "dolphin/gx/GXGeometry.h"
#include "dolphin/gx/GXLighting.h"
#include "dolphin/gx/GXStruct.h"
#include "dolphin/gx/GXTransform.h"
#include "dolphin/gx/GXVert.h"
#include "dolphin/mtx.h"
#include "mathHelper.h"
#include "types.h"

f32 TTestStrBarrel::sItemVelScale = 0.6f;
f32 TTestStrBarrel::sKartVelScale = 3.0f;

f32 TTestString::sAdjustRatio = 0.2f;
f32 TTestString::sAirFric = 0.999999f;
f32 TTestString::sGravity = -0.2f;
f32 TTestString::sWidth = 20.0f;
f32 TTestString::sDownVel = 5.0f;

TTestStrSupervisor *TTestString::sSupervisor;

void TTestStrBarrel::reset() {
    GeographyObj::resetObject();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    clrObjFlagHidding();
    _58 = 0;
}

void TTestStrBarrel::calc() {
    JGeometry::TVec3f local_70;
    JGeometry::TVec3f pointDir;
    JGeometry::TVec3f zNorm;
    mString->getPoint(&mPos);
    zNorm.set(mString->mObjData->zNorm);
    mString->getPointDir(&pointDir);
    pointDir.negate();
    local_70.cross(pointDir, zNorm);
    local_70.normalize();
    zNorm.cross(local_70, pointDir);
    zNorm.normalize();
    mPos.scaleAdd(-100.0f, pointDir, mPos);
    mRotMtx.setXYZDir(local_70, pointDir, zNorm);
    checkItemHitting();
    moveShadowModel();
}

void TTestStrBarrel::moveShadowModel() {
    JGeometry::TPos3f newRotMtx;
    
    if (!mShadowMdl->isInvisibleAll()) {
        CrsGround crsGround = RaceMgr::sRaceManager->getCourse();
        crsGround.search(this, mPos, mPos);

        PSMTXCopy(mRotMtx, newRotMtx.mMtx);
        newRotMtx[0][3] = mPos.x;
        newRotMtx[1][3] = crsGround.getHeight();
        newRotMtx[2][3] = mPos.z;
        mShadowMdl->setBaseTRMtx(newRotMtx);

        J3DModel *model = getShadowModel()->getModel();
        model->setBaseScale(mScale);
    }
}

// FIX: Register mismatch.
void TTestStrBarrel::checkItemHitting() {
    JGeometry::TVec3f vel;
    JGeometry::TVec3f diff;
    JGeometry::TVec3f itemPos;
    if (tstItemHitting()) {
        u32 itemKind = mColItemObj->getKind();

        switch (itemKind) {
        case 0:
        case 1:
        case 2:
        case 7:
        case 9:
        case 11: {
            vel.set(mColItemObj->mVel);
            if (mReaction.tstFlg(itemKind) == 1) {
                vel.negate();
            }
            vel.scale(sItemVelScale);
            mString->attack(vel);
            break;

        }
        case 8:
        {
            // FIX: Register mismatches here:
            itemPos = mColItemObj->mPos;
            diff.sub(mPos, itemPos);
            diff.setLength(50.0f);
            mString->attack(diff);
            break;
        }

        case 3:
        case 4:
        case 5:
        case 6:
        case 10:
        default:
            break;
        }
    }
}

void TTestStrBarrel::createColModel(J3DModelData *modelData) {
    GeographyObj::createBoundsSphere(115.0f, 1.0f);
}

void TTestStrBarrel::doKartColCallBack(int kartIdx) {
    JGeometry::TVec3f kartVel;
    ObjUtility::getKartVel(kartIdx, &kartVel);

    if (kartVel.squared() < 1.0f) {
        kartVel.zero();
    }

    kartVel.scale(sKartVelScale);

    JGeometry::TVec3f kartPos;
    f32 radius;
    GetGeoObjMgr()->getKartCollidePosRadius(kartPos, radius, kartIdx);

    JGeometry::TVec3f dir;
    dir.sub(getBoundsGlPos(0), kartPos);
    dir.normalize();

    JGeometry::TVec3f forward;
    mRotMtx.getYDir(forward);
    forward.normalize();
    forward.scale(100.0f);

    JGeometry::TVec3f attackPos;
    attackPos.scaleAdd(radius + 115.0f, dir, kartPos);
    attackPos.add(forward);

    mString->attack(attackPos, kartVel);

    vibrate(kartIdx);
    GetKartCtrl()->getKartSound(kartIdx)->mSoundMgr->setCrushSe(getSoundID(), mBounds[0]->getBoundDepth() * 0.03f);
}

void TTestStrBarrel::vibrate(int kartIdx) {
    JGeometry::TVec3f kartVel;
    ObjUtility::getKartVel(kartIdx, &kartVel);
    if (kartVel.squared() > 100.0f) {
        GetKartCtrl()->getKartBody(kartIdx)->getStrat()->DoMotor((MotorManager::MotorType)getMotorType());
    }
}


void TTestStrItem::reset() {
    GeographyObj::resetObject();
    clrObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    clrObjFlagHidding();
    _150 = 0;
    _58 = 0;
    for (int i = 0; i < 2; i++) {
        _154[i] = nullptr;
    }
}

void TTestStrItem::calc() {
    JGeometry::TVec3f xDir;
    JGeometry::TVec3f yDir;
    JGeometry::TVec3f zDir;
    
    _14c->getPoint(&mPos);
    zDir.set(0.0f, 0.0f, 1.0f);
    _14c->getPointDir(&yDir);

    xDir.cross(yDir, zDir);
    xDir.normalize();
    zDir.cross(xDir, yDir);
    zDir.normalize();
    mRotMtx.setXYZDir(xDir, yDir, zDir);

    GeographyObj::moveShadowModel();
    if (tstObjFlagHidding()) {
        _150++;
        if (_150 > 600) {
            setAllCheckKartHitFlag();
            clrObjFlagHidding();
            _150 = 0;
        }
    }
}

void TTestStrItem::doKartColCallBack(int kartIdx) {
    if (tstIsHitKartFlg(kartIdx) != 0) {
        GeoItemCar::hitItemBoxProc(kartIdx, _154);
        clrAllCheckKartHitFlag();
        setObjFlagHidding();
    }
}

void TTestStrItem::createColModel(J3DModelData *modelData) {
    GeographyObj::createBoundsSphere(150.0f, 1.0f);
}

TTestStrSupervisor::~TTestStrSupervisor() {
    delete _30;
}

void TTestStrSupervisor::drawAll(u32 param_1) {
    GXColor white = {0xff, 0xff, 0xff, 0xff};
    GXSetTevColor(GX_TEVREG0, white);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
    GXSetNumChans(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1e, GX_FALSE, 0x7d);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetCurrentMtx(0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_CLR_RGBA, GX_F32, 0);
    GXSetAlphaUpdate(GX_TRUE);
    GXSetAlphaCompare(GX_GREATER, GX_FALSE, GX_AOP_OR, GX_GREATER, GX_FALSE);
    _30->load(GX_TEXMAP0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_CLR_RGBA, GX_F32, 0);
    GXSetArray(GX_VA_TEX0, &sStringTexOffset, 8);
    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_CLR_RGBA, GX_F32, 0);
    for (JSULink<TTestString> *testStr = _24.getFirst(); testStr != nullptr; testStr = testStr->getNext()) {
        testStr->getObject()->draw(param_1);
    }
}

void TTestStrSupervisor::load() {
    ResTIMG *timg = (ResTIMG *)ObjUtility::getPtrCourseArc("/Objects/TestHimo1Tex.bti");
    JUT_ASSERT(timg != 0);
    timg->mWrapS = 1;
    _30->storeTIMG(timg);

    J3DModelData *modelData = J3DModelLoaderDataBase::load(ObjUtility::getPtrCourseArc("/Objects/TestHimo1.bmd"), 0x19200010);
    modelData->newSharedDisplayList(0x40000);
    modelData->simpleCalcMaterial(0, j3dDefaultMtx);
    modelData->makeSharedDL();

    for (JSULink<TTestString> *testStr = _24.getFirst(); testStr != nullptr; testStr = testStr->getNext()) {
        testStr->getObject()->load(modelData);
    }
}

void TTestStrSupervisor::createModel(JKRSolidHeap *jkrSolidHeap, u32 p2) {
    for (JSULink<TTestString> *testStr = _24.getFirst(); testStr != nullptr; testStr = testStr->getNext()) {
        testStr->getObject()->createModel(jkrSolidHeap, p2);
    }
}

TTestString::TTestString(const CrsData::SObject &sObject) : GeographyObj(sObject), JKRDisposer(), _164(this) {
    _180 = sObject.mParam1;
    if (_180 < 2) {
        _180 = 5;
    }

    _184 = sObject.mParam2;
    if (_184 <= 0.0f) {
        _184 = 1000.0f;
    }

    _188 = new f32[_180];

    mExStrNodeMgr = new ExStringNodeManager(_180, 30.0f, true, false, 0);

    if (sSupervisor == nullptr) {
        sSupervisor = new TTestStrSupervisor();
    }
    sSupervisor->_24.append(&_164);

    _17c = (TTestStrItem *)GetGeoObjMgr()->createSubObj(0xE0D);
    _17c->_14c = this;
}

TTestString::~TTestString() {
    sSupervisor = nullptr;
    delete mExStrNodeMgr;
}

void TTestString::reset() {
    GeographyObj::resetObject();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    PSMTXIdentity(mRotMtx);
    mExStrNodeMgr->resetNodeAll(nullptr);

    if (mObjData->mParam3 == 0) {
        f32 dVar5 = _184;
        for (u32 i = 0; i < _180 - 1; i++) {
            dVar5 *= 0.5f;
            _188[i] = dVar5;

            mExStrNodeMgr->setNodeLength(i, _188[i]);
            mExStrNodeMgr->link_set_30(i, false);
        }
        _188[_180 - 1] = dVar5;
        mExStrNodeMgr->setNodeLength(_180 - 1, _188[_180 - 1]);
        
    } else {
        f32 nodeLen = _184 / _180;
        mExStrNodeMgr->setNodeLengthAll(nodeLen);
        for (u32 i = 0; i < _180 - 1; i++) {
            _188[i] = nodeLen;
            mExStrNodeMgr->link_set_30(i, false);
        }
    }
    setParameters();
}

void TTestString::load(J3DModelData *modelData) {}

void TTestString::calc() {
    JGeometry::TVec3f upDir;
    JGeometry::TVec3f nodeVel;
    
    getUpDir(&upDir);
    mExStrNodeMgr->_28 = upDir;
    mExStrNodeMgr->calc();
    mExStrNodeMgr->setNodePos(0, mObjData->position);
    nodeVel.scale(-sDownVel,  upDir);
    mExStrNodeMgr->addNodeVel(_180 - 1, nodeVel);
}

void TTestString::attack(const JGeometry::TVec3f &param_1) {
    mExStrNodeMgr->addNodeVel(_180 - 1, param_1);
}

void TTestString::attack(const JGeometry::TVec3f &param_1, const JGeometry::TVec3f &param_2) {
    mExStrNodeMgr->setNodePos(_180 - 1, param_1);
    mExStrNodeMgr->addNodeVel(_180 - 1, param_2);
}

void TTestString::draw(u32 param_1) {
    JGeometry::TVec3f nodePosOne;
    JGeometry::TVec3f nodePosTwo;
    
    for (int i = 0; i < _180 - 1; i++) {
        mExStrNodeMgr->getNodePos(i, &nodePosOne);
        mExStrNodeMgr->getNodePos(i + 1, &nodePosTwo);
        if (i == 0) {
            _18c[0].set(nodePosOne);
            _18c[1].set(nodePosOne.x + sWidth, nodePosOne.y, nodePosOne.z);
            _18c[2].set(sWidth * 0.5f + nodePosOne.x, nodePosOne.y, sWidth * 0.85f + nodePosOne.z);
        }
        drawNode(_188[i], nodePosOne, nodePosTwo, param_1);
    }
}

void TTestString::drawNode(f32 param_1, JGeometry::TVec3f &param_2, JGeometry::TVec3f &param_3, u32 cameraNo) {

}

void TTestString::drawCrevicePyramid(JGeometry::TVec3f &p1, JGeometry::TVec3f &p2, JGeometry::TVec3f &p3) {
    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 5);
    GXPosition3f32(_18c[0].x, _18c[0].y, _18c[0].z);
    GXColor1u8(3);
    GXPosition3f32(_18c[1].x, _18c[1].y, _18c[1].z);
    GXColor1u8(0);
    GXPosition3f32(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
    GXColor1u8(4);
    GXPosition3f32(p1.x + p3.x, p1.y + p3.y, p1.z + p3.z);
    GXColor1u8(1);
    GXPosition3f32(_18c[2].x, _18c[2].y, _18c[2].z);
    GXColor1u8(2);
    GXEnd();
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);
    GXPosition3f32(_18c[0].x, _18c[0].y, _18c[0].z);
    GXColor1u8(3);
    GXPosition3f32(p1.x + p3.x, p1.y + p3.y, p1.z + p3.z);
    GXColor1u8(0);
    GXPosition3f32(_18c[2].x, _18c[2].y, _18c[2].z);
    GXColor1u8(4);
    GXEnd();
}

void TTestString::drawPrism(JGeometry::TVec3f &p1, JGeometry::TVec3f &p2, JGeometry::TVec3f &p3, JGeometry::TVec3f &p4, JGeometry::TVec3f &p5) {
    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 5);
    GXPosition3f32(p2.x + p3.x, p2.y + p3.y, p2.z + p3.z);
    GXColor1u8(3);
    GXPosition3f32(p1.x + p3.x, p1.y + p3.y, p1.z + p3.z);
    GXColor1u8(0);
    GXPosition3f32(p2.x + p4.x, p2.y + p4.y, p2.z + p4.z);
    GXColor1u8(4);
    GXPosition3f32(p1.x + p4.x, p1.y + p4.y, p1.z + p4.z);
    GXColor1u8(1);
    GXPosition3f32(p1.x + p5.x, p1.y + p5.y, p1.z + p5.z);
    GXColor1u8(2);
    GXEnd();
    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 5);
    GXPosition3f32(p2.x + p4.x, p2.y + p4.y, p2.z + p4.z);
    GXColor1u8(4);
    GXPosition3f32(p1.x + p5.x, p1.y + p5.y, p1.z + p5.z);
    GXColor1u8(2);
    GXPosition3f32(p2.x + p5.x, p2.y + p5.y, p2.z + p5.z);
    GXColor1u8(5);
    GXPosition3f32(p1.x + p3.x, p1.y + p3.y, p1.z + p3.z);
    GXColor1u8(0);
    GXPosition3f32(p2.x + p3.x, p2.y + p3.y, p2.z + p3.z);
    GXColor1u8(3);
    GXEnd();
}

void TTestString::getUpDir(JGeometry::TVec3f *upDir) {
    JGeometry::TVec3f *yDir = RCMGetCourse()->getDirY();
    f32 upDirX = yDir->x;
    f32 upDirY = yDir->y;
    f32 upDirZ = yDir->z;
    upDir->set(upDirX, upDirY, upDirZ);
}

void TTestString::getPoint(JGeometry::TVec3f *point) {
    mExStrNodeMgr->getNodePos(_180 - 1, point);
}

void TTestString::getPointDir(JGeometry::TVec3f *dir) {
    JGeometry::TVec3f nodeTwoPos;    
    JGeometry::TVec3f nodeOnePos;
    mExStrNodeMgr->getNodePos(_180 - 2, &nodeTwoPos);
    mExStrNodeMgr->getNodePos(_180 - 1, &nodeOnePos);
    dir->sub(nodeOnePos, nodeTwoPos);
    dir->normalize();
}

void TTestString::setParameters() {
    mExStrNodeMgr->_24 = sAdjustRatio;
    mExStrNodeMgr->_20 = sAirFric;
    mExStrNodeMgr->_1c = sGravity;
}

void TTestString::createModel(JKRSolidHeap *, u32) {}

#include "JSystem/JAudio/JASFakeMatch2.h"
