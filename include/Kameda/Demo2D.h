#ifndef DEMO2D_H
#define DEMO2D_H

#include "Kaneshige/HioMgr.h"

class J2DOrthoGraph;
class J2DScreen;

class Demo2DParam : public JORReflexible {
public:
    Demo2DParam();
    virtual ~Demo2DParam() {}

    u16 _48;
    u8 _4A;
}; // class Demo2DParam

class Demo2DHioNode : public HioNode {
public:
    Demo2DHioNode() : HioNode("Ｄｅｍｏ２Ｄ", &mParam, 0, 0) {}

    Demo2DParam mParam;
}; // class Demo2DHioNode

class Demo2D {
public:
    Demo2D(JKRHeap *heap); // 0x8018313c
    void init();           // 0x8018325c
    void draw();           // 0x801832d0
    void calc();           // 0x80183344

    void reset() { init(); }

    // Inline/Unused
    ~Demo2D();

private:
    Demo2DHioNode *mNode;
    J2DOrthoGraph *mOrtho;
    J2DScreen *mScreen;
}; // class Demo2D

#endif // DEMO2D_H
