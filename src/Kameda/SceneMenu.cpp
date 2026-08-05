#include "Kameda/SceneMenu.h"
#include "Kameda/Scene.h"

#include "kartEnums.h"
#include "mathHelper.h"

const u32 SceneMenu::mCharVoice[] = {
    0x00020014, 0x00020015, 0x00020012, 0x00020013, 
    0x00020018, 0x00020019, 0x0002000E, 0x0002000F, 
    0x0002001E, 0x0002001F, 0x00020011, 0x00020010, 
    0x0002001A, 0x0002001B, 0x0002001C, 0x0002001D, 
    0x00020016, 0x00020017, 0x00020020, 0x00020021, 
};

const u32 SceneMenu::mCharGoVoice[] = {
    0x0002004E, 0x0002004F, 0x0002004C, 0x0002004D, 
    0x00020052, 0x00020053, 0x00020048, 0x00020049, 
    0x00020058, 0x00020059, 0x0002004B, 0x0002004A, 
    0x00020054, 0x00020055, 0x00020056, 0x00020057, 
    0x00020050, 0x00020051, 0x0002005A, 0x0002005B, 
};

const int SceneMenu::mCharIndex[] = {
    0x00000007, 0x00000008, 0x00000005, 0x00000006, 
    0x0000000B, 0x0000000C, 0x00000001, 0x00000002, 
    0x00000011, 0x00000012, 0x00000004, 0x00000003, 
    0x0000000D, 0x0000000E, 0x0000000F, 0x00000010, 
    0x00000009, 0x0000000A, 0x00000014, 0x00000013, 
};

const char *SceneMenu::mCharName[] = {
    "ChaName_MARIO.bti", "ChaName_LUIGI.bti",
    "ChaName_PEACH.bti", "ChaName_DAISY.bti",
    "ChaName_YOSHI.bti", "ChaName_BIRDO.bti",
    "ChaName_BABY_MARIO.bti", "ChaName_BABY_LUIGI.bti",
    "ChaName_KINOPIO.bti", "ChaName_KINOPICO.bti",
    "ChaName_KOOPA.bti", "ChaName_PARATOOPA.bti",
    "ChaName_DONKEY_KONG.bti", "ChaName_DIDDY_KONG.bti",
    "ChaName_BOWSER.bti", "ChaName_BOWSER_JR.bti",
    "ChaName_WARIO.bti", "ChaName_WALUIGI.bti",
    "ChaName_PAKKUN.bti", "ChaName_TERESA.bti"
};

const int SceneMenu::mKartIndex[] = {
    0x00000005, 0x0000000D, 0x00000003, 0x0000000B, 
    0x00000009, 0x0000000F, 0x00000010, 0x00000011, 
    0x00000000, 0x00000008, 0x00000004, 0x0000000C, 
    0x00000002, 0x0000000A, 0x0000000E, 0x00000006, 
    0x00000001, 0x00000007, 0x00000013, 0x00000012, 
    0x00000014, 
};

const u8 SceneMenu::mKartPerf[] = {
    0x01, 0x05, 0x02, 0x02, 0x04, 0x02, 0x02, 0x04, 
    0x02, 0x01, 0x05, 0x02, 0x04, 0x02, 0x03, 0x04, 
    0x03, 0x01, 0x02, 0x04, 0x02, 0x01, 0x05, 0x02, 
    0x03, 0x03, 0x03, 0x04, 0x02, 0x02, 0x02, 0x04, 
    0x03, 0x03, 0x03, 0x02, 0x02, 0x04, 0x03, 0x03, 
    0x03, 0x04, 0x03, 0x03, 0x03, 0x04, 0x02, 0x04, 
    0x04, 0x02, 0x04, 0x05, 0x01, 0x05, 0x04, 0x02, 
    0x05, 0x02, 0x04, 0x05, 0x04, 0x03, 0x04,
};

const ERaceLevel SceneMenu::mLevelID[] = {
    LVL_50CC, LVL_100CC, LVL_150CC, LVL_MIRROR
};

const u64 SceneMenu::mPlayerIconBckTag[] = {
    'N_Stok', 'Cstok_p', 'Cstok_pb', 'Ns1234', 
    'Ns12_3_4', 'Ns12_34', 'SMpb1', 'SMpb2', 
    'SMpb3', 'SMpb4', 'SMpb5', 'SMpb6', 
    'SMpb7', 'SMpb8', 'SMpb9', 'SMpm1', 
    'SMpm2', 'SMpm3', 'SMpm4', 'SMpm5', 
    'SMpm6', 'SMpm7', 'SMpm8', 'SMpm9', 
    'SMpm10', 'SMpm11', 'SMpm12', 'SMm1', 
    'SMm2', 'SMm3', 'SMm4', 'SMm5', 
    'SMm6', 
};

const char *SceneMenu::mPlayerIconPlayerName[] = {
    "Player1.bti", 
    "Player2.bti",
    "Player3.bti",
    "Player4.bti"
};

const u64 SceneMenu::mHowManyDriversBckTag[] = {
    'N_Entry', 'Ecrsr1', 'Ecrsr2', 'Ecrsr3', 
    'Ecrsr4', 'ENplay1', 'ENplay2', 'ENplay3', 
    'ENplay4', 'Eplay1b', 'Eplay2b', 'Eplay3b', 
    'Eplay4b', 
};

const u64 SceneMenu::mHowManyDriversBrkTag[] = {
    'Ebar1', 'Ebar2', 'Ebar3', 'Ebar4', 
    'Eplay1', 'Eplay2', 'Eplay3', 'Eplay4', 
    'Eplay1b', 'Eplay2b', 'Eplay3b', 'Eplay4b', 
    'Emario11', 'Emario21', 'Emario22', 'Emario31', 
    'Emario32', 'Emario33', 'Emario41', 'Emario42', 
    'Emario43', 'Emario44',
};

const u64 SceneMenu::mSingleTagSelectBckTag[] = {
    'N_CoVs', 'N_C2p', 'C2crsr1', 'C2crsr2', 
    'C2chaw1N', 'C2chaw2N', 'C2chaw3N', 'CNP2M1', 
    'CNP2M2', 'CNm22p', 'C2pb1', 'C2pb2', 
    'N_C3p', 'C3crsr1', 'C3crsr2', 'CPcN31', 
    'CPcN32', 'CNP3M1', 'CNP3M2', 'C3pb1', 
    'C3pb2', 'C3pb3', 'C3pb4', 'C3pb5', 
    'N_C4p', 'C4crsr1', 'C4crsr2', 'C4crsr3', 
    'CPcN41', 'CPcN42', 'CPcN43', 'CNP4M1', 
    'CNP4M2', 'CNP4M3', 'C4pb1', 'C4pb2', 
    'C4pb3', 'C4pb4', 'C4pb5', 'C4pb6', 
    'C4pb7', 'C4pb8', 'C4pb9',
};

const u64 SceneMenu::mSingleTagSelectBtkTag[] = {
    'C2bar1', 'C2bar2', 'C2cha_w1', 'C2cha_w2', 
    'C2cha_w3', 'C3bar1', 'C3bar2', 'C3cha_w1', 
    'C3cha_w2', 'C3cha_w3', 'C3cha_w4', 'C3cha_w5', 
    'C4bar1', 'C4bar2', 'C4bar3', 'C4cha_w1', 
    'C4cha_w2', 'C4cha_w3', 'C4cha_w4', 'C4cha_w5', 
    'C4cha_w6', 'C4cha_w7', 'C4cha_w8', 'C4cha_w9', 
};

const u64 SceneMenu::mSingleTagSelectBrkTag[] = {
    'C2bar1', 'C2bar2', 'C2cha1', 'C2cha2', 
    'C2cha3', 'C2cha4', 'CNm22p', 'C2pm1', 
    'C2pm2', 'C2pm3', 'C2pm4', 'C2pb1', 
    'C2pb2', 'C2pvsm1', 'C3bar1', 'C3bar2', 
    'C3cha1', 'C3cha2', 'C3cha3', 'C3cha4', 
    'C3cha5', 'C3cha6', 'C3pm1', 'C3pm2', 
    'C3pm3', 'C3pm4', 'C3pm5', 'C3pm6', 
    'C3pb1', 'C3pb2', 'C3pb3', 'C3pb4', 
    'C3pb5', 'C3vsm1', 'C3vsm2', 'C3vsm4', 
    'C4bar1', 'C4bar2', 'C4bar3', 'C4cha1', 
    'C4cha2', 'C4cha3', 'C4cha4', 'C4cha5', 
    'C4cha6', 'C4cha7', 'C4cha8', 'C4cha9', 
    'C4cha10', 'C4cha11', 'C4cha12', 'C4pm1', 
    'C4pm2', 'C4pm3', 'C4pm4', 'C4pm5', 
    'C4pm6', 'C4pm7', 'C4pm8', 'C4pm9', 
    'C4pm10', 'C4pm11', 'C4pm12', 'C4pb1', 
    'C4pb2', 'C4pb3', 'C4pb4', 'C4pb5', 
    'C4pb6', 'C4pb7', 'C4pb8', 'C4pb9', 
    'C4vsm1', 'C4vsm2', 'C4vsm3', 'C4vsm4', 
    'C4vsm5', 'C4vsm6',
};

const u64 SceneMenu::mSingleTagSelectBpkTag[] = {
    'C2crsr1', 'C2crsr2', 'C3crsr1', 'C3crsr2', 
    'C4crsr1', 'C4crsr2', 'C4crsr3',
};

const u64 SceneMenu::mModeBckTag[] = {
    'N_Mode2', 'M4crsr1', 'M4crsr2', 'M4crsr3', 
    'MNgpP21', 'MNvsP21', 'MvsPN22', 'Mvsb2', 
    'MNminP21', 'MNminP22', 'MMinP21', 'MMinP22', 
    'MMinP23', 'Mgpm1N', 'MgpM21', 'MvsN', 
    'MvsM21', 'MminiN', 'MminM21', 'N_Pw_gp2', 
    'Mpow1', 'NPowL1', 'NPowR1', 'Mp2s1', 
    'Mp2s2', 'Mpow1m', 'N_Pw_vs', 'Mpow2', 
    'Mpow2m', 'Mp2s3', 'Mp2s4', 'NPowL2', 
    'NPowR2', 'N_Mode1', 'M4crsr4', 'M4crsr5', 
    'N_Mode11', 'MNgpP11', 'MNtaP11', 'MNtaPic', 
    'MNtaNe', 'MtaEye1', 'MtaEye2', 'MtaLi', 
    'MgpmN2', 'MgpM11', 'MtamN', 'MtaM11', 
    'N_Mode12', 'MNVspc1', 'MvsPN12', 'Mvsb1', 
    'MMinPN11', 'MMinPN12', 'MMinP11', 'MMinP12', 
    'MMinP13', 'MvsN1', 'MvsM12', 'MminiN1', 
    'MminiM12', 'N_Pw_gp1', 'NPowL3', 'NPowR3', 
    'Mp2s7', 'Mp2s8', 'Mp2s17', 'Mp2s18', 
    'Mpow4', 'Mpow1m1', 'MtaName', 'NNM1', 
    'NNM2', 'NNM3', 'OK_wb11',
};

const u64 SceneMenu::mModeBtkTag[] = {
    'M4bar1', 'M4bar2', 'M4bar3', 'Mgpbb', 
    'Mgpb1', 'C4cha_10', 'C4cha_11', 'C4cha_12', 
    'C4cha_13', 'MMinb1', 'Mp2s11', 'Mp2s12', 
    'Mp2s13', 'Mp2s14', 'Mgpbb11', 'Mgpb2', 
    'M4bar4', 'M4bar5', 'MMinb2', 'C4cha_01', 
    'C4cha_02', 'C4cha_03', 'C4cha_04', 'Mp2s17', 
    'Mp2s18', 'OK_w11', 
};

const u64 SceneMenu::mModeBrkTag[] = {
    'M4bar1', 'M4bar2', 'M4bar3', 'MMinb1', 
    'MgpM21', 'Mgpm1', 'MvsM21', 'Mvs', 
    'MminM21', 'Mmini', 'Mp2s1', 'Mp2s2', 
    'Mp2s3', 'Mp2s4', 'Mp2s11', 'Mp2s12', 
    'MgpM11', 'Mgpm2', 'MtaM11', 'Mtam', 
    'M4bar4', 'M4bar5', 'MMinb2', 'MvsM12', 
    'Mvs1', 'MminiM12', 'Mmini1', 'Mp2s7', 
    'Mp2s8', 'OK_wb11',
};

const u64 SceneMenu::mModeBpkTag[] = {
    'M4crsr1', 'M4crsr2', 'M4crsr3', 'M4crsr4', 
    'M4crsr5', 'MtaLi', 
};

const char *SceneMenu::mPowerMojiName[] = {
    "Power_Mozi.1.bti",
    "Power_Mozi.2.bti",
    "Power_Mozi.3.bti",
    "Power_Mozi.4.bti"
};

const char *SceneMenu::mPowerPicName[] = {
    "Entry_Engine1.bti",
    "Entry_Engine2.bti",
    "Entry_Engine3.bti",
    "Entry_Engine4.bti",
};

const char *SceneMenu::mBattleName[] = {
    "BattleSnap1.bti",
    "BattleSnap2.bti",
    "BattleSnap3.bti"
};

const u64 SceneMenu::mBattleBckTag[] = {
    'NSlBtl', 'Curs1', 'Curs2', 'Curs3', 
    'Bar1', 'Bar2', 'Bar3', 'Word1', 
    'Word2', 'Word3', 'BtlP1', 'BtlP2', 
    'BtlP3', 'NPict', 'BtlPict',
};

const u64 SceneMenu::mBattleBrkTag[] = {
    'Bar1', 'Bar2', 'Bar3'
};

const u64 SceneMenu::mCharCCBckTag[] = {
    'Cha1', 'Cha2', 
};

const u64 SceneMenu::mCharCCBrkTag[] = {
    'CW1', 'CW2', 'CWC1', 'CWC2', 
};

const u64 SceneMenu::mCharCCBpkTag[] = {
    'CW1', 'CW2', 'CP1', 'CP2', 
    'CC1', 'CC2', 
};

const char *SceneMenu::mCharCCName[] = {
    "SC_P1_u.bti",
    "SC_P2_u.bti",
    "SC_P3_u.bti",
    "SC_P4_u.bti",
    "SC_P1_s.bti",
    "SC_P2_s.bti",
    "SC_P3_s.bti",
    "SC_P4_s.bti",
};

const u64 SceneMenu::mChar1BckTag[] = {
    'N_HC1', 'C11P1', 'C11P2', 'HC11w', 
    'N_P1', 'ArL11', 'ArR11', 'Wait11', 
    'NRandom',
};

const u64 SceneMenu::mChar1PlusBckTag[] = {
    'N11F', 'N11H', 'N11B', 'N_MP11', 
    'MP11S1', 'MP11S2', 'MP11S3', 'MP11S4', 
    'MP11S5', 'MP11A1', 'MP11A2', 'MP11A3', 
    'MP11A4', 'MP11A5', 'MP11W1', 'MP11W2', 
    'MP11W3', 'MP11W4', 'MP11W5',
};

const u64 SceneMenu::mChar1PlusBrkTag[] = {
    'N11F', 'N11B', 
};

const u64 SceneMenu::mChar2BckTag[] = {
    'N_HC2', 'C21P1', 'C21P2', 'C22P1', 
    'C22P2', 'HC21w', 'HC22w', 'N_P21', 
    'N_P22', 'OK_wb21', 'OK_wb22', 'OK_w21', 
    'OK_w22', 'ArL21', 'ArL22', 'ArR21', 
    'ArR22', 'Wait21', 'Wait22', 'NRandom', 
};

const u64 SceneMenu::mChar2BrkTag[] = {
   'C22P1', 'C22P2', 'HC22w', 'OK_w22', 
    'Ksb22', 
};

const u64 SceneMenu::mChar3BckTag[] = {
    'N_HC3', 'C31P1', 'C31P2', 'C32P1', 
    'C33P1', 'HC31w', 'HC32w', 'HC33w', 
    'N_P31', 'N_P32', 'N_P33', 'OK_wb31', 
    'OK_wb32', 'OK_wb33', 'OK_w31', 'OK_w32', 
    'OK_w33', 'ArL31', 'ArL32', 'ArL33', 
    'ArR31', 'ArR32', 'ArR33', 'Wait31', 
    'Wait32', 'Wait33', 'NRandom', 
};

const u64 SceneMenu::mChar3BrkTag[] = {
    'C32P1', 'C33P1', 'HC32w', 'HC33w', 
    'Ksb32', 'Ksb33', 'OK_w32', 'OK_w33', 
};

const u64 SceneMenu::mChar4BckTag[] = {
    'N_HC4', 'C41P1', 'C42P1', 'C43P1', 
    'C44P1', 'HC41w', 'HC42w', 'HC43w', 
    'HC44w', 'N_P41', 'N_P42', 'N_P43', 
    'N_P44', 'ArL41', 'ArL42', 'ArL43', 
    'ArL44', 'ArR41', 'ArR42', 'ArR43', 
    'ArR44', 'Wait41', 'Wait42', 'Wait43', 
    'Wait44', 'NRandom', 
};

SceneMenu::SceneMenu(JKRArchive *archive, JKRHeap *heap) : Scene(archive, heap) {
    /*
    J2DGrafContext::scissor(float, float, float, float) {}
    J2DTextBox::setFontColor(JUtility::TColor, JUtility::TColor) {}
    J2DTextBox::getFontSize(J2DTextBox::TFontSize &) const {}
    Kart2DCommon::getCharIconTexture(ECharID) {}
    MenuTitleLine::create(JKRArchive *, JKRHeap *) {}
    MenuBackground::create(JKRArchive *) {}
    CreateJPAMgr(JKRHeap *, bool) {}
    CharacterSelect3D::create(JKRHeap *) {}
    */
}

void SceneMenu::reset() {}

void SceneMenu::init() {}

SceneMenu::~SceneMenu() {}

void SceneMenu::draw() {}

void SceneMenu::calc() {}

void SceneMenu::calcModeAnm() {}

void SceneMenu::caleEngine(SceneMenu::BckData *) {}

void SceneMenu::calcEngineMoji(SceneMenu::BckData *) {}

void SceneMenu::calcEnginePic(SceneMenu::BckData *) {}

void SceneMenu::calcEngineBrkAnm(SceneMenu::BrkData *) {}

void SceneMenu::calcModeOrder() {}

void SceneMenu::entry_init() {}

void SceneMenu::entry() {}

void SceneMenu::entry_end() {}

void SceneMenu::vs_init() {}

void SceneMenu::vs() {}

void SceneMenu::vs_end() {}

void SceneMenu::mode_init() {}

void SceneMenu::mode() {}

void SceneMenu::mode_end() {}

void SceneMenu::battle_init() {}

void SceneMenu::battle() {}

void SceneMenu::battle_end() {}

void SceneMenu::character_init() {}

void SceneMenu::character() {}

void SceneMenu::character_end() {}

void SceneMenu::nextScene() {}

void SceneMenu::selectA(int) {}

void SceneMenu::selectB(int) {}

void SceneMenu::selectUp(int) {}

void SceneMenu::selectDown(int) {}

void SceneMenu::selectLeft(int) {}

void SceneMenu::selectRight(int) {}

void SceneMenu::selectZ(int) {}

void SceneMenu::isSelectAnm(int) {}

void SceneMenu::moveCursor(int, SceneMenu::Cursor) {}

void SceneMenu::isRndRoulette() {}

void SceneMenu::rndRoulette() {}

void SceneMenu::rndSelect() {}

void SceneMenu::setCharacterFlag() {}

void SceneMenu::getKart(int, int) {}

void SceneMenu::changeKart(int, int, int, int) {}

void SceneMenu::characterAnm() {}

void SceneMenu::characterOkWaitAnm() {}

void SceneMenu::setChar1Plus() {}

void SceneMenu::setRaceData() {}

void SceneMenu::setCharSelFlag() {}

void SceneMenu::useCharSel(int, int) {}

void SceneMenu::getCharSel(int &, int &) {}

void SceneMenu::getRndCharSel(int &, int &) {}

void SceneMenu::changeText() {}

#include "JSystem/JAudio/JASFakeMatch2.h"
