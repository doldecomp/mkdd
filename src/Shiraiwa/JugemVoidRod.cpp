#include "JSystem/JAudio/JASFakeMatch2.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Sato/AnmController.h"
#include "Shiraiwa/AnmPlayer.h"
#include "Shiraiwa/JugemItem.h"
#include "Shiraiwa/JugemMain.h"
#include "Shiraiwa/JugemRodItem.h"
#include "types.h"

const s32 TJugemRodItem::cJugemRodItem_Max = 6;

TAnmInfo TJugemVoidRod::sAnmInfos_Rod_Lap[4] = {
    {"/Objects/rod_wz2fg.bca", nullptr, nullptr, 0, 30, 1, 1, 0},
    {"/Objects/rod_lk.bca", nullptr, nullptr, 2, 30, 2, 2, 0},
    {"/Objects/rod_by.bca", nullptr, nullptr, 2, 30, 2, 3, 0},
    {"/Objects/rod_wz.bca", nullptr, nullptr, 2, 0, 0, 255, 0},
};

TAnmInfo TJugemVoidRod::sAnmInfos_Rod_Start[5] = {
    {"/Objects/rod_th.bca", nullptr, nullptr, 0, 60, 1, 1, 0},
    {"/Objects/rod_ww.bca", nullptr, nullptr, 2, 60, 0, 2, 0},
    {"/Objects/rod_cd.bca", nullptr, nullptr, 0, 10, 1, 3, 0},
    {"/Objects/rod_go.bca", nullptr, nullptr, 0, 60, 1, 4, 0},
    {"/Objects/rod_wz.bca", nullptr, nullptr, 2, 0, 0, 255, 0},
};

TAnmInfo TJugemVoidRod::sAnmInfos_Rod_Reverse[2] = {
    {"/Objects/rod_no.bca", nullptr, nullptr, 2, 30, 6, 1, 0},
    {"/Objects/rod_st.bca", nullptr, nullptr, 2, 30, 6, 0, 0},
};

TAnmInfo TJugemVoidRod::sAnmInfos_Rod_Rescue[3] = {
    { "/Objects/rod_ca.bca", nullptr, nullptr, 2, 0, 0, 1, 0 },
    { "/Objects/rod_cu.bca", nullptr, nullptr, 0, 0, 0, 2, 0 },
    { "/Objects/rod_wz.bca", nullptr, nullptr, 2, 0, 0, 255, 0 },
};

TAnmInfo TJugemVoidRod::sAnmInfos_Rod_Signal[3] = {
    { "/Objects/jg_rod_in_a.bck", nullptr, nullptr, 0, 0, 1, 1, 0 },
    { "/Objects/jg_rod_wait.bck", nullptr, nullptr, 2, 0, 0, 2, 0 },
    { "/Objects/jg_rod_out.bck",  nullptr, nullptr, 0, 0, 1, 255, 0 },
};

TAnmInfo TJugemVoidRod::sAnmInfos_Rod_PukuS[3] = {
    {"/Objects/jg_rod_in_b.bck", nullptr, nullptr, 0, 0, 1, 1, 0},
    {"/Objects/jg_rod_wait.bck", nullptr, nullptr, 2, 0, 0, 2, 0},
    {"/Objects/jg_rod_out.bck",  nullptr, nullptr, 0, 0, 1, 255, 0},
};

TAnmInfo TJugemVoidRod::sAnmInfos_Rod_PukuL[3] = {
    { "/Objects/jg_rod_in_c.bck", nullptr, nullptr, 0, 0, 1, 1, 0 },
    { "/Objects/jg_rod_wait.bck", nullptr, nullptr, 2, 0, 0, 2, 0 },
    { "/Objects/jg_rod_out.bck",  nullptr, nullptr, 0, 0, 1, 255, 0 },
};

TJugemAnmTableEntry TJugemVoidRod::sAnmStateTable[4] = {
    { sAnmInfos_Rod_Start, 5 },
    { sAnmInfos_Rod_Lap, 4 },
    { sAnmInfos_Rod_Rescue, 3 },
    { sAnmInfos_Rod_Reverse, 2 },
};

TJugemAnmTableEntry TJugemVoidRod::sDemoAnmStateTable[3] = {
    { sAnmInfos_Rod_Signal, 3 },
    { sAnmInfos_Rod_PukuS, 3 },
    { sAnmInfos_Rod_PukuL, 3 },
};


TJugemVoidRod::TJugemVoidRod() : TJugemItem(0x101) {
    _184 = nullptr;
    if (TJugem::isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            mDemoRodAnmController[i] = new AnmController();
        }
    } else {
        for (int i = 0; i < 4; i++) {
            mRodAnmController[i] = new AnmController();
        }
    }

    reset();

    for (int i = 0; i < TJugemRodItem::cJugemRodItem_Max; i++) {
        _188[i] = nullptr;
    }
    
    if (TJugem::isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            TAnmPlayer::resetAnimations(sDemoAnmStateTable[i].tAnmInfo, sDemoAnmStateTable[i].arraySize);
        }
    } else {
        for (int i = 0; i < 4; i++) {
            TAnmPlayer::resetAnimations(sAnmStateTable[i].tAnmInfo, sAnmStateTable[i].arraySize);
        }
    }
}

TJugemVoidRod::~TJugemVoidRod() {
    if (TJugem::isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            delete mDemoRodAnmController[i];
        }
    } else {
        for (int i = 0; i < 4; i++) {
            delete mRodAnmController[i];
        }
    }
}

void TJugemVoidRod::reset() {
    resetObject();
    _58 = 0;
    clrAllCheckKartHitFlag();
    clrObjFlagCheckGeoHitting();
    clrObjFlagCheckItemHitting();
    setObjFlagHidding();
    if (_184 != nullptr) {
        _184->hide();
    }
}

void TJugemVoidRod::loadAnimation() {
    J3DModelData *modelData = mModel.getModelData();
    if (TJugem::isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            TAnmPlayer::loadAnimations(sDemoAnmStateTable[i].tAnmInfo, sDemoAnmStateTable[i].arraySize, modelData, ResMgr::mcArcOpening);
        }
    } else {
        for (int i = 0; i < 4; i++) {
            TAnmPlayer::loadAnimations(sAnmStateTable[i].tAnmInfo, sAnmStateTable[i].arraySize, modelData, ResMgr::mcArcMRAM);
        }
    }
}

const char *TJugemVoidRod::getShadowBmdFileName() {
    return NULL;
}

const char *TJugemVoidRod::getBmdFileName() {
    static const char *cBmdName = "/Objects/rod_model.bmd";
    return cBmdName;
}

void TJugemVoidRod::createModel(JKRSolidHeap *jkrSolidHeap, u32 param_2) {
    mModel.createModel(jkrSolidHeap, param_2, 0);

    _1a4 = mModel.getModelData()->getJointName()->getIndex("ef_hook");
    J3DJoint* joint = mModel.getModelData()->getJointNodePointer(_1a4);
    joint->setCallBack(nodeCallBack);

    mModel._14 = nullptr;
    if (TJugem::isDemoMode()) {
        for (int i = 0; i < 3; i++) {
            TAnmPlayer::registAnimations(mDemoRodAnmController[i], &mModel, sDemoAnmStateTable[i].tAnmInfo, sDemoAnmStateTable[i].arraySize);
        }
    } else {
        for (int i = 0; i < 4; i++) {
            TAnmPlayer::registAnimations(mRodAnmController[i], &mModel, sAnmStateTable[i].tAnmInfo, sAnmStateTable[i].arraySize);
        }
    }
}

void TJugemVoidRod::calc() {
    if (!tstObjFlagHidding()) {
        mJugemAnmPlayer.update();
    }
}

void TJugemVoidRod::createColModel(J3DModelData *j3dModelData) {}

void TJugemVoidRod::setPosition(const JGeometry::TVec3f &rPos) { 
    mPos.set(rPos); 
}

void TJugemVoidRod::setRMtx(const JGeometry::TPos3f &rMtx) { 
    mRotMtx.set(rMtx.mMtx); 
}


void TJugemVoidRod::hideModel(u32 param_1) {
    mModel.clipAll(param_1, false);
    if (_184 != nullptr) {
        _184->mModel.clipAll(param_1, false);
    }
}

void TJugemVoidRod::show(u8 param_1) {
    clrObjFlagHidding();
    if (_184 != nullptr) {
        _184->show(param_1);
    }
}

void TJugemVoidRod::update() {
    setModelMatrixAndScale();
    mModel.update(0);
}

void TJugemVoidRod::hide() {
    setObjFlagHidding();
    if (_184 != nullptr) {
        _184->hide();
    }
}

void TJugemVoidRod::hideAll() {
    setObjFlagHidding();
    for (int i = 0; i < TJugemRodItem::cJugemRodItem_Max; i++) {
        if (_188[i] != nullptr) {
            _188[i]->hideAll();
        }
    }
}

void TJugemVoidRod::setJugemRodItem(TJugemRodItem *rodItem, u32 type) {
    #line 341
    JUT_ASSERT_MSG(type < TJugemRodItem::cJugemRodItem_Max, "id < TJugemRodItem::cJugemRodItem_Max");
    _188[type] = rodItem;
}

void TJugemVoidRod::hold(u32 id) {
    if (_184 != nullptr) {
        _184->hide();
    }
#line 356
    JUT_ASSERT_MSG(id < TJugemRodItem::cJugemRodItem_Max, "id < TJugemRodItem::cJugemRodItem_Max");
    if (id == 0) {
        _184 = nullptr;
    } else if (_188[id] != nullptr) {
        _184 = _188[id];
    }
    mModel._14 = (u32 *)_184;
}

void TJugemVoidRod::changeAnimation(u32 type) {
    if (TJugem::isDemoMode()) {
        #line 376
        JUT_MINMAX_ASSERT(0, type, 3);
        mAnmCtrl = mDemoRodAnmController[type];
        mJugemAnmPlayer.init(mDemoRodAnmController[type], sDemoAnmStateTable[type].tAnmInfo, sDemoAnmStateTable[type].arraySize);
    } else {
        #line 380
        JUT_MINMAX_ASSERT(0, type, 4);
        mAnmCtrl = mRodAnmController[type];
        mJugemAnmPlayer.init(mRodAnmController[type], sAnmStateTable[type].tAnmInfo, sAnmStateTable[type].arraySize);
    }

    if (mAnmCtrl->IsAvailableTrans()) {
        mAnmCtrl->StopTrans();
    }
}

bool TJugemVoidRod::nodeCallBack(J3DJoint *joint, int param_2) {
    if (!param_2) {
        u16 jointIndex = joint->mJointIdx;

        J3DModel *model = j3dSys._38;
        J3DModelData *modelData = model->mModelData;

        // FIX: This is almost certainly wrong.
        //      Need to find out what casts to use to make this work correctly.
        TJugemRodItem *obj = (TJugemRodItem *)((ExModel *)model->getUserArea())->_14;
        if (obj) {
            Mtx& mtx = model->getAnmMtx(jointIndex);

            JGeometry::TPos3f copy;
            copy.set(mtx);

            JGeometry::TVec3f pos;
            pos.set(mtx[0][3], mtx[1][3], mtx[2][3]);

            obj->setPosition(pos);   // 0x80
            obj->setRMtx(copy);      // 0x84
        }
    }
    return true;
}
