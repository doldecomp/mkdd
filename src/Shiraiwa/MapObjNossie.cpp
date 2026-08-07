#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Util.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JMath/JMath.h"
#include "Shiraiwa/Objects/MapObjNossie.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/AnmController.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/Coord3DUtil.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"
#include "mathHelper.h"

const f32 TMapObjNossie::sOffsetY[4] = {
    540.0f, 540.0f, 362.5f, 362.5f
};

// Query: sHeadOffset is 0x4C (76 bytes) in size...?
//     .comm sHeadOffset__13TMapObjNossie, 0x4C, 4
// What's strange is that it's only ever consumed as a vector.
const JGeometry::TVec3f TMapObjNossie::sHeadOffset(0.0f, -460.0f, 550.0f);
const JGeometry::TVec3f TMapObjNossie::sBodyOffset(0.0f, 2400.0f, 500.0f);

const char *TMapObjNossie::sHeadJointName = "head";
const char *TMapObjNossie::sNossieLegJointName[4] = {
    "wrist_l",
    "wrist_r",
    "ankle_l",
    "ankle_r",
};

s32 TMapObjNossie::sNossieLegJointNo[4] = {
    0, 0, 0, 0
};

TAnmInfo TMapObjNossie::sAnmInfos[16] = {
    { "/Objects/Nossie_a1.bca", nullptr, nullptr, 0, 0, 1, 0x1, 0 },
    { "/Objects/Nossie_a2.bca", nullptr, nullptr, 0, 0, 1, 0x2, 0 },
    { "/Objects/Nossie_a3.bca", nullptr, nullptr, 0, 0, 1, 0x3, 0 },
    { "/Objects/Nossie_a4.bca", nullptr, nullptr, 0, 0, 1, 0x4, 0 },
    { "/Objects/Nossie_a5.bca", nullptr, nullptr, 0, 0, 1, 0x5, 0 },
    { "/Objects/Nossie_a6.bca", nullptr, nullptr, 0, 0, 1, 0x6, 0 },
    { "/Objects/Nossie_a7.bca", nullptr, nullptr, 0, 0, 1, 0x7, 0 },
    { "/Objects/Nossie_a8.bca", nullptr, nullptr, 0, 0, 1, 0x0, 0 },
    { "/Objects/Nossie_b1.bca", nullptr, nullptr, 0, 0, 1, 0xF, 0 },
    { "/Objects/Nossie_b2.bca", nullptr, nullptr, 0, 0, 1, 0x8, 0 },
    { "/Objects/Nossie_b3.bca", nullptr, nullptr, 0, 0, 1, 0x9, 0 },
    { "/Objects/Nossie_b4.bca", nullptr, nullptr, 0, 0, 1, 0xA, 0 },
    { "/Objects/Nossie_b5.bca", nullptr, nullptr, 0, 0, 1, 0xB, 0 },
    { "/Objects/Nossie_b6.bca", nullptr, nullptr, 0, 0, 1, 0xC, 0 },
    { "/Objects/Nossie_b7.bca", nullptr, nullptr, 0, 0, 1, 0xD, 0 },
    { "/Objects/Nossie_b8.bca", nullptr, nullptr, 0, 0, 1, 0xE, 0 },
};

TAnmInfo TMapObjNossie::sAnmInfosTA[1] = {
    {"/Objects/Nossie_TA.bca", nullptr, nullptr, 2, 0, 0, 0xFF, 0}
};




const u32 TMapObjNossie::scNossieEffectAppearPos[16] = {
    2, 1, 0, 3,
    2, 1, 0, 3,
    2, 1, 0, 3,
    2, 1, 0, 3
};

s32 TMapObjNossie::sHeadJointNo;
J3DAnmTransform *TMapObjSwimNossie::sSwimNossieBckAnmTrans;
J3DMtxCalc *TMapObjSwimNossie::sSwimNossieBckMtxCalc;
J3DAnmTexPattern *TMapObjSwimNossie::sSwimNossieBtpAnm;

f32 TMapObjNossieColFoot::sShakeRadius = 10000.0f;
f32 TMapObjSwimNossie::sMoveMaxSpeed = 5.0f;
f32 TMapObjSwimNossie::sMoveAccel = 0.02f;
f32 TMapObjSwimNossie::sReachLength = 500.0f;
f32 TMapObjPtera::sMoveMaxSpeed = 30.0f;
f32 TMapObjPtera::sMoveAccel = 0.5f;
f32 TMapObjPtera::sReachLength = 2000.0f;



TMapObjNossie::TMapObjNossie(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
    mNossieHead = GetGeoObjMgr()->createGenSubObj(0x11a2);
    mNossieTail = GetGeoObjMgr()->createGenSubObj(0x11a8);

    for (int i = 0; i < 4; i++) {
        u32 id;

        switch (i) {
            case 0:
            case 1:
                id = 0x11A3;
                break;

            case 2:
            case 3:
                id = 0x11A7;
                break;

            default:
                #line 107
                JUT_ASSERT(false);
                break;
        }

        mNossieFoot[i] = (TMapObjNossieColFoot *)GetGeoObjMgr()->createGenSubObj(id);
    }
    
    if (isTimeAttack()) {
        mAnmPlayer.resetAnimations(sAnmInfosTA, 1);
    } else {
        mAnmPlayer.resetAnimations(sAnmInfos, 16);
    }
}

const char *TMapObjNossie::getBmdFileName() {
    static const char *cBmdName = "/Objects/Nossie.bmd";
    return cBmdName;
}

void TMapObjNossie::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);
    if (isTimeAttack()) {
        mAnmPlayer.registAnimations(getAnmCtrl(), &mModel, sAnmInfosTA, 1);
    } else {
        mAnmPlayer.registAnimations(getAnmCtrl(), &mModel, sAnmInfos, 16);
    }

    for (int i = 0; i < 4; i++) {
        sNossieLegJointNo[i] = mModel.getModelData()->getJointName()->getIndex(sNossieLegJointName[i]);
    }
    sHeadJointNo = mModel.getModelData()->getJointName()->getIndex(sHeadJointName);
}

void TMapObjNossie::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    if (isTimeAttack()) {
        mAnmPlayer.loadAnimations(sAnmInfosTA, 1, modelData, ResMgr::mcArcCourse);
    } else {
        mAnmPlayer.loadAnimations(sAnmInfos, 16, modelData, ResMgr::mcArcCourse);
    }
}

void TMapObjNossie::reset() {
    JGeometry::TPos3f rotMtx;

    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();

    if (isTimeAttack()) {
        mAnmPlayer.init(mAnmCtrl, sAnmInfosTA, 1);
    } else {
        mAnmPlayer.init(mAnmCtrl, sAnmInfos, 16);
    }

    mAnmCtrl->Reset();
    _58 = 10;

    for (int i = 0; i < 4; i++) {
        u32 unknownValue = 0x40045;
        rotMtx.set(mRotMtx);
        if (i == 1 || i == 3) {
            // might be setRotate?
            f32 s = sinf(JGeometry::TUtilf::PI());
            f32 c = cosf(JGeometry::TUtilf::PI());

            rotMtx[0][0] = c;
            rotMtx[0][1] = -s;
            rotMtx[1][0] = s;
            rotMtx[1][1] = c;
            rotMtx[2][2] = 1.0f;
            rotMtx[2][1] = 0.0f;
            rotMtx[1][2] = 0.0f;
            rotMtx[2][0] = 0.0f;
            rotMtx[0][2] = 0.0f;

            PSMTXConcat(mRotMtx, rotMtx, rotMtx);
            unknownValue = 0x40046;
        }
        mNossieFoot[i]->mRotMtx.set(rotMtx);
        mNossieFoot[i]->mScale.set(mScale);
        mNossieFoot[i]->_188 = unknownValue;
    }
}

bool TMapObjNossie::isTimeAttack() {
    return RCMGetManager()->getRaceMode() == 1;
}

void TMapObjNossie::calc() {
    JGeometry::TVec3f pos;
    JGeometry::TVec3f offset;

    for (u8 i = 0; i < 4; i++) {
        getJointPosition(&pos, sNossieLegJointNo[i]);

        if (RCMGetManager()->getRaceMode() != 1) {
            makeStepEmitter(i, pos);
        }

        pos.y -= sOffsetY[i];

        TMapObjNossieColFoot *foot = mNossieFoot[i];
        foot->mPos.set(pos);
    }

    getJointPosition(&pos, sHeadJointNo);
    
    offset = sHeadOffset;
    PSMTXMultVecSR(mRotMtx, &offset, &offset);
    offset.mul(mScale);
    pos.add(offset);
    mNossieHead->mPos.set(pos);

    offset.set(sBodyOffset);
    PSMTXMultVecSR(mRotMtx, &offset, &offset);
    offset.mul(mScale);
    pos.add(mPos, offset);
    mNossieTail->mPos.set(pos);


    if (RCMGetManager()->getRaceMode() != 1) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < RCMGetManager()->getKartNumber(); j++) {
                if (mNossieFoot[i]->isStepped(j)) {
                    reverse();
                    break;
                }
            }
        }
    }

    mAnmPlayer.update();
}

static const char *emName[4] = {
    "mk_nossieWrist_a",
    "mk_nossieWrist_b",
    "mk_nossieAnkle_a",
    "mk_nossieAnkle_b",
};

void TMapObjNossie::makeStepEmitter(u32 foot, const JGeometry::TVec3f &pos) {
    J3DFrameCtrl *frameCtrl = mAnmPlayer.mController->getFrameCtrl(mAnmPlayer.getCurAnmNumber());

    if (frameCtrl->getEnd() - frameCtrl->getFrame() <= frameCtrl->getRate()) {
        u32 current = scNossieEffectAppearPos[mAnmPlayer.getCurAnmNumber()];
        if (current == foot) {
            switch (current) {
                case 0:
                case 1:
                    JPEffectMgr::getEffectMgr()->createEmt(emName[0], pos);
                    JPEffectMgr::getEffectMgr()->createEmt(emName[1], pos);
                    break;

                case 2:
                case 3:
                    JPEffectMgr::getEffectMgr()->createEmt(emName[2], pos);
                    JPEffectMgr::getEffectMgr()->createEmt(emName[3], pos);
                    break;
            }
        }
    }
}

void TMapObjNossie::reverse() {
    // Nossie doesn't stomp in time trials.
    if (RCMGetManager()->isRaceModeTA()) {
        return;
    }

    s8 currentAnmNo = mAnmPlayer.getCurAnmNumber();

    s32 nextAnmNo = currentAnmNo;
    if (currentAnmNo <= 7) {
        nextAnmNo += 8;
    } else {
        nextAnmNo -= 8;
    }

    mAnmPlayer._11 = nextAnmNo;
    mAnmPlayer._10 = true;
    mAnmPlayer._e |= 2;
}

void TMapObjNossie::getJointPosition(JGeometry::TVec3f *jointPos, s32 idx) {
    MtxPtr anmMtx = mModel.getModel()->getAnmMtx(idx);
    jointPos->set(anmMtx[0][3], anmMtx[1][3], anmMtx[2][3]);
}

void TMapObjNossieColBase::reset() {
    GeographyObj::resetObject();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = getReaction();
}

void TMapObjNossieColHead::createColModel(J3DModelData *modelData) {
    createBoundsSphere(620.0f, mScale.y);
}

void TMapObjNossieColHead::calc() {
    getSoundMgr()->setSe(0x40063);
}

void TMapObjNossieColBody::createColModel(J3DModelData *modelData) {
    createBoundsCylinder(1500.0f, 1900.0f, mScale.y);
}

void TMapObjNossieColFoot::reset() {
    resetFrame();
    GeographyObj::resetObject();
    setObjFlagCheckItemHitting();
    setAllCheckKartHitFlag();
    _58 = getReaction();
    mFootPos.set(mPos);
    _180 = 0.0f;
    _184 = 0;
    _185 = 0;
}

void TMapObjNossieColFoot::createColModel(J3DModelData *modelData) {
    createBoundsCylinder(500.0f, 1800.0f, 1.0f);
}

bool TMapObjNossieColFoot::isStepped(int idx) {
    bool isStepped = false;
    if ((_185 & (1 << idx)) != 0) {
        if ((GetKartCtrl()->getKartBody(idx)->mCarStatus & 0x200000000ULL) != 0) {
            isStepped = true;
        }
    }
    return isStepped;
}

void TMapObjNossieColFoot::calc() {
    mVel.sub(mPos, mFootPos);
    mFootPos.set(mPos);

    if (getShadowModel()) {
        moveShadowModel();
    }

    bool shouldStamp = false;
    if (mFootPos.isZero() || JMAAbs(mVel.y) > 100.0f) {
        shouldStamp = true;
    }

    if (!shouldStamp) {
        if (_180 < 0.0f && _180 * (mVel.y - 0.1f) <= 0.0f) {
            ObjUtility::executeShakeCamera(mPos, sShakeRadius, 0.4f, 2000.0f);
            getSoundMgr()->setSe(_188);
        }
    }
    _180 = mVel.y - 0.1f;

    u8 hitFlag = 0;
    getHitFlag(&hitFlag);
    _185 = hitFlag & ~_184;
    _184 = hitFlag;

    for (int kartIdx = 0; kartIdx < RCMGetManager()->getKartNumber(); kartIdx++) {
        shouldStamp = false;
        if (_185 & 1 << kartIdx) {
            if (GetKartCtrl()->GetKartStatus(kartIdx) & 0x200000000) {
                shouldStamp = true;
            }
        }
        if (shouldStamp) {
            startStamp(kartIdx);
        }
        if (!(GetKartCtrl()->GetKartStatus(kartIdx) & 0x200000000)) {
            endStamp(kartIdx);
        }
    }
    frameProc();
}

void TMapObjNossieColFoot::getHitFlag(u8 *hitFlag) {
    for (int kartNo = 0; kartNo < 8; kartNo++) {
        *hitFlag |= (tstIsHitKartFlg(kartNo) << kartNo);
    }
}

void TMapObjNossieColFoot::moveShadowModel() {
    if (getShadowModel()->isInvisibleAll()) {
        return;
    }
    CrsGround crsGround(RCMGetCourse());
    crsGround.search(this, mPos, mPos);
    
    JGeometry::TPos3f newRotMtx;
    PSMTXCopy(mRotMtx, newRotMtx);

    newRotMtx[0][3] = mPos.x;
    newRotMtx[1][3] = crsGround.getHeight();
    newRotMtx[2][3] = mPos.z;

    getShadowModel()->setBaseTRMtx(newRotMtx);
    getShadowModel()->getModel()->setBaseScale(mScale);
}

TMapObjSwimNossie::TMapObjSwimNossie(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    if (mObjData->mPathID == 0xffff) {
        #line 526
        JUT_ASSERT_MSG(false, "SwimNossie: illegal Path ID.");
    }
    mPathUtil = new TPathUtil(&sObject);
    NewAnmCtrl();
    createSoundMgr();
}

TMapObjSwimNossie::~TMapObjSwimNossie() {
    delete mPathUtil;
}

void TMapObjSwimNossie::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createDifferedModel(heap, p2, 0x20000, 0);

    AnmController *anmCtrl = mAnmCtrl;
    anmCtrl->mTrans = new AnmControlTrans();
    anmCtrl->mTrans->initAnm(1, &mModel);

    mAnmCtrl->RegisterTrans(0, sSwimNossieBckAnmTrans, sSwimNossieBckMtxCalc);
    mAnmCtrl->getFrameCtrl(0)->setAttribute(2);

    mAnmObjMaterial.setExModel(&mModel);
    mAnmObjMaterial.setAnmBase(sSwimNossieBtpAnm);
    mAnmObjMaterial.initFrameCtrl(mAnmObjMaterial.getAnmBase());
}

void TMapObjSwimNossie::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    J3DAnmObjTrans::setupTransAnmData(&sSwimNossieBckAnmTrans, &sSwimNossieBckMtxCalc, modelData, ObjUtility::getPtrCourseArc("/Objects/SwimNossie.bca"));
    J3DAnmObjMaterial::setupTexPatternAnmData(&sSwimNossieBtpAnm, modelData, ObjUtility::getPtrCourseArc("/Objects/SwimNossie.btp"));
}

void TMapObjSwimNossie::update() {
    mAnmObjMaterial.anmFrameProc();
    setModelMatrixAndScale();
    mModel.update(0);
}

void TMapObjSwimNossie::reset() {
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirY;
    JGeometry::TVec3f vecDirZ;
    
    GeographyObj::resetObject();
    mAnmCtrl->Reset();
    mPathUtil->reset();

    CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(mPathUtil->mObj->mPathID, mPathUtil->getPointIndex());
    f32 posX = pointData->pos.x;
    f32 posY = pointData->pos.y;
    f32 posZ = pointData->pos.z;

    mPos.set(posX, posY, posZ);

    mPathUtil->getDirection(vecDirZ);
    
    vecDirZ.y = 0.0f;
    vecDirZ.normalize();

    vecDirY.set(0.0f, 1.0f, 0.0f);
    vecDirX.cross(vecDirY, vecDirZ);
    vecDirX.normalize();

    mRotMtx.setXYZDir(vecDirX, vecDirY, vecDirZ);

    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    mAnmObjMaterial.resetFrame();

    u16 newFrame = mObjData->mParam2;
    if (newFrame >= mAnmObjMaterial.getFrameCtrl()->getEnd()) {
        newFrame = 0;
    }
    mAnmObjMaterial.getFrameCtrl()->setFrame(newFrame);
}

void TMapObjSwimNossie::calc() {
    if (moveToNextNode() < sReachLength) {
        TPathUtil *pathUtil = mPathUtil;
        pathUtil->mDistanceDirty = true;
        pathUtil->setPointIndex(pathUtil->getNextNode());
    }
    getSoundMgr()->setSe(0x40063);
}

f32 TMapObjSwimNossie::moveToNextNode() {
    TPathUtil *pathUtil = mPathUtil;
    const u16 pointIdx = pathUtil->getNextNode();
    
    CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(pathUtil->mObj->mPathID, pointIdx);

    JGeometry::TVec3f pointDataPos;
    pointDataPos.set(pointData->pos);

    JGeometry::TVec3f pointDataDiff;
    pointDataDiff.sub(pointDataPos, mPos);

    f32 nextNode = pointDataDiff.length();
    pointDataDiff.normalize();
    
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirY;
    JGeometry::TVec3f vecDirZ;
    
    mRotMtx.getZDir(vecDirZ);
    
    JGeometry::TVec3f accelVec;
    accelVec.scale(sMoveAccel, pointDataDiff);
    mVel.add(accelVec);

    if (mVel.length() > sMoveMaxSpeed) {
        mVel.setLength(sMoveMaxSpeed);
    }

    mPos.add(mVel);

    if (mVel.x == 0.0f && mVel.z == 0.0f) {
        mRotMtx.getZDir(vecDirZ);
    } else {
        vecDirZ.set(mVel);
    }

    vecDirZ.y = 0.0f;
    vecDirZ.normalize();

    vecDirY.set(0.0f, 1.0f, 0.0f);
    vecDirX.cross(vecDirY, vecDirZ);
    vecDirX.normalize();

    mRotMtx.setXYZDir(vecDirX, vecDirY, vecDirZ);
    return nextNode;
}

TAnmInfo TMapObjPtera::sAnmInfos[2] = {
    {"/Objects/PteraFlya.bck", nullptr, nullptr, 2, 0, 0, 1, 0},
    {"/Objects/PteraFlyb.bck", nullptr, nullptr, 2, 0, 0, 0, 0}
};

TMapObjPtera::TMapObjPtera(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    NewAnmCtrl();
    createSoundMgr();
    mAnmPlayer.resetAnimations(sAnmInfos, 2);
    mPathUtil = new TPathUtil(&sObject);
}

TMapObjPtera::~TMapObjPtera() {
    delete mPathUtil;
}

void TMapObjPtera::reset() {
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirY;
    JGeometry::TVec3f vecDirZ;
    
    GeographyObj::resetObject();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();
    _58 = 0;

    mAnmPlayer.init(getAnmCtrl(), sAnmInfos, 2);
    SiUtil::setRandomStartFrame(getAnmCtrl()->mTrans->getFrameCtrl(0), getGeoRnd());

    mPathUtil->reset();

    CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(mPathUtil->mObj->mPathID, mPathUtil->getPointIndex());
    f32 posX = pointData->pos.x;
    f32 posY = pointData->pos.y;
    f32 posZ = pointData->pos.z;

    mPos.set(posX, posY, posZ);

    mPathUtil->getDirection(vecDirZ);
    vecDirZ.y = 0.0f;
    vecDirZ.normalize();

    vecDirY.set(0.0f, 1.0f, 0.0f);
    vecDirX.cross(vecDirY, vecDirZ);
    vecDirX.normalize();

    mRotMtx.setXYZDir(vecDirX, vecDirY, vecDirZ);
}

void TMapObjPtera::calc() {
    mAnmPlayer.update();
    if (moveToNextNode() < sReachLength) {
        TPathUtil *pathUtil = mPathUtil;
        mPathUtil->mDistanceDirty = true;
        pathUtil->setPointIndex(pathUtil->getNextNode());
        
        pathUtil = mPathUtil;
        const u16 nextNode = mPathUtil->getNextNode();
        f32 height = RCMGetCourse()->getCrsData()->getPointData(pathUtil->mObj->mPathID, nextNode)->pos.y;
        
        if (mAnmPlayer.mController->getNowTransNo() == 0 && mPos.y > height) {
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 2;
        } 
        else if (mAnmPlayer.mController->getNowTransNo() == 1 && mPos.y <= height) {
            mAnmPlayer._10 = true;
            mAnmPlayer._e |= 2;
        }
    }

    getSoundMgr()->setSe(0x4005E);

    if (mAnmPlayer.mController->mTrans->getNowAnmNo() == 0) {
        J3DFrameCtrl *frameCtrl = mAnmPlayer.mController->mTrans->getFrameCtrl(mAnmPlayer.mController->mTrans->getNowAnmNo());

        if (SiUtil::checkPassFrame(2.0f, frameCtrl->getFrame(), frameCtrl->getRate())) {
            getSoundMgr()->setSe(0x4005C);
        }
    }
}

f32 TMapObjPtera::moveToNextNode() {
    TPathUtil *pathUtil = mPathUtil;
    const u16 pointIdx = pathUtil->getNextNode();
    
    CrsData::PointData *pointData = RCMGetCourse()->getCrsData()->getPointData(pathUtil->mObj->mPathID, pointIdx);

    JGeometry::TVec3f pointDataPos;
    pointDataPos.set(pointData->pos);

    JGeometry::TVec3f pointDataDiff;
    pointDataDiff.sub(pointDataPos, mPos);

    f32 nextNode = pointDataDiff.length();
    pointDataDiff.normalize();
    
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirY;
    JGeometry::TVec3f vecDirZ;
    
    mRotMtx.getZDir(vecDirZ);
    
    JGeometry::TVec3f accelVec;
    accelVec.scale(sMoveAccel, pointDataDiff);
    mVel.add(accelVec);

    if (PSVECMag(&mVel) > sMoveMaxSpeed) {
        mVel.normalize(sMoveMaxSpeed);
    }

    mPos.add(mVel);

    if (mVel.isZero()) {
        mRotMtx.getZDir(vecDirZ);
    } else {
        vecDirZ.set(mVel);
    }

    vecDirZ.normalize();
    vecDirY.set(0.0f, 1.0f, 0.0f);

    vecDirX.cross(vecDirY, vecDirZ);
    vecDirX.normalize();

    vecDirY.cross(vecDirZ, vecDirX);
    vecDirY.normalize();

    mRotMtx.setXYZDir(vecDirX, vecDirY, vecDirZ);
    return nextNode;
}

void TMapObjPtera::loadAnimation() {
    mAnmPlayer.loadAnimations(sAnmInfos, 2, mModel.getModelData(), ResMgr::mcArcCourse);
}

void TMapObjPtera::update() {
    setModelMatrixAndScale();
    mModel.update(0);
}

void TMapObjPtera::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);
    mAnmPlayer.registAnimations(getAnmCtrl(), &mModel, sAnmInfos, 2);
}

#include "JSystem/JAudio/JASFakeMatch2.h"
