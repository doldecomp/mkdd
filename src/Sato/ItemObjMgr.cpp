#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTDbPrint.h"
#include "JSystem/J3D/J3DModelLoader.h"
#include "JSystem/J3D/J3DTransform.h"
#include "Kaneshige/ExModel.h"
#include "Kaneshige/Objects/GeoRabbitMark.h"
#include "Kaneshige/SimpleDrawer.h"
#include "Kaneshige/TexLODControl.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObjMgr.h"
#include "Sato/ItemObjMgr.h"
#include "Sato/ItemBanana.h"
#include "Sato/ItemBomb.h"
#include "Sato/ItemFlyTurtle.h"
#include "Sato/ItemKinoko.h"
#include "Sato/ItemObj.h"
#include "Sato/ItemWanWan.h"
#include "Sato/ItemStar.h"
#include "Sato/ItemThunder.h"
#include "Sato/ItemTurtle.h"
#include "Sato/ItemYoshiEgg.h"
#include "Sato/ObjUtility.h"
#include "Sato/RivalBodyCtrl.h"
#include "Sato/stMath.h"
#include "Yamamoto/kartCamera.h"

#include "Yamamoto/kartCtrl.h"
#include "dolphin/gx/GXEnum.h"
#include "dolphin/mtx.h"
#include "mathHelper.h"

// WIP
int ItemObjMgr::sTopBalloonNum;
ItemObjMgr::SLAN_DebugItemDataArray *ItemObjMgr::sLANDbgDataPtr;
ItemObjMgr::SLAN_DebugKartPadDataArray *ItemObjMgr::sLANDbgKartPadBuffer;
ItemObjMgr *ItemObjMgr::gItemMgr;

namespace
{
    u8 sMaxMoveItemNum[] = {
        6, 2, 6, 11, 3, 6, 1, 1,
        2, 5, 1, 2, 0, 1, 0, 6};

    u8 sMaxItemNum[] = {
        15, 4, 15, 15, 5, 11, 3, 1,
        5, 5, 1, 4, 2, 1, 2, 9};

    int sMaxCreateItemNum[] = {
        sMaxItemNum[0], sMaxItemNum[1], sMaxItemNum[2], sMaxItemNum[3] + 5,
        sMaxItemNum[4], sMaxItemNum[5] + 10, sMaxItemNum[6], sMaxItemNum[7],
        sMaxItemNum[8], sMaxItemNum[9], sMaxItemNum[10], sMaxItemNum[11],
        sMaxItemNum[12], sMaxItemNum[13], sMaxItemNum[14], sMaxItemNum[15]};

    u8 sNormalDrawItemKind[] = {
        7, 8, 3, 4, 6, 10, 12, 11,
        13, 15};

    u32 sSimpleDrawSpeqEnvTexMapID[] = {
        1, 1, 1, 1, 1};

    u8 sMAX_TOTAL_MOVING_ITEM_NUM = 16;
    u8 sSimpleDrawItemKind[5] = {0, 1, 2, 5, 14};
    const u8 sSimpleDrawOtherItemFlg[] = {1};
    f32 sHeartEquipKartColRadius = 150.0f;
    u8 sMaxHoldMiniBombNum = 5;
    f32 sGPEnemyFlyTurtleItemSlotRatio = 1.5f;
    s16 sMaxThunderDisableFrame = 30 * 60;
    s16 sMiniGameMaxThunderDisableFrame = 1;
    s16 sSlotNormalMaxCount = 130;
    s16 sSlotInvalidButtonCount = 50;

    bool sTempSlotUseItem[18];
    int sTempSpecialRatio[9];

    const bool sSimpleDrawItemFlg[] = {
        true, true, true, false, false, true, false, false,
        false, true, false, true, false, false, true, false};
    const u8 sJ3DUpdateItemKind[] = {
        7, 8, 3, 4, 6, 10, 12, 0,
        1, 2, 5, 11, 13, 14, 15};

    s16 sSlotWaitCount;
}

ItemObjMgr::ItemObjMgr()
{
    gItemMgr = this;
    initItemMaxNums();
    createShuffleMgr();
    GetSysDebug()->appendPage(this);
    mObjectsCreated = false;
    mpHandOffsetData = (ItemObj::ItemHandOffsetData *)ObjUtility::getPtrMainArc("/Item/ItemHandOffsetParam.iop");

    for (int i = 0; i < 16; i++)
    {
        mModel[i] = nullptr;
    }
    mOtherModel[0] = nullptr;

    for (u8 i = 0; i < 2; i++)
    {
        mShadowModel[i] = nullptr;
    }

    for (int i = 0; i < 22; i++)
    {
        _3e8[i].initiate();
        _4f0[i].initiate();
        _5f8[i].initiate();
    }

    for (int i = 0; i < 16; i++)
    {
        _73c[i].initiate();
    }

    for (int i = 0; i < 4; i++)
    {
        _700[i].initiate();
    }

    _7fc.initiate();
    mMiniGameList.initiate();
    loadOtherModelData();
    reset();
    load();
    ItemShuffleMgr::loadSlotData();
    _824 = 0;
    _820 = RCMGetManager()->getConsoleNumber() == 1;

    if (_820)
        initCharShadowModelData();

    if (RCMGetManager()->isLANMode())
    {
        JKRHeap *dbgHeap = GetSysDebug()->getDebugHeap();

        if (dbgHeap)
        {
            if (sLANDbgDataPtr == nullptr)
                sLANDbgDataPtr = new (dbgHeap, 0) SLAN_DebugItemDataArray[300];
            if (sLANDbgKartPadBuffer == nullptr)
                sLANDbgKartPadBuffer = new (dbgHeap, 0) SLAN_DebugKartPadDataArray[300];
        }

        if (sLANDbgDataPtr && sLANDbgKartPadBuffer)
        {
            for (u32 i = 0; i < 300; i++)
            {
                for (u32 j = 0; j < 64; j++)
                    sLANDbgDataPtr[i].itemData[j].reset();
                for (u32 j = 0; j < 16; j++)
                    sLANDbgKartPadBuffer[i].padData[j].reset();
            }
        }
    }
}

ItemObjMgr::~ItemObjMgr() {}

void ItemObjMgr::initItemMaxNums()
{
    sMAX_TOTAL_MOVING_ITEM_NUM = 0x10;

    int maxMoveItemCount = 2;
    int maxItemCount = 5;

    if (GetGeoObjMgr()->isBombBattle())
    {
        maxMoveItemCount = 64;
        maxItemCount = 64;
    }

    sMaxMoveItemNum[8] = maxMoveItemCount;
    sMaxCreateItemNum[8] = sMaxItemNum[8] = maxItemCount;
}

void ItemObjMgr::createShuffleMgr()
{
    for (int i = 0; i < RCMGetManager()->getKartNumber(); i++)
    {
        for (u8 j = 0; j < 2; j++)
        {
            if (GetGeoObjMgr()->isBalloonBattle() || GetGeoObjMgr()->isEscapeBattle())
            {
                mShuffMgr[i][j] = new ItemRndSpecialShuffleMgr();
            }
            else
            {
                s32 charID = RCMGetManager()->getKartInfo(i)->getDriverCharID(j);
                mShuffMgr[i][j] = !IsSpecialCharacter(charID) ? new ItemShuffleMgr() : new ItemRndSpecialShuffleMgr();
            }
        }
    }
}

ItemShuffleMgr::~ItemShuffleMgr() {}

void ItemObjMgr::reset()
{
    resetLists();
    createAllObj();
    sTopBalloonNum = 0;
    setThunderDisableUseCounter();
}

void ItemObjMgr::resetLists()
{
    JSUList<ItemObj> *list = _3e8;
    JSUList<ItemObj> *list2 = _5f8;
    for (int i = 0; i < 22; i++, list++, list2++)
    {
        for (JSUListIterator<ItemObj> it(list->getFirst()); it.isAvailable();)
        {
            ShadowModel *shdw = it->mShadow;
            if (shdw)
                shdw->clrVisibleAll();
            removeItem(it);
        }
        list->initiate();

        for (JSUListIterator<ItemObj> it(list2->getFirst()); it.isAvailable();)
        {
            ShadowModel *shdw = it->mShadow;
            if (shdw)
                shdw->clrVisibleAll();
            removeItem(it);
        }
        list2->initiate();
    }

    list = _73c;
    for (int i = 0; i < 16; i++, list++)
    {
        for (JSUListIterator<ItemObj> it(list->getFirst()); it.isAvailable(); it = list->getFirst())
        {
            list->remove(&it->_160);
        }
        list->initiate();
    }

    for (JSUListIterator<ItemObj> it(_7fc.getFirst()); it.isAvailable(); it = _7fc.getFirst())
    {
        _7fc.remove(&it->_170);
    }
    _7fc.initiate();

    list = _700;
    for (int i = 0; i < 4; i++, list++)
    {
        for (JSUListIterator<ItemObj> it(list->getFirst()); it.isAvailable(); it = list->getFirst())
        {
            list->remove(&it->_180);
        }
        list->initiate();
    }

    for (JSUListIterator<ItemObj> it(mMiniGameList.getFirst()); it.isAvailable(); it = mMiniGameList.getFirst())
    {
        mMiniGameList.remove(&it->mMiniGameLink);
    }
    mMiniGameList.initiate();

    clrKartHittingList();
    clrKartStockingEquipItemList();
    clrKartItemEquipTriggerList();
    clrKartItemUseTriggerList();
}

void ItemObjMgr::clrKartHittingList()
{
    KartHitList *hitList = mHitList;
    for (int i = 0; i < RCMGetManager()->getKartNumber(); i++, hitList++)
    {
        for (int j = 0; j < 17; j++)
            hitList->mObjects[j] = nullptr;
        for (u8 j = 0; j < 2; j++)
            mIsHitting[i][j] = false;
    }
}

void ItemObjMgr::clrKartItemEquipTriggerList()
{
    for (int i = 0; i < RCMGetManager()->getKartNumber(); i++)
    {
        for (u8 j = 0; j < 2; j++)
        {
            mIsEquiped[i][j] = false;
        }
    }
}

void ItemObjMgr::clrKartItemUseTriggerList()
{
    for (int i = 0; i < RCMGetManager()->getKartNumber(); i++)
    {
        for (u8 j = 0; j < 2; j++)
        {
            mItemUse[i][j] = false;
        }
    }
}

void ItemObjMgr::clrKartStockingEquipItemList()
{
    StockItem *stockItemList = mStockItem[0];
    ItemObj **equipList = mEquipItem[0];
    ItemHeart **heartList = mHeartItem;
    for (int i = 0; i < RCMGetManager()->getKartNumber(); i++, equipList += 2, stockItemList += 2, heartList++)
    {
        equipList[0] = nullptr;
        equipList[1] = nullptr;
        heartList[0] = nullptr;
        stockItemList[0].clear();
        stockItemList[1].clear();
        for (u8 j = 0; j < 2; j++)
        {
            mShuffMgr[i][j]->reset();
            mShuffMgr[i][j]->setKartNumber(i);
        }
    }
}

void ItemObjMgr::createAllObj()
{
    if (mObjectsCreated)
        return;

    if (RCMGetManager()->isStaffRoll())
    {
        for (u8 i = 0; i < 1; i++)
            removeItem(new ItemBomb(8));
    }
    else if (GetGeoObjMgr()->isBombBattle())
    {
        for (u8 i = 0; i < sMaxCreateItemNum[8]; i++)
            removeItem(new ItemBomb(8));
        for (u8 i = 0; i < 16; i++)
            removeItem(new ItemBombSuc(0x14));
    }
    else
    {
        // Create Normal Items (Why the loop and switch...)
        for (u32 i = 0; i < 0x10; i++)
        {
            int max = sMaxCreateItemNum[i];
            switch (i)
            {
            case 0:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemGTurtle(0));
                break;
            case 1:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemGTurtleBig(1));
                break;
            case 2:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemRTurtle(2));
                break;
            case 3:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemBanana(3));
                break;
            case 4:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemBananaBig(4));
                break;
            case 5:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemKinoko(5));
                break;
            case 6:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemStar(6));
                break;
            case 7:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemWanWanObj(7));
                break;
            case 8:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemBomb(8));
                break;
            case 9:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemFireBall(9));
                break;
            case 10:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemThunder(0xa));
                break;
            case 11:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemYoshiEgg(0xb));
                break;
            case 12:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemGoldenKinoko(0xc));
                break;
            case 13:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemFlyTurtle(0xd));
                break;
            case 14:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemHeart(0xe));
                break;
            case 15:
                for (u8 j = 0; j < max; j++)
                    removeItem(new ItemFakeItemBox(0xf));
                break;
            }
        }
        // Create Succession Items (Why the loop and switch again...)
        for (u32 i = 0x11; i < 0x16; i++)
        {
            switch (i)
            {
            case 17:
                for (u8 j = 0; j < 8; j++)
                    removeItem(new ItemGTurtleSuc(17));
                break;
            case 18:
                for (u8 j = 0; j < 8; j++)
                    removeItem(new ItemKinokoSuc(18));
                break;
            case 19:
                for (u8 j = 0; j < 8; j++)
                    removeItem(new ItemRTurtleSuc(19));
                break;
            case 21:
                for (u8 j = 0; j < 8; j++)
                    removeItem(new ItemFireBallSuc(21));
                break;
            }
        }
    }

    mObjectsCreated = true;
}

ItemObj *ItemObjMgr::equipItem(u32 kind) // UNUSED
{
    JSUListIterator<ItemObj> it(_4f0[kind].getFirst());
    if (it.isAvailable())
    {
        it->reset();
        _3e8[kind].append(&it->_150);
        return it.getObject();
    }
    return nullptr;
}

ItemObj *ItemObjMgr::equipItemSuccession(u32 kind, int kart_index, u8 driver_index)
{
    if (GetGeoObjMgr()->isBombBattle())
    {
        kind = 0x14;
    }

    JSUListIterator<ItemObj> it(_4f0[kind].getFirst());
    if (it.isAvailable())
    {
        ItemObjSuc *sucObj = (ItemObjSuc *)it.getObject();
        sucObj->reset();
        ItemObj::cMultiType multiType = ItemObj::cMultiType_1;
        if (GetGeoObjMgr()->isBombBattle())
        {
            sucObj->_1fc = 1;
        }
        else if (!(it->_12c & 0x40))
        {
            multiType = ItemObj::getCharaMultiType(kart_index, driver_index);
        }
        sucObj->setSucMultiType(multiType);

        for (u8 i = 0; i < it->_1fc; i++)
        {
            ItemObj *equipObj = equipItem(it->mSucKind);
            if (equipObj)
            {
                it->_1a4.append(&equipObj->mSuccessionItemLink);
                equipObj->setMultiType(multiType);
                equipObj->setSuccessionParent(sucObj);
                equipObj->setOwnerNum(kart_index);
                equipObj->setDriverNum((u8)driver_index);
                equipObj->setStateEquip(true, false);
            }
        }
        // this block might be inline
        sucObj->setOwnerNum(kart_index);
        sucObj->setDriverNum((u8)driver_index);
        sucObj->setStateEquip(true, false);
        //
        _3e8[kind].append(&it->_150);
        return it.getObject();
    }

    return nullptr;
}

void ItemObjMgr::appendItemSuccession(ItemObj *obj, u32 kind, u8 driver_index)
{

}

bool ItemObjMgr::robRivalOfItem(int kart_index1, int kart_index2, u8 driver_index)
{
    bool robbed = false;

    u8 tandemDriver = getNowTandemDriverNum(kart_index1);

    if (getKartEquipItem(kart_index1, tandemDriver) == 0 && !IsRollingSlot(kart_index1))
    {
        ItemObj *obj = getKartEquipItem(kart_index2, driver_index);
        if (obj)
        {
            mEquipItem[kart_index1][tandemDriver] = obj;
            obj->setOrigOwnerNum(kart_index2);
            obj->setOrigDriverNum((u8)driver_index);
            obj->setOwnerNum(kart_index1);
            obj->setDriverNum(tandemDriver);
            obj->setStateDivested(true);
            delete_stockItemToKart(kart_index2, driver_index);

            mEquipItem[kart_index2][driver_index] = nullptr;
            robbed = true;
        }
    }
    else
    {
        ItemObj *obj = getKartEquipItem(kart_index2, driver_index);
        if (obj->IsSuccessionItem())
        {
            int n = obj->getSuccessionItemNum() - 1;
            for (int i = 0; i < n; i++)
            {
                obj->setChildStateFall();
            }
        }
        else
        {
            getKartEquipItem(kart_index2, driver_index)->setStateFall(true);
        }
    }

    return robbed;
}

bool ItemObjMgr::transferPartnerOfItem(int kart_index, bool swap)
{
    bool ret = false;

    u8 swappedTandem = getNowTandemDriverNum(kart_index);

    if (swap)
    {
        if (swappedTandem == 0)
        {
            swappedTandem = 1;
        }
        else
        {
            swappedTandem = 0;
        }
    }
    u8 driver_index = 0;
    if (swappedTandem == 0)
    {
        driver_index = 1;
    }

    if (getKartEquipItem(kart_index, (u8)driver_index) == 0 && !IsRollingSlot(kart_index))
    {
        ItemObj *obj = getKartEquipItem(kart_index, (u8)swappedTandem);
        if (obj)
        {
            mEquipItem[kart_index][driver_index] = obj;
            obj->setOrigOwnerNum(kart_index);
            obj->setOrigDriverNum(swappedTandem);
            obj->setOwnerNum(kart_index);
            obj->setDriverNum(driver_index);
            obj->setTransferPartner();
            obj->setStateDivested(true);
            delete_stockItemToKart(kart_index, (u8)swappedTandem);

            mEquipItem[kart_index][swappedTandem] = nullptr;
            ret = true;
        }
    }
    return ret;
}

bool ItemObjMgr::stockItemToKart(int kind, int kart_index, u8 driver_index, bool p4, u8 colorID)
{
    bool ret = false;
    if (mStockItem[kart_index][driver_index].mKind == 0x10)
    {
        mStockItem[kart_index][driver_index].mKind = kind;
        mStockItem[kart_index][driver_index]._4 = p4;
        mStockItem[kart_index][driver_index].mColorID = colorID;
        ret = true;
    }
    return ret;
}

// regswaps, functionally equivalent
bool ItemObjMgr::equipItemToKart(int kind, int kart_index, u8 driver_index, bool doEffect, u8 colorID)
{
    bool ret = false;
    if (mEquipItem[kart_index][driver_index] == nullptr)
    {
        ItemObj *obj = equipItem2(kind, kart_index, driver_index);

        if (obj == nullptr)
        {
            if (doEffect)
            {
                ItemHeart *heart = getHeartItem(kart_index);
#line 1055
                JUT_ASSERT_F(heart, "heartNullCheck 0");
                heart->initMoveEffect(obj);
            }
            return false;
        }
        
        if (!obj->IsSuccessionItem())
        {
            obj->setOwnerNum(kart_index);
            obj->setDriverNum(driver_index);
            if (doEffect)
            {
                obj->setStateHeartWaiting(colorID);
                ItemHeart *heart = getHeartItem(kart_index);
                JUT_ASSERT_F(heart, "heartNullCheck 1");
                heart->initMoveEffect(obj);
            }
            else
            {
                obj->setStateEquip(true, false);
            }
        }
        mIsEquiped[kart_index][driver_index] = true;
        mEquipItem[kart_index][driver_index] = obj;
        ret = true;
    }
    return ret;
}

ItemObj *ItemObjMgr::occurItem(u32 kind, const JGeometry::TVec3f &p2, const JGeometry::TVec3f &p3)
{
    ItemObj *obj = equipItem(kind);
    if (obj)
        obj->setStateOccur(p2, p3);
    return obj;
}

void ItemObjMgr::delete_stockItemToKart(int kart_index, u8 driver_index)
{
    mStockItem[kart_index][driver_index].mKind = 16;
    mStockItem[kart_index][driver_index]._4 = false;
    mStockItem[kart_index][driver_index].mColorID = 0;
}

void ItemObjMgr::removeItem(ItemObj *obj)
{
    for (JSUListIterator<ItemObj> it(obj->_1a4.getFirst()); it.isAvailable(); it = obj->_1a4.getFirst())
    {
        obj->_1a4.remove(&it->mSuccessionItemLink);
        removeItem(it.getObject());
    }
    _4f0[obj->getKind()].append(&obj->_150);
    deleteItemByMoveList(obj);
}

void ItemObjMgr::removeItem(JSUListIterator<ItemObj> &list)
{
    for (JSUListIterator<ItemObj> it(list->_1a4.getFirst()); it.isAvailable(); it = list->_1a4.getFirst())
    {
        list->_1a4.remove(&it->mSuccessionItemLink);
        removeItem(it.getObject());
    }

    JSUListIterator<ItemObj> it(list.mLink->getNext()); // is this legal access?

    _4f0[list->getKind()].append(&list->_150);

    if (!list->IsSuccessionItem())
    {
        _7fc.remove(&list->_170);
        _73c[list->getKind()].remove(&list->_160);
    }
    list = it;
}

void ItemObjMgr::deleteItemByMoveList(ItemObj *obj)
{
    if (!obj->IsSuccessionItem())
    {
        _7fc.remove(&obj->_170);
        _73c[obj->getKind()].remove(&obj->_160);
    }
}

void ItemObjMgr::deleteItemByKartEquipItemList(ItemObj *obj)
{
    if (mEquipItem[obj->getOwnerNum()][obj->getDriverNum()] == obj)
    {
        mEquipItem[obj->getOwnerNum()][obj->getDriverNum()] = nullptr;
        deleteItemKeepMax(obj);
    }
}

void ItemObjMgr::deleteItemByKartEquipItemListSuc(ItemObj *obj)
{
    deleteItemKeepMax(obj);
}

void ItemObjMgr::deleteItemKeepMax(ItemObj *obj)
{
    u32 seqID = obj->getKind();
#line 1243
    JUT_MINMAX_ASSERT(0, seqID, 16);

    if (seqID == 14)
        return;

    _7fc.append(&obj->_170);
    _73c[seqID].append(&obj->_160);

    if (_73c[seqID].getNumLinks() > sMaxMoveItemNum[seqID])
    {
        deleteMaxOverItem(_73c[seqID]);
    }
    else if (_7fc.getNumLinks() > sMAX_TOTAL_MOVING_ITEM_NUM)
    {
        ItemObj *deleteObj = getDeleteObjByPriorityList(&_7fc);
        if (deleteObj)
            deleteObj->setStateForceDisappear();
    }
}

void ItemObjMgr::deleteMaxOverItem(const JSUList<ItemObj> &itemList) {
    for (JSUListIterator<ItemObj> it(itemList.getFirst()); it.isAvailable(); ++it) {
        ItemObj *obj = it.getObject();

        if(obj && obj->isCertainState()) {            
            obj->setStateForceDisappear();
            return;
        }
    }
}

ItemObj *ItemObjMgr::getDeleteObjByPriorityList(JSUList<ItemObj> *pList) {
    for (int i = 0; i < 4; i ++) {
        for (JSUListIterator<ItemObj> it(_700[i].getFirst()); it.isAvailable(); it = _700[i].getFirst()) {
            _700[i].remove(&it->_180);
        }
    }

    for (JSUListIterator<ItemObj> it(pList->getFirst()); it.isAvailable(); ++it) {
        _700[it->_1ec].append(&it->_180);
    }

    for (int i = 0; i < 3; i ++) {
        for (JSUListIterator<ItemObj> it(_700[i].getFirst()); it.isAvailable(); ++it) {
            ItemObj *obj = it.getObject();

            if(obj && obj->isCertainState()) {            
                return obj;
            }
        }
    }
    return nullptr;
}

void ItemObjMgr::releaseItemByKartEquipItemList(ItemObj *obj)
{
    if (mEquipItem[obj->getOwnerNum()][obj->getDriverNum()] == obj)
        mEquipItem[obj->getOwnerNum()][obj->getDriverNum()] = nullptr;
}

bool ItemObjMgr::IsItemAvailable(int) { 
    // UNUSED
    // SIZE: 0x48
}

ItemObj *ItemObjMgr::getKartEquipItem(int kart_index, u8 driver_index)
{
    return mEquipItem[kart_index][driver_index];
}

ItemObj *ItemObjMgr::getKartEquipItem(int kart_index)
{
    return getKartEquipItem(kart_index, getNowTandemDriverNum(kart_index));
}

bool ItemObjMgr::tstKartEquipItemTrigger(int kart_index, u8 driver_index)
{
    // UNUSED
    // SIZE: 0x18
}

int ItemObjMgr::getStockItem(int kart_index, u8 driver_index)
{
    int ret = mStockItem[kart_index][driver_index].mKind;
    ItemObj *equipItem = mEquipItem[kart_index][driver_index];
    if (equipItem != nullptr)
    {
        ret = equipItem->getKind();
    }
    return ret;
}

int ItemObjMgr::getStockItem(int kart_index)
{
    return getStockItem(kart_index, getNowTandemDriverNum(kart_index));
}

bool ItemObjMgr::tstColReactionKart(int kart_index, ItemObj *obj)
{
    KartHitList *hitList = &mHitList[kart_index];
    return hitList->mObjects[obj->getKartReaction()] != nullptr;
}

void ItemObjMgr::loadModelData(ItemObj *obj)
{
    u32 kind = obj->getKind();
    if (mModel[kind] == nullptr)
    {
        const void *bmd = ResMgr::getPtr(ResMgr::mcArcMRAM, obj->getBmdFileName());
        mModel[kind] = J3DModelLoaderDataBase::load(bmd, 0x19220010 | obj->getModelDataAppendLoadFlg());
        ExModel::patchModelData(mModel[kind]);

        if (obj->getModelDataAppendLoadFlg() == 0x20)
        {
            J3DModelData *model = mModel[kind];
            for (u16 i = 0; i < model->getShapeNum(); i++)
            {
                J3DShape *shp = model->getShapeNodePointer(i);
                shp->setTexMtxLoadType(0x2000);
            }
        }
        if (obj->getKind() != 0xf)
        {
            ExModel::setLODBias(mModel[kind], TexLODControl::getGeographyLODBias());
        }
        RCMGetCourse()->setFogInfo(mModel[kind]);

        if (sSimpleDrawItemFlg[kind])
        {
            ExModel::setLightMask(mModel[kind], GX_LIGHT1);
        }
        mModel[kind]->newSharedDisplayList(0x40000);
        if ((sSimpleDrawItemFlg)[kind])
        {
            mModel[kind]->simpleCalcMaterial(j3dDefaultMtx);
            mModel[kind]->makeSharedDL();
        }
    }
}

void ItemObjMgr::loadShadowModelData()
{
    for (u8 i = 0; i < 2; i++)
    {
        if (mShadowModel[i] == nullptr)
        {
            const void *shadowBmd = ResMgr::getPtr(ResMgr::mcArcMRAM, ItemObj::getShadowBmdFileName((u8)i));
            mShadowModel[i] = J3DModelLoaderDataBase::load(shadowBmd, 0x18210010);
            ExModel::patchModelData(mShadowModel[i]);
            mShadowModel[i]->newSharedDisplayList(0x40000);
        }
    }
}

void ItemObjMgr::loadOtherModelData()
{
    for (int i = 0; i < 1; i++)
    {
        const void *bmd = ResMgr::getPtr(ResMgr::mcArcMRAM, getOtherBmdFileName(0));
        mOtherModel[i] = J3DModelLoaderDataBase::load(bmd, 0x19220010);
        ExModel::patchModelData(mOtherModel[i]);
        ExModel::setLODBias(mOtherModel[i], TexLODControl::getGeographyLODBias());
        RCMGetCourse()->setFogInfo(mOtherModel[i]);
        mOtherModel[i]->newSharedDisplayList(0x40000);

        if (sSimpleDrawOtherItemFlg[i])
        {
            mOtherModel[i]->simpleCalcMaterial(j3dDefaultMtx);
            mOtherModel[i]->makeSharedDL();
        }
    }
}

const char *ItemObjMgr::getOtherBmdFileName(u32 id)
{
    switch (id)
    {
    case 0:
        return "/Item/wanwan_chn.bmd";
    default:
        return nullptr;
    }
}

void ItemObjMgr::load()
{
    loadShadowModelData();

    for (u8 i = 0; i < 0x10; i++)
    {
        JSUList<ItemObj> *curObjList = _4f0 + i;

        if (curObjList->getFirst())
        {
            ItemObj *obj = curObjList->getFirst()->getObject();
            if (obj)
            {
                loadModelData(obj);

                for (JSUListIterator<ItemObj> it(curObjList->getFirst()); it != curObjList->getEnd(); ++it)
                {
                    it->loadmodel(mModel[it->getKind()]);
                    J3DModelData *shadowModel = mShadowModel[0];
                    if (it->getKind() == 0xf)
                    {
                        shadowModel = mShadowModel[1];
                    }
                    if (it->getKind() != 9)
                    {
                        it->loadshadowmodel(shadowModel);
                    }
                }
            }
        }
    }

    if (!GetGeoObjMgr()->isBombBattle() && !RCMGetManager()->isStaffRoll())
    {
        ItemBanana::loadAnmData(mModel[3]);
        ItemWanWanObj::loadAnmData(mModel[7]);
        ItemFlyTurtle::loadAnmData(mModel[13]);
        ItemGoldenKinoko::loadAnmData(mModel[12]);
        ItemBananaBig::loadAnmData(mModel[4]);
        ItemYoshiEgg::loadAnmData(mModel[11]);
        ItemFakeItemBox::loadAnmData(mModel[15]);
    }
    ItemBomb::loadAnmData(mModel[8]);
}

void ItemObjMgr::createModel(JKRSolidHeap *heap, u32 camera_index)
{
    for (u8 i = 0; i < 16; i++)
    {
        JSUListIterator<ItemObj> it(_4f0[i].getFirst());
        if (it.isAvailable())
        {
            for (; it.isAvailable(); ++it)
            {
                it->createModel(heap, camera_index, 2);
                if (it->getKind() != 9)
                    it->createShadowModel(heap, camera_index, 0x60000);
            }
        }
    }

    for (u8 i = 0; i < 16; i++)
    {
        JSUListIterator<ItemObj> it(_4f0[i].getFirst());
        if (it.isAvailable() && it->mModel.getModelData())
        {
            Mtx m;
            PSMTXIdentity(m);
            PSMTXCopy(m, j3dSys.getViewMtx());
            it->mModel.makeSharedDL();
        }
    }

    _4f0[8].getFirst()->getObject()->mShadow->makeSharedDL();

    for (u8 i = 0; i < 16; i++)
    {
        JSUListIterator<ItemObj> it(_4f0[i].getFirst());
        if (it.isAvailable())
        {
            for (; it.isAvailable(); ++it)
            {
                if (!it.isAvailable() || it->mModel.getModelData() == nullptr)
                    break;
                it->mModel.lock();
                ShadowModel *shdw = it->mShadow;
                if (shdw)
                    shdw->lock();
            }
        }
    }

    if (_820)
    {
        createCharShadowModel(heap, camera_index);
    }
}

void ItemObjMgr::initCharShadowModelData()
{
    mpMdlData = nullptr;
    if (mpMdlData == nullptr) // it just got set to null, so yeah it definitely has to be checked if it's null
    {
        mpMdlData = J3DModelLoaderDataBase::load(ObjUtility::getPtrMainArc("/Item/driver_shadow1.bmd"), 0x19210010);
        ExModel::patchModelData(mpMdlData);
        mpMdlData->newSharedDisplayList(0x40000);
    }

    for (u8 i = 0; i < 2; i++)
    {
        mpShadowMdl[i] = new ShadowModel(0xf, ShadowModel::cShadowKind_Geo, 0xff);
        mpShadowMdl[i]->setModelData(mpMdlData);
    }

    if (RCMGetManager()->isLANMode())
    {
        for (int i = 0; i < RCMGetManager()->getKartNumber(); i++)
        {
            if (KartInfo::KartCharacter::convPlayerKind(RCMGetManager()->getKartInfo(i)->getPad(0)) == 1)
            {
                _824 = i;
                break;
            }
        }
    }
}

void ItemObjMgr::createCharShadowModel(JKRSolidHeap *heap, u32 camera_index)
{
    mpMdlData->makeSharedDL();
    for (u8 i = 0; i < 2; i++)
    {
        SYSDBG_SetHeapGroup("OBJECT SHADOW MODEL", nullptr);
        mpShadowMdl[i]->createModel(heap, camera_index, 0x60000);
        SYSDBG_SetHeapGroup("OBJECT MODEL", nullptr);
        mpShadowMdl[i]->getModel()->lock();
    }
}

void ItemObjMgr::calcCharShadowModel() {}

void ItemObjMgr::calc()
{
    if (_820)
    {
        calcCharShadowModel();
    }

    if (RCMGetManager()->isLANMode())
    {
        setLANDebugInfo();
    }
}

void ItemObjMgr::calcIsHitItem(JSUListIterator<ItemObj> &list1, JSUListIterator<ItemObj> &list2) {
    // I hope you got fired for writing this function
    if ((list2->_12c & 1) == 0)
        return;

    const JGeometry::TVec3f colPos = list2->mColPos; // should've used a reference
    if (list1->IsHitObject(list1->mBounds, colPos, list2->mBounds)) {
        ItemObj *item1 = list1.getObject(); // pls use this for the actualy function calls as well, it's the exact same as operator->
        ItemObj *item2 = list2.getObject();
        ItemObj::ItemColFlag flag1 = list1->calcColReaction(item2);
        ItemObj::ItemColFlag flag2 = list2->calcColReaction(item1);
        
        if (flag1 != 6 && flag2 != 6) {            
            list1->callHitItemSound(item2);
            list2->callHitItemSound(item1);
            list1->setTransfer1();
            list2->setTransfer1();
            ItemObj::colItemReflect(item1, item2, flag1, flag2);
        }   
    }
}

void ItemObjMgr::setKartHittingList(JSUListIterator<ItemObj> &list, int kart_index)
{
    mHitList[kart_index].mObjects[list->getKartReaction()] = list.getObject();
}

void ItemObjMgr::checkItemByShuffle() {}

void ItemObjMgr::abortItemShuffle(int kart_index)
{
    for (u8 i = 0; i < 2; i++)
    {
        mShuffMgr[kart_index][i]->reset();
    }
}

ItemObj *ItemObjMgr::getItemWanWanObj(ItemObj *obj)
{
    ItemObj *ret = nullptr;
    if (obj->getKind() == 7)
    {
        ret = obj;
    }
    return ret;
}

ItemObj *ItemObjMgr::getItemFireBall(ItemObj *obj)
{
    ItemObj *ret = nullptr;
    if (obj->getKind() == 9)
    {
        ret = obj;
    }
    return ret;
}

ItemObj *ItemObjMgr::getItemFireBallSuc(ItemObj *obj)
{
    ItemObj *ret = nullptr;
    if (obj->getKind() == 0x15)
    {
        ret = obj;
    }
    return ret;
}

ItemObj *ItemObjMgr::getItemYoshiEgg(ItemObj *obj)
{
    ItemObj *ret = nullptr;
    if (obj->getKind() == 0xb)
    {
        ret = obj;
    }
    return ret;
}

void ItemObjMgr::setHeartItem(int kart_index, ItemHeart *heart)
{
#line 2392
    JUT_MINMAX_ASSERT(0, kart_index, 8);
    mHeartItem[kart_index] = heart;
}

void ItemObjMgr::deleteHeartItem(int kart_index)
{
    ItemHeart *heart = getHeartItem(kart_index);

    if (heart)
        heart->setStateDisappear(true);

    setHeartItem(kart_index, nullptr);
    for (u8 i = 0; i < 2; i++)
    {
        mStockItem[kart_index][i]._4 = false;
    }
}

bool ItemObjMgr::tstHaveItemHeart(int kart_index)
{
#line 2424
    JUT_MINMAX_ASSERT(0, kart_index, 8);
    return (mHeartItem[kart_index] && mHeartItem[kart_index]->hasHeartsRemaining());
}

void ItemObjMgr::executeColHitHeart(ItemObj *obj, int kart_index)
{
#line 2439
    JUT_MINMAX_ASSERT(0, kart_index, 8);
    mHeartItem[kart_index]->setHitItem(*obj);
}

ItemHeart *ItemObjMgr::getHeartItem(int kart_index)
{
#line 2453
    JUT_MINMAX_ASSERT(0, kart_index, 8);
    return mHeartItem[kart_index];
}

void ItemObjMgr::setItemUseTrigger(int kart_index, u8 driver_index)
{
#line 2466
    JUT_MINMAX_ASSERT(0, kart_index, 8);
    JUT_MINMAX_ASSERT(0, driver_index, 2);
    mItemUse[kart_index][driver_index] = true;
}

int ItemObjMgr::tstItemUseTrigger(int kart_index, u8 driver_index) const
{
#line 2481
    JUT_MINMAX_ASSERT(0, kart_index, 8);
    JUT_MINMAX_ASSERT(0, driver_index, 2);

    return mItemUse[kart_index][driver_index];
}

void ItemObjMgr::setThunderDisableUseCounter()
{
    mThunderDisableTimer = sMaxThunderDisableFrame;

    if (GetGeoObjMgr()->isMiniGame())
    {
        mThunderDisableTimer = sMiniGameMaxThunderDisableFrame;
    }
}

u8 ItemObjMgr::getNowTandemDriverNum(int kart_index)
{
    int driverNum = 0;
    if (GetKartCtrl())
    {
        if (GetKartCtrl()->WhichNowDriver(kart_index) == 0)
        {
            driverNum = 1;
        }
    }
    else
    {
        driverNum = 1;
    }
    return driverNum;
}

bool ItemObjMgr::IsSpecialCharacter(s32 charID) { return charID == 0x13 || charID == 0x14; }

bool ItemObjMgr::IsAbleToCreateItem(int itemIdx) { // UNUSED, fabricated, size mismatch
    bool ret = false;

    if (itemIdx == 10 && isThunderDisabled()) {
        return false;
    }
    
    int iVar2 = 0;
    u32 kind = itemIdx;

    if (itemIdx > 0x10) {
        iVar2 = kind == 0x15 ? 4 : 2;
        kind = ItemObjMgr::getConvertSucItemKind(kind);
    }
    int moreTemps = iVar2 + 1;
    int links = _3e8[kind].getNumLinks();
    links += getNowStockingKindNum(itemIdx, moreTemps);

    if (sMaxItemNum[kind] - links >= moreTemps) {
        ret = true;
    }
    
    return ret;
}

int ItemObjMgr::getNowStockingKindNum(int kind, u32 p2) { // UNUSED, fabricated, size match
    int ret = 0;
    for (int i = 0; i < RCMGetManager()->getKartNumber(); i++) {
        for (u8 j = 0; j < 2; j++) {
            if (mStockItem[i][j].mKind == kind) {
                ret += p2;
            }
        }
    }
    return ret;
}

bool ItemObjMgr::IsRollingSlot(int kart_index)
{
    bool ret = false;
    for (u8 i = 0; i < 2; i++)
    {
        if (!mShuffMgr[kart_index][i]->isNotRolling())
        {
            ret = true;
            break;
        }
    }

    return ret;
}

bool ItemObjMgr::IsRollingSlot(int kart_index, u8 driver_index)
{
    bool ret = false;
    if (!mShuffMgr[kart_index][driver_index]->isNotRolling())
        ret = true;
    return ret;
}

bool ItemObjMgr::IsAvailableRollingSlotDriver(int kart_index, u8 driver_index)
{
    // Size mismatch (target: 0xc4, current: 0xc0)
    bool ret = false;
    if (mShuffMgr[kart_index][driver_index]->isNotRolling()) {
        if (GetGeoObjMgr()->isBombBattle() && getRobberyItemNum(kart_index, driver_index) < getMaxHoldMiniBombNum()) {
            ret = true;
        }
        else if(getKartEquipItem(kart_index, driver_index) == nullptr) {
            ret = true;
        }
    }
    return ret;
}

int ItemObjMgr::getMaxHoldMiniBombNum() { return sMaxHoldMiniBombNum; }

void ItemObjMgr::getNowEnableSlotDriver(u8 *pDriverIndex, int kart_index) {
    if(GetKartCtrl()->CheckChange(kart_index) && getKartEquipItem(kart_index, *pDriverIndex)) {
        *pDriverIndex = swapDriver(*pDriverIndex);
    }
}

bool ItemObjMgr::IsAvailableRollingSlot(int kart_index, u32 kind) {
    bool ret = false;
    if (kind == 0xc) {
        for (u8 i = 0; i < 2; i++) {
            ret = IsAvailableRollingSlotDriver(kart_index, (u8)i);
            if(ret) {
                break;
            }
        }
    }
    else {
        u8 driver_index = getNowTandemDriverNum(kart_index);
        getNowEnableSlotDriver(&driver_index, kart_index);
        ret = IsAvailableRollingSlotDriver(kart_index, (u8)driver_index);
    }
    return ret;
}

void ItemObjMgr::startItemShuffleSingle(int kart_index, bool doShuffle) {
    u8 driver_index = getNowTandemDriverNum(kart_index);
    getNowEnableSlotDriver(&driver_index, kart_index);

    mShuffMgr[kart_index][driver_index]->startShuffle();
    if(doShuffle) {
        mShuffMgr[kart_index][driver_index]->set_7(true);
    }
}

void ItemObjMgr::startItemShuffleDouble(int kart_index)
{
    for (u8 i = 0; i < 2; i++)
    {
        if (GetGeoObjMgr()->isBombBattle())
        {
            u8 maxBombHold = sMaxHoldMiniBombNum;
            if (getRobberyItemNum(kart_index, (u8)i) < maxBombHold)
            {
                mShuffMgr[kart_index][i]->startShuffle();
            }
        }
        else if (mEquipItem[kart_index][i] == nullptr)
        {
            mShuffMgr[kart_index][i]->startShuffle();
        }
    }
}

ItemObj::ItemHandOffsetData &ItemObjMgr::getHandOffsetData(ItemObj *obj)
{
#line 2782
    JUT_MINMAX_ASSERT(0, obj->getOwnerNum(), 8);
    JUT_MINMAX_ASSERT(0, obj->getDriverNum(), 2);

    ECharID charID = RCMGetManager()->getKartInfo(obj->getOwnerNum())->getDriverCharID(obj->getDriverNum());
    int shuffleNum = obj->getItemID();

#line 2790
    JUT_MINMAX_ASSERT(cCharIDNone, charID, cCharIDMax);
    return (mpHandOffsetData + (shuffleNum * sizeof(ItemObj::ItemHandOffsetData)))[charID - 1];
}

u32 ItemObjMgr::getConvertSucItemKind(u32 kind) {
    u32 ret = kind;
    if (kind> 0x10) { // special item kind
        switch (kind) {
        case 17: { // triple green shells
            ret = 0; // single green shell
            break;
        }
        case 19: { // triple red shells
            ret = 2; // single red shell
            break;
        }
        case 18: { // triple mushrooms
            ret = 5; // single mushroom
            break;
        }
        case 21: { // red/green fireballs
            ret = 9; // in my documentation this says just "special item"
            break;
        }
        }
    }
    return ret;
}

int ItemObjMgr::getRobberyItemNum(int kart_index, u8 driver_index)
{
    int ret = 0;
    if (mEquipItem[kart_index][driver_index])
    {
        ret = mEquipItem[kart_index][driver_index]->getSuccessionItemNum();
    }
    return ret;
}

void ItemObjMgr::removeMiniGameList(ItemObj *obj)
{
    mMiniGameList.remove(&obj->mMiniGameLink);
}

void ItemObjMgr::update(ItemObjMgr::eDrawSimplModelItemType type, int owner) {

    for(int i = 0; i < 15; i++) {
        for (JSUListIterator<ItemObj> it(_3e8[sJ3DUpdateItemKind[i]].getFirst()); it.isAvailable(); ++it) {
            bool doUpdate = true;
            switch(type) {
            case ItemType_1: {
                doUpdate = it->isHeldByOwner(owner);
                break;
            }
            case ItemType_2: {
                doUpdate = it->isHeld();
                break;
            }
            }

            if (doUpdate) {
                it->setLightMask();
                it->setTevColor();
                it->update();
            }
        }
    }
}

void ItemObjMgr::viewCalc(u32 viewNo) {
    J3DUClipper *clipper =  GetKartCtrl()->getKartCam(viewNo)->GetClipper();
    const Mtx &viewMtx = j3dSys.getViewMtx();
    for(int i = 0; i < 15; i++) {
        for (JSUListIterator<ItemObj> it(_3e8[sJ3DUpdateItemKind[i]].getFirst()); it.isAvailable(); ++it) {
            if(it->tst_80()) {
                it->mModel.hide();
            }
            else {
                it->mModel.clipBySphere(viewNo, clipper, viewMtx, it->mEquipScale);
            }            
        }
    }

    for(int i = 0; i < 10; i++) {
        for (JSUListIterator<ItemObj> it(_3e8[sNormalDrawItemKind[i]].getFirst()); it.isAvailable(); ++it)
            it->viewCalc(viewNo);
    }
}

void ItemObjMgr::setCurrentViewNo(u32 viewNo)
{
    PSMTXCopy(RCMGetManager()->getCamera(viewNo)->GetMtx(), j3dSys.getViewMtx());
    for (int i = 0; i < 10; i++)
    {
        for (JSUListIterator<ItemObj> it(_3e8[sNormalDrawItemKind[i]].getFirst()); it.isAvailable(); ++it)
            it->setCurrentViewNo(viewNo);
    }
}

void ItemObjMgr::drawSimpleModel(u32 viewNo, ItemObjMgr::eDrawSimplModelItemType type, int owner, LightObj *lightObj) {  
    J3DUClipper *clipper = GetKartCtrl()->getKartCam(viewNo)->GetClipper();
    MtxPtr viewMtx = j3dSys.getViewMtx();
    MtxPtr effectMtx = nullptr;
    if (lightObj) {
        effectMtx = lightObj->getEffectMtx();
    }
    
    SimpleDrawer drawer;

    for(int i = 0; i < 5; i++) {
        JSUList<ItemObj> &list = _3e8[sSimpleDrawItemKind[i]];
        if (!list.getFirst())
            continue;
        
        ExModel *mdl = list.getFirst()->getObject()->getModel();
        if (!mdl->getModelData())
            continue;

        drawer.drawInit(mdl);
        while (drawer.loadPreDrawSetting()) {
            for (JSUListIterator<ItemObj> it(list.getFirst()); it.isAvailable(); ++it) {
                bool doUpdate = true;
                switch(type) {
                case ItemType_1: {
                    doUpdate = it->isHeldByOwner(owner);
                    break;
                }
                case ItemType_2: {
                    doUpdate = it->isHeld();
                    break;
                }
                }

                if (doUpdate) {
                    it->setTevColor();
                    it->simpleDraw(viewNo, effectMtx, sSimpleDrawSpeqEnvTexMapID[i]);
                }
            }
        }
    }

    for (JSUListIterator<ItemObj> it(_3e8[7].getFirst()); it.isAvailable(); ++it) {
        bool doUpdate = true;
        switch(type) {
        case ItemType_1: {
            doUpdate = it->isHeldByOwner(owner);
            break;
        }
        case ItemType_2: {
            doUpdate = it->isHeld();
            break;
        }
        }

        if (doUpdate) {
            it->drawSimpleModel(viewNo, viewMtx, clipper, effectMtx);
        }
    }
}

void ItemObjMgr::drawColModel(u32) {}

void ItemObjMgr::setLANDebugInfo()
{
    if (sLANDbgDataPtr == nullptr || sLANDbgKartPadBuffer == nullptr)
        return;

    JSUList<ItemObj> *objList = gItemMgr->_3e8;
    int fiveSecintervals = RCMGetManager()->getRaceFrame() % 300;
    u32 itemNum = 0;

    for (int i = 0; i < 16; i++)
    {
        for (JSUListIterator<ItemObj> it(objList->getFirst()); it != objList->getEnd(); ++it)
        {
            if (itemNum < 0x40)
            {
                SLAN_DebugItemData *itemData = &sLANDbgDataPtr[fiveSecintervals].itemData[itemNum];
                itemData->init(it.getObject());
                itemNum++;
            }
        }
        objList++;
    }

    for (u8 i = 0; i < 16; i++)
    {
        sLANDbgKartPadBuffer[fiveSecintervals].padData[i].mButtons = gpaKartPad[i]->mButtons;
        sLANDbgKartPadBuffer[fiveSecintervals].padData[i].mMainStick = gpaKartPad[i]->mMainStick;
    }
}

void ItemObjMgr::draw()
{
    JUTReport(20, 60, "SlotTable");
    // i have a feeling this is inlined / has some sort of macro
    ItemShuffleMgr::KartSlotData *slotData = &ItemShuffleMgr::mSlotList;
    JUTReport(100, 80, "Player");

    for (u8 kartNum = 0; kartNum < slotData->kartCount; kartNum++)
    {
        for (u8 slotNum = 0; slotNum < slotData->totalSlots; slotNum++)
        {
            JUTReport(kartNum * 33 + 20, slotNum * 12 + 100, "%u", slotData->mList[kartNum].slotTable[0][slotNum]);
        }
    }

    if (RCMGetManager()->isRaceModeGp())
    {
        ItemShuffleMgr::KartSlotData *slotData = &ItemShuffleMgr::mSlotListEnemy;
        JUTReport(400, 80, "Enemy");
        for (u8 kartNum = 0; kartNum < slotData->kartCount; kartNum++)
        {
            for (u8 slotNum = 0; slotNum < slotData->totalSlots; slotNum++)
            {
                JUTReport(kartNum * 33 + 320, slotNum * 12 + 100, "%u", slotData->mList[kartNum].slotTable[0][slotNum]);
            }
        }
    }
}

ItemShuffleMgr::KartSlotData ItemShuffleMgr::mSlotList;
ItemShuffleMgr::KartSlotData ItemShuffleMgr::mSlotListEnemy;
int ItemShuffleMgr::SLOT_MAX_COUNT = sSlotNormalMaxCount;
int ItemShuffleMgr::SLOT_INVALID_BUTTON_COUNT = sSlotInvalidButtonCount;
int ItemShuffleMgr::SLOT_WAIT_COUNT = sSlotWaitCount;
u8 ItemShuffleMgr::sSlotNormalItemNum;  // 9
u8 ItemShuffleMgr::sSlotSpecialItemNum; // 9
int *ItemShuffleMgr::sSlotKindIndexArray;

namespace
{
    int sSlotItemIndex[27] = {
        0, 2, 13, 3,
        5, 18, 6, 10,
        15, 17, 1, 4,
        21, 8, 11, 7,
        12, 14, 17, 1,
        4, 21, 8, 11,
        7, 12, 14};

    int sRndNormalSlotIndex[] = {
        0, 2, 15, 3, 5, 18, 6, 10, 13};

    int sRndSpecialSlotIndex[] = {
        17, 1, 4, 21, 8, 11, 7, 12, 14};

    const int sSlotTblIndexForLAN_5P[5] = {
        1, 3, 4, 6, 8};
    const int sSlotTblIndexForLAN_6P[6] = {
        1, 2, 4, 5, 7, 8};
    const int sSlotTblIndexForLAN_7P[7] = {
        1, 2, 3, 4, 6, 7, 8};
}

u32 ItemShuffleMgr::getRndItemKindPossibilityGetting(stRandom *rnd, const int kart_index, const u8 driver_index)
{
    u8 numItems = 9;
    u8 numSpecialItems = 1;
    bool special = false;
    if (GetGeoObjMgr()->isMiniGame()) {
        if (GetGeoObjMgr()->isEscapeBattle()) {
            numItems -= 1;
        }
        else {
            numItems -= 2;
        }
        
        numSpecialItems = 5;
        special = true;
    }
    else {
        s32 charID = RCMGetManager()->getKartInfo(kart_index)->getDriverCharID(driver_index);
        if (charID == 0x14 || charID == 0x13) {
            numSpecialItems = 9;
            special = true;
        }
    }

    u32 randomNum = rnd->getRandomMax(numItems + numSpecialItems - 1);
    if (randomNum < numItems) {
        return sRndNormalSlotIndex[randomNum];
    }

    if (!special) {
        return ItemObj::getSpecialKind(kart_index, driver_index);
    }

#line 3430
    u32 tmpNum = randomNum - numItems;
    JUT_MINMAX_ASSERT(0, tmpNum, numSpecialItems);
    return sRndSpecialSlotIndex[tmpNum];
}

void ItemShuffleMgr::loadSlotData()
{
    SLOT_MAX_COUNT = sSlotNormalMaxCount;
    SLOT_INVALID_BUTTON_COUNT = sSlotInvalidButtonCount;
    SLOT_WAIT_COUNT = sSlotWaitCount;

    if (GetGeoObjMgr()->isBombBattle())
    {
        SLOT_MAX_COUNT = 0;
        SLOT_INVALID_BUTTON_COUNT = 0;
        SLOT_WAIT_COUNT = 0;
    }
    u8 *slotListData = (u8 *)ResMgr::getPtr(ResMgr::mcArcMRAM, getSlotList(RCMGetManager()->getItemSlotType()));
    sSlotNormalItemNum = 9;
    sSlotSpecialItemNum = 9;

    sSlotKindIndexArray = sSlotItemIndex;
    u8 slotCount = 0;
    u8 enemySlotCount = 0;
    bool isLanVs = false;

    switch (RCMGetManager()->getRaceLevel())
    {
    case LVL_50CC:
        slotCount = 0;
        enemySlotCount = 1;
        break;
    case LVL_100CC:
        slotCount = 2;
        enemySlotCount = 3;
        break;
    case LVL_150CC:
    case LVL_MIRROR:
        slotCount = 4;
        enemySlotCount = 5;
        break;
    }

    if (RCMGetManager()->isLANMode())
    {
        switch (RCMGetManager()->getKartNumber())
        {
        case 2:
            slotCount = 6;
            break;
        case 3:
            slotCount = 7;
            break;
        case 4:
            slotCount = 8;
            break;
        default:
            slotCount = 9;
            break;
        }
        isLanVs = true;
    }
    else
    {
        switch (RCMGetManager()->getRaceMode())
        {
        case VERSUS_RACE:
        {
            switch (RCMGetManager()->getKartNumber())
            {
            case 2:
                slotCount = 6;
                break;
            case 3:
                slotCount = 7;
                break;
            case 4:
                slotCount = 8;
                break;
            }
        }
        break;
        }
    }
    if (GetGeoObjMgr()->isMiniGame())
    {
        isLanVs = false;
        switch (RCMGetManager()->getRaceMode())
        {
        case BALLOON_BATTLE:
            slotCount = 10;
            break;
        case ESCAPE_BATTLE:
            slotCount = 12;
            break;
        }
    }

    u8 *defaultSlotList = slotListData + calcBufOffset(slotListData + 1, slotCount);
    setToList(defaultSlotList + 1, &mSlotList);
    if (isLanVs)
    {
        switch (RCMGetManager()->getKartNumber())
        {
        case 5:
            setToListLAN(&mSlotList, 5, sSlotTblIndexForLAN_5P);
            break;
        case 6:
            setToListLAN(&mSlotList, 6, sSlotTblIndexForLAN_6P);
            break;
        case 7:
            setToListLAN(&mSlotList, 7, sSlotTblIndexForLAN_7P);
            break;
        }
    }
    else
    {
        u8 *enemyList = slotListData;
        if (RCMGetManager()->getRaceMode() == VERSUS_RACE)
        {
            enemyList += calcBufOffset(enemyList + 1, slotCount);
            enemyList += 1;
        }
        else
        {
            enemyList += calcBufOffset(enemyList + 1, enemySlotCount);
            enemyList += 1;
        }
        setToList(enemyList, &mSlotListEnemy);
    }
}

u32 ItemShuffleMgr::calcBufOffset(u8 *pData, int n)
{
    u32 offset = 0;
    while (n-- > 0)
    {
        u8 playerCount = *pData++;
        u8 itemCount = *pData++;
        offset += playerCount * itemCount + 2;
        pData += playerCount * itemCount;
    }
    return offset;
}

void ItemShuffleMgr::setToList(u8 *pData, ItemShuffleMgr::KartSlotData *slotData)
{
    u8 playerCount = *pData++;
    u8 itemCount = *pData++;

    slotData->mList = new KartSlotList[playerCount];
    slotData->kartCount = playerCount;
    slotData->totalSlots = itemCount;
    slotData->useableSlots = itemCount - sSlotSpecialItemNum;

    for (u8 playerIndex = 0; playerIndex < playerCount; playerIndex++)
    {
        KartSlotList &list = slotData->mList[playerIndex];
        if (list.slotTable[0] == nullptr)
        {
            list.slotTable[0] = new u8[itemCount];
        }
        if (list.slotTable[1] == nullptr)
        {
            list.slotTable[1] = new u8[itemCount];
        }
    }

    for (u8 itemIndex = 0; itemIndex < itemCount; itemIndex++)
    {
        for (u8 playerIndex = 0; playerIndex < playerCount; playerIndex++)
        {
            slotData->mList[playerIndex].slotTable[0][itemIndex] = *pData++;
        }
    }
}

void ItemShuffleMgr::setToListLAN(ItemShuffleMgr::KartSlotData *data, const u8 &playerCount, const int *pTblIndex)
{
    data->kartCount = playerCount;
    for (u8 rnum = 0; rnum < playerCount; rnum++)
    {
        if (rnum != pTblIndex[rnum])
        {
#line 3643
            JUT_ASSERT(data->mList + rnum != 0);
            for (u8 slot = 0; slot < data->totalSlots; slot++)
            {
                data->mList[rnum].slotTable[0][slot] = data->mList[pTblIndex[rnum] - 1].slotTable[0][slot];
            }
        }
    }
}

void ItemShuffleMgr::setAllDynamicRate()
{
    setDynamicRate(&mSlotList, false);
    if (!RCMGetManager()->isLANMode() && RCMGetManager()->getRaceMode() == GRAND_PRIX)
        setDynamicRate(&mSlotListEnemy, true);
}

void ItemShuffleMgr::setDynamicRate(ItemShuffleMgr::KartSlotData *slotData, bool isGrandPrix)
{
    for (int i = 0; i < slotData->totalSlots; i++) {
        bool ableToCreate = false;
        if (GetItemObjMgr()->IsAbleToCreateItem(sSlotItemIndex[i]))
            ableToCreate = true;
        
        for (int j = 0; j < slotData->kartCount; j++) {
            if (ableToCreate) {
                slotData->mList[j].slotTable[1][i] = slotData->mList[j].slotTable[0][i];
                if (isGrandPrix && RivalBodyCtrl::isPlayerFirst() && sSlotItemIndex[i] == 0xd) {
                    slotData->mList[j].slotTable[1][i] *= sGPEnemyFlyTurtleItemSlotRatio;
                }
            }
            else {
                slotData->mList[j].slotTable[1][i] = 0;
            }
        }    
    }
}

int ItemShuffleMgr::SlotItem(const int kart_index, const u8 driver_index) {
    bool hasEnemy = !ObjUtility::isPlayerDriver(kart_index);

    if (!RCMGetManager()->isRaceModeGp()) {
        hasEnemy = false;
    }

    const KartSlotData *slotList = hasEnemy ? &mSlotListEnemy : &mSlotList;

    int specialKind = ItemObj::getSpecialKind(kart_index, driver_index);
    int rankIdx = RCMGetKartChecker(kart_index)->getRank() - 1;
    switch(RCMGetManager()->getRaceMode()) {
    case BALLOON_BATTLE: {
        rankIdx = ItemObjMgr::sTopBalloonNum - RCMGetKartChecker(kart_index)->getBalloonNumber();;
        if (rankIdx > 2)
            rankIdx = 2;
        break;
    }
    case ESCAPE_BATTLE: {
        if (kart_index == GeoRabbitMark::getSupervisor()->getRabbitKartNo()) {
            rankIdx = 0;
        }
        else {
            rankIdx = RCMGetManager()->getKartNumber();
            if (rankIdx >= 5) {
                rankIdx = 4;
            }
            else {
                rankIdx -= 1;
            }
        }
        break;
    }
    case BOMB_BATTLE: {
        return 8;
    }
    }

    u8 swapped_idx = driver_index == 0 ? 1 : 0;
    ItemObj *equipedItem = GetItemObjMgr()->getKartEquipItem(kart_index, swapped_idx);
    int normalKind;
    if (equipedItem != nullptr) {
        normalKind = equipedItem->getKind();
    }
    else {
        normalKind = GetItemObjMgr()->getStockItem(kart_index, swapped_idx);
    }
    const bool combi = RCMGetManager()->getKartInfo(kart_index)->isDefaultCharCombi();
    if (_7) {
        int slot = calcRndSpecialSlot(rankIdx, *slotList, combi);
        if (slot >= 0)
            return slot;
    }
    KartSlotRankDataSet rdata;
    rdata.slotData = slotList;
    rdata.kart_rank = rankIdx;
    rdata.kart_index = kart_index;
    return calcSlot(rdata, specialKind, normalKind, combi);
}

bool ItemShuffleMgr::doShuffle()
{
    if (mRollFlags & 1)
    {
        mSlotCount++;
        if (mSlotCount > SLOT_MAX_COUNT)
        {
            mSlotCount = 0;
            mRollFlags = 0;
            return true;
        }

        if (mSlotCount > SLOT_INVALID_BUTTON_COUNT)
        {
            bool requestReset = false; // inline?
            if (ObjUtility::isPlayerDriver(mKartNumber))
            {
                KartGamePad *coCont = GetKartCtrl()->GetCoDriveCont(mKartNumber);
                if (coCont->testTrigger(GetKartCtrl()->getKartPad(mKartNumber)->mItemMask))
                    requestReset = true;
            }
            else
            {
                if (GetKartCtrl()->getKartEnemy(mKartNumber)->getBodyCtrl()->isRival())
                {
                    requestReset = true;
                }
            }
            if (requestReset)
            {
                resetRolling();
                return true;
            }
        }
    }
    else if (mRollFlags & 2)
    {
        mSlotCount++;
        if (mSlotCount > SLOT_WAIT_COUNT)
        {
            mSlotCount = 0;
            mRollFlags = 0;
        }
    }
    return false;
}

void ItemShuffleMgr::resetRolling()
{
    mRollFlags &= ~1;
    mRollFlags |= 2;
    mSlotCount = 0;
}

void ItemShuffleMgr::calcRaceUseNormalItem(u32 *pTotal, ItemShuffleMgr::KartSlotRankDataSet *rdata, int maxId)
{
    for (int i = 0; i < sSlotNormalItemNum; i++)
    {
        int itemIdx = sSlotItemIndex[i];
        if ((((itemIdx != 10) && (itemIdx != 6) && (itemIdx != 0x12)) && (itemIdx != 0xd)) || maxId != itemIdx)
        {
            *pTotal += rdata->slotData->mList[rdata->kart_rank].slotTable[1][i];
            sTempSlotUseItem[i] = true;
        }
        else
        {
            sTempSlotUseItem[i] = false;
        }
    }
}

void ItemShuffleMgr::calcSpecialItemNum(u32 *pTotal, ItemShuffleMgr::KartSlotRankDataSet *data, int itemID1, int itemID2, bool charCombi)
{
    data->specialItemIndex = -1;
    
    for (int i = sSlotNormalItemNum; i < data->slotData->totalSlots; i++)
    {
        if (itemID1 == sSlotKindIndexArray[i]) {
            data->specialItemIndex = i;

            if (ItemObj::IsSpecialItem(itemID2) || (itemID1 == 0xe && ItemObjMgr::getItemObjMgr()->getHeartItem(data->kart_index))) {
                sTempSlotUseItem[sSlotNormalItemNum] = false;
            }
            else {
                sTempSlotUseItem[sSlotNormalItemNum] = true;
                u32 chance = data->slotData->mList[data->kart_rank].slotTable[1][i];

                if (charCombi) {
                    chance *= 1.5f;
                }
                *pTotal += chance;
                
                data->specialItemChance = chance;
            }
            break;
        }      
    }

#line 3920
    JUT_ASSERT(data->specialItemIndex != -1)
}

void ItemShuffleMgr::calcRndSpecialRatio(u32 *pTotal, ItemShuffleMgr::KartSlotRankDataSet *data, bool charCombi) {
    for (int i = 0; i < sSlotSpecialItemNum; i++) {
        if ((sSlotKindIndexArray[i + sSlotNormalItemNum] == 0xe && ItemObjMgr::getItemObjMgr()->getHeartItem(data->kart_index))) {
            sTempSlotUseItem[i + sSlotNormalItemNum] = false;
        }
        else {
            sTempSlotUseItem[i + sSlotNormalItemNum] = true;
            u32 chance = data->slotData->mList[data->kart_rank].slotTable[1][i + sSlotNormalItemNum + sSlotSpecialItemNum];

            if (!GetGeoObjMgr()->isMiniGame() && charCombi) {
                chance *= 1.5f;
            }
            *pTotal += chance;
            
            sTempSpecialRatio[i] = chance;
        }
    }
}

int ItemShuffleMgr::calcRank(KartSlotRankDataSet rdata)
{
    int ret = -1;
    u32 randomNum = stGetRnd(0)->getRandomMax(rdata.total - 1);

    int prevTotalChance = 0;
    int totalChance = 0;

    for (int i = 0; i < sSlotNormalItemNum + 1; i++, prevTotalChance = totalChance)
    {
        int itemChance = rdata.slotData->mList[rdata.kart_rank].slotTable[1][i];
        int itemIdx = i;
        if (i >= sSlotNormalItemNum)
        {
            itemIdx = rdata.specialItemIndex;
            itemChance = rdata.specialItemChance;
        }

        totalChance += sTempSlotUseItem[i] ? itemChance : 0;
        if ((prevTotalChance <= randomNum) && (totalChance > randomNum))
        {
            ret = sSlotKindIndexArray[itemIdx];
            break;
        }
    }
    // if ret == -1 hang, in the release version, just give the player a banana as failsafe
#line 3995
    JUT_ASSERT(ret != -1);

    if (ret == -1)
        ret = 3;

    return ret;
}

int ItemShuffleMgr::calcSlot(KartSlotRankDataSet &rdata, int p2, int p3, bool charCombi)
{
#line 4017
    JUT_MINMAX_ASSERT(0, rdata.kart_rank, rdata.slotData->kartCount);
    u32 total = 0;

    calcRaceUseNormalItem(&total, &rdata, p3);
    calcSpecialItemNum(&total, &rdata, p2, p3, charCombi);

    rdata.total = total;

    return calcRank(rdata);
}

int ItemShuffleMgr::calcRndSpecialSlot(int kart_rank, const ItemShuffleMgr::KartSlotData &rdata, bool combi) {
#line 4044
    JUT_MINMAX_ASSERT(0, kart_rank, rdata.kartCount);

    u32 total = 0;
    KartSlotRankDataSet data;
    data.slotData = &rdata;
    data.kart_rank = kart_rank;
    calcRndSpecialRatio(&total, &data, combi);
    data.total = total;
    return calcRndSpecialRank(data);
}

void ItemRndSpecialShuffleMgr::calcRaceUseNormalItem(u32 *pTotal, ItemShuffleMgr::KartSlotRankDataSet *rdata, int maxId)
{
    for (int i = 0; i < sSlotNormalItemNum; i++)
    {
        int itemIdx = sSlotItemIndex[i];
        if ((((itemIdx != 10) && (itemIdx != 6) && (itemIdx != 0x12)) && (itemIdx != 0xd)) || maxId != itemIdx)
        {
            *pTotal += rdata->slotData->mList[rdata->kart_rank].slotTable[1][i];
            sTempSlotUseItem[i] = true;
        }
        else
        {
            sTempSlotUseItem[i] = false;
        }
    }
}

void ItemRndSpecialShuffleMgr::calcSpecialItemNum(u32 *pTotal, ItemShuffleMgr::KartSlotRankDataSet *data, int itemID1, int itemID2, bool charCombi) {
    if (ItemObj::IsSpecialItem(itemID2)) {
        for (int i = 0; i < sSlotSpecialItemNum; i++) {
            sTempSlotUseItem[i + sSlotNormalItemNum] = false;
        }
        return;
    }
    
    calcRndSpecialRatio(pTotal, data, charCombi);
}

int ItemShuffleMgr::calcRndSpecialRank(KartSlotRankDataSet data)
{
    int ret = -1;
    u32 randomNum = stGetRnd(0)->getRandomMax(data.total - 1);
    int prevTotalChance = 0;
    int totalChance = 0;

    for (int idx = sSlotNormalItemNum; idx < data.slotData->totalSlots; idx++, prevTotalChance = totalChance)
    {
        int ratio = sTempSpecialRatio[idx - sSlotNormalItemNum];
        totalChance += sTempSlotUseItem[idx] ? ratio : 0;
        if ((prevTotalChance <= randomNum) && (totalChance > randomNum))
        {
            ret = sSlotKindIndexArray[idx];
            break;
        }
    }

    return ret;
}

int ItemRndSpecialShuffleMgr::calcRank(KartSlotRankDataSet slotRankData)
{
    int ret = -1;
    u32 randomNum = stGetRnd(0)->getRandomMax(slotRankData.total - 1);

    int prevTotalChance = 0;
    int totalChance = 0;

    for (int idx = 0; idx < slotRankData.slotData->totalSlots; idx++, prevTotalChance = totalChance)
    {
        u32 itemChance = idx >= sSlotNormalItemNum ? sTempSpecialRatio[idx - sSlotNormalItemNum] : slotRankData.slotData->mList[slotRankData.kart_rank].slotTable[1][idx];

        totalChance += sTempSlotUseItem[idx] ? itemChance : 0;
        if ((prevTotalChance <= randomNum) && (totalChance > randomNum))
        {
            ret = sSlotKindIndexArray[idx];
            break;
        }
    }
#line 4189
    JUT_ASSERT(ret != -1);

    if (ret == -1)
        ret = 3;

    return ret;
}

#include "JSystem/JAudio/JASFakeMatch2.h"
