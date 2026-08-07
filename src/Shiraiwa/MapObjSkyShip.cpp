#include "Shiraiwa/Objects/MapObjSkyShip.h"
#include "JSystem/J3D/J3DAnmTextureSRTKey.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JMath/JMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/shadowModel.h"
#include "Sato/GeographyObj.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3D.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "dolphin/mtx.h"
#include "math.h"

StateObserver::StateFuncSet<TMapObjUFO> TMapObjUFO::sTable[2] = {
    { 0, &TMapObjUFO::initFunc_Fly, &TMapObjUFO::doFunc_Fly },
    { 1, &TMapObjUFO::initFunc_Wait, &TMapObjUFO::doFunc_Wait },
};

J3DAnmTextureSRTKey *TMapObjPathYoshiHeli::sPathYoshiHeliBtkAnm;

f32 TMapObjSkyShip::sAmplitude = 300.0f;
s16 TMapObjSkyShip::sAmpTimer = 100;
f32 TMapObjPathYoshiHeli::sAmplitude = 5.0f;
s16 TMapObjPathYoshiHeli::sAmpTimer = 7000;
f32 TMapObjPathYoshiHeli::sReachDistance = 1000.0f;
f32 TMapObjMashBalloon::sAmplitude = 500.0f;
s16 TMapObjMashBalloon::sAmpTimer = 100;
f32 TMapObjUFO::sRotRad = 0.06;
f32 TMapObjUFO::sFlySpeed = 500.0f;
s16 TMapObjUFO::sWaitFrame = 120;

TMapObjSkyShip::TMapObjSkyShip(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
#line 28
    JUT_ASSERT_MSG(mObjData->mPathID != 0xffff, "TMapObjSkyShip: Illegal Path ID");
    setObjFlagSimpleDraw();
    mPathMove = new TPathMove(&sObject);
    mFreeRotate = new TFreeRotate();
}

TMapObjSkyShip::~TMapObjSkyShip() {
    delete mPathMove;
    delete mFreeRotate;
}

void TMapObjSkyShip::reset() {
    GeographyObj::resetObject();
    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _58 = 0;

    f32 speed = mObjData->mParam1 * 0.01f;
    if (speed == 0.0f) {
        speed = 10.0f;
    }

    if (mObjData->mPathID != 0xffff) {
        mPathMove->getNodePosition(&mPos, 0);
        TPathMove *pathMove = mPathMove;
        pathMove->_14 = speed;
        pathMove->_18 = speed;
        pathMove->_20 = (speed * 0.5f) * (speed * 0.5f);
        mPathMove->init(&mPos, &mVel);
        mFreeRotate->init(&mRotMtx);

        JGeometry::TVec3f nodeDir;
        getNodeDir(mPathMove->getNextNode(), &nodeDir);
        nodeDir.normalize();

        mRotMtx.setZDir(nodeDir);
        mFreeRotate->setTargetVec(nodeDir, 1.0f, 1.0f, 0.0f, 'z');
        mPathMove->setTargetNode();
        mVel.zero();
    }

    f32 yPos = mPos.y;
    _154 = yPos;
    _150 = yPos;
    _14c = 0;
}

const char *TMapObjSkyShip::getBmdFileName() {
    static const char *cBmdName = "/Objects/SkyShip1.bmd";
    return cBmdName;
}

const char *TMapObjSkyShip::getShadowBmdFileName() {
    static const char *cShadowBmdName = "/Objects/SkyShip1Shadow.bmd";
    return cShadowBmdName;
}

void TMapObjSkyShip::calc() {
    if (mObjData->mPathID != 0xffff) {
        mPos.y = _154;
        if (!mPathMove->update()) {
            JGeometry::TVec3f nodeDir;
            getNodeDir(mPathMove->getNextNode(), &nodeDir);

            f32 fVar1 = 10.0f / nodeDir.length();
            nodeDir.normalize();
            mFreeRotate->setTargetVec(nodeDir, fVar1, fVar1, 0.0f, 'z');
            mPathMove->setTargetNode();
        }
        mFreeRotate->update();
        _154 = mPos.y;
    }
    localMove();
    moveShadowRot();
}

void TMapObjSkyShip::moveShadowRot() {
    if (mShadowMdl == nullptr) {
        return;
    }

    if (mShadowMdl->isInvisibleAll()) {
        return;
    }

    CrsGround ground(RCMGetCourse());
    ground.search(mPos);

    JGeometry::TPos3f newRotMtx;
    PSMTXCopy(mRotMtx, newRotMtx);

    // Surprising this doesn't use mtx.setTrans();
    newRotMtx[0][3] = mPos.x;
    newRotMtx[1][3] = ground.getHeight();
    newRotMtx[2][3] = mPos.z;

    mShadowMdl->setBaseTRMtx(newRotMtx);
    mShadowMdl->getModel()->setBaseScale(mScale);
}

void TMapObjSkyShip::getNodeDir(u16 param_1, JGeometry::TVec3f *dir) {
    JGeometry::TVec3f nodePos;
    mPathMove->getNodePosition(&nodePos, param_1);
    dir->sub(nodePos, mPos);
}

void TMapObjSkyShip::localMove() {
    f32 offset = getAmplitude() * JMASSin(_14c);
    mPos.y = _150 + offset;
    _14c += getAmpTimer();
}

void TMapObjPathYoshiHeli::reset() {
    GeographyObj::resetObject();
    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _58 = 0;

    f32 speed = mObjData->mParam1 * 0.01f;
    if (speed == 0.0f) {
        speed = 10.0f;
    }
    
    if (mObjData->mPathID != 0xffff) {
        mPathMove->getNodePosition(&mPos, 0);
        TPathMove *pathMove = mPathMove;
        pathMove->_14 = speed * 0.01f;
        pathMove->_18 = speed;
        pathMove->_20 = (speed * 0.5f) * (speed * 0.5f);
        mPathMove->_20 = sReachDistance * sReachDistance;
        mPathMove->init(&mPos, &mVel);
        
        JGeometry::TVec3f nodeDir;
        getNodeDir(mPathMove->getNextNode(), &nodeDir);
        nodeDir.normalize();

        mRotMtx.setZDir(nodeDir);
        mPathMove->setTargetNode();
        mVel.zero();
    }

    f32 yPos = mPos.y;
    _154 = yPos;
    _150 = yPos;
    _14c = 0;
    mAnmObjMaterial.resetFrame();
}

void TMapObjPathYoshiHeli::calc() {
    if (mObjData->mPathID != 0xffff) {
        mPos.y = _154;
        if (!mPathMove->update()) {
            JGeometry::TVec3f nodePosDiff;
            JGeometry::TVec3f nodePos;
            mPathMove->getNodePosition(&nodePos, mPathMove->getNextNode());
            nodePosDiff.sub(nodePos, mPos); // HACK: Unused?
            mPathMove->setTargetNode();
        }
        aimVelDir();
        _154 = mPos.y;
    }
    localMove();
    moveShadowRot();
    mAnmObjMaterial.update();
}

void TMapObjPathYoshiHeli::aimVelDir() {
    JGeometry::TVec3f dirX;
    JGeometry::TVec3f dirY;
    JGeometry::TVec3f dirZ;

    mRotMtx.getXDir(dirX);
    mRotMtx.getYDir(dirY);
    mRotMtx.getZDir(dirZ);

    if (!mVel.isZero()) {
        dirZ.set(mVel);
    }
    dirZ.y = 0.0f;

    dirZ.normalize();
    if (dirZ.isZero()) {
        dirZ.cross(dirX, dirY);
        dirZ.normalize();
    }

    dirY.set(0.0f, 1.0f, 0.0f);
    dirX.cross(dirY, dirZ);
    dirX.normalize();
    mRotMtx.setXYZDir(dirX, dirY, dirZ);
}

void TMapObjPathYoshiHeli::update() {
    mAnmObjMaterial.anmFrameProc();
    GeographyObj::update();
}

void TMapObjPathYoshiHeli::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/YoshiHeliB.btk");
    J3DAnmObjMaterial::setupTexSRTAnmData(&sPathYoshiHeliBtkAnm, modelData, ptrCourseArc);
}

void TMapObjPathYoshiHeli::createModel(JKRSolidHeap *jkrSolidHeap, u32 p2) {
    mAnmObjMaterial.setExModel(&mModel);
    mAnmObjMaterial.setAnmBase(sPathYoshiHeliBtkAnm);
    mAnmObjMaterial.initFrameCtrl(mAnmObjMaterial.mAnmBase);
}

void TMapObjMashBalloon::reset() {
    GeographyObj::resetObject();

    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _58 = 0;
    _160.set(mRotMtx);

    static f32 sMaxSpeed = 10.0f;
    static f32 sAccel = 0.03f;
    static f32 sReachDistance = 2000.0f;

    if (mObjData->mPathID != 0xffff) {
        mPathMove->getNodePosition(&mPos, 0);
        TPathMove *pathMove = mPathMove;

        // This feels really hack-y... why do this with the static
        // values declared earlier?
        f32 maxSpeed = sMaxSpeed;
        f32 halfMaxSpeed = sMaxSpeed * 0.5f;
        pathMove->_14 = sAccel;
        pathMove->_18 = maxSpeed;
        pathMove->_20 = halfMaxSpeed * halfMaxSpeed;
        mPathMove->_20 = sReachDistance * sReachDistance;

        mPathMove->init(&mPos, &mVel);
        mFreeRotate->init(&_160);
        mPathMove->setTargetNode();
        mVel.zero();
    }

    f32 yPos = mPos.y;
    _154 = yPos;
    _150 = yPos;
    _14c = 0;
}


TMapObjUFO::TMapObjUFO(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
#line 475
    JUT_ASSERT_MSG(mObjData->mPathID != 0xffff, "TMapObjUFO: Illegal Path ID");
    setObjFlagSimpleDraw();
    mPathUtil = new TPathUtil(&sObject);
    createSoundMgr();
}

TMapObjUFO::~TMapObjUFO() {
    delete mPathUtil;
}

TPathUtil::~TPathUtil() {}

void TMapObjUFO::reset() {
    GeographyObj::resetObject();

    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    clrObjFlagHidding();
    _58 = 0;
    PSMTXIdentity(_15c);

    const f32 angle = sRotRad;
    const f32 s = sinf(angle);
    const f32 c = cosf(angle);
    _15c[1][1] = 1.0f;
    _15c[0][0] = c;
    _15c[0][2] = s;
    _15c[2][0] = -s;
    _15c[2][2] = c;
    _15c[2][1] = 0.0f;
    _15c[1][2] = 0.0f;
    _15c[1][0] = 0.0f;
    _15c[0][1] = 0.0f;

    mPathUtil->reset();
}

void TMapObjUFO::InitExec() { Observer_FindAndInit(TMapObjUFO, 2); }

void TMapObjUFO::MoveExec() { Observer_FindAndExec(TMapObjUFO, 2); }

void TMapObjUFO::calc() {
    ExecuteState();
    PSMTXConcat(mRotMtx, _15c, mRotMtx);
    getSoundMgr()->setSe(0x40081);
}

void TMapObjUFO::initFunc_Fly() {
    TPathUtil *pathUtil = mPathUtil;
    pathUtil->mDistanceDirty = true;
    pathUtil->setPointIndex(pathUtil->getNextNode());
    
    CrsData::PointData *point = RCMGetCourse()->getCrsData()->getPointData(mPathUtil->mObj->mPathID, mPathUtil->getPointIndex());

    JGeometry::TVec3f target;
    target.set(point->pos);

    JGeometry::TVec3f dir;
    dir.sub(target, mPos);
    dir.normalize();
    mVel.scale(sFlySpeed, dir);
}

void TMapObjUFO::doFunc_Fly() {
    mPos.add(mVel);
    TPathUtil *pathUtil = mPathUtil;

    CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(pathUtil->mObj->mPathID, pathUtil->getPointIndex());

    JGeometry::TVec3f target;
    target.set(pointData->pos);

    JGeometry::TVec3f diff;
    diff.sub(target, mPos);

    if (diff.dot(mVel) <= 0.0f) {
        mPos.set(target);
        setState(1);
    }
    getSoundMgr()->setSe(0x40080);
}

void TMapObjUFO::initFunc_Wait() {
    if (RCMGetCourse()->getCrsData()->getPointData(mObjData->mPathID, mPathUtil->mPointIndex)->linkPoint == 0) {
        setState(0);
    }
}

void TMapObjUFO::doFunc_Wait() {
    if (getStateCount() > sWaitFrame) {
        setState(0);
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
