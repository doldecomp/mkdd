#include "Kameda/Pause2D.h"

#include "Inagaki/GameAudioMain.h"
#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "JSystem/JMath/JMath.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/PauseManager.h"
#include "Kameda/ReadPrintMessage.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/FontMgr.h"
#include "Osako/ResMgr.h"
#include "Osako/system.h"
#include "mathHelper.h"  // For unused data
#include "string.h"

int Pause2D::mPauseSelector;
Pause2D::PauseStatus Pause2D::mStatus;

Pause2DParam::Pause2DParam() : mFlag(1) {}

Pause2D::Pause2D(JKRHeap *heap)
{
    mOrthoGraph = System::getJ2DOrtho();
    mHioNode = new (heap, 0) Pause2DHioNode();

    switch (RCMGetManager()->getRaceMode())
    {
    case GRAND_PRIX:
    case AWARD_DEMO:
    case STAFF_ROLL:
    {
        mScreen = new (heap, 0) J2DScreen();
        mScreen->set("pause2.blo", 0x40000, PauseManager::getManager()->getArchive());

        void *bck = JKRGetNameResource("pause2.bck", PauseManager::getManager()->getArchive());
        mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(bck);
        mScreen->setAnimation(mTransform);

        mColor = (J2DAnmColor *)J2DAnmLoaderDataBase::load(
            JKRGetNameResource("pause2.bpk", PauseManager::getManager()->getArchive()));
        mScreen->setAnimation(mColor);

        for (int i = 0; i < 2; ++i)
        {
            mPaneLines[i].mPane = mScreen->search((u64)'Line1' + (u64)i);
            mPaneLines[i].mAnm = J2DAnmLoaderDataBase::load(bck);
            mPaneLines[i].mPane->setAnimation((J2DAnmTransform *)mPaneLines[i].mAnm);
        }

        for (int i = 0; i < 4; ++i)
        {
            J2DPane *pane = mScreen->search((u64)'kr_a' + (u64)i);
#line 91
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mPaneStarsPos[i] = pane;
            mStarsPos[i].x = mPaneStarsPos[i]->getTranslateX();
            mStarsPos[i].y = mPaneStarsPos[i]->getTranslateY();
            mPaneStarsPos[i]->hide();
        }

        for (int i = 0; i < 2; ++i)
        {
            J2DPicture *pane = (J2DPicture *)mScreen->search((u64)'PM1' + (u64)i);
#line 101
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mMenuEntries[i] = pane;
        }

        mMenuEntries[0]->changeTexture("Mozi_Continue.bti", 0);
        mMenuEntries[1]->changeTexture("Mozi_Quit.bti", 0);

        setPadText();

        mSetDraw = &Pause2D::setDrawGP;
        mSelector = &Pause2D::selectorGP;

        break;
    }
    case VERSUS_RACE:
    {
        mScreen = new (heap, 0) J2DScreen();
        mScreen->set("pause5.blo", 0x40000, PauseManager::getManager()->getArchive());

        void *bck = JKRGetNameResource("pause5.bck", PauseManager::getManager()->getArchive());
        mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(bck);
        mScreen->setAnimation(mTransform);

        mColor = (J2DAnmColor *)J2DAnmLoaderDataBase::load(
            JKRGetNameResource("pause5.bpk", PauseManager::getManager()->getArchive()));
        mScreen->setAnimation(mColor);

        for (int i = 0; i < 5; ++i)
        {
            mPaneLines[i].mPane = mScreen->search((u64)'Line1' + (u64)i);
            mPaneLines[i].mAnm = J2DAnmLoaderDataBase::load(bck);
            mPaneLines[i].mPane->setAnimation((J2DAnmTransform *)mPaneLines[i].mAnm);
        }

        for (int i = 0; i < 10; ++i)
        {
            J2DPane *pane = mScreen->search((u64)'kr_a' + (u64)i);
#line 135
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mPaneStarsPos[i] = pane;
            mStarsPos[i].x = mPaneStarsPos[i]->getTranslateX();
            mStarsPos[i].y = mPaneStarsPos[i]->getTranslateY();
            mPaneStarsPos[i]->hide();
        }

        for (int i = 0; i < 5; ++i)
        {
            J2DPicture *pane = (J2DPicture *)mScreen->search((u64)'PM1' + (u64)i);
#line 145
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mMenuEntries[i] = pane;
        }

        mSetDraw = &Pause2D::setDrawVS;
        mSelector = &Pause2D::selectorVS;

        break;
    }
    case TIME_ATTACK:
    {
        mScreen = new (heap, 0) J2DScreen();
        mScreen->set("pause5.blo", 0x40000, PauseManager::getManager()->getArchive());

        void *bck = JKRGetNameResource("pause5.bck", PauseManager::getManager()->getArchive());
        mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(bck);
        mScreen->setAnimation(mTransform);

        mColor = (J2DAnmColor *)J2DAnmLoaderDataBase::load(
            JKRGetNameResource("pause5.bpk", PauseManager::getManager()->getArchive()));
        mScreen->setAnimation(mColor);

        for (int i = 0; i < 5; ++i)
        {
            mPaneLines[i].mPane = mScreen->search((u64)'Line1' + (u64)i);
            mPaneLines[i].mAnm = J2DAnmLoaderDataBase::load(bck);
            mPaneLines[i].mPane->setAnimation((J2DAnmTransform *)mPaneLines[i].mAnm);
        }

        for (int i = 0; i < 10; ++i)
        {
            J2DPane *pane = mScreen->search((u64)'kr_a' + (u64)i);
#line 172
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mPaneStarsPos[i] = pane;
            mStarsPos[i].x = mPaneStarsPos[i]->getTranslateX();
            mStarsPos[i].y = mPaneStarsPos[i]->getTranslateY();
            mPaneStarsPos[i]->hide();
        }

        for (int i = 0; i < 5; ++i)
        {
            J2DPicture *pane = (J2DPicture *)mScreen->search((u64)'PM1' + (u64)i);
#line 182
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mMenuEntries[i] = pane;
        }

        mSetDraw = &Pause2D::setDrawTA;
        mSelector = &Pause2D::selectorTA;

        break;
    }
    case BALLOON_BATTLE:
    case BOMB_BATTLE:
    case ESCAPE_BATTLE:
    {
        mScreen = new (heap, 0) J2DScreen();
        mScreen->set("pause6.blo", 0x40000, PauseManager::getManager()->getArchive());

        void *bck = JKRGetNameResource("pause6.bck", PauseManager::getManager()->getArchive());
        mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(bck);
        mScreen->setAnimation(mTransform);

        mColor = (J2DAnmColor *)J2DAnmLoaderDataBase::load(
            JKRGetNameResource("pause6.bpk", PauseManager::getManager()->getArchive()));
        mScreen->setAnimation(mColor);

        for (int i = 0; i < 6; ++i)
        {
            mPaneLines[i].mPane = mScreen->search((u64)'Line1' + (u64)i);
            mPaneLines[i].mAnm = J2DAnmLoaderDataBase::load(bck);
            mPaneLines[i].mPane->setAnimation((J2DAnmTransform *)mPaneLines[i].mAnm);
        }

        for (int i = 0; i < 12; ++i)
        {
            J2DPane *pane = mScreen->search((u64)'kr_a' + (u64)i);
#line 211
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mPaneStarsPos[i] = pane;
            mStarsPos[i].x = mPaneStarsPos[i]->getTranslateX();
            mStarsPos[i].y = mPaneStarsPos[i]->getTranslateY();
            mPaneStarsPos[i]->hide();
        }

        for (int i = 0; i < 6; ++i)
        {
            J2DPicture *pane = (J2DPicture *)mScreen->search((u64)'PM1' + (u64)i);
#line 221
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mMenuEntries[i] = pane;
        }

        mMenuEntries[2]->changeTexture("Mozi_Change_Map.bti", 0);
        mMenuEntries[3]->changeTexture("Mozi_Change_Character.bti", 0);
        mMenuEntries[4]->changeTexture("Mozi_Change_MiniGame.bti", 0);

        mSetDraw = &Pause2D::setDrawMG;
        mSelector = &Pause2D::selectorMG;

        break;
    }
    default:
#line 233
        JUT_ASSERT(0);
        break;
    }

    mPaneStarsPosOffset = mScreen->search((u64)'kr');

    for (int i = 0; i < 2; ++i)
    {
        J2DPane *pane = mScreen->search((u64)'star1' + (u64)i);
#line 243
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mStars[i] = pane;
    }

    init();
}

void Pause2D::init()
{
    mStatus = PAUSE_ZERO;
    _14 = -1;
    _18 = 0;

    for (int i = 0; i < 2; ++i)
    {
        mAnmFrame[i] = 0;
    }

    switch (RCMGetManager()->getRaceMode())
    {
    case TIME_ATTACK:
    case GRAND_PRIX:
    case VERSUS_RACE:
    case BALLOON_BATTLE:
    case BOMB_BATTLE:
    case ESCAPE_BATTLE:
    case AWARD_DEMO:
    case STAFF_ROLL:
        mPauseSelector = 0;
        break;
    default:
#line 277
        JUT_ASSERT(0);
        break;
    }

    mCounter = 0;
    mInitialPos[0] = mStarsPos[0];
    mTargetPos[0] = mStarsPos[0];
    mInitialPos[1] = mStarsPos[1];
    mTargetPos[1] = mStarsPos[1];
    mTransform->setFrame(0.0f);
    mColor->setFrame(0.0f);

    for (int i = 0; i < 6; ++i)
    {
        mPaneLines[i].mScale = 0.0f;
    }

    mScreen->animation();
    (this->*mSetDraw)();
}

Pause2D::~Pause2D()
{
    delete mHioNode;
    // Symbol not available; implementation incomplete.
}

void Pause2D::draw()
{
    if ((mHioNode->mParam.mFlag & 1) && PauseManager::tstPause())
    {
        mOrthoGraph->setPort();
        mScreen->draw(0.0f, 0.0f, mOrthoGraph);
    }
}

void Pause2D::calc(KartGamePad *pad)
{
    if (!(mHioNode->mParam.mFlag & 1))
        return;

    if (pad != nullptr && mStatus != PAUSE_FOUR && mCounter == 0)
    {
        (this->*mSelector)(pad);
    }

    switch (mStatus)
    {
    case PAUSE_ZERO:
        break;
    case PAUSE_ONE:
        if (++_14 == 9)
        {
            mStatus = PAUSE_TWO;
        }
        break;
    case PAUSE_TWO:
        if (++_18 < 30)
        {
            _14 = 9;
        }
        else
        {
            mStatus = PAUSE_THREE;
        }
        break;
    case PAUSE_THREE:
        _14 = 9;
        break;
    case PAUSE_FOUR:
        if (++_14 >= 20)
        {
            mStatus = PAUSE_ZERO;
        }
        break;
    default:
#line 370
        JUT_ASSERT(0);
        break;
    }

    mTransform->setFrame((f32)_14);
    mColor->setFrame((f32)_14);
    mScreen->animation();

    for (int i = 0; i < 2; ++i)
    {
        s32 frame = mAnmFrame[i] + 1;
        mAnmFrame[i] = frame;
        mAnmFrame[i] = frame % 50;
        mScale[i] = getScale(mAnmFrame[i]);
    }

    (this->*mSetDraw)();
}

void Pause2D::selectorGP(KartGamePad *pad)
{
    if (pad->testRepeat(JUTGamePad::MAINSTICK_UP))
    {
        switch (mPauseSelector)
        {
        case 0:
            mPauseSelector = 8;
            break;
        case 8:
            mPauseSelector = 0;
            break;
        default:
#line 405
            JUT_ASSERT(0);
            break;
        }
        selectorCommon();
    }
    else if (pad->testRepeat(JUTGamePad::MAINSTICK_DOWN))
    {
        switch (mPauseSelector)
        {
        case 0:
            mPauseSelector = 8;
            break;
        case 8:
            mPauseSelector = 0;
            break;
        default:
#line 419
            JUT_ASSERT(0);
            break;
        }
        selectorCommon();
    }
}

void Pause2D::selectorVS(KartGamePad *pad)
{
    if (pad->testRepeat(JUTGamePad::MAINSTICK_UP))
    {
        switch (mPauseSelector)
        {
        case 8:
            mPauseSelector = 8;
            break;
        case 0:
            mPauseSelector = 0;
            break;
        case 1:
            mPauseSelector = 1;
            break;
        case 2:
            mPauseSelector = 2;
            break;
        case 4:
            mPauseSelector = 4;
            break;
        default:
#line 453
            JUT_ASSERT(0);
            break;
        }
        selectorCommon();
    }
    else if (pad->testRepeat(JUTGamePad::MAINSTICK_DOWN))
    {
        switch (mPauseSelector)
        {
        case 1:
            mPauseSelector = 1;
            break;
        case 2:
            mPauseSelector = 2;
            break;
        case 4:
            mPauseSelector = 4;
            break;
        case 8:
            mPauseSelector = 8;
            break;
        case 0:
            mPauseSelector = 0;
            break;
        default:
#line 476
            JUT_ASSERT(0);
            break;
        }
        selectorCommon();
    }
}

void Pause2D::selectorTA(KartGamePad *pad)
{
    if (pad->testRepeat(JUTGamePad::MAINSTICK_UP))
    {
        switch (mPauseSelector)
        {
        case 8:
            mPauseSelector = 8;
            break;
        case 0:
            mPauseSelector = 0;
            break;
        case 1:
            mPauseSelector = 1;
            break;
        case 2:
            mPauseSelector = 2;
            break;
        case 4:
            mPauseSelector = 4;
            break;
        default:
#line 510
            JUT_ASSERT(0);
            break;
        }
        selectorCommon();
    }
    else if (pad->testRepeat(JUTGamePad::MAINSTICK_DOWN))
    {
        switch (mPauseSelector)
        {
        case 1:
            mPauseSelector = 1;
            break;
        case 2:
            mPauseSelector = 2;
            break;
        case 4:
            mPauseSelector = 4;
            break;
        case 8:
            mPauseSelector = 8;
            break;
        case 0:
            mPauseSelector = 0;
            break;
        default:
#line 533
            JUT_ASSERT(0);
            break;
        }
        selectorCommon();
    }
}

void Pause2D::selectorMG(KartGamePad *pad)
{
    if (pad->testRepeat(JUTGamePad::MAINSTICK_UP))
    {
        switch (mPauseSelector)
        {
        case 8:
            mPauseSelector = 8;
            break;
        case 0:
            mPauseSelector = 0;
            break;
        case 1:
            mPauseSelector = 1;
            break;
        case 3:
            mPauseSelector = 3;
            break;
        case 4:
            mPauseSelector = 4;
            break;
        case 6:
            mPauseSelector = 6;
            break;
        default:
#line 570
            JUT_ASSERT(0);
            break;
        }
        selectorCommon();
    }
    else if (pad->testRepeat(JUTGamePad::MAINSTICK_DOWN))
    {
        switch (mPauseSelector)
        {
        case 1:
            mPauseSelector = 1;
            break;
        case 3:
            mPauseSelector = 3;
            break;
        case 4:
            mPauseSelector = 4;
            break;
        case 6:
            mPauseSelector = 6;
            break;
        case 8:
            mPauseSelector = 8;
            break;
        case 0:
            mPauseSelector = 0;
            break;
        default:
#line 596
            JUT_ASSERT(0);
            break;
        }
        selectorCommon();
    }
}

void Pause2D::selectorCommon()
{
    mAnmFrame[0] = 0;
    mAnmFrame[1] = 0;
    GameAudio::Main::getAudio()->startSystemSe(0x20000);
    mCounter = 1;
}

void Pause2D::setDrawGP()
{
    switch (mPauseSelector)
    {
    case 0:
    {
        mMenuEntries[0]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[0];
        mTargetPos[1] = mStarsPos[1];

        if (++mPaneLines[0].mScale > 7.0f)
        {
            mPaneLines[0].mScale = 7.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        break;
    }
    case 8:
    {
        mMenuEntries[1]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[2];
        mTargetPos[1] = mStarsPos[3];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (++mPaneLines[1].mScale > 7.0f)
        {
            mPaneLines[1].mScale = 7.0f;
        }
        break;
    }
    default:
#line 643
        JUT_ASSERT(0);
        break;
    }

    for (int i = 0; i < 2; ++i)
    {
        mPaneLines[i].mAnm->setFrame(mPaneLines[i].mScale);
    }

    setDrawCommon();
}

void Pause2D::setDrawVS()
{
    switch (mPauseSelector)
    {
    case 0:
    {
        mMenuEntries[0]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[0];
        mTargetPos[1] = mStarsPos[1];

        if (++mPaneLines[0].mScale > 7.0f)
        {
            mPaneLines[0].mScale = 7.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }

        break;
    }
    case 1:
    {
        mMenuEntries[1]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[2];
        mTargetPos[1] = mStarsPos[3];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (++mPaneLines[1].mScale > 7.0f)
        {
            mPaneLines[1].mScale = 7.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }

        break;
    }
    case 2:
    {
        mMenuEntries[2]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[4];
        mTargetPos[1] = mStarsPos[5];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (++mPaneLines[2].mScale > 7.0f)
        {
            mPaneLines[2].mScale = 7.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }

        break;
    }
    case 4:
    {
        mMenuEntries[3]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[6];
        mTargetPos[1] = mStarsPos[7];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (++mPaneLines[3].mScale > 7.0f)
        {
            mPaneLines[3].mScale = 7.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }

        break;
    }
    case 8:
    {
        mMenuEntries[4]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[8];
        mTargetPos[1] = mStarsPos[9];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (++mPaneLines[4].mScale > 7.0f)
        {
            mPaneLines[4].mScale = 7.0f;
        }

        break;
    }
    default:
#line 716
        JUT_ASSERT(0);
        break;
    }

    for (int i = 0; i < 5; ++i)
    {
        mPaneLines[i].mAnm->setFrame(mPaneLines[i].mScale);
    }

    setDrawCommon();
}

void Pause2D::setDrawTA()
{
    switch (mPauseSelector)
    {
    case 0:
    {
        mMenuEntries[0]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[0];
        mTargetPos[1] = mStarsPos[1];

        if (++mPaneLines[0].mScale > 7.0f)
        {
            mPaneLines[0].mScale = 7.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }

        break;
    }
    case 1:
    {
        mMenuEntries[1]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[2];
        mTargetPos[1] = mStarsPos[3];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (++mPaneLines[1].mScale > 7.0f)
        {
            mPaneLines[1].mScale = 7.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }

        break;
    }
    case 2:
    {
        mMenuEntries[2]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[4];
        mTargetPos[1] = mStarsPos[5];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (++mPaneLines[2].mScale > 7.0f)
        {
            mPaneLines[2].mScale = 7.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }

        break;
    }
    case 4:
    {
        mMenuEntries[3]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[6];
        mTargetPos[1] = mStarsPos[7];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (++mPaneLines[3].mScale > 7.0f)
        {
            mPaneLines[3].mScale = 7.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }

        break;
    }
    case 8:
    {
        mMenuEntries[4]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[8];
        mTargetPos[1] = mStarsPos[9];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (++mPaneLines[4].mScale > 7.0f)
        {
            mPaneLines[4].mScale = 7.0f;
        }

        break;
    }
    default:
#line 788
        JUT_ASSERT(0);
        break;
    }

    for (int i = 0; i < 5; ++i)
    {
        mPaneLines[i].mAnm->setFrame(mPaneLines[i].mScale);
    }

    setDrawCommon();
}

void Pause2D::setDrawMG()
{
    switch (mPauseSelector)
    {
    case 0:
    {
        mMenuEntries[0]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[0];
        mTargetPos[1] = mStarsPos[1];

        if (++mPaneLines[0].mScale > 7.0f)
        {
            mPaneLines[0].mScale = 7.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }
        if (--mPaneLines[5].mScale < 0.0f)
        {
            mPaneLines[5].mScale = 0.0f;
        }

        break;
    }
    case 1:
    {
        mMenuEntries[1]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[2];
        mTargetPos[1] = mStarsPos[3];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (++mPaneLines[1].mScale > 7.0f)
        {
            mPaneLines[1].mScale = 7.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }
        if (--mPaneLines[5].mScale < 0.0f)
        {
            mPaneLines[5].mScale = 0.0f;
        }

        break;
    }
    case 3:
    {
        mMenuEntries[2]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[4];
        mTargetPos[1] = mStarsPos[5];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (++mPaneLines[2].mScale > 7.0f)
        {
            mPaneLines[2].mScale = 7.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }
        if (--mPaneLines[5].mScale < 0.0f)
        {
            mPaneLines[5].mScale = 0.0f;
        }

        break;
    }
    case 4:
    {
        mMenuEntries[3]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[6];
        mTargetPos[1] = mStarsPos[7];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (++mPaneLines[3].mScale > 7.0f)
        {
            mPaneLines[3].mScale = 7.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }
        if (--mPaneLines[5].mScale < 0.0f)
        {
            mPaneLines[5].mScale = 0.0f;
        }

        break;
    }
    case 6:
    {
        mMenuEntries[4]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[8];
        mTargetPos[1] = mStarsPos[9];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (++mPaneLines[4].mScale > 7.0f)
        {
            mPaneLines[4].mScale = 7.0f;
        }
        if (--mPaneLines[5].mScale < 0.0f)
        {
            mPaneLines[5].mScale = 0.0f;
        }

        break;
    }
    case 8:
    {
        mMenuEntries[5]->updateScale(mScale[0]);
        mTargetPos[0] = mStarsPos[10];
        mTargetPos[1] = mStarsPos[11];

        if (--mPaneLines[0].mScale < 0.0f)
        {
            mPaneLines[0].mScale = 0.0f;
        }
        if (--mPaneLines[1].mScale < 0.0f)
        {
            mPaneLines[1].mScale = 0.0f;
        }
        if (--mPaneLines[2].mScale < 0.0f)
        {
            mPaneLines[2].mScale = 0.0f;
        }
        if (--mPaneLines[3].mScale < 0.0f)
        {
            mPaneLines[3].mScale = 0.0f;
        }
        if (--mPaneLines[4].mScale < 0.0f)
        {
            mPaneLines[4].mScale = 0.0f;
        }
        if (++mPaneLines[5].mScale > 7.0f)
        {
            mPaneLines[5].mScale = 7.0f;
        }

        break;
    }
    default:
#line 880
        JUT_ASSERT(0);
        break;
    }

    for (int i = 0; i < 6; ++i)
    {
        mPaneLines[i].mAnm->setFrame(mPaneLines[i].mScale);
    }

    setDrawCommon();
}

void Pause2D::setDrawCommon()
{
    for (int i = 0; i < 2; ++i)
    {
        if (mCounter < 8)
        {
            JGeometry::TVec2f advance =
                JGeometry::TVec2f((mTargetPos[i] - mInitialPos[i]) * (f32)mCounter / 8.0f);
            mDrawPos[i] = mInitialPos[i] + advance;
        }
        else
        {
            mInitialPos[i] = mTargetPos[i];
            mDrawPos[i] = mInitialPos[i];
        }

        mStars[i]->setOffset(mDrawPos[i].x + mPaneStarsPosOffset->getTranslateX(),
                             mDrawPos[i].y + mPaneStarsPosOffset->getTranslateY());
        mStars[i]->updateScale(mScale[1]);
    }

    if (mCounter != 0)
    {
        if (mCounter < 8)
        {
            ++mCounter;
        }
        else
        {
            mCounter = 0;
        }
    }
}

f32 Pause2D::getScale(int frame)
{
    u32 num = (frame * 65535) / 50 + 0x7FFF;
    return 1.0f + 0.3f * (0.5f + -0.5f * JMath::sincosTable_.mTable[(num >> 5) & 0x07FF].second);
}

int Pause2D::getPauseSelector()
{
    return mPauseSelector;
}

Pause2D::PauseStatus Pause2D::getPauseStatus()
{
    return mStatus;
}

void Pause2D::setPauseStatus(Pause2D::PauseStatus status)
{
    mStatus = status;
}

void Pause2D::setPadText()
{
    struct PadTextInfo
    {
        J2DTextBox *pane;
        u64 name;
        int messageIdx;
    };

    PadTextInfo info[5] = {
        {nullptr, (u64)'TMG01', 0},
        {nullptr, (u64)'TMG02', 0},
        {nullptr, (u64)'TTMGW', 4},
        {nullptr, (u64)'TMGW1', 1},
        {nullptr, (u64)'TMGW2', 2},
    };

    JGeometry::TVec2f scale[5];

    char buffer[64];
    for (int i = 0; i < 64; ++i)
    {
        buffer[i] = ' ';
    }
    buffer[63] = '\0';

    for (int i = 0; i < 5; ++i)
    {
        J2DTextBox *pane = (J2DTextBox *)mScreen->search(info[i].name);
#line 1001
        JUT_ASSERT(pane->getTypeID() == J2DPane_TextBox);
        info[i].pane = pane;
        info[i].pane->setFont(FontMgr::mspResFont);
        info[i].pane->setString(buffer);
        scale[i].x = info[i].pane->mFontSize.x;
        scale[i].y = info[i].pane->mFontSize.y;
    }

    for (int i = 0; i < 5; ++i)
    {
        void *bmcPtr = ResMgr::getPtr(ResMgr::mcColor);
        void *bmdPtr = ResMgr::getPtr(ResMgr::mcRace);

        int messageIdx = info[i].messageIdx;
        char *message = ReadPrintMessage::getMessage(bmdPtr, messageIdx);

        JUtility::TColor color(
            ReadPrintMessage::getColor(bmcPtr, ReadPrintMessage::getDefaultColor(bmdPtr)));
        info[i].pane->setFontColor(color, color);

        info[i].pane->setFontSize(
            scale[i].x * ReadPrintMessage::getRate(bmdPtr, messageIdx) / 100.0f,
            scale[i].y * ReadPrintMessage::getRate(bmdPtr, messageIdx) / 100.0f);
        info[i].pane->setCharSpace((f32)ReadPrintMessage::getCharSpace(bmdPtr, messageIdx));

        J2DTextBox::TFontSize fontSize;
        info[i].pane->getFontSize(fontSize);
        char buf[64];
        ReadPrintMessage::tagCnv(message, bmcPtr, fontSize, sizeof(buf), buf);
        strncpy(info[i].pane->getStringPtr(), buf, sizeof(buf));
    }

    J2DPane *singlePane = mScreen->search((u64)'NSingle');
    J2DPane *tagPane = mScreen->search((u64)'NTag');

    if (RCMGetManager()->getKartInfo(0)->getPad(1))
    {
        singlePane->hide();
        tagPane->show();
    }
    else
    {
        singlePane->show();
        tagPane->hide();
    }
}
