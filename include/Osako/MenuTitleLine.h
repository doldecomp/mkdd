#ifndef MENUTITLELINE_H
#define MENUTITLELINE_H

#include "JSystem/J2D/J2DPane.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRArchive.h"

class MenuTitleLine
{
public:
    enum Title
    {
        mcHowManyPlayers,
        mcCharacterSelect,
        mcSelectMode,
        mcVsCoop,
        mcCourseSelect,
        mcBattleSelect,
        mcCupSelect,
        mcMapSelect,
        mcGhostCheck,
        mcGhostLoad,
        mcGhostSave,
        mcTitleMax
    };

    // Fabricated
    enum Animation {
        mcAnim0,
        mcAnimDrop,
        mcAnim2,
        mcAnimLift
    };

    // Global
    MenuTitleLine(JKRArchive *archive); // 0x80219158
    void drop(Title);                   // 0x802192e8
    void lift();                        // 0x802193c8
    void calc();                        // 0x802193d4

    ~MenuTitleLine() { mspMenuTitleLine = nullptr; }

    static MenuTitleLine *create(JKRArchive *archive, JKRHeap *heap)
    {
        if (mspMenuTitleLine == nullptr)
            mspMenuTitleLine = new (heap, 0) MenuTitleLine(archive);
        return mspMenuTitleLine;
    }

    static MenuTitleLine *ptr() { return mspMenuTitleLine; }

private:
    static MenuTitleLine *mspMenuTitleLine; // 0x80416a90

    Animation mAnmType;
    Title mTitle;
    J2DScreen mScreen; // 08 - 120 J2DScreen
    J2DPicture *mPicture; // might be J2DPictureEx?
    J2DPicture *mGhostPicture;
    J2DAnmTransform *mTransform;
    J2DAnmTextureSRTKey *mTexSRTKey;
    s16 mTransFrame;
    s16 mKeyFrame;
    ResTIMG *mImages[mcTitleMax];
}; // Size: 0x160

#endif // MENUTITLELINE_H
