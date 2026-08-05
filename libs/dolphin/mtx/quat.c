#include "PowerPC_EABI_Support/MSL_C/PPC_EABI/math_ppc.h"
#include "dolphin/mtx.h"

void PSQUATMultiply(const register Quaternion *p,
                    const register Quaternion *q,
                    register Quaternion *pq)
{
    register f32 pxy, pzw;
    register f32 qxy, qzw;
    register f32 pnxy, pnzw, pnxny, pnznw;
    register f32 rxy, rzw;
    register f32 sxy, szw;

#ifdef __MWERKS__
    asm {
        psq_l pxy, 0x0(p), 0, 0;
        psq_l pzw, 0x8(p), 0, 0;
        psq_l qxy, 0x0(q), 0, 0;
        ps_neg pnxny, pxy;
        psq_l qzw, 0x8(q), 0, 0;
        ps_neg pnznw, pzw;
        ps_merge01 pnxy, pnxny, pxy;
        ps_muls0 rxy, pzw, qxy;
        ps_muls0 rzw, pnxny, qxy;
        ps_merge01 pnzw, pnznw, pzw;
        ps_muls1 szw, pnxy, qxy;
        ps_madds0 rxy, pnxy, qzw, rxy;
        ps_muls1 sxy, pnzw, qxy;
        ps_madds0 rzw, pnzw, qzw, rzw;
        ps_madds1 szw, pnznw, qzw, szw;
        ps_merge10 rxy, rxy, rxy;
        ps_madds1 sxy, pxy, qzw, sxy;
        ps_merge10 rzw, rzw, rzw;
        ps_add rxy, rxy, sxy;
        psq_st rxy, 0x0(pq), 0, 0;
        ps_sub rzw, rzw, szw;
        psq_st rzw, 0x8(pq), 0, 0;
    }
#endif
}

void PSQUATScale(const register Quaternion *q, register Quaternion *r, register f32 scale)
{
    register f32 rxy, rzw;

#ifdef __MWERKS__
    asm {
        psq_l rxy, 0(q), 0, 0;
        psq_l rzw, 8(q), 0, 0;
        ps_muls0 rxy, rxy, scale;
        psq_st rxy, 0(r), 0, 0;
        ps_muls0 rzw, rzw, scale;
        psq_st rzw, 8(r), 0, 0;
    }
#endif
}

f32 PSQUATDotProduct(const register Quaternion *p, const register Quaternion *q)
{
    register f32 pxy, pzw, qxy, qzw, dp;

#ifdef __MWERKS__
    asm {
        psq_l pxy, 0(p), 0, 0;
        psq_l qxy, 0(q), 0, 0;
        ps_mul dp, pxy, qxy;
        psq_l pzw, 8(p), 0, 0;
        psq_l qzw, 8(q), 0, 0;
        ps_madd dp, pzw, qzw, dp;
        ps_sum0 dp, dp, dp, dp;
    }
#endif

    return dp;
}

void C_QUATRotAxisRad(float r, float *axis, Vec *rad)
{
    Vec vec;
    float fVar1, fVar2, fVar3;

    PSVECNormalize(rad, &vec);

    fVar3 = fVar2 = 0.5f * r;
    fVar3 = sinf(fVar3);
    fVar1 = fVar3;
    fVar3 = cosf(fVar2);
    *axis = fVar1 * vec.x;
    axis[1] = fVar1 * vec.y;
    axis[2] = fVar1 * vec.z;
    axis[3] = fVar3;
}
