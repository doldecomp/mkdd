#include "Shiraiwa/Objects/MapObjPakkun.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/ItemThunder.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/SiUtil.h"


TAnmInfo TMapObjPakkun::sAnmInfos[3] = {
    { "/Objects/Pakkun_wait.bca", nullptr, nullptr, 2, 10, 0, 1, 0 },
    { "/Objects/Pakkun_attack.bca", nullptr, nullptr, 0, 10, 0, 0, 0 },
    { "/Objects/Pakkun_damage.bca", nullptr, nullptr, 2, 30, 0, 0, 0 }
};

StateObserver::StateFuncSet<TMapObjPakkun> TMapObjPakkun::sTable[5] = {
    { 0, &TMapObjPakkun::initFunc_Stand, &TMapObjPakkun::doFunc_Stand },
    { 1, &TMapObjPakkun::initFunc_Search, &TMapObjPakkun::doFunc_Search },
    { 2, &TMapObjPakkun::initFunc_Attack, &TMapObjPakkun::doFunc_Attack },
    { 3, &TMapObjPakkun::initFunc_Rest, &TMapObjPakkun::doFunc_Rest },
    { 4, &TMapObjPakkun::initFunc_Damage, &TMapObjPakkun::doFunc_Damage }
};

const JGeometry::TVec3f TMapObjPakkun::scHeadCenterOffset(73.0f, 0.0f, -6.0f);
f32 TMapObjPakkun::sThunderDamageRandeMax = 30.0f;
f32 TMapObjPakkun::sTurnPow = 0.02f;
f32 TMapObjPakkun::sTurnPowFar = 0.1f;
f32 TMapObjPakkun::sTurnAttackScale = 2.0f;
s16 TMapObjPakkun::sRestFrame = 60;
s16 TMapObjPakkun::sSearchFrame = 30;
TPakkunSupervisor *TMapObjPakkun::sSupervisor;
const s8 TMapObjPakkun::sAnmTable[5] = {
    0, 0, 1, 0, 2
};
const char *TMapObjPakkun::scRootJointName = "skil_root";
const char *TMapObjPakkun::scHeadJointName = "kuki_4";

void TMapObjPakkunColHead::reset() {
    resetObject();
    if (RCMGetManager()->getRaceMode() != 1) {
        setObjFlagCheckItemHitting();
        setAllCheckKartHitFlag();
        _58 = 0x12;
    } else {
        clrObjFlagCheckItemHitting();
        clrAllCheckKartHitFlag();
        _58 = 0;
    }
    _14c = false;
}

void TMapObjPakkunColHead::createColModel(J3DModelData *modelData) {
    createBoundsSphere(85.0f, mScale.y);
}

void TMapObjPakkunColHead::setScale(const JGeometry::TVec3f &scale) {
    
}

void TMapObjPakkunColHead::doKartColCallBack(int kartIdx) {
    if (!(GetKartCtrl()->GetKartStatus(kartIdx) & 0x40000)) {
        return;
    }
    _14c = true;
}

bool TMapObjPakkunColHead::isDamage() { }

void TMapObjPakkunColHead::calc() {
    TMapObjPakkunColHead::checkItemHitting();
    moveShadowModel();
}

void TMapObjPakkunColHead::setPosition(const JGeometry::TVec3f &position) {}

void TMapObjPakkunColHead::getKartThrowDirPow(JGeometry::TVec3f *throwDir, f32 *pow, int unused) {
    #line 151
    JUT_ASSERT(mBody != 0);

    if (throwDir == nullptr) {
        return;
    }

    JGeometry::TVec3f bodyPos;
    mBody->getPosition(&bodyPos);
    
    throwDir->sub(mPos, bodyPos);
    throwDir->y = 0.0f;

    if (throwDir->isZero()) {
        throwDir->set(mVel);
        throwDir->y = 0.0f;
        if (throwDir->isZero()) {
            throwDir->set(1.0f, 0.0f, 0.0f);
        }
    }
    throwDir->normalize();
}

void TMapObjPakkunColHead::checkItemHitting() {
    if (!tstItemHitting()) {
        return;
    }

    // TODO: Check if this can become an inline.
    switch (getColItemObj()->getKind()) {
        case 0:
        case 1:
        case 2:
        case 7:
        case 8:
        case 9:
        case 11:
            _14c = true;
            break;

        default:
            return;
    }
}

TMapObjPakkun::TMapObjPakkun(const CrsData::SObject &sObject) : TMapObjHioNode(sObject), StateObserver(), JKRDisposer(), mLinkPakkun(this) {
    NewAnmCtrl();
    createSoundMgr();
    mAnmPlayer.resetAnimations(sAnmInfos, 3);

    if (getSupervisor() == nullptr) {
        sSupervisor = new TPakkunSupervisor;
    }
    sSupervisor->entry(this);
    _244 = (TMapObjPakkunColHead *)GetGeoObjMgr()->createGenSubObj(0xe81);
    _244->mBody = this;
    _1b4.set(mRotMtx);
}

TMapObjPakkun::~TMapObjPakkun() {
    sSupervisor = nullptr;
}

void TMapObjPakkun::reset() {
    GeographyObj::resetObject();
    ResetState();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = 1;
    getAnmCtrl()->Reset();

    mAnmPlayer.init(getAnmCtrl(), sAnmInfos, 3);
    SiUtil::setRandomStartFrame(getAnmCtrl()->getFrameCtrl(0), getGeoRnd());

    _198 = mObjData->mParam1;
    if (_198 == 0) {
        _198 = 300;
    }

    f32 fVar1 = mObjData->mParam2;
    if (fVar1 <= 0.0f) {
        fVar1 = 2500.0f;
    }
    _19c = fVar1;

    fVar1 = mObjData->mParam3;
    if (fVar1 <= 0.0f) {
        fVar1 = 1500.0f;
    }
    _1a0 = fVar1;

    mRotMtx.getZDir(_1a8);
    _1b4.set(mRotMtx);
    PSMTXIdentity(_1e4);
    PSMTXInverse(mRotMtx, _1e4);
    PSMTXConcat(_1e4, _1b4, _1b4);

    mKartIdx = -1;
    _19a = 0;
    _1a6 = 0;

    JGeometry::TPos3f tempMtx;
    PSMTXIdentity(tempMtx);
    PSMTXInverse(mRotMtx, _214);
    PSMTXScale(tempMtx, 1.0f / mScale.x, 1.0f / mScale.y, 1.0f / mScale.z);
    PSMTXConcat(_214, tempMtx, _214);

    TMapObjPakkunColHead* colHead = _244;
    colHead->mScale.set(mScale);
    colHead->getBounds(0)->setScale(mScale);
}

void TMapObjPakkun::calc() {
    ExecuteState();
    mAnmPlayer.update();
}

void TMapObjPakkun::loadAnimation() {
    mAnmPlayer.loadAnimations(sAnmInfos, 3, mModel.getModelData(), ResMgr::mcArcCourse);
}

void TMapObjPakkun::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);
    mAnmPlayer.registAnimations(getAnmCtrl(), &mModel, sAnmInfos, 3);
    setCallBack(scRootJointName, (J3DJointCallBack2)rotCallBack);       // TODO: Eliminate cast.
    if (RCMGetManager()->getRaceMode() != 1) {
        setCallBack(scHeadJointName, (J3DJointCallBack2)headCallBack);  // TODO: Eliminate cast.
    }
}

void TMapObjPakkun::createColModel(J3DModelData *modelData) {
    createBoundsCylinder(90.0f, 200.0f, mScale.x);
}

void TMapObjPakkun::InitExec() { Observer_FindAndInit(TMapObjPakkun, 5); }

void TMapObjPakkun::MoveExec() { Observer_FindAndExec(TMapObjPakkun, 5); }

void TMapObjPakkun::changeAllState(u16 state) {}

void TMapObjPakkun::doKartColCallBack(int kartIdx) {
    if ((GetKartCtrl()->getKartBody(kartIdx)->mCarStatus & 0x40000)) {
        _19a = 0;
        changeDamageProc();
    }
}

void TMapObjPakkun::changeDamageProc() {
    getAnmCtrl()->ChangeBlendTransAnm(2, 1, 1);
    setState(4);
}

void TMapObjPakkun::initFunc_Stand() {
    clrObjFlagHidding();
    mKartIdx = -1;
}

void TMapObjPakkun::doFunc_Stand() {
    bool temp;
    
    TMapObjPakkun::checkItemHitting();
    TMapObjPakkunColHead *pakkunColHead = _244;

    temp = pakkunColHead->_14c;
    pakkunColHead->_14c = false;

    if (temp != false) {
        _19a = 0;

        setState(4);
        if (sAnmTable[4] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[4];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }

    s16 nearKartCircle = SiUtil::searchNearKartCircle(mPos, _19c);
    if (nearKartCircle != -1) {
        mKartIdx = nearKartCircle;

        setState(1);
        if (sAnmTable[1] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[1];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}

void TMapObjPakkun::initFunc_Attack() {
    getSoundMgr()->setSe(0x40058);
}

void TMapObjPakkun::doFunc_Attack() {
    TMapObjPakkun::checkItemHitting();
    const bool temp = _244->_14c;
    _244->_14c = false;
    if (temp != false) {
        _19a = 0;

        setState(4);
        if (sAnmTable[4] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[4];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }

    JGeometry::TVec3f kartPos;
    ObjUtility::getKartPos(mKartIdx, &kartPos);
    turnTo(kartPos, sTurnAttackScale);
    if (mAnmPlayer.isCurAnmReachEnd() != false) {
        setState(3);
        if (sAnmTable[3] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[3];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}

void TMapObjPakkun::initFunc_Rest() {}

void TMapObjPakkun::doFunc_Rest() {
    TMapObjPakkun::checkItemHitting();

    JGeometry::TVec3f kartPos;
    ObjUtility::getKartPos(mKartIdx, &kartPos);
    turnTo(kartPos, 1.0f);

    if (getStateCount() > sRestFrame) {
        setState(1);
        if (sAnmTable[1] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[1];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}

void TMapObjPakkun::initFunc_Search() {
    _1a6 = 0;
}

void TMapObjPakkun::doFunc_Search() {
    f32 zxSquared;
    
    _1a6++;
    if (_1a6 > sSearchFrame) {
        _1a6 = 0;
        mKartIdx = SiUtil::searchNearKartCircle(mPos, _19c);
        if (mKartIdx == -1) {
            setState(0);
            if (sAnmTable[0] != mAnmPlayer.getCurAnmNumber()) {
                mAnmPlayer._11 = sAnmTable[0];
                mAnmPlayer._10 = true;
                mAnmPlayer._e |= 1;
            }
            return;
        }
    }

    TMapObjPakkun::checkItemHitting();
    JGeometry::TVec3f kartPos;
    ObjUtility::getKartPos(mKartIdx, &kartPos);
    turnTo(kartPos, 1.0f);

    JGeometry::TVec3f test;
    test.sub(mPos, kartPos);
    zxSquared = test.squaredZX();
    if (RCMGetManager()->getRaceMode() != 1 && zxSquared < SQUARE(_1a0)) {
        setState(2);
        if (sAnmTable[2] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[2];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    } else if (zxSquared > SQUARE(_19c)) {
        setState(0);
        if (sAnmTable[0] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[0];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}


void TMapObjPakkun::turnTo(const JGeometry::TVec3f &param_1, f32 param_2) {
    JGeometry::TVec3f local_80;
    JGeometry::TVec3f local_8c;
    JGeometry::TVec3f local_98;
    JGeometry::TVec3f test;
    JGeometry::TVec3f local_a4;
    JGeometry::TVec3f VStack_b0;
    JGeometry::TVec3f local_bc;
    JGeometry::TVec3f local_c8;

    local_8c.set(_1a8);

    local_80.sub(param_1, mPos);
    local_80.normalize();

    local_98.sub(local_80, local_8c);

    f32 turnPow;

    if (local_80.dot(local_8c) > 0.0f) {
        turnPow = sTurnPow;
    } else {
        turnPow = sTurnPowFar;
    }

    turnPow *= param_2;

    if (local_98.length() > turnPow) {
        local_98.setLength(turnPow);
    }

    test.add(local_8c, local_98);

    if (test.isZero()) {
        static JGeometry::TVec3f updir(0.0f, 1.0f, 0.0f);

        VStack_b0.cross(updir, local_8c);
        local_8c.scaleAdd(0.01f, local_8c, VStack_b0);
    } else {
        local_8c.set(test);
    }

    local_8c.y = 0.0f;
    local_8c.normalize();
    _1a8.set(local_8c);

    local_c8.set(0.0f, 1.0f, 0.0f);
    local_bc.cross(local_c8, local_8c);
    _1b4.setXYZDir(local_bc, local_c8, local_8c);
    PSMTXConcat(_1e4, _1b4, _1b4);
}

// FIX: Why is J3DJointCallBack's return type an int for this function, but everywhere else it's a bool?
void TMapObjPakkun::setCallBack(const char *jointName, J3DJointCallBack2 jointCallBack) {   // TODO: Eliminate casting data type.
    u16 jointIdx = mModel.getModelData()->getJointName()->getIndex(jointName);
    mModel.getModelData()->getJointTree().getJointNodePointer(jointIdx)->setCallBack((J3DJointCallBack)jointCallBack);
    mModel._14 = (u32 *)this;
}

void TMapObjPakkun::setRotMtx(int index, Mtx newRotMtx) {}

void TMapObjPakkun::setHeadPos(Mtx) {}

bool TMapObjPakkun::rotCallBack(J3DJoint *joint, int idx) {
    if (idx == 0) {
        int jointIndex = joint->getJntNo();

        // Access global J3D system
        J3DModel *model = j3dSys.getModel();
        TMapObjPakkun *obj = (TMapObjPakkun *)((ExModel *)model->getUserArea())->_14;

        if (obj != nullptr) {
            Mtx &mtx = model->getAnmMtx(jointIndex);

            JGeometry::TPos3f local_48;
            local_48.set(mtx);
            local_48.zeroTrans();

            PSMTXConcat(obj->_1b4, local_48, local_48);

            local_48[0][3] = mtx[0][3];
            local_48[1][3] = mtx[1][3];
            local_48[2][3] = mtx[2][3];

            PSMTXCopy(local_48, obj->mModel.getModel()->getAnmMtx(jointIndex));
            PSMTXCopy(local_48, mtx);
            PSMTXCopy(mtx, J3DSys::mCurrentMtx);
        }
    }
    return true;
}

bool TMapObjPakkun::headCallBack(J3DJoint *joint, int idx) {
    if (idx == 0) {
        int jointIndex = joint->getJntNo();

        // Access global J3D system
        J3DModel *model = j3dSys.getModel();
        TMapObjPakkun *obj = (TMapObjPakkun *)((ExModel *)model->getUserArea())->_14;

        if (obj != nullptr) {
            Mtx& mtx = model->getAnmMtx(jointIndex);

            JGeometry::TVec3f offset;
            JGeometry::TVec3f trans;
            trans.set(
                mtx[0][3],
                mtx[1][3],
                mtx[2][3]
            );

            JGeometry::TPos3f rotationMtx;
            rotationMtx.set(mtx);
            rotationMtx.zeroTrans();

            PSMTXMultVecSR(rotationMtx, &scHeadCenterOffset, &offset);
            
            TMapObjPakkunColHead *colHead = obj->_244;
            trans.add(offset);
            colHead->mVel.sub(trans, colHead->mPos);
            colHead->mPos.set(trans);
        }
    }
    return true;
}

void TMapObjPakkun::checkItemHitting() {
    if (!tstItemHitting())
        return;

    // TODO: Check if this can become an inline.
    switch (getColItemObj()->getKind()) {
        case 0:
        case 1:
        case 2:
        case 7:
        case 8:
        case 9:
        case 11:
            _19a = 0;
            getAnmCtrl()->ChangeBlendTransAnm(2, 1, 1);
            setState(4);
            break;

        default:
            return;
    }
}

void TMapObjPakkun::initFunc_Damage() {
    _198 = mObjData->mParam1 + _19a;
    getSoundMgr()->setSe(0x4007e);
}

void TMapObjPakkun::doFunc_Damage() {
    if (getStateCount() > _198) {
        setState(0);
        if (sAnmTable[0] != mAnmPlayer.getCurAnmNumber()) {
            mAnmPlayer._11 = sAnmTable[0];
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 1;
        }
    }
}

TPakkunSupervisor::TPakkunSupervisor() : GeoObjSupervisor() {
    mList.initiate();
    ItemThunder::entrySuperVisor(this);
}

void TPakkunSupervisor::entry(TMapObjPakkun *pakkun) {
    mList.append(&pakkun->mLinkPakkun);
}

void TPakkunSupervisor::thunderDownAll() {
    f32 unknown = 1.0f;
    for (JSULink<TMapObjPakkun> *pakkun = mList.getFirst(); pakkun != nullptr; pakkun = pakkun->getNext()) {
        TMapObjPakkun *pakkunObj = pakkun->getObject();
        pakkunObj->_19a = TMapObjPakkun::sThunderDamageRandeMax * pakkunObj->getGeoRnd()->get_ufloat_1();
        pakkunObj->setState(4);
        
        const s8 anmItem = pakkunObj->sAnmTable[4];
        if (anmItem != pakkunObj->mAnmPlayer.getCurAnmNumber()) {
            pakkunObj->mAnmPlayer._11 = anmItem;
            pakkunObj->mAnmPlayer._10 = true;
            pakkunObj->mAnmPlayer._e |= 1;
        }
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
