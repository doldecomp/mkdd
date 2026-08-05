#include "Kameda/Race2D.h"
#include "Kaneshige/RaceTime.h"
#include "mathHelper.h"


Race2D *Race2D::mThis;
Task *Race2D::mTask;

JUTColor Race2D::mPlayerNumberColor[10] = {
    0xff5a5aff, 0x006affff, 0x00ff00ff, 0xffff00ff,
    0xff80ffff, 0xc87124ff, 0x00caf2ff, 0x9d4cffff,
    0x9696ffff, 0xa0a0a0ff
};

const char *Race2D::mPlayerNumberName[] = {
    "PlayerNumber_1P.bti",
    "PlayerNumber_2P.bti",
    "PlayerNumber_3P.bti",
    "PlayerNumber_4P.bti",
    "PlayerNumber_5P.bti",
    "PlayerNumber_6P.bti",
    "PlayerNumber_7P.bti",
    "PlayerNumber_8P.bti",
};

const char *Race2D::mLANPlayerNumberName[] = {
    "LANPlayerNumber_1.bti",
    "LANPlayerNumber_2.bti",
    "LANPlayerNumber_3.bti",
    "LANPlayerNumber_4.bti",
    "LANPlayerNumber_5.bti",
    "LANPlayerNumber_6.bti",
    "LANPlayerNumber_7.bti",
    "LANPlayerNumber_8.bti",
};

const char *Race2D::mTimeLapName[] = {
    "im_time1_3.bti", "im_lap1_1.bti"
};


const char *Race2D::mDriverName[] = {
    "BabyMario_MapIcon32.bti",
    "BabyLuigi_MapIcon32.bti",
    "Patapata_MapIcon32.bti",
    "Nokonoko_MapIcon32.bti",
    "Peach_MapIcon32.bti",
    "Daisy_MapIcon32.bti",
    "Mario_MapIcon32.bti",
    "Luigi_MapIcon32.bti",
    "Wario_MapIcon32.bti",
    "Waluigi_MapIcon32.bti",
    "yoshi_MapIcon32.bti",
    "Catherine_MapIcon32.bti",
    "DonkeyKong_MapIcon32.bti",
    "DiddyKong_MapIcon32.bti",
    "Koopa_MapIcon32.bti",
    "Koopajr_MapIcon32.bti",
    "Kinopio_MapIcon32.bti",
    "Kinopico_MapIcon32.bti",
    "Teresa_MapIcon32.bti",
    "Pakkun_MapIcon32.bti",
};

const char *Race2D::mPlayerName[] = {
    "Map_Icon_Player1.bti",
    "Map_Icon_Player2.bti",
    "Map_Icon_Player3.bti",
    "Map_Icon_Player4.bti",
    "Map_Icon_Player5.bti",
    "Map_Icon_Player6.bti",
    "Map_Icon_Player7.bti",
    "Map_Icon_Player8.bti",
};

const char *Race2D::mLANPlayerName[] = {
    "LANMap_Icon_Player1.bti",
    "LANMap_Icon_Player2.bti",
    "LANMap_Icon_Player3.bti",
    "LANMap_Icon_Player4.bti",
    "LANMap_Icon_Player5.bti",
    "LANMap_Icon_Player6.bti",
    "LANMap_Icon_Player7.bti",
    "LANMap_Icon_Player8.bti",
};

const char *Race2D::mCharacterName[] = {
    "baby_mario.bti",
    "baby_luigi.bti",
    "pata.bti",
    "noko.bti",
    "peach.bti",
    "daisy.bti",
    "mario.bti",
    "luigi.bti",
    "wario.bti",
    "waluigi.bti",
    "yoshi.bti",
    "catherine.bti",
    "donky.bti",
    "diddy.bti",
    "koopa.bti",
    "koopa_jr.bti",
    "kinopio.bti",
    "kinopico.bti",
    "teresa.bti",
    "pakkun.bti"
};

const u64 Race2D::mLapTag[] = {
    'm1_lap', 'ml1_1', 'ml1_s1', 'ml1_2',
    'm2_lap', 'ml2_1', 'ml2_s1', 'ml2_2',
    'm3_lap', 'ml3_1', 'ml3_s1', 'ml3_2',
    'm4_lap', 'ml4_1', 'ml4_s1', 'ml4_2',
};
const u64 Race2D::mItemExTag[] = {
    'itm1bu', 'itm1fu', 'itm1bs', 'itm1fs',
    'itm2bu', 'itm2fu', 'itm2bs', 'itm2fs',
    'itm3bu', 'itm3fu', 'itm3bs', 'itm3fs',
    'itm4bu', 'itm4fu', 'itm4bs', 'itm4fs',
};
const u64 Race2D::mItemExDrawTag[] = {
    'slot1bb', 'itm1bu', 'itm1bs', 'slot1b', 
    'slot1bh2', 'slot1bh1', 'slot1fb', 'itm1fu', 
    'itm1fs', 'slot1f', 'slot1fh2', 'slot1fh1', 
    'slot2bb', 'itm2bu', 'itm2bs', 'slot2b', 
    'slot2bh2', 'slot2bh1', 'slot2fb', 'itm2fu', 
    'itm2fs', 'slot2f', 'slot2fh2', 'slot2fh1', 
    'slot3bb', 'itm3bu', 'itm3bs', 'slot3b', 
    'slot3bh2', 'slot3bh1', 'slot3fb', 'itm3fu', 
    'itm3fs', 'slot3f', 'slot3fh2', 'slot3fh1', 
    'slot4bb', 'itm4bu', 'itm4bs', 'slot4b', 
    'slot4bh2', 'slot4bh1', 'slot4fb', 'itm4fu', 
    'itm4fs', 'slot4f', 'slot4fh2', 'slot4fh1',
};

const char *Race2D::mItemName[] = {
    "item_kame_green.bti",
    "item_koopa.bti",
    "item_kame_red.bti",
    "item_banana.bti",
    "item_big_banana.bti",
    "item_kinoko.bti",
    "item_star.bti",
    "item_wanwan.bti",
    "item_bomb_hei.bti",
    "item_fire_ball.bti",
    "item_thunder.bti",
    "item_egg.bti",
    "item_GoldenKinoko.bti",
    "item_kame_wing.bti",
    "item_heart.bti",
    "item_dummybox.bti",
    "item_empty.bti",
    "item_kame_green_3.bti",
    "item_kinoko_3.bti",
    "item_kame_red_3.bti",
    "item_kame_green_3.bti",
    "item_fire_ball.bti",
    "item_empty.bti",
    "item_kame_green_2.bti",
    "item_kinoko_2.bti",
    "item_kame_red_2.bti",
    "item_fire_ball_luigi.bti",
    "item_egg_catherine.bti",
    "item_kame_green.bti",
    "item_kinoko.bti",
    "item_kame_red.bti",
};

const u64 Race2D::mRankTag[] = {
    'pos11', 'pos12', 'pos13', 'pos14', 
    'pos15', 'pos16', 'pos17', 'pos18', 
    'pos21', 'pos22', 'pos23', 'pos24', 
    'pos25', 'pos26', 'pos27', 'pos28', 
    'pos31', 'pos32', 'pos33', 'pos34', 
    'pos35', 'pos36', 'pos37', 'pos38', 
    'pos41', 'pos42', 'pos43', 'pos44', 
    'pos45', 'pos46', 'pos47', 'pos48',
};

const u64 Race2D::mRankGoalTag[] = {
    'pos11g', 'pos12g', 'pos13g', 'pos14g', 
    'pos15g', 'pos16g', 'pos17g', 'pos18g', 
    'pos21g', 'pos22g', 'pos23g', 'pos24g', 
    'pos25g', 'pos26g', 'pos27g', 'pos28g', 
};

const u64 Race2D::mTimeDrawTag[] = {
    'time17', 'time16', 'time15', 'time1t2', 
    'time14', 'time13', 'time1t1', 'time12', 
    'time11', 'time10', 'time27', 'time26', 
    'time25', 'time2t2', 'time24', 'time23', 
    'time2t1', 'time22', 'time21', 'time20',
};

const u64 Race2D::mLapTimeDrawTag[] = {
    'time16_1', 'time15_1', 'time14_1', 'time13_1', 
    'time12_1', 'time11_1', 'time10_1', 'time1t11', 
    'time1t21', 'Tl11', 'Tlapm1', 'time16_2', 
    'time15_2', 'time14_2', 'time13_2', 'time12_2', 
    'time11_2', 'time10_2', 'time1t12', 'time1t22', 
    'Tl12', 'Tlapm1', 'time16_3', 'time15_3', 
    'time14_3', 'time13_3', 'time12_3', 'time11_3', 
    'time10_3', 'time1t13', 'time1t23', 'Tl13', 
    'Tlapm1', 'time16_4', 'time15_4', 'time14_4', 
    'time13_4', 'time12_4', 'time11_4', 'time10_4', 
    'time1t14', 'time1t24', 'Tl14', 'Tlapm1', 
    'time16_5', 'time15_5', 'time14_5', 'time13_5', 
    'time12_5', 'time11_5', 'time10_5', 'time1t15', 
    'time1t25', 'Tl15', 'Tlapm1', 'time16_6', 
    'time15_6', 'time14_6', 'time13_6', 'time12_6', 
    'time11_6', 'time10_6', 'time1t16', 'time1t26', 
    'Tl16', 'Tlapm1', 'time16_7', 'time15_7', 
    'time14_7', 'time13_7', 'time12_7', 'time11_7', 
    'time10_7', 'time1t17', 'time1t27', 'Tl17', 
    'Tlapm1', 'time16_8', 'time15_8', 'time14_8', 
    'time13_8', 'time12_8', 'time11_8', 'time10_8', 
    'time1t18', 'time1t28', 'Tl18', 'Tlapm1', 
    'time16_9', 'time15_9', 'time14_9', 'time13_9', 
    'time12_9', 'time11_9', 'time10_9', 'time1t19', 
    'time1t29', 'Tl19', 'Tlapm1', 'time26_1', 
    'time25_1', 'time24_1', 'time23_1', 'time22_1', 
    'time21_1', 'time20_1', 'time2t11', 'time2t21', 
    'Tl21', 'Tlapm2', 'time26_2', 'time25_2', 
    'time24_2', 'time23_2', 'time22_2', 'time21_2', 
    'time20_2', 'time2t12', 'time2t22', 'Tl22', 
    'Tlapm2', 'time26_3', 'time25_3', 'time24_3', 
    'time23_3', 'time22_3', 'time21_3', 'time20_3', 
    'time2t13', 'time2t23', 'Tl23', 'Tlapm2', 
    'time26_4', 'time25_4', 'time24_4', 'time23_4', 
    'time22_4', 'time21_4', 'time20_4', 'time2t14', 
    'time2t24', 'Tl24', 'Tlapm2', 'time26_5', 
    'time25_5', 'time24_5', 'time23_5', 'time22_5', 
    'time21_5', 'time20_5', 'time2t15', 'time2t25', 
    'Tl25', 'Tlapm2', 'time26_6', 'time25_6', 
    'time24_6', 'time23_6', 'time22_6', 'time21_6', 
    'time20_6', 'time2t16', 'time2t26', 'Tl26', 
    'Tlapm2', 'time26_7', 'time25_7', 'time24_7', 
    'time23_7', 'time22_7', 'time21_7', 'time20_7', 
    'time2t17', 'time2t27', 'Tl27', 'Tlapm2', 
    'time26_8', 'time25_8', 'time24_8', 'time23_8', 
    'time22_8', 'time21_8', 'time20_8', 'time2t18', 
    'time2t28', 'Tl28', 'Tlapm2', 'time26_9', 
    'time25_9', 'time24_9', 'time23_9', 'time22_9', 
    'time21_9', 'time20_9', 'time2t19', 'time2t29', 
    'Tl29', 'Tlapm2',
};

const u64 Race2D::mSpeedTag[] = {
    'sp3', 'sp2', 'sp1',
};

const u64 Race2D::mSpeedChipTag[] = {
    'sp_mtr1', 'sp_mtr7', 'sp_mtr6', 'sp_mtr5', 
    'sp_mtr4', 'sp_mtr3', 'sp_mtr2', 
};

const u64 Race2D::mSpeedDrawTag[] = {
    'sp_mtr1', 'sp_mtr7', 'sp_mtr6', 'sp_mtr5', 
    'sp_mtr4', 'sp_mtr3', 'sp_mtr2', 'sp1', 
    'sp2', 'sp3', 'km_h',
};

const char *Race2D::mSpeedName[] = {
    "deg0.bti",
    "deg1.bti",
    "deg2.bti",
    "deg3.bti",
    "deg4.bti",
    "deg5.bti",
    "deg6.bti",
    "deg7.bti",
    "deg8.bti",
    "deg9.bti",
};

const u64 Race2D::mShineTimeTag[] = {
    'ShineB', 'ShineM', 'MtimeB10', 'MtimeB11', 
    'MtimeA10', 
};

static f32 sSpinRotate[8];
static int sPreLap[4];
static int sItemState[4][2];
static int sPreBombPoint[4];
static int sPreRank[4];
static int sRankAnm[4];
static int sThunderAnm[8];

static int sChangeFrame;
static int sColorIndex;
static u8 sPreWhichDriver[4];
static u8 sPreWhichDriver2[8];
static bool sPreShineFlag;
static bool sRollingFlag[4][2];
static bool sUseFlag[4][2];
static int sPreShinePlayer;
static RaceTime sBestLapTime;
static u8 sPreThunder[8];

Race2D::Race2D(JKRHeap *) {
    /*
J2DPane::getTranslateY() const {}
J2DPane::getTranslateX() const {}
JUTColor::TColor(const JUTColor &) {}
RaceMgr::getTotalLapNumber() const {}
RaceMgr::getPlayerNumber() const {}
RaceMgr::getKartNumber() const {}
std::atan2(f32, f32) {}
RCMGetCourse() {}
JUTColor::TColor(u32) {}
RaceMgr::isMirror() const {}
Race2DParam::setS(f32) {}
Race2DParam::setY(f32) {}
Race2DParam::setX(f32) {}
ResMgr::getCourseID() {}
Race2DHioNode::getParam() {}
JUTColor::set(_GXColor) {}
J2DManager::getNetFlag() {}
RaceMgr::getConsoleNumber() const {}
Race2DHioNode::Race2DHioNode(const char *) {}
Race2DParam::~Race2DParam() {}
    */
}


void Race2D::init() {}

void Race2D::drawPlayerMark() {}

void Race2D::drawMask() {}

void Race2D::draw() {}

void Race2D::drawAlarm() {}

void Race2D::anmTA(int) {}

void Race2D::getAlphaAnmTA(int, u8 &) {}

void Race2D::getBestTimeColor(int, JUTColor &, J2DPicture::TCornerColor &) {}

void Race2D::getBestLapColor(int, JUTColor &) {}

void Race2D::drawMiniGame() {}

void Race2D::drawBomb() {
    //KartChecker::getWinBombPoint() {}
}

void Race2D::getBombScale(int, int, int) {}

void Race2D::getReachBombScale(int, int) {}

void Race2D::drawEscape() {}

void Race2D::getStartShineTimeScale(int) {}

void Race2D::drawCourse() {}

void Race2D::calc() {}

void Race2D::calcPlayerMark() {}

void Race2D::calcParticle() {}

void Race2D::setLayoutData() {}

void Race2D::calcMiniGame() {}

void Race2D::calcBomb() {}

void Race2D::getBombColor(JUTColor, JUTColor) {}

void Race2D::isBombReach(int) {}

void Race2D::isBombWinner(int) {}

void Race2D::setEffectPos(int) {}

void Race2D::calcEscape() {}

void Race2D::calcTimeFrame(bool, int) {}

void Race2D::getCornerColor(int) {}

void Race2D::setEscapeColor(int) {}

void Race2D::getTimeColor(JUTColor, JUTColor) {}

void Race2D::startBombDemo() {}

bool Race2D::isAlarm(int) {}

void Race2D::getMapPos(int, JGeometry::TVec3f &, JGeometry::TVec2f &) {}

void Race2D::getCharacterInfo(int, int, f32 &, f32 &, f32 &) {}

void Race2D::getCharacterClr(int, int, int, JUTColor &, JUTColor &, u8 &) {}

void Race2D::getItemInfo(int, int, int, f32 &, f32 &, f32 &) {}

void Race2D::decideItem(int, u8, int &) {}

void Race2D::calcLapInit() {}

void Race2D::calcLap() {}

void Race2D::lapInit() {}

void Race2D::lapMain() {}

void Race2D::lapTimeInit() {}

void Race2D::lapTimeMain() {}

void Race2D::calcItem() {}

void Race2D::itemStartInit() {}

void Race2D::itemStart() {}

void Race2D::itemBlinkInit() {}

void Race2D::itemBlink() {}

void Race2D::itemGetInit() {}

void Race2D::itemGetMain() {}

void Race2D::itemSpecialInit() {}

void Race2D::itemSpecialMain() {}

void Race2D::setUsisi(int, ECharID) {}

void Race2D::alarmX(int, f32 &) {}

void Race2D::getStartScaleA(int, f32 &) {}

void Race2D::getStartScaleB(int, f32 &) {}

void Race2D::getStartSpeedMeterScale(int, int, f32 &) {}

void Race2D::getGoalSpeedMeterPos(int, f32 &) {}

void Race2D::getStartLapPos(int, int, f32 &) {}

void Race2D::getGoalLapPos(int, f32 &) {}

void Race2D::getStartCharPos(int, int, f32 &) {}

void Race2D::getGoalCharPos(int, int, f32 &) {}

void Race2D::getThunderCharScale(int) {}

void Race2D::getStartRankPos(int, int, JGeometry::TVec2f &) {}

void Race2D::getRaceRankScale(int) {}

void Race2D::getGoalRankPos(int, int, JGeometry::TVec2f &) {}

void Race2D::getGoalRankScale(int, int, f32 &) {}

void Race2D::getGoalRankRot(int, f32 &) {}

void Race2D::getStartLapTimePos(int, int, f32 &) {}

void Race2D::getGoalLapTimePos(int, int, f32 &) {}

#include "JSystem/JAUdio/JASFakeMatch2.h"
