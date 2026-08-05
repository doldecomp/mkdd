#include "JSystem/J3D/J3DJoint.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/J3D/J3DSys.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Quat.h"
#include "JSystem/JGeometry/Util.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JMath/JMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/JUtility/JUTNameTab.h"
#include "Kaneshige/Course/Course.h"
#include "Kaneshige/Course/CrsArea.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/DarkAnmMgr.h"
#include "Kaneshige/ExModel.h"
#include "Kaneshige/Movie/MovieApp.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/ResMgr.h"
#include "Sato/AnmController.h"
#include "Sato/GeographyObj.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/AnmPlayer.h"
#include "Shiraiwa/JugemHeadItem.h"
#include "Shiraiwa/JugemItem.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartBody.h"
#include "Yamamoto/kartCtrl.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "types.h"
#include "Shiraiwa/JugemMain.h"
#include "mathHelper.h"

f32 TJugem::scJugemDistance = 700.0f;
const f32 TJugem::scJugemHeight = 200.0f;
const f32 TJugem::scEraseHeight = 7000.0f;
static const u8 scJugemCameraMax = 4;
s16 TJugem::sChaseDistance = 0x3c;
f32 TJugem::sChaseAccel = 0.5f;
f32 TJugem::sChaseDecel = 0.6f;
f32 TJugem::sChaseEndSpeed = 0.0001f;
s16 TJugem::sShadowEraseFrame = 0x1e;

// ORDER IS IMPORTANT - Ordered correctly for [.data-0] segment.
StateObserver::StateFuncSet<TJugem> TJugem::sTable[23] = {
    {   0, &TJugem::initFunc_Wait, &TJugem::doFunc_Wait},
    {   1, &TJugem::initFunc_Erase, &TJugem::doFunc_Erase},
    {   2, &TJugem::initFunc_Lap, &TJugem::doFunc_Lap},
    {   3, &TJugem::initFunc_LapStay, &TJugem::doFunc_LapStay},
    {   4, &TJugem::initFunc_LapEnd, &TJugem::doFunc_LapEnd},
    {   5, &TJugem::initFunc_Reverse, &TJugem::doFunc_Reverse},
    {   6, &TJugem::initFunc_RevEnd, &TJugem::doFunc_RevEnd},
    {   7, &TJugem::initFunc_RescueWait, &TJugem::doFunc_RescueWait},
    {   8, &TJugem::initFunc_Rescue, &TJugem::doFunc_Rescue},
    {   9, &TJugem::initFunc_Goal, &TJugem::doFunc_Goal},
    { 10, &TJugem::initFunc_StartWaitPermission, &TJugem::doFunc_StartWaitPermission},
    { 11, &TJugem::initFunc_StartWaitCamera, &TJugem::doFunc_StartWaitCamera},
    { 12, &TJugem::initFunc_StartDown, &TJugem::doFunc_StartDown},
    { 13, &TJugem::initFunc_StartWaitBlend, &TJugem::doFunc_StartWaitBlend},
    { 14, &TJugem::initFunc_StartCountDown, &TJugem::doFunc_StartCountDown},
    { 15, &TJugem::initFunc_StartUp, &TJugem::doFunc_StartUp},
    { 16, &TJugem::initFunc_StartWaitHide, &TJugem::doFunc_StartWaitHide},
    { 17, &TJugem::initFunc_DemoHide, &TJugem::doFunc_DemoHide},
    { 18, &TJugem::initFunc_DemoIn, &TJugem::doFunc_DemoIn},
    { 19, &TJugem::initFunc_DemoWait, &TJugem::doFunc_DemoWait},
    { 20, &TJugem::initFunc_WaitDemoOut, &TJugem::doFunc_WaitDemoOut},
    { 21, &TJugem::initFunc_DemoOut, &TJugem::doFunc_DemoOut},
    { 22, &TJugem::initFunc_Ending, &TJugem::doFunc_Ending},
};

TJugemAnmTableEntry TJugem::sAnmStateTable[5] = {
    { sAnmInfos_Start,   5 },
    { sAnmInfos_Lap,     4 },
    { sAnmInfos_Rescue,  3 },
    { sAnmInfos_Reverse, 2 },
    { sAnmInfos_Goal,    1 },    // ...THUNDERBIRDS ARE GO!
};

TJugemAnmTableEntry TJugem::sDemoAnmStateTable[3] = {
    { sAnmInfos_Demo1, 3 },
    { sAnmInfos_Demo2, 3 },
    { sAnmInfos_Demo3, 3 },
};

TAnmInfo TJugem::sAnmInfos_Start[5] = {
    { "/Objects/jg_th.bca", nullptr, nullptr, 0, 60, 1, 1, 0 },
    { "/Objects/jg_ww.bca", nullptr, nullptr, 2, 60, 0, 2, 0 },
    { "/Objects/jg_cd.bca", nullptr, nullptr, 0, 10, 1, 3, 0 },
    { "/Objects/jg_go.bca", nullptr, nullptr, 0, 60, 1, 4, 0 },
    { "/Objects/jg_wz.bca", nullptr, nullptr, 2, 0, 0, 255, 0 },
};
TAnmInfo TJugem::sAnmInfos_Lap[4] = {
    { "/Objects/jg_wz2fg.bca", nullptr, nullptr, 0, 30, 1, 1, 0 },
    { "/Objects/jg_lk.bca", nullptr, nullptr, 2, 30, 2, 2, 0 },
    { "/Objects/jg_by.bca", nullptr, nullptr, 2, 30, 2, 3, 0 },
    { "/Objects/jg_wz.bca", nullptr, nullptr, 2, 0, 0, 255, 0 },
};
TAnmInfo TJugem::sAnmInfos_Rescue[3] = {
    { "/Objects/jg_ca.bca", nullptr, nullptr, 2, 0, 0, 1, 0 },
    { "/Objects/jg_cu.bca", nullptr, nullptr, 0, 0, 0, 2, 0 },
    { "/Objects/jg_wz.bca", nullptr, nullptr, 2, 0, 0, 255, 0 },
};
TAnmInfo TJugem::sAnmInfos_Reverse[2] = {
    { "/Objects/jg_no.bca", nullptr, nullptr, 2, 30, 6, 1, 0 },
    { "/Objects/jg_st.bca", nullptr, nullptr, 2, 30, 6, 0, 0 },
};
TAnmInfo TJugem::sAnmInfos_Goal[1] = {
    { "/Objects/jg_wy.bca", nullptr, nullptr, 2, 0, 0, 255, 0 }
};


TAnmInfo TJugem::sAnmInfos_Demo1[3] = {
    { "/Objects/jg_in_a.bck", nullptr, nullptr, 0, 0, 1, 1, 0 },
    { "/Objects/jg_wait.bck", nullptr, nullptr, 2, 0, 0, 2, 0 },
    { "/Objects/jg_out.bck", nullptr, nullptr, 0, 0, 1, 255, 0 },
};

TAnmInfo TJugem::sAnmInfos_Demo2[3] = {
    { "/Objects/jg_in_b.bck", nullptr, nullptr, 0, 0, 1, 1, 0 },
    { "/Objects/jg_wait.bck", nullptr, nullptr, 2, 0, 0, 2, 0 },
    { "/Objects/jg_out.bck", nullptr, nullptr, 0, 0, 1, 255, 0 },
};

TAnmInfo TJugem::sAnmInfos_Demo3[3] = {
    { "/Objects/jg_in_c.bck", nullptr, nullptr, 0, 0, 1, 1, 0 },
    { "/Objects/jg_wait.bck", nullptr, nullptr, 2, 0, 0, 2, 0 },
    { "/Objects/jg_out.bck", nullptr, nullptr, 0, 0, 1, 255, 0 },
};

TJugem::TJugem() : TMapObjHioNode(0xFF), mJugemItem(nullptr), mJugemHeadItem(nullptr) {
    mSignal = nullptr;

    if (isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            mDemoAnmController[i] = new AnmController();
        }
    } else {
        for (int i = 0; i < 5; i++) {
            mAnmController[i] = new AnmController();
        }
    }

    mCameraNum = 0;
    mKartNum = 0;

    mSplineInterp.init(&_208);

    for (int i = 0; i < 3; i++) {
        mItem[i] = nullptr;
    }

    for (int i = 0; i < 2; i++) {
        mHeadItem[i] = nullptr;
    }

    mDarkAnmPlayer = nullptr;
    if (!isDemoMode()) {
        mDarkAnmPlayer = new ObjDarkAnmPlayer(mKind);
    }
    
    resetStaticData();
    reset();
}

TJugem::~TJugem() {
    AnmController *anmController;
    if (isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            delete mDemoAnmController[i];
        }
    } else {
        for (int i = 0; i < 5; i++) {
            delete mAnmController[i];
        }
    }
}

void TJugem::makeAnmCtrl(int num) {
    // UNUSED
    JUT_MINMAX_ASSERT(0, num, 5);
}

void TJugem::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    if (isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            TAnmPlayer::loadAnimations(sDemoAnmStateTable[i].tAnmInfo, sDemoAnmStateTable[i].arraySize, modelData, ResMgr::mcArcOpening);
        }
    } else {
        for (int i = 0; i < 5; i++) {
            TAnmPlayer::loadAnimations(sAnmStateTable[i].tAnmInfo, sAnmStateTable[i].arraySize, modelData, ResMgr::mcArcMRAM);
        }
    }

    for (u16 i = 0; i < modelData->getShapeNum(); i++) {
        modelData->getShapeNodePointer(i)->setTexMtxLoadType(0x2000);
    }
}

void TJugem::resetStaticData() {
    if (isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            TAnmPlayer::resetAnimations(sDemoAnmStateTable[i].tAnmInfo, sDemoAnmStateTable[i].arraySize);
        }
    } else {
        for (int i = 0; i < 5; i++) {
            TAnmPlayer::resetAnimations(sAnmStateTable[i].tAnmInfo, sAnmStateTable[i].arraySize);
        }
    }
}

void TJugem::reset() {
    GeographyObj::resetObject();
    _58 = 0;
    mCheckKartHitFlags = 0;

    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();

    if (mDarkAnmPlayer != nullptr) {
        mDarkAnmPlayer->reset();
    }

    mDemoPattern = 0;
    _24c = 0;
    _276 = 0;
    _278 = 0;
    _208.zero();
    _220.zero();
    _214.zero();
    _22c.zero();
    _238.zero();
    mIsAbleStart = false;
    _274 = 1;
    mIsStartPoint = true;

    ResetState();
    if (isDemoMode()) {
        setState(0x11);
    } else {
        resetStartPosition();
    }
}

void TJugem::hold(u32 id, u32 param_2) {
    if (mJugemItem != nullptr) {
        mJugemItem->hide();
    }

#line 367
    JUT_MINMAX_ASSERT(0, id, 3);
    if (mItem[id] != nullptr) {
        mJugemItem = mItem[id];
        mJugemItem->hold(param_2);
    }
}

void TJugem::setJugemItem(TJugemItem *tJugemItem, u32 id) {
#line 382
    JUT_MINMAX_ASSERT(0, id, 3);
    mItem[id] = tJugemItem;
    tJugemItem->mJugemParentPtr = (int)this;
}

void TJugem::setJugemHeadItem(TJugemHeadItem *tJugemHeadItem, u32 id) {
#line 394
    JUT_MINMAX_ASSERT(0, id, 2);
    mHeadItem[id] = tJugemHeadItem;
    tJugemHeadItem->mJugemParentPtr = (int)this;
}

const char *TJugem::getShadowBmdFileName() {
    static const char *cShadowBmdName = "/Objects/jg_shadow.bmd";
    return cShadowBmdName;
}

const char *TJugem::getBmdFileName() {
    int index = 0;
    if (!isDemoMode() && SiUtil::getConsoleNum() != 1) {
        index = 1;
    }
    static const char *cBmdName[2] = {
        "/Objects/jg_model.bmd",
        "/Objects/jg_modelL.bmd"
    };
    return cBmdName[index];
}

void TJugem::createModel(JKRSolidHeap *param_1, u32 param_2) {
    mModel.createDifferedModel(param_1, param_2, 0x1000200, 1);

    u16 idx = mModel.getModelData()->getJointName()->getIndex("ef_head");
    J3DJoint* joint = mModel.getModelData()->getJointNodePointer(idx);
    joint->setCallBack(nodeCallBack);

    mModel._14 = nullptr;
    if (isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            TAnmPlayer::registAnimations(mDemoAnmController[i], &mModel, sDemoAnmStateTable[i].tAnmInfo, sDemoAnmStateTable[i].arraySize);
        }
    } else {
        for (int i = 0; i < 5; i++) {
            TAnmPlayer::registAnimations(mAnmController[i], &mModel, sAnmStateTable[i].tAnmInfo, sAnmStateTable[i].arraySize);
        }
    }
}

void TJugem::setCurrentViewNo(u32 viewNo) {
    mModel.setCurrentViewNo(viewNo);
    
    MtxPtr effectMtx;
    if (!isDemoMode()) {
        effectMtx = RaceMgr::getCurrentManager()->getKartDrawer(getKartNum())->getLight(viewNo)->getEffectMtx();
    } else {
        effectMtx = MovieApp::getMovieApp()->getMdlViewer()->getChaseLight()->getEffectMtx();
    }
    mModel.setEffectMtx(effectMtx, 1);
}

void TJugem::createColModel(J3DModelData *) {}

void TJugem::createShadowModel(JKRSolidHeap *jkrSolidHeap, u32 p2) {
    GeographyObj::createShadowModel(jkrSolidHeap, p2);

    if (getShadowModel() != nullptr) {
        for (u32 cnsNo = 0; cnsNo < RaceMgr::getCurrentManager()->mRaceInfo->mPlayerNum; cnsNo++) {
            if (cnsNo == mCameraNum) {
                ShadowModel *shadowModel = mShadowMdl;
                shadowModel->setVisible(cnsNo);
            }
        }
    }
}

void TJugem::viewCalc(u32 param_1) {
    if (checkVisible(param_1) != 0) {
        mModel.clipAll(param_1, false);
        TJugemItem *tJugemItem = mJugemItem;
        if (tJugemItem != nullptr) {
            tJugemItem->hideModel(param_1);
        }
        TJugemHeadItem *tJugemHeadItem = mJugemHeadItem;
        if (tJugemHeadItem != nullptr) {
            tJugemHeadItem->hideModel(param_1);
        }
    }
    mModel.viewCalc(param_1);
}

bool TJugem::checkVisible(u32 unknownVisibilityParam) {
    if ((unknownVisibilityParam == 3) && (RaceMgr::sRaceManager->mRaceInfo->mStatusNum == 3)) {
        return GetKartCtrl()->getKartCam(3)->GetTargetNum() != mCameraNum;
    }
    return unknownVisibilityParam != (u8)mCameraNum;
}

void TJugem::initFunc_Erase() {}

void TJugem::doFunc_Erase() {
    f32 normalisedRange = SiUtil::getNormalRange(getStateCount(), 0.0f, sShadowEraseFrame);
    
    f32 a = (1.0f - normalisedRange);
    getShadowModel()->setAlpha(255.0f * a);
    if (a <= 0.0f) {
        setState(0);
    }
}

void TJugem::initFunc_Wait() {
    hideAll();
    AnmControlTrans *anmControlTrans = getAnmCtrl()->mTrans;
    if (anmControlTrans != nullptr) {
        anmControlTrans->setFlag();
    }

    if (mJugemItem == nullptr) return;
    if (mJugemItem->getAnmCtrl() == nullptr) return;
    if (mJugemItem->getAnmCtrl()->mTrans == 0) return;

    mJugemItem->getAnmCtrl()->mTrans->setFlag();
}

void TJugem::doFunc_Wait() {
    if (checkLapProc() != 0) {
        if (RaceMgr::getCurrentManager()->getKartChecker(getKartNum())->isGoal() != false) {
            setState(9);
        } else {
            setState(2);
        }
    } else if (checkReverseProc() != 0) {
        setState(5);
    }
}

void TJugem::calc() {
    ExecuteState();
    if ((mGeoObjFlag & 0x20) == 0) {
        mAnmPlayer.update();
        CrsArea crsArea;
        crsArea.search(0, mPos);

        if (mDarkAnmPlayer != nullptr) {
            mDarkAnmPlayer->calc(crsArea);
        }
    }

    if (checkRescueInProc() != false) {
        setState(7);
    }
}

void TJugem::update() {
    if (mDarkAnmPlayer != nullptr) {
        mDarkAnmPlayer->setTevColor(&mModel);
    }
    setModelMatrixAndScale();
    mModel.update(0);
}

void TJugem::resetJugemOriginCourse() {
    JGeometry::TVec3f coursePos;
    JGeometry::TVec3f jugemPos;

    mIsStartPoint = RaceMgr::sRaceManager->getStartPoint(&coursePos, &jugemPos, mKartNum);
    coursePos.y += RaceMgr::sRaceManager->getStartJugemOffsetY(mKartNum);
    coursePos.y += 75.0f;
    resetJugemOrigin(coursePos, jugemPos);
}

void TJugem::resetJugemOriginNumber() {
    JGeometry::TVec3f kartPos;
    JGeometry::TVec3f jugemPos;
    
    ObjUtility::getKartPos(mKartNum, &kartPos);
    getZDir(mKartNum, &jugemPos);
    resetJugemOrigin(kartPos, jugemPos);
}

void TJugem::getZDir(int param_1, JGeometry::TVec3f *vecZDir) {
    JGeometry::TVec3f local_58;
    JGeometry::TVec3f kartZDir;
    
    ObjUtility::getKartZdir(param_1, &local_58);
    if (checkKartCrash()) {
        kartZDir.set(_238);
    }
    else {
        kartZDir.x = local_58.x;
        kartZDir.y = 0.0;
        kartZDir.z = local_58.z;
    }

    local_58.squared();
    kartZDir.normalize();

    if (kartZDir.isZero()) {
        kartZDir.x = 1.0;
        kartZDir.y = 0.0;
        kartZDir.z = 0.0;
    }

    vecZDir->set(kartZDir);
    _238.set(kartZDir);
}

// TODO: Once we work out correct JGeometry implementations for TRot3f and TQuat4f,
//      this template below can be uncommented and adjusted as needed.
// template <>
// inline void JGeometry::TRot3f::setRotate(const TVec3f &a, const TVec3f &b) {
//     TQuat4f q;
//     q.setRotate(a, b);
//     setQuat(q);
// }

void TJugem::resetJugemOrigin(const JGeometry::TVec3f &param_1, const JGeometry::TVec3f &param_2) {
    JGeometry::TVec3f jugemOrigin;
    JGeometry::TVec3f newPos;
    
    _22c.set(param_1);
    getJugemOrigin(param_2, &jugemOrigin);
    globalMove(&jugemOrigin);

    _214.set(jugemOrigin);
    localMove(&jugemOrigin);
    setPosition(jugemOrigin);

    newPos.sub(_22c, mPos);
    newPos.y = 0.0f;

    newPos.squaredZX();
    newPos.normalize();

    static JGeometry::TVec3f zAxis(0.0f, 0.0f, 1.0f);
    mRotMtx.setRotate(zAxis, newPos);
}

void TJugem::getJugemOrigin(const JGeometry::TVec3f &param_1, JGeometry::TVec3f *param_2) {
    JGeometry::TVec3f temp;
    temp.set(param_1);
    temp.setLength(scJugemDistance);

    if (temp.y < 0.0f) {
        temp.y = 200.0f;
    } else {
        temp.y += 200.0f;
    }

    param_2->set(temp);
}

void TJugem::move(int kartIndex) {
    JGeometry::TVec3f local_c8;
    JGeometry::TVec3f local_d4;
    JGeometry::TVec3f kartPosCopy;
    JGeometry::TVec3f kartPosDiff;
    JGeometry::TVec3f local_f8;
    JGeometry::TVec3f local_104;
    JGeometry::TVec3f local_110;
    JGeometry::TVec3f kartZDir;
    JGeometry::TVec3f local_128;
    
    if (tstObjFlagHidding() != 0) {
        return;
    }

    if (_274 != 0) {
        ObjUtility::getKartPos(kartIndex, &_22c);
        kartPosCopy = _22c;
        kartPosDiff.sub(_22c, kartPosCopy);

        const f32 kSnapThreshold = 0.1f;
        if (kartPosDiff.squared() < kSnapThreshold) {
            _22c.x = kartPosCopy.x;
            _22c.y = kartPosCopy.y;
            _22c.z = kartPosCopy.z;
        }
    }

    ObjUtility::getKartZdir(kartIndex, &kartZDir);
    if (checkKartCrash()) {
        local_110.set(_238);
    } else {
        local_110.set(kartZDir.x, 0.0f, kartZDir.z);
    }

    local_110.normalize();

    if (local_110.isZero()) {
        local_110.set(1.0f, 0.0f, 0.0f);
    }
    
    local_d4.set(local_110);
    _238.set(local_110);
    local_128.set(local_d4);
    local_128.setLength(scJugemDistance);
    
    if (local_128.y < 0.0f) {
        local_128.y = 200.0f;
    } else {
        local_128.y += 200.0f;
    }

    local_c8.set(local_128);
    globalMove(&local_c8);
    chase(kartIndex, local_c8, _214, local_c8);
    _214.set(local_c8);
    mSplineInterp.update();

    localMove(&local_c8);

    f32 limitY = 5.0f;

    switch (getState()) {
        
        case 5:
            if (mSplineInterp.checkUnknownBool13() != 0) {
                limitY = 10.0f;
            }
            break;
        case 9:
            limitY = 15.0f;
            break;

        default:
            break;
    }

    fixPosition(&local_c8, limitY);
    mVel.sub(local_c8, mPos);
    setPosition(local_c8);

    if (getGlobalState() == 1) {
        if (checkKartCrash()) {
            mRotMtx.getZDirInline(local_d4);
        } else {
            ObjUtility::getKartVel(mKartNum, &local_f8);
            if (local_f8.dot(local_d4) >= 0.0f) {
                limitY = local_f8.squared();
                local_f8.normalize();
                
                local_104.sub(mPos, _22c);
                limitY = local_104.squared();
                local_104.normalize();
                local_d4.add(local_f8, local_104);
                local_d4.negate();
            } else {
                mRotMtx.getZDirInline(local_d4);
            }
        }
    } else {
        local_d4.sub(_22c, mPos);
    }

    local_d4.y = 0.0f;
    local_d4.normalize();

    static JGeometry::TVec3f zAxis(0.0f, 0.0f, 1.0f);
    static JGeometry::TVec3f yAxis(0.0f, 1.0f, 0.0f);

    if (zAxis.dot(local_d4) < -0.9999f) {
        f32 s = sinf(JGeometry::TUtilf::PI());
        f32 c = cosf(JGeometry::TUtilf::PI());
        mRotMtx[1][1] = 1.0f;
        mRotMtx[0][0] = c;
        mRotMtx[0][2] = s;
        mRotMtx[2][0] = -s;
        mRotMtx[2][2] = c;
        mRotMtx[2][1] = 0.0f;
        mRotMtx[1][2] = 0.0f;
        mRotMtx[1][0] = 0.0f;
        mRotMtx[0][1] = 0.0f;
    } else {
        mRotMtx.setRotate(zAxis, local_d4);
    }

    if (mJugemItem != nullptr) {
        mJugemItem->setRMtx(mRotMtx);
    }
    GeographyObj::moveShadowModel();
}

bool TJugem::checkKartCrash() {
    bool kartHasCrashed = false;

    if ((KartCtrl::getKartCtrl()->getKartBody(mKartNum)->
        mCarStatus & 0x0000280001980000uLL) != 0
    ) {
        kartHasCrashed = true;
    }

    return kartHasCrashed;
}

void TJugem::setPosition(const JGeometry::TVec3f &newPos) {
    if (getState() == 7) {
        setState(8);
    }

    mPos.set(newPos);
    if (mJugemItem != nullptr) {
        mJugemItem->setPosition(newPos);
    }
}

void TJugem::setRMtx(const JGeometry::TPos3f &rotMtx) {
    if (getState() == 7) {
        resetAllPosAndMtx();
        setState(8);
    }

    mRotMtx.set(rotMtx.mMtx);

    if (mJugemItem != nullptr) {
        mJugemItem->setRMtx(rotMtx);
    }
}

void TJugem::resetAllPosAndMtx() {
    mPos.zero();
    PSMTXIdentity(mRotMtx);
    if (mJugemItem != nullptr) {
        mJugemItem->setPosition(mPos);
        mJugemItem->setRMtx(mRotMtx);
    }
}


void TJugem::localMove(JGeometry::TVec3f *vec) {
    const f32 half = 0.5f;
    f32 someX = _208.x;
    f32 someY = _208.y;
    f32 someZ = _208.z;

    if ((getGlobalState() == 1) && !mIsStartPoint) {
        someX *= -1.0f;
    }

    static JGeometry::TQuat4f qtAxisY;

    f32 dx = vec->x - _22c.x;
    f32 dz = vec->z - _22c.z;

    f32 theta = atan2f(dx, dz) * half;    
    f32 s = sinf(theta);
    f32 c = cosf(theta);

    qtAxisY.set(0.0f, s, 0.0f, c);

    JGeometry::TVec3f local;
    local.set(someX, someY, someZ);

    JGeometry::TVec3f rotated;
    qtAxisY.transform(local, rotated);

    vec->add(rotated);
}

void TJugem::globalMove(JGeometry::TVec3f *param_1) {
    param_1->add(_22c);
}

int TJugem::getGlobalState() {
    int globalState = 0;
    switch (getState()) {
        case 0:
        case 1:
            globalState = 0;
            break;
        case 2:
        case 3:
        case 4:
            globalState = 1;
            break;
        case 5:
        case 6:
            globalState = 2;
            break;
        case 7:
        case 8:
            globalState = 3;
            break;
        case 9:
            globalState = 4;
            break;
        case 10:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0xf:
        case 0x10:
            globalState = 5;
            break;
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
            globalState = 6;
            break;
        case 0x16:
            globalState = 7;
            break;
        }
    return globalState;
}

void TJugem::fixPosition(JGeometry::TVec3f* pos, f32 limitY) {
    if (getGlobalState() == 5 || (GetKartCtrl()->getKartBody(getKartNum())->mCarStatus) & 0x100000000ull) {
        return;
    }
    

    Course *course = RaceMgr::sRaceManager->getCourse();
    CrsGround crsGround(course);

    // Is this actually used for anything?
    JGeometry::TVec3f tempPos = mPos;

    setLimitation(pos, limitY, 15.0f);

    f32 groundY = fixGround(crsGround, *pos);
    fixWall(crsGround, pos);
    fixNear(pos);

    if (pos->y < groundY) {
        pos->y = groundY;
    }

    JGeometry::TVec3f test;
    test.sub(*pos, mPos);

    if (test.squared() < 1.0f) {
        pos->set(mPos);
    }
    return;
}

f32 TJugem::fixGround(CrsGround &crsGround, const JGeometry::TVec3f &param_2) {
    JGeometry::TVec3f local_3c;
    local_3c = param_2;
    crsGround.search(_22c);

    JGeometry::TVec3f crsGroundNormal;
    crsGround.getNormal(&crsGroundNormal);

    crsGroundNormal.squaredZX();
    crsGroundNormal.normalize();
    f32 fVar1 = JGeometry::TUtilf::sqrt(crsGroundNormal.squaredZX());

    f32 fVar4 = param_2.y;
    local_3c.y += scJugemDistance * fVar1;

    crsGround.exceptValley(1);
    crsGround.search(local_3c);
    crsGround.exceptValley(0);

    f32 crsGroundHeight = crsGround.getHeight();
    if ((fVar4 - crsGroundHeight) < 80.0f) {
        fVar4 = crsGroundHeight + 80.0f;
    }
    return fVar4;
}

// TODO: Fix Registers (code otherwise 100% match.)
void TJugem::fixWall(CrsGround &crsGround, JGeometry::TVec3f *pos) {
    JGeometry::TVec3f dir;
    dir.sub(_22c, *pos);

    const f32 step = 80.0f;
    const f32 stepHalf = 40.0f;

    int steps = dir.normalize() / step;
    dir.scale(step);

    if (steps > 0x14) {
        steps = 0x14;
    }

    f32 heightStep = 0.0f;
    if (steps > 0) {
        heightStep = ((pos->y + 100.0f) - _22c.y) / steps;
    }

    JGeometry::TVec3f cur;
    JGeometry::TVec3f next;

    for (s32 i = steps; i >= 0; i--) {
        JMAVECScaleAdd(&dir, pos, &cur, i);
        cur.y  = heightStep * (steps - i) + _22c.y;

        JMAVECScaleAdd(&dir, pos, &next, i + 1);
        next.y = heightStep * (steps - i - 1) + _22c.y;

        crsGround.search(cur);
        f32 minY = crsGround.getHeight() + stepHalf + step;
        if (cur.y < minY) {
            cur.y = minY;
        }

        crsGround.search(cur, next, step, false);

        cur.y = pos->y;

        int attr = crsGround.getAttribute();
        if (attr != CrsGround::Attr_2 && attr != CrsGround::Attr_18) {
            continue;
        }
        
        JGeometry::TVec3f normal;

        f32 strength = crsGround.getWallNormal(&normal, nullptr);
        if (strength > 0.1f) {
            if (strength > 1600.0f) {
                strength = 1600.0f;
            }

            JGeometry::TVec3f dirNorm(dir);
            dirNorm.normalize();

            f32 dot = dirNorm.dot(normal);
            if (dot != 0.0f) {
                dirNorm.scale(strength / dot);
            } else {
                dirNorm.zero();
            }

            dirNorm.y = 0.0f;
            pos->add(cur, dirNorm);
            break;
        }
    }
}

void TJugem::fixNear(JGeometry::TVec3f *param_1) {
    JGeometry::TVec3f distanceDiff;
    distanceDiff.sub(*param_1, _22c);

    f32 distSq = distanceDiff.squaredZX();
    f32 normalRange = SiUtil::getNormalRange(distSq, 10000.0f, 160000.0f);

    f32 newY;
    if ((1.0f - normalRange) > 0.0f) {
        newY = ((1.0f - normalRange) * 350.0f + _22c.y);
    }
    if (param_1->y < newY) {
        param_1->y = newY;
    }
}

void TJugem::setLimitation(JGeometry::TVec3f *param_1, f32 param_2, f32 param_3) {
    JGeometry::TVec3f kartVel;
    ObjUtility::getKartVel(mKartNum, &kartVel);

    f32 kartVelMag = PSVECMag(&kartVel);

    JGeometry::TVec3f local_70;
    local_70.sub(*param_1, mPos);

    if (param_2 > 0.0f) {
        f32 maxLen = kartVelMag + param_2;
        f32 yPosDiff = local_70.y;

        JGeometry::TVec3f local_7c;
        local_7c = local_70;
        local_7c.y = 0.0f;

        if (PSVECMag(&local_7c) > maxLen) {
            local_7c.setLength(maxLen);
            local_7c.y = yPosDiff;
            param_1->add(mPos, local_7c);
        }
    }

    local_70.sub(*param_1, mPos);

    JGeometry::TVec3f splineInterp;
    mSplineInterp.getVel(&splineInterp);

    f32 fVar1 = (kartVel.y + splineInterp.y) - param_3;
    f32 fVar2 = param_3 + (kartVel.y + splineInterp.y);

    if (local_70.y < fVar1) {
        param_1->y = mPos.y + fVar1;
    } else if (local_70.y > fVar2) {
        param_1->y = mPos.y + fVar2;
    }
}

void TJugem::chase(int param_1, const JGeometry::TVec3f &param_2, const JGeometry::TVec3f &param_3, JGeometry::TVec3f &param_4) {
    f32 vecMag;
    JGeometry::TPos3f local_60;
    JGeometry::TVec3f local_6c;
    JGeometry::TVec3f local_78;
    
    if (!param_2.equals(param_3)) {
        local_6c.sub(param_2, param_3);
        vecMag = PSVECMag(&local_6c);
        if (vecMag > (sChaseDistance ^ 0x80000000)) {
            local_6c.setLength(sChaseAccel);
            local_6c.y *= 1.5f;
            _220.add(local_6c);
            vecMag = PSVECMag(&_220);
            if (vecMag > 10.0f) {
                _220.setLength(10.0f);
            }
        } else {
            local_6c.setLength(sChaseDecel);
            local_6c.y *= 3.0f;
            if (_220.x * local_6c.x < 0.0f) {
                _220.x += local_6c.x;
            }
            if (_220.y * local_6c.y < 0.0f) {
                _220.y += local_6c.y;
            }
            if (_220.z * local_6c.z < 0.0f) {
                _220.z += local_6c.z;
            }
            vecMag = PSVECMag(&_220);
            if (vecMag < sChaseEndSpeed) {
                param_4.set(param_2);
                _220.zero();
            }
        }
        ObjUtility::getKartVel(param_1,&local_78);
        mVel.add(local_78, _220);
        param_4.add(param_3, mVel);
    }
}
void TJugem::hide() {
    setObjFlagHidding();
    ShadowModel *shadowModel = getShadowModel();
    if (shadowModel != nullptr) {
        shadowModel->clearVisible(mCameraNum);
    }

    if (mJugemItem) {
        mJugemItem->hide();
    }

    if (mJugemHeadItem) {
        mJugemHeadItem->hide();
    }
}

void TJugem::hideAll() {
    setObjFlagHidding();
    ShadowModel *shadowModel = getShadowModel();
    if (shadowModel != nullptr) {
        shadowModel->clearVisible(mCameraNum);
    }

    for (s32 i = 0; i < 3; i++) {
        TJugemItem *tJugemItem = mItem[i];
        if (tJugemItem != nullptr) {
            tJugemItem->hideAll();
        }
    }

    for (s32 i = 0; i < 2; i++) {
        TJugemHeadItem *tJugemHeadItem = mHeadItem[i];
        if (tJugemHeadItem != nullptr) {
            tJugemHeadItem->hideAll();
        }
    }
}

void TJugem::show(u8 id) {
    if (mJugemItem != nullptr) {
        mJugemItem->show(id);
    }

    if (mJugemHeadItem != nullptr) {
        mJugemHeadItem->show(id);
    }
    
    clrObjFlagHidding();
    ShadowModel *shadowModel = mShadowMdl;

    if (shadowModel != nullptr) {
        shadowModel->setVisible(mCameraNum);
        getShadowModel()->setAlpha(0xff);
    }
}

void TJugem::wearCap(u32 id) {
    if (mJugemHeadItem != nullptr) {
        mJugemHeadItem->hide();
    }
#line 1403
    JUT_MINMAX_ASSERT(0, id, 2)
    if (mHeadItem[id] != nullptr) {
        mJugemHeadItem = mHeadItem[id];
        mModel._14 = (u32 *)mJugemHeadItem;
    }
}

void TJugem::InitExec() {
    Observer_FindAndInit(TJugem, 23);
}

void TJugem::MoveExec() {
    Observer_FindAndExec(TJugem, 23);
}

void TJugem::changeAnimation(u32 type) {
    if (isDemoMode()) {
        #line 1448
        JUT_MINMAX_ASSERT(0, type, 3);
        mAnmCtrl = mDemoAnmController[type];
        mAnmPlayer.init(mDemoAnmController[type], sDemoAnmStateTable[type].tAnmInfo, sDemoAnmStateTable[type].arraySize);
    } else {
        #line 1452
        JUT_MINMAX_ASSERT(0, type, 5);
        mAnmCtrl = mAnmController[type];
        mAnmPlayer.init(mAnmController[type], sAnmStateTable[type].tAnmInfo, sAnmStateTable[type].arraySize);
    }
    if (mAnmCtrl->IsAvailableTrans()) {
        mAnmCtrl->StopTrans();
    }

    if (mJugemItem != nullptr) {
        mJugemItem->changeAnimation(type);
    }
}

bool TJugem::isAbleStart() {
    return mIsAbleStart;
}

void TJugem::cutFishLine() {
    if (getState() != 8) {
        return;
    }
    mAnmPlayer._10 = 1;
    mAnmPlayer._e |= 1;
    
    TJugemItem *tJugemItem = mJugemItem;
    if (tJugemItem == nullptr) {
        return;
    }
    tJugemItem->mJugemAnmPlayer._10 = 1;
    tJugemItem->mJugemAnmPlayer._e |= 1;
    return;
}

bool TJugem::nodeCallBack(J3DJoint* joint, int param_2) {
    if (param_2 == false) {
        // Get joint index
        u16 jointIndex = joint->mJointIdx;

        // Access global J3D system
        J3DModel* model = j3dSys._38;
        J3DModelData* modelData = model->mModelData;

        // FIX: This is almost certainly wrong. Need to work out correct casting.
        TJugemHeadItem* obj = (TJugemHeadItem *)((ExModel *)model->getUserArea())->_14;
        if (obj) {
            Mtx& mtx = model->getAnmMtx(jointIndex);

            JGeometry::TPos3f copy;
            copy.set(mtx);

            JGeometry::TVec3f pos;
            pos.set(mtx[0][3], mtx[1][3], mtx[2][3]);

            obj->setPosition(pos);   // vtable + 0x80
            obj->setRMtx(copy);      // vtable + 0x84
        }
    }
    return true;
}

void TJugem::setCameraNum(u8 cam) {
    #line 1520
    JUT_ASSERT_MSG(cam < scJugemCameraMax, "cam < scJugemCameraMax");
    mCameraNum = cam;
    if (mSignal != nullptr) {
        mSignal->mKartCamIndex = mCameraNum;
    }
}

void TJugem::setKartNum(u8 kart) {
    #line 1530
    u8 scJugemKartMax = 8;
    JUT_ASSERT_MSG(kart < scJugemKartMax, "kart < scJugemKartMax");
    mKartNum = kart;
}

u32 TJugem::getScreenType() {
    s32 consoleNum = RaceMgr::getCurrentManager()->getConsoleNumber();
    s32 screenType = 0;

    #line 1711
    JUT_ASSERT_MSG(consoleNum > 0, "consoleNum > 0");

    // TODO: See if this can become an enum.
    switch (consoleNum) {
        case 1:
            screenType = 0;
            break;
        case 2:
            screenType = 1;
            break;
        case 3:
        case 4:
            screenType = 2;
            break;
        default:
            #line 1725
            JUT_ASSERT_MSG(false, "false");
            break;
    }
    return screenType;
}

#include "JSystem/JAudio/JASFakeMatch2.h"
