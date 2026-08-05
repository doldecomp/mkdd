#include "Kameda/SceneOption.h"
#include "Kameda/Scene.h"
#include "mathHelper.h"

const u64 SceneOption::mTag_menu[] = {
    'Sd_M', 'BGM_M', 'Rm_M', 'Gh_M', 
    'Is_M', 'Lp_M', 'ReTtl_M', 
};

const u64 SceneOption::mTag_secret[] = {
    'Ques1', 'Ques2', 'Ques3', 
};

const u64 SceneOption::mTag_cursor[] = {
    'SdCs', 'BGMCs', 'RmCs', 'GhCs', 
    'IsCs', 'LpCs', 'ReTtlCs', 
};

const u64 SceneOption::mTag_submenu[] = {
    'NSdSb', 'NSbBGM', 'NSbRm', 'NSbGh', 
    'NSbIs', 'NSbLp', 
};

const u64 SceneOption::mTag_submenu_bgms[] = {
    'SbBGMw0', 'SbBGMw1', 'SbBGMw2', 
};

const u64 SceneOption::mTag_cursorl1[] = {
    'NSdSbL1', 'NSbBGML1', 'NSbRmL1', 'NSbGhL1', 
    'NSbIsL1', 'NSbLpL1',
};

const u64 SceneOption::mTag_cursorr1[] = {
    'NSdSbR1', 'NSbBGMR1', 'NSbRmR1', 'NSbGhR1', 
    'NSbIsR1', 'NSbLpR1', 
};

const u64 SceneOption::mTag_cursorl2[] = {
    'NSdSbL2', 'NSbBGML2', 'NSbRmL2', 'NSbGhL2', 
    'NSbIsL2', 'NSbLpL2',
};

const u64 SceneOption::mTag_cursorr2[] = {
    'NSdSbR2', 'NSbBGMR2', 'NSbRmR2', 'NSbGhR2', 
    'NSbIsR2', 'NSbLpR2',
};

const u64 SceneOption::mTag_cursorl3[] = {
   'SdSbL1', 'SbBGML1', 'SbRmL1', 'SbGhL1', 
    'SbIsL1', 'SbLpL1', 
};

const u64 SceneOption::mTag_cursorr3[] = {
    'SdSbR1', 'SbBGMR1', 'SbRmR1', 'SbGhR1', 
    'SbIsR1', 'SbLpR1', 
};

const u64 SceneOption::mTag_cursorl4[] = {
    'SdSbL2', 'SbBGML2', 'SbRmL2', 'SbGhL2', 
    'SbIsL2', 'SbLpL2', 
};
const u64 SceneOption::mTag_cursorr4[] = {
    'SdSbR2', 'SbBGMR2', 'SbRmR2', 'SbGhR2', 
    'SbIsR2', 'SbLpR2',
};

const u64 SceneOption::mTag_submenumoji[] = {
    'SbSd_M', 'XXXXXXXX', 'SbRm_M', 'SbGh_M', 
    'SbIs_M', 'SbLp_M', 
};

const u64 SceneOption::mTag_color[] = {
    'CSel1', 'CSel2', 
};

const u64 SceneOption::mTag_knob[] = {
    'BGMKnob', 'SEKnob', 
};

const u64 SceneOption::mTag_volume[] = {
    'BGMBar2', 'SEBar2',
};

const char *SceneOption::mBtiName_sound[] = {
    "Mozi_Mono.bti",
    "Mozi_Stereo.bti",
    "Mozi_DolbySurround.bti"
};

const char *SceneOption::mBtiName_onOff[] = {
    "Mozi_Off.bti",
    "Mozi_On.bti",
};

const char *SceneOption::mBtiName_itemSlot[] = {
    "Mozi_Standard.bti",
    "Mozi_Smash.bti",
    "Mozi_Super.bti",
    "Mozi_Simple.bti"
};

const char *SceneOption::mBtiName_bgmsign[] = {
    "MarioFont_Number_XLine.bti",
    "MarioFont_Number_Plus.bti",
};

const char *SceneOption::mBtiName_default = "Mozi_Question.bti";

const u64 SceneOption::mTagSP_menu[] = {
    'SPM1', 'SPM2', 'SPM3', 'SPM4', 
};

const u64 SceneOption::mTagSP_submenu[] = {
    'NSb1', 'NSb2', 'NSb3', 
};

const u64 SceneOption::mTagSP_cursor[] = {
    'Crs1', 'Crs2', 'Crs3', 'Crs4', 
};

const u64 SceneOption::mTagSP_cursorl1[] = {
    'NSbStL1', 'NSbStL2', 'NSbStL3', 
};

const u64 SceneOption::mTagSP_cursorr1[] = {
    'NSbStR1', 'NSbStR2', 'NSbStR3',
};

const u64 SceneOption::mTagSP_cursorl2[] = {
    'NSbSsL1', 'NSbSsL2', 'NSbSsL3', 
};

const u64 SceneOption::mTagSP_cursorr2[] = {
    'NSbSsR1', 'NSbSsR2', 'NSbSsR3', 
};

const u64 SceneOption::mTagSP_cursorl3[] = {
    'SbL1', 'SbL2', 'SbL3', 
};

const u64 SceneOption::mTagSP_cursorr3[] = {
    'SbR1', 'SbR2', 'SbR3', 
};

const u64 SceneOption::mTagSP_cursorl4[] = {
    'SbLL1', 'SbLL2', 'SbLL3',
};

const u64 SceneOption::mTagSP_cursorr4[] = {
    'SbLR1', 'SbLR2', 'SbLR3', 
};

const u64 SceneOption::mTagSP_numeral1[] = {
    'Sb1M11', 'Sb2M11', 'Sb3M11',
};

const u64 SceneOption::mTagSP_numeral21[] = {
    'Sb1M21', 'Sb2M21', 'Sb3M21', 
};

const u64 SceneOption::mTagSP_numeral22[] = {
    'Sb1M22', 'Sb2M22', 'Sb3M22',
};

const u64 SceneOption::mTagSP_numeral31[] = {
    'Sb1M31', 'Sb2M31', 'Sb3M31', 
};

const u64 SceneOption::mTagSP_numeral32[] = {
    'Sb1M32', 'Sb2M32', 'Sb3M32', 
};

const u64 SceneOption::mTagSP_numeral33[] = {
    'Sb1M33', 'Sb2M33', 'Sb3M33', 
};

const u64 SceneOption::mTagSP_color[] = {
    'CSel1', 'CSel2',
};

SceneOption::SceneOption(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {
    
}

void SceneOption::SceneOption_ConstructerOption(JKRHeap *) {}

void SceneOption::SceneOption_ConstructerBack(JKRHeap *) {}

void SceneOption::init() {}

void SceneOption::init_option() {}

void SceneOption::init_back() {}

 SceneOption::~SceneOption() {}

void SceneOption::draw() {}

void SceneOption::calcAnmCursor() {}

void SceneOption::calcAnm() {}

void SceneOption::calcMatAnmCursor() {}

void SceneOption::calc() {}

void SceneOption::fadeIn() {}

void SceneOption::option() {}

void SceneOption::soundplayer() {}

void SceneOption::nextScene() {}

void SceneOption::getJASoundMode(int) {}

void SceneOption::SceneChange_ExitOption(int) {}

void SceneOption::rumble(int) {}

void SceneOption::rumbleOff(int) {}

void SceneOption::save() {}

void SceneOption::nosave() {}

void SceneOption::osSetSound(int) {}

#include "JSystem/JAudio/JASFakeMatch2.h"
