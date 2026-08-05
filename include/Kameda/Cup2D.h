#ifndef CUP2D_H
#define CUP2D_H

#include "kartEnums.h"

#include "JSystem/JKernel/JKRArchive.h"

class Cup2D
{
public:
    Cup2D(JKRArchive *archive);  // 0x80197f10
    ~Cup2D();                    // 0x80197fc0

    void *getCupTexture(ERaceGpCup cup, int rank);  // 0x80198014

private:
    static Cup2D *mCup2D;  // 0x80416388

    void *mTextures[CUP_MAX][3];
};

#endif  // CUP2D_H
