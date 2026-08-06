#include "Kameda/LANResult2D.h"
#include "Kameda/J2DManager.h"

#include "Osako/system.h"

#include "JSystem/JKernel.h"
#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/JAudio/JASFakeMatch2.h"

#include "Kaneshige/RaceMgr.h"

#include "mathHelper.h"


LANResult2D::LANResult2D(JKRHeap *heap) {
    mNode = new (heap, 0) LANResult2DHioNode();

    mOrtho = System::mspJ2DOrtho;

    mScreen = new (heap, 0) J2DScreen();

    mScreen->set("LANResult.blo", 0x40000, J2DManager::mThis->mArchive);

    void* resource = JKRFileLoader::getGlbResource(
        "LANResult.bck",
        J2DManager::getManager()->getArchive());

    mAnimation = (J2DAnmTransform*)J2DAnmLoaderDataBase::load(resource);

    mScreen->setAnimation(mAnimation);

    init();
}

LANResult2DParam::~LANResult2DParam(){}

void LANResult2D::init() {
    mDrawFlag = false;
    mTransformFrame = 0.f;
    mAnimation->mCurrentFrame = 0.f;
    mScreen->animation();
}

LANResult2DHioNode::~LANResult2DHioNode(){}

void LANResult2D::draw() {
    if (mNode->mParam.mFlag & 1 && mDrawFlag)
    {
        ERacePhase racePhase = RCMGetManager()->getRacePhase();
        if (racePhase != PHASE_CRS_DEMO)
        {
            RaceMgr *raceMgr = RCMGetManager();
            if (!raceMgr->isReplayMode() && !raceMgr->mRaceInfo->isWaitDemo())
            {
                mOrtho->setPort();
                mScreen->draw(0.0f, 0.0f, mOrtho);
            }
        }
    }
}

void LANResult2D::calc() {
    if (mNode->mParam.mFlag & 1
        && mDrawFlag
        && mTransformFrame < 300.f)
    {
        f32 frame = mTransformFrame++;
        if(frame > 100.f)
        {
            frame = 100.f;
        }

        mAnimation->setFrame(frame);
        mScreen->animation();
    }
}

void LANResult2D::start(int kart) {
#line 147
    JUT_MINMAX_ASSERT(0, kart, 8);

    mDrawFlag = true;
    GameAudio::Main::getAudio()->startSystemSe(0x2005C);

    J2DPicture *pane = (J2DPicture *) mScreen->search(0x4e756d42);

#line 154
    JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);

    pane->setCornerColor(Race2D::getCornerColor(kart));

    pane = (J2DPicture *) mScreen->search(0x4e756d);
#line 157
    JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);

    pane->setCornerColor(Race2D::getCornerColor(kart));
    pane->changeTexture(J2DManager::mKartNumberName[kart], 0);
}
