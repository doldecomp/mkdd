#include "Yamamoto/kartSus.h"

#include "Yamamoto/kartBody.h"
#include "Yamamoto/kartCtrl.h"
#include "Yamamoto/kartLine.h"
#include "Yamamoto/kartParams.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

KartBody *KartSus::getKartBody() { return mBody; }

void KartSus::InitTirePose(int n)
{
    JGeometry::TVec3f localVec;
    localVec.set(mSusMtx[0][3] + mTireDim[1], mSusMtx[1][3] - mTireRadius - mSusBase, mSusMtx[2][3]);
    PSMTXMultVec(getKartBody()->_110, &localVec, &mContactPos);
    mGndPoint.set(mContactPos);
    mCrsGnd.reset();
    mCrsGnd.search(mContactPos, mContactPos);
    mGndHeight = mCrsGnd.getHeight();
    mCrsGnd.getNormal(&mGndNormal);
    if (mCrsGnd.isObject()) {
        mObjKind = mCrsGnd.getObject()->getKind();
    }
    else {
        mObjKind = 0;
    }
    localVec.set(mSusMtx[0][3] + mTireDim[1], 0.0f, mSusMtx[2][3]);
    PSMTXMultVec(getKartBody()->_110, &localVec, &mContactPos);
    mContactPos.y = mGndHeight;
    int idx = getKartBody()->mIdx;
    if (n == 1 || n == 0) {
        mContactPos.x += mGndNormal.x * (mTireRadius - mSusBase * BodyOpData[idx]->_8);
        mContactPos.y += mGndNormal.y * (mTireRadius - mSusBase * BodyOpData[idx]->_8);
        mContactPos.z += mGndNormal.z * (mTireRadius - mSusBase * BodyOpData[idx]->_8);
    }
    else {
        mContactPos.x += mGndNormal.x * (mTireRadius - mSusBase * BodyOpData[idx]->_c);
        mContactPos.y += mGndNormal.y * (mTireRadius - mSusBase * BodyOpData[idx]->_c);
        mContactPos.z += mGndNormal.z * (mTireRadius - mSusBase * BodyOpData[idx]->_c);
    }
    mSusPos.set(0.0f, 1.0f, 0.0f);
    mSplinePts[0].set(mContactPos);
    mSplinePts[1].set(mContactPos);
    mSplinePts[2].set(mContactPos);
    mSplinePts[3].set(mContactPos);
}

void KartSus::InitSettingParam(int n)
{
    getKartBody();
    getKartBody();
    if (n == 0 || n == 1) {
        mSusPowerMul = 26.0f;
        mSusBase = SusParamsData[getKartBody()->mIdx]->_30;
        mSuspFrontY = SusParamsData[getKartBody()->mIdx]->_34;
    }
    else {
        mSusPowerMul = 26.0f;
        mSusBase = SusParamsData[getKartBody()->mIdx]->_38;
        mSuspRearY = SusParamsData[getKartBody()->mIdx]->_3c;
    }
    mSusFactor = 0.97f;
}

void KartSus::InitParam(int n)
{
    JGeometry::TVec3f localVec;
    Mtx mtx1 = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };
    GetKartCtrl();
    int idx = getKartBody()->mIdx;
    mTireDim[2] = 0.0f;
    mTireDispAngle = 0.0f;
    mWheelRPM = 0.0f;
    mSusFlags = 0;
    mGripScale = 1.0f;
    mSusParam0 = 0.0f;
    mRPM = 0.0f;
    mWheel = mLoader->getExModelWheel(n);
    mArm = mLoader->getExModelArm(n);
    mShock = mLoader->getExModelShock(n);
    InitSettingParam(n);
    if (n == 0 || n == 1) {
        mSpring = SusParamsData[idx]->_00[0];
        mDamp = SusParamsData[idx]->_00[1];
        localVec.set(TireOpData[idx][n]._10, mSuspFrontY, TireOpData[idx][n]._14);
    }
    else {
        mSpring = SusParamsData[idx]->_00[2];
        mDamp = SusParamsData[idx]->_00[3];
        localVec.set(TireOpData[idx][n]._10, mSuspRearY, TireOpData[idx][n]._14);
    }
    mTireSusA = TireParamsData[idx]->_1c;
    mTireSusB = TireParamsData[idx]->_20;
    mTireRadius = TireOpData[idx][n]._8;
    mTireDim[1] = TireOpData[idx][n]._c;
    mTireDim[0] = 1.2f * TireOpData[idx][2]._8;
    mSusCur = mSusBase;
    mSusPrev = mSusCur;
    mDampExt = SusParamsData[idx]->_00[4];
    mDampComp = SusParamsData[idx]->_00[5];
    mSpringBase = SusParamsData[idx]->_00[7];
    mSpringMax = SusParamsData[idx]->_00[8];
    mSpringMin = SusParamsData[idx]->_00[9];
    mSpringDiv = SusParamsData[idx]->_00[11];
    mSpringK = SusParamsData[idx]->_00[10];
    Mtx mtx2 = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };
    if (n == 1 || n == 3) {
        GetKartCtrl()->RotZMatrix(mtx2, 0.174444f);
    }
    else {
        GetKartCtrl()->RotZMatrix(mtx2, -0.174444f);
    }
    GetKartCtrl()->SetPosePosMatrix(mSusMtx, mtx2, &localVec);
    GetKartCtrl()->SetPosePosMatrix(mArmMtx, mtx1, &localVec);
    mTireGrip = TireParamsData[idx]->_18;
    mSusPower = SusParamsData[idx]->_00[6];
    localVec.set(ArmOpData[idx][n]._0, ArmOpData[idx][n]._4, ArmOpData[idx][n]._8);
    GetKartCtrl()->SetPosePosMatrix(mSuspArmMtx, mtx1, &localVec);
    localVec.set(DumpOpData[idx][n]._0, DumpOpData[idx][n]._4, DumpOpData[idx][n]._8);
    GetKartCtrl()->SetPosePosMatrix(mDumpMtx, mtx1, &localVec);
}

void KartSus::Init(int n)
{
    Mtx localMtx = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };
    mSpring = 0.0f;
    mDamp = 0.0f;
    mSusBase = 0.0f;
    mSusCur = 0.0f;
    mSusForce = 0.0f;
    mTireRadius = 0.0f;
    mTireDim[0] = 0.0f;
    mTireDim[1] = 0.0f;
    mTireDim[2] = 0.0f;
    mTireDispAngle = 0.0f;
    mWheelRPM = 0.0f;
    mSusFactor = 0.0f;
    mRPM = 0.0f;
    mTireGrip = 0.0f;
    mGripScale = 0.0f;
    mDampExt = 0.0f;
    mDampComp = 0.0f;
    mSusPower = 0.0f;
    mSusPowerMul = 0.0f;
    mTireSusA = 0.0f;
    mTireSusB = 0.0f;
    mGndHeight = 0.0f;
    mSpringBase = 0.0f;
    mSpringMax = 0.0f;
    mSpringMin = 0.0f;
    mSpringDiv = 0.0f;
    mSpringK = 0.0f;
    mSusParam2 = 0.0f;
    mSusFlags = 0;
    mSuspFrontY = 0.0f;
    mSuspRearY = 0.0f;
    mSusPos.zero();
    mContactPos.zero();
    mGndPoint.zero();
    mForcePos.zero();
    mGndNormal.zero();
    mScratchVec.zero();
    mSusState = 0xff;
    GetKartCtrl()->SetPosePosMatrix(mSusMtx, localMtx, &mContactPos);
    GetKartCtrl()->SetPosePosMatrix(mTireMtx, localMtx, &mContactPos);
    GetKartCtrl()->SetPosePosMatrix(mArmMtx, localMtx, &mContactPos);
    InitParam(n);
    InitTirePose(n);
}

void KartSus::NormalInit(int n)
{
    mSpline = new Spline;
    Init(n);
}

void KartSus::ResetInit(int n)
{
    Init(n);
}

void KartSus::CircleFriction(JGeometry::TVec3<float> *vec, float value)
{
    f32 len = vec->length();
    if (len != 0.0f && len > value) {
        vec->scale(value / len);
    }
}

f32 KartSus::GetCornerForce()
{
    if (getKartBody()->mCarStatus & 3) {
        return 250.0f;
    }
    else {
        return 280.0f;
    }
}

f32 KartSus::GetCircleFric()
{
    f32 ret;
    switch (getKartBody()->mBodyGround.getAttribute()) {
        case 1:
            ret = getKartBody()->_530;
            break;
        case 0:
        case 3:
        case 11:
        case 12:
        case 17:
            ret = getKartBody()->_534;
            break;
        case 4:
            ret = getKartBody()->_538;
            break;
        default:
            ret = getKartBody()->_530;
            break;
    }
    if (this->mSusState == 6 || getKartBody()->_58c == 3) {
        ret = 0.0f;
    }
    else if (getKartBody()->mCarStatus & 0x40020074000ULL) {
        ret = getKartBody()->_530;
    }
    if (ret > 1.0f) {
        ret = getKartBody()->_530;
    }
    return ret;
}

void KartSus::DoSusAction(int)
{
    JGeometry::TVec3f v3c;
    JGeometry::TVec3f v30;
    JGeometry::TVec3f v24;
    JGeometry::TVec3f v18;
    f32 tmp;

    if (getKartBody()->getThunder()->mFlags & 1) {
        v3c.set((mSusMtx[0][3] + mTireDim[1]) * getKartBody()->getThunder()->getScale(),
            mSusMtx[1][3],
            mSusMtx[2][3] * getKartBody()->getThunder()->getScale());
    }
    else {
        v3c.set(mSusMtx[0][3] + mTireDim[1], mSusMtx[1][3], mSusMtx[2][3]);
    }
    PSMTXMultVec(getKartBody()->_110, &v3c, &mContactPos);
    mSusForce = 0.0f;
    mSusFlags &= ~4;
    f32 tireRadius = mTireRadius;
    if (getKartBody()->getThunder()->mFlags & 1) {
        v3c.set((mSusMtx[0][3] + mTireDim[1]) * getKartBody()->getThunder()->getScale(),
            mSusMtx[1][3] - tireRadius - mSusCur,
            mSusMtx[2][3] * getKartBody()->getThunder()->getScale());
    }
    else {
        v3c.set(mSusMtx[0][3] + mTireDim[1], mSusMtx[1][3] - tireRadius - mSusCur, mSusMtx[2][3]);
    }
    PSMTXMultVec(getKartBody()->_110, &v3c, &v18);
    mCrsGnd.search(v18, mGndPoint);

    JGeometry::TVec3f v0c;
    bool isSpecial = false;
    int attr = mCrsGnd.getAttribute();
    switch (attr) {
    case 18:
    case 2:
        isSpecial = true;
        break;
    default:
        mGndHeight = mCrsGnd.getHeight();
        mCrsGnd.getNormal(&mGndNormal);
        mGndPoint.set(v18);
        mSplinePts[3].set(v18.x, mGndHeight, v18.z);
        mSpline->setAll(mSplinePts);
        mSpline->getBezierPoint(&v0c, 0.95f);
        mGndHeight = v0c.y;
        if (mGndHeight > mContactPos.y) {
            mContactPos.y = mGndHeight;
        }
        break;
    }
    mSusPos.set(v18.x, mGndHeight, v18.z);
    mSplinePts[0].set(mSplinePts[1]);
    mSplinePts[1].set(mSplinePts[2]);
    mSplinePts[2].set(mSplinePts[3]);
    if (mCrsGnd.isObject()) {
        mObjKind = mCrsGnd.getObject()->getKind();
    }
    else {
        mObjKind = 0;
    }
    if (isSpecial || mCrsGnd.getAttribute() == 10 || getKartBody()->_2fc.y < 0.0f ||
        (getKartBody()->_2fc.y < 0.1f && !(getKartBody()->mCarStatus & 0x100000ULL)) ||
        (getKartBody()->getRescue()->mFlags & 0x20) ||
        (getKartBody()->getCannon()->mFlags & 0x20)) {
        mSusCur = mSusBase;
        mSusFlags &= ~1;
        return;
    }
    v3c.set(v18.x, mGndHeight, v18.z);
    v24.sub(v3c, mContactPos);
    f32 length = v24.length();
    tmp = length - tireRadius;
    if (tmp > mSusBase) {
        tmp = mSusCur;
        GetKartCtrl()->ChaseFnumber(&tmp, mSusBase, 0.35f);
        if (tmp > mSusBase) {
            tmp = mSusBase;
        }
        mSusFlags &= ~1;
    }
    else {
        mSusFlags |= 1;
        if (tmp < 0.0f) {
            tmp = 0.0f;
        }
        f32 f30v = tmp - mSusBase;
        f32 spring;
        if (tmp == mSusBase) {
            spring = 0.0f;
        }
        else {
            f32 f5 = tmp - mSusCur;
            if (f5 >= 0.0f) {
                f32 t = 1.0f - f5 / mSpringDiv;
                if (t <= 0.1f) {
                    t = 0.1f;
                }
                spring = f5 * (mDampComp * t);
            }
            else {
                f32 t = 1.0f - f5 / (-mSpringDiv);
                if (t <= 0.1f) {
                    t = 0.1f;
                }
                spring = f5 * (mDampExt * t);
            }
        }
        f32 f29 = mSpring;
        f32 f2 = 1.0f - (tmp / (mSusBase * mSpringBase) - 1.0f);
        if (f2 > 1.0f) {
            f2 *= 0.98f;
        }
        if (f2 > mSpringMax) {
            f2 = mSpringMax;
        }
        if (f2 < mSpringMin) {
            f2 = mSpringMin;
        }
        f29 *= f2;
        f32 f28 = (mDamp * spring) * (0.000025f * getKartBody()->_3a4);
        f30v = (-f29 * f30v) * (0.00002f * getKartBody()->_3a4) - f28;
        f30v *= mSusPowerMul * SusPowerData[getKartBody()->mIdx];
        if (f30v >= 95.0f * getKartBody()->_3a4) {
            f30v = 95.0f * getKartBody()->_3a4;
        }
        if (!(getKartBody()->mGameStatus & 4)) {
            f30v = getKartBody()->_3a4;
        }
        if (getKartBody()->mCarStatus & 0x3ULL) {
            v30.set(0.2f * (f30v * getKartBody()->_2fc.x), f30v * getKartBody()->_2fc.y, 0.2f * (f30v * getKartBody()->_2fc.z));
        }
        else {
            v30.set(0.73f * (f30v * getKartBody()->_2fc.x), f30v * getKartBody()->_2fc.y, 0.73f * (f30v * getKartBody()->_2fc.z));
        }
        mForcePos.set(mContactPos.x - length * getKartBody()->_2fc.x, mContactPos.y - length * getKartBody()->_2fc.y, mContactPos.z - length * getKartBody()->_2fc.z);
        getKartBody()->DoForce(&mForcePos, &v30);
        mSusForce = f30v;
    }
    mSusCur = tmp;
    v30.set(0.0f, mSusPower * getKartBody()->_3ac, 0.0f);
    getKartBody()->DoForce(&mContactPos, &v30);
    mSusState = 0xff;
}

void KartSus::DoTireAction()
{
    JGeometry::TVec3f A;
    JGeometry::TVec3f B;
    JGeometry::TVec3f C;
    JGeometry::TVec3f D;
    JGeometry::TVec3f E;
    JGeometry::TVec3f F;
    Mtx m = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };

    if (mCrsGnd.getAttribute() == 2) {
        return;
    }
    if (mSusForce == 0.0f) {
        return;
    }
    if (getKartBody()->mCarStatus & 0x00202000ULL) {
        getKartBody()->GroundReflection(&mForcePos, &mGndNormal, getKartBody()->_430, getKartBody()->_42c, 0.5f);
        return;
    }
    A.zero();
    GetKartCtrl()->MulMatrix(m, mSusMtx, getKartBody()->_110);
    E.set(m[0][0], m[1][0], m[2][0]);
    B.cross(E, mGndNormal);
    B.normalize();
    f32 scale = 3.0f * (-200.0f * mWheelRPM);
    D.set(B.x * scale, B.y * scale, B.z * scale);
    E.set(mForcePos.x - getKartBody()->mPos.x, mForcePos.y - getKartBody()->mPos.y, mForcePos.z - getKartBody()->mPos.z);
    E.scale(5.0f * mTireGrip);
    if (getKartBody()->_458 <= 30.0f && !(getKartBody()->mCarStatus & 0x3ULL)) {
        GetKartCtrl()->ChaseFnumber(&mGripScale, 0.4f, 0.2f);
        E.scale(mGripScale);
    }
    else {
        mGripScale = 1.0f;
    }
    JGeometry::TVec3f G;
    G.set(getKartBody()->_2c0);
    if (getKartBody()->mBodyGround.getSpiralCode() == 1) {
        G.scale(0.221f);
    }
    else {
        G.scale(0.15f);
    }
    E.cross(G, E);
    C.add(getKartBody()->mVel, E);
    C.add(D);
    GetKartCtrl()->VectorElement(&E, &C, &mGndNormal);
    if (getKartBody()->mBodyGround.getSpiralCode() == 1) {
        E.scale(2.3f);
    }
    else {
        E.scale(2.15f);
    }
    C.sub(E);
    F.set(C);
    f32 cornerForce = GetCornerForce();
    E.cross(C, B);
    E.cross(E, B);
    E.scale(mSusForce * cornerForce);
    C.scale(-mSusForce);
    A.add(C, E);
    mSusState = getKartBody()->mBodyGround.getAttribute();
    f32 fric = GetCircleFric();
    CircleFriction(&A, fric * mSusForce);
    getKartBody()->DoForce(&mForcePos, &A);
    f32 dot = A.dot(B);
    mWheelRPM = mWheelRPM - 4.0f * (dot / getKartBody()->_3a4);
}
