#ifndef KARTPARAMS_H
#define KARTPARAMS_H

#include <JSystem/JGeometry/Vec.h>

#include "JSystem/JGeometry/Matrix.h"
#include "dolphin/mtx.h"
#include "types.h"

struct ArmOp
{
    f32 _0, _4, _8, _c;
}; // 10

struct BodyOp
{
    f32 _0, _4, _8, _c, _10, _14;
}; // 18

struct DumpOp
{
    f32 _0, _4, _8, _c, _10;
}; // 14

struct TireOp
{
    f32 _0, _4, _8, _c, _10, _14, _18, _1c, _20, _24, _28, _2c, _30;
}; // 34

struct CubeParam
{
    JGeometry::TVec3f _0[8];
}; // 60

struct TireParamData
{
    f32 _0, _4, _8, _c, _10, _14, _18, _1c, _20, _24;
}; // 28

struct BodyParamData 
{
    f32  _0,  _4,  _8,  _c, _10, _14, _18, _1c, 
        _20, _24, _28, _2c, _30, _34, _38, _3c, 
        _40, _44, _48, _4c, _50, _54, _58, _5c,
        _60, _64, _68, _6c, _70, _74, _78, _7c;
    f32 _80, _84, _88, _8c, _90, _94, _98, _9c; // MJB - These may not exist...
};


// Fit Params

// Unused
extern TireOp normalTireOp[4];
extern TireOp minilTireOp[4];
// extern UNK bigArmOp[4];
// extern UNK miniArmOp[4];
extern DumpOp miniDumpOp[4];
extern BodyOp miniBodyOp[4];
// extern UNK cubeNormalPos;
// extern UNK cubeMiniPos;

extern TireOp marioluiziTireOp[4];
extern TireOp biglTireOp[4];
extern TireOp KuppaTireOp[4];
extern TireOp peachTireOp[4];
extern TireOp yossyTireOp[4];
extern TireOp nokonokoTireOp[4];
extern TireOp babyTireOp[4];
extern TireOp warioTireOp[4];
extern TireOp kuppajrTireOp[4];
extern TireOp waluiziTireOp[4];
extern TireOp pakknTireOp[4];
extern TireOp didyTireOp[4];
extern TireOp kinoTireOp[4];
extern TireOp demolTireOp[4];
extern TireOp *TireOpData[21];
extern ArmOp normalArmOp[4];
extern ArmOp peachArmOp[4];
extern ArmOp kuppaArmOp[4];
extern ArmOp donkyArmOp[4];
extern ArmOp yossyArmOp[4];
extern ArmOp nokonokoArmOp[4];
extern ArmOp babyArmOp[4];
extern ArmOp warioArmOp[4];
extern ArmOp kuppajrArmOp[4];
extern ArmOp waluiziArmOp[4];
extern ArmOp pakknArmOp[4];
extern ArmOp didiyArmOp[4];
extern ArmOp kinopioArmOp[4];
extern ArmOp demoArmOp[4];
extern ArmOp *ArmOpData[21];
extern DumpOp normalDumpOp[4];
extern DumpOp bigDumpOp[4];
extern DumpOp peachDumpOp[4];
extern DumpOp kuppaDumpOp[4];
extern DumpOp yossyDumpOp[4];
extern DumpOp nokonokoDumpOp[4];
extern DumpOp babyDumpOp[4];
extern DumpOp warioDumpOp[4];
extern DumpOp kuppajrDumpOp[4];
extern DumpOp waluiziDumpOp[4];
extern DumpOp pakknDumpOp[4];
extern DumpOp demoDumpOp[4];
extern DumpOp didyDumpOp[4];
extern DumpOp *DumpOpData[21];
extern BodyOp marioBodyOp;
extern BodyOp bigBodyOp;
extern BodyOp peachBodyOp;
extern BodyOp nokonokoBodyOp;
extern BodyOp babyBodyOp;
extern BodyOp warioBodyOp;
extern BodyOp kuppaBodyOp;
extern BodyOp waluiziBodyOp;
extern BodyOp pakknBodyOp;
extern BodyOp didyBodyOp;
extern BodyOp kuppajrBodyOp;
extern BodyOp kinopioBodyOp;
extern BodyOp demoBodyOp;
extern BodyOp *BodyOpData[21];
// extern CubePos cubeMarioPos;
// extern CubePos cubeDonkyPos;
// extern CubePos cubeYossyPos;
// extern CubePos cubeHokoNokoPos;
// extern CubePos cubePeachPos;
// extern CubePos cubeBabyPos;
// extern CubePos cubeWarioPos;
// extern CubePos cubeKuppaPos;
// extern CubePos cubeMarioPos;
// extern CubePos didyMiniPos;
// extern CubePos cubeYossyPos;
// extern CubePos cubeNokoNokoPos;
// extern CubePos cubePeachPos;
// extern CubePos cubeBabyPos;
// extern CubePos cubeWluiziPos;
// extern CubePos cubeBabyPos;
// extern CubePos kinopioMiniPos;
// extern CubePos kinopioMiniPos;
// extern CubePos cubePukknPos;
// extern CubePos cubePukknPos;
// extern CubePos demoNormalPos;
extern CubeParam *CubeParamsData[21];
// extern UNK marioCamOp;
// extern UNK donkyCamOp;
// extern UNK yossyCamOp;
// extern UNK nokonokoCamOp;
// extern UNK peachCamOp;
// extern UNK babyCamOp;
// extern UNK waruCamOp;
// extern UNK kuppaCamOp;
// extern UNK CameraOpData;
// extern UNK RescueSeaDepth;
// extern UNK floorDepth;
// extern UNK floorThunderDepth;
// extern UNK bridgeDepth;
// extern UNK bridgeThunderDepth;
extern f32 tireOffsetPos[21];

// Other Params

//extern BodyParamData *normalBody;
//extern BodyParamData *bigBody;
//extern BodyParamData *miniBody;
//extern BodyParamData *warioBody;
//extern BodyParamData *kuppaBody;
//extern BodyParamData *kinoBody;
//extern BodyParamData *peachBody;
// extern UNK normalSus;
// extern UNK malioSus;
// extern UNK nokonokoSus;
// extern UNK peachSus;
// extern UNK kuppaSus;
// extern UNK warioSus;
// extern UNK babySus;
// extern UNK didySus;
// extern UNK kinopioSus;
// extern UNK demoSus;
// extern UNK normalTire;
// extern UNK Jdata;
extern f32 BodyFullHeightData[21];
extern f32 BodyFullBumpData[21];
extern f32 SusPowerData[21];
extern JGeometry::TVec2f ThunderOffet[21];
extern f32 SusDushDive[21];
extern f32 SetEngineClassData[3];
struct SusParam
{
    f32 _00[0x30 / 4];
    f32 _30;
    f32 _34;
    f32 _38;
    f32 _3c;
    f32 _40;
    f32 _44;
}; // 48

extern SusParam *SusParamsData[21];
extern TireParamData *TireParamsData[21];
extern BodyParamData *BodyParamsData[21];

#endif // KARTPARAMS_H
