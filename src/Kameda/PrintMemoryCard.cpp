#include "Kameda/PrintMemoryCard.h"

#include "Inagaki/GameAudioMain.h"
#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/JUtility/JUTGamePad.h"
#include "Kameda/ReadPrintMessage.h"
#include "Osako/NetGameMgr.h"
#include "Osako/ResMgr.h"
#include "Osako/kartPad.h"

PrintMemoryCard::PrintMemoryCard(JKRHeap *heap)
{
    mPrintWindow = new (heap, 0) PrintWindow(heap);
    mPrintWindow->mInfo.type = 1;
    mPrintWindow->mInfo._2f0 = 0;

    mResColor = ResMgr::getPtr(ResMgr::mcColor);
    mResCard = ResMgr::getPtr(ResMgr::mcCard);

    mColor = ReadPrintMessage::getColor(mResColor, ReadPrintMessage::getDefaultColor(mResCard));

    reset();
}

void PrintMemoryCard::reset()
{
    _c = false;
    _d = false;
    _e = false;
    _f = false;
    mFrame = 0;
    _24 = 0;
    _28 = 0;

    mPrintWindow->mInfo.frame = mFrame;
    mPrintWindow->calc();
}

void PrintMemoryCard::initWindow()
{
    // Unknown implementation.
}

void PrintMemoryCard::init(PrintMemoryCard::MessageID messageID)
{
    _f = true;
    mMessageID = messageID;
    _14 = 2;
    _1c = 2;
}

void PrintMemoryCard::changeMessage()
{
    _14 = 2;
    _1c = 0;

    switch (_24)
    {
    case 0:
        mFrame = -1;
        break;
    case 1:
        mFrame = 24;
        break;
    default:
        break;
    }

    if (mMessageID == mcMsg40)
        return;

    mPrintWindow->mInfo.type = getWindowSize();
    mPrintWindow->mInfo._2f0 = getWindowColor();

    switch (getChoiceType())
    {
    case 0:
        _18 = 2;
        mPrintWindow->mInfo._2ec = 1;
        mPrintWindow->mInfo._2f8 = 2;
        break;
    case 1:
        _18 = 0;
        mPrintWindow->mInfo._2ec = 0;
        mPrintWindow->mInfo._2f8 = 0;
        break;
    case 2:
        _18 = 1;
        mPrintWindow->mInfo._2ec = 0;
        mPrintWindow->mInfo._2f8 = 1;
        break;
    default:
#line 151
        JUT_ASSERT(0);
    }

    if (isMessage())
    {
        mPrintWindow->getTextBox()->show();
    }
    else
    {
        mPrintWindow->getTextBox()->hide();
    }

    if (getChoiceType() == 0)
    {
        mPrintWindow->mInfo._2ec = 1;
    }
    else
    {
        mPrintWindow->mInfo._2ec = 0;
    }

    if (isMessage())
    {
        setBmgPtr();

        int bmgMessageID = ReadPrintMessage::getBmgMessageID(mMessageID);
        char *message = ReadPrintMessage::getMessage(mResCard, bmgMessageID);

        mColor = ReadPrintMessage::getColor(mResColor, ReadPrintMessage::getDefaultColor(mResCard));
        mPrintWindow->getTextBox()->setFontColor(mColor, mColor);

        {
            J2DTextBox::TFontSize fontSize = mPrintWindow->getBaseFontSize();
            mPrintWindow->getTextBox()->setFontSize(
                fontSize.x * ReadPrintMessage::getRate(mResCard, bmgMessageID) / 100.0f,
                fontSize.y * ReadPrintMessage::getRate(mResCard, bmgMessageID) / 100.0f);
        }

        mPrintWindow->getTextBox()->setCharSpace(
            ReadPrintMessage::getCharSpace(mResCard, bmgMessageID));

        {
            J2DTextBox::TFontSize fontSize;
            mPrintWindow->getTextBox()->getFontSize(fontSize);
            char buf[512];
            ReadPrintMessage::tagCnv(message, mResColor, fontSize, sizeof(buf), buf);
            strncpy(mPrintWindow->getTextBox()->getStringPtr(), buf, sizeof(buf));
        }
    }

    if (!_c)
    {
        GameAudio::Main::getAudio()->startSystemSe(0x2000b);
        _c = true;
    }
    else if (!_e)
    {
        GameAudio::Main::getAudio()->startSystemSe(0x20047);
    }
    _e = false;
}

PrintMemoryCard::~PrintMemoryCard() {}

void PrintMemoryCard::draw()
{
    if (mMessageID != PrintMemoryCard::mcMsg40)
    {
        mPrintWindow->draw();
    }
}

void PrintMemoryCard::calc()
{
    if (mMessageID == mcMsg40)
        return;

    bool pressed;
    bool left;
    bool right;
    if (NetGameMgr::ptr()->getState() != 0)
    {
        pressed = gpaKartPad[0]->testTrigger(JUTGamePad::A);
        left = gpaKartPad[0]->testRepeat(JUTGamePad::MAINSTICK_LEFT);
        right = gpaKartPad[0]->testRepeat(JUTGamePad::MAINSTICK_RIGHT);
    }
    else
    {
        pressed = gpaGamePad[0]->testTrigger(JUTGamePad::A);
        left = gpaGamePad[0]->testRepeat(JUTGamePad::MAINSTICK_LEFT);
        right = gpaGamePad[0]->testRepeat(JUTGamePad::MAINSTICK_RIGHT);
    }

    if (getChoiceType() != 0)
    {
        if (pressed)
        {
            if (_14 == 2 && _1c == 1)
            {
                switch (_18)
                {
                case 0:
                    GameAudio::Main::getAudio()->startSystemSe(0x20003);
                    _d = true;
                    _e = true;
                    _14 = 0;
                    break;
                case 1:
                    GameAudio::Main::getAudio()->startSystemSe(0x20005);
                    _d = true;
                    _e = true;
                    _14 = 1;
                    break;
                default:
#line 272
                    JUT_ASSERT(0);
                }
                closeWindowNoSe();
            }
        }
        else if (left && _18 == 1)
        {
            if (_14 == 2 && _1c == 1)
            {
                GameAudio::Main::getAudio()->startSystemSe(0x20000);
                _18 = 0;
            }
        }
        else if (right && _18 == 0)
        {
            if (_14 == 2 && _1c == 1)
            {
                GameAudio::Main::getAudio()->startSystemSe(0x20000);
                _18 = 1;
            }
        }
    }
    else
    {
        switch (mMessageID)
        {
        case mcMissSaved:
        case mcInCardPushButton:
        case mcChangeCardPushButton:
        case mcMissFormat:
        case mcMissMakeFile:
        case mcGstNoLoad:
        case mcGstBrokenNoLoad:
        case mcBrokenNoLoad:
            for (int i = 0; i < 1; ++i)  // To match reference assembly; iterator not even used.
            {
                if (pressed && _14 == 2 && _1c == 1)
                {
                    GameAudio::Main::getAudio()->startSystemSe(0x20003);
                    _d = true;
                    _e = true;
                    _14 = 0;
                    closeWindowNoSe();
                }
            }
            break;
        default:
            break;
        }
    }

    switch (_1c)
    {
    case 0:
        switch (_24)
        {
        case 0:
            if (mFrame++ + 1 >= 10)
            {
                mFrame = 9;
                _1c = 1;
            }
            break;
        case 1:
            if (mFrame++ + 1 >= 30)
            {
                mFrame = 30;
                _1c = 1;
            }
            break;
        default:
#line 340
            JUT_ASSERT(0);
        }
        break;
    case 1:
        break;
    case 2:
    case 3:
        switch (_28)
        {
        case 0:
            if (--mFrame < 0)
            {
                mFrame = 0;
            }
            break;
        case 1:
            if (mFrame++ + 1 >= 25)
            {
                mFrame = 25;
            }
            break;
        default:
#line 360
            JUT_ASSERT(0);
        }
        if ((mFrame == 0 || mFrame == 25) && _f)
        {
            _f = false;
            changeMessage();
        }
        break;
    default:
#line 373
        JUT_ASSERT(0);
    }

    if (getChoiceType())
    {
        switch (_18)
        {
        case 0:
            mPrintWindow->mInfo._2f8 = 0;
            break;
        case 1:
            mPrintWindow->mInfo._2f8 = 1;
            break;
        case 2:
            mPrintWindow->mInfo._2f8 = 2;
            break;
        default:
#line 390
            JUT_ASSERT(0);
        }
    }

    mPrintWindow->mInfo.frame = mFrame;
    mPrintWindow->calc();
}

void PrintMemoryCard::closeWindow()
{
    if (_1c == 3)
        return;

    if (_1c == 2)
        return;

    GameAudio::Main::getAudio()->startSystemSe(0x20046);

    _d = true;
    _e = true;

    _1c = 2;

    switch (_28)
    {
    case 0:
        mFrame = 9;
        break;
    case 1:
        mFrame = 20;
        break;
    default:
#line 424
        JUT_ASSERT(0);
    }
}

void PrintMemoryCard::closeWindowNoSe()
{
    if (_1c == 3)
        return;

    if (_1c == 2)
        return;

    _1c = 2;

    switch (_28)
    {
    case 0:
        mFrame = 9;
        break;
    case 1:
        mFrame = 20;
        break;
    default:
#line 451
        JUT_ASSERT(0);
    }
}

void PrintMemoryCard::setBmgPtr()
{
    switch (mMessageID)
    {
    case mcTooManyFilesContinue:
    case mcNoSpaceContinue:
    case mcMsg38:
        mResCard = ResMgr::getPtr(ResMgr::mcLan);
        break;
    case mcMsg39:
        mResCard = ResMgr::getPtr(ResMgr::mcRace);
        break;
    default:
        mResCard = ResMgr::getPtr(ResMgr::mcCard);
        break;
    }
}

bool PrintMemoryCard::isMessage()
{
    bool result;

    if (0 <= mMessageID && mMessageID < 41)
    {
        if (mMessageID == mcMsg40)
        {
            result = false;
        }
        else
        {
            result = true;
        }
    }
    else
    {
#line 497
        JUT_ASSERT(0);
    }

    return result;
}

s32 PrintMemoryCard::getChoiceType()
{
    s32 result;

    switch (mMessageID)
    {
    case mcGameSavingNoTouch:
    case mcGetSavingNoTouch:
    case mcMissSaved:
    case mcInCardPushButton:
    case mcChangeCardPushButton:
    case mcMissFormat:
    case mcFormatNoTouch:
    case mcMakeFileNoTouch:
    case mcMadeFile:
    case mcMissMakeFile:
    case mcNoUse:
    case mcWithoutMK:
    case mcBrokenNoUse:
    case mcBrokenNeedFormat:
    case mcLoaded:
    case mcNoSpaceNoSave:
    case mcTooManyFiles:
    case mcSaved:
    case mcNoGst:
    case mcGstNoLoad:
    case mcGstBrokenNoLoad:
    case mcBrokenNoLoad:
    case mcMsg38:
    case mcMsg40:
        result = 0;
        break;
    case mcMakeFile:
    case mcNoSpaceGoIPL:
    case mcTooManyFilesGoIPL:
        result = 1;
        break;
    case mcNoCardContinue:
    case mcNoUseContinue:
    case mcWithouMKContinue:
    case mcBrokenNoSaveContinue:
    case mcBrokenGoFormat:
    case mcNoFileNoSave:
    case mcFormatLost:
    case mcNoSaveContinue:
    case mcGstLoad:
    case mcGstSave:
    case mcGstOverwrite:
    case mcTooManyFilesContinue:
    case mcNoSpaceContinue:
    case mcMsg39:
        result = 2;
        break;
    default:
#line 561
        JUT_ASSERT(0);
    }

    return result;
}

int PrintMemoryCard::getWindowSize()
{
    int result;

    switch (mMessageID)
    {
    case mcMadeFile:
    case mcGstLoad:
    case mcLoaded:
    case mcGstSave:
    case mcSaved:
    case mcGstOverwrite:
    case mcMsg39:
        result = 0;
        break;
    case mcNoCardContinue:
    case mcNoUseContinue:
    case mcWithouMKContinue:
    case mcBrokenNoSaveContinue:
    case mcBrokenGoFormat:
    case mcNoFileNoSave:
    case mcGameSavingNoTouch:
    case mcGetSavingNoTouch:
    case mcMissSaved:
    case mcInCardPushButton:
    case mcChangeCardPushButton:
    case mcMissFormat:
    case mcFormatLost:
    case mcFormatNoTouch:
    case mcNoSaveContinue:
    case mcMakeFile:
    case mcMakeFileNoTouch:
    case mcMissMakeFile:
    case mcNoSpaceGoIPL:
    case mcTooManyFilesGoIPL:
    case mcNoUse:
    case mcWithoutMK:
    case mcBrokenNoUse:
    case mcBrokenNeedFormat:
    case mcNoSpaceNoSave:
    case mcTooManyFiles:
    case mcNoGst:
    case mcGstNoLoad:
    case mcGstBrokenNoLoad:
    case mcBrokenNoLoad:
    case mcTooManyFilesContinue:
    case mcNoSpaceContinue:
    case mcMsg38:
    case mcMsg40:
        result = 1;
        break;
    default:
#line 624
        JUT_ASSERT(0);
    }

    return result;
}

int PrintMemoryCard::getWindowColor()
{
    int result;

    switch (mMessageID)
    {
    case mcNoCardContinue:
    case mcNoUseContinue:
    case mcWithouMKContinue:
    case mcBrokenNoSaveContinue:
    case mcBrokenGoFormat:
    case mcNoFileNoSave:
    case mcGameSavingNoTouch:
    case mcGetSavingNoTouch:
    case mcInCardPushButton:
    case mcChangeCardPushButton:
    case mcFormatLost:
    case mcFormatNoTouch:
    case mcNoSaveContinue:
    case mcMakeFile:
    case mcMakeFileNoTouch:
    case mcMadeFile:
    case mcNoSpaceGoIPL:
    case mcTooManyFilesGoIPL:
    case mcNoUse:
    case mcWithoutMK:
    case mcBrokenNoUse:
    case mcBrokenNeedFormat:
    case mcGstLoad:
    case mcLoaded:
    case mcNoSpaceNoSave:
    case mcTooManyFiles:
    case mcGstSave:
    case mcSaved:
    case mcGstOverwrite:
    case mcNoGst:
    case mcTooManyFilesContinue:
    case mcNoSpaceContinue:
    case mcMsg38:
    case mcMsg39:
    case mcMsg40:
        result = 0;
        break;
    case mcMissSaved:
    case mcMissFormat:
    case mcMissMakeFile:
    case mcGstNoLoad:
    case mcGstBrokenNoLoad:
    case mcBrokenNoLoad:
        result = 1;
        break;
    default:
#line 687
        JUT_ASSERT(0);
    }

    return result;
}
