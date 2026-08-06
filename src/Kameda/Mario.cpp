#include "Kameda/Mario.h"

#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/J2DManager.h"
#include "Kameda/WipeManager.h"
#include "Osako/system.h"

Mario::Mario(JKRHeap *heap)
{
    mPicture = new (heap, 0) J2DPicture(
        (ResTIMG *)J2DManager::getManager()->getArchive()->getResource("timg/wipe_star.bti"));

    mPictureWidth = mPicture->getTexture(0)->getWidth();
    mPictureHeight = mPicture->getTexture(0)->getHeight();

    switch (WipeManager::getManager()->getScreenCount())
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
            mOrthoGraph[i]->scissor(System::get3DScisDiv2X(i),
                                    System::get3DScisDiv2Y(i),
                                    System::get3DScisDiv2W(i),
                                    System::get3DScisDiv2H(i));
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
            mOrthoGraph[i]->scissor(System::get3DScisDiv4X(i),
                                    System::get3DScisDiv4Y(i),
                                    System::get3DScisDiv4W(i),
                                    System::get3DScisDiv4H(i));
        }
        break;
    default:
#line 71
        JUT_ASSERT(0);
        break;
    }

    mOrthoGraph2D = new (heap, 0) J2DOrthoGraph(System::get2DVpX(),
                                                System::get2DVpY(),
                                                System::get2DVpW(),
                                                System::get2DVpH(),
                                                -1.0f,
                                                1.0f);
    mOrthoGraph2D->scissor(System::get2DScisX(),
                           System::get2DScisY(),
                           System::get2DScisW(),
                           System::get2DScisH());

    mWidth = System::get2DVpW();
    mHeight = System::get2DVpH();

    s32 aspectRatio = (mWidth / mPictureWidth);
    aspectRatio += 1;
    mScaledWidth = (mPictureWidth * aspectRatio);
    mScaledHeight = (mPictureHeight * aspectRatio);

    init();
}

void Mario::init() {}

Mario::~Mario()
{
    // Unknown; unused.
}

void Mario::draw(int status, f32 f, JUTColor color)
{
    // Unknown; unused.
}

void Mario::draw(f32 f, JUTColor color)
{
    mOrthoGraph2D->setPort();

    mPicture->setWhite(color);

    if (f < 0.75f)
    {
        mPicture->setAlpha((4.0f * (255.0f * f)) / 3.0f);
    }
    else
    {
        mPicture->setAlpha(0xFF);
    }

    f32 w = mWidth;
    f32 h = mHeight;

    if (f != 1.0f)
    {
        f32 w_ = mScaledWidth * (1.0f - f);
        f32 h_ = mScaledHeight * (1.0f - f);

        mPicture->drawOut(0.0f, 0.0f, w, h, (w - w_) / 2.0f, (h - h_) / 2.0f, w_, h_);
    }
    else
    {
        mOrthoGraph2D->setColor(color);
        mOrthoGraph2D->fillBox(0.0f, 0.0f, w, h);
    }
}
