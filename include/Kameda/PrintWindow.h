#ifndef PRINTWINDOW_H
#define PRINTWINDOW_H

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

//private:
    u8 _0[0x2e8];
    u32 _2e8;
    u32 _2ec;
    u32 _2f0;
    f32 _2f4;
    u32 _2f8;
    u8 _2fc[0x308 - 0x2fc];
}; // Size: 0x308

#endif
