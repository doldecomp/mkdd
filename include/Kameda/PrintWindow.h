#ifndef PRINTWINDOW_H
#define PRINTWINDOW_H

#include "types.h"
#include <JSystem/J2D/J2DPrint.h>
#include <JSystem/J2D/J2DPane.h>
#include <JSystem/JKernel/JKRHeap.h>

class PrintWindow
{
public:
    PrintWindow(JKRHeap *heap);
    void init();
    void draw();
    void calc();

    J2DTextBox *getTextBox();
    J2DTextBox::TFontSize getBaseFontSize();

    // TODO: many inlines

    static const u64 mBckTag[13];    // 0x80375e70
    static const u64 mBtkTag[4];     // 0x80375ed8
    static const u64 mBrkTag[9];     // 0x80375ef8
    static const u64 mTextBoxTag[4]; // 0x80375f40
    static const u64 mVisibleTag[3]; // 0x80375f60

//private:
    struct Container
    {
        J2DAnmBase *animation;
        J2DPane *pane;
        f32 frame;
    };

    struct TextBoxContainer
    {
        J2DTextBox *mTextBox[4];
    };

    struct FontSizeContainer
    {
        J2DTextBox::TFontSize mFontSize[4];
    };

    struct VisibleContainer
    {
        J2DPane *pane;
    };

    Container mBckContainer[2][13];            // 0x0
    Container mBtkContainer[2][4];             // 0x138
    Container mBrkContainer[2][9];             // 0x198
    TextBoxContainer mTextBoxContainer[2];     // 0x270
    FontSizeContainer mFontSizeContainer[2];   // 0x290
    VisibleContainer mVisibleContainer[2][3];  // 0x2d0

    struct
    {
        s32 type;   // 0x2e8
        u32 _2ec;
        s32 _2f0;
        f32 frame;  // 0x2f4
        u32 _2f8;
    } mInfo;

    J2DGrafContext *mGrafContext;  // 0x2fc
    JKRArchive *mArchive;          // 0x300
    J2DScreen *mScreen;            // 0x304
}; // Size: 0x308

#endif
