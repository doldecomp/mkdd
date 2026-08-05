#include "Shiraiwa/SecretScene.h"
#include "Inagaki/GameAudioMain.h"
#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DPane.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/J3D/J3DSys.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/JUtility/TColor.h"
#include "JSystem/ResTIMG.h"
#include "Kameda/CharacterSelect3D.h"
#include "Kameda/PrintWindow.h"
#include "Kameda/ReadPrintMessage.h"
#include "Kameda/Scene.h"
#include "Kameda/SequenceInfo.h"
#include "Osako/FontMgr.h"
#include "Osako/ResMgr.h"
#include "Osako/SequenceApp.h"
#include "Osako/SystemRecord.h"
#include "Osako/kartPad.h"
#include "Osako/system.h"
#include "Sato/JPEffectMgr.h"
#include "Sato/StateObserver.h"
#include "dolphin/mtx.h"
#include "kartEnums.h"
#include <cstring>

StateObserver::StateFuncSet<TAutoPWindow> TAutoPWindow::sTable[5] = {
    {0, &TAutoPWindow::initFunc_BeforeOpen, &TAutoPWindow::doFunc_Hide}, // ???
    {1, &TAutoPWindow::initFunc_Open, &TAutoPWindow::doFunc_Open},
    {2, &TAutoPWindow::initFunc_Display, &TAutoPWindow::doFunc_Display},
    {3, &TAutoPWindow::initFunc_Close, &TAutoPWindow::doFunc_Close},
    {4, &TAutoPWindow::initFunc_Hide, &TAutoPWindow::doFunc_Hide}
};

TAutoPWindow::TAutoPWindow(JKRHeap *heap, J2DScreen *screen, J2DAnmTransform *transform) {
    mPrintWindow = nullptr;
    mScreen = nullptr;
    mTransform = nullptr;
    _30 = nullptr;
    mStrBuf = nullptr;
    mBmg = nullptr;
    mBmc = nullptr;
    mPrintWindow = new (heap, 0) PrintWindow(heap);
    mBmg = ResMgr::getPtr(ResMgr::mcSecret);
    mBmc = ResMgr::getPtr(ResMgr::mcColor);
    mScreen = screen;
    mTransform = transform;
    parseBlo(heap);
}

TAutoPWindow::~TAutoPWindow() {
    delete mPrintWindow;
    delete[] mStrBuf;
}
    
void TAutoPWindow::parseBlo(JKRHeap *heap) {
    mStrBuf = new(heap, 0) char[0x100];

    for (int i = 0; i < 255; i++) {
        mStrBuf[i] = ' ';
    }
    mStrBuf[255] = '\0';
    _30 = (J2DTextBox*)mScreen->search('TGDEr2');
    _30->setFont(FontMgr::mspResFont);
    _30->setString(mStrBuf);
    mScreen->setAnimation(mTransform);    
}

void TAutoPWindow::reset() {
    StateObserver::ResetState();
    setFrames(0.0f, 0.0f);

    setType(1);
    mPrintWindow->getTextBox()->hide();
    _30->hide();
    mPrintWindow->_2f0 = 0;
    mPrintWindow->_2ec = 1;
    mPrintWindow->_2f8 = 2;
    mTransform->setFrame(0.0f);
    mScreen->animation();
    setAudioIDs(0, 0);
}

void TAutoPWindow::setType(u32 type) {
    mType = type;
    if (mType >= 2) {
        _34 = _30;
    }
    else {
        mPrintWindow->_2e8 = type;
        _34 = mPrintWindow->getTextBox();
    }
}

void TAutoPWindow::setAnimationFrame(f32 frame) {
    if (mType >= 2) {
        mTransform->setFrame(frame);
        mScreen->animation();
    }
    else {
        mPrintWindow->_2f4 = frame;
    }
}

void TAutoPWindow::calc() {
    StateObserver::ExecuteState();
    mPrintWindow->calc();
    mScreen->animation();
}

void TAutoPWindow::draw() {
    if (mType >= 2) {
        J2DOrthoGraph *ortho = System::getJ2DOrtho();
        ortho->setPort();
        mScreen->draw(0.0f, 0.0f, ortho);
    }
    else {
        mPrintWindow->draw();
    }
}

void TAutoPWindow::InitExec() {
    Observer_FindAndInit(TAutoPWindow, 5);
}

void TAutoPWindow::MoveExec() {
    Observer_FindAndExec(TAutoPWindow, 5);
}

void TAutoPWindow::initFunc_BeforeOpen() {}

void TAutoPWindow::doFunc_BeforeOpen() {}

void TAutoPWindow::initFunc_Hide() {
    _34->hide();
}

void TAutoPWindow::doFunc_Hide() {}

void TAutoPWindow::initFunc_Open() {
    setAnimationFrame(0.0f);
    if (mAudioIDOpen != 0) {
        GetGameAudioMain()->startSystemSe(mAudioIDOpen);
    }
}

void TAutoPWindow::doFunc_Open() {
    f32 frame = StateObserver::getStateCount();
    setAnimationFrame(frame);
    if (frame >= mOpenFrame) {
        StateObserver::setState(2);
    }
}

void TAutoPWindow::initFunc_Display() {
    _34->show();
}

void TAutoPWindow::setMessageID(u32 p1, int p2, int p3, int p4, int p5) {
    if (p1 == 1) {
        setMessage(mPrintWindow->getTextBox(), p2, p3, p4, p5);
    }
    else {
        setMessage(_30, p2, p3, p4, p5);
    }
}

void TAutoPWindow::setMessage(J2DTextBox *textBox, int p2, int p3, int p4, int p5) {
    if (p3 != 0) {
        ReadPrintMessage::mStr[0] = ReadPrintMessage::getMessage(mBmg, p3 & 0xffff);
    }
    if (p4 != 0) {
        ReadPrintMessage::mStr[1] = ReadPrintMessage::getMessage(mBmg, p4 & 0xffff);
    }
    if (p5 != 0) {
        ReadPrintMessage::mStr[2] = ReadPrintMessage::getMessage(mBmg, p5 & 0xffff);
    }

    J2DTextBox::TFontSize fsz;
    textBox->getFontSize(fsz);
    textBox->setCharSpace(ReadPrintMessage::getCharSpace(mBmg, p2 & 0xffff));
    char *msg = ReadPrintMessage::getMessage(mBmg, p2 & 0xffff);

    char buf[256];
    ReadPrintMessage::tagCnv(msg, mBmc, fsz, sizeof(buf), buf);
    std::strncpy(textBox->getStringPtr(), buf, sizeof(buf));
}

void TAutoPWindow::doFunc_Display() {
    if (gpaGamePad[0]->testTrigger(mSkipBtnMask)) { // force close
        StateObserver::setState(3);
    }
}

void TAutoPWindow::initFunc_Close() {
    _34->hide();
    setAnimationFrame(mCloseFrame);
    if (mAudioIDClose != 0) {
        GetGameAudioMain()->startSystemSe(mAudioIDClose);
    }
}

void TAutoPWindow::doFunc_Close() {
    f32 frame = mCloseFrame - StateObserver::getStateCount();
    setAnimationFrame(frame);
    if (frame <= 0.0f) {
        StateObserver::setState(4);
    }
}

const JGeometry::TVec3f SecretScene::scCameraPos(0.0f, 380.0f, 1000.0f);
const JGeometry::TVec3f SecretScene::scCameraTarget(0.0f, -40.0f, 0.0f);

const JGeometry::TVec3f SecretScene::scKartPos[13] = {
    JGeometry::TVec3f(0.0f, 83.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 90.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 88.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 85.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 95.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 100.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 88.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 88.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 88.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 88.0f, 170.0f),
    JGeometry::TVec3f(10.0f, 75.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 88.0f, 170.0f),
    JGeometry::TVec3f(0.0f, 88.0f, 170.0f),
};

const JGeometry::TVec3f SecretScene::scKartDeg[13] = {
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
    JGeometry::TVec3f(0.0f, 0.0f, 0.0f),
};

const JGeometry::TVec3f SecretScene::scCharPos[2][2] = {
    JGeometry::TVec3f(-150.0f, 140.0f, 350.0f),
    JGeometry::TVec3f(150.0f, 140.0f, 350.0f),
    JGeometry::TVec3f(-150.0f, 115.0f, 340.0f),
    JGeometry::TVec3f(140.0f, 100.0f, 340.0f),
};

const JGeometry::TVec3f SecretScene::scCharDeg[2][2] = {
    JGeometry::TVec3f(355.0f, 9.0f, 354.0f),
    JGeometry::TVec3f(355.0f, 348.0f, 4.0f),
    JGeometry::TVec3f(355.0f, 9.0f, 354.0f),
    JGeometry::TVec3f(340.0f, 315.0f, 4.0f),
};

const SecretScene::TMesgList SecretScene::scSecretGameMesgList1[9] = {
    {0x70004, 0x70009, 0x7000B, 0x0},
    {0x70005, 0x7000A, 0x00, 0x0},
    {0x70004, 0x70006, 0x7000C, 0x0},
    {0x70004, 0x7000A, 0x7000B, 0x0},
    {0x70004, 0x70009, 0x7000C, 0x0},
    {0x00, 0x00, 0x00, 0x0},
    {0x00, 0x00, 0x00, 0x0},
    {0x70004, 0x70008, 0x7000C, 0x0},
    {0x70005, 0x70007, 0x00, 0x0},
};

const SecretScene::TMesgList SecretScene::scSecretGameMesgList2[9] = {
    {0x70013, 0x70001, 0x70000, 0x0},
    {0x70013, 0x70002, 0x70003, 0x0},
    {0x70010, 0x00, 0x00, 0x0},
    {0x7000E, 0x70009, 0x00, 0x0},
    {0x7000E, 0x70006, 0x00, 0x0},
    {0x00, 0x00, 0x00, 0x0},
    {0x00, 0x00, 0x00, 0x0},
    {0x70012, 0x70021, 0x00, 0x0},
    {0x70012, 0x70020, 0x00, 0x0},
};

const SecretScene::TMesgList SecretScene::scSecretKartMesgList1[13] = {
    {0x70004, 0x70008, 0x7000B, 0x0},
    {0x70004, 0x7000A, 0x7000D, 0x0},
    {0x70004, 0x7000A, 0x7000C, 0x0},
    {0x70004, 0x70009, 0x7000D, 0x0},
    {0x00000, 0x00000, 0x00000, 0x0},
    {0x70005, 0x70008, 0x00000, 0x0},
    {0x70004, 0x70008, 0x7000D, 0x0},
    {0x70004, 0x70007, 0x7000D, 0x0},
    {0x70004, 0x70007, 0x7000C, 0x0},
    {0x70004, 0x70007, 0x7000B, 0x0},
    {0x00000, 0x00000, 0x00000, 0x0},
    {0x70005, 0x70009, 0x00000, 0x0},
    {0x70005, 0x70006, 0x00000, 0x0},
};

const SecretScene::TMesgList SecretScene::scSecretKartMesgList2[13] = {
    {0x7000F, 0x70014, 0x0, 0x0}, 
    {0x7000F, 0x7001D, 0x0, 0x0}, 
    {0x7000F, 0x70017, 0x0, 0x0},
    {0x7000F, 0x70018, 0x0, 0x0},
    {0x7000F, 0x7001A, 0x0, 0x0},
    {0x7000F, 0x70019, 0x0, 0x0},
    {0x7000F, 0x7001B, 0x0, 0x0},
    {0x7000F, 0x70016, 0x0, 0x0},
    {0x7000F, 0x70015, 0x0, 0x0},
    {0x7000F, 0x7001F, 0x0, 0x0},
    {0x7000F, 0x7001C, 0x0, 0x0},
    {0x7000F, 0x7001E, 0x0, 0x0},
    {0x70011, 0x00000, 0x0, 0x0},
};

const char *SecretScene::scBloFileName = "SecretInd.blo";
const char *SecretScene::scBckFileName = "SecretInd.bck";

f32 SecretScene::sScr1OpenFrame = 10.0f;
f32 SecretScene::sScr2OpenFrame = 10.0f;

const f32 SecretScene::scGPortWidth = System::get2DVpW();
const f32 SecretScene::scGPortHeight = System::get2DVpH();
const f32 SecretScene::scGPortH = SecretScene::scGPortHeight / 2.0f;
const f32 SecretScene::scAspect = SecretScene::scGPortWidth / SecretScene::scGPortHeight;

const f32 SecretScene::scKartScale[13] = {
    0.9f, 0.95f, 0.87f, 0.92f, 
    0.9f, 0.95f, 0.87f, 0.87f, 
    0.87f, 0.87f, 0.75f, 0.87f,
    0.87f,
};
const f32 SecretScene::scCharScale[2][2] = {
    {0.9f, 0.9f},
    {0.75f, 0.75f}
};

const char *SecretScene::scTextureName[5] = {
    "CupWithPict_Reverse2Cup.bti",
    "CupWithPict_SpecialCup.bti",
    "CupWithPict_Mirror.bti",
    "Mini1_nameSecret.bti",
    "Mini5_nameSecret.bti"
};

const EKartID SecretScene::scSecret2KartID[13] = {
    RATTLE_BUGGY,
    PARA_WING,
    BARREL_TRAIN,
    BULLET_BLASTER,
    TOAD_KART,
    TOADETTE_KART,
    GREEN_FIRE,
    BLOOM_COACH,
    TURBO_BIRDO,
    WALUIGI_RACER,
    PIRANHA_PIPES,
    BOO_PIPES,
    PARADE_KART
};

StateObserver::StateFuncSet<SecretScene> SecretScene::sTable[4] = {
    {0, &SecretScene::initFunc_FadeIn, &SecretScene::doFunc_FadeIn},
    {1, &SecretScene::initFunc_Screen1, &SecretScene::doFunc_Screen1},
    {2, &SecretScene::initFunc_Screen2, &SecretScene::doFunc_Screen2},
    {3, &SecretScene::initFunc_FadeOut, &SecretScene::doFunc_FadeOut}
};

SecretScene::SecretScene(JKRArchive *arc, JKRHeap *heap) : Scene(arc, heap) {
    mCharSelect3D = nullptr;
    mOrtho = nullptr;
    mEffectMgr = nullptr;
    mOrtho = new (heap, 0) J2DOrthoGraph(0.0f, 0.0f, scGPortWidth,scGPortHeight,-1.0f,1.0f);
    mOrtho->scissor(0.0f, 0.0f, scGPortWidth, scGPortHeight);

    mEffectMgr = new (heap, 0) JPEffectMgr(heap, true, 1);
    mCharSelect3D = CharacterSelect3D::create(heap);
#line 553
    JUT_ASSERT_MSG(mCharSelect3D, "Can't create CharcterSelect3D.")
    load(heap);
}

SecretScene::~SecretScene() {
    delete mAutoPWindow;
    delete mEffectMgr;
}

void SecretScene::init() {
    mCharSelect3D->init();
    reset();
}

void SecretScene::reset() {
    StateObserver::ResetState();
    mEffectMgr->reset_forMenu();
    mEmitter = nullptr;
    mAutoPWindow->reset();
    mMesgList = nullptr;

    gSequenceInfo.lockGame(SystemRecord::Ending); 
    gSequenceInfo.lockGame(SystemRecord::SpecialEnding);

    if (!gSequenceInfo.checkAnyUnlocked()) {
        StateObserver::setState(3);
    }
    
    checkFlag();
}

void SecretScene::checkFlag() {
    mMesgList = nullptr;
    mPicture->hide();

    int msgID = -1;

    if (gSequenceInfo.checkAnyGameUnlocked()) {
        for (int i = 0; i < 9; i++) {
            if (gSequenceInfo.tstGameUnlocked(i) && (i - 5) > 1u) {
                msgID = i;
                break;                
            }
        }
        if (msgID != -1) {
            switch (msgID) {
            case 0: {
                mMesgList = &scSecretKartMesgList2[4];
                _4c = 2;
                mCharIDs[0] = TOAD;
                mCharIDs[1] = TOADETTE;
                mSecretID = 4;
                gSequenceInfo.lockKart(4);
                break;
            }
            case 1: {
                mMesgList = &scSecretKartMesgList2[10];
                _4c = 2;
                mCharIDs[0] = PETEY;
                mCharIDs[1] = KING_BOO;
                mSecretID = 10;
                gSequenceInfo.lockKart(10);
                break;
            }
            default: {
                _4c = 0;
                mPicture->show();
                
                static const int isDisplayTexFlag[] = {
                    4, 3, 2, 7, 8
                };
                for (int i = 0; i < 5; i++) {
                    if (msgID == isDisplayTexFlag[i]) {
                        mPicture->changeTexture(mResource[i], 0);
                    }
                }
                break;
            }
            }
            setMessageID(1, scSecretGameMesgList1[msgID]);
            setMessageID(3, scSecretGameMesgList2[msgID]);
            gSequenceInfo.lockGame(msgID);
        }
    }

    if (msgID == -1 && gSequenceInfo.checkAnySecretKartUnlocked()) {
        for (int i = 0; i < 13; i++) {
            if (gSequenceInfo.tstSecretKartUnlocked(i)) {
                msgID = i;
                break;
            }
        }
        if (msgID != -1) {
            if (msgID == 4 || msgID == 10) {
#line 694
                JUT_ASSERT_MSG(false, "Wrong Flag.");
            }
            _4c = 1;
            mSecretID = msgID;
            setMessageID(1, scSecretKartMesgList1[msgID]);
            setMessageID(3, scSecretKartMesgList2[msgID]);
            gSequenceInfo.lockKart(msgID);
        }
    }

    if (_4c == 2) {
        for (int i = 0; i < 2; i++) {
            mCharSelect3D->setCharacterStatus(0, i, CharacterSelect3D::STATUS_0);
        }
    }
}

void SecretScene::load(JKRHeap *heap) {
    mScreen = new (heap, 0) J2DScreen();
    mScreen->set(scBloFileName, 0x40000, mArchive);
    mPicture = (J2DPictureEx*)mScreen->search('Banner');
    mPicture->remove((u8)0);
    
    for (u8 i = 0; i < 5; i++) {
#line 728
        mResource[i] = (const ResTIMG*)JKRGetNameResource(scTextureName[i], mArchive);
        JUT_ASSERT(mResource[i] != 0);
    }

    void *bck = JKRGetNameResource(scBckFileName, mArchive);
    JUT_ASSERT_F(bck, "Can\'t load %s.", scBckFileName);
    mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(bck);
    mAutoPWindow = new (heap, 0) TAutoPWindow(heap, mScreen, mTransform);
}

void SecretScene::draw() {
    mOrtho->place(0.0f, 0.0f, scGPortWidth, scGPortHeight);
    mOrtho->setPort();
    mCharSelect3D->draw(0, scAspect, scCameraPos, scCameraTarget);
    j3dSys.reinitGX();

    MtxPtr m = mCharSelect3D->getCameraMatrix();
    f32 y = mCharSelect3D->getFovy();
    JPADrawInfo drawInfo(m,  y, scAspect);

    mEffectMgr->getEm2DMgr()->draw(&drawInfo);
    System::getJ2DOrtho()->setPort();
    mAutoPWindow->draw();
}

void SecretScene::calc() {
    mEffectMgr->calc_forMenu();
    if (mEmitter == nullptr) {
        mEmitter = mEffectMgr->createEmt2D("mk_secretKami", scKartPos[4]);
    }

    switch (_4c) {
    case 0:
        break;
    case 1: {
        if (StateObserver::getState() >= 2) {
            mCharSelect3D->setKart(0, scSecret2KartID[mSecretID], scKartPos[mSecretID], scKartDeg[mSecretID], scKartScale[mSecretID]);
        }
        break;
    }
    case 2: {
        if (StateObserver::getState() >= 2) {
            for (int i = 0; i < 2; i++) {
                int idx = 0;
                if (mSecretID == 10) {
                    idx = 1;
                }
                    
                mCharSelect3D->setCharacterStatus(0, i, CharacterSelect3D::STATUS_1);
                mCharSelect3D->setCharacter(0, i, mCharIDs[i], scCharPos[idx][i], scCharDeg[idx][i], scCharScale[idx][i]);
                mCharSelect3D->setKart(0, scSecret2KartID[mSecretID], scKartPos[mSecretID], scKartDeg[mSecretID], scKartScale[mSecretID]);
            }
        }
    }
    }

    mCharSelect3D->calc();
    mAutoPWindow->calc();
    StateObserver::ExecuteState();
}

void SecretScene::InitExec() {
    Observer_FindAndInit(SecretScene, 4);
}

void SecretScene::MoveExec() {
    Observer_FindAndExec(SecretScene, 4);
}

void SecretScene::viewCalc() {}

void SecretScene::update() {}

void SecretScene::initFunc_FadeIn() {
    SYSTEM_StartFadeIn(15);
    
    SYSTEM_GetFader()->setColor(JUtility::TColor(0,0,0,0xff));
    GetGameAudioMain()->startStreamBgm(0x200002c);
}

#pragma push
#pragma dont_inline on // currently i don't have any idea on how to prevent this from inlining, so pragma it is
void SecretScene::setMessageID(u32 p1, const SecretScene::TMesgList &msgList) {
    mAutoPWindow->setMessageID(p1, msgList._0, msgList._4, msgList._8, msgList._c);
}
#pragma pop

void SecretScene::doFunc_FadeIn() {
    if (SYSTEM_IsFadingIn()) {
        StateObserver::setState(1);
    }
}

void SecretScene::initFunc_Screen1() {
    mAutoPWindow->setAudioIDs(0x2000B, 0);
    mAutoPWindow->setFrames(sScr1OpenFrame, sScr1OpenFrame);
    mAutoPWindow->setType(1);
    mAutoPWindow->setState(1);
}

void SecretScene::doFunc_Screen1() {
    if (mAutoPWindow->getState() == 4) {
        mAutoPWindow->setState(0);
        StateObserver::setState(2);
    }
}

void SecretScene::initFunc_Screen2() {
    if (mMesgList != nullptr || gSequenceInfo.checkAnyUnlocked()) {
        mAutoPWindow->setAudioIDs(0x2000B, 0);
    }
    else {
        mAutoPWindow->setAudioIDs(0x2000b, 0x20002);
    }
    mAutoPWindow->setFrames(sScr2OpenFrame, sScr2OpenFrame);
    mAutoPWindow->setType(3);
    mAutoPWindow->setState(1);
}

void SecretScene::doFunc_Screen2() {
    if (mAutoPWindow->getState() != 4) {
        return;
    }
    mAutoPWindow->setState(0);

    if (mMesgList) {
        setMessageID(3, *mMesgList);
        mMesgList = nullptr;
        initFunc_Screen2();
    }
    else if (gSequenceInfo.checkAnyUnlocked()) {
        reset();
    }
    else {
        StateObserver::setState(3);
    }
}

void SecretScene::initFunc_FadeOut() {
    SYSTEM_StartFadeOut(90);
    GetGameAudioMain()->fadeOutAll(90);
    mEffectMgr->deleteEmt2D(mEmitter);
}

void SecretScene::doFunc_FadeOut() {
    if (SEQUENCEAPP_isReady(SCENE_TITLE) && SYSTEM_IsFadingOut()) {
        SequenceApp::ptr()->setNextScene(SCENE_TITLE);
    }
}

#include "JSystem/JAudio/JASFakeMatch.h"
