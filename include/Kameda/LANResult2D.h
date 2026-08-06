#ifndef LANRESULT2D_H
#define LANRESULT2D_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JORReflexible.h"

#include "Kaneshige/HioMgr.h"

class LANResult2DParam : public JORReflexible {
public:
    LANResult2DParam() : _48(0), mFlag(1) {}
    virtual ~LANResult2DParam();

    u16 _48; // 0x48
    u8 mFlag; // 0x4a
};

class LANResult2DHioNode : public HioNode {
public:
    LANResult2DHioNode()
    : HioNode("ＬＡＮＲｅｓｕｌｔ２Ｄ", &mParam, 0, 0) {}

    virtual ~LANResult2DHioNode();

    LANResult2DParam mParam;
};

class LANResult2D {
public:
    LANResult2D(JKRHeap *heap); // 0x80195358
    ~LANResult2D(); // Unused

    void init(); // 0x801954a8
    void draw(); // 0x80195554
    void calc(); // 0x801955f0
    void start(int kart); // 0x80195664

    void reset() { init(); }

private:
    J2DAnmTransform* mAnimation; // 0x0
    PLACEHOLDER_BYTES(0x04, 0x08); // 0x4
    f32 mTransformFrame; // 0x08
    LANResult2DHioNode* mNode; // 0xc
    J2DOrthoGraph* mOrtho; // 0x10
    J2DScreen* mScreen; // 0x14
    bool mDrawFlag; // 0x18
    PLACEHOLDER_BYTES(0x19, 0x1C); // 0x19
}; // class LANResult2D
#endif // LANRESULT2D_H
