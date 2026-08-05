#include "Kameda/Go3212D.h"

#include "JSystem/J2D/J2DAnimation.h"
#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "Kameda/J2DManager.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/system.h"
#include "kartEnums.h"
#include "mathHelper.h"  // For unused data

Go3212DParam::Go3212DParam() {
    mUnknown = 0;
    mFlag = true;
}

Go3212D::Go3212D(JKRHeap *heap)
{
    mHioNode = new (heap, 0) Go3212DHioNode();

    mGrafContext = System::getJ2DOrtho();

    mScreen = new (heap, 0) J2DScreen();
    mScreen->set("go321.blo", 0x40000, J2DManager::getManager()->getArchive());

    mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("go321.bck", J2DManager::getManager()->getArchive()));
    mScreen->setAnimation(mTransform);

    mTexPattern = (J2DAnmTexPattern *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("go321.btp", J2DManager::getManager()->getArchive()));
    mTexPattern->searchUpdateMaterialID(mScreen);
    mScreen->setAnimation(mTexPattern);

    init();
}

void Go3212D::init()
{
    mTransformFrame = 0.0f;
    mTexPatternFrame = 0.0f;
    mIsStart = false;
}

Go3212D::~Go3212D() {
    delete mHioNode;
}

void Go3212D::draw()
{
    if (mHioNode->mParam.mFlag & 1 && mIsStart)
    {
        ERacePhase racePhase = RaceMgr::getManager()->getRacePhase();
        if (racePhase != PHASE_CRS_DEMO)
        {
            RaceMgr *raceMgr = RaceMgr::getManager();
            if (!raceMgr->isReplayMode() && !raceMgr->mRaceInfo->isWaitDemo() &&
                mTransformFrame < 180.0f)
            {
                mGrafContext->setPort();
                mScreen->draw(0.0f, 0.0f, mGrafContext);
            }
        }
    }
}

void Go3212D::calc()
{
    if (mHioNode->mParam.mFlag & 1 && mIsStart && mTransformFrame < 180.0f)
    {
        if (RaceMgr::getManager()->mRaceInfo->getRaceMode() == BOMB_BATTLE)
        {
            if (mTransformFrame == 0.0f)
            {
                J2DManager::getManager()->startBombDemo();
            }
            if (mTransformFrame == 80.0f)
            {
                J2DManager::getManager()->endBombDemo();
            }
        }

        mTransform->setFrame(mTransformFrame++);
        mTexPattern->setFrame(mTexPatternFrame++);

        mScreen->animation();
    }
}
