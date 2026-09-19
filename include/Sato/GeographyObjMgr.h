#ifndef GEOGRAPHYOBJMGR_H
#define GEOGRAPHYOBJMGR_H

#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObj.h"
#include "Shiraiwa/Balloon.h"
#include "Shiraiwa/JugemMain.h"

class GeographyObjManager : JKRDisposer
{
public:
    GeographyObjManager(const CrsData &);

    void calc();
    void createModel(JKRSolidHeap *, u32);
    void reset(const CrsData &);
    void drawGXObj(u32 cameraNo);
    void drawPrim(u32 cameraNo);

    void update();
    void drawSimpleModel(u32 cameraNo);
    void setCurrentViewNo(u32 cameraNo);
    void viewCalc(u32 cameraNo);

    void update_groundObj();
    void drawSimpleModel_groundObj(u32 cameraNo);    
    void setCurrentViewNo_groundObj(u32 cameraNo);
    void viewCalc_groundObj(u32 cameraNo);

    void update_foreground();
    void drawSimpleModel_foreground(u32 cameraNo);    
    void setCurrentViewNo_foreground(u32 cameraNo);
    void viewCalc_foreground(u32 cameraNo);

    void setKartHitItemBoxTrigger(const int kart_index);

    void setNewItemBoxObj(GeographyObj *obj);

    TJugem *getJugem(int camera_index) const;
    TBalloonManager *getBalloonMgr(int kart_index) const;

    ExGeographyObj *search_boundEx(const JGeometry::TVec3f &pos, GeographyObj *obj);
    ExGeographyObj *search_boundSp(const JGeometry::TVec3f &pos, GeographyObj *obj);
    ExGeographyObj *search_boundSp(const JGeometry::TVec3f &pos, f32);

    GeographyObj *createSubObj(u32 id);
    GeographyObj *createGenSubObj(u32 id);
    GeographyObj *createPrevDrawSubObj(u32 id);

    f32 getKartHitDepthNormalObj(int kart_index) const { return mKartHitDepth[kart_index]; }
    static void createMgr(const CrsData &crsData) { gMgr = new GeographyObjManager(crsData); }
    static GeographyObjManager *getGeographyObjManager() { return gMgr; }

    bool isMiniGame() const { return mIsBalloonBattle || mIsBombBattle || mIsRobberyBattle || mIsEscapeBattle; }
    bool isRobberBattle() const { return mIsRobberyBattle; }
    bool isBalloonBattle() const { return mIsBalloonBattle; }
    bool isEscapeBattle() const { return mIsEscapeBattle; }
    bool isBombBattle() const { return mIsBombBattle; }

    void *getGeoHitTable() const { return mGeoHitTable; }


    inline GeographyObj **getKartReactHitObjectList(int myNum) {
        return &mKartHittingList[myNum][0];
    }
    inline const JGeometry::TVec3f *getKartHitRefVecNormalObj(int myNum) const {
        return &mKartHitRefVec[myNum];
    }

    void getKartCollidePosRadius(JGeometry::TVec3f &, f32 &, int);

    static GeographyObjManager *gMgr;

    u8 _18[0x10];               // 18
    GeographyObj *mKartHittingList[8][20]; // 28
    f32 mKartHitDepth[8];       // 2a8
    JGeometry::TVec3f mKartHitRefVec[8]; // 2c8
    u8 _328[0x348 - 0x328];     // 328
    bool mKartHitItemBox[8];    // 348
    u8 _350[0xAC];              // 350
    TJugem *mJugem[4];          // 3FC
    u8 _40C[0x410 - 0x40C];     // 40C
    JGeometry::TVec3f mKartCollidePos[8]; // 410
    f32 mKartCollideRadius[8];  // 470
    u8 _490;                    // 490
    bool mIsBalloonBattle;      // 491
    bool mIsBombBattle;         // 492
    bool mIsRobberyBattle;      // 493
    bool mIsEscapeBattle;       // 494
    u8 _495[0x49C - 0x495];     // 495
    void* mGeoHitTable;         // 49C
    u8 _4A0[0x4A4 - 0x4A0];     // 4A0

}; // size 0x4A4

inline GeographyObjManager *GetGeoObjMgr() { return GeographyObjManager::getGeographyObjManager(); };

inline void CreateGeoObjMgr(const CrsData *crsData) { GeographyObjManager::createMgr(*crsData); }

#endif
