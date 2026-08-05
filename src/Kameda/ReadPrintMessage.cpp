#include <string.h>
#include <JSystem/JUtility/JUTDbg.h>
#include <JSystem/J2D/J2DPane.h>
#include "Kameda/ReadPrintMessage.h"

namespace ReadPrintMessage
{
    int mNum[10];
    char *mStr[5];
    Inf *mInfoPtr;
    Block *mDataPtr;

    u32 getColor(void *bmcPtr, int index)
    {
#line 35
        JUT_ASSERT(bmcPtr != 0);

        BMC *ptr = (BMC *)bmcPtr;
        JUT_ASSERT(ptr->header.signature == 'MGCL');
        JUT_ASSERT(ptr->header.dataType == 'bmc1');
        JUT_ASSERT(ptr->header.dataSize == 1088);
        JUT_ASSERT(ptr->header.numBlocks == 1);
        JUT_ASSERT(ptr->colorTable.header.kind == 'CLT1');
        JUT_ASSERT(ptr->colorTable.numColors == 256);
        JUT_ASSERT(ptr->colorTable.colorFormat == 0);

        JUT_MINMAX_ASSERT(0, index, 256);
        return ptr->colorTable.entries[index];
    }

    void readBmg(void *bmgPtr)
    {
#line 58
        JUT_ASSERT(bmgPtr != 0);

        Header *headerPtr = (Header *)bmgPtr;
        Block *blockPtr = (Block *)(headerPtr + 1);

        JUT_ASSERT(headerPtr->signature == 'MESG');
        JUT_ASSERT(headerPtr->dataType == 'bmg1');
        // JUT_ASSERT(headerPtr->dataSize == 1088);

        JUT_ASSERT(headerPtr->numBlocks <= 2);

        for (int i = 0; i < headerPtr->numBlocks; i++)
        {
            Block *curBlock = blockPtr;
            blockPtr = (Block *)((u8 *)blockPtr + blockPtr->size);
            switch (curBlock->kind)
            {
            case BLOCK_KIND_INF:
                mInfoPtr = (Inf *)curBlock;
                break;
            case BLOCK_KIND_DAT:
                mDataPtr = curBlock;
                break;
            default:
#line 80
                JUT_ASSERT(0);
            }
        }
    }

    u8 getDefaultColor(void *bmgPtr)
    {
        readBmg(bmgPtr);
        return mInfoPtr->defaultColor;
    }

    u16 getRate(void *bmgPtr, int idx)
    {
        readBmg(bmgPtr);
        return mInfoPtr->entries[idx].rate;
    }

    s8 getCharSpace(void *bmgPtr, int idx)
    {
        readBmg(bmgPtr);
        return mInfoPtr->entries[idx].charSpace;
    }

    char *getMessage(void *bmgPtr, int idx)
    {
        readBmg(bmgPtr);
        return (char *)&mDataPtr->data[mInfoPtr->entries[idx].offset];
    }

    void tagCnv(char *msg, void *bmcPtr, J2DTextBox::TFontSize fontSize, int n, char *txtBuf)
    {
        while (*msg && n--)
        {
            u8 tagData[32];
            char buf[32];

            if (*msg != 0x1a)
            { // no escape sequence, normal formatting
                *txtBuf++ = *msg++;
                continue;
            }
            *msg++;

            u8 tagSize = *msg++;
            u8 tagGroup = *msg++;
            u16 tagID = (*msg++ << 8);
            tagID += *msg++;

            for (u8 i = 0; i < tagSize - 5; i++)
                tagData[i] = *msg++;

            switch (tagGroup)
            {
            case 0:
            {
                switch (tagID)
                {
                case 0:
                {
                    JUtility::TColor color = getColor(bmcPtr, tagData[0]);
                    JUtility::TColor gradient = getColor(bmcPtr, tagData[1]);
                    snprintf(buf, sizeof(buf), "\eCC[%02X%02X%02X%02X]\eGC[%02X%02X%02X%02X]",
                             color.r, color.g, color.b, color.a,
                             gradient.r, gradient.g, gradient.b, gradient.a);
                    break;
                }
                default:
#line 193
                    JUT_ASSERT(0);
                }
                break;
            }
            case 1:
                JUT_MINMAX_ASSERT(0, tagID, 10);
                snprintf(buf, sizeof(buf), "%d", mNum[tagID]);
                break;
            case 2:
                JUT_MINMAX_ASSERT(0, tagID, 10);
                snprintf(buf, sizeof(buf), "%02d", mNum[tagID]);
                break;
            case 3:
                JUT_MINMAX_ASSERT(0, tagID, 10);
                snprintf(buf, sizeof(buf), "%03d", mNum[tagID]);
                break;
            case 4:
                JUT_MINMAX_ASSERT(0, tagID, 5);
                snprintf(buf, sizeof(buf), "%s", mStr[tagID]);
                break;
            case 0xff:
                switch (tagID)
                {
                case 0:
                    JUtility::TColor color = getColor(bmcPtr, tagData[0]);
                    snprintf(buf, sizeof(buf), "\eCC[%02X%02X%02X%02X]\eGC[%02X%02X%02X%02X]",
                             color.r, color.g, color.b, color.a,
                             color.r, color.g, color.b, color.a);
                    break;
                case 1:
                    u16 scalar = (u16)((tagData[0] << 8) + tagData[1]);
                    int fontX = fontSize.x * scalar / 100.0f;
                    int fontY = fontSize.y * scalar / 100.0f;
                    snprintf(buf, sizeof(buf), "\eFX[%d]\eFY[%d]", fontX, fontY);
                    break;
                default:
#line 240
                    JUT_ASSERT(0);
                    break;
                }
                break;
            default:
                JUT_ASSERT(0);
                break;
            }

            for (char *bufPtr = buf; *bufPtr != 0;)
                *txtBuf++ = *bufPtr++;
        }
        *txtBuf = 0;
    }

    int getBmgMessageID(PrintDvdError::MessageID msgID)
    {
        int id;
        switch (msgID)
        {
        case PrintDvdError::mcFatalError:
            id = 0;
            break;
        case PrintDvdError::mcRetryError:
            id = 2;
            break;
        case PrintDvdError::mcCoverOpen:
            id = 3;
            break;
        case PrintDvdError::mcDiskError:
            id = 1;
            break;
        case PrintDvdError::mcLANError0:
            id = 0x50005;
            break;
        case PrintDvdError::mcLANError1:
            id = 0x50006;
            break;
        case PrintDvdError::mcLANError2:
            id = 0x50007;
            break;
        case PrintDvdError::mcLANError3:
            id = 0x50008;
            break;
        case PrintDvdError::mcLANDisconnect:
            id = 0x50003;
            break;
        case PrintDvdError::mcLANDisconnectError:
            id = 0x50004;
            break;
        default:
#line 301
            JUT_ASSERT(0);
            break;
        }

        return id % 65536;
    }

    int getBmgMessageID(PrintProgressive::MessageID msgID)
    {
        int id;
        switch (msgID)
        {
        case PrintProgressive::mcShow_Dialog:
            id = 0x10002;
            break;
        case PrintProgressive::mcSet_Option1:
            id = 0x10001;
            break;
        case PrintProgressive::mcSet_Option2:
            id = 0x10000;
            break;

        case PrintProgressive::mcShow_Logo: // there's no text here, so panic
        default:
#line 330
            JUT_ASSERT(0);
            break;
        }
        return id % 65536;
    }

    int getBmgMessageID(PrintPal::MessageID msgID)
    {
        int id;
        switch (msgID)
        {
        case PrintPal::mcShow_Dialog:
            id = 0x30000;
            break;
        case PrintPal::mcSet_Option1:
            id = 0x30001;
            break;
        case PrintPal::mcSet_Option2:
            id = 0x30002;
            break;
        default:
            JUT_ASSERT(0);
            break;
        }
        return id % 65536;
    }

    int getBmgMessageID(PrintMemoryCard::MessageID msgID)
    {
        int id;
        switch (msgID)
        { // TODO
        case PrintMemoryCard::mcNoCardContinue:
            id = 0x20017;
            break;
        case PrintMemoryCard::mcNoUseContinue:
            id = 0x2001e;
            break;
        case PrintMemoryCard::mcWithouMKContinue:
            id = 0x20023;
            break;
        case PrintMemoryCard::mcBrokenNoSaveContinue:
            id = 0x20003;
            break;
        case PrintMemoryCard::mcBrokenGoFormat:
            id = 0x20000;
            break;
        case PrintMemoryCard::mcNoFileNoSave:
            id = 0x20018;
            break;
        case PrintMemoryCard::mcGameSavingNoTouch:
            id = 0x20008;
            break;
        case PrintMemoryCard::mcGetSavingNoTouch:
            id = 0x2000e;
            break;
        case PrintMemoryCard::mcMissSaved:
            id = 0x20016;
            break;
        case PrintMemoryCard::mcInCardPushButton:
            id = 0x2000f;
            break;
        case PrintMemoryCard::mcChangeCardPushButton:
            id = 0x20005;
            break;
        case PrintMemoryCard::mcMissFormat:
            id = 0x20014;
            break;
        case PrintMemoryCard::mcFormatLost:
            id = 0x20006;
            break;
        case PrintMemoryCard::mcFormatNoTouch:
            id = 0x20007;
            break;
        case PrintMemoryCard::mcNoSaveContinue:
            id = 0x2001a;
            break;
        case PrintMemoryCard::mcMakeFile:
            id = 0x20012;
            break;
        case PrintMemoryCard::mcMakeFileNoTouch:
            id = 0x20013;
            break;
        case PrintMemoryCard::mcMadeFile:
            id = 0x20011;
            break;
        case PrintMemoryCard::mcMissMakeFile:
            id = 0x20015;
            break;
        case PrintMemoryCard::mcNoSpaceGoIPL:
            id = 0x2001b;
            break;
        case PrintMemoryCard::mcTooManyFilesGoIPL:
            id = 0x20021;
            break;
        case PrintMemoryCard::mcNoUse:
            id = 0x2001d;
            break;
        case PrintMemoryCard::mcWithoutMK:
            id = 0x20022;
            break;
        case PrintMemoryCard::mcBrokenNoUse:
            id = 0x20004;
            break;
        case PrintMemoryCard::mcBrokenNeedFormat:
            id = 0x20001;
            break;
        case PrintMemoryCard::mcGstLoad:
            id = 0x2000a;
            break;
        case PrintMemoryCard::mcLoaded:
            id = 0x20010;
            break;
        case PrintMemoryCard::mcNoSpaceNoSave:
            id = 0x2001c;
            break;
        case PrintMemoryCard::mcTooManyFiles:
            id = 0x20020;
            break;
        case PrintMemoryCard::mcGstSave:
            id = 0x2000d;
            break;
        case PrintMemoryCard::mcSaved:
            id = 0x2001f;
            break;
        case PrintMemoryCard::mcGstOverwrite:
            id = 0x2000c;
            break;
        case PrintMemoryCard::mcNoGst:
            id = 0x20019;
            break;
        case PrintMemoryCard::mcGstNoLoad:
            id = 0x2000b;
            break;
        case PrintMemoryCard::mcGstBrokenNoLoad:
            id = 0x20009;
            break;
        case PrintMemoryCard::mcBrokenNoLoad:
            id = 0x20002;
            break;
        case PrintMemoryCard::mcMsg40:
        default:
#line 486
            JUT_ASSERT(0);
            break;
        case PrintMemoryCard::mcTooManyFilesContinue:
            id = 0x50000;
            break;
        case PrintMemoryCard::mcNoSpaceContinue:
            id = 0x50002;
            break;
        case PrintMemoryCard::mcMsg38:
            id = 0x50003;
            break;
        case PrintMemoryCard::mcMsg39:
            id = 0x80003;
            break;
        }

        return id % 65536;
    }

    int getBmgMessageID(int msgID)
    {
        int id;

        switch (msgID)
        {
        case 0:
            id = 0x40000;
            break;
        case 1:
            id = 0x40002;
            break;
        case 2:
            id = 0x40001;
            break;
        default:
#line 526
            JUT_ASSERT(0);
            break;
        }

        return id % 65536;
    }
}
