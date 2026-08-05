#ifndef PRINTPROGRESSIVE_H
#define PRINTPROGRESSIVE_H

#include <JSystem/JKernel/JKRHeap.h>
#include "types.h"

class PrintProgressive
{
public:
    enum MessageID // according to debug test screen
    {
        mcShow_Dialog,
        mcSet_Option1,
        mcSet_Option2,
        mcShow_Logo,
        mcShow_Set
    };
    PrintProgressive(JKRHeap *); // 0x8016aac4
    void init(MessageID);        // 0x8016b138
    void draw();                 // 0x8016b65c
    void calc(int);              // 0x8016b6dc
    void isMessage();            // 0x8016bc28
    void isChoice();             // 0x8016bcbc
    void getCounter();           // 0x8016bd48
    // Inline/Unused
    // void ~PrintProgressive();
    // void calc_bck();
    // void calc_texChange();
    // void setCounter(int);

    int getState() { return _68; }
    int getOption() { return _6c; }

private:
    // TODO
    u8 _0[0x64];
    MessageID mMsgId;
    int _68;
    int _6c;
    u8 _70[0x94 -0x70];
}; // Size: 0x94

#endif // PRINTPROGRESSIVE_H
