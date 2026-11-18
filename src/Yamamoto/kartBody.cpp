#include "Yamamoto/kartBody.h"
#include "JSystem/JAudio/JASFakeMatch2.h"
#include "JSystem/JGeometry/Vec.h"
#include "Kameda/MotorManager.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/ExModel.h"
#include "Kaneshige/KartInfo.h"
#include "Kaneshige/KartLoader.h"
#include "Kaneshige/RaceMgr.h"
#include "Kaneshige/SysDebug.h"
#include "Osako/ResMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/JPEffectPerformer.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Yamamoto/kartCtrl.h"
#include "Yamamoto/KartSound.h"
#include "Yamamoto/kartParams.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "kartEnums.h"
#include "types.h"
#include <std/math.h>
#include "mathHelper.h"
// settingPtr2
// settingMiniPtr2

// body settings
static const char *marioBody[2] = {
    "/setting/i_mario_car.set",
    "/setting/i_mario_car.set"
};

static const char *babymarioBody[2] = {
    "/setting/a_babymario_car.set",
    "/setting/a_babymario_car.set"
};

static const char *nokonokoBody[2] = {
    "/setting/c_nokonoko_car.set",
    "/setting/c_nokonoko_car.set"
};

static const char *peachBody[2] = {
    "/setting/k_peach_car.set",
    "/setting/k_peach_car.set"
};

static const char *yossyBody[2] = {
    "/setting/m_yoshi_car.set",
    "/setting/m_yoshi_car.set"
};

static const char *warioBody[2] = {
    "/setting/p_wario_car.set",
    "/setting/p_wario_car.set"
};

static const char *donkyBody[2] = {
    "/setting/q_donkey_car.set",
    "/setting/q_donkey_car.set"
};

static const char *kuppaBody[2] = {
    "/setting/r_koopa_car.set",
    "/setting/r_koopa_car.set"
};

static const char *luigiBody[2] = {
    "/setting/j_luigi_car.set",
    "/setting/j_luigi_car.set"
};

static const char *babyluigiBody[2] = {
    "/setting/b_babyluigi_car.set",
    "/setting/b_babyluigi_car.set"
};

static const char *daisyBody[2] = {
    "/setting/l_daisy_car.set",
    "/setting/l_daisy_car.set"
};

static const char *catherineBody[2] = {
    "/setting/n_catherine_car.set",
    "/setting/n_catherine_car.set"
};

static const char *patapataBody[2] = {
    "/setting/d_patapata_car.set",
    "/setting/d_patapata_car.set"
};

static const char *diddykongBody[2] = {
    "/setting/e_diddykong_car.set",
    "/setting/e_diddykong_car.set"
};

static const char *waluigiBody[2] = {
    "/setting/o_waluigi_car.set",
    "/setting/o_waluigi_car.set"
};

static const char *koopajrBody[2] = {
    "/setting/f_koopajr_car.set",
    "/setting/f_koopajr_car.set"
};

static const char *kinopioBody[2] = {
    "/setting/g_kinopio_car.set",
    "/setting/g_kinopio_car.set"
};

static const char *kinopiogirlBody[2] = {
    "/setting/h_kinopiogirl_car.set",
    "/setting/h_kinopiogirl_car.set"
};

static const char *pakkunBody[2] = {
    "/setting/s_pakkun_car.set",
    "/setting/s_pakkun_car.set"
};

static const char *teresaBody[2] = {
    "/setting/t_teresa_car.set",
    "/setting/t_teresa_car.set"
};

static const char *extraBody[2] = {
    "/setting/u_extra_car.set",
    "/setting/u_extra_car.set"
};

static const char *marioBodyMini[2] = {
    "/setting/i_mario_car_mini.set",
    "/setting/i_mario_car_mini.set"
};

static const char *babymarioBodyMini[2] = {
    "/setting/a_babymario_car_mini.set",
    "/setting/a_babymario_car_mini.set"
};

static const char *nokonokoBodyMini[2] = {
    "/setting/c_nokonoko_car_mini.set",
    "/setting/c_nokonoko_car_mini.set"
};

static const char *peachBodyMini[2] = {
    "/setting/k_peach_car_mini.set",
    "/setting/k_peach_car_mini.set"
};

static const char *yossyBodyMini[2] = {
    "/setting/m_yoshi_car_mini.set",
    "/setting/m_yoshi_car_mini.set"
};

static const char *warioBodyMini[2] = {
    "/setting/p_wario_car_mini.set",
    "/setting/p_wario_car_mini.set"
};

static const char *donkyBodyMini[2] = {
    "/setting/q_donkey_car_mini.set",
    "/setting/q_donkey_car_mini.set"
};

static const char *kuppaBodyMini[2] = {
    "/setting/r_koopa_car_mini.set",
    "/setting/r_koopa_car_mini.set"
};

static const char *luigiBodyMini[2] = {
    "/setting/j_luigi_car_mini.set",
    "/setting/j_luigi_car_mini.set"
};

static const char *babyluigiBodyMini[2] = {
    "/setting/b_babyluigi_car_mini.set",
    "/setting/b_babyluigi_car_mini.set"
};

static const char *daisyBodyMini[2] = {
    "/setting/l_daisy_car_mini.set",
    "/setting/l_daisy_car_mini.set"
};

static const char *catherineBodyMini[2] = {
    "/setting/n_catherine_car_mini.set",
    "/setting/n_catherine_car_mini.set"
};

static const char *patapataBodyMini[2] = {
    "/setting/d_patapata_car_mini.set",
    "/setting/d_patapata_car_mini.set"
};

static const char *diddykongBodyMini[2] = {
    "/setting/e_diddykong_car_mini.set",
    "/setting/e_diddykong_car_mini.set"
};

static const char *waluigiBodyMini[2] = {
    "/setting/o_waluigi_car_mini.set",
    "/setting/o_waluigi_car_mini.set"
};

static const char *koopajrBodyMini[2] = {
    "/setting/f_koopajr_car_mini.set",
    "/setting/f_koopajr_car_mini.set"
};

static const char *kinopioBodyMini[2] = {
    "/setting/g_kinopio_car_mini.set",
    "/setting/g_kinopio_car_mini.set"
};

static const char *kinopiogirlBodyMini[2] = {
    "/setting/h_kinopiogirl_car_mini.set",
    "/setting/h_kinopiogirl_car_mini.set"
};

static const char *pakkunBodyMini[2] = {
    "/setting/s_pakkun_car_mini.set",
    "/setting/s_pakkun_car_mini.set"
};

static const char *teresaBodyMini[2] = {
    "/setting/t_teresa_car_mini.set",
    "/setting/t_teresa_car_mini.set"
};

static const char *extraBodyMini[2] = {
    "/setting/u_extra_car_mini.set",
    "/setting/u_extra_car_mini.set"
};

static const char **settingPtr2[21] = {
    marioBody, donkyBody, yossyBody, nokonokoBody,
    peachBody, babymarioBody, warioBody, kuppaBody,
    luigiBody, diddykongBody, catherineBody, patapataBody,
    daisyBody, babyluigiBody, waluigiBody, koopajrBody,
    kinopioBody, kinopiogirlBody, teresaBody, pakkunBody,
    extraBody
};

static const char **settingMiniPtr2[21] = {
    marioBodyMini, donkyBodyMini, yossyBodyMini, nokonokoBodyMini,
    peachBodyMini, babymarioBodyMini, warioBodyMini, kuppaBodyMini,
    luigiBodyMini, diddykongBodyMini, catherineBodyMini, patapataBodyMini,
    daisyBodyMini, babyluigiBodyMini, waluigiBodyMini, koopajrBodyMini,
    kinopioBodyMini, kinopiogirlBodyMini, teresaBodyMini, pakkunBodyMini,
    extraBodyMini
};

// comments inside functions are inline functions being called in that function

void KartBody::DegubBody(unsigned long unknown) {
    SysDebug::checkNaNMatrix(_110, unknown);
    SysDebug::checkNaNVector(&mVel, unknown);
    SysDebug::checkNaNVector(&mWg, unknown);
    SysDebug::checkNaNVector(&_2cc, unknown);
    SysDebug::checkNaNVector(&_2d8, unknown);
}

void KartBody::MakeMassBody(f32 f1, f32 f2, f32 f3, f32 f4) {
    f32 massBodyMultiplier = f2 * f3 * f4 * f1 * 8.0f;
    _3a4 = massBodyMultiplier;
    massBodyMultiplier = (massBodyMultiplier / 12.0f) * 4.0f;
    _230.set(
        massBodyMultiplier * (f3 * f3 + f4 * f4),
        massBodyMultiplier * (f4 * f4 + f2 * f2),
        massBodyMultiplier * (f2 * f2 + f3 * f3)
    );
    return;
}

void KartBody::MakeRigidVertexBody(f32 f1, f32 f2, f32 f3, f32 f4) {
    MakeMassBody(f1, f2, f3, f4);
}

void KartBody::SetUpRigidBodyPose(JGeometry::TVec3f *inVecOne, JGeometry::TVec3f *inVecTwo) {
    Mtx mtx;
    
    mPos.set(inVecOne->x, inVecOne->y, inVecOne->z);
    mPrevPos.set(inVecOne->x, inVecOne->y, inVecOne->z);
    mVel.zero();
    mWg.zero();
    _2cc.zero();
    _2d8.zero();
    _2c0.zero();
    mSpeed = 0.0f;
    _448 = 0.0f;
    mCarRPM = 0.0f;

    _338.set(inVecTwo->x, inVecTwo->y, inVecTwo->z);

    GetKartCtrl()->RotYMatrix(mtx, std::atan2f(inVecTwo->x,inVecTwo->z));
    GetKartCtrl()->SetPosePosMatrix(_110, mtx, &mPos);

    PSMTXCopy(_110, mPlayerPosMtx);
    PSMTXCopy(_110, _1a0);

    f32 kartBodyOffset = GetKartCtrl()->GetKartBodyOffset(mMynum);
    mPlayerPosMtx[0][3] += mPlayerPosMtx[0][1] * kartBodyOffset;
    mPlayerPosMtx[1][3] += mPlayerPosMtx[1][1] * kartBodyOffset;
    mPlayerPosMtx[2][3] += mPlayerPosMtx[2][1] * kartBodyOffset;

    _1a0[0][3] = mPlayerPosMtx[0][3];
    _1a0[1][3] = mPlayerPosMtx[1][3];
    _1a0[2][3] = mPlayerPosMtx[2][3];

    _2f0.set(_110[0][0], _110[1][0], _110[2][0]);
    _2fc.set(_110[0][1], _110[1][1], _110[2][1]);
    _308.set(_110[0][2], _110[1][2], _110[2][2]);
}

void KartBody::SetUpRigidBodyStartGridPose() {
    JGeometry::TVec3f vec1;
    JGeometry::TVec3f vec2;
    JGeometry::TVec3f vec3;

    vec1.setNormal(getSus(0)->_c0, getSus(1)->_c0, getSus(2)->_c0);

    vec1.scale(-1.0f);
    vec2.cross(vec1, _338);
    vec3.cross(vec2, vec1);

    _110[0][0] = vec2.x;
    _110[1][0] = vec2.y;
    _110[2][0] = vec2.z;

    _110[0][1] = vec1.x;
    _110[1][1] = vec1.y;
    _110[2][1] = vec1.z;

    _110[0][2] = vec3.x;
    _110[1][2] = vec3.y;
    _110[2][2] = vec3.z;

    mPos.x = (
        getSus(0)->_c0.x + 
        getSus(1)->_c0.x + 
        getSus(2)->_c0.x + 
        getSus(3)->_c0.x
    ) / 4.0f;

    mPos.y = (
        getSus(0)->_c0.y + 
        getSus(1)->_c0.y + 
        getSus(2)->_c0.y + 
        getSus(3)->_c0.y
    ) / 4.0f;
    
    mPos.z = (
        getSus(0)->_c0.z + 
        getSus(1)->_c0.z + 
        getSus(2)->_c0.z + 
        getSus(3)->_c0.z
    ) / 4.0f;

    _110[0][3] = mPos.x;
    _110[1][3] = mPos.y;
    _110[2][3] = mPos.z;

    PSMTXCopy(_110, mPlayerPosMtx);
    PSMTXCopy(_110, _1a0);

    f32 kartBodyOffset = GetKartCtrl()->GetKartBodyOffset(mMynum);

    mPlayerPosMtx[0][3] += mPlayerPosMtx[0][1] * kartBodyOffset;
    mPlayerPosMtx[1][3] += mPlayerPosMtx[1][1] * kartBodyOffset;
    mPlayerPosMtx[2][3] += mPlayerPosMtx[2][1] * kartBodyOffset;

    _1a0[0][3] = mPlayerPosMtx[0][3];
    _1a0[1][3] = mPlayerPosMtx[1][3];
    _1a0[2][3] = mPlayerPosMtx[2][3];

    _2f0.set(_110[0][0], _110[1][0], _110[2][0]);
    _2fc.set(_110[0][1], _110[1][1], _110[2][1]);
    _308.set(_110[0][2], _110[1][2], _110[2][2]);

    mKartLoader->getExModelBody()->setBaseTRMtx(mPlayerPosMtx);
}

void KartBody::DoForce(JGeometry::TVec3f *inVecOne, JGeometry::TVec3f *inVecTwo) {
    JGeometry::TVec3f vecOne;
    JGeometry::TVec3f vecTwo;
    
    vecTwo.set(inVecOne->x - mPos.x, inVecOne->y - mPos.y, inVecOne->z - mPos.z);
    vecOne.cross(vecTwo, *inVecTwo);
    _2cc.add(_2cc, *inVecTwo);
    _2d8.add(_2d8, vecOne);
}

void KartBody::DoTq(JGeometry::TVec3f *inVecOne, JGeometry::TVec3f *inVecTwo) {
    JGeometry::TVec3f vecOne;
    JGeometry::TVec3f vecTwo;
    
    vecTwo.set(inVecOne->x - mPos.x, inVecOne->y - mPos.y, inVecOne->z - mPos.z);
    vecOne.cross(vecTwo, *inVecTwo);
    _2d8.add(_2d8, vecOne);
}

void KartBody::DoForceOnly(KartBody *kartBody, JGeometry::TVec3f *inVecOne, JGeometry::TVec3f *inVecTwo) {
    JGeometry::TVec3f vecOne;
    JGeometry::TVec3f vecTwo;

    vecTwo.set(
        inVecOne->x - kartBody->mPos.x,
        inVecOne->y - kartBody->mPos.y,
        inVecOne->z - kartBody->mPos.z
    );
    vecOne.cross(vecTwo, *inVecTwo);
    kartBody->_2cc.add(kartBody->_2cc, *inVecTwo);
}

void KartBody::DoForceOnly(JGeometry::TVec3f *inVecOne, JGeometry::TVec3f *inVecTwo) {
    JGeometry::TVec3f vecOne;
    JGeometry::TVec3f vecTwo;
    
    vecTwo.set(inVecOne->x - mPos.x, inVecOne->y - mPos.y, inVecOne->z - mPos.z);
    vecOne.cross(vecTwo, *inVecTwo);
    _2cc.add(_2cc, *inVecTwo);
    return;
}

void KartBody::DoGForce() {
    JGeometry::TVec3f vecGForce;
    f32 gravity;

    // Select gravity based on car status flags
    if ((mCarStatus & 0x100000) != 0) {
        gravity = -11.8f;
    } else if ((mCarStatus & 0x3) != 0) {
        gravity = -18.0f;
    } else if ((mCarStatus & 0x20000009000uLL) != 0) {
        gravity = -13.5f;
    } else {
        gravity = _3a0;
    }

    vecGForce.set(0.0f, gravity * _3a4, 0.0f);

    if (mBodyGround.getAttribute() != 6) {
        if (_458 > 60.0f && _32c.y < 0.705f && getTouchNum() != 0) {
            vecGForce.x += _3a0 * _3a4 * _2fc.x * 0.3f;
            vecGForce.y += _3a0 * _3a4 * _2fc.y * 0.3f;
            vecGForce.z += _3a0 * _3a4 * _2fc.z * 0.3f;

            _4d8 = 0.0f;
            _4d4 = 0.0f;
            _4c4 = 0.0f;
            _4c0 = 0.0f;

        } else if ((mCarStatus & 0x8000) != 0 && getTouchNum() != 0) {
            vecGForce.x += _3a0 * _3a4 * _2fc.x * 0.3f;
            vecGForce.z += _3a0 * _3a4 * _2fc.z * 0.3f;
        }
    }

    DoForce(&mPos, &vecGForce);
}

void KartBody::CalcMovement(JGeometry::TVec3f* out, JGeometry::TVec3f* a, JGeometry::TVec3f* b, JGeometry::TVec3f* c) {
    out->x = (a->y * (c->y - c->z) * a->z + b->x) / c->x;
    out->y = (a->z * (c->z - c->x) * a->x + b->y) / c->y;
    out->z = (a->x * (c->x - c->y) * a->y + b->z) / c->z;
}

int KartBody::DoCalcWgVel() {
    JGeometry::TVec3f vec1; // 0x44
    JGeometry::TVec3f vec2; // 0x38
    JGeometry::TVec3f vec3; // 0x2c
    JGeometry::TVec3f vec4; // 0x20
    JGeometry::TVec3f vec5; // 0x14
    JGeometry::TVec3f vec6; // 0x8, declared, but unused...?

    DegubBody(0x21);
    vec5.div(_3a4, _2cc);
    vec5.scale(mSpeedScale);
    mVel.add(vec5);

    DegubBody(0x22);
    GetKartCtrl()->DevMatrixByVector(&vec1, &_2d8, _110);

    DegubBody(0x23);
    vec2.set(_230);

    if ((getThunder()->mFlags & 1) != 0) {
        vec2.scale(getThunder()->getScale());
    }

    CalcMovement(&vec5, &mWg, &vec1, &vec2);
    vec3.set(
        mSpeedScale * vec5.x / 2.0f + mWg.x,
        mSpeedScale * vec5.y / 2.0f + mWg.y,
        mSpeedScale * vec5.z / 2.0f + mWg.z
    );

    CalcMovement(&vec5, &vec3, &vec1, &vec2);
    vec4.set(mSpeedScale * vec5.x, mSpeedScale * vec5.y, mSpeedScale * vec5.z);
    mWg.add(vec4);

    DegubBody(0x24);
    int statusCrl = getStrat()->DoStatusCrl();

    DegubBody(0x25);
    if ((mGameStatus & 0x20) != 0) {
        getGame()->DoStopItm();
    }
    if ((mCarStatus & 0x80001080000) == 0) {
        getStrat()->DoWgCrl(0.523333f);
    }

    DegubBody(0x26);
    PSMTXMultVecSR(_110, &mWg, &_2c0);

    _2cc.zero();
    _2d8.zero();
    DegubBody(0x27);

    return statusCrl;
}

void KartBody::DoPose() {
    Mtx mtxUnknown1;
    Mtx mtxUnknown2;
    JGeometry::TVec3f vecPos;
    JGeometry::TVec3f vecUnknown;

    int wgVel = DoCalcWgVel();

    _254.set(_1a0[0][3], _1a0[1][3], _1a0[2][3]);
    mPrevPos.set(mPos);

    if (wgVel != 2) {
        getStrat()->ShakeGround();
        getStrat()->DoAdjustment();

        if (wgVel == 3) {
            vecPos.set(mPos.x, mPos.y + mEffctVel.y, mPos.z);
        } else {
            vecPos.set(mPos.x + mEffctVel.x, mPos.y + mEffctVel.y, mPos.z + mEffctVel.z);
        }

        vecUnknown.set(mSpeedScale * _2c0.x, mSpeedScale * _2c0.y, mSpeedScale * _2c0.z);
        GetKartCtrl()->OmegaMatrix(mtxUnknown1, &vecUnknown, 0.0f);
        GetKartCtrl()->MulMatrix(mtxUnknown2, _110, mtxUnknown1);
        GetKartCtrl()->AddMatrix(_110, mtxUnknown2);

        getStrat()->DoPoseCrl();

        GetKartCtrl()->NormalizeMatrix(_110);
        mPos.set(vecPos.x, vecPos.y, vecPos.z);

        GetKartCtrl()->SetPosMatrix(_110, &vecPos);
        wgVel = mIdx;

#line 840
        JUT_MINMAX_ASSERT(0, mIdx, 22);
        
        PSMTXCopy(_110, mPlayerPosMtx);

        f32 kartBodyOffset = GetKartCtrl()->GetKartBodyOffset(mMynum);

        mPlayerPosMtx[0][3] = mPlayerPosMtx[0][1] * kartBodyOffset + mPlayerPosMtx[0][3];
        mPlayerPosMtx[1][3] = mPlayerPosMtx[1][1] * kartBodyOffset + mPlayerPosMtx[1][3];
        mPlayerPosMtx[2][3] = mPlayerPosMtx[2][1] * kartBodyOffset + mPlayerPosMtx[2][3];

        _2f0.set(_110[0][0], _110[1][0], _110[2][0]);
        _2fc.set(_110[0][1], _110[1][1], _110[2][1]);
        _308.set(_110[0][2], _110[1][2], _110[2][2]);
    }
}

void KartBody::DoCalcMassMatrix(Mtx massMatrix, f32 mass, JGeometry::TVec3f* invInertiaDiag, JGeometry::TVec3f* offset) {
    f32 invInertiaX = 1.0f / invInertiaDiag->x;
    f32 invInertiaY = 1.0f / invInertiaDiag->y;
    f32 invInertiaZ = 1.0f / invInertiaDiag->z;
    f32 invMass     = 1.0f / mass;

    massMatrix[0][0] = (invInertiaY * offset->z * offset->z) + (invInertiaZ * offset->y * offset->y) + invMass;
    massMatrix[1][0] = -invInertiaZ * offset->x * offset->y;
    massMatrix[2][0] = -invInertiaY * offset->x * offset->z;
    massMatrix[0][1] = massMatrix[1][0];
    massMatrix[1][1] = (invInertiaZ * offset->x * offset->x) + (invInertiaX * offset->z * offset->z) + invMass;
    massMatrix[2][1] = -invInertiaX * offset->y * offset->z;
    massMatrix[0][2] = massMatrix[2][0];
    massMatrix[1][2] = massMatrix[2][1];
    massMatrix[2][2] = (invInertiaX * offset->y * offset->y) + (invInertiaY * offset->x * offset->x) + invMass;
    return;
}

void KartBody::WallFriction(JGeometry::TVec3f *wallFriction, f32 mass, JGeometry::TVec3f *invInertiaDiag,
    JGeometry::TVec3f *offset, JGeometry::TVec3f *initialImpulse, f32 frictionValue)
{
    f32 invertedImpulse[3];
    f32 vecFriction[3];

    Mtx massMatrix;
    DoCalcMassMatrix(massMatrix, mass, invInertiaDiag, offset);
    
    Mtx33 newMassMatrix;
    newMassMatrix[0][0] = massMatrix[0][0];
    newMassMatrix[1][0] = massMatrix[1][0];
    newMassMatrix[2][0] = massMatrix[2][0];
    newMassMatrix[0][1] = massMatrix[0][1];
    newMassMatrix[1][1] = massMatrix[1][1];
    newMassMatrix[2][1] = massMatrix[2][1];
    newMassMatrix[0][2] = massMatrix[0][2];
    newMassMatrix[1][2] = massMatrix[1][2];
    newMassMatrix[2][2] = massMatrix[2][2];

    invertedImpulse[0] = -initialImpulse->x;
    invertedImpulse[1] = -initialImpulse->y;
    invertedImpulse[2] = -initialImpulse->z;

    if (GetKartCtrl()->Gauss(newMassMatrix, invertedImpulse, vecFriction) == 0.0f) {
        wallFriction->zero();
    } else {
        wallFriction->set(vecFriction[0], vecFriction[1], vecFriction[2]);

        f32 vecLength = GetKartCtrl()->VectorLength(wallFriction);
        f32 frictionLimit = frictionValue * frictionValue;

        if (vecLength > frictionLimit) {
            if (vecLength > 0.0f) {
                wallFriction->scale(GetKartCtrl()->SpeedySqrtf(frictionLimit / vecLength));
            } else {
                wallFriction->zero();
            }
        }
    }
}

void KartBody::GroundReflection(JGeometry::TVec3f *inVecOne, JGeometry::TVec3f *inVecTwo, f32 f1, f32 f2, f32 f3) { 
    JGeometry::TVec3f vec0x5c;   // 0x5C
    JGeometry::TVec3f vec0x50;   // 0x50
    JGeometry::TVec3f vec0x44;   // 0x44
    JGeometry::TVec3f vec0x38;   // 0x38
    JGeometry::TVec3f vec0x2c;   // 0x2c
    JGeometry::TVec3f vec0x20;   // 0x20
    JGeometry::TVec3f vec0x14;   // 0x14
    JGeometry::TVec3f vec0x08;   // 0x08

    // For some reason, moving `_424` and `_3a4` to separate variables corrects order...
    f32 fVar4 = _424;
    f32 fVar5 = _3a4;
    f32 fVar7 = fVar4 * fVar5;
    f32 fVar6 = ((1.0f - f1) * _428 * fVar5);

    GetKartCtrl()->DevMatrixByVector(&vec0x38, inVecTwo, mPlayerPosMtx);
    GetKartCtrl()->DevCrdMatrixByVector(&vec0x14, inVecOne, mPlayerPosMtx);
    GetKartCtrl()->DevMatrixByVector(&vec0x50, &mVel, mPlayerPosMtx);

    vec0x44.cross(mWg, vec0x14);
    vec0x08.set(vec0x50.x + vec0x44.x,
                vec0x50.y + vec0x44.y,
                vec0x50.z + vec0x44.z);
    f32 fVar3 = vec0x08.dot(vec0x38);
    f32 fVar1 = fVar7 * f3 - (fVar6 * fVar3);
    if (fVar1 < 0.0f) {
        return;
    }

    f32 yScale = fVar1 / _3a4;
    if (((mCarStatus & 0x100000) == 0) && (yScale > 65.0f)) {
        yScale = 65.0f;
    }
    yScale = (yScale * _3a4);
    vec0x50.set(vec0x38.x * fVar1, vec0x38.y * yScale, vec0x38.z * fVar1); 
    GetKartCtrl()->MulMatrixByVector(&vec0x5c, &vec0x50, mPlayerPosMtx);
    DoForce(inVecOne, &vec0x5c);
    if (f2 > 0.0f) {
        GetKartCtrl()->SetVector(&vec0x2c,
            vec0x08.x - fVar3 * vec0x38.x,
            vec0x08.y - fVar3 * vec0x38.y,
            vec0x08.z - fVar3 * vec0x38.z
        );
        WallFriction(&vec0x5c, _3a4, &_230, &vec0x14, &vec0x2c, f2 * fVar1);
        GetKartCtrl()->OuterCalc(&vec0x20, &vec0x14, &vec0x5c);
        GetKartCtrl()->MulMatrixByVector(&vec0x50, &vec0x5c, mPlayerPosMtx);

        GetKartCtrl()->AddVector(&mVel, vec0x50.x / _3a4, vec0x50.y / _3a4, vec0x50.z / _3a4);
        mVel.scale(1.01f);
        GetKartCtrl()->AddVector(&mWg, vec0x20.x / _230.x, vec0x20.y / _230.y, vec0x20.z / _230.z); 
        mWg.scale(1.01f);
        getStrat()->DoVelCrl((_3f0 / 2.16f) / mSpeedScale);
    }
    GetKartCtrl()->MulMatrixByVector(&_2c0, &mWg, mPlayerPosMtx);
}

void KartBody::WallFallReflection(JGeometry::TVec3f *v1, JGeometry::TVec3f *v2) {
    // UNUSED, but seems to be the candidate function that first made use of 15.0f
    // entirely fabricated, probably not close to the original at all
    JGeometry::TVec3f wallNormalVec;

    _2a8.zero();
    f32 angle = v1->dot(*v2);

    WallReflection(&wallNormalVec, _40c, _408, angle);
    if ((angle > 15.0f) && ((mGameStatus & 0x200) == 0)) {
        JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown5, mMynum, *v1, 2);
    }
}

void KartBody::RigidReflectionAnime(KartBody *kartBodyOne, KartBody *kartBodyTwo, f32 f3, f32 f4) {
    JGeometry::TVec3f unknownCross;
    JGeometry::TVec3f kartsRelativePos;
    
    if ((mGameStatus & 0x20) != 0) {
        return;
    }
    kartsRelativePos.sub(kartBodyOne->mPos, kartBodyTwo->mPos);
    kartsRelativePos.normalize();

    unknownCross.cross(kartBodyTwo->_308, kartsRelativePos);
    if ((getChecker()->CheckOnlyTandemPartsClearKey(mMynum) == 0) && (getGame()->mCountDownDuration == 0)) {
        if (f3 <= f4) {
            if (GetKartCtrl()->getKartAnime(mMynum)->IsAttack(mMynum) == 0) {
                if (unknownCross.y <= 0.0f) {
                    GetKartCtrl()->getKartAnime(kartBodyTwo->mMynum)->mFlags |= 0x10;
                } else {
                    GetKartCtrl()->getKartAnime(kartBodyTwo->mMynum)->mFlags |= 0x20;
                }
                GetKartCtrl()->getKartSound(kartBodyTwo->mMynum)->DoStrikeVoice();
            }
        } else {
            if (GetKartCtrl()->getKartAnime(mMynum)->IsDangerPlusAnime(mMynum) == 0) {
                if (unknownCross.y > 0.0f) {
                    GetKartCtrl()->getKartAnime(kartBodyTwo->mMynum)->mFlags |= 0x4000000;
                } else {
                    GetKartCtrl()->getKartAnime(kartBodyTwo->mMynum)->mFlags |= 0x2000000;
                }
                GetKartCtrl()->getKartSound(kartBodyTwo->mMynum)->DoStaggerVoice();
            }
        }
    }
}

bool KartBody::RigidReflectionPower(KartBody *kartBodyOne, KartBody *kartBodyTwo, f32 *f3, f32 *f4) {
    if (((kartBodyOne->mCarStatus & 0x40000) == 0) && ((kartBodyTwo->mCarStatus & 0x40000) != 0)) {
        kartBodyTwo->getStrat()->DoPowerMotor(1.0f, 5, 0);
        return true;
    }
    else if (((kartBodyOne->mCarStatus & 0x44004) == 0) && ((kartBodyTwo->mCarStatus & 0x4004) != 0)) {
        kartBodyTwo->getStrat()->DoPowerMotor(1.0f, 5, 0);
        return true;
    }
    else if (((kartBodyOne->getThunder()->mFlags & 1) != 0) && ((kartBodyTwo->getThunder()->mFlags & 1) == 0)) {
        kartBodyTwo->getStrat()->DoPowerMotor(1.0f, 5, 0);
        return true;
    }
    else if (((kartBodyOne->getDamage()->mFlags & 0x170) != 0) && ((kartBodyTwo->getDamage()->mFlags & 0x170) == 0)) {
        kartBodyTwo->getStrat()->DoPowerMotor(1.0f, 5, 0);
        return true;
    }
    else if (((kartBodyOne->mCarStatus & 0x100000) != 0) && ((kartBodyTwo->mCarStatus & 0x100000) == 0)) {
        kartBodyTwo->getStrat()->DoPowerMotor(1.0f, 5, 0);
        return true;
    }
    
    *f3 = kartBodyOne->_420 / kartBodyTwo->_420;
    if (*f3 <= 1.0f) {
        *f3 = 1.0f;
    } else {
        *f3 *= 1.5f;
    }

    *f4 = kartBodyTwo->_420 / kartBodyOne->_420;
    if (*f4 <= 1.0f) {
        *f4 = 1.0f;
    } else {
        *f4 *= 1.5f;
    }

    if ((kartBodyOne->mCarStatus & 0x40000) != 0 && (kartBodyTwo->mCarStatus & 0x40000) == 0) {
        *f3 = 3.0f;
        *f4 = 1.0f;
        kartBodyTwo->_590 |= 0x10;
        kartBodyTwo->mUnkSub10c = kartBodyOne;  // Interesting...
        getStrat()->DoMotor(MotorManager::MotorType_2);
        if ((kartBodyOne->mCarStatus & 0x10) == 0) {
            GetKartCtrl()->getKartSound(kartBodyOne->mMynum)->DoKartsetSeSound(0x1005a);
        }
        return false;
    }

    if (((kartBodyOne->mCarStatus & 0x4000) != 0) && ((kartBodyTwo->mCarStatus & 0x4000) == 0)) {
        *f3 = 3.0f;
        *f4 = 1.0f;
        kartBodyTwo->getDamage()->mFlags |= 2;
        kartBodyTwo->mUnkSub10c = kartBodyOne;
        kartBodyTwo->getTumble()->MakeDashTumble();
        return false;
    }

    if (((kartBodyOne->getThunder()->mFlags & 1) == 0) &&
        ((kartBodyTwo->getThunder()->mFlags & 1) != 0)) {
        *f3 = 4.0f;
        *f4 = 1.0f;
        kartBodyTwo->getCrash()->DoRabbitMark();
        return false;
    }

    if (((kartBodyOne->mCarStatus & 4) != 0) && ((kartBodyTwo->mCarStatus & 4) == 0)) {
        *f3 = 3.0f;
        *f4 = 1.0f;
        kartBodyTwo->getDamage()->mFlags |= 4;
        return false;
    }

    if (((kartBodyOne->mCarStatus & 0x4004) != 0) && ((kartBodyTwo->mCarStatus & 0x4004) != 0)) {
        *f3 = 2.0f;
        *f4 = 2.0f;
        return false;
    }

    if (((kartBodyOne->getDamage()->mFlags & 0x170) == 0) &&
        ((kartBodyTwo->getDamage()->mFlags & 0x170) != 0)) {
        *f3 = 2.0f;
        *f4 = 1.0f;
        return false;
    }

    if (((kartBodyOne->mCarStatus & 0x100000) == 0) &&
        ((kartBodyTwo->mCarStatus & 0x100000) != 0)) {
        *f3 = 2.0f;
        *f4 = 1.0f;
        return false;
    }
    return false;
}

void KartBody::RigidReflection(KartBody *kartBodyOne, KartBody *kartBodyTwo, JGeometry::TVec3f *inVecOne,
          JGeometry::TVec3f *inVecTwo, f32 f1, f32 f2, f32 f3)
{
    JGeometry::TVec3f vec0x58;
    JGeometry::TVec3f vec0x4c;
    JGeometry::TVec3f vec0x40;
    JGeometry::TVec3f vec0x34;
    JGeometry::TVec3f vec0x28;
    JGeometry::TVec3f vec0x1c;
    JGeometry::TVec3f vec0x10;
    f32 fVar7;
    f32 fVar6;
    f32 fVar5;
    f32 fVar4;
    f32 fVar3 = f3;
    f32 fVar2 = f1;
    f32 fVar1;

    if (RigidReflectionPower(kartBodyOne, kartBodyTwo, &fVar7, &fVar6)) {
        return;
    }

    vec0x40.set(kartBodyTwo->mPrevPos);
    vec0x4c.set(*inVecOne);
    vec0x4c.y = vec0x40.y;

    kartBodyTwo->_284.sub(vec0x40, vec0x4c);
    kartBodyTwo->_284.normalize();
    kartBodyTwo->_284.scale(fVar3 / 2.0f);

    if ((kartBodyTwo->mCarStatus & 0x400) != 0) {
        kartBodyTwo->_284.zero();
    }

    fVar1 = fVar7 * (kartBodyOne->_3a4 + kartBodyTwo->_3a4);
    fVar5 = kartBodyOne->_410 * fVar1;
    fVar4 = (1.0f - fVar2) * kartBodyOne->_414 * fVar1;

    vec0x34.sub(*inVecOne, kartBodyTwo->mPrevPos);
    vec0x28.sub(*inVecOne, kartBodyOne->mPrevPos);
    vec0x10.zero();

    vec0x4c.cross(vec0x10, vec0x34);
    vec0x4c.add(kartBodyTwo->mVel);

    vec0x40.cross(vec0x10, vec0x28);
    vec0x40.add(kartBodyOne->mVel);

    vec0x1c.sub(vec0x4c, vec0x40);

    fVar2 = (kartBodyTwo->_308.angle(vec0x34) * 180.0f) / 3.141f;

    if (((kartBodyOne->mCarStatus & 0x100000) == 0) &&
        ((kartBodyTwo->mCarStatus & 0x100000) == 0) && fVar6 < fVar7 &&
        (fVar2 > 45.0f && fVar2 < 135.0f)) {

        fVar2 = (fVar7 / fVar6) * 130.0f;
        if (fVar2 > 300.0f) {
            fVar2 = 300.0f;
        }
        fVar1 = fVar2 * _3a4;
        vec0x58.set(inVecTwo->x * fVar1,inVecTwo->y * fVar1 * 0.1f, inVecTwo->z * fVar1);
        DoForceOnly(kartBodyTwo, inVecOne, &vec0x58);

        if ((kartBodyTwo->mCarStatus & 0x10) == 0) {
            if (fVar7 == 1.0f) {
                kartBodyTwo->getStrat()->DoPowerMotor(1.0f, 5, 0);
            } else {
                kartBodyTwo->getStrat()->DoPowerMotor(1.0f, 12, 0);
            }
        }
        RigidReflectionAnime(kartBodyOne, kartBodyTwo, fVar7, fVar6);
    } else {
        if ((kartBodyTwo->mCarStatus & 0x10) == 0) {
            kartBodyTwo->getStrat()->DoPowerMotor(1.0f, 5, 0);
        }
        if (((kartBodyOne->mCarStatus & 0x100000) == 0) &&
            ((kartBodyTwo->mCarStatus & 0x100000) == 0) &&
            fVar6 >= fVar7 && fVar2 > 45.0f && fVar2 < 135.0f) {
            RigidReflectionAnime(kartBodyOne, kartBodyTwo, fVar7, fVar6);
        }

        fVar2 = vec0x1c.dot(*inVecTwo);
        fVar2 = (fVar5 * fVar3 - (fVar4 * fVar2)) / _3a4;

        if (fVar2 < 0.0f) {
            return;
        }
        
        if (fVar2 < 3.0f) {
            fVar2 = 3.0f;
        }

        u16 uVar1 = (u16)(fVar2 / 3.0f);
        uVar1 /= 0xc;
        if (uVar1 > 0xc) {
            uVar1 = 0xc;
        }
        if ((kartBodyTwo->mCarStatus & 0x10) == 0) {
            kartBodyTwo->getStrat()->DoPowerMotor(1.0f, uVar1, 0);
        }
        fVar2 *= _3a4;
        vec0x58.set(inVecTwo->x * fVar2, inVecTwo->y * fVar2 * 0.1f, inVecTwo->z * fVar2);
        DoForceOnly(kartBodyTwo, inVecOne, &vec0x58);
    }
    return;
}

void KartBody::ObjectReflection(JGeometry::TVec3f *vecObj) {
    
    _29c.x = vecObj->x;
    _29c.y = vecObj->y;
    _29c.y = 0.0f;
    _29c.z = vecObj->z;
    
    if ((mCarStatus & 0x200100000) == 0) {
        f32 objLen = vecObj->length();
        if (objLen > 20.0f) {
            JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown6, mMynum, mPos, 0);
            if (objLen > 40.0f) {
                getStrat()->DoPowerMotor(1.0f, 10, 0);
            } else if (objLen > 30.0f) {
                getStrat()->DoPowerMotor(1.0f, 8, 0);
            } else if (objLen > 20.0f) {
                getStrat()->DoPowerMotor(1.0f, 5, 0);
            }
        } else if (objLen >= 10.0f) {
            getStrat()->DoPowerMotor(1.0f, 3, 0);
        }
        
        JGeometry::TVec3f objReflection;
        objReflection.set(*vecObj);
        objReflection.x = objReflection.x / mSpeedScale;
        objReflection.y = objReflection.y / mSpeedScale;
        objReflection.z = objReflection.z / mSpeedScale;
        objReflection.scale(2.0f);
        objReflection.scale(_3a4);
        DoForce(&mPos, &objReflection);
    }
}

void KartBody::ObjectWallReflection(GeographyObj* geoObj) {
    JGeometry::TVec3f geoPosition;
    JGeometry::TVec3f reflectionVector;
    JGeometry::TVec3f unusedVector;
    int myNum = mMynum;

    
    geoObj->getPosition(&geoPosition);
    geoObj->getColScaleRadius();

    f32 hitDepth = GetGeoObjMgr()->getKartHitDepthNormalObj(myNum);
    reflectionVector.set(*GetGeoObjMgr()->getKartHitRefVecNormalObj(myNum));
    
    JGeometry::TVec3f directionVector;
    geoPosition.y = mPos.y;
    directionVector.sub(geoPosition, mPos);
    directionVector.normalize();

    if (_308.angle(directionVector) <= 0.523333f) {
        mCarStatus |= 0x8000000;
    } else {
        mCarStatus &= 0xfffffffff7ffffff;
    }

    ObjectReflection(&reflectionVector);

    _29c.x = reflectionVector.x;
    _29c.y = 0.0f;
    _29c.z = reflectionVector.z;

    mCarStatus |= 0x4000000;

    if ((mCarStatus & 0x200100000) == 0) {
        if (hitDepth > 3.0f) {
            if (GetKartCtrl()->getKartSound(myNum)->mWallSoundActive == false) {
                getStrat()->DoMotor((MotorManager::MotorType)geoObj->getMotorType());
                GetKartCtrl()->getKartSound(myNum)->DoWallObjectSound(geoObj->getSoundID());
            }
        }
        reflectionVector.length();
        JGeometry::TVec3f scaledVec;

        scaledVec.set(reflectionVector);
        scaledVec.x /= mSpeedScale;
        scaledVec.y /= mSpeedScale;
        scaledVec.z /= mSpeedScale;

        scaledVec.scale(2.0f);
        scaledVec.scale(_3a4);
        DoForce(&mPos, &scaledVec);
        mCarStatus |= 0x4000000;
    }
}

bool KartBody::StarReact(GeographyObj *geoObj) {
    if ((mCarStatus & 0x40000) == 0) {
        return false;
    }

    u8 myNum = mMynum;
    
    KartCtrl *kartCtrl = GetKartCtrl();
    if (kartCtrl->getKartSound(myNum)->mWallSoundActive == true) {
        return true;
    }

    GetKartCtrl()->getKartSound(myNum)->mWallSoundActive = true;
    getStrat()->DoMotor((MotorManager::MotorType)geoObj->getMotorType());
    GetKartCtrl()->getKartSound(myNum)->DoKartsetSeSound(0x1005a);
    JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown12, mMynum, mPos, 0);
    return true;
}

void KartBody::CheckObjectReflection() {
    GeographyObj **geographyObjects;
    GeographyObj *geoObj;
    u8 myNum = mMynum;
    
    mCarStatus &= 0xfffffffff3ffffff;
    if (getChecker()->CheckSpecialDyn()) {
        return;
    }

    geographyObjects = GetGeoObjMgr()->getKartReactHitObjectList(myNum);

    // It's either variable assignments like this in if statements,
    // or nested if statements with too many braces... I hate nesting,
    // so I chose the "lesser evil" to make things more readable.
    if ((geoObj = geographyObjects[10]) != nullptr) {
        getDossin()->DoDossin();
    } else if ((geoObj = geographyObjects[9]) != nullptr) {
        if (!StarReact(geoObj)) {
            getCrash()->MakeBombCrash(nullptr);
        }
    } else if ((geoObj = geographyObjects[14]) != nullptr) {
        if (!StarReact(geoObj)) {
            getTumble()->MakePoiHanaTumble();
        }
    } else if ((geoObj = geographyObjects[19]) != nullptr) {
        if (!StarReact(geoObj)) {
            getCrash()->MakeFreezeCrash();
        }
    } else if ((geoObj = geographyObjects[11]) != nullptr) {
        if (!StarReact(geoObj)) {
            GetKartCtrl()->getKartSound(myNum)->DoWallObjectSound(geoObj->getSoundID());
            if (GetKartCtrl()->IsWallReact(myNum)) {
                ObjectWallReflection(geoObj);
            } else {
                getTumble()->DoTumble();
            }
        }
    } else if ((geoObj = geographyObjects[18]) != nullptr) {
        if (!StarReact(geoObj)) {
            GetKartCtrl()->getKartSound(myNum)->DoWallObjectSound(geoObj->getSoundID());
            if (GetKartCtrl()->IsWallReact(myNum)) {
                ObjectWallReflection(geoObj);
            } else {
                getTumble()->DoPakunTumble();
            }
        }
    } else if ((geoObj = geographyObjects[15]) != nullptr) {
        if (!StarReact(geoObj)) {
            GetKartCtrl()->getKartSound(myNum)->DoWallObjectSound(geoObj->getSoundID());
            if (GetKartCtrl()->IsWallReact(myNum)) {
                ObjectWallReflection(geoObj);
            } else {
                getTumble()->DoHanaTumble();
            }
        }
    } else if ((geoObj = geographyObjects[13]) != nullptr) {
        if (!StarReact(geoObj)) {
            getCrash()->MakeBurn(nullptr);
        }
    } else if ((geoObj = geographyObjects[4]) != nullptr) {
        if (!StarReact(geoObj)) {
            GetKartCtrl()->getKartSound(myNum)->DoWallObjectSound(geoObj->getSoundID());
            if (GetKartCtrl()->GetCarSpeed(myNum) <= 80.0f || GetKartCtrl()->IsWallReact(myNum)) {
                ObjectWallReflection(geoObj);
            } else {
                getCrash()->MakeSpin(nullptr);
            }
        }
    } else if ((geoObj = geographyObjects[3]) != nullptr) {
        if (!StarReact(geoObj)) {
            GetKartCtrl()->getKartSound(myNum)->DoWallObjectSound(geoObj->getSoundID());
            if (GetKartCtrl()->GetCarSpeed(myNum) <= 80.0f || GetKartCtrl()->IsWallReact(myNum)) {
                ObjectWallReflection(geoObj);
            } else {
                getCrash()->MakeHalfSpin(nullptr);
            }
        }
    }

    if (geographyObjects[15] == nullptr && geographyObjects[1] == nullptr) {
        if (geoObj == nullptr) {
            GetKartCtrl()->getKartSound(myNum)->mWallSoundActive = false;
        }
    } else {
        geoObj = (geographyObjects[15] != nullptr)
            ? geographyObjects[15] 
            : geographyObjects[1];
        ObjectWallReflection(geoObj);
    }
    return;
}

void KartBody::CheckRoofReflection() {
    JGeometry::TVec3f currentPlayerPos;
    JGeometry::TVec3f unknownVec;

    currentPlayerPos.set(
        mPlayerPosMtx[0][3], 
        mPlayerPosMtx[1][3], 
        mPlayerPosMtx[2][3]
    );

    mRoofArea.searchRoof(currentPlayerPos);
    if (!mRoofArea.isInside()) {
        return;
    }

    // Why use a case statement on a Boolean?!
    switch ((u8)getChecker()->CheckSpecialDyn()) {
        case 0:
            break;
        default:
            return;
    }

    mRoofArea.getRoofPosition(&currentPlayerPos);
    mRoofArea.getRoofPosition(&unknownVec);
    if (!(mPlayerPosMtx[1][3] - (currentPlayerPos.y - 180.0f) <= 0.0f)) {
        if (mVel.y < 0.0f) {
            return;
        }
        mVel.y = -3.0f;
        _2cc.zero();
    }
}

void KartBody::WallReflection(JGeometry::TVec3f *inVec, f32 f2, f32 f3, f32 f4) {
    JGeometry::TVec3f unknownVec;
    unknownVec.x = inVec->x * f4;
    unknownVec.y = inVec->y * f4;
    unknownVec.z = inVec->z * f4;
    _2a8.set(unknownVec);

    JGeometry::TVec3f vec0x50;  // 0x50
    JGeometry::TVec3f vec0x44;  // 0x44
    JGeometry::TVec3f vec0x38;  // 0x38
    JGeometry::TVec3f vec0x2c;  // 0x2c
    JGeometry::TVec3f vec0x20;  // 0x20
    JGeometry::TVec3f vec0x14;  // 0x14
    JGeometry::TVec3f vec0x08;  // 0x08

    f32 dVar3 = _3a4;
    f32 dVar4 = _400 * dVar3;
    f32 dVar5 = (1.0f - f2) * _404 * dVar3;

    GetKartCtrl()->DevMatrixByVector(&vec0x38, inVec, mPlayerPosMtx);
    GetKartCtrl()->DevMatrixByVector(&vec0x50, &mVel, mPlayerPosMtx);

    vec0x08.set(vec0x50.x, vec0x50.y, vec0x50.z);

    f32 fVar1 = (dVar4 * f4 - dVar5 * vec0x08.dot(vec0x38));
    if (fVar1 < 0.0f) {
        return;
    }

    vec0x50.set(vec0x38.x * fVar1, vec0x38.y * fVar1, vec0x38.z * fVar1);
    GetKartCtrl()->MulMatrixByVector(&unknownVec, &vec0x50, mPlayerPosMtx);
    DoForce(&mPos, &unknownVec);
}

void KartBody::GroundBoundReflection(CrsGround crsGround, JGeometry::TVec3f *wallVertex) {
    // UNUSED
}

void KartBody::GroundVertexReflection(int index, CrsGround crsGround, JGeometry::TVec3f *inVec, u8 *someFlag){
    JGeometry::TVec3f crsNormal;
    
    f32 crsHeight = crsGround.getHeight();
    crsGround.getNormal(&crsNormal);

    f32 relativeHeight = crsHeight - inVec->y;
    if (relativeHeight >= 0.0f) {
        if ((mCarStatus & 0x8000) != 0) {
            GroundReflection(inVec, &crsNormal, _430, 0.01f, relativeHeight);
            *someFlag = 1;
        } else if ((mCarStatus & 0x100000) == 0) {
            GroundReflection(inVec, &crsNormal, _430, 0.1f, relativeHeight);
        } else {
            GroundReflection(inVec, &crsNormal, _430, _42c, relativeHeight);
        }
    }
    _1d0[index].set(*inVec);
    return;
}

void KartBody::WallVertexReflection(CrsGround crsGround, JGeometry::TVec3f *wallVertex, u8 *unknownBool1, u8 *unknownBool2) {
    JGeometry::TVec3f wallNormalVec;

    _2a8.zero();

    f32 wallAngle = crsGround.getWallNormal(&wallNormalVec, nullptr);
    f32 angle = wallAngle;

    if (wallAngle >= 0.0f) {
        *unknownBool2 = 1;
        WallReflection(&wallNormalVec, _40c, _408, angle);
        if ((_458 > 30.0f && angle > 15.0f) && ((mGameStatus & 0x200) == 0)) {
            if (crsGround.getMaterial() != CrsGround::Mat_255) {
                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown5, mMynum, *wallVertex, 2);
            }

            if ((mGameStatus & 2) != 0) {
                wallAngle = (angle / 120.0f);
                if (wallAngle > 1.0f) {
                    wallAngle = 1.0f;
                }
                
                if (getChecker()->CheckPlayer() != 0) {
                    GetKartCtrl()->getKartSound(mMynum)->DoWallHitSound(wallAngle, crsGround.getMaterial());
                }
            }
        }

        angle = _2f0.angle(wallNormalVec);
        if (((mCarStatus & 0x100000) == 0) && (getTouchNum() != 0)) {
            if (_458 > 30.0f) {
                if ((angle > 1.046666f) && (angle < 2.0933299f)) {
                    *unknownBool1 = 1;
                }
            }
            else if ((angle > 0.348888f) && (angle < 2.79111f)) {
                *unknownBool1 = 1;
            }
        }
    }
}

void KartBody::CheckVertexReflection() {
    TireOp *tireOpData;

    JGeometry::TVec3f vec1;
    JGeometry::TVec3f vec2;
    
    CrsGround crsGround(nullptr);
    
    u8 local_156 = 0;
    u8 local_157 = 0;
    u8 local_158 = 0;
    
#line 1946
    JUT_MINMAX_ASSERT(0, mIdx, 22);
    for (int i = 0; i < 8; i++) {
        if (i >= 4) {
            vec2.set(
                TireOpData[mIdx][i - 4]._10 + TireOpData[mIdx][i - 4]._c,
                CubeParamsData[mIdx]->_0[i].y,
                TireOpData[mIdx][i - 4]._14);
        } else {
            vec2.set(TireOpData[mIdx][i]._10 + TireOpData[mIdx][i]._c,
                    CubeParamsData[mIdx]->_0[i].y,
                    TireOpData[mIdx][i]._14);
        }
        PSMTXMultVec(_1a0, &vec2, &vec1);
        crsGround.search(vec1, _1d0[i]);

        switch (crsGround.getAttribute()) {
            case CrsGround::Attr_2:
            case CrsGround::Attr_18:
                break;
            default:
                if (crsGround.getAttribute() != CrsGround::Attr_10) {
                    GroundVertexReflection(i, crsGround, &vec1, &local_158);
                }
                break;
        }
    }
    vec1.set(_1a0[0][3], _1a0[1][3], _1a0[2][3]);
    crsGround.search(vec1, _254, BodyOpData[mIdx]->_4, true);
    if (!getChecker()->CheckSpecialDyn2()) {
        WallVertexReflection(crsGround, &vec1, &local_156, &local_157);
    }

    if (local_157 == 1) {
        mCarStatus |= 0x400;
    } else {
        mCarStatus &= ~0x400;
    }

    if (local_156 == 1) {
        mCarStatus |= 0x20;
    } else {
        mCarStatus &= ~0x20;
    }

    if (local_158 == 1) {
        mCarStatus |= 0x10000000;
    } else {
        mCarStatus &= ~0x10000000;
    }

    if (getChecker()->CheckSpecialDyn() != 0) {
        mCarStatus &= ~0x400;
        mCarStatus &= ~0x20;
        mCarStatus &= ~0x10000000;
    }
    return;
}

bool KartBody::CheckBodyHit(KartBody *kartOne, KartBody *enemy) {
    JGeometry::TVec3f collisionVec;     // 0x44
    JGeometry::TVec3f tvec5;            // 0x38
    JGeometry::TVec3f kartOnePrevPos;   // 0x2c
    JGeometry::TVec3f kartTwoPrevPos;   // 0x20
    JGeometry::TVec3f kartOnePos;       // 0x14
    JGeometry::TVec3f kartTwoPos;       // 0x08
    
    kartOnePrevPos.set(kartOne->mPrevPos);
    kartTwoPrevPos.set(enemy->mPrevPos);

    f32 unknownSum = kartOne->_3a8 + enemy->_3a8;  // MJB - is this the width of both karts?
    f32 kartOverlapPrevPosX = (kartOnePrevPos.x - kartTwoPrevPos.x);

    bool bodyWasHit;
    if (!getChecker()->CheckBodyHit(kartOne->mMynum, enemy->mMynum) == 0) {
        bodyWasHit = false;
    } else {
        if ((kartOverlapPrevPosX > unknownSum) || (kartOverlapPrevPosX < -unknownSum)) {
            bodyWasHit = false;
        } else if ((kartOnePrevPos.y - kartTwoPrevPos.y) > unknownSum || (kartOnePrevPos.y - kartTwoPrevPos.y) < -unknownSum) {
            bodyWasHit = false;
        } else if ((kartOnePrevPos.z - kartTwoPrevPos.z) > unknownSum || (kartOnePrevPos.z - kartTwoPrevPos.z) < -unknownSum) {
            bodyWasHit = false;
        } else {
            collisionVec.sub(kartTwoPrevPos, kartOnePrevPos);
            kartOverlapPrevPosX = GetKartCtrl()->VectorLength(&collisionVec);

            if (kartOverlapPrevPosX <= (unknownSum * unknownSum)) {
                kartOnePos.set(kartOne->mPos);
                kartTwoPos.set(enemy->mPos);

                collisionVec.sub(kartTwoPos, kartOnePos);
                unknownSum -= collisionVec.length();
                collisionVec.normalize();

                tvec5.set(collisionVec.x * kartOne->_3a8 + kartOnePrevPos.x,
                          collisionVec.y * kartOne->_3a8 + kartOnePrevPos.y,
                          collisionVec.z * kartOne->_3a8 + kartOnePrevPos.z);
                
                if (unknownSum > 15.0f) {
                    kartOnePos.set(tvec5);
                    kartTwoPos.set(kartOne->mVel.x, kartOne->mVel.y, kartOne->mVel.z);
                    kartOnePos.add(kartTwoPos);
                    kartOverlapPrevPosX = (unknownSum / 25.0f);
                    if (kartOverlapPrevPosX > 1.0f) {
                        kartOverlapPrevPosX = 1.0f;
                    }
                    
                    if ((kartOne->mGameStatus & 2) != 0 || (enemy->mGameStatus & 2) != 0) {
                        if ((kartOne->mCarStatus & 0x1000000000) == 0) {
                            if ((enemy->mCarStatus & 0x1000000000) == 0) {
                                enemy->mCarStatus |= 0x1000000000;
                                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown4, enemy->mMynum, kartOnePos, 2);
                            }
                        }
                    } else {
                        if ((kartOne->mCarStatus & 0x1000000000) == 0) {
                            if ((enemy->mCarStatus & 0x1000000000) == 0) {
                                enemy->mCarStatus |= 0x1000000000;
                                JPEffectPerformer::setEffect(JPEffectPerformer::Effect_Unknown4, enemy->mMynum, kartOnePos, 2);
                            }
                        }
                    }
                    GetKartCtrl()->getKartSound(enemy->mMynum)->DoKartHitSound(kartOverlapPrevPosX);
                } else {
                    GetKartCtrl()->getKartSound(enemy->mMynum)->DoKartHitSound(0.3f);
                }

                RigidReflection(kartOne, enemy, &tvec5, &collisionVec, kartOne->_41c, kartOne->_418, unknownSum);
                enemy->mCarStatus |= 0x10;
#line 2105
                JUT_MINMAX_ASSERT(0, enemy->mMynum, 8);

                if ((kartOne->mCarStatus & 0x4004) != 0 && (enemy->mCarStatus & 0x65000) == 0) {
                    kartOne->_5b8[enemy->mMynum] = 1;
                } else if ((kartOne->mCarStatus & 0x40000) != 0 && (enemy->mCarStatus & 0x65000) == 0) {
                    kartOne->_5b8[enemy->mMynum] = 1;
                }
                bodyWasHit = true;
            } else {
                bodyWasHit = false;
            }
        }
    }
    return bodyWasHit;
}


void KartBody::InitBodySetting() {
    u8 kartDriver = mDriver;
#line 2137
    JUT_MINMAX_ASSERT(0, mDriver, 2);

    u8 kartClass = mClass;
    // TODO: Verify what gets stored in mSettingPtrs.
    f32 *unknownSettings = (f32 *)mSettingPtrs[mDriver];
#line 2143
    JUT_MINMAX_ASSERT(0, mClass, 3);
    
    _3d0 = unknownSettings[0x18];
    _3d8 = unknownSettings[0x1b];
    _3d4 = unknownSettings[0x19];
    _3dc = unknownSettings[0x1c];
    _3b0 = unknownSettings[0x29];
    _420 = unknownSettings[0x21];
    _47c = unknownSettings[0x2e];
    _480 = unknownSettings[0x2f];
    _484 = unknownSettings[0x2d];
    _3f0 = unknownSettings[0x14] * SetEngineClassData[kartClass];
    _3f4 = unknownSettings[0x15] * SetEngineClassData[kartClass];
    _3f8 = unknownSettings[0x16] * SetEngineClassData[kartClass];
    _3fc = unknownSettings[0x17] * SetEngineClassData[kartClass];
    _4f4 = unknownSettings[0x26];
    _4f8 = unknownSettings[0x28];
    _50c = unknownSettings[0x1a];
    _514 = unknownSettings[0x2b];
    _51c = unknownSettings[0x1d];
    _520 = unknownSettings[0x1e];
    _524 = unknownSettings[0x1f];
    _530 = unknownSettings[0x0c];
    _534 = unknownSettings[0x0d];
    _538 = unknownSettings[0x0e];
    _48c = unknownSettings[0x39];
    mMTBoostMax = 1.0f + unknownSettings[0x20];
    return;
}

void KartBody::GetMiniGameCamData(int index) {
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();

    int driverId = raceMgr->getKartInfo(mMynum)->getDriverCharID(0);
    int passengerId = raceMgr->getKartInfo(mMynum)->getDriverCharID(1);

    if ((driverId == 0x14) || (passengerId == 0x14)) {
        GetPakkunMiniGameCamData(index);
        return;
    }

    if (index == 1) {
        index = 0;
    }
    if (index == 2) {
        index = 1;
    }
    if ((index == 3) || (index == 4)) {
        index = 2;
    }

    switch (raceMgr->getRaceMode()) {
        case 4:
            _4e4 = balloonMinidata[index]._0;
            _4e8 = balloonMinidata[index]._4;
            _350.y = balloonMinidata[index]._8;
            break;
        case 5:
            _4e4 = turtleMinidata[index]._0;
            _4e8 = turtleMinidata[index]._4;
            _350.y = turtleMinidata[index]._8;
            break;
        case 6:
            _4e4 = bombMinidata[index]._0;
            _4e8 = bombMinidata[index]._4;
            _350.y = bombMinidata[index]._8;
            break;
        case 7:
            _4e4 = devilMinidata[index]._0;
            _4e8 = devilMinidata[index]._4;
            _350.y = devilMinidata[index]._8;
            break;
        default:
            _4e4 = balloonMinidata[index]._0;
            _4e8 = balloonMinidata[index]._4;
            _350.y = balloonMinidata[index]._8;
            break;
    }

    return;
}

void KartBody::GetPakkunMiniGameCamData(int index) {
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();

    if (index == 1) {
        index = 0;
    }
    if (index == 2) {
        index = 1;
    }
    if ((index == 3) || (index == 4)) {
        index = 2;
    }

    switch (raceMgr->getRaceMode()) {
        case 4:
            _4e4 = balloonPakkunMinidata[index]._0;
            _4e8 = balloonPakkunMinidata[index]._4;
            _350.y = balloonPakkunMinidata[index]._8;
            break;
        case 6:
            _4e4 = bombPakkunMinidata[index]._0;
            _4e8 = bombPakkunMinidata[index]._4;
            _350.y = bombPakkunMinidata[index]._8;
            break;
        case 7:
            _4e4 = devilPakkunMinidata[index]._0;
            _4e8 = devilPakkunMinidata[index]._4;
            _350.y = devilPakkunMinidata[index]._8;
            break;
        default:
            _4e4 = balloonPakkunMinidata[index]._0;
            _4e8 = balloonPakkunMinidata[index]._4;
            _350.y = balloonPakkunMinidata[index]._8;
            break;
    }
    return;
}

void KartBody::GetPakkunGameCamData(int index) {
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();

    int driverCharId = raceMgr->getKartInfo(mMynum)->getDriverCharID(0);
    int passengerCharId = raceMgr->getKartInfo(mMynum)->getDriverCharID(1);

    f32 unknownFloat1 = 18.0f;
    f32 unknownFloat2 = 22.0f;

    if ((driverCharId == 0x14) || (passengerCharId == 0x14)) {
        if ((index == 1) || (index == 4)) {
            _4e4 = 500.0f;
            unknownFloat1 = -200.0f;
            _4e8 = unknownFloat2;
            _350.y = unknownFloat1;
        } else if (index == 2) {
            _4e4 = 580.0f;
            unknownFloat2 = -200.0f;
            _4e8 = unknownFloat1;
            _350.y = unknownFloat2;
        }
    }
}


void KartBody::InitCamSetting() {
    KartCtrl *kartCtrl = GetKartCtrl();

    // MJB - Not sure why the cast to `f32 *` is happening...
    //       more info around mSettingPtrs needed.
    // TODO: Verify what gets stored in mSettingPtrs.
    f32 *unknownSettings = (f32 *)mSettingPtrs[mDriver];
    
    bool isMiniGame = false;
    int cameraNumber = RaceMgr::getCurrentManager()->getCameraNumber();

    if (kartCtrl->IsMiniGame()) {
        isMiniGame = true;
    }

    if (RaceMgr::getCurrentManager()->isSubScrExist() != false) {
        _4e4 = unknownSettings[0];
        _4e8 = unknownSettings[1];
        _350.y = unknownSettings[2];
    } else {
        if (cameraNumber == 1) {
            _4e4 = unknownSettings[0];
            _4e8 = unknownSettings[1];
            _350.y = unknownSettings[2];
            if (isMiniGame) {
                GetMiniGameCamData(1);
            } else {
                GetPakkunGameCamData(1);
            }
        } else if (cameraNumber == 2) {
            _4e4 = unknownSettings[3];
            _4e8 = unknownSettings[4];
            _350.y = unknownSettings[5];
            if (isMiniGame) {
                GetMiniGameCamData(2);
            } else {
                GetPakkunGameCamData(2);
            }
        } else if ((cameraNumber == 3) || (cameraNumber == 4)) {
            _4e4 = unknownSettings[6];
            _4e8 = unknownSettings[7];
            _350.y = unknownSettings[8];
            if (isMiniGame) {
                GetMiniGameCamData(4);
            } else {
                GetPakkunGameCamData(4);
            }
        } else {
            _4e4 = unknownSettings[0];
            _4e8 = unknownSettings[1];
            _350.y = unknownSettings[2];
            if (isMiniGame) {
                GetMiniGameCamData(1);
            }
        }
    }
    _4ec = unknownSettings[9];
    _4f0 = unknownSettings[10];
}

void KartBody::InitBodyCamParam() {
    InitCamSetting();
}

void KartBody::InitBodyParam() {
    KartCtrl *kartCtrl = GetKartCtrl();
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();
    mClass = 1;
    switch (raceMgr->getRaceLevel()) {
        case LVL_50CC:
            mClass = 0;
            break;
        case LVL_100CC:
            mClass = 1;
            break;
        case LVL_150CC:
            mClass = 2;
            break;
        case LVL_MIRROR:
            mClass = 2;
            break;
        default:
            break;
    }

    if (raceMgr->isAwardDemoMode() != 0) {
        mClass = 1;
    }

    CharIDs[0] = raceMgr->getKartInfo(mMynum)->getDriverCharID(0);
    CharIDs[1] = raceMgr->getKartInfo(mMynum)->getDriverCharID(1);

    raceMgr->getKartInfo(mMynum)->getDriverWeight(0);   // Driver.
    raceMgr->getKartInfo(mMynum)->getDriverWeight(1);   // Passenger.
    raceMgr->getKartInfo(mMynum)->getWeight();

    u8 index = mIdx;
    if (kartCtrl->IsMiniGame() != false) {
        index &= 0xff;
        mSettingPtrs[0] = (u8 *)ResMgr::getPtr(ResMgr::mcArcMRAM, settingMiniPtr2[index][0]);
        mSettingPtrs[1] = (u8 *)ResMgr::getPtr(ResMgr::mcArcMRAM, settingMiniPtr2[index][1]);
    } else {
        index &= 0xff;
        mSettingPtrs[0] = (u8 *)ResMgr::getPtr(ResMgr::mcArcMRAM, settingPtr2[index][0]);
        mSettingPtrs[1] = (u8 *)ResMgr::getPtr(ResMgr::mcArcMRAM, settingPtr2[index][1]);
    }

    _32c.set(0.0f, 1.0f, 0.0f);
    _368.set(0.0f, 1.0f, 0.0f);
    _54c = 0.02f * mMynum;
    _550 = 0.0174444f * mMynum;
    _384 = 10.0f;
    _38c = 10.0f;
    _5b0 = 0;
    _5b1 = 0;
    InitBodySetting();
    _3e0 = TireParamsData[mIdx]->_0;
    _3e4 = TireParamsData[mIdx]->_4;
    mSpeedScale = 0.313f;
    _474 = 0.313f;
    _320.set(BodyParamsData[mIdx]->_1c, BodyParamsData[mIdx]->_20, BodyParamsData[mIdx]->_24);
    MakeRigidVertexBody(BodyParamsData[mIdx]->_10, _320.x, _320.y, _320.z);
    _3a0 = -BodyParamsData[mIdx]->_18;
    _3b4 = BodyParamsData[mIdx]->_6c;
    _3b8 = BodyParamsData[mIdx]->_70;
    _3bc = BodyParamsData[mIdx]->_74;
    _3c0 = BodyParamsData[mIdx]->_78;
    _3c4 = BodyParamsData[mIdx]->_7c;
    _3ac = BodyParamsData[mIdx]->_10;
    _3a8 = BodyOpData[mIdx]->_0;
    _400 = BodyParamsData[mIdx]->_0;
    _404 = BodyParamsData[mIdx]->_4;
    _408 = BodyParamsData[mIdx]->_8;
    _40c = BodyParamsData[mIdx]->_c;
    _410 = BodyParamsData[mIdx]->_28;
    _414 = BodyParamsData[mIdx]->_2c;
    _418 = BodyParamsData[mIdx]->_30;
    _41c = BodyParamsData[mIdx]->_34;
    _424 = BodyParamsData[mIdx]->_38;
    _428 = BodyParamsData[mIdx]->_3c;
    _42c = BodyParamsData[mIdx]->_40;
    _430 = BodyParamsData[mIdx]->_44;
    _434 = BodyParamsData[mIdx]->_48;
    _438 = BodyParamsData[mIdx]->_4c;
    _43c = BodyParamsData[mIdx]->_50;
    _440 = BodyParamsData[mIdx]->_54;
    _498 = TireParamsData[mIdx]->_c;
    _49c = TireParamsData[mIdx]->_10;
    _4a0 = TireParamsData[mIdx]->_14; 
    _4dc = BodyParamsData[mIdx]->_58;
    _344.x = BodyParamsData[mIdx]->_5c;
    _344.y = BodyParamsData[mIdx]->_60;
    _344.z = BodyParamsData[mIdx]->_64;
    mYawSterr = BodyParamsData[mIdx]->_88;
    mBtnMinusLiftSterr = BodyParamsData[mIdx]->_8c;
    _508 = BodyParamsData[mIdx]->_98;
    _540 = BodyParamsData[mIdx]->_9c;

    for (int i = 0; i < 8; i++) {
        PSMTXMultVec(_110, &CubeParamsData[mIdx][0]._0[i], &_1d0[i]);
    }
}

void KartBody::Init(int index) {
    Mtx mtxPose = {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    };
    
    mFrame = 0.0f;
    _384 = 0.0f;
    _388 = 0.0f;
    _38c = 0.0f;
    mLiftframe = 0.0f;
    _394 = 0.0f;
    _398 = 0.0f;
    _39c = 0.0f;
    _3a0 = 0.0f;
    _3a4 = 0.0f;
    _3a8 = 0.0f;
    _3ac = 0.0f;
    _3b0 = 0.0f;
    _3b4 = 0.0f;
    _3b8 = 0.0f;
    _3bc = 0.0f;
    _3c0 = 0.0f;
    _3c4 = 0.0f;
    _3c8 = 0.0f;
    _3cc = 0.0f;
    _3d0 = 0.0f;
    _3d4 = 0.0f;
    _3d8 = 0.0f;
    _3dc = 0.0f;
    _3e0 = 0.0f;
    _3e4 = 0.0f;
    _3e8 = 0.0f;
    _3ec = 0.0f;
    _3f0 = 0.0f;
    _3f4 = 0.0f;
    _3f8 = 0.0f;
    _3fc = 0.0f;
    _400 = 0.0f;
    _404 = 0.0f;
    _408 = 0.0f;
    _40c = 0.0f;
    _410 = 0.0f;
    _414 = 0.0f;
    _418 = 0.0f;
    _41c = 0.0f;
    _420 = 0.0f;
    _424 = 0.0f;
    _428 = 0.0f;
    _42c = 0.0f;
    _430 = 0.0f;
    _434 = 0.0f;
    _438 = 0.0f;
    _43c = 0.0f;
    _440 = 0.0f;
    mSpeed = 0.0f;
    _448 = 0.0f;
    mCarRPM = 0.0f;
    mKartRPM = 0.0f;
    _454 = 0.0f;
    _458 = 0.0f;
    _45c = 0.0f;
    _460 = 0.0f;
    _464 = 0.0f;
    _468 = 0.0f;
    _46c = 0.0f;
    mSpeedScale = 0.0f;
    _474 = 0.0f;
    mTireAngle = 0.0f;
    _47c = 0.0f;
    _484 = 0.0f;
    _498 = 0.0f;
    _49c = 0.0f;
    _4a0 = 0.0f;
    _4a8 = 0.0f;
    _4ac = 0.0f;
    _4b0 = 0.0f;
    _4b4 = 0.0f;
    _4b8 = 0.0f;
    _4bc = 0.0f;
    _4c0 = 0.0f;
    _4c4 = 0.0f;
    _4c8 = 0.0f;
    _4cc = 0.0f;
    _4d4 = 0.0f;
    _4d8 = 0.0f;
    _4dc = 0.0f;
    _4e0 = 0.0f;
    _4e4 = 0.0f;
    _4e8 = 0.0f;
    _4ec = 0.0f;
    _4f0 = 0.0f;
    _52c = 0.0f;
    _4f4 = 0.0f;
    _4f8 = 0.0f;
    mBtnMinusLiftSterr = 0.0f;
    _504 = 0.0f;
    mYawSterr = 0.0f;
    _50c = 0.0f;
    _508 = 0.0f;
    _540 = 0.0f;
    _514 = 0.0f;
    _518 = 0.0f;
    _51c = 0.0f;
    _520 = 0.0f;
    _524 = 0.0f;
    _528 = 0.0f;
    _530 = 0.0f;
    _534 = 0.0f;
    _538 = 0.0f;
    _53c = 0.0f;
    _4a4 = 0.0f;
    _4d0 = 0.0f;
    _544 = 0.0f;
    _548 = 0.0f;
    _554 = 0.0f;
    _558 = 0.0f;
    _54c = 0.0f;
    _550 = 0.0f;
    mSterrNorm = 0.0f;
    _55c = 0.0f;
    _48c = 0.0f;
    _560 = 0.0f;
    _564 = 0.0f;
    _568 = 1.0f;
    mSterrFrame = 0.0f;
    mSterrFrameChase = 0.0f;
    _510 = 0.0f;
    _230.zero();
    for (int i = 0; i < 8; i++) {
        _1d0[i].zero();
    }
    mPos.zero();
    mPrevPos.zero();
    _254.zero();
    mVel.zero();
    _284.zero();
    _290.zero();
    _2b4.zero();
    _2c0.zero();
    _2cc.zero();
    _2d8.zero();
    mWg.zero();
    _2f0.zero();
    _2fc.zero();
    _308.zero();
    _320.zero();
    _32c.zero();
    _338.zero();
    _344.zero();
    _350.zero();
    _368.zero();
    mTireG.zero();
    _314.zero();
    _29c.zero();
    _2a8.zero();
    _35c.zero();
    _374.zero();
    mEffctVel.zero();
    mCarStatus = 0;
    _57c = 0;
    _580 = 0;
    mGameStatus = 0;
    _584 = 0;
    _588 = 0;
    _594 = 0;
    mBoostTimer = 0;
    mTouchNum = 0;
    mIdx = 0;
    _5b0 = 0;
    _5b1 = 0;
    mDriver = 0;
    mSlideTimer = 0;
    mMynum = 0;
    _5c5 = 0;
    _592 = 0;
    _59a = 0;
    _5b4 = 0;
    _5c2 = 0;
    mClass = 0;
    _5b5 = 0;
    _590 = 0;
    _5c3 = 0;
    _59c = 0;
    mMyRank = 0;
    mMTBoost = 0;
    mDriftSterr = 0;
    mMTState = 0;
    _598 = 0;
    mCameraNum = 100;
    for (int i = 0; i < 8; i++) {
        _5b8[i] = 0;
    }
    _5c1 = 0;
    GetKartCtrl()->SetPosePosMatrix(_110, mtxPose, &mPos);
    GetKartCtrl()->SetPosePosMatrix(mPlayerPosMtx, mtxPose, &mPos);
    GetKartCtrl()->SetPosePosMatrix(_170, mtxPose, &mPos);
    GetKartCtrl()->SetPosePosMatrix(_1a0, mtxPose, &mPos);

    JGeometry::TVec3f unknownVec001;
    JGeometry::TVec3f unknownVec002;
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();
    raceMgr->getStartPoint(&unknownVec001, &unknownVec002, index);
    KartDrawer *kartDrawer = raceMgr->getKartDrawer(index);
    mBodyGround.reset();
    mBodyGround.search(unknownVec001, mPos);
    unknownVec001.y = 300.0f + mBodyGround.getHeight();
    if (mBodyGround.isObject() != 0) {
        _58c = mBodyGround.getObject()->getKind();
    } else {
        _58c = 0;
    }
    mIdx = raceMgr->getKartInfo(index)->getKartID();
    mBodyModel = mKartLoader->getExModelBody();
    mExModels[0] = mKartLoader->getExModelDriver(0);
    mExModels[1] = mKartLoader->getExModelDriver(1);
    mDriverModels[0] = (DriverModel *)mExModels[1];
    mDriverModels[1] = (DriverModel *)mExModels[0];
    mShadowModel = mKartLoader->getShadowModel();
    mShadowModel->setDetailFlg();
    mShadowModel->setScale(1.0f, 1.0f);
    mShadowModel->setVisibleAll();
    mCarStatus = 0;
    mMynum = index;
    _584 = 0;
    _588 = 0;

    int playerKind = RaceMgr::getManager()->getKartInfo(index)->getPlayerKind(0);
    mGameStatus |= 0x2000;

    switch (playerKind) {
        case 1:
            mGameStatus |= 0x102;
            if (RaceMgr::getCurrentManager()->isReplayMode() != false) {
                mGameStatus |= 0x80;
            }
            break;

        case 2:
            mGameStatus |= 8;
            break;

        case 4:
            if (RaceMgr::getCurrentManager()->getKartInfo(mMynum)->isAvailableKart() == false) {
                mGameStatus |= 0x112;
                mGameStatus &= 0xFFFFDFFF;
                mShadowModel->clrVisibleAll();
                kartDrawer->hide();
            } else {
                mGameStatus |= 0x2112;
                mShadowModel->setVisibleAll();
            }
            mShadowModel->clrVisibleAll();
            break;

        case 3:
            mGameStatus |= 0x802;
            break;
    }
        
    mGameStatus |= 0x200;
    if ((raceMgr->getKartInfo(index)->isTagMode() != 0) && ((mGameStatus & 8) == 0)) {
        mGameStatus |= 1;
    }

    InitBodyParam();
    SetUpRigidBodyPose(&unknownVec001, &unknownVec002);
    InitBodyCamParam();

    mExModels[0]->setBaseTRMtx(mPlayerPosMtx);
    mExModels[1]->setBaseTRMtx(mPlayerPosMtx);
    
    mKartStrat->Init(index);
    mKartGame->Init(index);
    mKartItem->Init(index);
    mKartRescue->Init(index);
    mKartCannon->Init(index);
    mKartDossin->Init(index);
    mKartTumble->Init(index);
    mKartCheck->Init(index);
    mKartDamage->Init(index);
    mKartStar->Init(index);
    mKartTire->Init(index);
    mKartThunder->Init(index);
    mKartCrash->Init(index);
    mKartHandle->Init(index);
    mKartPipe->Init(index);
    mKartAnt->Init(index);
    mKartPerCam->Init(index);
    mKartScene->mChapter = 1;
    return;
}
