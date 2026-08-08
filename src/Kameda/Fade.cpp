#include "Kameda/Fade.h"

#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/WipeManager.h"
#include "Osako/system.h"

Fade::Fade(JKRHeap *heap)
{
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
        mSizes[0].mWidth = System::get3DVpW();
        mSizes[0].mHeight = System::get3DVpH();
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
            mSizes[i].mWidth = System::get3DVpDiv2W(i);
            mSizes[i].mHeight = System::get3DVpDiv2H(i);
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
            mSizes[i].mWidth = System::get3DVpDiv4W(i);
            mSizes[i].mHeight = System::get3DVpDiv4H(i);
        }
        break;
    default:
#line 71
        JUT_ASSERT(0);
        break;
    }

    init();
}

void Fade::init() {}

Fade::~Fade()
{
    _0 = nullptr;

    for (int i = 0; i < 4; ++i)
    {
        delete mOrthoGraph[i];
        mOrthoGraph[i] = nullptr;
    }
}

void Fade::draw(int status, f32 f, JUTColor color)
{
#line 113
    JUT_MINMAX_ASSERT(0, status, 4);

    mOrthoGraph[status]->setPort();
    mOrthoGraph[status]->setColor(JUTColor(color.r, color.g, color.b, color.a * f));
    mOrthoGraph[status]->fillBox(0.0f, 0.0f, mSizes[status].mWidth, mSizes[status].mHeight);
}
