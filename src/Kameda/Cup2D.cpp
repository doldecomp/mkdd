#include "Kameda/Cup2D.h"

#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JUtility/JUTAssert.h"
#include "PowerPC_EABI_Support/MSL_C/MSL_Common/printf.h"
#include "kartEnums.h"
#include "mathHelper.h"  // For unused data

Cup2D *Cup2D::mCup2D;

Cup2D::Cup2D(JKRArchive *archive)
{
    mCup2D = this;

    for (int cup = 0; cup < CUP_MAX; ++cup)
    {
        for (int rank = 0; rank < 3; ++rank)
        {
            mTextures[cup][rank] = nullptr;

            char buffer[0x40];
            snprintf(buffer, sizeof(buffer), "Cup%c%d.bti", cup + 'A', rank + 1);

            mTextures[cup][rank] = archive->getResource(buffer);
        }
    }
}

Cup2D::~Cup2D()
{
    if (mCup2D)
        mCup2D = nullptr;
}

void *Cup2D::getCupTexture(ERaceGpCup cup, int rank)
{
#line 63
    JUT_MINMAX_ASSERT(0, cup, CUP_MAX);
    JUT_MINMAX_ASSERT(0, rank, 3);

    return mTextures[cup][rank];
}
