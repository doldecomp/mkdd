#ifndef _DOLPHIN_MTX_H
#define _DOLPHIN_MTX_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif // ifdef __cplusplus

    typedef float Mtx[3][4];
    typedef float (*MtxPtr)[4];
    typedef float Mtx23[2][3];
    typedef float Mtx33[3][3];
    typedef float Mtx44[4][4];
    typedef float PSQuaternion[4];

    typedef struct
    {
        f32 x, y, z;
    } Vec, *VecPtr;

    typedef struct
    {
        s16 x, y, z;
    } S16Vec, *S16VecPtr;

    typedef struct
    {
        f32 x, y, z, w;
    } Quaternion, *QuaternionPtr;

// (pi/180)
#define MTXDegToRad(a) ((a) * 0.01745329252f) 

    // Paired single versions

    void PSMTXIdentity(Mtx m);
    void PSMTXCopy(const Mtx src, Mtx dst);
    void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
    void PSMTXConcatArray(const Mtx a, const Mtx *srcBase, Mtx *dstBase, u32 count);
    void PSMTXTranspose(const Mtx src, Mtx xPose);
    u32 PSMTXInverse(const Mtx src, Mtx inv);
    u32 PSMTXInvXpose(const Mtx src, Mtx invX);
    void PSMTXMultVec(const Mtx m, const Vec *src, Vec *dst);
    void PSMTXMultVecArray(const Mtx m, const Vec *srcBase, Vec *dstBase, u32 count);
    void PSMTXMultVecSR(const Mtx m, const Vec *src, Vec *dst);
    void PSMTXMultVecArraySR(const Mtx m, const Vec *srcBase, Vec *dstBase, u32 count);
    void PSMTXQuat(Mtx m, const Quaternion *q);
    void PSMTXReflect(Mtx m, const Vec *p, const Vec *n);
    void PSMTXTrans(Mtx m, f32 xT, f32 yT, f32 zT);
    void PSMTXTransApply(const Mtx src, Mtx dst, f32 xT, f32 yT, f32 zT);
    void PSMTXScale(Mtx m, f32 xS, f32 yS, f32 zS);
    void PSMTXScaleApply(const Mtx src, Mtx dst, f32 xS, f32 yS, f32 zS);
    void PSMTXRotRad(Mtx m, char axis, f32 rad);
    void PSMTXRotTrig(Mtx m, char axis, f32 sinA, f32 cosA);
    void PSMTXRotAxisRad(Mtx m, const Vec *axis, f32 rad);

    // move to vec.h?
    void PSVECAdd(const Vec *a, const Vec *b, Vec *ab);
    void PSVECSubtract(const Vec *a, const Vec *b, Vec *ab);
    void PSVECScale(const Vec *src, Vec *dst, f32 scale);
    void PSVECNormalize(const Vec *src, Vec *dst);
    f32 PSVECSquareMag(const Vec *v);
    f32 PSVECMag(const Vec *v);
    f32 PSVECDotProduct(const Vec *a, const Vec *b);
    void PSVECCrossProduct(const Vec *a, const Vec *b, Vec *ab);
    f32 PSVECSquareDistance(const Vec *a, const Vec *b);
    f32 PSVECDistance(const Vec *a, const Vec *b);

    void PSQUATMultiply(const Quaternion*, const Quaternion*, Quaternion*);
    void PSQUATScale(const Quaternion*, Quaternion*, f32);
    f32 PSQUATDotProduct(const Quaternion*, const Quaternion*);
    void PSQUATNormalize(const Quaternion*, Quaternion*);
    void PSQUATInverse(const Quaternion*, Quaternion*);
    void C_QUATMtx(Quaternion*, const Mtx);
    void C_QUATLerp(const Quaternion*, const Quaternion*, Quaternion*, f32);
    void C_QUATSlerp(const Quaternion*, const Quaternion*, Quaternion*, f32);

    void PSMTX44MultVec(const Mtx44 m, const Vec *src, Vec *dst);
    void PSMTX44MultVecArray(const Mtx44 m, const Vec *srcBase, Vec *dstBase, u32 count);
    void PSMTX44MultVecSR(const Mtx44 m, const Vec *src, Vec *dst);
    void PSMTX44MultVecArraySR(const Mtx44 m, const Vec *srcBase, Vec *dstBase, u32 count);

    // C Versions

    void C_MTXIdentity(Mtx m);
    void C_MTXCopy(const Mtx src, Mtx dst);
    void C_MTXConcat(const Mtx a, const Mtx b, Mtx ab);
    void C_MTXConcatArray(const Mtx a, const Mtx *srcBase, Mtx *dstBase, u32 count);
    void C_MTXTranspose(const Mtx src, Mtx xPose);
    u32 C_MTXInverse(const Mtx src, Mtx inv);
    u32 C_MTXInvXpose(const Mtx src, Mtx invX);
    void C_MTXMultVec(const Mtx m, const Vec *src, Vec *dst);
    void C_MTXMultVecArray(const Mtx m, const Vec *srcBase, Vec *dstBase, u32 count);
    void C_MTXMultVecSR(const Mtx m, const Vec *src, Vec *dst);
    void C_MTXMultVecArraySR(const Mtx m, const Vec *srcBase, Vec *dstBase, u32 count);
    void C_MTXQuat(Mtx m, const Quaternion *q);
    void C_MTXReflect(Mtx m, const Vec *p, const Vec *n);
    void C_MTXTrans(Mtx m, f32 xT, f32 yT, f32 zT);
    void C_MTXTransApply(const Mtx src, Mtx dst, f32 xT, f32 yT, f32 zT);
    void C_MTXScale(Mtx m, f32 xS, f32 yS, f32 zS);
    void C_MTXScaleApply(const Mtx src, Mtx dst, f32 xS, f32 yS, f32 zS);
    void C_MTXRotRad(Mtx m, char axis, f32 rad);
    void C_MTXRotTrig(Mtx m, char axis, f32 sinA, f32 cosA);
    void C_MTXRotAxisRad(Mtx m, const Vec *axis, f32 rad);
    void C_MTXLookAt(Mtx m, const Vec *camPos, const Vec *camUp, const Vec *target);
    void C_MTXFrustum(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
    void C_MTXPerspective(Mtx44 m, f32 fovY, f32 aspect, f32 n, f32 f);
    void C_MTXOrtho(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
    void C_MTXLightFrustum(Mtx m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 scaleS, f32 scaleT, f32 transS,
                           f32 transT);

    void C_MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT, f32 transS,
                               f32 transT);

    void C_MTXLightOrtho(Mtx m, f32 t, f32 b, f32 l, f32 r, f32 scaleS, f32 scaleT, f32 transS,
                         f32 transT);

#ifdef __cplusplus
}
#endif

#endif
