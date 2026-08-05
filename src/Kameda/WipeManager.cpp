#include "Kameda/J2DManager.h"
#include "Kameda/WipeManager.h"
#include "Kaneshige/RaceMgr.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

WipeManager *WipeManager::mThis;
Task *WipeManager::mTask;
bool WipeManager::mMoveFlag[8];
bool WipeManager::mKillFlag[8];

#ifdef DEBUG
#include "mathHelper.h"
#endif

WipeParam::WipeParam()
{
    mDrawFlag = 1;
    _4 = 0;
    _6 = 0;
    _7 = 0;
    _8 = 0;
    _9 = 0xff;
    _a = 10;
    _c = 30;
    _e = 10;
    _10 = 0;
}

WipeManager::WipeManager(JKRHeap *heap)
{
    if (heap == nullptr)
        heap = JKRGetCurrentHeap();

    mThis = this;
    mTask = new (heap, 0) Task(0xc);

    switch (RCMGetManager()->getConsoleNumber())
    {
    case 1:
        mScrnCount = 1;
        break;
    case 2:
        mScrnCount = 2;
        break;
    case 3:
    case 4:
        mScrnCount = 4;
        break;

    default:
#line 145
        JUT_ASSERT(0);
    }

    mKartNum = RCMGetManager()->getKartNumber();

    mFade = new (heap, 0) Fade(heap);
    mMario = new (heap, 0) Mario(heap);
    mWipeSlide = new (heap, 0) TWipeSlide(heap);
    mWipeCheck = new (heap, 0) TWipeCheck(heap);
    mWipeShut = new (heap, 0) TWipeShut(heap);
    mWipeLineA = new (heap, 0) TWipeLineA(heap);
    mWipeLineB = new (heap, 0) TWipeLineB(heap);
    mWipeWaveA = new (heap, 0) TWipeWaveA(heap);
    mWipeCircle = new (heap, 0) TWipeCircle(heap);
    mWipeCurtain = new (heap, 0) TWipeCurtain(heap);

    TWipeBase::createBuffer(RCMGetManager()->getStatusNumber(), heap);
    reset();
    mNode = new (heap, 0) WipeHioNode();
}

WipeManager::~WipeManager() {
    // UNUSED, required to get destructor for WipeHioNode at the correct place
    // original size is 0xc8
    delete mNode;
    delete mTask;
    mNode = nullptr;
    mTask = nullptr;
    mThis = nullptr;
}

void WipeManager::reset()
{
    for (int i = 0; i < 8; i++)
    {
        _0[i].reset();
        mMoveFlag[i] = true;
        mKillFlag[i] = false;
    }

    mTask->init();
}

void WipeManager::draw()
{
    if (!mNode->mParam.isDrawEnabled())
        return;

    for (int i = 0; i < mKartNum; i++)
    {
        int status = J2DManager::getKart2Status(i);
        if (status >= 0 && status < 4)
        {
            switch (_0[i].mWipeType)
            {
            case 0:
                break;
            // draw in
            case 1:
            case 3:
            case 5:
            case 7:
            case 9:
            case 11:
            case 13:
            case 15:
            case 17:
            case 19:
            {
                if (RCMGetManager()->getKartChecker(i)->tstDead() || RCMGetManager()->isRaceModeMiniGame())
                {
                    drawIn(i, status, _0[i].mWipeType);
                }
                else
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (i == J2DManager::getStatus2Kart(j))
                            drawIn(i, j, _0[i].mWipeType);
                    }
                }

                break;
            }
            // draw out
            case 2:
            case 4:
            case 6:
            case 8:
            case 10:
            case 12:
            case 14:
            case 16:
            case 18:
            case 20:
            {
                if (RCMGetManager()->getKartChecker(i)->tstDead() || RCMGetManager()->isRaceModeMiniGame())
                {
                    drawOut(i, status, _0[i].mWipeType);
                }
                else
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (i == J2DManager::getStatus2Kart(j))
                            drawOut(i, j, _0[i].mWipeType);
                    }
                }
                break;
            }

            default:
#line 305
                JUT_ASSERT(0);
            }
        }
    }
}

void WipeManager::drawIn(int kart, int status, int wipeType)
{
#line 332
    JUT_MINMAX_ASSERT(0, kart, 8);
    JUT_MINMAX_ASSERT(0, status, 4);

    switch (_0[kart].mWipeState)
    {
    case 0:
    {
        switch (wipeType)
        {
        case 1:
            mFade->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 3:
        case 5:
            mWipeSlide->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 7:
            mWipeCheck->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 9:
            mWipeShut->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 11:
            mWipeLineA->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 13:
            mWipeLineB->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 15:
            mWipeWaveA->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 17:
            mWipeCircle->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 19:
            mWipeCurtain->draw(status, 1.0f, _0[kart].mColor);
            break;
        default:
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 1:
    {
        f32 f = 1.0f + -1.0f * _0[kart]._8 / (_0[kart]._c - 1.0f);
        switch (wipeType)
        {
        case 1:
            mFade->draw(status, f, _0[kart].mColor);
            break;
        case 3:
        case 5:
            mWipeSlide->draw(status, f, _0[kart].mColor);
            break;
        case 7:
            mWipeCheck->draw(status, f, _0[kart].mColor);
            break;
        case 9:
            mWipeShut->draw(status, f, _0[kart].mColor);
            break;
        case 11:
            mWipeLineA->draw(status, f, _0[kart].mColor);
            break;
        case 13:
            mWipeLineB->draw(status, f, _0[kart].mColor);
            break;
        case 15:
            mWipeWaveA->draw(status, f, _0[kart].mColor);
            break;
        case 17:
            mWipeCircle->draw(status, f, _0[kart].mColor);
            break;
        case 19:
            mWipeCurtain->draw(status, f, _0[kart].mColor);
            break;
        default:
#line 411
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 2:
    {
        switch (wipeType)
        {
        case 1:
            mFade->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 3:
        case 5:
            mWipeSlide->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 7:
            mWipeCheck->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 9:
            mWipeShut->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 11:
            mWipeLineA->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 13:
            mWipeLineB->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 15:
            mWipeWaveA->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 17:
            mWipeCircle->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 19:
            mWipeCurtain->draw(status, 0.0f, _0[kart].mColor);
            break;
        default:
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 6:
        break;
    default:
        JUT_ASSERT(0);
        break;
    }
}

void WipeManager::drawOut(int kart, int status, int wipeType)
{
#line 472
    JUT_MINMAX_ASSERT(0, kart, 8);
    JUT_MINMAX_ASSERT(0, status, 4);

    switch (_0[kart].mWipeState)
    {
    case 3:
    {
        switch (wipeType)
        {
        case 2:
            mFade->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 4:
        case 6:
            mWipeSlide->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 8:
            mWipeCheck->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 10:
            mWipeShut->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 12:
            mWipeLineA->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 14:
            mWipeLineB->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 16:
            mWipeWaveA->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 18:
            mWipeCircle->draw(status, 0.0f, _0[kart].mColor);
            break;
        case 20:
            mWipeCurtain->draw(status, 0.0f, _0[kart].mColor);
            break;
        default:
#line 510
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 4:
    {
        f32 f = _0[kart]._8 / (_0[kart]._c - 1.0f);
        switch (wipeType)
        {
        case 2:
            mFade->draw(status, f, _0[kart].mColor);
            break;
        case 4:
        case 6:
            mWipeSlide->draw(status, f, _0[kart].mColor);
            break;
        case 8:
            mWipeCheck->draw(status, f, _0[kart].mColor);
            break;
        case 10:
            mWipeShut->draw(status, f, _0[kart].mColor);
            break;
        case 12:
            mWipeLineA->draw(status, f, _0[kart].mColor);
            break;
        case 14:
            mWipeLineB->draw(status, f, _0[kart].mColor);
            break;
        case 16:
            mWipeWaveA->draw(status, f, _0[kart].mColor);
            break;
        case 18:
            mWipeCircle->draw(status, f, _0[kart].mColor);
            break;
        case 20:
            mWipeCurtain->draw(status, f, _0[kart].mColor);
            break;
        default:
#line 551
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 5:
    {
        switch (wipeType)
        {
        case 2:
            mFade->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 4:
        case 6:
            mWipeSlide->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 8:
            mWipeCheck->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 10:
            mWipeShut->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 12:
            mWipeLineA->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 14:
            mWipeLineB->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 16:
            mWipeWaveA->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 18:
            mWipeCircle->draw(status, 1.0f, _0[kart].mColor);
            break;
        case 20:
            mWipeCurtain->draw(status, 1.0f, _0[kart].mColor);
            break;
        default:
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 6:
        break;
    default:
        JUT_ASSERT(0);
        break;
    }
}

void WipeManager::calc()
{
    mTask->run();

    for (int i = 0; i < mKartNum; i++)
    {
        int status = J2DManager::getKart2Status(i);
        if (status >= 0 && status < 4)
        {
            switch (_0[i].mWipeType)
            {
            case 0:
                break;
            // draw in
            case 1:
            case 3:
            case 5:
            case 7:
            case 9:
            case 11:
            case 13:
            case 15:
            case 17:
            case 19:
            {

                for (int j = 0; j < 4; j++)
                {
                    if (i == J2DManager::getStatus2Kart(j))
                        calcIn(i, j, _0[i].mWipeType);
                }

                break;
            }
            // draw out
            case 2:
            case 4:
            case 6:
            case 8:
            case 10:
            case 12:
            case 14:
            case 16:
            case 18:
            case 20:
            {

                for (int j = 0; j < 4; j++)
                {
                    if (i == J2DManager::getStatus2Kart(j))
                        calcOut(i, j, _0[i].mWipeType);
                }

                break;
            }

            default:
#line 656
                JUT_ASSERT(0);
            }
        }
    }
}

void WipeManager::calcIn(int kart, int status, int wipeType)
{
#line 672
    JUT_MINMAX_ASSERT(0, kart, 8);
    JUT_MINMAX_ASSERT(0, status, 4);

    switch (_0[kart].mWipeState)
    {
    case 0:
    {
        switch (wipeType)
        {
        case 1:
            break;
        case 3:
        case 5:
            mWipeSlide->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 7:
            mWipeCheck->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 9:
            mWipeShut->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 11:
            mWipeLineA->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 13:
            mWipeLineB->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 15:
            mWipeWaveA->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 17:
            mWipeCircle->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 19:
            mWipeCurtain->calc(status, 1.0f, _0[kart].mColor);
            break;
        default:
#line 708
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 1:
    {
        f32 f = 1.0f + -1.0f * _0[kart]._8 / (_0[kart]._c - 1.0f);
        switch (wipeType)
        {
        case 1:
            break;
        case 3:
        case 5:
            mWipeSlide->calc(status, f, _0[kart].mColor);
            break;
        case 7:
            mWipeCheck->calc(status, f, _0[kart].mColor);
            break;
        case 9:
            mWipeShut->calc(status, f, _0[kart].mColor);
            break;
        case 11:
            mWipeLineA->calc(status, f, _0[kart].mColor);
            break;
        case 13:
            mWipeLineB->calc(status, f, _0[kart].mColor);
            break;
        case 15:
            mWipeWaveA->calc(status, f, _0[kart].mColor);
            break;
        case 17:
            mWipeCircle->calc(status, f, _0[kart].mColor);
            break;
        case 19:
            mWipeCurtain->calc(status, f, _0[kart].mColor);
            break;
        default:
#line 747
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 2:
    {
        switch (wipeType)
        {
        case 1:
            break;
        case 3:
        case 5:
            mWipeSlide->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 7:
            mWipeCheck->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 9:
            mWipeShut->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 11:
            mWipeLineA->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 13:
            mWipeLineB->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 15:
            mWipeWaveA->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 17:
            mWipeCircle->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 19:
            mWipeCurtain->calc(status, 0.0f, _0[kart].mColor);
            break;
        default:
#line 783
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 6:
        break;
    default:
        JUT_ASSERT(0);
        break;
    }
}

void WipeManager::calcOut(int kart, int status, int wipeType)
{
#line 806
    JUT_MINMAX_ASSERT(0, kart, 8);
    JUT_MINMAX_ASSERT(0, status, 4);

    switch (_0[kart].mWipeState)
    {
    case 3:
    {
        switch (wipeType)
        {
        case 2:
            break;
        case 4:
        case 6:
            mWipeSlide->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 8:
            mWipeCheck->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 10:
            mWipeShut->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 12:
            mWipeLineA->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 14:
            mWipeLineB->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 16:
            mWipeWaveA->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 18:
            mWipeCircle->calc(status, 0.0f, _0[kart].mColor);
            break;
        case 20:
            mWipeCurtain->calc(status, 0.0f, _0[kart].mColor);
            break;
        default:
#line 842
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 4:
    {
        f32 f = _0[kart]._8 / (_0[kart]._c - 1.0f);
        switch (wipeType)
        {
        case 2:
            break;
        case 4:
        case 6:
            mWipeSlide->calc(status, f, _0[kart].mColor);
            break;
        case 8:
            mWipeCheck->calc(status, f, _0[kart].mColor);
            break;
        case 10:
            mWipeShut->calc(status, f, _0[kart].mColor);
            break;
        case 12:
            mWipeLineA->calc(status, f, _0[kart].mColor);
            break;
        case 14:
            mWipeLineB->calc(status, f, _0[kart].mColor);
            break;
        case 16:
            mWipeWaveA->calc(status, f, _0[kart].mColor);
            break;
        case 18:
            mWipeCircle->calc(status, f, _0[kart].mColor);
            break;
        case 20:
            mWipeCurtain->calc(status, f, _0[kart].mColor);
            break;
        default:
#line 881
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 5:
    {
        switch (wipeType)
        {
        case 2:
            break;
        case 4:
        case 6:
            mWipeSlide->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 8:
            mWipeCheck->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 10:
            mWipeShut->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 12:
            mWipeLineA->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 14:
            mWipeLineB->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 16:
            mWipeWaveA->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 18:
            mWipeCircle->calc(status, 1.0f, _0[kart].mColor);
            break;
        case 20:
            mWipeCurtain->calc(status, 1.0f, _0[kart].mColor);
            break;
        default:
#line 917
            JUT_ASSERT(0);
            break;
        }
        break;
    }
    case 6:
        break;
    default:
#line 925
        JUT_ASSERT(0);
        break;
    }
}

int WipeManager::getWipeState(int kart)
{
#line 940
    JUT_MINMAX_ASSERT(0, kart, 8);
    return _0[kart].mWipeState;
}

void WipeManager::setWipe(int kart, int wipeType, short p3, short p4, short p5, JUtility::TColor color)
{
#line 964
    JUT_MINMAX_ASSERT(0, kart, 8);

    switch (wipeType)
    {
    case 5:
    case 6:
        mWipeSlide->reset();
        break;
    case 7:
    case 8:
        mWipeCheck->reset();
        break;
    case 9:
    case 10:
        mWipeShut->reset();
        break;
    case 11:
    case 12:
        mWipeLineA->reset();
        break;
    case 13:
    case 14:
        mWipeLineB->reset();
        // forgor break?
    case 15:
    case 16:
        mWipeWaveA->reset();
        break;
    case 17:
    case 18:
        mWipeCircle->reset();
        break;
    case 19:
    case 20:
        mWipeCurtain->reset();
        break;
    }

    _0[kart].setWipeType(wipeType);
    _0[kart].setPreMainAfter(p3, p4, p5);
    _0[kart].setColor(color);

    Task::TCB *cb = mTask->make(nullptr, 0x8000);
    if (cb)
    {
        cb->mExecIdx = kart;
        cb->_20 = wipeType;
        cb->_24 = p3;
        cb->_26 = p4;
        cb->_28 = p5;
        mTask->change(initPre, cb);
    }
}

void WipeManager::initPre()
{
    Task::TCB *cb = mTask->mCurCB;
    cb->_10 = 0;
    switch (cb->_20)
    {
    // in
    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
    case 17:
    case 19:
    {
        mThis->_0[cb->mExecIdx].mWipeState = 0;
        break;
    }
    // out
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
    case 16:
    case 18:
    case 20:
    {
        mThis->_0[cb->mExecIdx].mWipeState = 3;
        break;
    }
    default:
#line 1054
        JUT_ASSERT(0);
        break;
    }
    mTask->change(preWipe, mTask->mCurCB);
}

void WipeManager::preWipe()
{
    Task::TCB *cb = mTask->mCurCB;
    if (mKillFlag[cb->mExecIdx])
    {
        // probably inline?
        mThis->_0[cb->mExecIdx].mWipeType = 0;
        mThis->_0[cb->mExecIdx].mWipeState = 6;
        cb->mRequestKill = true;
        mKillFlag[cb->mExecIdx] = false;
    }
    if (!mMoveFlag[cb->mExecIdx])
        return;

    int unk = cb->_10++;
    if (unk < cb->_24)
    {
        mThis->_0[cb->mExecIdx]._8 = unk;
    }
    else
    {
        mTask->change(initMain, mTask->mCurCB);
    }
}

void WipeManager::initMain()
{
    Task::TCB *cb = mTask->mCurCB;
    cb->_10 = 0;
    switch (cb->_20)
    {
    // in
    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
    case 17:
    case 19:
    {
        mThis->_0[cb->mExecIdx].mWipeState = 1;
        break;
    }
    // out
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
    case 16:
    case 18:
    case 20:
    {
        mThis->_0[cb->mExecIdx].mWipeState = 4;
        break;
    }
    default:
#line 1113
        JUT_ASSERT(0);
        break;
    }
    mTask->change(mainWipe, mTask->mCurCB);
}

void WipeManager::mainWipe()
{
    Task::TCB *cb = mTask->mCurCB;
    if (mKillFlag[cb->mExecIdx])
    {
        // probably inline?
        mThis->_0[cb->mExecIdx].mWipeType = 0;
        mThis->_0[cb->mExecIdx].mWipeState = 6;
        cb->mRequestKill = true;
        mKillFlag[cb->mExecIdx] = false;
    }
    if (!mMoveFlag[cb->mExecIdx])
        return;

    int unk = cb->_10++;
    if (unk < cb->_26)
    {
        mThis->_0[cb->mExecIdx]._8 = unk;
    }
    else
    {
        mTask->change(initAfter, mTask->mCurCB);
    }
}

void WipeManager::initAfter()
{
    Task::TCB *cb = mTask->mCurCB;
    cb->_10 = 0;
    switch (cb->_20)
    {
    // in
    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
    case 17:
    case 19:
    {
        mThis->_0[cb->mExecIdx].mWipeState = 2;
        break;
    }
    // out
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
    case 16:
    case 18:
    case 20:
    {
        mThis->_0[cb->mExecIdx].mWipeState = 5;
        break;
    }
    default:
#line 1172
        JUT_ASSERT(0);
        break;
    }
    mTask->change(afterWipe, mTask->mCurCB);
}

void WipeManager::afterWipe()
{
    Task::TCB *cb = mTask->mCurCB;
    if (mKillFlag[cb->mExecIdx])
    {
        // probably inline?
        mThis->_0[cb->mExecIdx].mWipeType = 0;
        mThis->_0[cb->mExecIdx].mWipeState = 6;
        cb->mRequestKill = true;
        mKillFlag[cb->mExecIdx] = false;
    }
    if (!mMoveFlag[cb->mExecIdx])
        return;

    int unk = cb->_10++;
    if (unk < cb->_28)
    {
        mThis->_0[cb->mExecIdx]._8 = unk;
        return;
    }

    mThis->_0[cb->mExecIdx].mWipeType = 0;
    mThis->_0[cb->mExecIdx].mWipeState = 6;
    cb->mRequestKill = true;
}