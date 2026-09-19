#include "JSystem/J3D/J3DSys.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "Kaneshige/Course/CrsData.h"
#include "Shiraiwa/Objects/MapObjHioNode.h"
#include "Shiraiwa/Objects/MapObjFerriswheel.h"
#include "dolphin/mtx.h"
#include "math.h"
#include "types.h"


const char *TMapObjFerriswheel::scGondolaJointName[16] = {
    "arm01", "arm02", "arm03", "arm04",
    "arm05", "arm06", "arm07", "arm08",
    "arm09", "arm10", "arm11", "arm12",
    "arm13", "arm14", "arm15", "arm16",
};

s32 TMapObjFerriswheel::sRotFrame = 9000;
const char *TMapObjFerriswheel::scRotJointName = "kanran_skl_root";
s32 TMapObjFerriswheel::sRotJointNo;

TMapObjFerriswheel::TMapObjFerriswheel(const CrsData::SObject &sObject) : TMapObjHioNode(sObject) {
    PSMTXIdentity(_14c);
    PSMTXIdentity(_17c);
    PSMTXIdentity(_1ac);
}

TMapObjFerriswheel::~TMapObjFerriswheel() {}

const char *TMapObjFerriswheel::getBmdFileName() {
    static const char *cBmdName = "/Objects/BabyKanran.bmd";
    return cBmdName;
}

void TMapObjFerriswheel::createModel(JKRSolidHeap *heap, u32 p2) {
    mModel.createModel(heap, p2, 0);
    setCallBack(scRotJointName, centerCallBack);

    // Create each individual gondola on the ferris wheel.
    for (int i = 0; i < 16; i++) {
        setCallBack(scGondolaJointName[i], gondolaCallBack);
    }
}

// FIX: Why is J3DJointCallBack's return type an int for this function, but everywhere else it's a bool?
// This is the second time I've come across this, the other is in MapObjPakkun.
void TMapObjFerriswheel::setCallBack(const char *jointName, J3DJointCallBack2 jointCallBack) {
    u16 jointIdx = mModel.getModelData()->getJointName()->getIndex(jointName);
    mModel.getModelData()->getJointTree().getJointNodePointer(jointIdx)->setCallBack((J3DJointCallBack)jointCallBack);
    mModel._14 = (u32 *)this;
}

void TMapObjFerriswheel::reset() {
    JGeometry::TVec3f vecDirX;
    JGeometry::TVec3f vecDirZ;
    JGeometry::TVec3f vecDirY;
    
    GeographyObj::resetObject();
    clrObjFlagHidding();
    clrObjFlagCheckItemHitting();
    clrAllCheckKartHitFlag();

    PSMTXIdentity(_14c);
    PSMTXIdentity(_17c);

    // might be setRotate?
    f32 angle = (-F_TAU / sRotFrame);
    f32 s = sinf(angle);
    f32 c = cosf(angle);

    _17c[0][0] = c;
    _17c[0][1] = -s;
    _17c[1][0] = s;
    _17c[1][1] = c;
    _17c[2][2] = 1.0f;
    _17c[2][1] = 0.0f;
    _17c[1][2] = 0.0f;
    _17c[2][0] = 0.0f;
    _17c[0][2] = 0.0f;

    PSMTXIdentity(_1ac);
    vecDirY.set(mObjData->zNorm);
    vecDirY.normalize();
    
    vecDirX.set(mObjData->yNorm);
    vecDirX.negate();
    vecDirX.normalize();
    
    vecDirZ.cross(vecDirY, vecDirX);
    vecDirZ.normalize();

    _1ac.setXYZDir(vecDirX, vecDirY, vecDirZ);
}

void TMapObjFerriswheel::calc() {}

// Inline/Unused
void TMapObjFerriswheel::rotate(MtxPtr) {}
void TMapObjFerriswheel::setGondolaMtx(int, MtxPtr) {}

// TODO: These return types might be Booleans. Validate if that's the case?
int TMapObjFerriswheel::centerCallBack(J3DJoint *joint, int idx) {
    if (idx == 0) {
        int jointIndex = joint->getJntNo();
        // Access global J3D system
        J3DModel *model = j3dSys.getModel();
        TMapObjFerriswheel *obj = (TMapObjFerriswheel *)((ExModel *)model->getUserArea())->_14;
        if (obj != nullptr) {
            Mtx &mtx = model->getAnmMtx(jointIndex);
            PSMTXConcat(obj->_17c, obj->_14c, obj->_14c);
            PSMTXConcat(mtx, obj->_14c, mtx);
            PSMTXCopy(mtx, obj->mModel.getModel()->getAnmMtx(sRotJointNo));
            J3DSys::setCurrentMtx(mtx);
        }
    }
    return 1;
}

int TMapObjFerriswheel::gondolaCallBack(J3DJoint *joint, int idx) {
    JGeometry::TPos3f transMtx;
    
    if (idx == 0) {
        int jointIndex = joint->getJntNo();
        // Access global J3D system
        J3DModel *model = j3dSys.getModel();
        TMapObjFerriswheel *obj = (TMapObjFerriswheel *)((ExModel *)model->getUserArea())->_14;
        if (obj != nullptr) {
            Mtx &mtx = model->getAnmMtx(jointIndex);
            PSMTXCopy(obj->_1ac, transMtx);
            transMtx[0][3] = mtx[0][3];
            transMtx[1][3] = mtx[1][3];
            transMtx[2][3] = mtx[2][3];
            PSMTXCopy(transMtx, obj->mModel.getModel()->getAnmMtx(jointIndex));
            PSMTXCopy(transMtx, mtx);
            J3DSys::setCurrentMtx(mtx);
        }
    }
    return 1;
}

#include "JSystem/JAudio/JASFakeMatch2.h"
