#include "Kameda/Go2D.h"

#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "Kameda/J2DManager.h"
#include "Kaneshige/RaceMgr.h"
#include "Osako/system.h"

Go2DParam::Go2DParam() {
    mUnknown = 0;
    mFlag = true;
}

Go2D::Go2D(JKRHeap *heap)
{
    mHioNode = new (heap, 0) Go2DHioNode();

    mGrafContext = System::getJ2DOrtho();

    mScreen = new (heap, 0) J2DScreen();
    mScreen->set("go.blo", 0x40000, J2DManager::getManager()->getArchive());

    mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("go.bck", J2DManager::getManager()->getArchive()));
    mScreen->setAnimation(mTransform);

    mTevRegKey = (J2DAnmTevRegKey *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("go.brk", J2DManager::getManager()->getArchive()));
    mTevRegKey->searchUpdateMaterialID(mScreen);
    mScreen->setAnimation(mTevRegKey);

    init();
}

void Go2D::init()
{
    mTransformFrame = 0.0f;
    mTevRegKeyFrame = 0.0f;

    mDrawFlag = false;

    mTransform->setFrame(mTransformFrame);
    mTevRegKey->setFrame(mTevRegKeyFrame);

    mScreen->animation();
}

Go2D::~Go2D() {
    delete mHioNode;
}

void Go2D::draw()
{
    if (mHioNode->mParam.mFlag & 1 && mDrawFlag)
    {
        ERacePhase racePhase = RCMGetManager()->getRacePhase();
        if (racePhase != PHASE_CRS_DEMO)
        {
            RaceMgr *raceMgr = RCMGetManager();
            if (!raceMgr->isReplayMode() && !raceMgr->mRaceInfo->isWaitDemo() &&
                mTransformFrame < 70.0f)
            {
                mGrafContext->setPort();
                mScreen->draw(0.0f, 0.0f, mGrafContext);
            }
        }
    }
}

void Go2D::calc()
{
    if (mHioNode->mParam.mFlag & 1)
    {
        f32 frame = mTransformFrame;
        if (frame < 70.0f)
        {
            ++mTransformFrame;
            mTransform->setFrame(frame);

            mTevRegKey->setFrame(mTevRegKeyFrame++);

            mScreen->animation();
        }
    }
}
