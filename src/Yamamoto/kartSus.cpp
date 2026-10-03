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
    localVec.set(_128[0][3] + _100[1], _128[1][3] - mTireRadius - _b0, _128[2][3]);
    PSMTXMultVec(getKartBody()->_110, &localVec, &_c0);
    _cc.set(_c0);
    mCrsGnd.reset();
    mCrsGnd.search(_c0, _c0);
    _114[3] = mCrsGnd.getHeight();
    mCrsGnd.getNormal(&_e4);
    if (mCrsGnd.isObject()) {
        *(u32 *)&_290[0x38] = mCrsGnd.getObject()->getKind();
    }
    else {
        *(u32 *)&_290[0x38] = 0;
    }
    localVec.set(_128[0][3] + _100[1], 0.0f, _128[2][3]);
    PSMTXMultVec(getKartBody()->_110, &localVec, &_c0);
    _c0.y = _114[3];
    int idx = getKartBody()->mIdx;
    if (n == 1 || n == 0) {
        _c0.x += _e4.x * (mTireRadius - _b0 * BodyOpData[idx]->_8);
        _c0.y += _e4.y * (mTireRadius - _b0 * BodyOpData[idx]->_8);
        _c0.z += _e4.z * (mTireRadius - _b0 * BodyOpData[idx]->_8);
    }
    else {
        _c0.x += _e4.x * (mTireRadius - _b0 * BodyOpData[idx]->_c);
        _c0.y += _e4.y * (mTireRadius - _b0 * BodyOpData[idx]->_c);
        _c0.z += _e4.z * (mTireRadius - _b0 * BodyOpData[idx]->_c);
    }
    _284.set(0.0f, 1.0f, 0.0f);
    _78[0].set(_c0);
    _78[1].set(_c0);
    _78[2].set(_c0);
    _78[3].set(_c0);
}

void KartSus::InitSettingParam(int n)
{
    getKartBody();
    getKartBody();
    if (n == 0 || n == 1) {
        *(f32 *)&_290[4] = 26.0f; // 0x294
        _b0 = SusParamsData[getKartBody()->mIdx]->_30;
        *(f32 *)&_290[0x30] = SusParamsData[getKartBody()->mIdx]->_34; // 0x2c0
    }
    else {
        *(f32 *)&_290[4] = 26.0f; // 0x294
        _b0 = SusParamsData[getKartBody()->mIdx]->_38;
        *(f32 *)&_290[0x2c] = SusParamsData[getKartBody()->mIdx]->_3c; // 0x2bc
    }
    *(f32 *)&_114[0] = 0.97f; // 0x114
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
    _100[2] = 0.0f;
    _10c = 0.0f;
    _110 = 0.0f;
    _124 = 0;
    *(f32 *)&_27c[4] = 1.0f;
    *(f32 *)&_290[16] = 0.0f;
    _278 = 0.0f;
    mWheel = mLoader->getExModelWheel(n);
    mArm = mLoader->getExModelArm(n);
    mShock = mLoader->getExModelShock(n);
    InitSettingParam(n);
    if (n == 0 || n == 1) {
        *(f32 *)&_a8[0] = SusParamsData[idx]->_00[0];
        *(f32 *)&_a8[4] = SusParamsData[idx]->_00[1];
        localVec.set(TireOpData[idx][n]._10, *(f32 *)&_290[0x30], TireOpData[idx][n]._14);
    }
    else {
        *(f32 *)&_a8[0] = SusParamsData[idx]->_00[2];
        *(f32 *)&_a8[4] = SusParamsData[idx]->_00[3];
        localVec.set(TireOpData[idx][n]._10, *(f32 *)&_290[0x2c], TireOpData[idx][n]._14);
    }
    *(f32 *)&_290[8] = TireParamsData[idx]->_1c;
    *(f32 *)&_290[12] = TireParamsData[idx]->_20;
    mTireRadius = TireOpData[idx][n]._8;
    _100[1] = TireOpData[idx][n]._c;
    _100[0] = 1.2f * TireOpData[idx][2]._8;
    _b4 = _b0;
    _b8 = _b4;
    _114[1] = SusParamsData[idx]->_00[4];
    _114[2] = SusParamsData[idx]->_00[5];
    *(f32 *)&_290[24] = SusParamsData[idx]->_00[7];
    *(f32 *)&_290[28] = SusParamsData[idx]->_00[8];
    *(f32 *)&_290[32] = SusParamsData[idx]->_00[9];
    *(f32 *)&_290[40] = SusParamsData[idx]->_00[11];
    *(f32 *)&_290[36] = SusParamsData[idx]->_00[10];
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
    GetKartCtrl()->SetPosePosMatrix(_128, mtx2, &localVec);
    GetKartCtrl()->SetPosePosMatrix(_188, mtx1, &localVec);
    *(f32 *)&_27c[0] = TireParamsData[idx]->_18;
    *(f32 *)&_290[0] = SusParamsData[idx]->_00[6];
    localVec.set(ArmOpData[idx][n]._0, ArmOpData[idx][n]._4, ArmOpData[idx][n]._8);
    GetKartCtrl()->SetPosePosMatrix(_1b8, mtx1, &localVec);
    localVec.set(DumpOpData[idx][n]._0, DumpOpData[idx][n]._4, DumpOpData[idx][n]._8);
    GetKartCtrl()->SetPosePosMatrix(_218, mtx1, &localVec);
}

void KartSus::Init(int n)
{
    Mtx localMtx = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
    };
    *(f32 *)&_a8[0] = 0.0f;
    *(f32 *)&_a8[4] = 0.0f;
    _b0 = 0.0f;
    _b4 = 0.0f;
    *(f32 *)&_bc[0] = 0.0f;
    mTireRadius = 0.0f;
    _100[0] = 0.0f;
    _100[1] = 0.0f;
    _100[2] = 0.0f;
    _10c = 0.0f;
    _110 = 0.0f;
    _114[0] = 0.0f;
    _278 = 0.0f;
    *(f32 *)&_27c[0] = 0.0f;
    *(f32 *)&_27c[4] = 0.0f;
    _114[1] = 0.0f;
    _114[2] = 0.0f;
    *(f32 *)&_290[0] = 0.0f;
    *(f32 *)&_290[4] = 0.0f;
    *(f32 *)&_290[8] = 0.0f;
    *(f32 *)&_290[12] = 0.0f;
    _114[3] = 0.0f;
    *(f32 *)&_290[24] = 0.0f;
    *(f32 *)&_290[28] = 0.0f;
    *(f32 *)&_290[32] = 0.0f;
    *(f32 *)&_290[40] = 0.0f;
    *(f32 *)&_290[36] = 0.0f;
    *(f32 *)&_290[52] = 0.0f;
    _124 = 0;
    *(f32 *)&_290[48] = 0.0f;
    *(f32 *)&_290[44] = 0.0f;
    _284.zero();
    _c0.zero();
    _cc.zero();
    _d8.zero();
    _e4.zero();
    _f0.zero();
    _290[0x14] = 0xff;
    GetKartCtrl()->SetPosePosMatrix(_128, localMtx, &_c0);
    GetKartCtrl()->SetPosePosMatrix(_158, localMtx, &_c0);
    GetKartCtrl()->SetPosePosMatrix(_188, localMtx, &_c0);
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
    if (this->_290[0x14] == 6 || getKartBody()->_58c == 3) {
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
        v3c.set((_128[0][3] + _100[1]) * getKartBody()->getThunder()->getScale(),
            _128[1][3],
            _128[2][3] * getKartBody()->getThunder()->getScale());
    }
    else {
        v3c.set(_128[0][3] + _100[1], _128[1][3], _128[2][3]);
    }
    PSMTXMultVec(getKartBody()->_110, &v3c, &_c0);
    *(f32 *)&_bc[0] = 0.0f;
    _124 &= ~4;
    f32 tireRadius = mTireRadius;
    if (getKartBody()->getThunder()->mFlags & 1) {
        v3c.set((_128[0][3] + _100[1]) * getKartBody()->getThunder()->getScale(),
            _128[1][3] - tireRadius - _b4,
            _128[2][3] * getKartBody()->getThunder()->getScale());
    }
    else {
        v3c.set(_128[0][3] + _100[1], _128[1][3] - tireRadius - _b4, _128[2][3]);
    }
    PSMTXMultVec(getKartBody()->_110, &v3c, &v18);
    mCrsGnd.search(v18, _cc);

    JGeometry::TVec3f v0c;
    bool isSpecial = false;
    int attr = mCrsGnd.getAttribute();
    switch (attr) {
    case 18:
    case 2:
        isSpecial = true;
        break;
    default:
        _114[3] = mCrsGnd.getHeight();
        mCrsGnd.getNormal(&_e4);
        _cc.set(v18);
        _78[3].set(v18.x, _114[3], v18.z);
        mSpline->setAll(_78);
        mSpline->getBezierPoint(&v0c, 0.95f);
        _114[3] = v0c.y;
        if (_114[3] > _c0.y) {
            _c0.y = _114[3];
        }
        break;
    }
    _284.set(v18.x, _114[3], v18.z);
    _78[0].set(_78[1]);
    _78[1].set(_78[2]);
    _78[2].set(_78[3]);
    if (mCrsGnd.isObject()) {
        *(u32 *)&_290[0x38] = mCrsGnd.getObject()->getKind();
    }
    else {
        *(u32 *)&_290[0x38] = 0;
    }
    if (isSpecial || mCrsGnd.getAttribute() == 10 || getKartBody()->_2fc.y < 0.0f ||
        (getKartBody()->_2fc.y < 0.1f && !(getKartBody()->mCarStatus & 0x100000ULL)) ||
        (getKartBody()->getRescue()->mFlags & 0x20) ||
        (getKartBody()->getCannon()->mFlags & 0x20)) {
        _b4 = _b0;
        _124 &= ~1;
        return;
    }
    v3c.set(v18.x, _114[3], v18.z);
    v24.sub(v3c, _c0);
    f32 length = v24.length();
    tmp = length - tireRadius;
    if (tmp > _b0) {
        tmp = _b4;
        GetKartCtrl()->ChaseFnumber(&tmp, _b0, 0.35f);
        if (tmp > _b0) {
            tmp = _b0;
        }
        _124 &= ~1;
    }
    else {
        _124 |= 1;
        if (tmp < 0.0f) {
            tmp = 0.0f;
        }
        f32 f30v = tmp - _b0;
        f32 spring;
        if (tmp == _b0) {
            spring = 0.0f;
        }
        else {
            f32 f5 = tmp - _b4;
            if (f5 >= 0.0f) {
                f32 t = 1.0f - f5 / *(f32 *)&_290[0x28];
                if (t <= 0.1f) {
                    t = 0.1f;
                }
                spring = f5 * (_114[2] * t);
            }
            else {
                f32 t = 1.0f - f5 / (-*(f32 *)&_290[0x28]);
                if (t <= 0.1f) {
                    t = 0.1f;
                }
                spring = f5 * (_114[1] * t);
            }
        }
        f32 f29 = *(f32 *)&_a8[0];
        f32 f2 = 1.0f - (tmp / (_b0 * *(f32 *)&_290[0x18]) - 1.0f);
        if (f2 > 1.0f) {
            f2 *= 0.98f;
        }
        if (f2 > *(f32 *)&_290[0x1c]) {
            f2 = *(f32 *)&_290[0x1c];
        }
        if (f2 < *(f32 *)&_290[0x20]) {
            f2 = *(f32 *)&_290[0x20];
        }
        f29 *= f2;
        f32 f28 = (*(f32 *)&_a8[4] * spring) * (0.000025f * getKartBody()->_3a4);
        f30v = (-f29 * f30v) * (0.00002f * getKartBody()->_3a4) - f28;
        f30v *= *(f32 *)&_290[4] * SusPowerData[getKartBody()->mIdx];
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
        _d8.set(_c0.x - length * getKartBody()->_2fc.x, _c0.y - length * getKartBody()->_2fc.y, _c0.z - length * getKartBody()->_2fc.z);
        getKartBody()->DoForce(&_d8, &v30);
        *(f32 *)&_bc[0] = f30v;
    }
    _b4 = tmp;
    v30.set(0.0f, *(f32 *)&_290[0] * getKartBody()->_3ac, 0.0f);
    getKartBody()->DoForce(&_c0, &v30);
    _290[0x14] = 0xff;
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
    if (*(f32 *)&_bc[0] == 0.0f) {
        return;
    }
    if (getKartBody()->mCarStatus & 0x00202000ULL) {
        getKartBody()->GroundReflection(&_d8, &_e4, getKartBody()->_430, getKartBody()->_42c, 0.5f);
        return;
    }
    A.zero();
    GetKartCtrl()->MulMatrix(m, _128, getKartBody()->_110);
    E.set(m[0][0], m[1][0], m[2][0]);
    B.cross(E, _e4);
    B.normalize();
    f32 scale = 3.0f * (-200.0f * _110);
    D.set(B.x * scale, B.y * scale, B.z * scale);
    E.set(_d8.x - getKartBody()->mPos.x, _d8.y - getKartBody()->mPos.y, _d8.z - getKartBody()->mPos.z);
    E.scale(5.0f * *(f32 *)&_27c[0]);
    if (getKartBody()->_458 <= 30.0f && !(getKartBody()->mCarStatus & 0x3ULL)) {
        GetKartCtrl()->ChaseFnumber((f32 *)&_27c[4], 0.4f, 0.2f);
        E.scale(*(f32 *)&_27c[4]);
    }
    else {
        *(f32 *)&_27c[4] = 1.0f;
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
    GetKartCtrl()->VectorElement(&E, &C, &_e4);
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
    E.scale(*(f32 *)&_bc[0] * cornerForce);
    C.scale(-*(f32 *)&_bc[0]);
    A.add(C, E);
    _290[0x14] = getKartBody()->mBodyGround.getAttribute();
    f32 fric = GetCircleFric();
    CircleFriction(&A, fric * *(f32 *)&_bc[0]);
    getKartBody()->DoForce(&_d8, &A);
    f32 dot = A.dot(B);
    _110 = _110 - 4.0f * (dot / getKartBody()->_3a4);
}
