#ifndef PRINTDVDERROR_H
#define PRINTDVDERROR_H

#include <JSystem/JKernel/JKRHeap.h>
#include <JSystem/JUtility/TColor.h>

#include "Kameda/PrintWindow.h"

class PrintDvdError
{
public:
    enum MessageID
    {
        mcFatalError,
        mcRetryError,
        mcError2, // Unused?
        mcCoverOpen,
        mcDiskError,
        mcLANError0,
        mcLANError1,
        mcLANError2,
        mcLANError3,
        mcLANDisconnect,
        mcLANDisconnectError
    };

    PrintDvdError(JKRHeap *heap);
    ~PrintDvdError();

    void init(int errorId);
    void draw();
    void calc();

private:
    void *mColorFile;
    void *mBmgFile;
    PrintWindow *mPrintWindow;
    int mBmgId;
    JUtility::TColor mColor;
};

#endif
