#ifndef PRINTMEMORYCARD_H
#define PRINTMEMORYCARD_H

#include "JSystem/JKernel/JKRHeap.h"
#include "Kameda/PrintWindow.h"
#include "Osako/ResMgr.h"

class PrintMemoryCard
{
public:
    // TODO: are these actually correct?
    enum MessageID
    {
        mcNoCardContinue,
        mcNoUseContinue,
        mcWithouMKContinue,
        mcBrokenNoSaveContinue,
        mcBrokenGoFormat,
        mcNoFileNoSave,
        mcGameSavingNoTouch,
        mcGetSavingNoTouch,
        mcMissSaved,
        mcInCardPushButton,
        mcChangeCardPushButton,
        mcMissFormat,
        mcFormatLost,
        mcFormatNoTouch,
        mcNoSaveContinue,
        mcMakeFile,
        mcMakeFileNoTouch,
        mcMadeFile,
        mcMissMakeFile,
        mcNoSpaceGoIPL,
        mcTooManyFilesGoIPL,
        mcNoUse,
        mcWithoutMK,
        mcBrokenNoUse,
        mcBrokenNeedFormat,
        mcGstLoad,
        mcLoaded,
        mcNoSpaceNoSave,
        mcTooManyFiles,
        mcGstSave,
        mcSaved,
        mcGstOverwrite,
        mcNoGst,
        mcGstNoLoad,
        mcGstBrokenNoLoad,
        mcBrokenNoLoad,
        // these are lan related for whatever reason
        mcTooManyFilesContinue,  // begin lan setup
        mcNoSpaceContinue,       // disconnect lan link
        mcMsg38,                 // disconnecting LAN Link
        mcMsg39,                 // Quit are you sure?
        mcMsg40                  // black screen, no text here, also panic if bmg gets requested
    };

    PrintMemoryCard(JKRHeap *heap);  // 0x8016be58
    ~PrintMemoryCard();

    // TODO
    void setStuff(bool a, bool b, bool c)
    {
        _c = a;
        _d = b;
        _e = c;
    }

    void reset();  // 0x8016bf24
    void initWindow();
    void init(MessageID);    // 0x8016bf90
    void changeMessage();    // 0x8016bfac
    void draw();             // 0x8016c3e0
    void calc();             // 0x8016c410
    void closeWindow();      // 0x8016c92c
    void closeWindowNoSe();  // 0x8016c9f8
    void setBmgPtr();        // 0x8016ca9c
    bool isMessage();        // 0x8016cb0c
    s32 getChoiceType();     // 0x8016cb98
    int getWindowSize();     // 0x8016cc34
    int getWindowColor();    // 0x8016cd00

    int get_1cThing()
    {
        if (mFrame == 0 || mFrame == 25)
            return 3;
        return _1c;
    }

    int get_14Thing()
    {
        if (_1c == 2 && (mFrame == 0 || mFrame == 25))
            return _14;
        return 2;
    }

private:
    void *mResColor;                        // 0x0
    void *mResCard;                         // 0x4
    PrintWindow *mPrintWindow;              // 0x8
    bool _c;                                // 0xc
    bool _d;                                // 0xd
    bool _e;                                // 0xe
    bool _f;                                // 0xf
    PrintMemoryCard::MessageID mMessageID;  // 0x10
    s32 _14;                                // 0x14
    s32 _18;                                // 0x18
    s32 _1c;                                // 0x1c
    s32 mFrame;                             // 0x20
    s32 _24;                                // 0x24
    s32 _28;                                // 0x28
    JUTColor mColor;                        // 0x2c
};  // Size: 0x30

#endif  // PRINTMEMORYCARD_H
