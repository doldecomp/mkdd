#ifndef JGEOMETRY_MATRIX_H
#define JGEOMETRY_MATRIX_H

#include <JSystem/JGeometry/Vec.h>
#include <JSystem/JGeometry/Quat.h>

#include "JSystem/JGeometry/Util.h"
#include "types.h"

namespace JGeometry {
    template <typename T>
    struct SMatrix34C
    {
    public:
        SMatrix34C() {}
        void set(const SMatrix34C<T> &rSrc) {
            JMath::gekko_ps_copy12(this, rSrc);
        }
        void set(T rxx, T ryx, T rzx, T tx, T rxy, T ryy, T rzy, T ty, T rxz, T ryz, T rzz, T tz);

        void scale(T);

        inline void set(const Mtx pSrc)
        {
            JMath::gekko_ps_copy12(this, pSrc);
        }

        f32 at(u32 i, u32 j) const { return mMtx[i][j]; }

        T &ref(u32 i, u32 j) { return mMtx[i][j]; }

        inline Mtx *toMtx()
        {
            return (Mtx *)mMtx;
        }

        inline const Mtx *toCMtx() const
        {
            return (const Mtx *)mMtx;
        }

        operator MtxPtr() const { return (MtxPtr)mMtx; }
        
        T mMtx[3][4];
    };

    typedef SMatrix34C<f32> TSMtxf;

    template <typename T>
    struct TMatrix34 : public T
    {
    public:
        void identity() // TODO: OK?
        {
            const f32 zero = 0;
            this->ref(2, 3) = zero;
            this->ref(1, 3) = zero;
            this->ref(0, 3) = zero;

            this->ref(1, 2) = zero;
            this->ref(0, 2) = zero;

            this->ref(2, 1) = zero;
            this->ref(0, 1) = zero;
            this->ref(2, 0) = zero;
            this->ref(1, 0) = zero;

            const f32 one = TUtilf::one();
            this->ref(2, 2) = one;
            this->ref(1, 1) = one;
            this->ref(0, 0) = one;
        }
        void concat(const T &rSrcA, const T &rSrcB);
        void concat(const T &rSrc);
        void invert(const TMatrix34<T> &rDest);

        void mult(const TVec3f &rSrc, TVec3f &rDest) const;

        void multTranspose(const TVec3f &a1, const TVec3f &a2) const;
    };

    template <class T>
    struct TRotation3 : public T
    {
    public:
        TRotation3() {}
        void identity33();

        inline void getXDir(TVec3f &rDest) const {
            rDest.set(this->at(0, 0), this->at(1, 0), this->at(2, 0));
        }

        inline void getYDir(TVec3f &rDest) const {
            rDest.set(this->at(0,1), this->at(1, 1), this->at(2, 1));
        }

        inline void getZDir(TVec3f &rDest) const
        {
            rDest.set(this->at(0, 2), this->at(1, 2), this->at(2, 2));
        };

        // From SMG
        void setXDir(const TVec3f &param_1)
        {
            this->ref(0, 0) = param_1.x;
            this->ref(1, 0) = param_1.y;
            this->ref(2, 0) = param_1.z;
        }
        void setXDir(f32 x, f32 y, f32 z)
        {
            this->ref(0, 0) = x;
            this->ref(1, 0) = y;
            this->ref(2, 0) = z;
        }
        void setYDir(const TVec3f &param_1)
        {
            this->ref(0, 1) = param_1.x;
            this->ref(1, 1) = param_1.y;
            this->ref(2, 1) = param_1.z;
        }
        void setYDir(f32 x, f32 y, f32 z)
        {
            this->ref(0, 1) = x;
            this->ref(1, 1) = y;
            this->ref(2, 1) = z;
        }
        void setZDir(const TVec3f &param_1)
        {
            this->ref(0, 2) = param_1.x;
            this->ref(1, 2) = param_1.y;
            this->ref(2, 2) = param_1.z;
        }
        void setZDir(f32 x, f32 y, f32 z)
        {
            this->ref(0, 2) = x;
            this->ref(1, 2) = y;
            this->ref(2, 2) = z;
        }
        void setXYZDir(const TVec3f &rSrcX, const TVec3f &rSrcY, const TVec3f &rSrcZ)
        {
            this->ref(0, 0) = rSrcX.x;
            this->ref(1, 0) = rSrcX.y;
            this->ref(2, 0) = rSrcX.z;
            this->ref(0, 1) = rSrcY.x;
            this->ref(1, 1) = rSrcY.y;
            this->ref(2, 1) = rSrcY.z;
            this->ref(0, 2) = rSrcZ.x;
            this->ref(1, 2) = rSrcZ.y;
            this->ref(2, 2) = rSrcZ.z;
        }

        void getEuler(TVec3f &rDest) const;
        void getEulerXYZ(TVec3f &rDest) const;
        void setEulerY(f32 val);
        void setEulerZ(f32 val);

        void setEulerXYZ(const TVec3f &rSrc);

        void setEulerXYZ(f32 x, f32 y, f32 z) { // fabricated? might actually exist
            f32 cr = std::cosf(x); // f26
            f32 cp = std::cosf(y); // f31
            f32 cy = std::cosf(z); // f30
            f32 sr = std::sinf(x);   // f29
            f32 sp = std::sinf(y); // f28
            f32 sy = std::sinf(z); // f7

            // ???
            f32 tmp = sy * sp;
            f32 cpy = (cy * cp);
            f32 spy = sp * sy;

            this->mMtx[0][0] = ((tmp) * sr + (cp * cr));
            this->mMtx[1][0] = (sr * cy);
            this->mMtx[2][0] = -(sp * cr) + (sy * cp * sr);
            
            this->mMtx[0][1] = ((-sr * cp) + (spy) * cr);
            this->mMtx[1][1] = (cy * cr);
            this->mMtx[2][1] = (sp * sr + sy * (cp * cr));

            this->mMtx[0][2] = (cy * sp);
            this->mMtx[1][2] = (-sy);
            this->mMtx[2][2] = (cpy);
        }

        void getQuat(TQuat4f &rDest) const { // Non matching
            f32 z,y,x;
            x = this->mMtx[0][0];
            y = this->mMtx[1][1];
            z = this->mMtx[2][2]; 
            // Case 1: trace is positive, straightforward calculation
            f32 len = x+y+z;
            if (len >= 0.0f) {
                len = TUtilf::sqrt( len + 1.0f);
                f32 invS = 0.5f / len;

                rDest.w = 0.5f * len;
                rDest.x = (this->mMtx[2][1] - this->mMtx[1][2]) * invS;
                rDest.y = (this->mMtx[0][2] - this->mMtx[2][0]) * invS;
                rDest.z = (this->mMtx[1][0] - this->mMtx[0][1]) * invS;
                return;
            }
            // Otherwise, find which major diagonal element has the greatest value
            f32 maxDiagxyz = x >= y ? x : y;
            maxDiagxyz = maxDiagxyz >= z ? maxDiagxyz : z;

            if (maxDiagxyz == x) {
                // Case: mMtx[0][0] is the largest
                len = TUtilf::sqrt(x - (y + z) + 1.0f); 

                f32 invS = 0.5f / len;

                rDest.x = 0.5f * len;
                rDest.y = (this->mMtx[0][1] + this->mMtx[1][0]) * invS;
                rDest.z = (this->mMtx[2][0] + this->mMtx[0][2]) * invS;
                rDest.w = (this->mMtx[2][1] - this->mMtx[1][2]) * invS;   
            }
            else if (maxDiagxyz == y) {
                // Case: mMtx[1][1] is the largest
                f32 s = TUtilf::sqrt((y - (z + x)) + 1.0f);
                f32 invS = 0.5f / s;

                rDest.y = 0.5f * s;
                rDest.z = (this->mMtx[1][2] + this->mMtx[2][1]) * invS;
                rDest.x = (this->mMtx[0][1] + this->mMtx[1][0]) * invS;
                rDest.w = (this->mMtx[0][2] - this->mMtx[2][0]) * invS;
            }
            else {
                f32 s = TUtilf::sqrt((z - (y + x)) + 1.0f);
                f32 invS = 0.5f / s;

                rDest.z = 0.5f * s;
                rDest.x = (this->mMtx[2][0] + this->mMtx[0][2]) * invS;
                rDest.y = (this->mMtx[1][2] + this->mMtx[2][1]) * invS;
                rDest.w = (this->mMtx[1][0] - this->mMtx[0][1]) * invS;                
            }
        }

        void setQuat(const TQuat4f &q) {
            const f32 qx = q.x;
            const f32 qy = q.y;
            const f32 qz = q.z;
            const f32 qw = q.w;

            f32 xx = 2.0f * qx * qx;
            f32 wx = 2.0f * qw * qx;
            
            f32 yy = 2.0f * qy * qy;
            f32 wy = 2.0f * qw * qy;

            f32 xy = 2.0f * qx * qy;
            f32 xz = 2.0f * qx * qz;
            f32 yz = 2.0f * qy * qz;
            
            f32 zz = 2.0f * qz * qz;
            f32 wz = 2.0f * qw * qz;
            
            this->mMtx[0][0] = 1.0f - yy - zz;
            this->mMtx[0][1] = xy - wz;
            this->mMtx[0][2] = xz + wy;

            this->mMtx[1][0] = xy + wz;
            this->mMtx[1][1] = 1.0f - xx - zz;
            this->mMtx[1][2] = yz - wx;

            this->mMtx[2][0] = xz - wy;
            this->mMtx[2][1] = yz + wx;
            this->mMtx[2][2] = 1.0f - xx - yy;
        }

        void makeMtx(MtxPtr pMtx) const {
            f32 yy = 2.0f * this->y * this->y;
            f32 zz = 2.0f * this->z * this->z;
            f32 xx = 2.0f * this->x * this->x;

            f32 xy = 2.0f * this->x * this->y;
            f32 xz = 2.0f * this->x * this->z;
            f32 yz = 2.0f * this->y * this->z;

            f32 wx = 2.0f * this->w * this->x;
            f32 wy = 2.0f * this->w * this->y;
            f32 wz = 2.0f * this->w * this->z;

            pMtx[0][0] = 1.0f - yy - zz;
            pMtx[0][1] = xy - wz;
            pMtx[0][2] = xz + wy;

            pMtx[1][0] = xy + wz;
            pMtx[1][1] = 1.0f - xx - zz;
            pMtx[1][2] = yz - wx;

            pMtx[2][0] = xz - wy;
            pMtx[2][1] = yz + wx;
            pMtx[2][2] = 1.0f - xx - yy;
        }

        void getScale(TVec3f &rDest) const;
        void setScale(const TVec3f &rSrc);

        void setRotate(const TVec3f& v1, const TVec3f& v2) {
            (void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;
            (void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;(void)0;
            TQuat4f q;
            q.setRotate(v1, v2);
            setQuat(q);
        }
        void setRotate(const TVec3f &, f32);

        void mult33(TVec3f &) const;
        void mult33(const TVec3f &, TVec3f &) const;

        inline void getXDirInline(TVec3f &rDest) const {
            f32 z = this->mMtx[2][0];
            f32 y = this->mMtx[1][0];
            f32 x = this->mMtx[0][0];
            rDest.set(x, y, z);
        }

        inline void getYDirInline(TVec3f &rDest) const {
            f32 z = this->mMtx[2][1];
            f32 y = this->mMtx[1][1];
            f32 x = this->mMtx[0][1];
            rDest.set(x, y, z);
        }

        inline void getZDirInline(TVec3f &rDest) const {
            f32 z = this->mMtx[2][2];
            f32 y = this->mMtx[1][2];
            f32 x = this->mMtx[0][2];
            rDest.set(x, y, z);
        }

#ifdef NON_MATCHING
        inline void mult33Inline(const TVec3f &rSrc, TVec3f &rDest) const
        {
            rDest.set<f32>(
                rSrc.z * mMtx[0][2] + (rSrc.y * mMtx[0][0] + (rSrc.x * mMtx[0][1])),
                rSrc.z * mMtx[1][2] + (rSrc.y * mMtx[1][0] + (rSrc.x * mMtx[1][1])),
                rSrc.z * mMtx[2][2] + (rSrc.y * mMtx[2][0] + (rSrc.x * mMtx[2][1])));
        }
#endif
    };

    template <class T>
    struct TPosition3 : public TRotation3<T>
    {
    public:
        TPosition3() {}

        
        void getTrans(TVec3f &rDest) const {
            rDest.set(this->at(0, 3), this->at(1, 3), this->at(2, 3));
        }
        void setTrans(const TVec3<f32>& translation) {
            setTrans(translation.x, translation.y, translation.z);
        }
        void setTrans(f32 x, f32 y, f32 z) {
            this->ref(0, 3) = x;
            this->ref(1, 3) = y;
            this->ref(2, 3) = z;
        }
        void zeroTrans()
        {
            this->ref(0, 3) = 0.0f;
            this->ref(1, 3) = 0.0f;
            this->ref(2, 3) = 0.0f;
        }

        void makeRotate(const TVec3f &, f32);
        void makeQuat(const TQuat4f &rSrc);
        void setPositionFromLookAt(const TPosition3<T> &rLookAt) {
            // regswaps
            this->ref(0, 0) = -rLookAt[0][0]; 
            this->ref(1, 1) = rLookAt[1][1];
            this->ref(2, 2) = -rLookAt[2][2];            
            this->ref(1, 0) = -rLookAt[0][1]; 
            this->ref(0, 1) = rLookAt[1][0]; 
            this->ref(2, 0) = -rLookAt[0][2];
            this->ref(0, 2) = -rLookAt[2][0]; 
            this->ref(1, 2) = -rLookAt[2][1];
            this->ref(2, 1) = rLookAt[1][2];
            

            f32 x = rLookAt[0][3];
            f32 y = rLookAt[1][3];
            f32 z = rLookAt[2][3];

            this->ref(0, 3) = (rLookAt[0][3] * this->mMtx[0][0]) - (rLookAt[1][3] * this->mMtx[0][1]) + rLookAt[2][3] * this->mMtx[0][2];
            this->ref(1, 3) = (rLookAt[0][3] * this->mMtx[1][0]) - (rLookAt[1][3] * this->mMtx[1][1]) + rLookAt[2][3] * this->mMtx[1][2];
            this->ref(2, 3) = (rLookAt[0][3] * this->mMtx[2][0]) - (rLookAt[1][3] * this->mMtx[2][1]) + rLookAt[0][3] * this->mMtx[2][2];
        }

        void setQT(const TQuat4f &rSrcQuat, const TVec3f &rSrcTrans);

        inline void getTransInline(TVec3f &rDest) const {
            f32 z = this->mMtx[2][3];
            f32 y = this->mMtx[1][3];
            f32 x = this->mMtx[0][3];
            rDest.set(x, y, z);
        }
    };

    typedef TMatrix34<TSMtxf> TMtx34f;
    typedef TRotation3<TMtx34f> TRot3f;
    typedef TPosition3<TMtx34f> TPos3f;
}

#endif
