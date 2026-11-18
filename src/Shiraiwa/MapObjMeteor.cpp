#include "Kaneshige/RaceMgr.h"
#include "Sato/ItemObjMgr.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/StateObserver.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/SiUtil.h"
#include "Shiraiwa/Objects/MapObjMeteor.h"
#include "mathHelper.h"

StateObserver::StateFuncSet<TMapObjMeteor> TMapObjMeteor::sTable[4] = {
    {0, &TMapObjMeteor::initFunc_Hide, &TMapObjMeteor::doFunc_Hide },
    {1, &TMapObjMeteor::initFunc_Appear, &TMapObjMeteor::doFunc_Appear },
    {2, &TMapObjMeteor::initFunc_Move, &TMapObjMeteor::doFunc_Move },
    {3, &TMapObjMeteor::initFunc_Crush, &TMapObjMeteor::doFunc_Crush },
};

JGeometry::TVec3f TMapObjMeteor::sItemVel(0.0f, 50.0f, 0.0f);
f32 TMapObjMeteor::sBoxSize = 100.0f;
f32 TMapObjMeteor::sObjRadius = 300.0f;
s16 TMapObjMeteor::sAppearFrame = 5;
f32 TMapObjMeteor::sRotSpeed = 0.1f;

TMapObjMeteor::TMapObjMeteor(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    createSoundMgr();
}

TMapObjMeteor::~TMapObjMeteor() {}

void TMapObjMeteor::createColModel(J3DModelData *modelData) {
    createBoundsSphere(sObjRadius, 1.0f);
}


void TMapObjMeteor::reset() {
    JGeometry::TVec3f temp;
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirY;
    JGeometry::TVec3f vecDirZ;
    
    resetObject();
    _164.set(mObjData->yNorm);
    _164.normalize();
    _170.set(mObjData->zNorm);
    _170.normalize();
    _158.cross(_164, _170);
    _158.normalize();

    _164.negate();

    _158.scale(mObjData->scale.x * sBoxSize);
    _164.scale(mObjData->scale.y * sBoxSize);
    _170.scale(mObjData->scale.z * sBoxSize);

    temp.x = (_158.x * 0.5f) + (_164.x) + (_170.x * 0.5f);
    temp.y = (_158.y * 0.5f) + (_164.y) + (_170.y * 0.5f);
    temp.z = (_158.z * 0.5f) + (_164.z) + (_170.z * 0.5f);

    _17c.scaleAdd(-1.0f, temp, mObjData->position);
    mScale.setAll(1.0f);

    PSMTXIdentity(mRotMtx);

    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _58 = 0xd;

    f32 fVar1 = mObjData->mParam2;
    if (fVar1 <= 0.0f) {
        fVar1 = 100.0f;
    }
    _188 = fVar1;

    vecDirY.set(0.0f, 1.0f, 0.0f);

    s16 angle = mObjData->mParam4 * 182.04445f;
    vecDirZ.set(JMASCos(angle), 0.0f, JMASSin(angle));
    vecDirX.cross(vecDirY, vecDirZ);
    mRotMtx.setXYZDir(vecDirX, vecDirY, vecDirZ);

    mEmitter = nullptr;
    ResetState();
}

void TMapObjMeteor::calc() {
    ExecuteState();
}

void TMapObjMeteor::initFunc_Hide() {
    setObjFlagHidding();
}

void TMapObjMeteor::doFunc_Hide() {
    if (getStateCount() > mObjData->mParam3) {
        setState(1);
    }
}

void TMapObjMeteor::initFunc_Appear() {
    f32 rand1 = getGeoRnd()->get_ufloat_1();
    f32 rand2 = getGeoRnd()->get_ufloat_1();

    JGeometry::TVec3f local_30;
    for (int i = 0; i < 1; i++) {
        local_30.set(_158);
        local_30.scale(rand1);
        local_30.scaleAdd(rand2, _170);
    }

    mPos.add(_17c, local_30);
    mScale.setAll(0.0f);
    clrObjFlagHidding();

    mEmitter = JPEffectMgr::getEffectMgr()->createEmt("mk_rbStarKira_a", mPos);
    if (mEmitter == nullptr) {
        #line 171
        JUT_WARNING_F2("%s", "TMapObjMeteor: Can\'t create Emitter");
    }
}

void TMapObjMeteor::doFunc_Appear() {
    f32 normalRange = SiUtil::getNormalRange(getStateCount(), 0.0f, sAppearFrame) + 0.0001f;

    mScale.setAll(normalRange);
    if (getStateCount() > sAppearFrame) {
        setState(2);
    }
    moveMainProc();
}

void TMapObjMeteor::initFunc_Move() {
    mVel.set(_164);
    mVel.setLength(_188);
    mScale.setAll(1.0f);
}

void TMapObjMeteor::doFunc_Move() {
    moveMainProc();

    JGeometry::TVec3f target;
    JGeometry::TVec3f delta;

    target.add(_17c, _164);
    delta.sub(target, mPos);

    if (checkGroundHitting()) {
        showItem();
        setState(3);
    } else if (delta.dot(_164) < 0.0f) {
        setState(3);
    }
}

void TMapObjMeteor::moveMainProc() {
    JGeometry::TPos3f local_58;
    
    mPos.add(mVel);
    PSMTXIdentity(local_58);

    // might be setRotate?
    const f32 angle = sRotSpeed;
    f32 s = sinf(angle);
    f32 c = cosf(angle);

    local_58[0][0] = c;
    local_58[0][1] = -s;
    local_58[1][0] = s;
    local_58[1][1] = c;
    local_58[2][2] = 1.0f;
    local_58[2][1] = 0.0f;
    local_58[1][2] = 0.0f;
    local_58[2][0] = 0.0f;
    local_58[0][2] = 0.0f;

    PSMTXConcat(mRotMtx, local_58, mRotMtx);
    if (mObjData->mParam1 == 0) {
        getSoundMgr()->setSe(0x40050);
    }

    if (mEmitter != nullptr) {
        mEmitter->setGlobalScale(mScale);
        mEmitter->setGlobalTranslation(mPos);
    }
}

void TMapObjMeteor::initFunc_Crush() {
    JPEffectMgr::getEffectMgr()->createEmt("mk_rbStarBreak_a", mPos);
    if (mEmitter != nullptr) {
        JPEffectMgr::getEffectMgr()->deleteEmt(mEmitter);
    }
    mScale.setAll(1.0f);
}

void TMapObjMeteor::doFunc_Crush() {
    f32 normalRange = SiUtil::getNormalRange(
        getStateCount(), 0.0f, sAppearFrame
    );

    normalRange = 1.0f - normalRange;
    normalRange = 0.0001f + normalRange;

    if (normalRange <= 0.0001f) {
        setObjFlagHidding();
        setState(0);
    }
    mScale.setAll(normalRange);
}

bool TMapObjMeteor::checkGroundHitting() {
    if (mObjData->mParam1 != 0) {
        return false;
    }
    
    bool hit = false;
    CrsGround ground(RCMGetCourse());
    ground.search(mPos);

    f32 groundHeight = ground.getHeight();

    switch (ground.getAttribute()) {
        case CrsGround::Attr_2:
        case CrsGround::Attr_5:
        case CrsGround::Attr_10:
            break;

        default:
            if (mPos.y - sObjRadius < groundHeight) {
                hit = true;
            }
            break;
    }

    return hit;
}

void TMapObjMeteor::showItem() {
    if (!RCMGetManager()->isRaceModeTA()) {
        if (getGeoRnd()->get_ufloat_1() <= 0.3f) {
            GetItemObjMgr()->occurItem(6, mPos, sItemVel);
            getSoundMgr()->setSe(0x40051);
            JPEffectMgr::getEffectMgr()->createEmt("mk_rbStarBreak_b", mPos);
        }
    }
}

void TMapObjMeteor::InitExec() { Observer_FindAndInit(TMapObjMeteor, 4); }

void TMapObjMeteor::MoveExec() { Observer_FindAndExec(TMapObjMeteor, 4); }

#include "JSystem/JAudio/JASFakeMatch2.h"
