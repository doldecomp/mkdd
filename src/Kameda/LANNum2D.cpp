#include "Kameda/LANNum2D.h"

#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JKernel.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/J2DManager.h"
#include "Kameda/Race2D.h"
#include "Kameda/Result2D.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/system.h"
#include "Yamamoto/kartCtrl.h"
#include "mathHelper.h"  // For unused data

LANNum2DParam::LANNum2DParam() : _48(0), mFlag(1) {}

LANNum2D::LANNum2D(JKRHeap *heap)
{
    mNode = new (heap, 0) LANNum2DHioNode();

    mOrthoGraph = System::mspJ2DOrtho;

    mScreen = new (heap, 0) J2DScreen();

    switch (RCMGetManager()->mRaceInfo->mConsoleNum)
    {
    case 1:
        mScreen->set("LANNum1.blo", 0x40000, GETJ2DManager()->getArchive());
        mAnimation = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("LANNum1.bck", GETJ2DManager()->getArchive()));
        mScreen->setAnimation(mAnimation);
        break;
    case 2:
        mScreen->set("LANNum2.blo", 0x40000, GETJ2DManager()->getArchive());
        mAnimation = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("LANNum2.bck", GETJ2DManager()->getArchive()));
        mScreen->setAnimation(mAnimation);
        for (int i = 0; i < 2; ++i)
        {
            mEndAnm[i].mPane = (J2DPicture *)mScreen->search((u64)'NEnd1' + (u64)i);
            mEndAnm[i].mAnmTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
                JKRFileLoader::getGlbResource("LANNum2.bck", GETJ2DManager()->getArchive()));
            mEndAnm[i].mPane->setAnimation(mEndAnm[i].mAnmTransform);
        }
        for (int i = 0; i < 2; ++i)
        {
            mNumAnm[i].mPane = (J2DPicture *)mScreen->search((u64)'NNum1' + (u64)i);
            mNumAnm[i].mAnmTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
                JKRFileLoader::getGlbResource("LANNum2.bck", GETJ2DManager()->getArchive()));
            mNumAnm[i].mPane->setAnimation(mNumAnm[i].mAnmTransform);
        }
        break;
    case 3:
    case 4:
        mScreen->set("LANNum4.blo", 0x40000, GETJ2DManager()->getArchive());
        mAnimation = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("LANNum4.bck", GETJ2DManager()->getArchive()));
        mScreen->setAnimation(mAnimation);
        for (int i = 0; i < 4; ++i)
        {
            mEndAnm[i].mPane = (J2DPicture *)mScreen->search((u64)'NEnd1' + (u64)i);
            mEndAnm[i].mAnmTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
                JKRFileLoader::getGlbResource("LANNum4.bck", GETJ2DManager()->getArchive()));
            mEndAnm[i].mPane->setAnimation(mEndAnm[i].mAnmTransform);
        }
        for (int i = 0; i < 4; ++i)
        {
            mNumAnm[i].mPane = (J2DPicture *)mScreen->search((u64)'NNum1' + (u64)i);
            mNumAnm[i].mAnmTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
                JKRFileLoader::getGlbResource("LANNum4.bck", GETJ2DManager()->getArchive()));
            mNumAnm[i].mPane->setAnimation(mNumAnm[i].mAnmTransform);
        }
        break;
    default:
#line 114
        JUT_ASSERT(0);
    }

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 2; j++)
        {
            mPlayerPane[i][j].mPane = (J2DPicture *)mScreen->search((u64)'NP11' + i * 0x0100 + j);

            J2DPicture *pane = (J2DPicture *)mScreen->search((u64)'PB11' + i * 0x0100 + j);
            if (pane)
            {
#line 128
                JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
                mPlayerPane[i][j].mBoxPane = pane;
            }
            else
            {
                mPlayerPane[i][j].mBoxPane = nullptr;
            }

            pane = (J2DPicture *)mScreen->search((u64)'PM11' + i * 0x0100 + j);
            if (pane)
            {
#line 136
                JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
                mPlayerPane[i][j].mNumberPane = pane;
            }
            else
            {
                mPlayerPane[i][j].mNumberPane = nullptr;
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        mKartPane[i].mPane = (J2DPicture *)mScreen->search((u64)'NNum1' + (u64)i);

        J2DPicture *pane = (J2DPicture *)mScreen->search((u64)'BNum1' + (u64)i);
        if (pane)
        {
#line 153
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mKartPane[i].mBoxPane = pane;
        }
        else
        {
            mKartPane[i].mBoxPane = nullptr;
        }

        pane = (J2DPicture *)mScreen->search((u64)'KNum1' + (u64)i);
        if (pane)
        {
#line 161
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            mKartPane[i].mNumberPane = pane;
        }
        else
        {
            mKartPane[i].mNumberPane = nullptr;
        }
    }

    init();
}

void LANNum2D::setDraw1()
{
    switch (RCMGetManager()->mRaceInfo->mStatusNum)
    {
    case 0:
        mPlayerPane[0][0].mPane->hide();
        mPlayerPane[0][1].mPane->hide();
        mKartPane[0].mPane->hide();
        break;
    case 1:
        break;
    default:
#line 190
        JUT_ASSERT(0);
    }

    int coPadPort;

    for (int i = 0; i < RCMGetManager()->mRaceInfo->mStatusNum; ++i)
    {
        int kartNo = J2DManager::getStatus2Kart(i);

        KartGamePad *pad = KartCtrl::getKartCtrl()->getPad(kartNo);
        KartGamePad *coPad = KartCtrl::getKartCtrl()->getCoPad(kartNo);

        int padPort = pad->getPadPort();
        if (0 <= padPort && padPort < 4)
        {
            mPlayerPane[i][0].mBoxPane->setCornerColor(Race2D::getCornerColor(padPort));
            mPlayerPane[i][0].mNumberPane->setCornerColor(Race2D::getCornerColor(padPort));
            mPlayerPane[i][0].mNumberPane->changeTexture(Result2D::mPlayerNumberName[padPort], 0);
        }

        if (RCMGetManager()->getKartInfo(kartNo)->getCoPad())
        {
            coPadPort = coPad->getPadPort();
            if (0 <= coPadPort && coPadPort < 4)
            {
                mPlayerPane[i][1].mBoxPane->setCornerColor(Race2D::getCornerColor(coPadPort));
                mPlayerPane[i][1].mNumberPane->setCornerColor(Race2D::getCornerColor(coPadPort));
                mPlayerPane[i][1].mNumberPane->changeTexture(Result2D::mPlayerNumberName[coPadPort],
                                                             0);
            }
        }
        else
        {
            mPlayerPane[i][1].mPane->hide();
        }

        if (0 <= kartNo && kartNo < 8)
        {
            mKartPane[i].mBoxPane->setCornerColor(Race2D::getCornerColor(kartNo));
            mKartPane[i].mNumberPane->setCornerColor(Race2D::getCornerColor(kartNo));
            mKartPane[i].mNumberPane->changeTexture(J2DManager::mKartNumberName[kartNo], 0);
        }
    }

    if (RCMGetManager()->mRaceInfo->mStatusNum == 1)
    {
        if (!RCMGetManager()->getKartInfo(J2DManager::getStatus2Kart(0))->getCoPad())
        {
            mPlayerPane[0][0].mPane->hide();
            mPlayerPane[0][1].mPane->hide();
        }
    }
}

void LANNum2D::setDraw2()
{
    switch (RCMGetManager()->mRaceInfo->mStatusNum)
    {
    case 0:
        mPlayerPane[0][0].mPane->hide();
        mPlayerPane[0][1].mPane->hide();
        mKartPane[0].mPane->hide();
    case 1:
        mPlayerPane[1][0].mPane->hide();
        mPlayerPane[1][1].mPane->hide();
        mKartPane[1].mPane->hide();
        break;
    case 2:
        break;
    default:
#line 251
        JUT_ASSERT(0);
    }

    int coPadPort;

    for (int i = 0; i < RCMGetManager()->mRaceInfo->mStatusNum; ++i)
    {
        int kartNo = J2DManager::getStatus2Kart(i);

        KartGamePad *pad = KartCtrl::getKartCtrl()->getPad(kartNo);
        KartGamePad *coPad = KartCtrl::getKartCtrl()->getCoPad(kartNo);

        int padPort = pad->getPadPort();
        if (0 <= padPort && padPort < 4)
        {
            mPlayerPane[i][0].mBoxPane->setCornerColor(Race2D::getCornerColor(padPort));
            mPlayerPane[i][0].mNumberPane->setCornerColor(Race2D::getCornerColor(padPort));
            mPlayerPane[i][0].mNumberPane->changeTexture(Result2D::mPlayerNumberName[padPort], 0);
        }

        if (RCMGetManager()->getKartInfo(kartNo)->getCoPad())
        {
            coPadPort = coPad->getPadPort();
            if (0 <= coPadPort && coPadPort < 4)
            {
                mPlayerPane[i][1].mBoxPane->setCornerColor(Race2D::getCornerColor(coPadPort));
                mPlayerPane[i][1].mNumberPane->setCornerColor(Race2D::getCornerColor(coPadPort));
                mPlayerPane[i][1].mNumberPane->changeTexture(Result2D::mPlayerNumberName[coPadPort],
                                                             0);
            }
        }
        else
        {
            mPlayerPane[i][1].mPane->hide();
        }

        if (0 <= kartNo && kartNo < 8)
        {
            mKartPane[i].mBoxPane->setCornerColor(Race2D::getCornerColor(kartNo));
            mKartPane[i].mNumberPane->setCornerColor(Race2D::getCornerColor(kartNo));
            mKartPane[i].mNumberPane->changeTexture(J2DManager::mKartNumberName[kartNo], 0);
        }
    }

    if (RCMGetManager()->mRaceInfo->mStatusNum == 1)
    {
        if (!RCMGetManager()->getKartInfo(J2DManager::getStatus2Kart(0))->getCoPad())
        {
            mPlayerPane[0][0].mPane->hide();
            mPlayerPane[0][1].mPane->hide();
        }
    }
}

void LANNum2D::setDraw4()
{
    switch (RCMGetManager()->mRaceInfo->mStatusNum)
    {
    case 0:
        mPlayerPane[0][0].mPane->hide();
        mPlayerPane[0][1].mPane->hide();
        mKartPane[0].mPane->hide();
    case 1:
        mPlayerPane[1][0].mPane->hide();
        mKartPane[1].mPane->hide();
    case 2:
        mPlayerPane[2][0].mPane->hide();
        mKartPane[2].mPane->hide();
    case 3:
        mPlayerPane[3][0].mPane->hide();
        mKartPane[3].mPane->hide();
        break;
    case 4:
        break;
    default:
#line 318
        JUT_ASSERT(0);
    }

    int coPadPort;

    for (int i = 0; i < RCMGetManager()->mRaceInfo->mStatusNum; ++i)
    {
        int kartNo = J2DManager::getStatus2Kart(i);

        KartGamePad *pad = KartCtrl::getKartCtrl()->getPad(kartNo);
        KartGamePad *coPad = KartCtrl::getKartCtrl()->getCoPad(kartNo);

        int padPort = pad->getPadPort();
        if (0 <= padPort && padPort < 4)
        {
            mPlayerPane[i][0].mBoxPane->setCornerColor(Race2D::getCornerColor(padPort));
            mPlayerPane[i][0].mNumberPane->setCornerColor(Race2D::getCornerColor(padPort));
            mPlayerPane[i][0].mNumberPane->changeTexture(Result2D::mPlayerNumberName[padPort], 0);
        }

        if (RCMGetManager()->getKartInfo(kartNo)->getCoPad() && i == 0)
        {
            coPadPort = coPad->getPadPort();
            if (0 <= coPadPort && coPadPort < 4)
            {
                mPlayerPane[i][1].mBoxPane->setCornerColor(Race2D::getCornerColor(coPadPort));
                mPlayerPane[i][1].mNumberPane->setCornerColor(Race2D::getCornerColor(coPadPort));
                mPlayerPane[i][1].mNumberPane->changeTexture(Result2D::mPlayerNumberName[coPadPort],
                                                             0);
            }
        }
        else if (i == 0)
        {
            mPlayerPane[i][1].mPane->hide();
        }

        if (0 <= kartNo && kartNo < 8)
        {
            mKartPane[i].mBoxPane->setCornerColor(Race2D::getCornerColor(kartNo));
            mKartPane[i].mNumberPane->setCornerColor(Race2D::getCornerColor(kartNo));
            mKartPane[i].mNumberPane->changeTexture(J2DManager::mKartNumberName[kartNo], 0);
        }
    }

    if (RCMGetManager()->mRaceInfo->mStatusNum == 1)
    {
        if (!RCMGetManager()->getKartInfo(J2DManager::getStatus2Kart(0))->getCoPad())
        {
            mPlayerPane[0][0].mPane->hide();
            mPlayerPane[0][1].mPane->hide();
        }
    }
}

void LANNum2D::init()
{
    mDrawFlag = false;
    mTransformEnd = false;
    mTransformFrame = 0.0f;
    mAnimation->mCurrentFrame = 0.0f;

    if (RCMGetManager()->mRaceInfo->mConsoleNum != 1)
    {
        for (int i = 0; i < RCMGetManager()->mRaceInfo->mStatusNum; ++i)
        {
            mEndAnm[i].mAnmTransform->mCurrentFrame = 0.0f;
        }
    }

    mScreen->animation();
}

LANNum2D::~LANNum2D()
{
    delete mNode;
}

void LANNum2D::draw()
{
    if (mNode->mParam.mFlag & 1 && mDrawFlag)
    {
        if (RCMGetManager()->getRacePhase() != PHASE_CRS_DEMO && !RCMGetManager()->isReplayMode() &&
            !RCMGetManager()->isWaitDemoMode())
        {
            if (mTransformFrame < 200.0f || mTransformEnd)
            {
                mOrthoGraph->setPort();
                mScreen->draw(0.0f, 0.0f, mOrthoGraph);
            }
            else
            {
                init();
            }
        }
    }
}

void LANNum2D::calc()
{
    if (mNode->mParam.mFlag & 1 && mDrawFlag && mTransformFrame < 200.0f)
    {
        f32 frame = mTransformFrame++;

        if (mTransformEnd)
        {
            if (!(frame < 100.0f))
            {
                frame = 100.0f;
            }
        }
        else if (!(frame < 100.0f))
        {
            if (frame < 190.0f)
            {
                frame = 100.0f;
            }
            else
            {
                frame -= 90.0f;
            }
        }

        mAnimation->setFrame(frame);

        if (RCMGetManager()->mRaceInfo->mConsoleNum != 1)
        {
            for (int i = 0; i < RCMGetManager()->mRaceInfo->mStatusNum; ++i)
            {
                mNumAnm[i].mAnmTransform->setFrame(frame);
            }
        }

        mScreen->animation();
    }
}

void LANNum2D::start()
{
    mDrawFlag = true;

    if (RCMGetManager()->mRaceInfo->mConsoleNum != 1)
    {
        for (int i = 0; i < RCMGetManager()->mRaceInfo->mStatusNum; ++i)
        {
            mEndAnm[i].mAnmTransform->mCurrentFrame = 10.0f;
        }
    }

    switch (RCMGetManager()->mRaceInfo->mConsoleNum)
    {
    case 1:
        setDraw1();
        break;
    case 2:
        setDraw2();
        break;
    case 3:
    case 4:
        setDraw4();
        break;
    default:
#line 489
        JUT_ASSERT(0);
    }

    if (!J2DManager::mNetFlag)
    {
        ((J2DPicture *)mScreen->search('NLan'))->hide();
    }
}

void LANNum2D::start2()
{
    mDrawFlag = true;
    mTransformEnd = true;

    if (RCMGetManager()->mRaceInfo->mConsoleNum != 1)
    {
        for (int i = 0; i < RCMGetManager()->mRaceInfo->mStatusNum; ++i)
        {
            mEndAnm[i].mAnmTransform->mCurrentFrame = 11.0f;
        }
    }

    switch (RCMGetManager()->mRaceInfo->mConsoleNum)
    {
    case 1:
        setDraw1();
        mPlayerPane[0][0].mPane->hide();
        mPlayerPane[0][1].mPane->hide();
        break;
    case 2:
        setDraw2();
        for (int i = 0; i < 2; ++i)
        {
            mPlayerPane[i][0].mPane->hide();
            mPlayerPane[i][1].mPane->hide();
        }
        break;
    case 3:
    case 4:
        setDraw4();
        for (int i = 0; i < 4; ++i)
        {
            mPlayerPane[i][0].mPane->hide();
        }
        mPlayerPane[0][1].mPane->hide();
        break;
    default:
#line 538
        JUT_ASSERT(0);
    }
}
