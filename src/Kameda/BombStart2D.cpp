#include "Kameda/BombStart2D.h"

#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/J2DManager.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/system.h"
#include "mathHelper.h"  // For unused data

BombStart2D::BombStart2D(JKRHeap *heap)
{
    mGrafContext = System::getJ2DOrtho();
    mScreen = new (heap, 0) J2DScreen();

    switch (RCMGetManager()->getConsoleNumber())
    {
    case 1:
#line 42
        JUT_ASSERT(!"NO DATA!!");
        break;
    case 2:
        mScreen->set("BombStart2P.blo", 0x40000, J2DManager::getManager()->getArchive());

        mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("BombStart2P.bck",
                                          J2DManager::getManager()->getArchive()));
        mScreen->setAnimation(mTransform);

        mTevRegKey = (J2DAnmTevRegKey *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("BombStart2P.brk",
                                          J2DManager::getManager()->getArchive()));
        mTevRegKey->searchUpdateMaterialID(mScreen);
        mScreen->setAnimation(mTevRegKey);

        mColor = (J2DAnmColor *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("BombStart2P.bpk",
                                          J2DManager::getManager()->getArchive()));
        mColor->searchUpdateMaterialID(mScreen);
        mScreen->setAnimation(mColor);
        break;
    case 3:
    case 4:
        mScreen->set("BombStart4P.blo", 0x40000, J2DManager::getManager()->getArchive());

        mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("BombStart4P.bck",
                                          J2DManager::getManager()->getArchive()));
        mScreen->setAnimation(mTransform);

        mTevRegKey = (J2DAnmTevRegKey *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("BombStart4P.brk",
                                          J2DManager::getManager()->getArchive()));
        mTevRegKey->searchUpdateMaterialID(mScreen);
        mScreen->setAnimation(mTevRegKey);

        mColor = (J2DAnmColor *)J2DAnmLoaderDataBase::load(
            JKRFileLoader::getGlbResource("BombStart4P.bpk",
                                          J2DManager::getManager()->getArchive()));
        mColor->searchUpdateMaterialID(mScreen);
        mScreen->setAnimation(mColor);

        if (RCMGetManager()->getStatusNumber() == 3)
        {
            J2DPane *pane = mScreen->search('N4P');
            pane->hide();
        }
        break;
    }

    init();
}

void BombStart2D::init()
{
    mVisible = false;

    mTransformFrame = 0.0f;
    mTevRegKeyFrame = 0.0f;
    mColorFrame = 0.0f;

    mTransform->setFrame(0.0f);
    mTevRegKey->setFrame(0.0f);
    mColor->setFrame(0.0f);

    mScreen->animation();
}

void BombStart2D::draw()
{
    if (mVisible)
    {
        ERacePhase racePhase = RCMGetManager()->getRacePhase();
        if (racePhase != PHASE_CRS_DEMO)
        {
            RaceMgr *raceMgr = RCMGetManager();
            if (!raceMgr->isReplayMode() && !raceMgr->mRaceInfo->isWaitDemo() &&
                mTransformFrame < 80.0f)
            {
                mGrafContext->setPort();
                mScreen->draw(0.0f, 0.0f, mGrafContext);
            }
        }
    }
}

void BombStart2D::calc()
{
    if (mVisible)
    {
        f32 frame = mTransformFrame;
        if (frame < 80.0f)
        {
            ++mTransformFrame;
            mTransform->setFrame(frame);

            mColor->setFrame(mColorFrame++);

            mScreen->animation();
        }
    }
}

void BombStart2D::start()
{
    mVisible = true;

    if (!RCMGetManager()->getKartInfo(J2DManager::getManager()->getStatus2Kart(0))->isComDriver(1))
    {
        mTevRegKeyFrame = 1.0f;
    }
    else
    {
        mTevRegKeyFrame = 0.0f;
    }
    mTevRegKey->setFrame(mTevRegKeyFrame);
}
