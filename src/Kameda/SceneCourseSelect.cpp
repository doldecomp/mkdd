#include "Kameda/SceneCourseSelect.h"

#include "Kameda/Scene.h"
#include "kartEnums.h"
#include "mathHelper.h"

int SceneCourseSelect::mCup; // ERaceGpCup?
int SceneCourseSelect::mCourse; // ECourseID?

const ERaceGpCup SceneCourseSelect::mRaceGpCup[5] = {
    MUSHROOM_CUP, FLOWER_CUP, STAR_CUP, SPECIAL_CUP,
    ALL_CUP_TOUR
};

const char *SceneCourseSelect::mCupName[] = {
    "CupName_MUSHROOM_CUP.bti",
    "CupName_FLOWER_CUP.bti",
    "CupName_STAR_CUP.bti",
    "CupName_SPECIAL_CUP.bti",
    "CupName_REVERSE2_CUP.bti",
};

const char *SceneCourseSelect::mSelectCupName[] = {
    "SelCourse_Kinoko1.bti",
    "SelCourse_Flower1.bti",
    "SelCourse_Star1.bti",
    "SelCourse_Special1.bti",
    "SelCourse_Reverse2.bti",
};

#define CourseDataBuilder(course_id, course_name) \
    course_id, "CoName_" #course_name ".bti", "COP_" #course_name ".bti" 

const SceneCourseSelect::CourseData SceneCourseSelect::mCourseData[] = {
    CourseDataBuilder(LUIGI_CIRCUIT, LUIGI_CIRCUIT),
    CourseDataBuilder(PEACH_BEACH, PEACH_BEACH),
    CourseDataBuilder(BABY_PARK, BABY_PARK),
    CourseDataBuilder(DRY_DRY_DESERT, KARA_KARA_DESERT),
    CourseDataBuilder(MUSHROOM_BRIDGE, KINOKO_BRIDGE),
    CourseDataBuilder(MARIO_CIRCUIT, MARIO_CIRCUIT),
    CourseDataBuilder(DAISY_CRUISER, DAISY_SHIP),
    CourseDataBuilder(WALUIGI_STADIUM, WALUIGI_STADIUM),
    CourseDataBuilder(SHERBET_LAND, SHERBET_LAND),
    MUSHROOM_CITY,  "CoName_KINOKO_CITY.bti", "COP_KONOKO_CITY.bti", 
    CourseDataBuilder(YOSHI_CIRCUIT, YOSHI_CIRCUIT),
    CourseDataBuilder(DK_MOUNTAIN, DK_MOUNTAIN),
    CourseDataBuilder(WARIO_COLOSSEUM, WARIO_COLOSSEUM),
    CourseDataBuilder(DINO_DINO_JUNGLE, DINO_DINO_JUNGLE),
    CourseDataBuilder(BOWSER_CASTLE, BOWSER_CASTLE),
    CourseDataBuilder(RAINBOW_ROAD, RAINBOW_ROAD),
    LUIGI_CIRCUIT, "", "",
    CRS_ID_INV, "", "",
    CRS_ID_INV, "", "",
    CRS_ID_INV, "", "",
};

const u64 SceneCourseSelect::mCourseSelectBckTag[] = {
    'NCo_Pict', 'NWCourse', 'NBest1', 'NCoCurs1', 
    'N_OK', 'CupTro', 'CupName', 'Course01', 
    'Course02', 'Course03', 'Course04', 'EfCoCs01', 
    'EfCoCs02', 'EfCoCs03', 'EfCoCs04', 'EfCoCs05', 
    'EfCoCs06', 'EfCoCs07', 'EfCoCs08', 'EfOKBx01', 
    'EfOKBx02', 'EfOKBx03', 'EfOKBx04', 'EfOKBx05', 
    'EfOKBx06', 'WCourse', 'BestT0', 'BestT1', 
    'BestT2', 'BestT3', 'BestT4', 'BestT5', 
    'BestT6', 'BestL0', 'BestL1', 'BestL2', 
    'BestL3', 'BestL4', 'BestL5', 'BestL6', 
    'Cwhite01', 'Cwhite02',
};

const u64 SceneCourseSelect::mCourseSelectBtkTag[] = {
    'CoCurs1', 'CoOKBox', 'WCo_Pict', 'WCourse', 
};

const u64 SceneCourseSelect::mCourseSelectBpkTag[] = {
    'CoOKB', 'Co_Pict2', 
};

SceneCourseSelect::SceneCourseSelect(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {

}

void SceneCourseSelect::reset() {}

void SceneCourseSelect::init() {}

SceneCourseSelect::~SceneCourseSelect() {}

void SceneCourseSelect::draw() {}

void SceneCourseSelect::calc() {}

void SceneCourseSelect::course_init() {}

void SceneCourseSelect::course() {}

void SceneCourseSelect::buttonA() {}

void SceneCourseSelect::buttonB() {}

void SceneCourseSelect::course_end() {}

void SceneCourseSelect::nextScene() {}

void SceneCourseSelect::nextRace() {}

void SceneCourseSelect::calcAnm() {}

void SceneCourseSelect::setTexture() {}

void SceneCourseSelect::setRivalKart() {}

void SceneCourseSelect::setStartNo() {}

void SceneCourseSelect::rndRoulette() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
