#ifndef COURSE_H
#define COURSE_H

#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JUtility/JUTAssert.h"

#include "Kaneshige/Course/AIPath.h"
#include "Kaneshige/CenterLine.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/ExModel.h"
#include "Kaneshige/JugemPoint.h"

#include "Osako/ResMgr.h"
#include "Sato/J3DAnmObject.h"
#include "Sato/stMath.h"

#include "kartEnums.h"
#include "types.h"

class CrsArea;

enum CrsAreaKind
{
    // TODO
    cCrsAreaKindShadow
};

class Course
{
public:
    class Area
    {
    public:
        Area()
        {
            mData = nullptr;
            mPos.zero();
            mZNorm.set(0.0f, 0.0f, 1.0f);
            mYNorm.set(0.0f, 1.0f, 0.0f);
            mXNorm.set(1.0f, 0.0f, 0.0f);
            mScale.setAll(100.0f);
        }

        void init(const CrsData::SArea *area)
        {
            mData = area;
            reset();
        }

        void reset();
        f32 getRate(const JGeometry::TVec3f &pos);
        void drawDebugModel(u32 cameraNo);
        bool check(JGeometry::TVec3f *out, const JGeometry::TVec3f &pos);

        u8 getKind() const { return mData->kind; } // enum
        f32 getEchoScale() const { return mEchoScale; }
        s16 getLightParamID() const { return mData->lightParamId; };
        s16 getShadowID() const { return mData->shadowId; };
        u8 getShape() const { return mData->shape; }

    public:
        const CrsData::SArea *mData;
        JGeometry::TVec3f mPos;
        JGeometry::TVec3f mZNorm;
        JGeometry::TVec3f mYNorm;
        JGeometry::TVec3f mXNorm;
        JGeometry::TVec3f mScale;
        f32 mFeather;    // stores 0.01f * feather value 2 of BOL
        f32 mFeatherInv; // stores the inverse of 0.01f * feather value 1 of BOL
        f32 mEchoScale;
    }; // size: 0x4c

    class Sector
    {
    public:
        Sector();                                                                            // 0x8019dcd0
        void setPointData(const CrsData::SCheckPoint *, const CrsData::SCheckPoint *, bool); // 0x8019ddf4
        void setPrevSector(Sector *);                                                        // 0x8019e0ac
        void setNextSector(Sector *);                                                        // 0x8019e18c
        void calcGeneration(Sector *sector);                                                 // 0x8019e26c
        f32 calcUnitDist(const JGeometry::TVec3f &);                                         // 0x8019e350
        bool isLRInside(const JGeometry::TVec3f &);                                          // 0x8019e434
        f32 calcXZVecCrossRate(const JGeometry::TVec3f &, const JGeometry::TVec3f &);
        // Inline Functions
        int getGeneration() const { return mGeneration; }
        int getNextNum() const { return mNextNum; }
        bool isMainSector() const { return mIsMainSector; }
        int getPrevNum() const { return mPrevNum; }
        Sector *getMainSector() { return mMainSector; }
        s16 getID() const { return mID; };
        u32 getShortcutID() const { return mBPoint->mShortcutID; }
        void setDivFlag() { mDivFlag = true; } // 0x80199f94;
        void setID(s16 id) { mID = id; }       // 0x80199fa0

        Sector *getPrevSector(int no)
        {
#line 100
            JUT_MINMAX_ASSERT(0, no, 4);
            return mPrevSectors[no];
        }

        Sector *getNextSector(int no)
        {
#line 105
            JUT_MINMAX_ASSERT(0, no, 4);
            return mNextSectors[no];
        }

        bool isDiv() const { return mDivFlag; }
        bool isRevSearchEnable() const { return mBPoint->revSearchEnable != 0; }
        f32 getTotalPriorDist() const { return mTotalPriorDist; }
        f32 getSectorDist(void) const { return mSectorDist; }
        bool isInvalid() const { return mBPoint->_0x19 != 0; }

        void getBNormal(JGeometry::TVec3f *out)
        {
            out->set(mBDir.x, 0.0f, mBDir.y);
            out->normalize();
        }

        s16 mID;
        s16 mGeneration;
        bool mIsMainSector;
        bool mDivFlag;
        const CrsData::SCheckPoint *mBPoint;
        const CrsData::SCheckPoint *mFPoint;
        JGeometry::TVec3f _10[4];
        JGeometry::TVec3f _40[4];
        JGeometry::TVec2f mBDir;
        JGeometry::TVec2f mFDir;
        f32 mSectorDist;
        s16 mPrevNum;
        s16 mNextNum;
        Sector *mPrevSectors[4];
        Sector *mNextSectors[4];
        Sector *mMainSector;
        f32 mTotalPriorDist;
    };

    Course(CrsData *, int, bool);                                                       // 0x80198cd4
    void reset();                                                                       // 0x8019922c
    ~Course();                                                                          // UNUSED
    void loadCrsData(bool);                                                             // 0x801996c8
    void createSector();                                                                // 0x8019995c
    void createJugemPoint();                                                            // 0x80199fcc
    void setLinkJugemPoint(CLPoint *, u16);                                             // 0x8019a098
    JugemPoint *searchJugemPoint(u16);                                                  // 0x8019a134
    Sector *searchSector(s16);                                                          // 0x8019a20c
    void createCLPoint(bool);                                                           // 0x8019a294
    CLPoint *getFirstCLPoint(int) const;                                                // 0x8019aef0
    void createArea();                                                                  // 0x8019afa0
    int getShadowAreaNumber();                                                          // UNUSED
    int getShadowArea(int);                                                            // UNUSED
    void setFogInfo(J3DModelData *);                                                    // 0x8019b20c
    void setFogInfo(ExModel *);                                                         // 0x8019b24c
    void loadModelData();                                                               // 0x8019b2a4
    static bool searchMaterial(u32, u32 *, s16);                                        // 0x8019bcf8
    static bool isHighPriority(const char *);                                           // 0x8019bd30
    void createModel(JKRSolidHeap *, u32);                                              // 0x8019be10
    void calcShaking();                                                                 // 0x8019be14
    void calc();                                                                        // 0x8019c400
    void convWorldPosToMapCoord(JGeometry::TVec3f *, const JGeometry::TVec3f &);        // 0x8019cafc
    void convWorldPosToGridCoord(JGeometry::TVec3f *, const JGeometry::TVec3f &);       // 0x8019cb90
    void convMapCoordToWorldPos(JGeometry::TVec3f *, const JGeometry::TVec3f &);        // 0x8019cca0
    void calcGroundNormal(JGeometry::TVec3f *, const JGeometry::TVec3f &);              // 0x8019cd34
    void calcPlaneParam(stPlaneParam *, f32, f32, f32, f32, const JGeometry::TVec3f &); // 0x8019cdc4
    void getShakeVelocity(JGeometry::TVec3f *, const JGeometry::TVec3f &);              // 0x8019ced0
    void update();                                                                      // 0x8019cf94
    void updateSky();                                                                   // 0x8019cfc4
    void drawBase(u32, u32 *, s16, bool);                                               // 0x8019d028
    void drawSky(u32);                                                                  // 0x8019d6ac
    void getStartLine(JGeometry::TVec3f *, JGeometry::TVec3f *);                        // 0x8019d7e8
    void getLightColor(JUTColor *);                                             // 0x8019d914
    void getLightOffsetPosition(JGeometry::TVec3f *);                                   // 0x8019d948
    void getAmbientColor(JUTColor *);                                           // 0x8019d97c
    u32 getCourseBGM();                                                                 // 0x8019d9b0
    u32 getFinalLapBGM();                                                               // 0x8019db78
    void drawModelForDebug(u32);                                                        // 0x8019dc2c
    bool searchAppendGround(const JGeometry::TVec3f &);                                 // 0x8019ed04

    // Inline Functions
    ECourseID getCourseID() const { return mCrsID; }
    Sector *getStartSector() const { return &mSectors[0]; }
    f32 getTrackSectorDist() const { return mTrackSectorDist; }
    int getTotalSectorNumber() const { return mTotalSectors; }
    int getTrackSectorNumber() const { return mTrackSectorNum; }
    CrsData *getCrsData() const { return mCrsData; }

    u32 getAppendGroundAttr() const { return mAppendGndAttr; }
    u32 getAppendGroundAttrIndex() const { return mAppendGndAttrIdx; }
    f32 getAppendGroundHeight() const { return mAppendGndHeight; }

    Sector *getSector(int no) const
    {
#line 300
        JUT_MINMAX_ASSERT(0, no, mTotalSectors);
        return &mSectors[no];
    }

    Sector *getMainSector(int gen) const
    {
        JUT_MINMAX_ASSERT(0, gen, mTrackSectorNum);
        return mMainSectors[gen];
    }

    bool isTexSRTAnmEnable(int anmNo) const // 0x801995a8
    {
#line 414
        JUT_MINMAX_ASSERT(0, anmNo, 3);
        return mTexSRTKey[anmNo] != nullptr;
    }

    bool isTevRegAnmEnable(int anmNo) const // 0x80199494
    {
#line 422
        JUT_MINMAX_ASSERT(0, anmNo, 1);
        return mTevRegKey[anmNo] != nullptr;
    }
    bool isTexPatAnmEnable() const { return mTexPattern != nullptr; }; // 0x80199534

    bool isShaking() const { return getCrsData()->isShaking(); }
    void disableShaking() { mShakeEnabled = false; }

    int getAreaNumber() const { return mAreaNum; }
    Area *getArea(int no) const { return &mAreas[no]; }

    void drawNormal(u32 cameraNo) { drawBase(cameraNo, mNormMatIDTable, mNormMatNum, false); }
    void drawHigh(u32 cameraNo) { drawBase(cameraNo, mHighMatIDTable, mHighMatNum, false); }

    f32 getGroundThickness() const { return sGndThickness; }

    JGeometry::TVec3f *getDirZ() { return &mDirZ; };
    JGeometry::TVec3f *getDirY() { return &mDirY; };
    JGeometry::TVec3f *getDirX() { return &mDirX; };

private: // add all documented stuff from Ghidra
    static u16 sDrawAreaModel;
    static bool sForceDebugFogSetting;
    static bool sConfigFog;

    static s16 sShakeRippleTime;

    static const u8 cGXTexMtxLabel[2];
    static const ResMgr::CourseDataId cCourseTexSRTAnmTable[];
    static const ResMgr::CourseDataId cCourseTevRegAnmTable[];

    static bool sAddShakeVel;
    static f32 sBoundsScale;
    static f32 sGndThickness;
    static f32 sShakeMaxAngle;
    static s16 sShakeFirstRippleTime;
    static s16 sShakeRoughTime;
    static s16 sShakeKeepTime;
    static s16 sShakeRecoverTime;
    static s16 sShakeFirstRippleTimeDemo;
    static f32 sShakeMaxAngleMini;
    static s16 sShakeFirstRippleTimeMini;
    static s16 sShakeRippleTimeMini;
    static s16 sShakeRoughTimeMini;
    static s16 sShakeKeepTimeMini;
    static s16 sShakeRecoverTimeMini;

    ECourseID mCrsID;
    s16 _4;
    u8 mFogType;
    f32 mFogStartZ;
    f32 mFogEndZ;
    f32 mFogNearZ;
    f32 mFogFarZ;
    JUTColor mFogColor;
    ExModel mTexModel;
    J3DAnmTextureSRTKey *mTexSRTKey[3];
    J3DAnmObjMaterial mTexSRTAnm[3];
    J3DAnmTexPattern *mTexPattern;
    J3DAnmObjMaterial mTexPatAnm;
    J3DAnmTevRegKey *mTevRegKey[1];
    J3DAnmObjMaterial mTevRegAnm[1];
    J3DAnmTextureSRTKey *_15c; // 15c to 184 are sky related
    J3DAnmObjMaterial mSkyBtkMat;
    J3DAnmTevRegKey *_180;
    J3DAnmObjMaterial _184;
    JGeometry::TVec3f mZNorm;
    JGeometry::TVec3f mYNorm;
    JGeometry::TVec3f mXNorm;
    JGeometry::TVec3f mShakeZNorm;
    JGeometry::TVec3f mShakeYNorm;
    JGeometry::TVec3f mShakeXNorm;
    JGeometry::TVec3f mPos;
    JGeometry::TPos3f mTexMtx;
    JGeometry::TVec3f mDirZ;
    JGeometry::TVec3f mDirY;
    JGeometry::TVec3f mDirX;
    JGeometry::TPos3f mCamMtx;
    bool mShakeEnabled;
    s16 _27e;
    int mShakeState;
    f32 mShakeMaxAngle;
    s16 mShakeFirstRippleTime;
    s16 mShakeRippleTime;
    s16 mShakeRoughTime;
    s16 mShakeKeepTime;
    s16 mShakeRecoverTime;
    s16 mShakeTime;
    s16 mShakeAngleFlag;
    s16 _296; // probably padding
    u16 *mMatFlags;
    s16 mNormMatNum;
    s16 mHighMatNum;
    u32 *mNormMatIDTable;
    u32 *mHighMatIDTable;
    int mCameraCount;
    ExModel mSkyModel[4];
    CrsData *mCrsData; // 0x4DC
    int mJugemPointNum;
    JugemPoint *mJugemPoint;
    CLPoint *mCenterPoint;
    CLPoint *mCLPoints[8];
    s16 mTotalSectors; // 0x50C
    s16 mTrackSectorNum;
    Sector *mSectors;
    f32 mTrackSectorDist;
    Sector **mMainSectors;
    int mPathNum;
    AIPath *mAIPaths;
    int mAreaNum;
    Area *mAreas;
    u8 mSkyMode;
    bool mIsTextCourse25;
    u8 _052e[0x530 - 0x52e]; // padding?
    u32 mAppendGndAttr;
    u32 mAppendGndAttrIdx;
    f32 mAppendGndHeight;
}; // Size: 0x53C

#endif
