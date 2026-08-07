#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/Objects/GeoAirJet.h"
#include "Kaneshige/Objects/GeoBombCar.h"
#include "Kaneshige/Objects/GeoBus.h"
#include "Kaneshige/Objects/GeoCannon.h"
#include "Kaneshige/Objects/GeoCar.h"
#include "Kaneshige/Objects/GeoCarShadow.h"
#include "Kaneshige/Objects/GeoDkCloud.h"
#include "Kaneshige/Objects/GeoHanaBody.h"
#include "Kaneshige/Objects/GeoHanaCar.h"
#include "Kaneshige/Objects/GeoItemCar.h"
#include "Kaneshige/Objects/GeoItemGen.h"
#include "Kaneshige/Objects/GeoKinoCar.h"
#include "Kaneshige/Objects/GeoKinojii.h"
#include "Kaneshige/Objects/GeoKuriCar.h"
#include "Kaneshige/Objects/GeoKuribo.h"
#include "Kaneshige/Objects/GeoNormCar.h"
#include "Kaneshige/Objects/GeoPuller.h"
#include "Kaneshige/Objects/GeoRabbitMark.h"
#include "Kaneshige/Objects/GeoShimmer.h"
#include "Kaneshige/Objects/GeoShine.h"
#include "Kaneshige/Objects/GeoSignal.h"
#include "Kaneshige/Objects/GeoSoundGen.h"
#include "Kaneshige/Objects/GeoSplash.h"
#include "Kaneshige/Objects/GeoStartLine.h"
#include "Kaneshige/Objects/GeoTideSensor.h"
#include "Kaneshige/Objects/GeoTruck.h"
#include "Kaneshige/Objects/GeoVision.h"
#include "Kaneshige/Objects/GeoWater.h"
#include "Sato/GeographyObj.h"
#include "Sato/Objects/GeoItemBox.h"
#include "Sato/Objects/GeoJumpNarrow.h"
#include "Sato/Objects/GeoRFlower.h"
#include "Sato/Objects/GeoTree.h"
#include "Sato/StringObj.h"
#include "Shiraiwa/Balloon.h"
#include "Shiraiwa/JugemHeadItem.h"
#include "Shiraiwa/JugemItem.h"
#include "Shiraiwa/JugemMain.h"
#include "Shiraiwa/JugemRodItem.h"
#include "Shiraiwa/LensFlare.h"
#include "Shiraiwa/Objects/MapObjAward.h"
#include "Shiraiwa/Objects/MapObjBubble.h"
#include "Shiraiwa/Objects/MapObjDonkyRock.h"
#include "Shiraiwa/Objects/MapObjDonkyRockGen.h"
#include "Shiraiwa/Objects/MapObjDonkyTree.h"
#include "Shiraiwa/Objects/MapObjDossun.h"
#include "Shiraiwa/Objects/MapObjFountain.h"
#include "Shiraiwa/Objects/MapObjGeyser.h"
#include "Shiraiwa/Objects/MapObjHeyho.h"
#include "Shiraiwa/Objects/MapObjIceBlock.h"
#include "Shiraiwa/Objects/MapObjKpGear.h"
#include "Shiraiwa/Objects/MapObjNoMove.h"
#include "Shiraiwa/Objects/MapObjNossie.h"
#include "Shiraiwa/Objects/MapObjPeachTree.h"
#include "Shiraiwa/Objects/MapObjPoihana.h"
#include "Shiraiwa/Objects/MapObjPool.h"
#include "Shiraiwa/Objects/MapObjSkyShip.h"
#include "Shiraiwa/Objects/MapObjSnowMan.h"
#include "Shiraiwa/Objects/MapObjSnowRock.h"
#include "Shiraiwa/Objects/MapObjSun.h"
#include "Shiraiwa/Objects/MapObjWall.h"
#include "Shiraiwa/Objects/MapObjWanwan.h"
#include "Shiraiwa/Objects/MapObjWluArrow.h"
#include "Shiraiwa/Objects/MapObjYoshiHeli.h"
#include "Shiraiwa/TestRollObj.h"
#include "Shiraiwa/TestString.h"
#include "Sato/GeographyObjMgr.h"

#define GEO_OBJ_NEW(GeoClassName) \
template<> GeoClassName *GeographyObj::New<GeoClassName>(const CrsData::SObject &sObject) { \
    return new GeoClassName(sObject); \
}

#define GEO_OBJ_NEW_S(GeoClassName) \
template<> GeoClassName *GeographyObj::NewS<GeoClassName>(const CrsData::SObject &sObject) { \
    return new GeoClassName(); \
}

#define EX_GEO_OBJ_NEW(ExGeoClassName) \
template<> ExGeoClassName *ExGeographyObj::ExNew<ExGeoClassName>(const CrsData::SObject &sObject) { \
    return new ExGeoClassName(sObject); \
}

#pragma region GeoObj Definitions

GEO_OBJ_NEW_S(GeoItemBox);
GEO_OBJ_NEW_S(GeoDoubleItemBox);
GEO_OBJ_NEW(GeoF_ItemBox);
GEO_OBJ_NEW_S(GeoStartLine);
GEO_OBJ_NEW(GeoMarioTree1);
GEO_OBJ_NEW(GeoMarioKinoko1);
GEO_OBJ_NEW(GeoMarioFlower1);
GEO_OBJ_NEW(TMapObjDossun);
GEO_OBJ_NEW(GeoNormCar);
GEO_OBJ_NEW_S(GeoCarShadow);
GEO_OBJ_NEW_S(GeoBombShadow);
GEO_OBJ_NEW_S(GeoKinoShadow);
GEO_OBJ_NEW_S(GeoHanaShadow);
GEO_OBJ_NEW_S(GeoHanaBodyShadow);
GEO_OBJ_NEW(GeoTruck);
GEO_OBJ_NEW(GeoBus);
GEO_OBJ_NEW(GeoBombCar);
GEO_OBJ_NEW(GeoKinoCar);
GEO_OBJ_NEW(GeoKuriCar);
GEO_OBJ_NEW(GeoItemCar);
GEO_OBJ_NEW(GeoHanaCar);
GEO_OBJ_NEW_S(GeoHanaBody);
GEO_OBJ_NEW_S(TJugem);
GEO_OBJ_NEW_S(TJugemVoidRod);
GEO_OBJ_NEW_S(TJugemHeadHear);
GEO_OBJ_NEW_S(TJugemRodBoard);
GEO_OBJ_NEW_S(TJugemRodBoard2);
GEO_OBJ_NEW_S(TJugemRodBoardRev);
GEO_OBJ_NEW_S(TJugemRodSignal);
GEO_OBJ_NEW_S(TJugemRodPukuPuku);
GEO_OBJ_NEW_S(TJugemFlag);
GEO_OBJ_NEW_S(TJugemHeadLamp);
GEO_OBJ_NEW(GeoSignal);
GEO_OBJ_NEW(GeoCannon);
GEO_OBJ_NEW(TMapObjWanwan);
GEO_OBJ_NEW_S(TMapObjWanwanChain);
GEO_OBJ_NEW_S(TMapObjWanwanPile);
GEO_OBJ_NEW(TMapObjSkyShip);
GEO_OBJ_NEW_S(TMapObjDonkyRock);
GEO_OBJ_NEW(TMapObjDonkyTree);
GEO_OBJ_NEW(GeoDkCloud);
GEO_OBJ_NEW_S(GeoWaterUp);
GEO_OBJ_NEW_S(GeoWaterUp2);
GEO_OBJ_NEW(GeoWater);
// GEO_OBJ_NEW(TMapObjFireBar);
GEO_OBJ_NEW(GeoVision);
GEO_OBJ_NEW(TMapObjPoihana);
GEO_OBJ_NEW(TMapObjWall);
// GEO_OBJ_NEW_S(TSMAButterfly);
// GEO_OBJ_NEW_S(TSMADonkyBird);
// GEO_OBJ_NEW_S(TSMAKamome);
GEO_OBJ_NEW(TMapObjGeyser);
GEO_OBJ_NEW(TMapObjGeyserSearch);
GEO_OBJ_NEW_S(TTestStrBarrel);
GEO_OBJ_NEW_S(TTestStrItem);
GEO_OBJ_NEW(TMapObjPeachTree);
GEO_OBJ_NEW(TMapObjSnowRock);
GEO_OBJ_NEW(TTestRollObj);
GEO_OBJ_NEW(TMapObjFountain);
GEO_OBJ_NEW(TMapObjHeyho);
GEO_OBJ_NEW_S(TMapObjSubHeyho);
// GEO_OBJ_NEW(TMapObjSanbo);
GEO_OBJ_NEW(TMapObjWlArrow);
GEO_OBJ_NEW(TMapObjSnowMan);
GEO_OBJ_NEW(TMapObjIceBlock);
GEO_OBJ_NEW(TMapObjNoMove);
GEO_OBJ_NEW(GeoKuribo);
GEO_OBJ_NEW(TMapObjSun);
GEO_OBJ_NEW(TMapObjBubble);
// GEO_OBJ_NEW(TMapObjDesun);
// GEO_OBJ_NEW(TMapObjSandPillar);
// GEO_OBJ_NEW(TMapObjFerriswheel);
// GEO_OBJ_NEW(TMapObjMoveItemBoxLimit);
GEO_OBJ_NEW_S(GeoShine);
GEO_OBJ_NEW_S(TBalloon);
// GEO_OBJ_NEW(TMapObjJetCoasterHead);
// GEO_OBJ_NEW_S(TMapObjJetCoasterBody);
GEO_OBJ_NEW(TMapObjPool);
// GEO_OBJ_NEW(TMapObjDeBalloon);
// GEO_OBJ_NEW(TMapObjJetKpFire);
// GEO_OBJ_NEW_S(TMapObjSanboHead);
// GEO_OBJ_NEW_S(TMapObjSanboBody);
// GEO_OBJ_NEW(TMapObjRollTable);
// GEO_OBJ_NEW(TMapObjMoveItemBox);
GEO_OBJ_NEW(TMapObjNossie);
// GEO_OBJ_NEW(TMapObjMareA);
// GEO_OBJ_NEW(TMapObjMareB);
// GEO_OBJ_NEW(TMapObjMareC);
// GEO_OBJ_NEW(TMapObjMonteA);
// GEO_OBJ_NEW(TMapObjMonteB);
// GEO_OBJ_NEW(TMapObjMonteC);
// GEO_OBJ_NEW(TMapObjMareW_A);
// GEO_OBJ_NEW(TMapObjMareW_B);
// GEO_OBJ_NEW(TMapObjMareW_C);
// GEO_OBJ_NEW(TMapObjMareM_A);
// GEO_OBJ_NEW(TMapObjMeteor);
// GEO_OBJ_NEW(TMapObjUkeleleMonte);
// GEO_OBJ_NEW(TMapObjDanceMonteA);
// GEO_OBJ_NEW(TMapObjDanceMonteB);
GEO_OBJ_NEW(TMapObjAwardItemBox);
// GEO_OBJ_NEW_S(TMapObjAwardCupKinoko);
// GEO_OBJ_NEW_S(TMapObjAwardCupFlower);
// GEO_OBJ_NEW_S(TMapObjAwardCupStar);
// GEO_OBJ_NEW_S(TMapObjAwardCupSpecial);
// GEO_OBJ_NEW_S(TMapObjAwardCupReverse);
// GEO_OBJ_NEW_S(TMapObjAwardCupAllTour);
GEO_OBJ_NEW(TMapObjAntLion);
// GEO_OBJ_NEW(TMapObjPakkun);
GEO_OBJ_NEW(GeoKinojii);
GEO_OBJ_NEW(TMapObjDinoTree);
GEO_OBJ_NEW(TMapObjDemoFountain);
GEO_OBJ_NEW(TMapObjKpGear);
// GEO_OBJ_NEW(TMapObjKpFireBar);
GEO_OBJ_NEW(TMapObjSwimNossie);
GEO_OBJ_NEW(TMapObjPtera);
GEO_OBJ_NEW(TMapObjPeachTree2);
GEO_OBJ_NEW(TMapObjYoshiHeli);
GEO_OBJ_NEW(TMapObjDesertTree);
GEO_OBJ_NEW(TMapObjWluSearchLight);
// GEO_OBJ_NEW(TMapObjVentilator);
// GEO_OBJ_NEW_S(TShineArrow);
GEO_OBJ_NEW(TMapObjUFO);
GEO_OBJ_NEW(TMapObjMashBalloon);
GEO_OBJ_NEW(TMapObjPathYoshiHeli);
// GEO_OBJ_NEW(TMapObjFireBall);
GEO_OBJ_NEW_S(GeoWaterDown);
GEO_OBJ_NEW_S(GeoWaterMiddleD);
GEO_OBJ_NEW(TMapObjDonkyRockGen);
// GEO_OBJ_NEW(TMapObjFireCircle);
GEO_OBJ_NEW(GeoItemGen);
// GEO_OBJ_NEW(TSMAButterflyGen);
// GEO_OBJ_NEW(TSMADonkyBirdGen);
// GEO_OBJ_NEW(TSMABoidBirdManager);
GEO_OBJ_NEW(TTestString);
GEO_OBJ_NEW_S(TBalloonString);
GEO_OBJ_NEW(TLensFlare);
// GEO_OBJ_NEW(TDistSnow);
GEO_OBJ_NEW_S(TBalloonManager);
GEO_OBJ_NEW(GeoAirJet);
GEO_OBJ_NEW_S(TMapObjNossieColHead);
GEO_OBJ_NEW_S(TMapObjNossieColFootFront);
GEO_OBJ_NEW_S(TMapObjNossieColFootRear);
GEO_OBJ_NEW_S(TMapObjNossieColBody);
GEO_OBJ_NEW(GeoSplash);
GEO_OBJ_NEW(GeoTideSensor);
GEO_OBJ_NEW(GeoPuller);
GEO_OBJ_NEW(GeoShimmer);
// GEO_OBJ_NEW_S(TMapObjPakkunColHead);
GEO_OBJ_NEW(GeoSoundGen);
GEO_OBJ_NEW(GeoRabbitMark);
EX_GEO_OBJ_NEW(GeoJumpNarrow);
//EX_GEO_OBJ_NEW(StringBridge);


#pragma endregion
// GeoObj Definitions

void GeographyObjManager::getKartCollidePosRadius(JGeometry::TVec3f &kartCollidePos, f32 &kartCollideRadius, int p3) {
    kartCollidePos = mKartCollidePos[p3];
    kartCollideRadius = mKartCollideRadius[p3];
}
