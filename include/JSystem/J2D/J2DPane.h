#ifndef _JSYSTEM_J2D_J2DPANE_H
#define _JSYSTEM_J2D_J2DPANE_H

#include <string.h>
#include "JSystem/JGeometry/Box.h"
#include "dolphin/gx/GXEnum.h"
#include "types.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

#include "JSystem/J2D/J2DAnimation.h"
#include "JSystem/J2D/J2DTypes.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JSupport/JSUStream.h"
#include "JSystem/JUtility/TColor.h"
#include "JSystem/J2D/J2DMaterial.h"
#include "JSystem/J2D/J2DManage.h"

struct J2DAnmBase;
struct J2DAnmColor;
struct J2DAnmTevRegKey;
struct J2DAnmTexPattern;
struct J2DAnmTextureSRTKey;
struct J2DAnmTransform;
struct J2DAnmVisibilityFull;
struct J2DAnmVtxColor;
struct J2DGrafContext;
struct J2DMaterial;
struct JKRArchive;
struct JUTFont;
struct JUTNameTab;
struct JUTPalette;
struct JUTResFont;
struct JUTTexture;
struct ResFONT;
struct ResTIMG;
struct ResTLUT;

enum J2DPaneType
{
    J2DPane_Screen = 0x8,
    J2DPane_Pane = 0x10,
    J2DPane_Window = 0x11,
    J2DPane_Picture = 0x12,
    J2DPane_TextBox = 0x13,

    J2DPaneType_Screen = 0x8,
    J2DPaneType_Pane = 0x10,
    J2DPaneType_Window = 0x11,
    J2DPaneType_Picture = 0x12,
    J2DPaneType_TextBox = 0x13
};

enum J2DRotateAxis
{
    J2DROTATE_X = 'x', // 0x78
    J2DROTATE_Y = 'y', // 0x79
    J2DROTATE_Z = 'z', // 0x7A
};

// Intended to be size 0x1.
enum J2DBasePosition
{
    J2DPOS_TopLeft = 0,
    J2DPOS_TopCenter = 1,
    J2DPOS_TopRight = 2,
    J2DPOS_CenterLeft = 3,
    J2DPOS_Center = 4,
    J2DPOS_CenterRight = 5,
    J2DPOS_BottomLeft = 6,
    J2DPOS_BottomCenter = 7,
    J2DPOS_BottomRight = 8,
};

struct J2DScrnBlockHeader
{
    u32 mBloBlockType; // _00
    int mBlockLength;  // _04
};

/**
 * @fabricated
 * @size{0x20}
 */
struct J2DTextBoxBlock {
    u16 _00;             // _00
    u16 mAnimPaneIndex;  // _02
    u16 mMaterialNum;    // _04
    s16 mCharSpacing;    // _06 - converted to f32
    s16 mLineSpacing;    // _08 - converted to f32
    u16 mFontSizeX;      // _0A - converted to f32
    u16 mFontSizeY;      // _0C - converted to f32 for _120
    u8 mHBind;           // _0E - J2DTextBoxHBinding for _130
    u8 mVBind;           // _0F - J2DTextBoxVBinding for _130
    u32 mCharColor;      // _10
    u32 mGradientColor;  // _14
    u8 mDoConnectParent; // _18
    u8 _19[3];           // _19, padding?
    u16 mTextBoxLength;  // _1C
    u16 mMaxReadLength;  // _1E
};

// Representation of "PAN2" entry in a scrnblo2 file
struct J2DPaneExBlock : J2DScrnBlockHeader {
    u16 _08;          // _08, padding?
    u16 mAnimIndex;   // _0A, "joint" index used for applying .bck animations
    bool mIsVisible;  // _0C, whether the pane is visible by default, applies to all children as well
    u8 mBasePosition; // _0D, anchor point for position, see J2DBasePosition enum
    u16 _0E;          // _0E, padding
    u64 mTag;         // _10, name of the pane
    u64 mMessageID;   // _18, secondary tag of pane, usually used for message id
    f32 mWidth;       // _20, pixel dimensions X (unused in j2dpane)
    f32 mHeight;      // _24, pixel dimensions Y (unused in j2dpane)
    f32 mWidthScale;  // _28, horizontal scale modifier
    f32 mHeightScale; // _2C, vertical scale modifier
    f32 mAngleX;      // _30, X axis rotation, never touched normally
    f32 mAngleY;      // _34, Y axis rotation, never touched normally
    f32 mAngleZ;      // _38, Z axis rotation, the one you probably want
    f32 mOffsetX;     // _3C, base X position, relative to parent
    f32 mOffsetY;     // _40, base Y position, relative to parent
};

/**
 * @size{0x30}
 * @fabricated
 */
struct J2DPictureBlockTrailer
{
    u8 _00[2];                      // _00
    s16 _02;                        // _02
    u16 _04;                        // _04
    u8 _06[2];                      // _06 - padding?
    s16 _08[2][2];                  // _08
    JGeometry::TVec2<short> _10[4]; // _10
    _GXColor _20[4];                // _20
};

/**
 * @size{0x30}
 * @fabricated
 */
struct J2DScrnBlockPictureParameter {
    u8 _00[2];                           // _00
    u16 mMaterialNum;                    // _02
    u16 mMaterialID;                     // _04
    u8 _06[2];                           // _06 - padding?
    u16 _08[4];                          // _08
    JGeometry::TVec2<s16> mTexCoords[4]; // _10
    u32 mCornerColor[4];                 // _20
};

struct J2DScreenInfoBlock : public J2DScrnBlockHeader
{

    // _00-_08 = J2DScrnBlockHeader
    u16 mWidth;  // _08
    u16 mHeight; // _0A
    u32 mColor;  // _0C
};

// Size: 0x100
struct J2DPane
{
    J2DPane();
    J2DPane(J2DPane *parent, bool isVisible, u64 tag, const JGeometry::TBox2f &box);
    // inline J2DPane(J2DPane* parent, bool isVisible, u64 tag, f32 x0, f32 y0, f32 x1, f32 y1)
    //     : J2DPane(parent, isVisible, tag, JGeometry::TBox2f(x0, y0, x1, y1)) {};
    J2DPane(u64 tag, const JGeometry::TBox2f &box);
    J2DPane(J2DPane *parent, JSURandomInputStream *input, u8 version);

    virtual ~J2DPane();                                     // _08
    virtual u16 getTypeID() const { return J2DPaneType_Pane; } // _0C (weak)
    virtual void move(f32 x, f32 y);                        // _10
    virtual void add(f32 x, f32 y);                         // _14
    virtual void resize(f32, f32);                          // _18
    virtual void setCullBack(bool shouldCullBack)
    {
        GXCullMode cullMode = GX_CULL_NONE;
        if (shouldCullBack) {
            cullMode = GX_CULL_BACK;
        }
        setCullBack((GXCullMode)cullMode);
    }                                                   // _1C (weak)
    virtual void setCullBack(GXCullMode cullMode);      // _20
    virtual void setAlpha(u8 alpha) { mAlpha = alpha; } // _24 (weak)
    virtual bool setConnectParent(bool connectParent)
    {
        mIsConnected = 0;
        return false;
    } // _28 (weak)
    virtual void calcMtx()
    {
        if (mTree.getList()) {
            makeMatrix(mOffset.x, mOffset.y);
        }
    }                                                                                             // _2C (weak)
    virtual void update() { }                                                                     // _30 (weak)
    virtual void drawSelf(f32, f32) { }                                                           // _34 (weak)
    virtual void drawSelf(f32, f32, Mtx*) { }                                                     // _38 (weak)
    virtual J2DPane* search(u64);                                                                 // _3C
    virtual J2DPane* searchUserInfo(u64);                                                         // _40
    virtual void makeMatrix(f32 f1, f32 f2) { makeMatrix(f1, f2, -mBounds.i.x, -mBounds.i.y); }   // _44 (weak)
    virtual void makeMatrix(f32, f32, f32, f32);                                                  // _48
    virtual bool isUsed(const ResTIMG* resource);                                                 // _4C
    virtual bool isUsed(const ResFONT* resource);                                                 // _50
    virtual void clearAnmTransform();                                                             // _54
    virtual void rewriteAlpha() { }                                                               // _58 (weak)
    virtual void setAnimation(J2DAnmBase* animation);                                             // _5C
    virtual void setAnimation(J2DAnmTransform* animation);                                        // _60
    virtual void setAnimation(J2DAnmColor* animation) { }                                         // _64 (weak)
    virtual void setAnimation(J2DAnmTexPattern* animation) { }                                    // _68 (weak)
    virtual void setAnimation(J2DAnmTextureSRTKey* animation) { }                                 // _6C (weak)
    virtual void setAnimation(J2DAnmTevRegKey* animation) { }                                     // _70 (weak)
    virtual void setAnimation(J2DAnmVisibilityFull* animation) { }                                // _74 (weak)
    virtual void setAnimation(J2DAnmVtxColor* animation) { }                                      // _78 (weak)
    virtual const J2DAnmTransform* animationTransform(const J2DAnmTransform* animation);          // _7C
    virtual void setVisibileAnimation(J2DAnmVisibilityFull* animation);                           // _80
    virtual void setAnimationVF(J2DAnmVisibilityFull* animationVF) { setAnimation(animationVF); } // _84 (weak)
    virtual void setVtxColorAnimation(J2DAnmVtxColor* animation);                                 // _88
    virtual void setAnimationVC(J2DAnmVtxColor* animationVC) { setAnimation(animationVC); }       // _8C (weak)
    virtual const J2DAnmTransform* animationPane(const J2DAnmTransform* animation);               // _90
    void animationTransform();
    void changeUseTrans(J2DPane *);
    void clip(const JGeometry::TBox2f &);
    void draw(f32, f32, const J2DGrafContext *, bool, bool);
    void gather(J2DPane **gatheredPanes, u64 minID, u64 maxID, int gatheredLimit, int &gatheredCount);

    JGeometry::TBox2f *getBounds();
    JGeometry::TVec3f getGlbVtx(u8) const;
    void *getPointer(JSURandomInputStream *, u32, JKRArchive *);
    u64 getTagName() const;

    /**
     * @reifiedAddress{80309D98}
     * @reifiedFile{plugProjectOgawaU/ogCallBackMessage.cpp}
     */
    u64 getUserInfo() const { return mMessageID; }

    J2DPane *getFirstChildPane();
    J2DPane *getNextChildPane();
    J2DPane *getParentPane();
    JSUTree<J2DPane> *getPaneTree() { return &mTree; }
    JSUTree<J2DPane> *getFirstChild() { return mTree.getFirstChild(); }
    JSUTree<J2DPane> *getEndChild() { return mTree.getEndChild(); }

    void initiate();
    void initialize(J2DPane *parent, bool isVisible, u64 tag, const JGeometry::TBox2f &box);
    void initialize(u64, const JGeometry::TBox2f &);
    void makePaneStream(J2DPane *parent, JSURandomInputStream *input);
    void makePaneExStream(J2DPane *parent, JSURandomInputStream *input);
    void place(const JGeometry::TBox2f &);
    void rotate(f32, f32, J2DRotateAxis, f32);
    void rotate(f32);
    void updateTransform(const J2DAnmTransform *);

    /**
     * @reifiedAddress{80309A74}
     * @reifiedFile{plugProjectOgawaU/ogCallBackMessage.cpp}
     */
    bool isVisible() const { return mIsVisible; }

    /**
     * @reifiedAddress{803CA760}
     * @reifiedFile{plugProjectEbisawaU/ebi2DGraph.cpp}
     */
    inline void show() { mIsVisible = true; }

    /**
     * @reifiedAddress{803CA910}
     * @reifiedFile{plugProjectEbisawaU/ebi2DGraph.cpp}
     */
    inline void hide() { mIsVisible = false; };

    void setBasePosition(J2DBasePosition);
    void setInfluencedAlpha(bool, bool);

    /**
     * @fabricated
     */
    void centerWithScale(f32 width, f32 height)
    {
        setBasePosition(J2DPOS_Center);
        mScale.x = width;
        mScale.y = height;
        calcMtx();
    }

    inline void updateScale(f32 scale)
    {
        mScale = JGeometry::TVec2f(scale, scale);
        calcMtx();
    }

    inline void updateScale(f32 scaleX, f32 scaleY) // called scale in TP
    {
        mScale = JGeometry::TVec2f(scaleX, scaleY);
        calcMtx();
    }

    inline void setAlphaFromFloat(f32 alpha)
    {
        if (alpha >= 0.0f)
        {
            alpha = 0.5f + alpha;
        }
        else
        {
            alpha = alpha - 0.5f;
        }

        setAlpha((int)alpha);
    }

    inline void resetAngle()
    {
        mAngleZ = 0.0f;
        calcMtx();
    }

    inline void setOffset(f32 x1, f32 x2, f32 y1, f32 y2)
    {
        mOffset = JGeometry::TVec2f(x1 + x2, y1 + y2);
        calcMtx();
    }

    inline void setOffset(f32 x, f32 y) // called translate in TP
    {
        mOffset = JGeometry::TVec2f(x, y);
        calcMtx();
    }

    inline void addOffsetY(f32 y)
    {
        mOffset.y += y;
        calcMtx();
    }

    inline void setAngle(f32 a)
    {
        mAngleZ = a;
        calcMtx();
    }

    inline void setMsgID(u64 msgID) { mMessageID = msgID; } // called setUserInfo in TP

    const Mtx *getMtx() const { return &mPositionMtx; }

    u8 getAlpha() const { return mAlpha; }
    f32 getScaleX() const { return mScale.x; }
    f32 getScaleY() const { return mScale.y; }
    f32 getTranslateX() const { return mOffset.x; }
    f32 getTranslateY() const { return mOffset.y; }
    int getKind() const { return mBloBlockType; }
    f32 getRotateZ() const { return mAngleZ; }
    f32 getRotOffsetX() const { return mAnchorPoint.x; }
    f32 getRotOffsetY() const { return mAnchorPoint.y; }
    Mtx *getGlobalMtx() { return &mGlobalMtx; }
    f32 getHeight() const { return mBounds.getHeight(); }
    f32 getWidth() const { return mBounds.getWidth(); }
    JGeometry::TBox2f& getGlobalBounds() { return mGlobalBounds; }

    bool isConnectParent() const { return mIsConnected; }

    void setMtx(Mtx mtx) { PSMTXCopy(mtx, mPositionMtx); }

    inline void removeFromParent()
    {
        if (getParentPane())
        {
            getParentPane()->removeChild(this);
        }
    }

    bool appendChild(J2DPane *child);
    bool prependChild(J2DPane *child);
    bool removeChild(J2DPane *child);

    static s16 J2DCast_F32_to_S16(f32, u8);

    // Unused/inlined:
    bool insertChild(J2DPane *before, J2DPane *child);
    f32 getRotate() const;
    void gatherUserInfo(J2DPane **, u64, u64, int, int &);

    static JGeometry::TBox2f static_mBounds;

    // _00 VTBL
    u16 mAnimPaneIndex;              // _004
    u32 mBloBlockType;               // _008
    u64 mTag;                        // _010
    u64 mMessageID;                  // _018
    JGeometry::TBox2f mBounds;       // _020
    JGeometry::TBox2f mGlobalBounds; // _030
    JGeometry::TBox2f mClipRect;     // _040
    Mtx mPositionMtx;                // _050
    Mtx mGlobalMtx;                  // _080
    bool mIsVisible;                 // _0B0
    u8 mCullMode;                    // _0B1
    u8 mAlpha;                       // _0B2
    u8 mColorAlpha;                  // _0B3
    bool mIsInfluencedAlpha;         // _0B4
    bool mIsConnected;               // _0B5
    u8 mRotationAxis;                // _0B6
    u8 mBasePosition;                // _0B7
    f32 mAngleX;                     // _0B8
    f32 mAngleY;                     // _0BC
    f32 mAngleZ;                     // _0C0
    JGeometry::TVec2f mAnchorPoint;  // _0C4
    JGeometry::TVec2f mScale;        // _0CC
    JGeometry::TVec2f mOffset;       // _0D4
    JSUTree<J2DPane> mTree;          // _0DC
    J2DAnmTransform *mTransform;     // _0F8
    u8 _0FC[4];                      // _0FC
};

// Size: 0x138
struct J2DTextBox : public J2DPane
{
    J2DTextBox();
    J2DTextBox(J2DPane *parent, JSURandomInputStream *input, JKRArchive *archive);
    J2DTextBox(J2DPane *parent, JSURandomInputStream *input, u32 flags, J2DMaterial *materials);
    J2DTextBox(u64, const JGeometry::TBox2f &, const ResFONT *, const char *, s16, J2DTextBoxHBinding, J2DTextBoxVBinding);
    J2DTextBox(u64 tag, const JGeometry::TBox2f &box);                                                                   // unused/inlined
    J2DTextBox(const ResFONT *, const char *, s16);                                                                      // unused/inlined
    J2DTextBox(const char *, const char *, s16);                                                                         // unused/inlined
    J2DTextBox(J2DPane *parent, JSURandomInputStream *input);                                                            // unused/inlined
    J2DTextBox(u64, const JGeometry::TBox2f &, const char *, const char *, s16, J2DTextBoxHBinding, J2DTextBoxVBinding); // unused/inlined

    virtual ~J2DTextBox();                                        // _08
    virtual u16 getTypeID() const { return J2DPaneType_TextBox; } // _0C (weak)
    virtual void draw(f32, f32);                                  // _94
    virtual void draw(f32, f32, f32, J2DTextBoxHBinding);         // _98    
    virtual void setFont(JUTFont *font);                          // _9C
    virtual JUTFont *getFont() const { return mFont; }            // _A0 (weak)
    virtual bool setBlack(JUtility::TColor black)                 // _A4 (weak)
    {
        mBlack = black;
        return true;
    }
    virtual bool setWhite(JUtility::TColor white) // _A8 (weak)
    {
        mWhite = white;
        return true;
    }

    virtual bool setBlackWhite(JUtility::TColor black, JUtility::TColor white) // _AC (weak)
    {
        mBlack = black;
        mWhite = white;
        return true;
    }
    virtual JUtility::TColor getBlack() const { return mBlack; }                       // _B0 (weak)
    virtual JUtility::TColor getWhite() const { return mWhite; }                       // _B4 (weak)
    virtual J2DMaterial *getMaterial() const { return nullptr; }                       // _B8 (weak)
    
    virtual void resize(f32, f32);                                // _18
    virtual bool setConnectParent(bool);                          // _28
    virtual void drawSelf(f32, f32);                              // _34
    virtual void drawSelf(f32, f32, Mtx*);                        // _38
    virtual bool isUsed(const ResFONT *resource);                 // _50
    
    
    virtual bool isUsed(const ResTIMG *resource) { return J2DPane::isUsed(resource); } // _4C (weak)
    virtual void rewriteAlpha() {}                                                     // _58 (weak)
    void initiate(const ResFONT *, const char *, s16, J2DTextBoxHBinding, J2DTextBoxVBinding);
    void private_readStream(J2DPane *parent, JSURandomInputStream *input, JKRArchive *archive);
    size_t setString(const char *, ...);

    struct TFontSize {
        f32 x, y;
    };

    void getFontSize(TFontSize &fontSize) {
        fontSize.x = mFontSize.x;
        fontSize.y = mFontSize.y;
    }

    void setFontColor(JUtility::TColor a, JUtility::TColor b)
    {
        mCharColor = a;
        mGradientColor = b;
    }

    // unused/inlined:
    void setFontSize();
    void setLineSpace();
    void getString(char *);
    char *getStringPtr() const;
    size_t setString(s16, const char *, ...);

    /** @fabricated */
    inline void copyString(const char *str, s16 maxLength)
    {
        size_t length = strlen(str);
        u16 finalLength = maxLength;
        if (maxLength == -1)
        {
            if (length >= 0xFFFF)
            {
                length = 0xFFFE;
            }
            finalLength = length + 1;
        }
        mStringPtr = new char[finalLength];
        if (finalLength != 0 && mStringPtr != nullptr)
        {
            strncpy(mStringPtr, str, finalLength - 1);
            mStringPtr[finalLength - 1] = '\0';
            mStringLength = finalLength;
        }
    }

    inline void setFontSize(f32 sizeX, f32 sizeY)
    {
        f32 x;
        if (sizeX > 0.0f)
        {
            x = sizeX;
        }
        else
        {
            x = 0.0f;
        }
        mFontSize.x = x;

        f32 y;
        if (sizeY > 0.0f)
        {
            y = sizeY;
        }
        else
        {
            y = 0.0f;
        }
        mFontSize.y = y;
    }

    inline void setCharSpace(f32 space) { mCharSpacing = space; }
    inline f32 getCharSpace() const { return mCharSpacing; }
    inline void setLineSpace(f32 space) { mLineSpacing = space; }
    inline f32 getLineSpace() const { return mLineSpacing; }

    inline J2DTextBoxVBinding getVBinding() const { return (J2DTextBoxVBinding)(mFlags & 3); }

    inline J2DTextBoxHBinding getHBinding() const { return (J2DTextBoxHBinding)((mFlags >> 2) & 3); }

    // _000      = VTBL
    // _000-_100 = J2DPane
    JUTFont *mFont;               // _100
    JUtility::TColor mCharColor;     // _104
    JUtility::TColor mGradientColor; // _108
    f32 mOffsetX;                    // _10C
    f32 mOffsetY;                    // _110
    f32 mCharSpacing;                // _114
    f32 mLineSpacing;                // _118
    TFontSize mFontSize;             // _11C
    char *mStringPtr;                // _124
    JUtility::TColor mWhite;         // _128
    JUtility::TColor mBlack;         // _12C
    u8 mFlags;                       // _130
    bool mIsTextFontOwned;           // _131
    s16 mStringLength;               // _132
    u8 _134[4];                      // _134
};

// Size: 0x148
struct J2DTextBoxEx : public J2DTextBox
{
    enum stage_enum
    {
        STAGE_0,
        STAGE_1,
        STAGE_2
    };

    J2DTextBoxEx(J2DPane *parent, JSURandomInputStream *input, u32 flags, J2DMaterial *materials);

    virtual ~J2DTextBoxEx();                                                        // _08
   
    virtual J2DMaterial *getMaterial() const { return mMaterial; }                  // _B8 (weak)
    virtual void setCullBack(bool shouldCullBack) { setCullBack(shouldCullBack ? GX_CULL_BACK : GX_CULL_NONE); } // _1C (weak)
    virtual void setCullBack(GXCullMode cullMode);                                  // _20
    virtual void setAlpha(u8 alpha);                                                // _24
    virtual void drawSelf(f32, f32, Mtx *);                                         // _38
    virtual bool isUsed(const ResTIMG *resource) { return J2DPane::isUsed(resource); } // _4C (weak)
    virtual bool isUsed(const ResFONT *resource);                                   // _50
    virtual void rewriteAlpha();                                                    // _58
    virtual void setAnimation(J2DAnmBase *animation) { J2DPane::setAnimation(animation); } // _5C (weak)
    virtual void setAnimation(J2DAnmTransform *animation) { J2DPane::setAnimation(animation); } // _60 (weak)
    virtual void setAnimation(J2DAnmColor *animation);                              // _64
    virtual void setAnimation(J2DAnmTexPattern *animation);                         // _68
    virtual void setAnimation(J2DAnmTextureSRTKey *animation);                      // _6C
    virtual void setAnimation(J2DAnmTevRegKey *animation);                          // _70
    virtual void setAnimation(J2DAnmVisibilityFull *animation);                     // _74
    virtual void setAnimation(J2DAnmVtxColor *animation) { J2DPane::setAnimation(animation); } // _78 (weak)
    virtual const J2DAnmTransform *animationPane(const J2DAnmTransform *animation); // _90
    virtual void draw(f32, f32);                                                    // _94
    virtual void draw(f32, f32, f32, J2DTextBoxHBinding);                           // _98
    virtual void setFont(JUTFont *font);                                            // _9C
    virtual JUTFont *getFont() const;                                            // _A0
    virtual bool setBlack(JUtility::TColor black);                                  // _A4
    virtual bool setWhite(JUtility::TColor white);                                  // _A8
    virtual bool setBlackWhite(JUtility::TColor black, JUtility::TColor white);     // _AC
    virtual JUtility::TColor getBlack() const;                                      // _B0
    virtual JUtility::TColor getWhite() const;                                      // _B4
    

    void setTevOrder(bool);
    void setTevStage(bool);
    void setStage(J2DTevStage *, stage_enum);
    bool getBlackWhite(JUtility::TColor *, JUtility::TColor *) const;
    bool isSetBlackWhite(JUtility::TColor, JUtility::TColor) const;

    // _000      = VTBL
    // _000-_138 = J2DTextBox
    J2DMaterial* mMaterial;               // _138
    u16 mAnimVisibilityIndex;             // _13C
    u16 mMaterialID;                      // _13E
    u8 _140;                              // _140
    J2DAnmVisibilityFull* mAnmVisibility; // _144
};

// Size: 0x40
struct J2DWindowData {
    u8 _00[0x10];          // _00, unknown
    u16 mContentIds[4];    // _10
    u8 _18;                // _18
    u8 _19;                // _19
    u16 mMinX;             // _1A
    u16 mMinY;             // _1C
    u16 mOffsetX;          // _1E
    u16 mOffsetY;          // _20
    u16 _22;               // _22
    u16 mParentId;         // _24
    u16 _26;               // _26
    u16 _28[4];            // _28
    u32 mContentColors[4]; // _30
};

// Size: 0x148
struct J2DWindow : public J2DPane
{
    struct TMaterial {
        J2DMaterial* _00; // _00
        J2DMaterial* _04; // _04
        J2DMaterial* _08; // _08
        J2DMaterial* _0C; // _0C
        J2DMaterial* _10; // _10
    };

    struct TContentsColor {
        TContentsColor() { }

        JUtility::TColor mColorA; // _00
        JUtility::TColor mColorB; // _04
        JUtility::TColor mColorC; // _08
        JUtility::TColor mColorD; // _0C
    };

    J2DWindow();
    J2DWindow(J2DPane *parent, JSURandomInputStream *input, JKRArchive *archive);
    J2DWindow(J2DPane *parent, JSURandomInputStream *input, J2DMaterial *materials);

    virtual ~J2DWindow();                                                              // _08
    virtual u16 getTypeID() const { return J2DPaneType_Window; }                       // _0C (weak)
    virtual void resize(f32, f32);                                                     // _18
    virtual void drawSelf(f32, f32);                                                   // _34
    virtual void drawSelf(f32, f32, f32 (*)[3][4]);                                    // _38
    virtual bool isUsed(const ResTIMG *resource);                                      // _4C
    virtual void draw(const JGeometry::TBox2f &);                                      // _94
    virtual void draw(const JGeometry::TBox2f &, const JGeometry::TBox2f &);           // _98
    virtual void draw(f32 l, f32 t, f32 w, f32 h) { draw(JGeometry::TBox2f(l, t, l + w, t + h)); }                                             // _9C (weak)
    virtual bool setBlack(JUtility::TColor black)                                      // _A0 (weak)
    {
        mBlack = black;
        return true;
    }
    virtual bool setWhite(JUtility::TColor white) // _A4 (weak)
    {
        mWhite = white;
        return true;
    }
    virtual bool setBlackWhite(JUtility::TColor black, JUtility::TColor white) // _A8 (weak)
    {
        mBlack = black;
        mWhite = white;
        return true;
    }
    virtual JUtility::TColor getBlack() const { return mBlack; }                       // _AC (weak)
    virtual JUtility::TColor getWhite() const { return mWhite; }                       // _B0 (weak)
    virtual JUTTexture *getFrameTexture(u8 frameMaterialIndex, u8 textureIndex) const; // _B4
    virtual JUTTexture *getContentsTexture(u8 textureIndex) const                      // _B8 (weak)
    {
        return (textureIndex != 0) ? nullptr : mContentsTexture;
    }
    virtual void getMaterial(TMaterial &material) const // _BC (weak)
    {
        material._00 = nullptr;
        material._04 = nullptr;
        material._08 = nullptr;
        material._0C = nullptr;
        material._10 = nullptr;
    }
    virtual J2DMaterial *getFrameMaterial(u8 index) const { return nullptr; } // _C0 (weak)
    virtual J2DMaterial *getContentsMaterial() const { return nullptr; }      // _C4 (weak)
    virtual void drawContents(const JGeometry::TBox2f &);                     // _C8

    virtual void rewriteAlpha() {}                                                 // _58 (weak)
    virtual bool isUsed(const ResFONT *resource) { return J2DPane::isUsed(resource); } // _50 (weak), currently gets placed at the wrong spot
    void private_readStream(J2DPane *, JSURandomInputStream *, JKRArchive *);
    void initinfo2();
    void draw_private(const JGeometry::TBox2f &, const JGeometry::TBox2f &);
    void setContentsColor(JUtility::TColor, JUtility::TColor, JUtility::TColor, JUtility::TColor);
    void drawFrameTexture(JUTTexture *, f32, f32, f32, f32, u16, u16, u16, u16, bool);
    void drawFrameTexture(JUTTexture *, f32, f32, bool, bool, bool);
    void drawContentsTexture(f32, f32, f32, f32);
    void setTevMode(JUTTexture *, JUtility::TColor, JUtility::TColor);

    void setContentsColor(JUTColor color) {
        setContentsColor(color,color,color,color);
    }

    void getContentsColor(TContentsColor &color) {
        color.mColorA = mContentsColorA;
        color.mColorB = mContentsColorB;
        color.mColorC = mContentsColorC;
        color.mColorD = mContentsColorD;
    }

     // _000      = VTBL
    // _000-_100 = J2DPane
    JUTTexture* mFrameTextureA;       // _100
    JUTTexture* mFrameTextureB;       // _104
    JUTTexture* mFrameTextureC;       // _108
    JUTTexture* mFrameTextureD;       // _10C
    JUTTexture* mContentsTexture;     // _110
    JGeometry::TBox2f mWindowArea;    // _114
    JUTPalette* mPalette;             // _124
    JUtility::TColor mContentsColorA; // _128
    JUtility::TColor mContentsColorB; // _12C
    JUtility::TColor mContentsColorC; // _130
    JUtility::TColor mContentsColorD; // _134
    JUtility::TColor mWhite;          // _138
    JUtility::TColor mBlack;          // _13C
    s16 mMinWidth;                    // _140
    s16 mMinHeight;                   // _142
    u8 mWrapFlags;                    // _144
    u8 _145;                          // _145
};

// Size: 0x180
struct J2DWindowEx : public J2DWindow {
    enum stage_enum {
        STAGE_Unk0 = 0,
        STAGE_Unk1,
        STAGE_Unk2,
        STAGE_Unk3,
        STAGE_Unk4,
    };

    J2DWindowEx(J2DPane* parent, JSURandomInputStream* input, u32 flags, J2DMaterial* materials);

    virtual ~J2DWindowEx();                                                                                        // _08
    
    virtual void getMaterial(J2DWindow::TMaterial& material) const                                                 // _BC (weak)
    {
        material._00 = mFrameMaterials[0];
        material._04 = mFrameMaterials[1];
        material._08 = mFrameMaterials[2];
        material._0C = mFrameMaterials[3];
        material._10 = mContentsMaterial;
    }
    
    virtual J2DMaterial* getFrameMaterial(u8 index) const { return (index >= 4) ? nullptr : mFrameMaterials[index]; } // _C0 (weak)
    virtual J2DMaterial* getContentsMaterial() const { return mContentsMaterial; }                                    // _C4 (weak)
    virtual void drawContents(const JGeometry::TBox2f&);                                                              // _C8
    virtual void draw(f32 l, f32 t, f32 x, f32 y) { draw(JGeometry::TBox2f(JGeometry::TVec2f(l, t), JGeometry::TVec2f(l + x, t + y))); } // _9C (weak)
    virtual void setCullBack(bool shouldCullBack) { setCullBack((shouldCullBack) ? GX_CULL_BACK : GX_CULL_NONE); } // _1C (weak)
    virtual bool isUsed(const ResFONT* resource) { return J2DPane::isUsed(resource); }                             // _50 (weak)
    virtual void setAnimation(J2DAnmBase* animation) { J2DPane::setAnimation(animation); }                         // _5C (weak)
    virtual void setAnimation(J2DAnmTransform* animation) { J2DPane::setAnimation(animation); }                    // _60 (weak)

    virtual void setCullBack(GXCullMode cullMode);                                                                 // _20
    virtual void setAlpha(u8);                                                                                     // _24
    virtual void drawSelf(f32, f32, Mtx*);                                                                         // _38
    virtual bool isUsed(const ResTIMG* resource);                                                                  // _4C
    virtual void rewriteAlpha();                                                                                   // _58
    virtual void setAnimation(J2DAnmColor* animation);                                                             // _64
    virtual void setAnimation(J2DAnmTexPattern* animation);                                                        // _68
    virtual void setAnimation(J2DAnmTextureSRTKey* animation);                                                     // _6C
    virtual void setAnimation(J2DAnmTevRegKey* animation);                                                         // _70
    virtual void setAnimation(J2DAnmVisibilityFull* animation);                                                    // _74
    virtual void setAnimation(J2DAnmVtxColor* animation);                                                          // _78
    virtual const J2DAnmTransform* animationPane(const J2DAnmTransform*);                                          // _90
    virtual void draw(const JGeometry::TBox2f&);                                                                   // _94
    virtual void draw(const JGeometry::TBox2f&, const JGeometry::TBox2f&);                                         // _98
    virtual bool setBlack(JUtility::TColor black);                                                                 // _A0
    virtual bool setWhite(JUtility::TColor white);                                                                 // _A4
    virtual bool setBlackWhite(JUtility::TColor black, JUtility::TColor white);                                    // _A8
    virtual JUtility::TColor getBlack() const;                                                                     // _AC
    virtual JUtility::TColor getWhite() const;                                                                     // _B0
    virtual JUTTexture* getFrameTexture(u8 frameMaterialIndex, u8 textureIndex) const;                             // _B4
    virtual JUTTexture* getContentsTexture(u8 textureIndex) const;                                                 // _B8

    void setMinSize();
    void draw_private(const JGeometry::TBox2f&, const JGeometry::TBox2f&);
    void drawFrameTexture(f32, f32, f32, f32, u16, u16, u16, u16, J2DMaterial*, bool);
    void setTevStage(bool);
    void setStage(J2DTevStage*, J2DWindowEx::stage_enum);
    bool getBlackWhite(JUtility::TColor*, JUtility::TColor*) const;
    bool isSetBlackWhite(JUtility::TColor, JUtility::TColor) const;
    bool isNeedSetAnm(u8 frameMaterialIndex);

    // unused/inlined:
    void initialize(u32, const ResTIMG**, const JGeometry::TBox2f*);
    void setTevOrder(bool);

    // _000      = VTBL
    // _000-_148 = J2DWindow
    J2DMaterial* mFrameMaterials[4];      // _148
    u16 _158[2];                          // _158
    u8 _15C[4];                           // _15C
    J2DMaterial* mContentsMaterial;       // _160
    u16 mMaterialID;                      // _164
    u16 _166;                             // _166
    u16 _168[4];                          // _168
    u8 mMaterialFlags;                    // _170
    J2DAnmVisibilityFull* mAnmVisibility; // _174
    J2DAnmVtxColor* mAnmVtxColor;         // _178
    u8 _17C;                              // _17C
};

#endif
