#include "Kameda/PrintWindow.h"

#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DPane.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/ReadPrintMessage.h"
#include "Osako/FontMgr.h"
#include "Osako/ResMgr.h"
#include "Osako/system.h"
#include "string.h"

const u64 PrintWindow::mBckTag[13] = {
    'NGDcw1', 'GDEf_011', 'GDEf_021', 'GDEf_031',
    'GDEf_041', 'GDEf_051', 'GDEf_061', 'GDEf_071',
    'GDEf_081', 'GDEf_091', 'GDEf_101', 'GDEf_111',
    'GDEf_121'
};

const u64 PrintWindow::mBtkTag[4] = {
    'WGD_Li1', 'GDhiLi1', 'GDNoW1', 'GDYesW1',
};

const u64 PrintWindow::mBrkTag[9] = {
    'WGD1', 'WGD_Li1', 'GDhiLi1', 'GDNoB1',
    'GDYesB1', 'GDNoW1', 'GDYesW1', 'GDYesM1',
    'GDNoM1'
};

const u64 PrintWindow::mTextBoxTag[4] = {
    'TGDEr1', 'TGD1', 'GDYesM1', 'GDNoM1'
};

const u64 PrintWindow::mVisibleTag[3] = {
    'N_YN1', 'NGDNo1', 'NGDyes1'
};

PrintWindow::PrintWindow(JKRHeap *heap)
{
    mGrafContext = System::getJ2DOrtho();

    mArchive = JKRMountArchive(ResMgr::getPtr(ResMgr::mcArcSystem, "Screen/window.arc"),
                               heap,
                               JKRArchive::MOUNT_DIRECTION_HEAD);

    mScreen = new (heap, 0) J2DScreen();
    mScreen->J2DScreen::set("Window.blo", 0x40000, mArchive);

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 13; ++j)
        {
            u64 id = mBckTag[j] + i;

            J2DPane *pane = mScreen->search(id);
            mBckContainer[i][j].pane = pane;

            void *resource = JKRFileLoader::getGlbResource("Window.bck", mArchive);
            J2DAnmBase *animation = J2DAnmLoaderDataBase::load(resource);
            mBckContainer[i][j].animation = animation;

            mBckContainer[i][j].pane->setAnimation(
                (J2DAnmTransform *)mBckContainer[i][j].animation);
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            u64 id = mBtkTag[j] + i;

            J2DPane *pane = mScreen->search(id);
            mBtkContainer[i][j].pane = pane;

            void *resource = JKRFileLoader::getGlbResource("Window.btk", mArchive);
            J2DAnmBase *animation = J2DAnmLoaderDataBase::load(resource);
            mBtkContainer[i][j].animation = animation;

            mBtkContainer[i][j].animation->searchUpdateMaterialID(mScreen);
            mBtkContainer[i][j].pane->setAnimation(
                (J2DAnmTextureSRTKey *)mBtkContainer[i][j].animation);
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            u64 id = mBrkTag[j] + i;

            J2DPane *pane = mScreen->search(id);
            mBrkContainer[i][j].pane = pane;

            void *resource = JKRFileLoader::getGlbResource("Window.brk", mArchive);
            J2DAnmBase *animation = J2DAnmLoaderDataBase::load(resource);
            mBrkContainer[i][j].animation = animation;

            mBrkContainer[i][j].animation->searchUpdateMaterialID(mScreen);
            mBrkContainer[i][j].pane->setAnimation(
                (J2DAnmTevRegKey *)mBrkContainer[i][j].animation);
        }
    }

    char buffer[512];
    for (int i = 0; i < 512; ++i)
    {
        buffer[i] = ' ';
    }
    buffer[511] = '\0';

    char shorterBuffer[32];
    for (int i = 0; i < 32; ++i)
    {
        shorterBuffer[i] = ' ';
    }
    buffer[511] = '\0';  // Apparent programmer error; null character set in the wrong buffer.

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            u64 id = mTextBoxTag[j] + i;

            J2DTextBox *pane = (J2DTextBox *)mScreen->search(id);
#line 116
            JUT_ASSERT(pane->getTypeID() == J2DPane_TextBox);

            mTextBoxContainer[i].mTextBox[j] = pane;

            mTextBoxContainer[i].mTextBox[j]->setFont(FontMgr::mspResFont);

            if (j < 2)
            {
                mTextBoxContainer[i].mTextBox[j]->setString(buffer);
            }
            else
            {
                mTextBoxContainer[i].mTextBox[j]->setString(shorterBuffer);
            }

            mTextBoxContainer[i].mTextBox[j]->getFontSize(mFontSizeContainer[i].mFontSize[j]);
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            u64 id = mVisibleTag[j] + i;
            J2DPane *pane = mScreen->search(id);

            mVisibleContainer[i][j].pane = pane;
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            void *colorRes = ResMgr::getPtr(ResMgr::mcColor);
            void *yesNoRes = ResMgr::getPtr(ResMgr::mcYesNo);

            int msgIdx = 1 - j;
            char *message = ReadPrintMessage::getMessage(yesNoRes, msgIdx);
            JUTColor color =
                ReadPrintMessage::getColor(colorRes, ReadPrintMessage::getDefaultColor(yesNoRes));
            mTextBoxContainer[i].mTextBox[j + 2]->setFontColor(color, color);

            J2DTextBox::TFontSize fontSize = mFontSizeContainer[i].mFontSize[j + 2];
            mTextBoxContainer[i].mTextBox[j + 2]->setFontSize(
                fontSize.x * ReadPrintMessage::getRate(yesNoRes, msgIdx) / 100.0f,
                fontSize.y * ReadPrintMessage::getRate(yesNoRes, msgIdx) / 100.0f);

            mTextBoxContainer[i].mTextBox[j + 2]->setCharSpace(
                ReadPrintMessage::getCharSpace(yesNoRes, msgIdx));

            J2DTextBox::TFontSize fontSize2;
            mTextBoxContainer[i].mTextBox[j + 2]->getFontSize(fontSize2);
            char buf[32];
            ReadPrintMessage::tagCnv(message, colorRes, fontSize2, sizeof(buf), buf);
            strncpy(mTextBoxContainer[i].mTextBox[j + 2]->getStringPtr(), buf, sizeof(buf));
        }
    }

    init();
}

void PrintWindow::init()
{
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 13; ++j)
        {
            mBckContainer[i][j].frame = 0.0f;
        }

        for (int j = 0; j < 4; ++j)
        {
            mBtkContainer[i][j].frame = 0.0f;
        }

        for (int j = 0; j < 9; ++j)
        {
            mBrkContainer[i][j].frame = 0.0f;
        }
    }

    mInfo.type = 1;
    mInfo._2ec = 1;
    mInfo._2f0 = 1;
    mInfo.frame = 0.0f;
    mInfo._2f8 = 2;
}

void PrintWindow::draw()
{
    mGrafContext->setPort();
    mScreen->draw(0.0f, 0.0f, mGrafContext);
}

void PrintWindow::calc()
{
    switch (mInfo.type)
    {
    case 0:
        mBckContainer[0][0].pane->mIsVisible = true;
        mBckContainer[1][0].pane->mIsVisible = false;
        break;
    case 1:
        mBckContainer[0][0].pane->mIsVisible = false;
        mBckContainer[1][0].pane->mIsVisible = true;
        break;
    default:
#line 215
        JUT_ASSERT(0);
    }

    for (int i = 0; i < 2; ++i)
    {
        switch (mInfo._2ec)
        {
        case 0:
            mTextBoxContainer[i].mTextBox[0]->mIsVisible = false;
            mVisibleContainer[i][0].pane->mIsVisible = true;
            break;
        case 1:
            mTextBoxContainer[i].mTextBox[0]->mIsVisible = true;
            mVisibleContainer[i][0].pane->mIsVisible = false;
            break;
        default:
#line 232
            JUT_ASSERT(0);
        }

        switch (mInfo._2f0)
        {
        case 0:
            mBrkContainer[i][0].frame = 0.0f;
            mBrkContainer[i][1].frame = 0.0f;
            mBrkContainer[i][2].frame = 0.0f;
            mBrkContainer[i][3].frame = 0.0f;
            mBrkContainer[i][4].frame = 0.0f;
            break;
        case 1:
            mBrkContainer[i][0].frame = 1.0f;
            mBrkContainer[i][1].frame = 1.0f;
            mBrkContainer[i][2].frame = 1.0f;
            mBrkContainer[i][3].frame = 1.0f;
            mBrkContainer[i][4].frame = 1.0f;
            break;
        default:
#line 253
            JUT_ASSERT(0);
        }

        mBckContainer[i][0].frame = mInfo.frame;

        switch (mInfo._2f8)
        {
        case 0:
            mBrkContainer[i][6].frame = 0.0f;
            mBrkContainer[i][7].frame = 0.0f;
            mBrkContainer[i][8].frame = 1.0f;
            if (mInfo._2f0 == 1)
            {
                mBrkContainer[i][5].frame = 2.0f;
            }
            else
            {
                mBrkContainer[i][5].frame = 1.0f;
            }
            mVisibleContainer[i][1].pane->mIsVisible = false;
            mVisibleContainer[i][2].pane->mIsVisible = true;
            break;
        case 1:
            mBrkContainer[i][5].frame = 0.0f;
            mBrkContainer[i][8].frame = 0.0f;
            mBrkContainer[i][7].frame = 1.0f;
            if (mInfo._2f0 == 1)
            {
                mBrkContainer[i][6].frame = 2.0f;
            }
            else
            {
                mBrkContainer[i][6].frame = 1.0f;
            }
            mVisibleContainer[i][1].pane->mIsVisible = true;
            mVisibleContainer[i][2].pane->mIsVisible = false;
            break;
        case 2:
            mVisibleContainer[i][1].pane->mIsVisible = false;
            mVisibleContainer[i][2].pane->mIsVisible = false;
            break;
        default:
#line 293
            JUT_ASSERT(0);
        }

        JUtility::TColor cornerColors[4];
        if (mInfo._2f0 == 1)
        {
            J2DPicture *pane = (J2DPicture *)mScreen->search('C4_2');
#line 300
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            cornerColors[0] = pane->mCornerColors[0];
            cornerColors[1] = pane->mCornerColors[1];
            cornerColors[2] = pane->mCornerColors[2];
            cornerColors[3] = pane->mCornerColors[3];
        }
        else
        {
            J2DPicture *pane = (J2DPicture *)mScreen->search('C4_1');
#line 304
            JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
            cornerColors[0] = pane->mCornerColors[0];
            cornerColors[1] = pane->mCornerColors[1];
            cornerColors[2] = pane->mCornerColors[2];
            cornerColors[3] = pane->mCornerColors[3];
        }
        // `cornerColors` does not appear to be read; only written.

        for (int j = 1; j < 13; ++j)
        {
            if (++mBckContainer[i][j].frame >= 79.0f)
            {
                mBckContainer[i][j].frame = 0.0f;
            }
        }

        if (++mBtkContainer[i][0].frame >= 360.0f)
        {
            mBtkContainer[i][0].frame = 0.0f;
        }
        if (++mBtkContainer[i][1].frame >= 120.0f)
        {
            mBtkContainer[i][1].frame = 0.0f;
        }
        if (++mBtkContainer[i][2].frame >= 120.0f)
        {
            mBtkContainer[i][2].frame = 0.0f;
        }
        if (++mBtkContainer[i][3].frame >= 120.0f)
        {
            mBtkContainer[i][3].frame = 0.0f;
        }

        for (int j = 0; j < 13; ++j)
        {
            mBckContainer[i][j].animation->mCurrentFrame = mBckContainer[i][j].frame;
        }
        for (int j = 0; j < 4; ++j)
        {
            mBtkContainer[i][j].animation->mCurrentFrame = mBtkContainer[i][j].frame;
        }
        for (int j = 0; j < 9; ++j)
        {
            mBrkContainer[i][j].animation->mCurrentFrame = mBrkContainer[i][j].frame;
        }
    }

    mScreen->animation();
}

J2DTextBox *PrintWindow::getTextBox()
{
    J2DTextBox *textBox;

#line 336
    JUT_MINMAX_ASSERT(0, mInfo.type, 2);

    switch (mInfo._2ec)
    {
    case 0:
        textBox = mTextBoxContainer[mInfo.type].mTextBox[1];
        break;
    case 1:
        textBox = mTextBoxContainer[mInfo.type].mTextBox[0];
        break;
    default:
#line 347
        JUT_ASSERT(0);
    }

    return textBox;
}

J2DTextBox::TFontSize PrintWindow::getBaseFontSize()
{
    J2DTextBox::TFontSize fontSize;

#line 363
    JUT_MINMAX_ASSERT(0, mInfo.type, 2);

    switch (mInfo._2ec)
    {
    case 0:
        fontSize = mFontSizeContainer[mInfo.type].mFontSize[1];
        break;
    case 1:
        fontSize = mFontSizeContainer[mInfo.type].mFontSize[0];
        break;
    default:
#line 374
        JUT_ASSERT(0);
    }
    return fontSize;
}
