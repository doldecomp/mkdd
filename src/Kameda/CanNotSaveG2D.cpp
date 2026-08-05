#include "Kameda/CanNotSaveG2D.h"

#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "Kameda/J2DManager.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/system.h"
#include "mathHelper.h"  // For unused data

CanNotSaveG2D::CanNotSaveG2D(JKRHeap *heap)
{
    mGrafContext = System::getJ2DOrtho();

    mScreen = new (heap, 0) J2DScreen();
    mScreen->set("CanNotSaveG.blo", 0x40000, J2DManager::getManager()->getArchive());

    mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("CanNotSaveG.bck", J2DManager::getManager()->getArchive()));
    mScreen->setAnimation(mTransform);

    mTevRegKey = (J2DAnmTevRegKey *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("CanNotSaveG.brk", J2DManager::getManager()->getArchive()));
    mTevRegKey->searchUpdateMaterialID(mScreen);
    mScreen->setAnimation(mTevRegKey);

    init();
}

void CanNotSaveG2D::init()
{
    mTransformFrame = 0.0f;
    mTevRegKeyFrame = 0.0f;

    mVisible = false;

    mTransform->mCurrentFrame = mTransformFrame;
    mTevRegKey->mCurrentFrame = mTevRegKeyFrame;

    mScreen->animation();
}

void CanNotSaveG2D::draw()
{
    if (mVisible)
    {
        ERacePhase racePhase = RaceMgr::getManager()->getRacePhase();
        if (racePhase != PHASE_CRS_DEMO)
        {
            RaceMgr *raceMgr = RaceMgr::getManager();
            if (!raceMgr->isReplayMode() && !raceMgr->mRaceInfo->isWaitDemo() &&
                !raceMgr->mRaceDirector->isRaceEnd())
            {
                mGrafContext->setPort();
                mScreen->draw(0.0f, 0.0f, mGrafContext);
            }
        }
    }
}

void CanNotSaveG2D::calc()
{
    if (mVisible)
    {
        if (mTransformFrame < 25.0f)
        {
            mTransform->mCurrentFrame = mTransformFrame;
            mTransformFrame += 1.0f;
        }

        if (++mTevRegKeyFrame >= 40.0f)
        {
            mTevRegKeyFrame = 0.0f;
        }
        mTevRegKey->mCurrentFrame = mTevRegKeyFrame;

        mScreen->animation();
    }
}
