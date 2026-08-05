#include "Osako/LANSelectMode.h"
#include "Inagaki/GameAudioMain.h"
#include "JSystem/J2D/J2DAnimation.h"
#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/J2D/J2DPane.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/ResTIMG.h"
#include "Osako/Kart2DCommon.h"
#include "Osako/LANPlayInfo.h"
#include "Osako/MenuTitleLine.h"
#include "Osako/NetGameMgr.h"
#include "Osako/kartPad.h"
#include "Osako/system.h"
#include "mathHelper.h"
#include "types.h"
#include <utility>

static J2DPicture *spaColorPicture[3];

void LANSelectMode::Background::setup(JKRArchive *archive) {
    mScreen.setPriority("Back2.blo", 0x20000, archive);
    void *bck = JKRGetNameResource("Back2.bck", archive);
    void *btk = JKRGetNameResource("Back2.btk", archive);
    mTrans = (J2DAnmTransform*)J2DAnmLoaderDataBase::load(bck);
    mTexKey = (J2DAnmTextureSRTKey*)J2DAnmLoaderDataBase::load(btk);
    mScreen.setAnimation(mTrans);
    mScreen.setAnimation(mTexKey);
}

void LANSelectMode::Background::calc() {
    mTrans->setFrame(mTransFrame);
    mTexKey->setFrame(mKeyFrame);

    if (++mTransFrame >= 178) {
        mTransFrame = 0;
    }

    if (++mKeyFrame >= 1400) {
        mKeyFrame = 0;
    }
    mScreen.animation();
}

LANSelectMode::Item::Item(u32 tagPrefix, J2DScreen *screen, void *bck) {
    // Size mismatch, should be 0x148 (currently 0x144)
    // however appears to match
    mState = 0;
    mTrans = (J2DAnmTransform*)J2DAnmLoaderDataBase::load(bck);
    
    const u64 tag = (u64)tagPrefix << 16ull;
    mBasePane = screen->search(0x5F43 + tag);
    J2DPane *pPane = screen->search(0x5F4D + tag);
#line 96
    JUT_ASSERT(pPane->getTypeID() == J2DPaneType_Picture);
    mBasePicture = (J2DPicture *)pPane;
    mBasePane->setAnimation(mTrans);
    mBasePicture->setAnimation(mTrans);
    mTransFrame = 20;
}

void LANSelectMode::Item::changeColor(LANSelectMode::Color color) {
    mBasePicture->setWhite(spaColorPicture[color]->getWhite());
    mBasePicture->setBlack(spaColorPicture[color]->getBlack());
}

// perhaps in and out should be swapped, who knows
void LANSelectMode::Item::selectIn() {
    mState = 3;
    changeColor(cLanColor_1);
}

void LANSelectMode::Item::selectOut() {
    mState = 1;
    changeColor(cLanColor_2);
}

void LANSelectMode::Item::unselect() {
    mTransFrame = 20;
    mState = 0;
    changeColor(cLanColor_1);
}

void LANSelectMode::Item::calc() {
    mTrans->setFrame(mTransFrame);
    switch (mState) {
    case 1: {
        if (++mTransFrame >= 29) {
            mTransFrame = 29;
            mState = 2;
        }
        break;
    }
    case 3: {
        if (--mTransFrame < 20) {
            mTransFrame = 20;
            mState = 0;
        }
        break;
    }
    }
}

LANSelectMode::SelectableItem::SelectableItem(u32 tagPrefix, J2DScreen *pScreen, void *bck, void *btk, void *brk, int max, int startIdx, u32 mask) : Item(tagPrefix, pScreen, bck) {
    mIdx = startIdx;
    mMax = max;
    mMask = mask;
    _24 = 0;
    mTrans1 = (J2DAnmTransform*)J2DAnmLoaderDataBase::load(bck);
    mTrans2 = (J2DAnmTransform*)J2DAnmLoaderDataBase::load(bck);
    
    mTexKey = (J2DAnmTextureSRTKey*)J2DAnmLoaderDataBase::load(btk);
    mTexKey->searchUpdateMaterialID(pScreen);
    
    mKey1 = (J2DAnmTevRegKey*)J2DAnmLoaderDataBase::load(brk);
    mKey1->searchUpdateMaterialID(pScreen);

    mKey2 = (J2DAnmTevRegKey*)J2DAnmLoaderDataBase::load(brk);
    mKey2->searchUpdateMaterialID(pScreen);
    mTransFrame1 = 90;
    mTransFrame2 = 40;
    mTexFrame = 0;
    mKeyFrame1 = 0;
    mKeyFrame2 = 0;

    J2DPane *pPane;
    
    pPane = pScreen->search(tagPrefix | 0x4e5f000000ull);
    pPane->setAnimation(mTrans);
    
    const u64 tag = (u64)tagPrefix << 24;
    pPane = pScreen->search(0x744c30 + tag);
    pPane->setAnimation(mTrans2);

    pPane = pScreen->search(0x745230 + tag);
    pPane->setAnimation(mTrans2);

    mLeftPane = pScreen->search(0x744c31 + tag);
    mLeftPane->setAnimation(mKey1);

    mRightPane = pScreen->search(0x745231 + tag);
    mRightPane->setAnimation(mKey1);

    pPane = pScreen->search(0x744c32 + tag);
    pPane->setAnimation(mTexKey);

    pPane = pScreen->search(0x745232 + tag);
    pPane->setAnimation(mTexKey);
}

bool LANSelectMode::SelectableItem::changeValueUp() {
    int origValue = mIdx;
    do {
        if (++mIdx >= mMax) {
            mIdx = 0;
        }
    } while (mMask & (1 << mIdx));

    if (mIdx != origValue) {
        _24 = 1;
        mLeftPane->setAnimation((J2DAnmTransform*)nullptr);
        mLeftPane->setAnimation(mKey1);
        mRightPane->setAnimation(mTrans1);
        mRightPane->setAnimation(mKey2);
        mTransFrame1 = 90;
        mKeyFrame2 = 2;
        return true;
    }
    return false;
}

bool LANSelectMode::SelectableItem::changeValueDown() {
    int origValue = mIdx;
    do {
        if (--mIdx < 0) {
            mIdx = mMax - 1;
        }
    } while (mMask & (1 << mIdx));

    if (mIdx != origValue) {
        _24 = 1;
        mLeftPane->setAnimation(mTrans1);
        mLeftPane->setAnimation(mKey2);
        mRightPane->setAnimation((J2DAnmTransform*)nullptr);
        mRightPane->setAnimation(mKey1);
        mTransFrame1 = 90;
        mKeyFrame2 = 2;
        return true;
    }
    return false;
}

void LANSelectMode::SelectableItem::setMask(u32 mask) {
    mMask = mask;
    if (mMask & 1 << mIdx) {
        changeValueDown();
    }
}

void LANSelectMode::SelectableItem::setMax(int max) {
    mMax = max;
    if (mIdx >= mMax) {
        changeValueDown();
    }
}

void LANSelectMode::SelectableItem::update() {
    _24 = 1;
    mTransFrame1 = 93;
    calc();
    mTransFrame1 = 90;
    _24 = 0;
}

void LANSelectMode::SelectableItem::calc() {
    Item::calc();
    mTrans1->setFrame(mTransFrame1);
    
    switch (_24) {
    case 1: {
        if (++mTransFrame1 >= 99) {
            mTransFrame1 = 98;
        }
        if (mKeyFrame2 > 1 && ++mKeyFrame2 >= 0x14) {
            mKeyFrame2 = 1;
            _24 = 0;
        } 
        break;
    }
    
    }
    mTrans2->setFrame(mTransFrame2);
    mTexKey->setFrame(mTexFrame);

    switch (mState) {
    case 1: {
        mKeyFrame1 = 1;
        mKeyFrame2 = 1;
        break;
    }
    case 2: {
        if (++mTransFrame2 >= 80) {
            mTransFrame2 = 40;
        }
        if (++mTexFrame >= 120) {
            mTexFrame = 0;
        }
        mKeyFrame1 = 1;
        break;
    }
    case 0: {
        mTransFrame2 = 40;
        mKeyFrame1 = 0;
        mTexFrame = 0;
        mKeyFrame2 = 0;
        break;
    }
    }

    mKey1->setFrame(mKeyFrame1);
    mKey2->setFrame(mKeyFrame2);
}

void LANSelectMode::SelectableItem1::changeColor(LANSelectMode::Color color) {
    Item::changeColor(color);
    mPicture->setWhite(spaColorPicture[color]->getWhite());
    mPicture->setBlack(spaColorPicture[color]->getBlack());
}

LANSelectMode::SelectableItem1::SelectableItem1(u32 tagPrefix, J2DScreen *screen, void *bck, void *btk, void *brk, ResTIMG **paImg, int max) : SelectableItem(tagPrefix, screen, bck, btk, brk, max, 0, 0) {
    mTimg = paImg;
    J2DPane *pPane = screen->search(0x7377ull + ((u64)tagPrefix << 16));
#line 406
    JUT_ASSERT(pPane->getTypeID() == J2DPaneType_Picture);
    mPicture = (J2DPicture*)pPane;
    mPicture->setAnimation(mTrans1);
    mPicture->changeTexture(mTimg[mIdx], 0);
}

void LANSelectMode::SelectableItem1::calc() {
    SelectableItem::calc();
    if (_24 == 1 && mTransFrame1 == 94) {
        mPicture->changeTexture(mTimg[mIdx], 0);
    }
}

LANSelectMode::SelectableItem2::SelectableItem2(u32 tagPrefix, J2DScreen *screen, void *bck, void *btk, void *brk) : SelectableItem(tagPrefix, screen, bck, btk, brk, 10, 0, 0) {
    J2DPane *pPane;
    pPane = screen->search(0x737730ull + ((u64)tagPrefix << 24));

#line 457
    JUT_ASSERT(pPane->getTypeID() == J2DPaneType_Picture);
    _54 = (J2DPicture*)pPane;
    _54->setAnimation(mTrans1);
    
    pPane = screen->search(0x737731ull + ((u64)tagPrefix << 24));
    JUT_ASSERT(pPane->getTypeID() == J2DPaneType_Picture);
    _50 = (J2DPicture*)pPane;
    _50->setAnimation(mTrans1);
    _50->hide();
}

void LANSelectMode::SelectableItem2::changeColor(LANSelectMode::Color color) {
    Item::changeColor(color);
    _54->setWhite(spaColorPicture[color]->getWhite());
    _54->setBlack(spaColorPicture[color]->getBlack());
    _50->setWhite(spaColorPicture[color]->getWhite());
    _50->setBlack(spaColorPicture[color]->getBlack());
}

void LANSelectMode::SelectableItem2::calc() {
    SelectableItem::calc();

    if (_24 == 1 && mTransFrame1 == 94) {
        if (mIdx != 0) {
            _50->changeTexture(Kart2DCommon::ptr()->getNumberTexture((Kart2DCommon::NumberTextureID)mIdx), 0);
            _50->show();
            _54->hide();
        }
        else {
            _54->show();
            _50->hide();
        }
    }
}

LANSelectMode::SelectableItemSw::SelectableItemSw(u32 tagPrefix, J2DScreen *screen, void *bck, void *btk, void *brk, ResTIMG **p6, ResTIMG **imgSw, int maxSw, ResTIMG **paImg, int max) : SelectableItem1(tagPrefix, screen, bck, btk, brk, paImg, max) {
    // perhaps swap p6 and p7 with p9 and p10
    mValueMode = cValueMode_0;
    mIdxSw = 0;
    mMaxSw = maxSw;
    mMaskSw = 0;
    mValueTimg = p6;
    mTimgSw = imgSw;
}

void LANSelectMode::SelectableItemSw::changeValueMode(LANSelectMode::SelectableItemSw::ValueMode mode) {
    if (mValueMode == mode) return;

    std::swap(mIdx, mIdxSw);
    std::swap(mMax, mMaxSw);
    std::swap(mMask, mMaskSw);
    std::swap(mTimg, mTimgSw);

    mPicture->changeTexture(mTimg[mIdx], 0);
    mBasePicture->changeTexture(mValueTimg[mode], 0);

    mValueMode = mode;
}

void LANSelectMode::SelectableItemSw::setMaskSw(u32 mask) {
    mMaskSw = mask;
    while (mMaskSw & 1 << mIdxSw) {
        if (--mIdxSw < 0) {
            mIdxSw = mMaxSw - 1;
        }
    }
}

LANSelectMode::SelectableItemDisp::SelectableItemDisp(u32 tagPrefix, J2DScreen *screen, void *bck, void *btk, void *brk) : SelectableItem(tagPrefix, screen, bck, btk, brk, 3, 0, 0) {
    J2DPane *pPane;
    pPane = screen->search(0x737730ull + ((u64)tagPrefix << 24));
    pPane->setAnimation(mTrans1);

#line 571
    pPane = screen->search(0x737731ull + ((u64)tagPrefix << 24));
    JUT_ASSERT(pPane->getTypeID() == J2DPaneType_Picture);

    mImgSw = (J2DPicture*)pPane;
    mImgSw->setAnimation(mTexKey);

    mKeyDisp = (J2DAnmTevRegKey*)J2DAnmLoaderDataBase::load(brk);
    mKeyDisp->searchUpdateMaterialID(screen);

    _60 = 0;

#line 585
    pPane = screen->search(0x6c31ull + ((u64)tagPrefix << 16));
    JUT_ASSERT(pPane->getTypeID() == J2DPaneType_Picture);

    mImgL1 = (J2DPicture*)pPane;
    mImgL1->setAnimation(mKeyDisp);
    mImgL1->hide();

#line 592
    pPane = screen->search(0x6c32ull + ((u64)tagPrefix << 16));
    JUT_ASSERT(pPane->getTypeID() == J2DPaneType_Picture);

    mImgL2 = (J2DPicture*)pPane;
    mImgL2->setAnimation(mKeyDisp);
    mImgL2->hide();
    
    pPane = screen->search(0x737730ull + ((u64)tagPrefix << 24));
    pPane->setAnimation(mTexKey);
}

void LANSelectMode::SelectableItemDisp::changeColor(LANSelectMode::Color color) {
    Item::changeColor(color);

    mImgSw->setWhite(spaColorPicture[color]->getWhite());
    mImgSw->setBlack(spaColorPicture[color]->getBlack());

    mImgL1->setWhite(spaColorPicture[color]->getWhite());
    mImgL1->setBlack(spaColorPicture[color]->getBlack());

    mImgL2->setWhite(spaColorPicture[color]->getWhite());
    mImgL2->setBlack(spaColorPicture[color]->getBlack());

    if (color == cLanColor_2) {
        mImgL1->setAnimation(mKeyDisp);
        mImgL2->setAnimation(mKeyDisp);
    }
    else {
        mImgL1->setAnimation((J2DAnmTevRegKey*)nullptr);
        mImgL2->setAnimation((J2DAnmTevRegKey*)nullptr);
    }
}

void LANSelectMode::SelectableItemDisp::calc() {
    SelectableItem::calc();

    if (_24 == 1 && mTransFrame1 == 94) {
        switch (mIdx) {
        case 0: {
            mImgL1->hide();
            mImgL2->hide();
            break;
        }
        case 1: {
            mImgL1->show();
            mImgL2->hide();
            break;
        }
        case 2: {
            mImgL1->show();
            mImgL2->show();
            break;
        }
        }
    }
    
    if (mState == 2) {
        mKeyDisp->setFrame(_60);
        if (++_60 >= 120) {
            _60 = 0;
        }
    }
}

const char *cpaGameModeTex[] = {
    "Entry_Versus",
    "Mozi_Battle1",
    "Mozi_Battle2"
};

const char *cpaCrsOrderTex[] = {
    "Mozi_Random01",
    "Mozi_OneCourse01",
    "Mozi_AllCourse01",
    "CupName_MUSHROOM_CUP",
    "CupName_FLOWER_CUP",
    "CupName_STAR_CUP",
    "CupName_SPECIAL_CUP"
};

const char *cpaStageOrderTex[] = {
    "Mozi_OneStage01",
    "Mozi_AllStage01"
};

const char *cpaLabelNameTex[] = {
    "Course",
    "Stage"
};

const char *cpaTagTex[] = {
    "Off", "On"
};

const char *cpaCrsNameTex[] = {
    "LUIGI_CIRCUIT",
    "PEACH_BEACH",
    "BABY_PARK",
    "KARA_KARA_DESERT",
    "KINOKO_BRIDGE",
    "MARIO_CIRCUIT",
    "DAISY_SHIP",
    "WALUIGI_STADIUM",
    "SHERBET_LAND",
    "KINOKO_CITY",
    "YOSHI_CIRCUIT",
    "DK_MOUNTAIN",
    "WARIO_COLOSSEUM",
    "DINO_DINO_JUNGLE",
    "BOWSER_CASTLE",
    "RAINBOW_ROAD",

};

LANSelectMode::LANSelectMode(JKRArchive *pArchive) : mBackground(pArchive) {
    mState = 1;
    mItemIdx = 0;
    mItemMask = 0;
    mScreen.setPriority("LANSelectMode.blo", 0x20000, pArchive);

    void *bck = JKRGetNameResource("LANSelectMode.bck", pArchive);
    void *btk = JKRGetNameResource("LANSelectMode.btk", pArchive);
    void *brk = JKRGetNameResource("LANSelectMode.brk", pArchive);

    mAnm = J2DAnmLoaderDataBase::load(bck);
    mAnmFrame = 0;

    char buf[0x40];
    for (int  i = 0; i < 3; i++) {
        snprintf(buf, sizeof(buf), "timg/%s.bti", cpaGameModeTex[i]);
        mGameModeImg[i] = (ResTIMG *)pArchive->getResource(buf);
    }

    for (int  i = 0; i < 4; i++) {
        snprintf(buf, sizeof(buf), "timg/Power_Mozi.%d.bti", i + 1);
        mPowerImg[i] = (ResTIMG*)pArchive->getResource(buf);
    }

    for (int  i = 0; i < 2; i++) {
        snprintf(buf, sizeof(buf), "timg/Mozi_Order%s01.bti", cpaLabelNameTex[i]);
        mOrderImg[i] = (ResTIMG*)pArchive->getResource(buf);
    }

    for (int  i = 0; i < 7; i++) {
        snprintf(buf, sizeof(buf), "timg/%s.bti", cpaCrsOrderTex[i]);
        mCrsOrderImg[i] = (ResTIMG*)pArchive->getResource(buf);
    }

    _2d8 = mCrsOrderImg[0];

    snprintf(buf, sizeof(buf), "timg/%s.bti", cpaStageOrderTex[0]);
    mStageOrderImg[0] = (ResTIMG*)pArchive->getResource(buf);

    snprintf(buf, sizeof(buf), "timg/%s.bti", cpaStageOrderTex[1]);
    mStageOrderImg[1] = (ResTIMG*)pArchive->getResource(buf);

    for (int  i = 0; i < 2; i++) {
        snprintf(buf, sizeof(buf), "timg/Mozi_%sSelect01.bti", cpaLabelNameTex[i]);
        mSelectImg[i] = (ResTIMG*)pArchive->getResource(buf);
    }

    for (int i = 0; i < 0x10; i++) {
        snprintf(buf, sizeof(buf), "timg/CoName_%s.bti", cpaCrsNameTex[i]);
        mCrsNameImg[i] = (ResTIMG*)pArchive->getResource(buf);
    }

    for (int i = 0; i < 6; i++) {
        snprintf(buf, sizeof(buf), "timg/Mozi_Map%d.bti", i + 1);
        mMapImg[i] = (ResTIMG*)pArchive->getResource(buf);
    }

    std::swap(mMapImg[5], mMapImg[4]);

    for (int i = 0; i < 2; i++) {
        snprintf(buf, sizeof(buf), "timg/Mozi_%s.bti", cpaTagTex[i]);
        mTagImg[i] = (ResTIMG*)pArchive->getResource(buf);
    }

    for (int i = 0; i < 3; i++) {
#line 820
        J2DPane *pPane = mScreen.search(0x436f6c30ull + i);
        JUT_ASSERT(pPane->getTypeID() == J2DPaneType_Picture);
        spaColorPicture[i] = (J2DPicture*)pPane; 
    }

    mModeItem = new SelectableItem1('GMM', &mScreen, bck, btk, brk, mGameModeImg, 3);
    mPowerItem = new SelectableItem1('POW', &mScreen, bck, btk, brk, mPowerImg, 4);
    mLapItem = new SelectableItem2('LAP', &mScreen, bck, btk, brk);
    mOrderItem = new SelectableItem1('TOU', &mScreen, bck, btk, brk, mCrsOrderImg, 7);
    mCourseSelectItem = new SelectableItemSw('COU', &mScreen, bck, btk, brk, mSelectImg, mMapImg, 6, mCrsNameImg, 16);
    mTagItem = new SelectableItem1('TAG', &mScreen, bck, btk, brk, mTagImg, 2);
    mDisplayItem = new SelectableItemDisp('DIS', &mScreen, bck, btk, brk);

    mItem = new Item('OKK', &mScreen, bck);
    mItems[0] = mModeItem;
    mItems[1] = mPowerItem;
    mItems[2] = mLapItem;
    mItems[3] = mOrderItem;
    mItems[4] = mCourseSelectItem;
    mItems[5] = mTagItem;
    mItems[6] = mDisplayItem;
    mItems[7] = mItem;

    mItems[0]->changeColor(cLanColor_2);
    mItems[1]->changeColor(cLanColor_1);
    mItems[2]->changeColor(cLanColor_1);
    mItems[3]->changeColor(cLanColor_1);
    mItems[4]->changeColor(cLanColor_0);
    mItems[5]->changeColor(cLanColor_1);
    mItems[6]->changeColor(cLanColor_1);
    mItems[7]->changeColor(cLanColor_1);
    gLANPlayInfo.resumeInfo(this);

    NetGameMgr *netMgr = NetGameMgr::ptr();
    if (!(netMgr->get12ec() & 4)) {
        mPowerItem->setMask(8);
    }
    if (!(netMgr->get12ec() & 8)) {
        mOrderItem->setMask(0x40);
    }

    u32 mask1 = 0;
    if (!(netMgr->get12ec() & 0x8)) {
        mask1 |= 0xf000;
    }

    u32 mask2 = 0;
    if (!(netMgr->get12ec() & 0x80)) {
        mask2 |= 0x10;
    }
    if (!(netMgr->get12ec() & 0x100)) {
        mask2 |= 0x20;
    }

    if (mCourseSelectItem->getValueMode() == SelectableItemSw::cValueMode_0) {
        mCourseSelectItem->setMask(mask1);
        mCourseSelectItem->setMaskSw(mask2);
    }
    else {
        mCourseSelectItem->setMask(mask2);
        mCourseSelectItem->setMaskSw(mask1);
    }

    for (int i = 0; i < 7; i++) {
        mItems[i]->update();
        directView(i);
    }

    mItemMask |= 0x10;
    
    mModeItem->selectOut();
}

LANSelectMode::Background::~Background() {}

void LANSelectMode::directView(int i) {
    switch (i) {
    case 0: {
        switch (mModeItem->get_18()) {
        case 0:
        {
            mPowerItem->changeColor(cLanColor_1);
            mItemMask &= ~2;
            mLapItem->changeColor(cLanColor_1);
            mItemMask &= ~4;
            u32 mask = 0;
            if (!(NetGameMgr::ptr()->get12ec() & 8)) {
                mask |= 0x40;
            }
            mOrderItem->changeTex(mOrderImg[0]);
            mOrderItem->changeImg(mCrsOrderImg);



            mOrderItem->setMask(mask);
            mCourseSelectItem->changeValueMode(SelectableItemSw::cValueMode_0);
            break;
        }
        case 1:
        case 2: {
            mPowerItem->changeColor(cLanColor_0);
            mItemMask |= 2;
            mLapItem->changeColor(cLanColor_0);
            mItemMask |= 4;

            mOrderItem->changeTex(mOrderImg[1]);
            mOrderItem->changeImg(&_2d8);

            mOrderItem->setMask(0x78);
            mCourseSelectItem->changeValueMode(SelectableItemSw::cValueMode_1);
            break;
        }
        }
        break;
    }
    case 3: {
        switch (mOrderItem->get_18()) {
        
        case 1: {
            mCourseSelectItem->changeColor(cLanColor_1);
            mItemMask &= ~0x10;
            break;
        }
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6: {
            mCourseSelectItem->changeColor(cLanColor_0);
            mItemMask |= 0x10;
            break;
        }
        default: {
            break;
        }
        }
        break;
    }
    case 5: {
        setDisplayMax();
        break;
    }
    }
}

int LANSelectMode::calc() {
    KartGamePad *pad = gpaKartPad[0];
    int ret = 0;

    switch (mState) {
    case 2: {
        if (pad->testRepeat(KartGamePad::MAINSTICK_UP)) {
            GetGameAudioMain()->startSystemSe(0x20000);
            mItems[mItemIdx]->selectIn();
            do {
                if (--mItemIdx < 0) {
                    mItemIdx = 7;
                }
            } while(mItemMask & 1 << mItemIdx);

            mItems[mItemIdx]->selectOut();
        }
        else if (pad->testRepeat(KartGamePad::MAINSTICK_DOWN)) {
            GetGameAudioMain()->startSystemSe(0x20000);
            mItems[mItemIdx]->selectIn();
            do {
                if (++mItemIdx >= 8) {
                    mItemIdx = 0;
                }
            } while(mItemMask & 1 << mItemIdx);

            mItems[mItemIdx]->selectOut();
        }
        else if (pad->testRepeat(KartGamePad::MAINSTICK_LEFT)) {
            if (mItems[mItemIdx]->changeValueDown()) {
                GetGameAudioMain()->startSystemSe(0x2000c);
                directView(mItemIdx);
            }
        }
        else if (pad->testRepeat(KartGamePad::MAINSTICK_RIGHT)) {
            if (mItems[mItemIdx]->changeValueUp()) {
                directView(mItemIdx);
                GetGameAudioMain()->startSystemSe(0x2000c);
            }
        }

        if (pad->testTrigger(KartGamePad::A)) {
            if (mItemIdx == 7) {
                GetGameAudioMain()->startSystemSe(0x20003);
                mState = 3;
                mAnmFrame = 120;
                _5 = true;
                MenuTitleLine::ptr()->lift();
                SYSTEM_StartFadeOut(15);
                GetGameAudioMain()->fadeOutAll(15);
            }
        }
        else if (pad->testTrigger(KartGamePad::B)) {
            if (_4) {
                GetGameAudioMain()->startSystemSe(0x20004);
                mState = 3;
                mAnmFrame = 120;
                _5 = false;
                MenuTitleLine::ptr()->lift();
                SYSTEM_StartFadeOut(15);
                GetGameAudioMain()->fadeOutAll(15);
            }
        }
        break;
    }
    case 4: {
        if (SYSTEM_IsFadingOut()) {
            if (_5) {
                ret = 1;
                gLANPlayInfo.saveInfo(this);
            }
            else {
                ret = 2;
            }
        }
        break;
    }
    }

    calcAnm();
    MenuTitleLine::ptr()->calc();
    return ret;
}

void LANSelectMode::calcAnm() {
    mBackground.calc();

    for (int i = 0; i < 8; i++) {
        mItems[i]->calc();
    }

    mAnm->setFrame(mAnmFrame);
    switch(mState) {
    case 1: {
        if (++mAnmFrame >= 24) {
            mState = 2;
        }
        break;
    }
    case 3: {
        if (++mAnmFrame >= 131) {
            mState = 4;
        }
        break;
    }
    }

    mScreen.animation();
}

void LANSelectMode::draw(const J2DGrafContext *graf) {
    mBackground.draw(graf);
    mScreen.draw(0.0f, 0.0f, graf);
    MenuTitleLine::ptr()->draw(graf);
}

void LANSelectMode::start(bool p1) {
    _4 = p1;

    if (_4) {
        GetGameAudioMain()->startSequenceBgm(0x1000002);
    }

    if (mItemIdx != 0) {
        mItems[mItemIdx]->unselect();
        mItemIdx = 0;
    }
    mItems[mItemIdx]->selectOut();
    mState = 1;
    mAnmFrame = 0;
    gLANPlayInfo.resumeInfo(this);
    for (int i = 0; i < 7; i++) {
        mItems[i]->update();
        directView(i);
    }

    MenuTitleLine::ptr()->drop(MenuTitleLine::mcSelectMode);
    SYSTEM_StartFadeIn(15);
}

void LANSelectMode::setDisplayMax() {
    if (NetGameMgr::ptr()->getNetworkCubes() > 4) {
        mDisplayItem->setMax(1);
    }
    else if (NetGameMgr::ptr()->getNetworkCubes() > 2 || mTagItem->get_18() == 1) {
        mDisplayItem->setMax(2);
    }
    else {
        mDisplayItem->setMax(3);
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
