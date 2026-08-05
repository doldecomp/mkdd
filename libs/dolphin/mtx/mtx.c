#include "dolphin/mtx.h"

#include "PowerPC_EABI_Support/MSL_C/PPC_EABI/math_ppc.h"

// .sbss
static float Unit01[2] = {0.0f, 1.0f};

static void unused_force_order_in_data_section()
{
    (void)1.0f;
    (void)0.0f;
}

void PSMTXIdentity(register Mtx m)
{
    register f32 c_zero = 0.0f;
    register f32 c_one = 1.0f;
    register f32 c_01;
    register f32 c_10;

#ifdef __MWERKS__
    asm
    {
        psq_st c_zero, 8(m), 0, 0;
        ps_merge01 c_01, c_zero, c_one;
        psq_st c_zero, 24(m), 0, 0;
        ps_merge10 c_10, c_one, c_zero;
        psq_st c_zero, 32(m), 0, 0;
        psq_st c_01, 16(m), 0, 0;
        psq_st c_10, 0(m), 0, 0;
        psq_st c_10, 40(m), 0, 0;
    }
#endif
}

void PSMTXCopy(register const Mtx src, register Mtx dst)
{
#ifdef __MWERKS__
    asm
    {
        psq_l f0, 0(src), 0, 0;
        psq_st f0, 0(dst), 0, 0;
        psq_l f1, 8(src), 0, 0;
        psq_st f1, 8(dst), 0, 0;
        psq_l f2, 16(src), 0, 0;
        psq_st f2, 16(dst), 0, 0;
        psq_l f3, 24(src), 0, 0;
        psq_st f3, 24(dst), 0, 0;
        psq_l f4, 32(src), 0, 0;
        psq_st f4, 32(dst), 0, 0;
        psq_l f5, 40(src), 0, 0;
        psq_st f5, 40(dst), 0, 0;
    }
#endif
}

asm void PSMTXConcat(register const Mtx mA, register const Mtx mB, register Mtx mAB)
{
#ifdef __MWERKS__
    nofralloc;
    stwu r1, -64(r1);
    psq_l f0, 0(mA), 0, 0;
    stfd f14, 8(r1);
    psq_l f6, 0(mB), 0, 0;
    lis r6, Unit01 @ha;
    psq_l f7, 8(mB), 0, 0;
    stfd f15, 16(r1);
    addi r6, r6, Unit01 @l;
    stfd f31, 40(r1);
    psq_l f8, 16(mB), 0, 0;
    ps_muls0 f12, f6, f0;
    psq_l f2, 16(mA), 0, 0;
    ps_muls0 f13, f7, f0;
    psq_l f31, 0(r6), 0, 0;
    ps_muls0 f14, f6, f2;
    psq_l f9, 24(mB), 0, 0;
    ps_muls0 f15, f7, f2;
    psq_l f1, 8(mA), 0, 0;
    ps_madds1 f12, f8, f0, f12;
    psq_l f3, 24(mA), 0, 0;
    ps_madds1 f14, f8, f2, f14;
    psq_l f10, 32(mB), 0, 0;
    ps_madds1 f13, f9, f0, f13;
    psq_l f11, 40(mB), 0, 0;
    ps_madds1 f15, f9, f2, f15;
    psq_l f4, 32(mA), 0, 0;
    psq_l f5, 40(mA), 0, 0;
    ps_madds0 f12, f10, f1, f12;
    ps_madds0 f13, f11, f1, f13;
    ps_madds0 f14, f10, f3, f14;
    ps_madds0 f15, f11, f3, f15;
    psq_st f12, 0(mAB), 0, 0;
    ps_muls0 f2, f6, f4;
    ps_madds1 f13, f31, f1, f13;
    ps_muls0 f0, f7, f4;
    psq_st f14, 16(mAB), 0, 0;
    ps_madds1 f15, f31, f3, f15;
    psq_st f13, 8(mAB), 0, 0;
    ps_madds1 f2, f8, f4, f2;
    ps_madds1 f0, f9, f4, f0;
    ps_madds0 f2, f10, f5, f2;
    lfd f14, 8(r1);
    psq_st f15, 24(mAB), 0, 0;
    ps_madds0 f0, f11, f5, f0;
    psq_st f2, 32(mAB), 0, 0;
    ps_madds1 f0, f31, f5, f0;
    lfd f15, 16(r1);
    psq_st f0, 40(mAB), 0, 0;
    lfd f31, 40(r1);
    addi r1, r1, 64;
    blr;
#endif
}

void PSMTXTranspose(register const Mtx src, register Mtx xPose)
{
    register float c_zero = 0.0f;

#ifdef __MWERKS__
    asm
    {
        psq_l f1, 0x0(r3), 0, 0;
        stfs c_zero, 0x2c(r4);
        psq_l f2, 0x10(r3), 0, 0;
        ps_merge00 f4, f1, f2;
        psq_l f3, 0x8(r3), 1, 0;
        ps_merge11 f5, f1, f2;
        psq_l f2, 0x18(r3), 1, 0;
        psq_st f4, 0x0(r4), 0, 0;
        psq_l f1, 0x20(r3), 0, 0;
        ps_merge00 f2, f3, f2;
        psq_st f5, 0x10(r4), 0, 0;
        ps_merge00 f4, f1, c_zero;
        psq_st f2, 0x20(r4), 0, 0;
        ps_merge10 f5, f1, c_zero;
        psq_st f4, 0x8(r4), 0, 0;
        lfs f3, 0x28(r3);
        psq_st f5, 0x18(r4), 0, 0;
        stfs f3, 0x28(r4);
    }
#endif
}

asm u32 PSMTXInverse(register const Mtx src, register Mtx inv)
{
#ifdef __MWERKS__
    psq_l f0, 0(src), 1, 0;
    psq_l f1, 4(src), 0, 0;
    psq_l f2, 16(src), 1, 0;
    ps_merge10 f6, f1, f0;
    psq_l f3, 20(src), 0, 0;
    psq_l f4, 32(src), 1, 0;
    ps_merge10 f7, f3, f2;
    psq_l f5, 36(src), 0, 0;
    ps_mul f11, f3, f6;
    ps_mul f13, f5, f7;
    ps_merge10 f8, f5, f4;
    ps_msub f11, f1, f7, f11;
    ps_mul f12, f1, f8;
    ps_msub f13, f3, f8, f13;
    ps_mul f10, f3, f4;
    ps_msub f12, f5, f6, f12;
    ps_mul f9, f0, f5;
    ps_mul f8, f1, f2;
    ps_sub f6, f6, f6;
    ps_msub f10, f2, f5, f10;
    ps_mul f7, f0, f13;
    ps_msub f9, f1, f4, f9;
    ps_madd f7, f2, f12, f7;
    ps_msub f8, f0, f3, f8;
    ps_madd f7, f4, f11, f7;
    ps_cmpo0 cr0, f7, f6;
    bne skip_return;
    li r3, 0;
    blr;
skip_return:
    fres f0, f7;
    ps_add f6, f0, f0;
    ps_mul f5, f0, f0;
    ps_nmsub f0, f7, f5, f6;
    lfs f1, 12(src);
    ps_muls0 f13, f13, f0;
    lfs f2, 28(src);
    ps_muls0 f12, f12, f0;
    lfs f3, 44(src);
    ps_muls0 f11, f11, f0;
    ps_merge00 f5, f13, f12;
    ps_muls0 f10, f10, f0;
    ps_merge11 f4, f13, f12;
    ps_muls0 f9, f9, f0;
    psq_st f5, 0(inv), 0, 0;
    ps_mul f6, f13, f1;
    psq_st f4, 16(inv), 0, 0;
    ps_muls0 f8, f8, f0;
    ps_madd f6, f12, f2, f6;
    psq_st f10, 32(inv), 1, 0;
    ps_nmadd f6, f11, f3, f6;
    psq_st f9, 36(inv), 1, 0;
    ps_mul f7, f10, f1;
    ps_merge00 f5, f11, f6;
    psq_st f8, 40(inv), 1, 0;
    ps_merge11 f4, f11, f6;
    psq_st f5, 8(inv), 0, 0;
    ps_madd f7, f9, f2, f7;
    psq_st f4, 24(inv), 0, 0;
    ps_nmadd f7, f8, f3, f7;
    li r3, 1;
    psq_st f7, 44(inv), 1, 0;
#endif
}

void PSMTXRotRad(Mtx m, char axis, f32 rad)
{
    f32 s = sinf(rad);
    f32 c = cosf(rad);

    PSMTXRotTrig(m, axis, s, c);
}

void PSMTXRotTrig(register Mtx m, register char axis, register f32 dsinA, register f32 dcosA)
{
    register f32 fc0;
    register f32 fc1;

    register f32 nsinA;
    register f32 fw0, fw1, fw2, fw3;

    register f32 cosA;
    register f32 sinA;

#ifdef __MWERKS__
    asm
    {
        frsp sinA, dsinA;
        frsp cosA, dcosA;
    }
#endif

    fc0 = 0.0f;
    fc1 = 1.0f;

#ifdef __MWERKS__
    asm
    {
        ori         axis, axis, 0x20;
        ps_neg      nsinA, sinA;
        cmplwi      axis, 'x';
        beq         _case_x;
        cmplwi      axis, 'y';
        beq         _case_y;
        cmplwi      axis, 'z';
        beq         _case_z;
        b           _end;

    _case_x:
        psq_st      fc1,  0(m), 1, 0;
        psq_st      fc0,  4(m), 0, 0;
        ps_merge00  fw0, sinA, cosA;
        psq_st      fc0, 12(m), 0, 0;
        ps_merge00  fw1, cosA, nsinA;
        psq_st      fc0, 28(m), 0, 0;
        psq_st      fc0, 44(m), 1, 0;
        psq_st      fw0, 36(m), 0, 0;
        psq_st      fw1, 20(m), 0, 0;
        b           _end;;

    _case_y:
        ps_merge00  fw0, cosA, fc0;
        ps_merge00  fw1, fc0, fc1;
        psq_st      fc0, 24(m), 0, 0;
        psq_st      fw0,  0(m), 0, 0;
        ps_merge00  fw2, nsinA, fc0;
        ps_merge00  fw3, sinA, fc0;
        psq_st      fw0, 40(m), 0, 0;
        psq_st      fw1, 16(m), 0, 0;
        psq_st      fw3,  8(m), 0, 0;
        psq_st      fw2, 32(m), 0, 0;
        b           _end;;

    _case_z:
        psq_st      fc0,  8(m), 0, 0;
        ps_merge00  fw0, sinA, cosA;
        ps_merge00  fw2, cosA, nsinA;
        psq_st      fc0, 24(m), 0, 0;
        psq_st      fc0, 32(m), 0, 0;
        ps_merge00  fw1, fc1, fc0;
        psq_st      fw0, 16(m), 0, 0;
        psq_st      fw2,  0(m), 0, 0;
        psq_st      fw1, 40(m), 0, 0;

    _end:
    }
#endif
}

static void __PSMTXRotAxisRadInternal(register Mtx m,
                                      const register Vec *axis,
                                      register f32 sT,
                                      register f32 cT)
{
    register float f_0;

    register float f_3;
    register float f_4;
    register float f_5;
    register float f_6;
    register float f_7;
    register float f_8;
    register float three;
    register float half;

    half = 0.5f;
    three = 3.0f;

#ifdef __MWERKS__
    asm
    {
        frsp f11, f2;
        psq_l f2, 0x0(r4), 0, 0;
        frsp f12, f1;
        lfs f3, 0x8(r4);
        ps_mul f4, f2, f2;
        fadds f8, half, half;
        ps_madd f5, f3, f3, f4;
        fsubs f1, half, half;
        ps_sum0 f6, f5, f3, f4;
        fsubs f0, f8, f11;
        frsqrte f7, f6;
        fmuls f4, f7, f7;
        fmuls f5, f7, half;
        fnmsubs f4, f4, f6, three;
        fmuls f7, f4, f5;
        ps_merge00 f11, f11, f11;
        ps_muls0 f2, f2, f7;
        ps_muls0 f3, f3, f7;
        ps_muls0 f6, f2, f0;
        ps_muls0 half, f2, f12;
        ps_muls0 f7, f3, f0;
        ps_muls1 f5, f6, f2;
        ps_muls0 f4, f6, f2;
        ps_muls0 f6, f6, f3;
        fnmsubs f0, f3, f12, f5;
        fmadds f8, f3, f12, f5;
        ps_neg f2, half;
        ps_sum0 three, f6, f1, half;
        ps_sum0 f4, f4, f0, f11;
        ps_sum1 f5, f11, f8, f5;
        ps_sum0 f0, f2, f1, f6;
        psq_st three, 0x8(r3), 0, 0;
        ps_sum0 f2, f6, f6, f2;
        psq_st f4, 0x0(r3), 0, 0;
        ps_muls0 f7, f7, f3;
        psq_st f5, 0x10(r3), 0, 0;
        ps_sum1 f6, half, f2, f6;
        psq_st f0, 0x18(r3), 0, 0;
        ps_sum0 f7, f7, f1, f11;
        psq_st f6, 0x20(r3), 0, 0;
        psq_st f7, 0x28(r3), 0, 0;
    }
#endif
}

void PSMTXRotAxisRad(Mtx m, const Vec *axis, f32 rad)
{
    const float fVar1 = sinf(rad);
    const float fVar2 = cosf(rad);
    __PSMTXRotAxisRadInternal(m, axis, fVar1, fVar2);
}

void PSMTXTrans(Mtx m, f32 xT, f32 yT, f32 zT)
{
    register float zero;
    register float one;

    zero = 0.0f;
    one = 1.0f;

#ifdef __MWERKS__
    asm
    {
        stfs f1, 0xc(r3);
        stfs f2, 0x1c(r3);
        psq_st zero, 0x4(r3), 0, 0;
        psq_st zero, 0x20(r3), 0, 0;
        stfs zero, 0x10(r3);
        stfs one, 0x14(r3);
        stfs zero, 0x18(r3);
        stfs one, 0x28(r3);
        stfs f3, 0x2c(r3);
        stfs one, 0x0(r3);
    }
#endif
}

void PSMTXTransApply(const Mtx src, Mtx dst, f32 xT, f32 yT, f32 zT)
{
#ifdef __MWERKS__
    asm
    {
        psq_l f4, 0x0(r3), 0, 0;
        frsp f1, f1;
        psq_l f5, 0x8(r3), 0, 0;
        frsp f2, f2;
        psq_l f7, 0x18(r3), 0, 0;
        frsp f3, f3;
        psq_l f8, 0x28(r3), 0, 0;
        psq_st f4, 0x0(r4), 0, 0;
        ps_sum1 f5, f1, f5, f5;
        psq_l f6, 0x10(r3), 0, 0;
        psq_st f5, 0x8(r4), 0, 0;
        ps_sum1 f7, f2, f7, f7;
        psq_l f9, 0x20(r3), 0, 0;
        psq_st f6, 0x10(r4), 0, 0;
        ps_sum1 f8, f3, f8, f8;
        psq_st f7, 0x18(r4), 0, 0;
        psq_st f9, 0x20(r4), 0, 0;
        psq_st f8, 0x28(r4), 0, 0;
    }
#endif
}

void PSMTXScale(Mtx m, f32 xS, f32 yS, f32 zS)
{
    register float zero = 0.0f;

#ifdef __MWERKS__
    asm
    {
        stfs f1, 0x0(r3);
        psq_st zero, 0x4(r3), 0, 0;
        psq_st zero, 0xc(r3), 0, 0;
        stfs f2, 0x14(r3);
        psq_st zero, 0x18(r3), 0, 0;
        psq_st zero, 0x20(r3), 0, 0;
        stfs f3, 0x28(r3);
        stfs zero, 0x2c(r3);
    }
#endif
}

void PSMTXScaleApply(const Mtx src, Mtx dst, f32 xS, f32 yS, f32 zS)
{
#ifdef __MWERKS__
    asm
    {
        frsp f1, f1;
        psq_l f4, 0x0(r3), 0, 0;
        frsp f2, f2;
        psq_l f5, 0x8(r3), 0, 0;
        frsp f3, f3;
        ps_muls0 f4, f4, f1;
        psq_l f6, 0x10(r3), 0, 0;
        ps_muls0 f5, f5, f1;
        psq_l f7, 0x18(r3), 0, 0;
        ps_muls0 f6, f6, f2;
        psq_l f8, 0x20(r3), 0, 0;
        psq_st f4, 0x0(r4), 0, 0;
        ps_muls0 f7, f7, f2;
        psq_l f2, 0x28(r3), 0, 0;
        psq_st f5, 0x8(r4), 0, 0;
        ps_muls0 f8, f8, f3;
        psq_st f6, 0x10(r4), 0, 0;
        ps_muls0 f2, f2, f3;
        psq_st f7, 0x18(r4), 0, 0;
        psq_st f8, 0x20(r4), 0, 0;
        psq_st f2, 0x28(r4), 0, 0;
    }
#endif
}

void PSMTXQuat(Mtx m, const Quaternion *q)
{
    register float f_0;
    register float one = 1.0f;

#ifdef __MWERKS__
    asm
    {
        psq_l f4, 0x0(r4), 0, 0;
        psq_l f5, 0x8(r4), 0, 0;
        fsubs f0, one, one;
        fadds f2, one, one;
        ps_mul f6, f4, f4;
        ps_merge10 f9, f4, f4;
        ps_madd f8, f5, f5, f6;
        ps_mul f7, f5, f5;
        ps_sum0 f3, f8, f8, f8;
        ps_muls1 f10, f9, f5;
        fres f11, f3;
        ps_sum1 f8, f7, f8, f6;
        ps_nmsub f3, f3, f11, f2;
        ps_muls1 f7, f5, f5;
        ps_mul f3, f11, f3;
        ps_sum0 f6, f6, f6, f6;
        fmuls f3, f3, f2;
        ps_madd f11, f4, f9, f7;
        ps_msub f7, f4, f9, f7;
        psq_st f0, 0xc(r3), 1, 0;
        ps_nmsub f6, f6, f3, one;
        ps_nmsub f8, f8, f3, one;
        psq_st f0, 0x2c(r3), 1, 0;
        ps_mul f11, f11, f3;
        ps_mul f7, f7, f3;
        psq_st f6, 0x28(r3), 1, 0;
        ps_madds0 f9, f4, f5, f10;
        ps_merge00 f5, f11, f8;
        ps_nmsub f10, f10, f2, f9;
        ps_merge10 f4, f8, f7;
        psq_st f5, 0x10(r3), 0, 0;
        ps_mul f9, f9, f3;
        ps_mul f10, f10, f3;
        psq_st f4, 0x0(r3), 0, 0;
        psq_st f9, 0x8(r3), 1, 0;
        ps_merge10 f7, f10, f0;
        ps_merge01 f11, f10, f9;
        psq_st f7, 0x18(r3), 0, 0;
        psq_st f11, 0x20(r3), 0, 0;
    }
#endif
}

void C_MTXLookAt(Mtx m, const Vec *camPos, const Vec *camUp, const Vec *target)
{
    Vec vLook;
    Vec vRight;
    Vec vUp;

    vLook.x = camPos->x - target->x;
    vLook.y = camPos->y - target->y;
    vLook.z = camPos->z - target->z;
    PSVECNormalize(&vLook, &vLook);
    PSVECCrossProduct(camUp, &vLook, &vRight);
    PSVECNormalize(&vRight, &vRight);
    PSVECCrossProduct(&vLook, &vRight, &vUp);
    m[0][0] = vRight.x;
    m[0][1] = vRight.y;
    m[0][2] = vRight.z;
    m[0][3] = -((camPos->z * vRight.z) + ((camPos->x * vRight.x) + (camPos->y * vRight.y)));
    m[1][0] = vUp.x;
    m[1][1] = vUp.y;
    m[1][2] = vUp.z;
    m[1][3] = -((camPos->z * vUp.z) + ((camPos->x * vUp.x) + (camPos->y * vUp.y)));
    m[2][0] = vLook.x;
    m[2][1] = vLook.y;
    m[2][2] = vLook.z;
    m[2][3] = -((camPos->z * vLook.z) + ((camPos->x * vLook.x) + (camPos->y * vLook.y)));
}

static void unused_force_order_in_data_section2()
{
    (void)2.0f;
    (void)-1.0f;
    (void)0.01745329252f;
}

void C_MTXLightPerspective(Mtx m,
                           f32 fovY,
                           f32 aspect,
                           f32 scaleS,
                           f32 scaleT,
                           f32 transS,
                           f32 transT)
{
    f32 angle;
    f32 cot;

    angle = (0.5f * fovY);
    angle = MTXDegToRad(angle);
    cot = 1.0f / tanf(angle);

    m[0][0] = scaleS * (cot / aspect);
    m[0][1] = 0.0f;
    m[0][2] = -transS;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = cot * scaleT;
    m[1][2] = -transT;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -1.0f;
    m[2][3] = 0.0f;
}

void C_MTXLightOrtho(Mtx m,
                     f32 t,
                     f32 b,
                     f32 l,
                     f32 r,
                     f32 scaleS,
                     f32 scaleT,
                     f32 transS,
                     f32 transT)
{
    f32 tmp;

    tmp = 1.0f / (r - l);
    m[0][0] = 2.0f * tmp * scaleS;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = transS + (scaleS * (tmp * -(r + l)));
    tmp = 1.0f / (t - b);
    m[1][0] = 0.0f;
    m[1][1] = 2.0f * tmp * scaleT;
    m[1][2] = 0.0f;
    m[1][3] = transT + (scaleT * (tmp * -(t + b)));
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 0.0f;
    m[2][3] = 1.0f;
}
