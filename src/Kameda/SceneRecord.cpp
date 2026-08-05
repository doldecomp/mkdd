#include "Kameda/SceneRecord.h"
#include "Kameda/Scene.h"

const u64 SceneRecord::mTagRecord_cursor[] = {
    'TrCurs1', 'TrCurs2', 'TrCurs3', 
};

const u64 SceneRecord::mTagRecord_menu[] = {
    'RecM1', 'RecM2', 'RecM3',
};

const u64 SceneRecord::mTagRecord_menubar[] = {
    'RecBar1', 'RecBar2', 'RecBar3',
};

const u64 SceneRecord::mTagRecord_submenu[] = {
    'NRecCo1', 'NRecCo2',
};

const u64 SceneRecord::mTagRecord_submenu2[] = {
    'RecCo1', 'NRecS1Ls', 'NRecS1Rs', 'RecCo2',
    'NRecS2Ls', 'NRecS2Rs',
};

const u64 SceneRecord::mTagRecord_submenucursor[] = {
    'NRecS1L', 'NRecS1R', 'NRecS2L', 'NRecS2R',
};

const u64 SceneRecord::mTagRecord_submenucursor2[] = {
    'RecSL1L', 'RecSL1R', 'RecSL2L', 'RecSL2R',
};

const u64 SceneRecord::mTagRecord_submenucursor3[] = {
    'RecSB1L', 'RecSB1R', 'RecSB2L', 'RecSB2R',
};

const u64 SceneRecord::mTagRecordGP_all[] = {
    'NGPM', 'NCoN', 'NReOp',
};

const u64 SceneRecord::mTagRecordGP_cursor[] = {
    'NCoTL', 'NCoTR',
};

const u64 SceneRecord::mTagRecordGP_cursor2[] = {
    'NCoSL', 'NCoSR',
};

const u64 SceneRecord::mTagRecordGP_cursor3[] = {
    'CoSBL', 'CoSBR',
};

const u64 SceneRecord::mTagRecordGP_cursorbtk[] = {
    'CoSLL', 'CoSLR',
};

const u64 SceneRecord::mTagRecordGP_linepostag[] = {
    'PowTag1', 'PowTag2', 'PowTag3', 'PowTag4',
};

const u64 SceneRecord::mTagRecordGP_linepostag2[] = {
    'PowSgl1', 'PowSgl2', 'PowSgl3', 'PowSgl4',
};

const u64 SceneRecord::mTagRecordGPLine_time[] = {
    'GPT0', 'GPT1', 'GPT2', 'GPT3',
    'GPT4', 'GPT5', 'GPT6',
};

const u64 SceneRecord::mTagRecordGPLine_quotation[] = {
    'GDTt02', 'GDTt01',
};

const u64 SceneRecord::mTagRecordGPLine_point[] = {
    'Point1', 'Point2', 'Point3',
};

const u64 SceneRecord::mTagRecordGPLine_charawindow[] = {
    'GPChaB1', 'GPChaB2', 'GPChaB3',
};

const u64 SceneRecord::mTagRecordGPLine_name[] = {
    'GPNm3', 'GPNm2', 'GPNm1',
};

const u64 SceneRecord::mTagRecordTA_all[] = {
    'N_BL', 'N_TA', 'NReOp',
};

const u64 SceneRecord::mTagRecordTA_cursor[] = {
    'NCoTL', 'NCoTR',
};

const u64 SceneRecord::mTagRecordTA_cursor2[] = {
    'NCoSL', 'NCoSR',
};

const u64 SceneRecord::mTagRecordTA_cursor3[] = {
    'CoSBL', 'CoSBR',
};

const u64 SceneRecord::mTagRecordTA_cursorbtk[] = {
    'CoSLL', 'CoSLR',
};

const u64 SceneRecord::mTagRecordTA_character[] = {
    'BTCha3', 'BTCha2', 'BTCha1',
};

const u64 SceneRecord::mTagRecordTA_characterwindow[] = {
    'BTChaB3', 'BTChaB2', 'BTChaB1',
};

const u64 SceneRecord::mTagRecordTA_bestlap[] = {
    'BLTm0', 'BLTm1', 'BLTm2', 'BLTm3',
    'BLTm4', 'BLTm5', 'BLTm6',
};

const u64 SceneRecord::mTagRecordTA_name[] = {
    'BLNm3', 'BLNm2', 'BLNm1',
};

const u64 SceneRecord::mTagRecordTA_quotation[] = {
    'BLTmt2', 'BLTmt1',
};

const u64 SceneRecord::mTagRecordTA_linepostag[] = {
    'Rank1', 'Rank2', 'Rank3', 'Rank4',
    'Rank5',
};

const u64 SceneRecord::mTagRecordTA_linecolor[] = {
    'Tex_1', 'Tex_2', 'Tex_3', 'Tex_4',
    'Tex_5',
};

const u64 SceneRecord::mTagRecordTALine_time[] = {
    'TTTm0', 'TTTm1', 'TTTm2', 'TTTm3',
    'TTTm4', 'TTTm5', 'TTTm6',
};

const u64 SceneRecord::mTagRecordTALine_quotation[] = {
    'TTTmt2', 'TTTmt1',
};

const u64 SceneRecord::mTagRecordTALine_character[] = {
    'TTCha3', 'TTCha2', 'TTCha1',
};

const u64 SceneRecord::mTagRecordTALine_characterwindow[] = {
    'TTChaB3', 'TTChaB2', 'TTChaB1',
};

const u64 SceneRecord::mTagRecordTALine_name[] = {
    'TTNm3', 'TTNm2', 'TTNm1',
};

const char *SceneRecord::mBtiName_Cup[] = {
    "CupWithPict_MushroomCup.bti",
    "CupWithPict_FlowerCup.bti",
    "CupWithPict_StarCup.bti",
    "CupWithPict_SpecialCup.bti",
    "CupWithPict_Reverse2Cup.bti"
};

const char *SceneRecord::mBtiName_Level[] = {
    "Power_Mozi.1.bti",
    "Power_Mozi.2.bti",
    "Power_Mozi.3.bti",
    "Power_Mozi.4.bti",
};

const char *SceneRecord::mBtiName_Trophy[3] = {
    "Trophy1.bti",
    "Trophy2.bti",
    "Trophy3.bti"
};

const char *SceneRecord::mBtiName_Rank[] = {
    "position1_1.bti",
    "position1_2.bti",
    "position1_3.bti",
    "position1_4.bti",
    "position1_5.bti",
};

const char *SceneRecord::mBtiName_characterwindow = "chara_window1.bti";
const char *SceneRecord::mBtiName_quotation = "kazu3Dten.bti";
const char *SceneRecord::mBtiName_colon1 = "timg/MarioFontColon1.bti";
const char *SceneRecord::mBtiName_colon2 = "timg/MarioFontColon2.bti";
const char *SceneRecord::mBtiName_minus = "MarioFont_Number_XLine.bti";

const u64 SceneRecord::mModeTag[] = {
    'mt_1gp', 'mt_2gp', 'ms_1gp', 'ms_2gp',
    'mc_betu', 'm_pice',
};

SceneRecord::SceneRecord(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {
    // J2DPicture::TCornerColor::TCornerColor() {}
}

void SceneRecord::SceneRecord_ConstructerMenu(JKRHeap *) {}

void SceneRecord::SceneRecord_ConstructerGP(JKRHeap *) {}

void SceneRecord::SceneRecord_ConstructerGPLine(JKRHeap *) {}

void SceneRecord::SceneRecord_ConstructerTA(JKRHeap *) {}

void SceneRecord::SceneRecord_ConstructerTALine(JKRHeap *) {}

void SceneRecord::SceneRecord_ConstructerBack(JKRHeap *) {}

void SceneRecord::init() {}

void SceneRecord::init_mode() {}

void SceneRecord::init_gp() {}

void SceneRecord::init_gpline() {}

void SceneRecord::init_ta() {}

void SceneRecord::init_taline() {}

void SceneRecord::init_back() {}

SceneRecord::~SceneRecord() {}

void SceneRecord::draw() {}

void SceneRecord::calcAnmRecordCursor() {}

void SceneRecord::calcAnmRecordGPCursor() {}

void SceneRecord::calcAnmRecordGPLineAll() {}

void SceneRecord::calcTexChangeRecordGPLine(ERaceLevel) {}

void SceneRecord::calcAnmRecordTACursor() {}

void SceneRecord::calcAnmRecordTALineAll() {}

void SceneRecord::calcTexChangeRecordTA() {}

void SceneRecord::calcTexChangeRecordTALine(int) {}

void SceneRecord::calcAnm() {}

void SceneRecord::calcMatAnmRecordCursor() {}

void SceneRecord::calcMatAnm() {}

void SceneRecord::calc() {}

void SceneRecord::fadeIn() {}

void SceneRecord::mode() {}

void SceneRecord::gp() {}

void SceneRecord::ta() {}

void SceneRecord::record() {}

void SceneRecord::piece() {}

void SceneRecord::nextScene() {}

void SceneRecord::ConvNum2ERace(int) {}

#include "JSystem/JAudio/JASFakeMatch.h"
