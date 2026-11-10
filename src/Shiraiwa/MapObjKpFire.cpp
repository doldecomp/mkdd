#include "Shiraiwa/Objects/MapObjKpFire.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3D/J3DFrameCtrl.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JParticle/JPAMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/JUtility/JUTNameTab.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/AnmController.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"
#include "types.h"

StateObserver::StateFuncSet<TMapObjKpFire> TMapObjKpFire::sTable[6] = {
    {0, &TMapObjKpFire::initFunc_Start, &TMapObjKpFire::doFunc_Start},
    {1, &TMapObjKpFire::initFunc_Walk, &TMapObjKpFire::doFunc_Walk},
    {2, &TMapObjKpFire::initFunc_Stop, &TMapObjKpFire::doFunc_Stop},
    {3, &TMapObjKpFire::initFunc_FireOpen, &TMapObjKpFire::doFunc_FireOpen},
    {4, &TMapObjKpFire::initFunc_Fire, &TMapObjKpFire::doFunc_Fire},
    {5, &TMapObjKpFire::initFunc_FireClose, &TMapObjKpFire::doFunc_FireClose},
};
f32 TMapObjKpFire::sAccel = 40.0f;
f32 TMapObjKpFire::sMaxSpeed = 40.0f;
f32 TMapObjKpFire::sShakeAccel = 40.0f;
f32 TMapObjKpFire::sShakeMaxSpeed = 50.0f;
s16 TMapObjKpFire::sShakeFrame = 25;
s16 TMapObjKpFire::sWaitFrame = 20;
s16 TMapObjKpFire::sFireInterval = 60;
u8 TMapObjKpFire::sFireLineNum = 1;
f32 TMapObjKpFire::sShakeMaxLength = 15000.0f;
const char *TMapObjKpFire::scFireJointName = "loc_fire";
u32 TMapObjKpFire::sFireJointNo;
J3DAnmTransform *TMapObjKpFire::sKpFireBckAnmTrans;
J3DMtxCalc *TMapObjKpFire::sKpFireBckMtxCalc;

TMapObjKpFire::TMapObjKpFire(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
    createSoundMgr();
    mPathUtil = new TPathUtil(&sObject);

    // Initialise each fireball instance.
    for (int i = 0; i < 18; i++) {
        mMapObjKpFireBall[i] = (TMapObjKpFireBall *)GetGeoObjMgr()->createSubObj(0x1260);
    }
}

TMapObjKpFire::~TMapObjKpFire() {
    delete mPathUtil;
}

void TMapObjKpFire::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjTrans::setupTransAnmData(&sKpFireBckAnmTrans, &sKpFireBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/KpFire1.bck"));
}

void TMapObjKpFire::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);

    AnmController *anmCtrl = mAnmCtrl;
    anmCtrl->mTrans = new AnmControlTrans;
    anmCtrl->mTrans->initAnm(1, &mModel);

    mAnmCtrl->RegisterTrans(0, sKpFireBckAnmTrans, sKpFireBckMtxCalc);
    mAnmCtrl->getFrameCtrl(0)->setAttribute(0);

    sFireJointNo = mModel.getModelData()->getJointName()->getIndex(scFireJointName);

    if (SiUtil::getConsoleNum() == 1) {
        sAccel = 40.0f;
        sMaxSpeed = 40.0f;
    } else {
        sAccel = 2.0f;
        sMaxSpeed = 10.0f;
    }
}

void TMapObjKpFire::reset() {
    GeographyObj::resetObject();
    StateObserver::ResetState();
    mPathUtil->reset();

    CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(mPathUtil->mObj->mPathID, 0);
    mPos.set(pointData->pos);

    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();

    JGeometry::TVec3f zPos;
    mRotMtx.getZDir(zPos);
    zPos.normalize();

    mAnmCtrl->Reset();
    mAnmCtrl->mTrans->getFrameCtrl(0)->setRate(0.0f);

    for (int i = 0; i < 18; i++) {
        mMapObjKpFireBall[i]->setVelDir(zPos);
    }
}

void TMapObjKpFire::calc() {
    ExecuteState();
}

void TMapObjKpFire::initFunc_Start() {}

void TMapObjKpFire::doFunc_Start() {
    if (getStateCount() > mObjData->mParam2) {
        setState(1);
    }
}

void TMapObjKpFire::initFunc_Walk() {
    TPathUtil *pathUtil = mPathUtil;
    pathUtil->mDistanceDirty = true;
    pathUtil->setPointIndex(pathUtil->getNextNode());
}

void TMapObjKpFire::doFunc_Walk() {

    TPathUtil *pathUtil = mPathUtil;
    const u16 nextNode = pathUtil->getNextNode();
    CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(pathUtil->mObj->mPathID, nextNode);

    JGeometry::TVec3f pathPointPos;
    pathPointPos.set(pointData->pos);

    JGeometry::TVec3f posDiff;
    posDiff.sub(pathPointPos, mPos);

    if (posDiff.length() < 50.0f) {
        u16 nextNodeCheckFire = mPathUtil->getNextNode();
        if (checkFire(nextNodeCheckFire)) {
            setState(2);
        } else {
            setState(1);
        }
    }

    posDiff.setLength(sAccel);
    mVel.add(posDiff);
    if (mVel.length() > sMaxSpeed) {
        mVel.setLength(sMaxSpeed);
    }
    mPos.add(mVel);
    mSoundMgr->setSe(0x40044);
}

void TMapObjKpFire::initFunc_Stop() {
    mSoundMgr->setSe(0x40038);
}

void TMapObjKpFire::doFunc_Stop() {
    CrsData::PointData *pointData;
    JGeometry::TVec3f pointDataPos;
    JGeometry::TVec3f posDiff;

    if (getStateCount() < sShakeFrame) {
        TPathUtil *pathUtil = mPathUtil;
        const u16 nextNode = pathUtil->getNextNode();
        pointData = RCMGetCourse()->getCrsData()->getPointData(pathUtil->mObj->mPathID, nextNode);
        pointDataPos.set(pointData->pos);

        posDiff.sub(pointDataPos, mPos);
        posDiff.setLength(sShakeAccel);
        mVel.add(posDiff);

        if (mVel.length() > sShakeMaxSpeed) {
            mVel.setLength(sShakeMaxSpeed);
        }
        mPos.add(mVel);
    }

    if (getStateCount() > sShakeFrame + sWaitFrame) {
        mVel.zero();
        TPathUtil *pathUtil = mPathUtil;
        const u16 uVar5 = pathUtil->getNextNode();
        pointData = RCMGetCourse()->getCrsData()->getPointData(pathUtil->mObj->mPathID, uVar5);
        mPos.set(pointData->pos);
        setState(3);
    }
}

bool TMapObjKpFire::checkFire(u16 point) {
    return !(RCMGetCourse()->getCrsData()->getPointData(mObjData->mPathID, point)->linkPoint);
}

void TMapObjKpFire::initFunc_FireOpen() {
    J3DFrameCtrl *frameCtrl = mAnmCtrl->getFrameCtrl(0);
    frameCtrl->setFrame(0.0f);
    frameCtrl->setRate(1.0f);
}

void TMapObjKpFire::doFunc_FireOpen() {
    if (mAnmCtrl->getFrameCtrl(0)->getFrame() > 12.0f) {
        setState(4);
    }
}

void TMapObjKpFire::initFunc_Fire() {}

void TMapObjKpFire::doFunc_Fire() {
    u32 stateCount = getStateCount();
    if (stateCount % sFireInterval != 0) {
        return;
    }
    
    if (stateCount / sFireInterval < sFireLineNum) {
        for (int i = 0; i < 18; i++) {
            if (mMapObjKpFireBall[i]->getState() == 0) {
                JGeometry::TVec3f firePos;
                getFirePos(&firePos);
                makeFireEmitter(firePos);
                mSoundMgr->setSe(0x40035);
                ObjUtility::executeShakeCamera(firePos, sShakeMaxLength, 0.4f, 5000.0f);
                mMapObjKpFireBall[i]->fire(firePos);
                return;
            }
            if (i == 17) {
                #line 326
                JUT_WARNING_F2("%s", "TMapObjKpFire: Too Many Fire Balls.\n");
            }
        }
    } else {
        setState(5);
    }
}

void TMapObjKpFire::makeFireEmitter(const JGeometry::TVec3f &param_1) {
    static JGeometry::TVec3f emScale(8.5f, 8.5f, 8.5f);

    static const char *emName[2] = {
        "mk_koopaz_a",
        "mk_koopaz_b",
    };
    static const bool isCameraClip[2] = {0, 1};

    for (int i = 0; i < 2; i++) {
        JPABaseEmitter *emitter = isCameraClip[i]
            ? JPEffectMgr::getEffectMgr()->createEmtCameraClip(emName[i], param_1)
            : JPEffectMgr::getEffectMgr()->createEmt(emName[i], param_1);

        if (emitter != nullptr) {
            JPASetRMtxfromMtx(mRotMtx, emitter->mGlobalRot);
            emitter->setGlobalScale(emScale);
        }
    }
}

const void TMapObjKpFire::getFirePos(JGeometry::TVec3f *firePos) {
    MtxPtr jointAnmMtx = mModel.getModel()->getAnmMtx(sFireJointNo);
    firePos->set(jointAnmMtx[0][3], jointAnmMtx[1][3], jointAnmMtx[2][3]);
}

void TMapObjKpFire::initFunc_FireClose() {}

void TMapObjKpFire::doFunc_FireClose() {
    if (mAnmCtrl->getFrameCtrl(0)->checkState(1)) {
        setState(1);
    }
}

void TMapObjKpFire::InitExec() { Observer_FindAndInit(TMapObjKpFire, 6); }

void TMapObjKpFire::MoveExec() { Observer_FindAndExec(TMapObjKpFire, 6); }

StateObserver::StateFuncSet<TMapObjKpFireBall> TMapObjKpFireBall::sTable[4] = {
    {0, &TMapObjKpFireBall::initFunc_Hide, &TMapObjKpFireBall::doFunc_Hide},
    {1, &TMapObjKpFireBall::initFunc_Appear, &TMapObjKpFireBall::doFunc_Appear},
    {2, &TMapObjKpFireBall::initFunc_Fly, &TMapObjKpFireBall::doFunc_Fly},
    {3, &TMapObjKpFireBall::initFunc_Disappear, &TMapObjKpFireBall::doFunc_Disappear},
};

s16 TMapObjKpFireBall::sMaxLife = 500;
s16 TMapObjKpFireBall::sAppearFrame = 1;
f32 TMapObjKpFireBall::sMaxSpeed = 30.0f;
f32 TMapObjKpFireBall::sVelocity0 = 140.0f;
f32 TMapObjKpFireBall::sDecelSpeed = -1.0f;
f32 TMapObjKpFireBall::sOrgScale = 1.0f;
f32 TMapObjKpFireBall::sRotRadRange = 0.05f;
f32 TMapObjKpFireBall::sRotRadMin = 0.1f;
J3DAnmTextureSRTKey *TMapObjKpFireBall::sKpFireBallBtkAnm;
J3DAnmTevRegKey *TMapObjKpFireBall::sKpFireBallTevRegKey;

TMapObjKpFireBall::TMapObjKpFireBall() : TMapObjHioNode(0x1260) {

    createSoundMgr();
}

TMapObjKpFireBall::~TMapObjKpFireBall() {}

void TMapObjKpFireBall::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x1000200, 0);

    mAnmObjMaterialOne.setExModel(&mModel);
    mAnmObjMaterialOne.setAnmBase(sKpFireBallBtkAnm);
    mAnmObjMaterialOne.initFrameCtrl(mAnmObjMaterialOne.getAnmBase());

    mAnmObjMaterialTwo.setExModel(&mModel);
    mAnmObjMaterialTwo.setAnmBase(sKpFireBallTevRegKey);
    mAnmObjMaterialTwo.initFrameCtrl(mAnmObjMaterialTwo.getAnmBase());
}

void TMapObjKpFireBall::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    mAnmObjMaterialOne.setupTexSRTAnmData(&sKpFireBallBtkAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/KpFire1Fire.btk"));
    mAnmObjMaterialOne.setupTevRegAnmData(&sKpFireBallTevRegKey, modelData, ObjUtility::getPtrCourseArc("/Objects/KpFire1Fire.brk"));
}

void TMapObjKpFireBall::InitExec() { Observer_FindAndInit(TMapObjKpFireBall, 4); }

void TMapObjKpFireBall::MoveExec() { Observer_FindAndExec(TMapObjKpFireBall, 4); }

void TMapObjKpFireBall::setVelDir(const JGeometry::TVec3f &velDir) {
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirZ;
    JGeometry::TVec3f vecDirY;
    
    mVel.set(velDir);
    vecDirZ.set(velDir);
    vecDirZ.normalize();
    vecDirY.set(0.0f, 1.0f, 0.0f);
    vecDirX.cross(vecDirY, vecDirZ);
    vecDirX.normalize();
    mRotMtx.setXYZDir(vecDirX, vecDirY, vecDirZ);
}

void TMapObjKpFireBall::reset() {
    GeographyObj::resetObject();
    ResetState();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();

    _58 = 13;
    mAnmObjMaterialOne.resetFrame();
    mAnmObjMaterialTwo.resetFrame();
    mEmitter = nullptr;
}

void TMapObjKpFireBall::calc() {
    ExecuteState();
    checkItemHitting();

    mAnmObjMaterialOne.update();
    mAnmObjMaterialTwo.update();

    if (mEmitter != nullptr) {
        JGeometry::TPos3f rMtx;
        rMtx.set(mRotMtx);
        rMtx[0][3] = mPos.x;        // Why not use `rMtx.setTrans()`?
        rMtx[1][3] = mPos.y;
        rMtx[2][3] = mPos.z;
        JPASetRMtxTVecfromMtx(rMtx, mEmitter->mGlobalRot, &mEmitter->mGlobalTrs);
    }
}

void TMapObjKpFireBall::checkItemHitting() {
    if (!tstItemHitting()) {
        return;
    }

    switch (getColItemObj()->getKind()) {
        case 1:
        case 7:
            makeBreakEmitter(3);
            setState(3);
            break;
        default:
            break;
    }
}

void TMapObjKpFireBall::update() {
    mAnmObjMaterialOne.anmFrameProc();
    mAnmObjMaterialTwo.anmFrameProc();
    GeographyObj::update();
}

void TMapObjKpFireBall::fire(const JGeometry::TVec3f &pos) {
    _198.set(pos);
    setState(1);
    SiUtil::setRandomStartFrame(mAnmObjMaterialOne.getFrameCtrl(), getGeoRnd());
}

void TMapObjKpFireBall::createColModel(J3DModelData *modelData) {
    createBoundsSphere(350.0f, 1.0f);
}

void TMapObjKpFireBall::doKartColCallBack(int kartIdx) {
    if ((GetKartCtrl()->GetKartStatus(kartIdx) & 0x40000)) {
        makeBreakEmitter(3);
        setState(3);
    }
}

void TMapObjKpFireBall::initFunc_Hide() {
    setObjFlagHidding();
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();
}

void TMapObjKpFireBall::doFunc_Hide() {}

void TMapObjKpFireBall::initFunc_Appear() {    
    clrObjFlagHidding();
    setAllCheckKartHitFlag();
    setObjFlagCheckItemHitting();
    
    mScale.setAll(0.0f);
    mPos.set(_198);
    mVel.setLength(sVelocity0);
    mEmitter = JPEffectMgr::getEffectMgr()->createEmtCameraClip("mk_kpbballTail", mPos);
}

void TMapObjKpFireBall::doFunc_Appear() {
    f32 normalRange = SiUtil::getNormalRange(getStateCount(), 0.0f, sAppearFrame) + 0.001f;
    if (normalRange >= 1.0f) {
        normalRange = 1.0f;
        setState(2);
    }
    mScale.setAll(normalRange * sOrgScale);
    mPos.add(mVel);
    mSoundMgr->setSe(0x40034);
}

void TMapObjKpFireBall::initFunc_Fly() {}

void TMapObjKpFireBall::doFunc_Fly() {
    mPos.add(mVel);

    if (getStateCount() > sMaxLife) {
        makeBreakEmitter(4);
        setState(3);
    }

    f32 speed = mVel.length();
    if (speed > sMaxSpeed) {
        speed += sDecelSpeed;

        if (speed < sMaxSpeed) {
            speed = sMaxSpeed;
        }

        mVel.setLength(speed);
    }

    f32 angle = sRotRadMin + sRotRadRange * SiUtil::getNormalRange(speed, sMaxSpeed, sVelocity0);

    JGeometry::TPos3f rotation;
    PSMTXIdentity(rotation);

    // might be setRotate?
    f32 s = sinf(angle);
    f32 c = cosf(angle);

    rotation[0][0] = c;
    rotation[0][1] = -s;
    rotation[1][0] = s;
    rotation[1][1] = c;
    rotation[2][2] = 1.0f;
    rotation[2][1] = 0.0f;
    rotation[1][2] = 0.0f;
    rotation[2][0] = 0.0f;
    rotation[0][2] = 0.0f;

    PSMTXConcat(mRotMtx, rotation, mRotMtx);
    getSoundMgr()->setSe(0x40034);
}

void TMapObjKpFireBall::initFunc_Disappear() {
    clrAllCheckKartHitFlag();
    clrObjFlagCheckItemHitting();

    if (mEmitter != nullptr) {
        JPEffectMgr::getEffectMgr()->deleteEmt(mEmitter);
        mEmitter = nullptr;
    }
}

void TMapObjKpFireBall::makeBreakEmitter(int param_1) {
    static JGeometry::TVec3f emScale(5.0f, 5.0f, 5.0f);

    static const char *emName[4] = {
        "mk_kpbball_a",
        "mk_kpbball_b",
        "mk_kpbball_c",
        "mk_kpbball_d",
    };
    static const bool isCameraClip[4] = {0, 0, 0, 1};

    for (int i = 0; i < param_1; i++) {
        JPABaseEmitter *emitter = isCameraClip[i]
            ? JPEffectMgr::getEffectMgr()->createEmtCameraClip(emName[i], mPos)
            : JPEffectMgr::getEffectMgr()->createEmt(emName[i], mPos);

        if (emitter != nullptr) {
            emitter->setGlobalScale(emScale);
        }
    }
}

void TMapObjKpFireBall::doFunc_Disappear() {
    mSoundMgr->setSe(0x40037);
    setState(0);
}

#include "JSystem/JAudio/JASFakeMatch2.h"
