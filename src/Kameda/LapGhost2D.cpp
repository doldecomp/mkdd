#include "Kameda/LapGhost2D.h"

#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/J2D/J2DPane.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/J2DManager.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/GhostFile.h"
#include "Osako/Kart2DCommon.h"
#include "Osako/KartPadRecord.h"
#include "Osako/system.h"
#include "mathHelper.h"  // For unused data

LapGhost2D::LapGhost2D(JKRHeap *heap)
{
    mGrafContext = System::getJ2DOrtho();
    mScreen = new (heap, 0) J2DScreen();

    mScreen->J2DScreen::set("LapGhost.blo", 0x40000, J2DManager::getManager()->getArchive());

    mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("LapGhost.bck", J2DManager::getManager()->getArchive()));
    mScreen->setAnimation(mTransform);

    mTevRegKey = (J2DAnmTevRegKey *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("LapGhost.brk", J2DManager::getManager()->getArchive()));
    mTevRegKey->searchUpdateMaterialID(mScreen);
    mScreen->setAnimation(mTevRegKey);

    for (int i = 0; i < 8; ++i)
    {
        J2DPicture *pane = (J2DPicture *)mScreen->search('GhTime0' + i);
#line 60
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);

        mPictures[i] = pane;
    }

    init();
}

void LapGhost2D::init()
{
    mTransformFrame = 0.0f;
    mTevRegKeyFrame = 0.0f;

    mVisible[0] = false;
    mVisible[1] = false;

    mTransform->setFrame(mTransformFrame);
    mTevRegKey->setFrame(mTevRegKeyFrame);

    mScreen->animation();
}

LapGhost2D::~LapGhost2D() {}

void LapGhost2D::draw()
{
    if (mVisible[0])
    {
        ERacePhase racePhase = RCMGetManager()->getRacePhase();
        if (racePhase != PHASE_CRS_DEMO)
        {
            RaceMgr *raceMgr = RCMGetManager();
            if (!raceMgr->isReplayMode() && !raceMgr->mRaceInfo->isWaitDemo())
            {
                mGrafContext->setPort();
                mScreen->draw(0.0f, 0.0f, mGrafContext);
            }
        }
    }
}

void LapGhost2D::calc()
{
    if (mVisible[0])
    {
        if (mTransformFrame < 130.0f)
        {
            mTransform->setFrame(mTransformFrame);
            mTevRegKey->setFrame(mTevRegKeyFrame);

            mTransformFrame += 1.0f;

            if (mVisible[1])
            {
                if (++mTevRegKeyFrame >= 20.0f)
                {
                    mTevRegKeyFrame = 0.0f;
                }
            }

            mScreen->animation();
        }
        else
        {
            mTransformFrame = 0.0f;
            mTevRegKeyFrame = 0.0f;

            mVisible[0] = false;
            mVisible[1] = false;

            mTransform->setFrame(mTransformFrame);
            mTevRegKey->setFrame(mTevRegKeyFrame);

            mScreen->animation();
        }
    }
}

void LapGhost2D::start(RaceTime raceTime, int lap)
{
    KartPadRecord &padRecord = gGhostFile.mFileData.mPadRecord;

    if (gGhostFile.isValidGhostOnRace())
    {
        if (padRecord.isFrameValid())
        {
            mVisible[0] = true;

            Kart2DCommon *kart2DCommon = Kart2DCommon::ptr();

            padRecord.calcLapTime(lap, raceTime);

            int minutes, seconds, milliseconds;
            raceTime.get(&minutes, &seconds, &milliseconds);

            if (raceTime.get() < 0)
            {
                mPictures[7]->changeTexture(kart2DCommon->getAsciiTexture('-'), 0);
                mVisible[1] = true;
            }
            else
            {
                mPictures[7]->changeTexture(kart2DCommon->getAsciiTexture('+'), 0);
                mVisible[1] = false;
            }

            kart2DCommon->changeNumberTexture(minutes, &mPictures[5], 2, true, false);
            kart2DCommon->changeNumberTexture(seconds, &mPictures[3], 2, true, false);
            kart2DCommon->changeNumberTexture(milliseconds, &mPictures[0], 3, true, false);
        }
    }
}
