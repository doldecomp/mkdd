#include "Shiraiwa/Objects/MapObjFireObj.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/J3D/J3DFrameCtrl.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/JGeometry/Quat.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"
#include "dolphin/mtx.h"
#include "types.h"

StateObserver::StateFuncSet<TMapObjFireObj> TMapObjFireObj::sTable[4] = {
    { 0, &TMapObjFireObj::initFunc_Normal, &TMapObjFireObj::doFunc_Normal },
    { 1, &TMapObjFireObj::initFunc_Accel, &TMapObjFireObj::doFunc_Accel },
    { 2, &TMapObjFireObj::initFunc_Drive, &TMapObjFireObj::doFunc_Drive },
    { 3, &TMapObjFireObj::initFunc_Decel, &TMapObjFireObj::doFunc_Decel },
};

J3DAnmTextureSRTKey *TMapObjFireBall::sFireBallBtkAnm;
J3DAnmTextureSRTKey *TMapObjKpFireBarBall::sKpFireBarBallBtkAnm;
J3DAnmTevRegKey *TMapObjKpFireBarBall::sKpFireBarBallBrkAnm;
s16 TMapObjFireObj::sDriveTime = 0x3c;


TMapObjFireBallBase::TMapObjFireBallBase(u32 id) : GeographyObj(id) {
    _16c = 0.0f;
    _170 = false;
    _171 = false;
}

TMapObjFireBallBase::~TMapObjFireBallBase() {}

void TMapObjFireBallBase::reset() {
    SiUtil::setItemReaction(GetGeoObjMgr()->getGeoHitTable(), this);
    setAllCheckKartHitFlag();
    setObjFlagCheckItemHitting();
    _190.set(0.0f, 0.0f, 0.0f, 1.0f);
    _174 = 1.0f;
    _58 = 0xd;
    _170 = false;
    _16c = nullptr;
}

void TMapObjFireBallBase::createColModel(J3DModelData *) {
    createBoundsSphere(100.0f, 1.0f);
}

void TMapObjFireBall::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x200, 0);
    anmObjMaterial.setExModel(&mModel);
    anmObjMaterial.setAnmBase(sFireBallBtkAnm);
    anmObjMaterial.initFrameCtrl(anmObjMaterial.getAnmBase());
}

void TMapObjFireBall::reset() {
    stRandom *geoRnd;
    TMapObjFireBallBase::reset();
    anmObjMaterial.resetFrame();
    SiUtil::setRandomStartFrame(anmObjMaterial.getFrameCtrl(), GeographyObj::getGeoRnd());
}

void TMapObjFireBall::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/FireBar1Fire.btk");
    J3DAnmObjMaterial::setupTexSRTAnmData(&sFireBallBtkAnm, modelData, ptrCourseArc);
}

void TMapObjKpFireBarBall::reset() {
    TMapObjFireBallBase::reset();
    anmObjMaterial.resetFrame();
    SiUtil::setRandomStartFrame(anmObjMaterial.getFrameCtrl(), GeographyObj::getGeoRnd());
    mAnmObjMaterial.resetFrame();
    SiUtil::setRandomStartFrame(mAnmObjMaterial.getFrameCtrl(), GeographyObj::getGeoRnd());
    PSMTXIdentity(mRotMtx);
    mEmitter = nullptr;
}

void TMapObjKpFireBarBall::createColModel(J3DModelData *modelData) {
    createBoundsSphere(160.0f, 1.0f);
}

void TMapObjKpFireBarBall::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x80200, false);
    anmObjMaterial.setExModel(&mModel);
    anmObjMaterial.setAnmBase(sKpFireBarBallBtkAnm);
    anmObjMaterial.initFrameCtrl(anmObjMaterial.getAnmBase());
    mAnmObjMaterial.setExModel(&mModel);
    mAnmObjMaterial.setAnmBase(sKpFireBarBallBrkAnm);
    mAnmObjMaterial.initFrameCtrl(mAnmObjMaterial.getAnmBase());
}

void TMapObjKpFireBarBall::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();

    void *ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/KpFireBar1Fire.btk");
    J3DAnmObjMaterial::setupTexSRTAnmData(&sKpFireBarBallBtkAnm, modelData, ptrCourseArc);

    ptrCourseArc = ObjUtility::getPtrCourseArc("/Objects/KpFireBar1Fire.brk");
    J3DAnmObjMaterial::setupTevRegAnmData(&sKpFireBarBallBrkAnm, modelData, ptrCourseArc);
}

void TMapObjKpFireBarBall::update() {
    mAnmObjMaterial.anmFrameProc();
    TMapObjFireBallBase::update();
}

void TMapObjKpFireBarBall::calc() {
    mAnmObjMaterial.getFrameCtrl()->update();

    _190.transform(_178);

    JGeometry::TVec3f pos;
    pos.set(_178);

    if (_174 != 1.0f) {
        pos.scale(_174);
    }

    // Couldn't collapse this with kpFireBarBallRotMtx.setTrans(_184);
    kpFireBarBallRotMtx[0][3] = _184.x;
    kpFireBarBallRotMtx[1][3] = _184.y;
    kpFireBarBallRotMtx[2][3] = _184.z;

    PSMTXMultVec(kpFireBarBallRotMtx, &pos, &mPos);

    if (mEmitter != nullptr) {
        mEmitter->setGlobalTranslation(mPos);
    } else {
        mEmitter = JPEffectMgr::getEffectMgr()->createEmtCameraClip("mk_kpcFB", mPos);
    }

    if (tstItemHitting()) {
        int kind = getColItemObj()->getKind();
        switch (kind) {
            case 7:
            case 8:
                _170 = true;
                break;
        
        }
    }

    anmObjMaterial.update();

    if (_171) {
        if (getSoundMgr() != nullptr) {
            getSoundMgr()->setSe(0x40032);
        }
    }
}

void TMapObjFireBallBase::setCurrentViewNo(u32 viewNo) {
    mModel.setCurrentViewNo(viewNo);
    Mtx effectMtx;
    ObjUtility::getCamDependLightMtx(viewNo, mPos, effectMtx);
    mModel.setEffectMtx(effectMtx, 1);
}

void TMapObjFireBallBase::calc() {
    _190.transform(_178);

    JGeometry::TVec3f pos;
    pos.set(_178);

    if (_174 != 1.0f) {
        pos.scale(_174);
    }

    mRotMtx.mMtx[0][3] = _184.x;
    mRotMtx.mMtx[1][3] = _184.y;
    mRotMtx.mMtx[2][3] = _184.z;

    PSMTXMultVec(mRotMtx, &pos, &mPos);

    if (tstItemHitting()) {
        int kind = getColItemObj()->getKind();
        switch (kind) {
            case 7:
            case 8:
                _170 = true;
                break;
        
        }
    }

    anmObjMaterial.update();

    if (_171) {
        if (getSoundMgr() != nullptr) {
            getSoundMgr()->setSe(0x40032);
        }
    }

    if (getShadowModel() != nullptr) {
        moveShadowModel();
    }
}

void TMapObjFireBallBase::doKartColCallBack(int idx) {
    if (_170 != false) {
        return;
    } else if ((GetKartCtrl()->GetKartStatus(idx) & 0x40000)) {
        _170 = true;
    }
}

void TMapObjFireBallBase::update() {
    anmObjMaterial.anmFrameProc();
    GeographyObj::update();
}

void TMapObjFireBallBase::applyDriveState() {}

TMapObjFireObj::TMapObjFireObj(const CrsData::SObject &sObject, u32 id) : TMapObjHioNode(sObject) {
    fireBalls = nullptr;
    fireBallCount = 0;
    if (id == 0x106a) {
        setObjFlagSimpleDraw();
    }

    u8 unknownParam = sObject.mParam1;
    if ((unknownParam & 0xFFu) < 1) {
        unknownParam = 1;
    }
    makeFire((u8)unknownParam, id);
}

TMapObjFireObj::~TMapObjFireObj() {
    for (int i = 0; i < fireBallCount; i++) {
        delete fireBalls[i];
    }
    delete[] fireBalls;
}

void TMapObjFireObj::reset() {
    resetObject();
    ResetState();
    resetFire();
}

void TMapObjFireObj::resetFire() {
    for (int i = 0; i < fireBallCount; i++) {
        fireBalls[i]->_178.setAll(0.0f);
        fireBalls[i]->setRMtx(mRotMtx);
        fireBalls[i]->_184.set(mPos);
        fireBalls[i]->mScale.set(mScale);
    }
    _160.set(0.0f, 0.0f, 0.0f, 1.0f);
    _170.set(0.0f, 0.0f, 0.0f, 1.0f);
}

void TMapObjFireObj::calc() {
    ExecuteState();
    if (checkDrive()) {
        setState(1);
    }
}

// WHY?!
bool TMapObjFireObj::checkDrive() {
    bool found = false;
    bool result = false;
    int i = 0;

    while (fireBallCount > i) {
        found = false;

        if (fireBalls[i]->_170) {
            found = true;
            fireBalls[i]->_170 = 0;
        }

        if (found) {
            result = true;
            break;
        }

        i++;
    }

    return result;
}

void TMapObjFireObj::setRotate(const JGeometry::TQuat4f &r) {
    for (int i = 0; i < fireBallCount; i++) {
        fireBalls[i]->_190 = r;
    }
}

void TMapObjFireObj::initFunc_Normal() {
    setRotate(_160);
}

void TMapObjFireObj::doFunc_Normal() {}

void TMapObjFireObj::initFunc_Accel() {}

void TMapObjFireObj::doFunc_Accel() {
    JGeometry::TQuat4f quat;
    
    quat.x = _180 * (_170.x - _160.x) + _160.x;
    quat.y = _180 * (_170.y - _160.y) + _160.y;
    quat.z = _180 * (_170.z - _160.z) + _160.z;
    quat.w = _180 * (_170.w - _160.w) + _160.w;

    quat.normalize(quat);
    setRotate(quat);

    _180 += 0.1f;
    if (_180 > 1.0f) {
        _180 = 1.0f;
        setState(2);
    }
}

void TMapObjFireObj::initFunc_Drive() {
    setRotate(_170);
}

void TMapObjFireObj::doFunc_Drive() {
    if (getStateCount() > sDriveTime) {
        setState(3);
    }
}

void TMapObjFireObj::initFunc_Decel() {}

void TMapObjFireObj::doFunc_Decel() {
    JGeometry::TQuat4f quat;
    
    quat.x = _180 * (_170.x - _160.x) + _160.x;
    quat.y = _180 * (_170.y - _160.y) + _160.y;
    quat.z = _180 * (_170.z - _160.z) + _160.z;
    quat.w = _180 * (_170.w - _160.w) + _160.w;

    quat.normalize(quat);
    setRotate(quat);

    _180 -= 0.01f;
    if (_180 < 0.0f) {
        _180 = 0.0f;
        setState(0);
    }
}

void TMapObjFireObj::makeFire(u8 param_1, u32 param_2) {
    fireBallCount = param_1;
    fireBalls = new TMapObjFireBallBase *[fireBallCount]();
    for (int i = 0; i < fireBallCount; i++) {
        fireBalls[i] = (TMapObjFireBallBase *)GetGeoObjMgr()->createSubObj(param_2);
        #line 558
        JUT_ASSERT_MSG(fireBalls[i] != nullptr, "Failed to make FireBallBase Object.")
    }
}

void TMapObjFireObj::InitExec() { Observer_FindAndInit(TMapObjFireObj, 4); }
void TMapObjFireObj::MoveExec() { Observer_FindAndExec(TMapObjFireObj, 4); }

#include "JSystem/JAudio/JASFakeMatch2.h"
