#include "Kaneshige/RaceMgr.h"

#include "Yamamoto/kartCtrl.h"
#include "Yamamoto/KartDisp.h"
#include "Yamamoto/kartParams.h"

#include <std/math.h>

#include "JSystem/JAudio/JASFakeMatch2.h"

#include "mathHelper.h"

// TODO: major cleanup

void KartDisp::Init(int i) { mBody = GetKartCtrl()->getKartBody(i); }

void KartDisp::DoShake()
{
    KartBody *body = mBody;
    f32 carSpd = GetKartCtrl()->GetCarSpeed(body->mMynum);
    f32 carRpm = GetKartCtrl()->GetCarRpm(body->mMynum); // Unused variable
    KartGamePad *driverPad = GetKartCtrl()->GetDriveCont(body->mMynum);

    if (carSpd < 20.0f)
        GetKartCtrl()->ChaseFnumber(&body->_54c, 0.35f, 0.05f);
    else
        GetKartCtrl()->ChaseFnumber(&body->_54c, 0.0f, 0.005f);

    f32 scale = std::sinf(body->_550);
    body->_548 = body->_54c * scale;
    body->_550 += 0.872222f;

    if (body->_550 > 6.18)
        body->_550 = 0.0f;

    if (body->mCarStatus & (0x4000 | 0x800))
    {
        GetKartCtrl()->ChaseFnumber(&body->_374.x, -0.261666f, 0.2f);
    }
    else if (body->mCarStatus & 0x20000)
    {
        GetKartCtrl()->ChaseFnumber(&body->_374.x, -0.1221111f, 0.2f);
    }
    else if (body->mCarStatus & 0x10000)
    {
        GetKartCtrl()->ChaseFnumber(&body->_374.x, -0.261666f, 0.4f);
    }
    else if (driverPad->testButton(KartGamePad::A) && carSpd >= 10.0f && carSpd <= 40.0f)
    {
        GetKartCtrl()->ChaseFnumber(&body->_374.x, -0.174444f, 0.05f);
    }
    else if (body->_5c3 & 1)
    {
        GetKartCtrl()->ChaseFnumber(&body->_374.x, 0.261666f, 0.05f);
    }
    else if (body->_5c3 & 4)
    {
        GetKartCtrl()->ChaseFnumber(&body->_374.x, 0.1395555f, 0.05f);
    }
    else if (body->mCarStatus & 0x20000000000)
    {
        GetKartCtrl()->ChaseFnumber(&body->_374.x, -0.0872222f, 0.5f);
    }
    else
    {
        GetKartCtrl()->ChaseFnumber(&body->_374.x, 0.0f, 0.1f);
    }

    if (body->_374.x > SusDushDive[body->mIdx])
        body->_374.x = SusDushDive[body->mIdx];

    if (body->_374.x < -SusDushDive[body->mIdx])
        body->_374.x = -SusDushDive[body->mIdx];

    if (RaceMgr::getCurrentManager()->isAwardDemoMode())
        body->_374.x = 0.0f;
}

void KartDisp::MakeDispBodyPose()
{
    KartBody *body = mBody;
    KartSus *sus1 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 0);
    KartSus *sus2 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 1);
    KartSus *sus3 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 2);
    KartSus *sus4 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 3);
    DoShake();

    body->_1a0[0][0] = body->mPlayerPosMtx[0][0];
    body->_1a0[1][0] = body->mPlayerPosMtx[1][0];
    body->_1a0[2][0] = body->mPlayerPosMtx[2][0];
    body->_1a0[0][1] = body->mPlayerPosMtx[0][1];
    body->_1a0[1][1] = body->mPlayerPosMtx[1][1];
    body->_1a0[2][1] = body->mPlayerPosMtx[2][1];
    body->_1a0[0][2] = body->mPlayerPosMtx[0][2];
    body->_1a0[1][2] = body->mPlayerPosMtx[1][2];
    body->_1a0[2][2] = body->mPlayerPosMtx[2][2];

    JGeometry::TVec3f v1, v2, v3, v4, v5, v6;

    Mtx m1 = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };
    Mtx m2 = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };

    v1.set(body->_374.x, 0.0f, body->_374.z);
    PSMTXMultVecSR(body->mPlayerPosMtx, (Vec *)&v1, (Vec *)&v2);

    GetKartCtrl()->OmegaMatrix(m1, (Vec *)&v2, 0);
    GetKartCtrl()->MulMatrix(m2, body->_1a0, m1);
    GetKartCtrl()->AddMatrix(body->_1a0, m2);
    GetKartCtrl()->NormalizeMatrix(body->_1a0);
}

void KartDisp::MakeDispBodyPos()
{
    KartBody *body = mBody;
    KartSus *sus1 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 0);
    KartSus *sus2 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 1);
    KartSus *sus3 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 2);
    KartSus *sus4 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 3);

    body->_1a0[0][3] = body->mPlayerPosMtx[0][3];
    body->_1a0[1][3] = body->mPlayerPosMtx[1][3];
    body->_1a0[2][3] = body->mPlayerPosMtx[2][3];

    f32 f1 = sus1->mSusBase - sus1->mSusCur;
    f32 f2 = sus2->mSusBase - sus2->mSusCur;
    f32 f3 = sus3->mSusBase - sus3->mSusCur;
    f32 f4 = sus4->mSusBase - sus4->mSusCur;

    sus1->mArmMtx[1][3] = sus1->mSusMtx[1][3];
    sus2->mArmMtx[1][3] = sus2->mSusMtx[1][3];
    sus3->mArmMtx[1][3] = sus3->mSusMtx[1][3];
    sus4->mArmMtx[1][3] = sus4->mSusMtx[1][3];

    sus1->mSusPrev = sus1->mSusCur;
    sus2->mSusPrev = sus2->mSusCur;
    sus3->mSusPrev = sus3->mSusCur;
    sus4->mSusPrev = sus4->mSusCur;

    if (body->getThunder()->mFlags & 1)
    {
        f32 offset = ThunderOffet[body->mIdx].x * (1.0f - body->getThunder()->getScale());
        sus1->mArmMtx[1][3] += offset;
        sus2->mArmMtx[1][3] += offset;
        sus3->mArmMtx[1][3] += offset;
        sus4->mArmMtx[1][3] += offset;
    }

    f32 bumpThing = (f1 + f2 + f3 + f4) / 4.0f;
    if (bumpThing > BodyFullBumpData[body->mIdx])
    {
        bumpThing = BodyFullBumpData[body->mIdx];
    }

    GetKartCtrl()->ChaseFnumber(&body->_554, -bumpThing, 0.5f);

    f32 fVar6 = (BodyOpData[body->mIdx]->_10 + body->_548 + body->_554);
    body->_1a0[0][3] += (fVar6 * body->_1a0[0][1]);
    body->_1a0[1][3] += (fVar6 * body->_1a0[1][1]);
    body->_1a0[2][3] += (fVar6 * body->_1a0[2][1]);

    if (body->getThunder()->mFlags & 1)
    {
        MatrixScale(body->_1a0, body->getThunder()->getScale());

        f32 fVar1;
        if((1.0f - body->getThunder()->getScale()) >= 0.2f) {
            fVar1 = BodyFullHeightData[body->mIdx] / (ThunderOffet[body->mIdx].y * (body->getThunder()->getScale()));
        }
        else {
            fVar1 = 0.0f;
        }

        f32 fVar2 = (fVar1 * (fVar6 + GetKartCtrl()->GetKartBodyOffset(body->mMynum)));
        body->_1a0[0][3] -= (body->_1a0[0][1] * fVar2);
        body->_1a0[1][3] -= (body->_1a0[1][1] * fVar2);
        body->_1a0[2][3] -= (body->_1a0[2][1] * fVar2);
    }
}

void KartDisp::MakeSusMatrix(int i)
{
    KartBody *body = mBody;
    KartSus *sus = GetKartCtrl()->getKartSus(body->mMynum * 4 + i);
    int idx = RaceMgr::getCurrentManager()->getKartInfo(body->mMynum)->getKartID();

#line 233
    JUT_MINMAX_ASSERT(0, idx, 22);
    JUT_MINMAX_ASSERT(0, i, 4);

    Mtx xNorm = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };
    Mtx zNorm = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };
    Mtx yNorm = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };

    JGeometry::TVec3f v1, v2, v3, v4, v5, v6;
    f32 fVal20, angle;

    angle = TireOpData[idx][i]._4;
    fVal20 = TireOpData[idx][i]._20;

    PSMTXCopy(body->_110, yNorm);
    if (body->getThunder()->mFlags & 1)
        MatrixScale(yNorm, body->getThunder()->getScale());

    GetKartCtrl()->RotZMatrix(zNorm, angle);
    PSMTXConcat(zNorm, sus->mArmMtx, sus->mTireMtx);
    PSMTXConcat(yNorm, sus->mTireMtx, sus->mTireMtx);
    f32 rot = (-sus->mSusPrev - fVal20);
    PSMTXConcat(body->_1a0, sus->mSuspArmMtx, sus->mSuspMtx);

    v1.set(sus->mSuspMtx[0][3], sus->mSuspMtx[1][3], sus->mSuspMtx[2][3]);
    sus->mTireMtx[0][3] = (sus->mTireMtx[0][1] * rot + sus->mTireMtx[0][3]);
    sus->mTireMtx[1][3] = (sus->mTireMtx[1][1] * rot + sus->mTireMtx[1][3]);
    sus->mTireMtx[2][3] = (sus->mTireMtx[2][1] * rot + sus->mTireMtx[2][3]);

    if ((i == 2) || (i == 3))
    {
        sus->mTireMtx[0][1] = sus->mTireMtx[0][1] * TireOpData[idx][i]._18;
        sus->mTireMtx[1][1] = sus->mTireMtx[1][1] * TireOpData[idx][i]._18;
        sus->mTireMtx[2][1] = sus->mTireMtx[2][1] * TireOpData[idx][i]._18;
        sus->mTireMtx[0][2] = sus->mTireMtx[0][2] * TireOpData[idx][i]._18;
        sus->mTireMtx[1][2] = sus->mTireMtx[1][2] * TireOpData[idx][i]._18;
        sus->mTireMtx[2][2] = sus->mTireMtx[2][2] * TireOpData[idx][i]._18;
    }
    if ((i == 1) || (i == 3))
    {
        v3.set(10.0f, ArmOpData[idx][i]._c, 0.0f);
    }
    else
    {
        v3.set(-10.0f, ArmOpData[idx][i]._c, 0.0f);
    }

    PSMTXMultVecSR(body->_1a0, (Vec *)&v3, (Vec *)&v4);
    v4.add(v1);
    v2.set(sus->mTireMtx[0][3], sus->mTireMtx[1][3], sus->mTireMtx[2][3]);
    f32 tireScale = TireOpData[idx][i]._2c;

    if (tireScale != 1.0f)
    {
        v2.x += sus->mTireMtx[0][1] * tireScale;
        v2.y += sus->mTireMtx[1][1] * tireScale;
        v2.z += sus->mTireMtx[2][1] * tireScale;
    }

    v5.sub(v1, v4);
    v6.sub(v1, v2);

    angle = v5.angle(v6);
    if ((i == 1) || (i == 3))
    {
        GetKartCtrl()->RotZMatrix(zNorm, (0.18f * angle));
    }
    else
    {
        GetKartCtrl()->RotZMatrix(zNorm, (0.18f * -angle));
    }

    GetKartCtrl()->RotXMatrix(xNorm, sus->mTireDispAngle);
    PSMTXConcat(zNorm, xNorm, xNorm);
    PSMTXConcat(sus->mTireMtx, xNorm, xNorm);
    PSMTXCopy(xNorm, sus->mTireMtx);

    if (TireOpData[idx][i]._28 != 1.0f)
    {
        sus->mTireMtx[0][0] = sus->mTireMtx[0][0] * TireOpData[idx][i]._28;
        sus->mTireMtx[1][0] = sus->mTireMtx[1][0] * TireOpData[idx][i]._28;
        sus->mTireMtx[2][0] = sus->mTireMtx[2][0] * TireOpData[idx][i]._28;
    }

    if (body->mGameStatus & 0x1000)
    {
        JGeometry::TVec3f v7, v8, v9;

        if (sus->mSuspArmMtx[0][3] >= 0.0f)
        {
            v7.sub(v2, v1);
        }
        else
        {
            v7.sub(v1, v2);
        }
        v7.normalize();

        v9.set(sus->mSuspMtx[0][2], sus->mSuspMtx[1][2], sus->mSuspMtx[2][2]);
        v9.normalize();
        v8.cross(v9, v7);
        v8.normalize();
        v7.cross(v8, v9);
        sus->mSuspMtx[0][0] = v7.x;
        sus->mSuspMtx[1][0] = v7.y;
        sus->mSuspMtx[2][0] = v7.z;
        sus->mSuspMtx[0][1] = v8.x;
        sus->mSuspMtx[1][1] = v8.y;
        sus->mSuspMtx[2][1] = v8.z;
        PSMTXConcat(body->_1a0, sus->mDumpMtx, sus->mDumpArmMtx);
        v1.set(sus->mDumpArmMtx[0][3], sus->mDumpArmMtx[1][3], sus->mDumpArmMtx[2][3]);
        v2.x = DumpOpData[idx][i]._c;
        v2.y = 0.0f;
        v2.z = 0.0f;
        PSMTXMultVec(sus->mSuspMtx, (Vec *)&v2, (Vec *)&v7);
        v8.sub(v1, v7);

        f32 len = v8.length();
        v9.set(sus->mDumpArmMtx[0][2], sus->mDumpArmMtx[1][2], sus->mDumpArmMtx[2][2]);
        v9.normalize();
        v7.cross(v8, v9);
        v7.normalize();
        v8.cross(v9, v7);
        sus->mDumpArmMtx[0][0] = v7.x;
        sus->mDumpArmMtx[1][0] = v7.y;
        sus->mDumpArmMtx[2][0] = v7.z;
        sus->mDumpArmMtx[0][1] = v8.x;
        sus->mDumpArmMtx[1][1] = v8.y;
        sus->mDumpArmMtx[2][1] = v8.z;

        f32 scale = len / DumpOpData[idx][i]._10;
        sus->mDumpArmMtx[0][1] = sus->mDumpArmMtx[0][1] * scale;
        sus->mDumpArmMtx[1][1] = sus->mDumpArmMtx[1][1] * scale;
        sus->mDumpArmMtx[2][1] = sus->mDumpArmMtx[2][1] * scale;

        if ((i == 3) || (i == 1))
        {
            GetKartCtrl()->ClearRotMatrix(xNorm);
            GetKartCtrl()->RotYMatrix(xNorm, 3.141f);
            PSMTXConcat(sus->mSuspMtx, xNorm, sus->mSuspMtx);
        }
    }
}

void KartDisp::MatrixScale(Mtx m, f32 scalar)
{
    m[0][0] *= scalar;
    m[1][0] *= scalar;
    m[2][0] *= scalar;

    m[0][1] *= scalar;
    m[1][1] *= scalar;
    m[2][1] *= scalar;

    m[0][2] *= scalar;
    m[1][2] *= scalar;
    m[2][2] *= scalar;
}

void KartDisp::MakeTireDispMatrix()
{
    KartBody *body = mBody;

    Mtx m = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };

    f32 yNorm = body->getDossin()->_1c;
    int num = body->mMynum;

    PSMTXCopy(GetKartCtrl()->getKartSus(num * 4)->mTireMtx, m);
    m[0][1] *= yNorm;
    m[1][1] *= yNorm;
    m[2][1] *= yNorm;

    GetKartCtrl()->getKartSus(num * 4)->mWheel->setBaseTRMtx(m);

    if (GetKartCtrl()->getKartAppendix(num * 2)->mModelWheel && (body->mIdx == 7))
    {
        m[0][3] += body->_1a0[0][2] * TireOpData[body->mIdx]->_30;
        m[1][3] += body->_1a0[1][2] * TireOpData[body->mIdx]->_30;
        m[2][3] += body->_1a0[2][2] * TireOpData[body->mIdx]->_30;

        GetKartCtrl()->getKartAppendix(num * 2)->mModelWheel->setBaseTRMtx(m);
    }

    PSMTXCopy(GetKartCtrl()->getKartSus(num * 4 + 1)->mTireMtx, m);
    m[0][1] *= yNorm;
    m[1][1] *= yNorm;
    m[2][1] *= yNorm;

    GetKartCtrl()->getKartSus(num * 4 + 1)->mWheel->setBaseTRMtx(m);

    if (GetKartCtrl()->getKartAppendix(num * 2 + 1)->mModelWheel && (body->mIdx == 7))
    {
        m[0][3] += body->_1a0[0][2] * TireOpData[body->mIdx]->_30;
        m[1][3] += body->_1a0[1][2] * TireOpData[body->mIdx]->_30;
        m[2][3] += body->_1a0[2][2] * TireOpData[body->mIdx]->_30;

        GetKartCtrl()->getKartAppendix(num * 2 + 1)->mModelWheel->setBaseTRMtx(m);
    }

    PSMTXCopy(GetKartCtrl()->getKartSus(num * 4 + 2)->mTireMtx, m);
    m[0][1] *= yNorm;
    m[1][1] *= yNorm;
    m[2][1] *= yNorm;

    GetKartCtrl()->getKartSus(num * 4 + 2)->mWheel->setBaseTRMtx(m);

    if (GetKartCtrl()->getKartAppendix(num * 2)->mModelWheel && (body->mIdx == 9))
    {
        m[0][3] += body->_1a0[0][2] * TireOpData[body->mIdx]->_30;
        m[1][3] += body->_1a0[1][2] * TireOpData[body->mIdx]->_30;
        m[2][3] += body->_1a0[2][2] * TireOpData[body->mIdx]->_30;

        GetKartCtrl()->getKartAppendix(num * 2)->mModelWheel->setBaseTRMtx(m);
    }

    PSMTXCopy(GetKartCtrl()->getKartSus(num * 4 + 3)->mTireMtx, m);
    m[0][1] *= yNorm;
    m[1][1] *= yNorm;
    m[2][1] *= yNorm;

    GetKartCtrl()->getKartSus(num * 4 + 3)->mWheel->setBaseTRMtx(m);

    if (GetKartCtrl()->getKartAppendix(num * 2 + 1)->mModelWheel && (body->mIdx == 9))
    {
        m[0][3] += body->_1a0[0][2] * TireOpData[body->mIdx]->_30;
        m[1][3] += body->_1a0[1][2] * TireOpData[body->mIdx]->_30;
        m[2][3] += body->_1a0[2][2] * TireOpData[body->mIdx]->_30;

        GetKartCtrl()->getKartAppendix(num * 2 + 1)->mModelWheel->setBaseTRMtx(m);
    }

    if (body->mGameStatus & 0x1000)
    {
        GetKartCtrl()->getKartSus(num * 4 + 0)->mArm->setBaseTRMtx(GetKartCtrl()->getKartSus(num * 4 + 0)->mSuspMtx);
        GetKartCtrl()->getKartSus(num * 4 + 1)->mArm->setBaseTRMtx(GetKartCtrl()->getKartSus(num * 4 + 1)->mSuspMtx);
        GetKartCtrl()->getKartSus(num * 4 + 2)->mArm->setBaseTRMtx(GetKartCtrl()->getKartSus(num * 4 + 2)->mSuspMtx);
        GetKartCtrl()->getKartSus(num * 4 + 3)->mArm->setBaseTRMtx(GetKartCtrl()->getKartSus(num * 4 + 3)->mSuspMtx);

        GetKartCtrl()->getKartSus(num * 4 + 0)->mShock->setBaseTRMtx(GetKartCtrl()->getKartSus(num * 4 + 0)->mDumpArmMtx);
        GetKartCtrl()->getKartSus(num * 4 + 1)->mShock->setBaseTRMtx(GetKartCtrl()->getKartSus(num * 4 + 1)->mDumpArmMtx);
        GetKartCtrl()->getKartSus(num * 4 + 2)->mShock->setBaseTRMtx(GetKartCtrl()->getKartSus(num * 4 + 2)->mDumpArmMtx);
        GetKartCtrl()->getKartSus(num * 4 + 3)->mShock->setBaseTRMtx(GetKartCtrl()->getKartSus(num * 4 + 3)->mDumpArmMtx);
    }
}

void KartDisp::MakeBodyDispMatrix()
{
    KartBody *body = mBody;
    Mtx m = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };

    f32 yNorm = body->getDossin()->_1c;
    PSMTXCopy(body->_1a0, m);

    m[0][1] *= yNorm;
    m[1][1] *= yNorm;
    m[2][1] *= yNorm;

    RaceMgr::getManager()->getKartDrawer(body->mMynum)->setShadowInfo(body->mShadowArea);
    body->mKartLoader->getExModelBody()->setBaseTRMtx(m);
    body->mShadowModel->setBaseTRMtx(body->_1a0);
    body->mShadowModel->setPosY(body->mBodyGround.getHeight());

    if (body->getThunder()->mFlags & 1)
        body->mShadowModel->setScale(body->getThunder()->getScale(), body->getThunder()->getScale());
    else
        body->mShadowModel->setScale(body->_568, body->_568);

    body->mExModels[0]->setBaseTRMtx(m);
    body->mExModels[1]->setBaseTRMtx(m);
}

void KartDisp::MakeScaleMatrix()
{
    KartBody *body = mBody;
    if (body->_568 != 1.0f)
    {
        MatrixScale(body->_1a0, body->_568);

        KartSus *sus1 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 0);
        KartSus *sus2 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 1);
        KartSus *sus3 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 2);
        KartSus *sus4 = GetKartCtrl()->getKartSus(body->mMynum * 4 + 3);

        MatrixScale(sus1->mTireMtx, body->_568);
        MatrixScale(sus2->mTireMtx, body->_568);
        MatrixScale(sus3->mTireMtx, body->_568);
        MatrixScale(sus4->mTireMtx, body->_568);

        MatrixScale(sus1->mSuspMtx, body->_568);
        MatrixScale(sus2->mSuspMtx, body->_568);
        MatrixScale(sus3->mSuspMtx, body->_568);
        MatrixScale(sus4->mSuspMtx, body->_568);

        MatrixScale(sus1->mDumpArmMtx, body->_568);
        MatrixScale(sus2->mDumpArmMtx, body->_568);
        MatrixScale(sus3->mDumpArmMtx, body->_568);
        MatrixScale(sus4->mDumpArmMtx, body->_568);
    }
}

void KartDisp::MakeKartDisp()
{
    MakeDispBodyPose();
    MakeDispBodyPos();
    MakeSusMatrix(0);
    MakeSusMatrix(1);
    MakeSusMatrix(2);
    MakeSusMatrix(3);
    MakeScaleMatrix();
    MakeTireDispMatrix();
    MakeBodyDispMatrix();
}

// TODO: figure out what/how these functions originally reported
void KartDisp::DomFalgDisp()
{
    // UNUSED
    KartBody *body = mBody;
    KartDrawer *drawer = RaceMgr::getCurrentManager()->getKartDrawer(body->mMynum);
    JUTReport(0, 0, "WHEEL_SPIN");
    JUTReport(0, 0, "ROLL_CRASH");
    JUTReport(0, 0, "MASH_DASH");
    JUTReport(0, 0, "START_DASH");
    JUTReport(0, 0, "BOARD_DASH");
    JUTReport(0, 0, "KAME_CRASH");
    JUTReport(0, 0, "BOMB_CRASH");
    JUTReport(0, 0, "HALF_SPIN");
    JUTReport(0, 0, "PUSH_START");
    JUTReport(0, 0, "eTHUNDER");
    JUTReport(0, 0, "JUMP_DASH");
    JUTReport(0, 0, "eNonStatus");
    JUTReport(0, 0, "eTumbleCrash");
    JUTReport(0, 0, "eKameCrash");
    JUTReport(0, 0, "eBombCrash");
    JUTReport(0, 0, "eHalfSpin");
    JUTReport(0, 0, "eWheelSpin");
    JUTReport(0, 0, "eShootCrash");
    JUTReport(0, 0, "NUMBER %d\n", body->mMynum);
}

void KartDisp::DoDispDebug()
{
    // UNUSED
    JUTReport(0, 0, "104: %f\n");
    JUTReport(0, 0, "105: %f\n");
    JUTReport(0, 0, "106: %f\n");
    JUTReport(0, 0, "108: %f\n");
    JUTReport(0, 0, "109: %f\n");
    JUTReport(0, 0, "110: %f\n");
    JUTReport(0, 0, "111: %f\n");
    JUTReport(0, 0, "112: %f\n");
    JUTReport(0, 0, "113: %f\n");
    JUTReport(0, 0, "114: %f\n");
    JUTReport(0, 0, "115: %f\n");
    JUTReport(0, 0, "116: %f\n");
    JUTReport(0, 0, "117: %f\n");
    JUTReport(0, 0, "124: %f\n");
    JUTReport(0, 0, "125: %f\n");
    JUTReport(0, 0, "128: %f\n");
    JUTReport(0, 0, "129: %f\n");
    JUTReport(0, 0, "130: %f\n");
    JUTReport(0, 0, "131: %f\n");
    JUTReport(0, 0, "132: %d\n");
    JUTReport(0, 0, "133: %d\n");
    JUTReport(0, 0, "134: %d\n");
    JUTReport(0, 0, "135: %d\n");
    JUTReport(0, 0, "136: %d\n");
    JUTReport(0, 0, "137: %d\n");
    JUTReport(0, 0, "138: %d\n");
    JUTReport(0, 0, "139: %d\n");
    JUTReport(0, 0, "140: %d\n");
    JUTReport(0, 0, "141: %d\n");
    JUTReport(0, 0, "142: %d\n");
    JUTReport(0, 0, "143: %d\n");
    JUTReport(0, 0, "144: %d\n");
    JUTReport(0, 0, "145: %d\n");
    JUTReport(0, 0, "146: %d\n");
    JUTReport(0, 0, "147: %d\n");
    JUTReport(0, 0, "148: %d\n");
    JUTReport(0, 0, "149: %d\n");

    JUTReport(0, 0, "151: %d\n");
    JUTReport(0, 0, "154: %d\n");
    JUTReport(0, 0, "155: %d\n");
    JUTReport(0, 0, "156: %d\n");
    JUTReport(0, 0, "157: %d\n");
    JUTReport(0, 0, "158: %d\n");
    JUTReport(0, 0, "159: %d\n");
    JUTReport(0, 0, "160: %d\n");
    JUTReport(0, 0, "161: %d\n");
    JUTReport(0, 0, "162: %d\n");
    JUTReport(0, 0, "163: %d\n");
    JUTReport(0, 0, "164: %d\n");
    JUTReport(0, 0, "165: %d\n");
    JUTReport(0, 0, "166: %d\n");
    JUTReport(0, 0, "167: %d\n");
    JUTReport(0, 0, "169: %d\n");
    JUTReport(0, 0, "170: %d\n");

    JUTReport(0, 0, "171: %f %f %f\n");
    JUTReport(0, 0, "172: %f %f %f\n");
    JUTReport(0, 0, "173: %f %f %f\n");
    JUTReport(0, 0, "174: %f %f %f\n");
    JUTReport(0, 0, "175: %f %f %f\n");
    JUTReport(0, 0, "176: %f %f %f\n");
    JUTReport(0, 0, "177: %f %f %f\n");
    JUTReport(0, 0, "178: %f %f %f\n");
    JUTReport(0, 0, "179: %f %f %f\n");
    JUTReport(0, 0, "180: %f %f %f\n");
    JUTReport(0, 0, "181: %f %f %f\n");
    JUTReport(0, 0, "182: %f %f %f\n");
    JUTReport(0, 0, "183: %f %f %f\n");
    JUTReport(0, 0, "184: %f %f %f\n");
    // JUTReport(0, 0, "185: %f %f %f\n");
    JUTReport(0, 0, "186: %f %f %f\n");
    JUTReport(0, 0, "187: %f %f %f\n");
    JUTReport(0, 0, "188: %f %f %f\n");
    JUTReport(0, 0, "189: %f %f %f\n");
    JUTReport(0, 0, "190: %f %f %f\n");
    JUTReport(0, 0, "191: %f %f %f\n");
    JUTReport(0, 0, "192: %f %f %f\n");
    JUTReport(0, 0, "193: %f %f %f\n");
    JUTReport(0, 0, "194: %f %f %f\n");
    JUTReport(0, 0, "195: %f %f %f\n");
    JUTReport(0, 0, "196: %f %f %f\n");
    JUTReport(0, 0, "197: %f %f %f\n");
    JUTReport(0, 0, "198: %f %f %f\n");
    JUTReport(0, 0, "199: %f %f %f\n");
    JUTReport(0, 0, "200: %f %f %f\n");
    JUTReport(0, 0, "201: %f %f %f\n");
    JUTReport(0, 0, "202: %f %f %f\n");
    JUTReport(0, 0, "203: %f %f %f\n");
    JUTReport(0, 0, "204: %f %f %f\n");
    JUTReport(0, 0, "205: %f %f %f\n");
    JUTReport(0, 0, "206: %f %f %f\n");
    JUTReport(0, 0, "207: %f %f %f\n");
    JUTReport(0, 0, "208: %f %f %f\n");
    JUTReport(0, 0, "209: %f %f %f\n");
    JUTReport(0, 0, "210: %f %f %f\n");
    JUTReport(0, 0, "211: %f %f %f\n");
    JUTReport(0, 0, "212: %f %f %f\n");
    JUTReport(0, 0, "213: %f %f %f\n");
    JUTReport(0, 0, "214: %f %f %f\n");
    JUTReport(0, 0, "215: %f\n");
    JUTReport(0, 0, "216: %d\n");
    JUTReport(0, 0, "217: %f\n");
    JUTReport(0, 0, "218: %f\n");

    JUTReport(0, 0, "309: %f f\n");

    JUTReport(0, 0, "1 + %d : %f %f %f\n");
    JUTReport(0, 0, "2 + %d : %f %f %f\n");
    JUTReport(0, 0, "3 + %d : %f %f %f\n");
    JUTReport(0, 0, "4 + %d : %f %f %f\n");
    JUTReport(0, 0, "5 + %d : %f %f %f\n");
    JUTReport(0, 0, "6 + %d : %f %f %f\n");
    JUTReport(0, 0, "7 + %d : %f %f %f\n");
    JUTReport(0, 0, "8 + %d : %f %f %f\n");
    JUTReport(0, 0, "9 + %d : %f %f %f\n");
    JUTReport(0, 0, "10+ %d : %f %f %f\n");
    JUTReport(0, 0, "11 + %d: %d\n");
    JUTReport(0, 0, "12 + %d: %d\n");
    JUTReport(0, 0, "13 + %d: %d\n");
    JUTReport(0, 0, "14 + %d: %f\n");
    JUTReport(0, 0, "15 + %d: %f\n");
    JUTReport(0, 0, "16 + %d: %f\n");
    JUTReport(0, 0, "17 + %d: %f\n");
    JUTReport(0, 0, "18 + %d: %f\n");
    JUTReport(0, 0, "19 + %d: %f\n");
    JUTReport(0, 0, "20 + %d: %f\n");
    JUTReport(0, 0, "21 + %d: %f\n");
    JUTReport(0, 0, "22 + %d: %f\n");
    JUTReport(0, 0, "23 + %d: %f\n");
    JUTReport(0, 0, "24 + %d: %f\n");
    JUTReport(0, 0, "25 + %d: %f\n");
    // JUTReport(0, 0, "26 + %d: %f\n");
    JUTReport(0, 0, "27 + %d: %f\n");
    JUTReport(0, 0, "28 + %d: %f\n");
    JUTReport(0, 0, "29 + %d: %f\n");
    JUTReport(0, 0, "30 + %d: %f\n");
    JUTReport(0, 0, "31 + %d: %f\n");
    JUTReport(0, 0, "32 + %d: %f\n");
    // JUTReport(0, 0, "33 + %d: %f\n");
    // JUTReport(0, 0, "34 + %d: %f\n");
    JUTReport(0, 0, "35 + %d: %f\n");
    JUTReport(0, 0, "36 + %d: %f\n");
    JUTReport(0, 0, "37 + %d: %f\n");
    JUTReport(0, 0, "38 + %d: %f\n");
    JUTReport(0, 0, "39 + %d: %f\n");
    JUTReport(0, 0, "40 + %d: %f\n");
    JUTReport(0, 0, "41 + %d: %f\n");
    JUTReport(0, 0, "42 + %d: %f\n");
    JUTReport(0, 0, "43 + %d: %f\n");
    JUTReport(0, 0, "44 + %d: %f\n");
    JUTReport(0, 0, "45 + %d: %f\n");
    JUTReport(0, 0, "46 + %d: %f\n");
    JUTReport(0, 0, "47 + %d: %f\n");
}