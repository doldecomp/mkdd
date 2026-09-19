#ifndef CRSDATA_H
#define CRSDATA_H

#include "JSystem/JUtility/TColor.h"
#include "Sato/stMath.h"
//#include "JSystem/JGeometry.h"

#include "kartEnums.h"
#include "kartLocale.h"
#include "types.h"

class Course; // Forward Declaration

// TODO: refactor all of these structs to use mVarName naming(or don't)

class CrsData
{
public:
    struct SArea
    {
        JGeometry::TVec3f pos;
        JGeometry::TVec3f scale;
        JGeometry::TVec3<s16> zNorm;
        JGeometry::TVec3<s16> yNorm;
        u8 shape; // 1 is cylinder?
        u8 kind;
        s16 cameraId;
        int feather1;
        int feather2;
        s16 echoScale;
        s16 _32;
        s16 shadowId;
        s16 lightParamId;
    };

    struct SCheckPoint
    {
        JGeometry::TVec3f cpStart;
        JGeometry::TVec3f cpEnd;
        u8 mShortcutID;
        u8 _0x19;
        bool revSearchEnable;
        u8 _0x1b;
    }; // Size: 0x1c

    struct SGround
    {
        int pointIndices[3];
        f32 dir;
        JGeometry::TVec3<s16> normDir;
        u8 attribute;
        u8 attrIdx;
        u8 zMaxIndex : 2;
        u8 xMaxIndex : 2;
        u8 zMinIndex : 2;
        u8 xMinIndex : 2;
        u8 camCode;
        u16 nearTrigs[3];
        u8 thickness : 4; // 20.0
        u8 itemType : 4;  // 20.4
        u8 _21; // unused?
        u8 spiralCode : 4; // 22.0
        u8 staggerCode : 4; // 22.4
        u8 splashCode;
    };

    struct SMaterial {
        u8 colFlag;
        u8 soundID;
        u16 _2;
        u32 _4;
        u32 _8;
    };

    struct SStartPoint {

    };

    struct SLightParam {

    };

    struct SColHeader
    {
        //char version[4];
        u32 version;
        u16 xGridNum;
        u16 zGridNum;
        int xStart; // TVec2?
        int zStart;
        u32 xSize;
        u32 zSize;
        u16 matNum;
        u32 indexTableOffset;
        u32 groundTableOffset;
        u32 vertexTableOffset;
        u32 matTableOffset;
    };

    struct SColInfoSphere 
    {
        JGeometry::TVec3f _0;
        JGeometry::TVec3f _c;
        f32 d;
        // TODO
    };

    struct SOblHeader
    {
        //char version[4];
        u32 version;
        u8 shaking;
        u8 ambientColor[3];
        JUtility::TColor lightColor; // u8 array/indiviudal members?
        JGeometry::TVec3f lightOffsetPos; // probably Vec type
        u8 totalLapNum;
        u8 courseID;
        u16 clPointNum;
        u16 checkPathNum;
        u16 objEntries;
        u16 areaNum;
        u16 cameraNum;
        u16 pathNum;
        u16 jugemPointNum;
        u8 fogType;
        u8 fogColor[3];
        f32 fogStartZ;
        f32 fogEndZ;
        u8 texLodBiasOn;
        u8 dummyStartLineOn;
        u8 snowEnabled;
        u8 shadowDepth;
        u8 shadowColor[3];
        u8 startPointNum;
        u8 skyMode;
        u8 lightParamNum;
        u8 mgParamNum;
        u32 fileStart;
        u32 clPointOffset;
        u32 checkPathOffset;
        u32 pathOffset;
        u32 pointOffset;
        u32 objectsOffset;
        u32 startPointOffset;
        u32 areaOffset;
        u32 cameraOffset;
        u32 jugemPointOffset;
        u32 lightParamOffset;
        u32 mgParamOffset;
        u8 _70[0xc];
    };

    struct SMatData {
        char name[16];
    };

    // https://mkdd.miraheze.org/wiki/mObjectData_(File_Format)#Section_1
    struct SCLPoint
    {
        //SCLPoint() : mPos() { }

        JGeometry::TVec3f pos;
        u16 driftDir; 
        s16 groupID; 
        f32 scale;
        char swerve;
        u8 groupSetting;
        u8 uniqueGroupID;
        u8 driftAcuteness;
        u8 driftDuration;
        u8 driftSupplement;
        u8 isNoMushroomZone;
        u8 pad[5];
    }; // Size: 0x20

    struct SObject
    {
        bool isAvailableObject();
        bool isSameProcLevel(int);

        JGeometry::TVec3f position;  // 0
        JGeometry::TVec3f scale;     // C
        JGeometry::TVec3<s16> zNorm; // 18
        JGeometry::TVec3<s16> yNorm; // 1e
        u16 objID;
        u16 mPathID;                 // 26
        u16 _28;                     //
        u16 pointIdx;                // 2a
        struct {
            u8 filter;               // 2c
            u8 flag;                 // 2d,  0x00 = all game modes disabled, 0x01 = single screen, 0x02 = split screen, 0x03 = all game modes.
        } presence;
        u8 colFlag;                  // 2e
        u8 _2f;                      //
        s16 mParam1;                 // 30
        s16 mParam2;                 // 32
        s16 mParam3;                 // 34
        s16 mParam4;                 // 36
        s16 mParam5;                 // 38, official name from assert(so ig everything in here should have the m naming scheme... not now)
        s16 mParam6;                 // 3a
        s16 mParam7;                 // 3c
        s16 mParam8;                 // 3e, amount of robbery karts
    };

    struct SJugemPoint
    { // mostly copied from https://mkdd.miraheze.org/wiki/mObjectData_(File_Format) (Section 9: Respawn Points)
        JGeometry::TVec3f position;
        s16 dirX; // TVec3?
        s16 dirY;
        s16 dirZ;
        s16 _12[3]; // seems to be padding
        u16 id;
        u16 cPointID;
        s16 cameraID;
        s16 sectorID;
    };


    struct CheckPath 
    {
        int getELinkNum() const;

        bool isMainPath() const { return !(link & 0x8000) ; }

        u16 pointCount;
        u16 link;
        u16 prev[4]; // The indices of up to 4 group links that follow to this one. Unneeded slots are set to value -1.
        s16 next[4]; // The indices of up to 4 group links to follow after this one. Unneeded slots are set to value -1.
    };

    struct Camera
    {
        void getFrontDirection(JGeometry::TVec3f *out) const;

        void getPosition(JGeometry::TVec3f *out) const { out->set(mPos); }
        void getTargetPos(JGeometry::TVec3f *out) const { out->set(mTargetPos); }
        void getTargetPos2(JGeometry::TVec3f *out) const { out->set(mTargetPos2); }

        f32 getFovy() const { return mFovY; }
        f32 getFovy2() const { return mFovY2; }
        u32 getKind() const { return mKind; }
        s16 getPathSpeed() const { return mPathSpeed; }
        s16 getPathID() const { return mPathID; }
        int getTime() const { return mTime; }
        int getNextCameraID() const { return mNextID; }
        int getShimmerZ0() const { return mShimmerZ0; }
        int getShimmerZ1() const { return mShimmerZ1; }
        bool isStartCamera() const { return mID != 0; }
    
        JGeometry::TVec3f mPos;
        JGeometry::TVec3<s16> mDir;
        u8 _12[0x18 - 0x12]; // padding?
        JGeometry::TVec3f mTargetPos2;
        JGeometry::TVec3f mTargetPos;
        u16 mKind;
        u16 mFovY; // start
        s16 mTime;
        u16 mID;
        s16 mShimmerZ0;
        s16 mShimmerZ1;
        s16 mPathID;
        s16 mPathSpeed;
        u16 mFovY2; // end
        s16 mNextID;
        u32 mName;
    }; // 48

    class Ground : public SGround
    {
    public:
        bool checkPosition(stPlaneParam *, const JGeometry::TVec3f &, const JGeometry::TVec3f &, const JGeometry::TVec3f *) const;                                                         // 0x8019fb34
        f32 checkWallBySphere(JGeometry::TVec3f *, const SColInfoSphere &, const JGeometry::TVec3f *, float *, JGeometry::TVec3f *, JGeometry::TVec3f *) const;                            // 0x801a0078
        void getNearPoint(const SColInfoSphere &, float, const JGeometry::TVec3f &, const JGeometry::TVec3f &, const JGeometry::TVec3f &, JGeometry::TVec3f *, JGeometry::TVec3f *) const; // 0x801a02fc
        bool checkFront(const CrsData *, const JGeometry::TVec3f &) const;                                                                                                                 // 0x801a0770
        bool checkFaceSide(const CrsData *, const JGeometry::TVec3f &) const;                                                                                                              // 0x801a083c
        f32 getPlaneY(const JGeometry::TVec3f &, Course *) const;                                                                                                                         // 0x801a097c
        u32 checkPolygonTouchBySphere(const JGeometry::TVec3f *, const JGeometry::TVec3f &, float, Course *) const;                                                                       // 0x801a0ae4
        u32 getTouchState(const SColInfoSphere &, const JGeometry::TVec3f &, const JGeometry::TVec3f &, JGeometry::TVec3f *, float *, float) const;                                       // 0x801a0c5c
        void getCenter(JGeometry::TVec3f *, const JGeometry::TVec3f *, Course *) const;                                                                                                    // 0x801a0f64
        // Inline
        int getZmaxIndex() const { return zMaxIndex; } // 0x801a0034
        int getXmaxIndex() const { return xMaxIndex; } // 0x801a0040
        int getZminIndex() const { return zMinIndex; } // 0x801a004c
        int getXminIndex() const { return xMinIndex; } // 0x801a0058
        u8 getAddThickness() const { return thickness; } // 0x801a0064
    };

    class Grid {
    public:
        f32 checkPolygonCoverWall(const JGeometry::TVec3f &, const SColInfoSphere &, const Ground *);                                                                                     // 0x801a0590
        Ground *searchGround(stPlaneParam *, const JGeometry::TVec3f &, const JGeometry::TVec3f &, const Ground *, const u16 *, const SGround *, const JGeometry::TVec3f *, u32, bool);
        Ground *checkPolygonBySphere(JGeometry::TVec3f*, f32 *, const CrsData *, const SColInfoSphere &, f32 *);
    
        u8 mCount;
        u16 _2;
        int mGroundIndex;
    };

    struct LightParam {
        void getLightColor(JUTColor *out) const { out->set(mLightColor.r, mLightColor.g, mLightColor.b, mLightColor.a); }
        void getLightOffsetPosition(JGeometry::TVec3f *out) const { out->set(mPos.x, mPos.y, mPos.z); }
        void getAmbientColor(JUTColor *out) const { out->set(mAmbientColor.r, mAmbientColor.g, mAmbientColor.b, 0);  }
        
        struct { 
            u8 r, g, b, a;
        } mLightColor;
        Vec mPos;
        struct { 
            u8 r, g, b;
        } mAmbientColor;
    };

    struct MGParam // Minigame Parameters
    {
        s16 getRabbitWinSec() const { return rabbitWinSec; }
        s16 getRabbitMinSec() const { return rabbitMinSec; }
        s16 getRabbitDecSec() const { return rabbitDecSec; }

        s16 rabbitWinSec;
        s16 rabbitMinSec;
        s16 rabbitDecSec;
        s16 _6; // probably padding
    };

    struct PathData
    {
        int getPointNumber() const { return pointCount; }
        bool isClosed() const { return flags & 1;}
        u16 pointCount;
        u16 pointStart;
        u16 unk;
        u16 flags;
        u8 pad[0x8];
    };

    struct PointData
    {
        JGeometry::TVec3f pos;
        s32 linkPoint;
        u8 pad[0x10];
    };
    

    class StartPoint {
    public:
        void getLinePosition(JGeometry::TVec3f *) const; // 0x801a1260

        bool isRight() const { return mOrientation != 0; };
        f32 getJugemOffsetY() const { return mJugemOffsetY; };

        void getPosition(JGeometry::TVec3f *dst) const
        {
            dst->set(mPos);
        }
        void getFrDirection(JGeometry::TVec3f *vec) const
        {
            vec->x = mRot.x * 0.0001f;
            vec->y = 0.0f;
            vec->z = mRot.z * 0.0001f;
            vec->normalize();
        }
        JGeometry::TVec3f mPos;
        JGeometry::TVec3f mScale;
        JGeometry::TVec3<s16> mRot;
        u8 _1E[0x24 - 0x1E];
        u8 mOrientation;
        u8 mPlayerID;
        s16 mJugemOffsetY;
    };

    CrsData(SColHeader *, SOblHeader *);
    bool isAvailableObj() const;
    bool isAvailableCol() const { return  mCollionData != nullptr; }
    void getAmbientColor(JUtility::TColor *) const;
    void getLightColor(JUtility::TColor *) const;
    void getShadowColor(JUtility::TColor *) const;
    void getLightOffsetPosition(JGeometry::TVec3f *) const;
    PathData *getPathData(u16 pathIdx) const;
    PointData *getPointData(u16, u16) const;
    void patchCamDataForStaffRoll(Language, VideoMode);
    SMaterial *searchSameAttr(u8, u8) const;
    u16 searchMaterial(u8, u8) const;
    int getFogType() const;
    f32 getFogStartZ() const;
    f32 getFogEndZ() const;
    void getFogColor(JUtility::TColor *color) const;
    StartPoint *getStartPoint(u8) const;
    int getStartCameraID() const;
    int getStartLoserCameraID() const;
    int searchCameraID(u32 camTag) const;
    f32 getXGridMin() const { return mCollionData->xStart; }
    f32 getZGridMin() const { return mCollionData->zStart; }
    f32 getXGridWidth() const { return mCollionData->xSize; }
    f32 getZGridWidth() const { return mCollionData->zSize; }
    int getXGridNum() const { return mCollionData->xGridNum; }
    int getZGridNum() const { return mCollionData->zGridNum; }
    int getMatNum() const { return mCollionData->matNum; }
    ECourseID getCourseID() const { return (ECourseID)mObjectData->courseID; }
    u8 getTotalLapNumber() const { return mObjectData->totalLapNum; }
    bool isTexLODBiasOn() const { return mObjectData->texLodBiasOn != 0; }
    u8 getShadowDepth() const { return mObjectData->shadowDepth; }
    u8 getShaking() const { return mObjectData->shaking; }
    int getAreaNumber() const { return mObjectData->areaNum; }
    int getCameraNumber() const { return mObjectData->cameraNum; }
    int getCheckPathNumber() const { return mObjectData->checkPathNum; }
    int getCLPointNumber() const { return mObjectData->clPointNum; }
    int getJugemPointNumber() const { return mObjectData->jugemPointNum; }
    int getLightParamNumber() const { return mObjectData->lightParamNum; }
    int getMGParamNumber() const { return mObjectData->mgParamNum; }    
    int getPathNumber() const { return mObjectData->pathNum; }
    int getStartPointNumber() const { return mObjectData->startPointNum; }
    bool isDummyStartLineOn() const  { return mObjectData->dummyStartLineOn != 0; } 
    bool isSkyFollow() const { return mObjectData->skyMode != 0; }
    bool isShaking() const { return mObjectData->shaking != 0; }

    LightParam *getLightParam(s16) const;
    MGParam *getMGParam(int) const;
    int getCheckPointOffset(int n) const;

    SCheckPoint *getCheckPointTable() const;
    Camera *getCameraData(int camIdx) const;

    // Unused
    const char *getMatTableLabel(int matNo) const;

    Grid *getGridTable() const { return (Grid*)((u8 *)mCollionData + sizeof(SColHeader));  }
    SMaterial *getMatTable() const { return (SMaterial*)((u8 *)mCollionData + mCollionData->matTableOffset); }
    Ground *getGroundTable() const { return (Ground*)((u8 *)mCollionData + mCollionData->groundTableOffset); }
    JGeometry::TVec3f *getVertexTable() const { return (JGeometry::TVec3f *)((u8 *)mCollionData + mCollionData->vertexTableOffset); }
    u16 *getIndexTable() const { return (u16*)((u8 *)mCollionData + mCollionData->indexTableOffset); }
    
    StartPoint* getStartPointTable() const { return (StartPoint*)((u8 *)mObjectData + mObjectData->startPointOffset); }
    SCLPoint *getCLPointTable() const { return (SCLPoint *)(((u8 *)mObjectData) + mObjectData->clPointOffset); }
    SArea *getAreaTable() const { return (SArea *)(((u8 *)mObjectData) + mObjectData->areaOffset); }
    CheckPath *getCheckPathTable() const { return (CheckPath *)(((u8 *)mObjectData) + mObjectData->checkPathOffset); }
    SJugemPoint *getJugemPointTable() const { return (SJugemPoint *)(((u8 *)mObjectData) + mObjectData->jugemPointOffset); }
    LightParam *getLightParamTable() const { return (LightParam *)((u8 *)mObjectData + mObjectData->lightParamOffset); }
    PathData *getPathTable() const { return (PathData *)((u8 *)mObjectData + mObjectData->pathOffset); }
    PointData *getPointTable() const { return (PointData *)((u8 *)mObjectData + mObjectData->pointOffset); }
    MGParam *getMGParamTable() const { return (MGParam *)((u8 *)mObjectData + mObjectData->mgParamOffset); }


    static const u32 cPatchCamTagTable[];
    static const s16 cPatchCamTableNtscE[];
    static const s16 cPatchCamTablePal[];
private:
    SColHeader *mCollionData;
    SOblHeader *mObjectData;
    SMatData *mMatData;
};

#endif
