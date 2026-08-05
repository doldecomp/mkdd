#include "Kameda/Result2D.h"
#include "JSystem/JUtility/TColor.h"
#include "Kaneshige/KartInfo.h"
#include "Yamamoto/kartCtrl.h"
#include "mathHelper.h"

bool Result2D::mDrawFlag;
int Result2D::mResultSelector;
int Result2D::mResultState;

JUTColor Result2D::mWhiteColor[4][2] = {
    0xff505dff, 0x80283cff, 
    0x0064ffff, 0x003280ff,
    0x1e871eff, 0x004600ff, 
    0xb4b4b4ff, 0xb4b4b4ff
};

const u64 Result2D::mOverBackTag = 'OverBack';

const char *Result2D::mRankName[] = {
    "position1_1.bti",
    "position1_2.bti",
    "position1_3.bti",
    "position1_4.bti",
    "position1_5.bti",
    "position1_6.bti",
    "position1_7.bti",
    "position1_8.bti",
};

const char *Result2D::mPlayerNumberName[] = {
    "PlayerNumberSimple_1P.bti",
    "PlayerNumberSimple_2P.bti",
    "PlayerNumberSimple_3P.bti",
    "PlayerNumberSimple_4P.bti"
};

const char *Result2D::mResultArrowName[] = {
    "ArrowGP1.bti",
    "ArrowGP2.bti",
    "ArrowGP3.bti"
};

const u64 Result2D::mSetWordLineTag[] = {
    'RCursBL1', 'RCursBL2', 'RCursBL3', 'RCursBR1', 
    'RCursBR2', 'RCursBR3', 'RMozi00', 'RCurs01', 
    'RCurs02', 
};

const u64 Result2D::mResultLayoutTag[] = {
    'RCoNaBA', 'RCoNaA', 'CuNameA', 'RCuPctA', 
    'RPow03A', 'RPow02A', 'RPow01A', 'RPowM1A', 
    'RPowM2A', 'RPowM3A', 'Mirror', 
};

const u64 Result2D::mResultLineTag[] = {
    'RCurs01', 'RCurs02', 'RCurs03', 'RChaB02', 
    'RChara02', 'RChaB01', 'RChara01', 'RPos00', 
    'Rtime00', 'Rtime01', 'Rtime02', 'Rtime03', 
    'Rtime04', 'Rtime05', 'Rtime06', 'RtimeT01', 
    'RtimeT02', 'RPlus01', 'RPlus', 'Arrow_00',
};

const u64 Result2D::mGetPointTag[] = {
    'Point_00', 'Point_01', 'Point_02', 'Point_03'
};

const u64 Result2D::mVSBattleCharacterTag[] = {
    'RChaW2', 'RChaPc2', 'RChaW1', 'RChaPc1', 
    'Win00', 'Pn_Ind1', 'Pn_Ind2', 'Pn_Ind3', 
    'Back1a', 'Back2a', 'Back3a', 'Back1b', 
    'Back2b', 'Back3b',
};

const u64 Result2D::mVSBattlePointTag[] = {
    'PPoint11', 'PPoint12', 
};

Result2D::Result2D(JKRHeap *) {
    /*
RaceMgr::getKartInfo(int) {}
RaceMgr::getStatusNumber() const {}
KartInfo::getDriverCharID(int) const {}
J2DPicture::setCornerColor(JUTColor) {}
KartInfo::getPad(int) {}
KartInfo::isTagMode() const {}
RaceInfo::getKartInfo(int) {}
J2DPane::getScaleY() const {}
J2DPane::getScaleX() const {}
J2DAnmBase::setFrame(float) {}
JKRGetNameResource(const char *, JKRFileLoader *) {}
J2DPicture::setCornerColor(J2DPicture::TCornerColor) {}
J2DPicture::getCornerColor(J2DPicture::TCornerColor &) {}
Race2D::getRankPic(int, int) {}
J2DManager::getRace2D() {}
JUTColor::operator= (const JUTColor &) {}
KartLocale::getLanguage() {}
Kart2DCommon::getNumberTexture(Kart2DCommon::NumberTextureID) {}
RaceMgr::getRaceLevel() const {}
Kart2DCommon::getCupIconTexture(ERaceGpCup) {}
Kart2DCommon::getCupNameTexture(ERaceGpCup) {}
SequenceInfo::getCup() const {}
Kart2DCommon::ptr() {}
J2DPane::show() {}
JGeometry::TVec2f::operator= (const JGeometry::TVec2f &) {}
J2DPane::hide() {}
JGeometry::TBox2f::TBox2(const JGeometry::TBox2f &) {}
J2DScreen::set(const char *, JKRArchive *) {}
J2DManager::getArchive() const {}
GETJ2DManager() {}
RaceMgr::getRaceMode() const {}
RCMGetManager() {}
System::getJ2DOrtho() {}
Result2DHioNode::Result2DHioNode(const char *) {}
Result2DParam::~Result2DParam() {}
Result2D::data2D::data2D() {}
    */
}

void Result2D::init() {
    //GetKartCtrl()->getKartLoader(0)->mKartInfo->isTagMode();
}

void Result2D::draw() {}

void Result2D::drawGP() {}

void Result2D::drawVS() {}

void Result2D::drawTA() {}

void Result2D::drawMG() {}

void Result2D::calc(KartGamePad *) {}

void Result2D::calcRaceRank() {}

void Result2D::calcGPRank() {}

void Result2D::calcResult() {}

void Result2D::calcVS() {}

void Result2D::calcTA(KartGamePad *) {}

void Result2D::calcMG() {}

void Result2D::calcPASSWD(KartGamePad *) {}

void Result2D::selectorGP(KartGamePad *) {}

void Result2D::selectorVS(KartGamePad *) {}

void Result2D::selectorTA(KartGamePad *) {}

void Result2D::selectorMG(KartGamePad *) {}

void Result2D::selectorCommon() {}

void Result2D::setDrawGP() {}

void Result2D::setDrawVS() {}

void Result2D::setDrawTA() {}

void Result2D::setDrawMG() {}

void Result2D::getResultSelector() {}

void Result2D::setRaceRank() {}

void Result2D::setGPRank() {}

void Result2D::setVS() {}

void Result2D::setTA() {}

void Result2D::setMG() {}

void Result2D::setBattleCharacter() {}

void Result2D::getRaceRankLayoutAlpha(int, u8 &) {}

void Result2D::getGPRankLayoutAlpha(int, u8 &) {}

void Result2D::getRaceRankLinePos(int, int, f32 &) {}

void Result2D::getGPRankLinePos(int, int, f32 &) {}

void Result2D::getResultLinePos(int, f32 &) {}

void Result2D::getRaceRankLineScale(int, int, JGeometry::TVec2f &, JGeometry::TVec2f &) {}

void Result2D::getGPRankLineScale(int, int, JGeometry::TVec2f &, JGeometry::TVec2f &) {}

void Result2D::getFrameClr(int, JUTColor &, u8 &) {}

void Result2D::getVSPointYPos(int, int, f32 &) {}

void Result2D::getMGPointYPos(int, f32 &) {}

void Result2D::getVSScale(int, f32 &) {}

void Result2D::getMGScale(int, f32 &) {}

void Result2D::getVSColor(int, JUTColor &) {}

void Result2D::getMGColor(int, JUTColor &) {}

void Result2D::getFrameColor(JUTColor &) {}

void Result2D::getScale(int) {}

void Result2D::setScale(int) {}

void Result2D::setGPClr() {}

void Result2D::getAnmEnd() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
