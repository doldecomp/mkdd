#ifndef K2DPICTURE_H
#define K2DPICTURE_H

#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/JGeometry/Box.h"
#include "JSystem/JUtility/TColor.h"

class K2DPicture {
public:
    static u8 mK2DGlobalAlpha; // 0x804145a8

    void drawK2D(f32 x0, f32 y0, f32 x1, f32 y1, bool tevMode); // 0x8013b9e8
    void setTevModeK2D(); // 0x8013bd34
    void makeMatrix(f32 x, f32 y); // 0x8013c0fc
    inline void getNewColor(JUTColor colors[4]); // 0x8013bbf8

    // Inline/Unused
    void drawOutK2D(const JGeometry::TBox2f &, const JGeometry::TBox2f &);
    ~K2DPicture();

    J2DPicture mPicture;
}; // class K2DPicture
#endif // K2DPICTURE_H
