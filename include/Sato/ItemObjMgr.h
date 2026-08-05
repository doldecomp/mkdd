#ifndef ITEMOBJMGR_H
#define ITEMOBJMGR_H

#include "Kaneshige/SysDebug.h"
#include "Kaneshige/LightMgr.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/ItemSuccession.h"
#include "Sato/ItemHeart.h"
#include "types.h"

class ItemShuffleMgr
{
public:
    struct SlotTable // fabricated struct, unknown if used currently
    {
        u8 chance[27];
        u8 total; // i don't know wheter or not this is used but 1 byte is not always enough to store this
    };

    class KartSlotList
    {
    public:
        KartSlotList()
        {
            slotTable[0] = nullptr;
            slotTable[1] = nullptr;
        }
        u8 *slotTable[2];
    };

    struct KartSlotData
    {
        u8 kartCount;        // 0
        u8 useableSlots;     // 1
        u8 totalSlots;       // 2
        KartSlotList *mList; // 4
    };

    struct KartSlotRankDataSet
    {
        const KartSlotData *slotData; // 0
        int specialItemIndex;     // 4
        u32 specialItemChance;    // 8
        int kart_rank;            // C
        int total;                // 10
        int kart_index;           // 14
    };
    static KartSlotData mSlotList;
    static KartSlotData mSlotListEnemy;

    static int SLOT_MAX_COUNT;
    static int SLOT_INVALID_BUTTON_COUNT;
    static int SLOT_WAIT_COUNT;

    static u8 sSlotNormalItemNum;  // 9
    static u8 sSlotSpecialItemNum; // 9
    static int *sSlotKindIndexArray;

    ItemShuffleMgr()
    {
        reset();
    }

    void reset()
    {
        mSlotCount = 0;
        mRollFlags = 0;
    }

    void startShuffle()
    {
        mRollFlags |= 1;
        _7 = false;
    }

    static const char *getSlotList(int slotType)
    {
        const char *ret = "/Item/ItemSlotList.dat";
        switch (slotType)
        {
        case 2:
            ret = "/Item/ItemSlotList1.dat";
            break;
        case 3:
            ret = "/Item/ItemSlotList2.dat";
            break;
        }
        return ret;
    }

    static u32 getRndItemKindPossibilityGetting(stRandom *, int, u8);
    static void loadSlotData();
    static u32 calcBufOffset(u8 *pData, int n);
    static void setToList(u8 *, ItemShuffleMgr::KartSlotData *);
    static void setToListLAN(ItemShuffleMgr::KartSlotData *, const u8 &, const int *);
    static void setAllDynamicRate();
    static void setDynamicRate(ItemShuffleMgr::KartSlotData *, bool);
    int SlotItem(int, u8);
    bool doShuffle();
    void resetRolling();

    int calcRndSpecialSlot(int, const ItemShuffleMgr::KartSlotData &, bool);
    int calcRndSpecialRank(ItemShuffleMgr::KartSlotRankDataSet);

    virtual ~ItemShuffleMgr();                                                     // 08
    virtual void calcRaceUseNormalItem(u32 *, KartSlotRankDataSet *, int);         // 0C
    virtual void calcSpecialItemNum(u32 *, KartSlotRankDataSet *, int, int, bool); // 10
    virtual int calcRank(KartSlotRankDataSet);                                     // 14

    int calcSlot(KartSlotRankDataSet &slotRankData, int p2, int p3, bool p4);

    // Inline/Unused
    void calcRndSpecialRatio(u32 *, ItemShuffleMgr::KartSlotRankDataSet *, bool);
    void setKartNumber(int no) { mKartNumber = no; }
    void set_7(bool enable) { _7 = enable; }
    bool isNotRolling() const { return !(mRollFlags & 1) && !(mRollFlags & 2); }

private:
    u16 mSlotCount;  // 4
    u8 mRollFlags;   // 6
    bool _7;         //
    int mKartNumber; // 8
};

class ItemRndSpecialShuffleMgr : public ItemShuffleMgr
{
public:
    ItemRndSpecialShuffleMgr() {}

    virtual ~ItemRndSpecialShuffleMgr() {}
    virtual void calcRaceUseNormalItem(u32 *, KartSlotRankDataSet *, int);
    virtual void calcSpecialItemNum(u32 *, KartSlotRankDataSet *, int, int, bool);
    virtual int calcRank(KartSlotRankDataSet);
};

class ItemObjMgr : public SysDbUsrPage
{
public:
    ItemObjMgr();
    virtual ~ItemObjMgr();
    virtual void draw();
    virtual void callbackColGeoObj() {}

    enum eDrawSimplModelItemType
    {
        ItemType_0,
        ItemType_1,
        ItemType_2
    };

    struct SLAN_DebugKartPadData
    {
        void reset()
        {
            mButtons.clear();
            mMainStick.clear();
        }

        JUTGamePad::CButton mButtons;  // _00
        JUTGamePad::CStick mMainStick; // _30
    };
    struct SLAN_DebugKartPadDataArray
    {
        SLAN_DebugKartPadDataArray() {}

        SLAN_DebugKartPadData padData[16];
    };
    struct SLAN_DebugItemData
    {
        void init(ItemObj *obj) // fabricated
        {
            _0 = obj->getKind();
            _4 = (obj->mPos);
            _10 = (obj->mVel);
            m.set(obj->mModel.getBaseTRMtx());
            _4c = obj->getState();
            _50 = obj->_24c;
        }

        void reset() // fabricated
        {
            _0 = 22;
            _4.zero();
            _10.zero();
            PSMTXIdentity(m);
            _4c = 0;
            _50 = 0;
        }

        u32 _0;                //
        JGeometry::TVec3f _4;  //
        JGeometry::TVec3f _10; //
        JGeometry::TSMtxf m;   // 1c
        u32 _4c;               //
        u8 _50;                //
    };
    struct SLAN_DebugItemDataArray
    {
        SLAN_DebugItemDataArray() {}

        SLAN_DebugItemData itemData[0x40];
    };

    struct StockItem
    {
        void clear()
        {
            mKind = 16;
            _4 = false;
            mColorID = 0;
        }

        int mKind;   // 0
        bool _4;     // 4
        u8 mColorID; // 5
    };

    struct KartHitList
    {
        ItemObj *mObjects[17]; // 0
    }; // Size: 0x44

    void initItemMaxNums();                                                        // 0x8023d24c
    void createShuffleMgr();                                                       // 0x8023d29c
    void reset();                                                                  // 0x8023d518
    void resetLists();                                                             // 0x8023d55c
    void clrKartHittingList();                                                     // 0x8023d734
    void clrKartItemEquipTriggerList();                                            // 0x8023d7b0
    void clrKartItemUseTriggerList();                                              // 0x8023d808
    void clrKartStockingEquipItemList();                                           // 0x8023d860
    void createAllObj();                                                           // 0x8023d908
    ItemObj *equipItemSuccession(u32, int, u8);                                    // 0x8023dfa0
    bool robRivalOfItem(int, int, u8);                                             // 0x8023e140
    bool transferPartnerOfItem(int, bool);                                         // 0x8023e28c
    bool stockItemToKart(int, int, u8, bool, u8);                                  // 0x8023e39c
    bool equipItemToKart(int, int, u8, bool, u8);                                  // 0x8023e3d0
    ItemObj *occurItem(u32, const JGeometry::TVec3f &, const JGeometry::TVec3f &); // 0x8023e5c4
    void delete_stockItemToKart(int, u8);                                          // 0x8023e658
    void removeItem(ItemObj *);                                                    // 0x8023e680
    void removeItem(JSUListIterator<ItemObj> &);                                   // 0x8023e938
    void deleteItemByMoveList(ItemObj *);                                          // 0x8023ea14
    void deleteItemByKartEquipItemList(ItemObj *);                                 // 0x8023ea70
    void deleteItemByKartEquipItemListSuc(ItemObj *);                              // 0x8023eabc
    void deleteItemKeepMax(ItemObj *);                                             // 0x8023eadc
    void deleteMaxOverItem(const JSUList<ItemObj> &);                              // 0x8023ebd8
    ItemObj *getDeleteObjByPriorityList(JSUList<ItemObj> *);                       // 0x8023ec48
    void releaseItemByKartEquipItemList(ItemObj *);                                // 0x8023ed54
    ItemObj *getKartEquipItem(int, u8);                                            // 0x8023ed84
    ItemObj *getKartEquipItem(int);                                                // 0x8023ed9c
    int getStockItem(int, u8);                                                     // 0x8023ede4
    bool tstColReactionKart(int, ItemObj *);                                       // 0x8023ee1c
    void loadOtherModelData();                                                     // 0x8023ee48
    const char *getOtherBmdFileName(u32);                                          // 0x8023eef4
    void load();                                                                   // 0x8023ef14
    void createModel(JKRSolidHeap *, u32);                                         // 0x8023f1dc
    void initCharShadowModelData();                                                // 0x8023f39c
    void createCharShadowModel(JKRSolidHeap *, u32);                               // 0x8023f540
    void calcCharShadowModel();                                                    // 0x8023f5f8
    void calc();                                                                   // 0x8023fc00
    void calcIsHitItem(JSUListIterator<ItemObj> &, JSUListIterator<ItemObj> &);    // 0x8024048c
    void setKartHittingList(JSUListIterator<ItemObj> &, int);                      // 0x802405d8
    void checkItemByShuffle();                                                     // 0x8024087c
    void abortItemShuffle(int);                                                    // 0x80240c00
    static ItemObj *getItemWanWanObj(ItemObj *);                                   // 0x80240c3c
    static ItemObj *getItemFireBall(ItemObj *);                                    // 0x80240c58
    static ItemObj *getItemFireBallSuc(ItemObj *);                                 // 0x80240c74
    static ItemObj *getItemYoshiEgg(ItemObj *);                                    // 0x80240c90
    void setHeartItem(int, ItemHeart *);                                           // 0x80240cac
    void deleteHeartItem(int);                                                     // 0x80240d48
    bool tstHaveItemHeart(int);                                                    // 0x80240e28
    void executeColHitHeart(ItemObj *, int);                                       // 0x80240edc
    ItemHeart *getHeartItem(int);                                                  // 0x80240f80
    void setItemUseTrigger(int, u8);                                               // 0x80241018
    int tstItemUseTrigger(int, u8) const;                                          // 0x80241100
    void setThunderDisableUseCounter();                                            // 0x802411e4
    static u8 getNowTandemDriverNum(int);                                          // 0x8024123c, does this return u8 or int?
    static bool IsSpecialCharacter(long);                                          // 0x80241294
    bool IsRollingSlot(int);                                                       // 0x802412b8
    bool IsRollingSlot(int, u8);                                                   // 0x8024131c
    bool IsAvailableRollingSlot(int, u32);                                         // 0x80241360
    void startItemShuffleSingle(const int, const bool);                                        // 0x80241534
    void startItemShuffleDouble(const int);                                              // 0x80241608
    ItemObj::ItemHandOffsetData &getHandOffsetData(ItemObj *);                     // 0x802416d0
    int getRobberyItemNum(int, u8);                                                // 0x80241928
    void removeMiniGameList(ItemObj *);                                            // 0x80241970
    void update(eDrawSimplModelItemType, int);                                     // 0x80241998
    void viewCalc(u32);                                                            // 0x80241adc
    void setCurrentViewNo(u32);                                                    // 0x80241bf0
    void drawSimpleModel(u32, eDrawSimplModelItemType, int, LightObj *);           // 0x80241cf4
    void drawColModel(u32);                                                        // 0x80241f64
    static void setLANDebugInfo();                                                 // 0x80241f68

    ItemObj *equipItem2(int kind, int kart_index, u8 driver_index) { 
        if (GetGeoObjMgr()->isBombBattle())
        {
            ItemObj *obj = equipItemSuccession(kind, kart_index, driver_index);
            removeMiniGameList(obj);
            return obj;
        }
        if (kind > 0x10)
        {
            return equipItemSuccession(kind, kart_index, driver_index);
        }
        return equipItem(kind);
    }

    // Inline/Unused
    ItemObj *equipItem(u32);
    void appendItemSuccession(ItemObj *, u32, u8);
    void createItemByKartStockingItemList();
    bool isEnableDeleteItem(ItemObj *);
    bool IsItemAvailable(int);
    bool tstKartEquipItemTrigger(int, u8);
    int getStockItem(int);
    void loadModelData(ItemObj *);
    void loadShadowModelData();
    void changeItemHoldCharacter();
    bool IsAbleToCreateItem(int);
    int getNowStockingKindNum(int, u32);
    void cancelRollingSlot(int);
    bool IsAvailableRollingSlotDriver(int, u8);
    static int getMaxHoldMiniBombNum();
    void getNowEnableSlotDriver(u8 *, int);
    static u32 getConvertSucItemKind(u32);
    void getMaxItemMoveNum();
    void setMaxItemMoveNum(const u8 &);
    void entryMiniGameList(ItemObj *);
    void tstEnableDrawGhost(bool, const JSUListIterator<ItemObj> &);

    static void createMgr() { gItemMgr = new ItemObjMgr(); }
    static ItemObjMgr *getItemObjMgr() { return gItemMgr; }

    static int sTopBalloonNum;
    static SLAN_DebugItemDataArray *sLANDbgDataPtr;
    static SLAN_DebugKartPadDataArray *sLANDbgKartPadBuffer;
    static ItemObjMgr *gItemMgr;

    void swapOwner(ItemObj *obj, bool fromRival, int kart_index1, int kart_index2, u8 driver_index1, u8 driver_index2)
    {
        mEquipItem[kart_index1][driver_index1] = obj; // here
        obj->setOrigOwnerNum(kart_index2);
        obj->setOrigDriverNum(driver_index2);
        obj->setOwnerNum(kart_index1);
        obj->setDriverNum(driver_index1);
        if (!fromRival)
            obj->mTransferFlags |= 0x200;
        obj->setStateDivested(true);
        delete_stockItemToKart(kart_index2, driver_index2);
        mEquipItem[kart_index2][driver_index2] = nullptr;
    }

    static u8 swapDriver(u8 driver_index) { // fabricated
        if (driver_index == 0)
            return 1;
        return 0;
    }

    bool isThunderDisabled() const { return mThunderDisableTimer != 0; }

    // private:
    u8 _28[0x2c - 0x28];                           //
    KartHitList mHitList[8];                       // 2c
    StockItem mStockItem[8][2];                    // 24c
    ItemObj *mEquipItem[8][2];                     // 2cc, items that are being held
    bool mIsHitting[8][2];                         // 30c
    bool mIsEquiped[8][2];                         // 31c
    bool mItemUse[8][2];                           // 32c
    ItemHeart *mHeartItem[8];                      // 33c
    ItemShuffleMgr *mShuffMgr[8][2];               // 35c
    J3DModelData *mModel[16];                      // 39c
    J3DModelData *mOtherModel[1];                  // 3dc
    J3DModelData *mShadowModel[2];                 // 3e0
    JSUList<ItemObj> _3e8[22];                     //
    JSUList<ItemObj> _4f0[22];                     //
    JSUList<ItemObj> _5f8[22];                     //
    JSUList<ItemObj> _700[4];                      //
    JSUList<ItemObj> mMiniGameList;                // 730
    JSUList<ItemObj> _73c[16];                     //
    JSUList<ItemObj> _7fc;                         //
    bool mObjectsCreated;                          // 808
    u32 mThunderDisableTimer;                      // 80c
    ItemObj::ItemHandOffsetData *mpHandOffsetData; // 810
    J3DModelData *mpMdlData;                       // 814
    ShadowModel *mpShadowMdl[2];                   // 818, 81c
    bool _820;                                     //
    u32 _824;                                      //
};

inline ItemObjMgr *GetItemObjMgr()
{
    return ItemObjMgr::getItemObjMgr();
}

inline void CreateItemObjMgr() { ItemObjMgr::createMgr(); }

#endif // ITEMOBJMGR_H
