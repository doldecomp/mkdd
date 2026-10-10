#include "Kameda/Goal2D.h"

#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/J2DManager.h"
#include "Kameda/PauseManager.h"
#include "Kameda/Result2D.h"
#include "Kameda/SequenceInfo.h"
#include "Kaneshige/KartChecker.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/system.h"
#include "Sato/JPEffectMgr.h"
#include "kartLocale.h"
#include "mathHelper.h"  // For unused data

bool Goal2D::mDrawEndFlag = false;

Goal2DParam::Goal2DParam() : mUnknown(0), mFlag(true) {}

Goal2D::Goal2D(JKRHeap *heap)
{
    mHioNode = new (heap, 0) Goal2DHioNode();

    for (int i = 0; i < 4; ++i)
    {
        mOrthoGraph[i] = nullptr;
    }

    switch (RCMGetManager()->getConsoleNumber())
    {
    case 1:
        mOrthoGraph[0] = new (heap, 0) J2DOrthoGraph(System::get3DVpX(),
                                                     System::get3DVpY(),
                                                     System::get3DVpW(),
                                                     System::get3DVpH(),
                                                     -1.0f,
                                                     1.0f);
        mOrthoGraph[0]->scissor(System::get3DScisX(),
                                System::get3DScisY(),
                                System::get3DScisW(),
                                System::get3DScisH());
        mViewport[0].mX = System::get3DVpX();
        mViewport[0].mY = System::get3DVpY();
        break;
    case 2:
        for (u8 i = 0; i < 2; ++i)
        {
            mOrthoGraph[i] = new (heap, 0) J2DOrthoGraph(System::get3DVpDiv2X(i),
                                                         System::get3DVpDiv2Y(i),
                                                         System::get3DVpDiv2W(i),
                                                         System::get3DVpDiv2H(i),
                                                         -1.0f,
                                                         1.0f);

            mOrthoGraph[i]->setOrtho(System::get3DVpDiv2X(i),
                                     System::get3DVpDiv2Y(i),
                                     2.0f * System::get3DVpDiv2W(i),
                                     2.0f * System::get3DVpDiv2H(i),
                                     -1.0f,
                                     1.0f);
            mOrthoGraph[i]->scissor(System::get3DScisDiv2X(i),
                                    System::get3DScisDiv2Y(i),
                                    System::get3DScisDiv2W(i),
                                    System::get3DScisDiv2H(i));
            mViewport[i].mX = System::get3DVpDiv2X(i) + System::get3DVpDiv2W(i) / 2.0f;
            mViewport[i].mY = System::get3DVpDiv2Y(i);
        }
        break;
    case 3:
    case 4:
        for (u8 i = 0; i < 4; ++i)
        {
            mOrthoGraph[i] = new (heap, 0) J2DOrthoGraph(System::get3DVpDiv4X(i),
                                                         System::get3DVpDiv4Y(i),
                                                         System::get3DVpDiv4W(i),
                                                         System::get3DVpDiv4H(i),
                                                         -1.0f,
                                                         1.0f);
            mOrthoGraph[i]->setOrtho(System::get3DVpDiv4X(i),
                                     System::get3DVpDiv4Y(i),
                                     2.0f * System::get3DVpDiv4W(i),
                                     2.0f * System::get3DVpDiv4H(i),
                                     -1.0f,
                                     1.0f);
            mOrthoGraph[i]->scissor(System::get3DScisDiv4X(i),
                                    System::get3DScisDiv4Y(i),
                                    System::get3DScisDiv4W(i),
                                    System::get3DScisDiv4H(i));
            mViewport[i].mX = System::get3DVpDiv4X(i);
            mViewport[i].mY = System::get3DVpDiv4Y(i);
        }
        break;
    default:
#line 121
        JUT_ASSERT(0);
        break;
    }

    for (int i = 0; i < RCMGetManager()->getConsoleNumber(); ++i)
    {
        mScreen[i] = new (heap, 0) J2DScreen();
        mScreen[i]->set("goal.blo", 0x40000, J2DManager::getManager()->getArchive());

        mTransform[i] = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("goal.bck", J2DManager::getManager()->getArchive()));
        mScreen[i]->setAnimation(mTransform[i]);

        mTevRegKey[i] = (J2DAnmTevRegKey *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("goal.brk", J2DManager::getManager()->getArchive()));
        mScreen[i]->setAnimation(mTevRegKey[i]);
    }

    for (int i = 0; i < RCMGetManager()->getConsoleNumber(); ++i)
    {
        mRetireScreen[i] = new (heap, 0) J2DScreen();
        mRetireScreen[i]->set("retire.blo", 0x40000, J2DManager::getManager()->getArchive());

        mRetireTransform[i] = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("retire.bck", J2DManager::getManager()->getArchive()));
        mRetireScreen[i]->setAnimation(mRetireTransform[i]);

        mRetireTevRegKey[i] = (J2DAnmTevRegKey *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("retire.brk", J2DManager::getManager()->getArchive()));
        mRetireScreen[i]->setAnimation(mRetireTevRegKey[i]);
    }

    for (int i = 0; i < RCMGetManager()->getConsoleNumber(); ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            J2DPane *pane = mScreen[i]->search('PICT_001' + j);
#line 158
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);

            mGoalPicture[i][j] = (J2DPicture *)pane;

            if (RCMGetManager()->isRaceModeMiniGame())
            {
                mGoalPicture[i][j]->changeTexture("MiniGame2DWin00.bti", 0);
            }
        }
    }

    for (int i = 0; i < RCMGetManager()->getConsoleNumber(); ++i)
    {
        J2DPane *pane = mRetireScreen[i]->search('Retire00');
#line 170
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);

        mRetirePicture[i] = (J2DPicture *)pane;

        if (RCMGetManager()->isRaceModeMiniGame())
        {
            mRetirePicture[i]->changeTexture("MiniGame2DLose00.bti", 0);
            mRetireScreen[i]->search('RetireB')->hide();
        }
    }

    init();
}

void Goal2D::init()
{
    for (int i = 0; i < RCMGetManager()->getStatusNumber(); ++i)
    {
        mViewport[i].mGoalAnm = false;
        mViewport[i].mRetireAnm = false;
        mViewport[i].mAnmFrame = 0;

        mTransform[i]->setFrame(0.0f);
        mTevRegKey[i]->setFrame(0.0f);
        mScreen[i]->animation();

        mRetireTransform[i]->setFrame(0.0f);
        mRetireTevRegKey[i]->setFrame(0.0f);
        mRetireScreen[i]->animation();
    }

    _e4 = 0;
    mVisible = false;
    mDrawEndFlag = false;
    _e9 = 0;
}

Goal2D::~Goal2D()
{
    delete mHioNode;
    // Symbol not available; implementation incomplete.
}

void Goal2D::drawGoal()
{
    if (mHioNode->mParam.mFlag & 1 && RCMGetManager()->getRacePhase() != PHASE_CRS_DEMO &&
        !RCMGetManager()->isReplayMode() && !RCMGetManager()->mRaceInfo->isWaitDemo() && mVisible)
    {
        for (int i = 0; i < RCMGetManager()->getStatusNumber(); ++i)
        {
            int kart = J2DManager::getManager()->getStatus2Kart(i);
            KartChecker *kartChecker = RCMGetManager()->getKartChecker(kart);

            if (!kartChecker->isGoal() && !J2DManager::getManager()->mWinnerAnmFlag[kart])
            {
                continue;
            }

            if (mViewport[i].mRetireAnm)
            {
                mOrthoGraph[i]->setPort();
                mScreen[i]->draw(mViewport[i].mX, mViewport[i].mY, mOrthoGraph[i]);
            }
        }
    }
}

void Goal2D::drawRetire()
{
    if (!mDrawEndFlag && mHioNode->mParam.mFlag & 1 &&
        RCMGetManager()->getRacePhase() != PHASE_CRS_DEMO && !RCMGetManager()->isReplayMode() &&
        !RCMGetManager()->mRaceInfo->isWaitDemo() && mVisible)
    {
        for (int i = 0; i < RCMGetManager()->getStatusNumber(); ++i)
        {
            KartChecker *kartChecker =
                RCMGetManager()->getKartChecker(J2DManager::getManager()->getStatus2Kart(i));

            if (!kartChecker->isGoal() && mViewport[i].mRetireAnm)
            {
                mOrthoGraph[i]->setPort();
                mRetireScreen[i]->draw(mViewport[i].mX, mViewport[i].mY, mOrthoGraph[i]);
            }
        }
    }
}

void Goal2D::calc()
{
    if (mDrawEndFlag)
        return;

    int finishedViewportCount = 0;
    int activeViewportCount = 0;

    for (int i = 0; i < RCMGetManager()->getStatusNumber(); ++i)
    {
        int kart = J2DManager::getManager()->getStatus2Kart(i);
        KartChecker *kartChecker = RCMGetManager()->getKartChecker(kart);

        if (!kartChecker->isGoal() && !J2DManager::getManager()->mWinnerAnmFlag[kart] &&
            !J2DManager::getManager()->mLoserAnmFlag[kart])
        {
            continue;
        }

        mVisible = true;

        if (mViewport[i].mAnmFrame < 128)
        {
            mViewport[i].mGoalAnm = true;
            mViewport[i].mRetireAnm = true;
        }

        if (mViewport[i].mAnmFrame == 0 && J2DManager::getManager()->mAnmFrame > 1 &&
            RCMGetManager()->getRaceMode() == VERSUS_RACE)
        {
            mViewport[i].mAnmFrame = 128;
            return;
        }

        ++mViewport[i].mAnmFrame;

        if (J2DManager::getManager()->mAnmFrame < 70 && !RCMGetManager()->isRaceModeMiniGame() &&
            mViewport[i].mAnmFrame == 70)
        {
            mViewport[i].mAnmFrame = 69;
        }

        if (kartChecker->isGoal() || J2DManager::getManager()->mWinnerAnmFlag[kart])
        {
            mTransform[i]->setFrame((f32)mViewport[i].mAnmFrame);
            mTevRegKey[i]->setFrame((f32)mViewport[i].mAnmFrame);
            mScreen[i]->animation();

            if (RCMGetManager()->getConsoleNumber() == 2)
            {
                for (int j = 0; j < 2; ++j)
                {
                    J2DPane *pane = mGoalPicture[i][j];
                    pane->updateScale(pane->mScale.x * 1.8f, pane->mScale.y * 1.8f);
                }
            }
        }
        else
        {
            mRetireTransform[i]->setFrame((f32)mViewport[i].mAnmFrame);
            mRetireTevRegKey[i]->setFrame((f32)mViewport[i].mAnmFrame);
            mRetireScreen[i]->animation();

            if (RCMGetManager()->getConsoleNumber() == 2)
            {
                mRetirePicture[i]->updateScale(mRetirePicture[i]->mScale.x * 1.4f,
                                               mRetirePicture[i]->mScale.y * 1.4f);
            }

            u8 alpha = 255;

            if (RCMGetManager()->isRaceModeMiniGame())
            {
                s32 remainder = mViewport[i].mAnmFrame - 300;
                if (remainder < 0)
                {
                    alpha = 255;
                }
                else if (remainder < 20)
                {
                    alpha = 255 - remainder * 255 / 20;
                }
                else
                {
                    alpha = 0;
                }
            }

            int hideFrame = J2DManager::getManager()->getHideFrameRace2D();
            u8 hideAlpha;

            if (hideFrame < 20)
            {
                hideAlpha = 255 - hideFrame * 255 / 20;
            }
            else
            {
                hideAlpha = 0;
            }

            if (alpha < hideAlpha)
            {
                mRetirePicture[i]->setAlpha(alpha);
            }
            else
            {
                mRetirePicture[i]->setAlpha(hideAlpha);
            }

            if (mViewport[i].mAnmFrame == 1)
            {
                JGeometry::TVec3f pos(0.0f, 0.0f, 0.0f);
                f32 scale = 1.0f;
                f32 scaleY /*= 1.0f*/;

                switch (RCMGetManager()->getConsoleNumber())
                {
                case 1:
                    pos.set(304.0f, 260.0f, 0.0f);
                    break;
                case 2:
                    scale = 0.7f;
                    scaleY = scale;
                    if (i == 0)
                    {
                        pos.set(304.0f, 130.0f, 0.0f);
                    }
                    else
                    {
                        pos.set(304.0f, 355.0f, 0.0f);
                    }
                    break;
                case 3:
                case 4:
                    scale = 0.5f;
                    scaleY = scale;
                    switch (i)
                    {
                    case 0:
                        pos.set(152.0f, 130.0f, 0.0f);
                        break;
                    case 1:
                        pos.set(456.0f, 130.0f, 0.0f);
                        break;
                    case 2:
                        pos.set(152.0f, 355.0f, 0.0f);
                        break;
                    case 3:
                        pos.set(456.0f, 355.0f, 0.0f);
                        break;
                    default:
#line 415
                        JUT_ASSERT(0);
                        break;
                    }
                    break;
                default:
#line 419
                    JUT_ASSERT(0);
                    break;
                }

                JPABaseEmitter *emitter = nullptr;

                switch (KartLocale::getLanguage())
                {
                case JAPANESE:
                    if (!J2DManager::getManager()->isLANDemo())
                    {
                        emitter = GetJPAMgr()->createEmt2D("mk_2D_retire_a", pos);
                    }
                    break;
                case GERMAN:
                case ITALIAN:
                case SPANISH:
                    if (!J2DManager::getManager()->isLANDemo())
                    {
                        emitter = GetJPAMgr()->createEmt2D("mk_2D_retire_c", pos);
                    }
                    break;
                case ENGLISH:
                case FRENCH:
                    if (!J2DManager::getManager()->isLANDemo())
                    {
                        emitter = GetJPAMgr()->createEmt2D("mk_2D_retire_b", pos);
                    }
                    break;
                default:
#line 443
                    JUT_ASSERT(0);
                    break;
                }

                if (emitter != nullptr)
                {
                    emitter->mGlobalScl.set(scale, scaleY, scale);
                    emitter->mGlobalPScl.set(scale, scaleY);
                }
            }
        }

        ++activeViewportCount;
        if (mViewport[i].mAnmFrame >= 128)
        {
            ++finishedViewportCount;
        }
    }

    if (!RCMGetManager()->isLANMode() && RCMGetManager()->isRaceEnd() &&
        finishedViewportCount == activeViewportCount &&
        PauseManager::getManager()->isResultStart() && J2DManager::getManager()->isRaceEnd())
    {
        mDrawEndFlag = true;

        if (!RCMGetManager()->isReplayMode())
        {
            switch (RCMGetManager()->getRaceMode())
            {
            case TIME_ATTACK:
                sequenceTA();
                break;
            case GRAND_PRIX:
                sequenceGP();
                break;
            case VERSUS_RACE:
                sequenceVS();
                break;
            case BALLOON_BATTLE:
            case BOMB_BATTLE:
            case ESCAPE_BATTLE:
                sequenceMG();
                break;
            default:
#line 485
                JUT_ASSERT(0);
                break;
            }
        }
    }
}

void Goal2D::sequenceGP()
{
    Result2D::setGPClr();
    gSequenceInfo.setClrGPCourse();
}

void Goal2D::sequenceVS()
{
    setVSMGRank();

    for (int i = 0; i < RCMGetManager()->getStatusNumber(); ++i)
    {
        int kart = J2DManager::getManager()->getStatus2Kart(i);
        int rank = RCMGetManager()->getKartChecker(kart)->getRank() - 1;

        gSequenceInfo.set_40_kart_rank(kart, rank);
    }
}

void Goal2D::sequenceTA()
{
    PauseManager::getManager()->setTA();
}

void Goal2D::sequenceMG()
{
    setVSMGRank();

    for (int i = 0; i < RCMGetManager()->getStatusNumber(); ++i)
    {
        int kart = J2DManager::getManager()->getStatus2Kart(i);

        if (RCMGetManager()->getKartChecker(kart)->getRank() - 1 == 0)
        {
            gSequenceInfo.set_80_kart(kart);
        }
    }
}

void Goal2D::setVSMGRank()
{
    for (int i = 0; i < RCMGetManager()->getStatusNumber(); ++i)
    {
        int kart = J2DManager::getManager()->getStatus2Kart(i);
        int rank = RCMGetManager()->getKartChecker(kart)->getRank() - 1;

        if (rank >= 0 && rank < RCMGetManager()->getStatusNumber())
        {
            gSequenceInfo.setStartNo(kart, rank);
        }
    }
}
