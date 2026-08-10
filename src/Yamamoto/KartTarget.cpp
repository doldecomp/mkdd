#include "Kaneshige/RaceMgr.h"

#include "Yamamoto/KartTarget.h"
#include "Yamamoto/kartCtrl.h"

#include "JSystem/JAudio/JASFakeMatch2.h"

void KartTarget::Init()
{
    if (GetKartCtrl()->IsMiniGame())
        return;

    RaceMgr::getCurrentManager(); // i love calling functions for no reason!
    mCenter = RaceMgr::getManager()->getCourse()->getFirstCLPoint(0);
}

CLPoint *KartTarget::GetBrosTarget(int idx, f32 *pLen)
{
    CLPoint *brosPoint;
    KartBody *body = GetKartCtrl()->getKartBody(idx);
    CLPoint *target = mCenter;

    if (GetKartCtrl()->IsMiniGame())
        return nullptr;

    JGeometry::TVec3f diff;
    JGeometry::TVec3f pntPos;

    f32 lenMax = 100000.0f;
    int pointCnt = mCenter->getBrosPointNumber();

    if (pointCnt == 0)
    {
        *pLen = 100000.0f;
        return mCenter;
    }

    for (int i = 0; i < pointCnt; i++)
    {
        brosPoint = mCenter->getBrosPoint(i);
        brosPoint->getPosition(&pntPos);
        diff.sub(body->mPos, pntPos);
        f32 len = diff.length();
        if (lenMax > len || i == 0)
        {
            lenMax = len;
            target = brosPoint;
        }
    }
    *pLen = lenMax;
    return target;
}

CLPoint *KartTarget::GetNextTarget(int idx, f32 *pLen)
{
    CLPoint *brosPoint;
    KartBody *body = GetKartCtrl()->getKartBody(idx);
    CLPoint *target;

    if (GetKartCtrl()->IsMiniGame())
        return nullptr;

    JGeometry::TVec3f diff;
    JGeometry::TVec3f pntPos;

    f32 lenMax;
    int pointCnt = mCenter->getNextPointNumber();

    for (int i = 0; i < pointCnt; i++)
    {
        brosPoint = mCenter->getNextPoint(i);
        brosPoint->getPosition(&pntPos);
        diff.sub(body->mPos, pntPos);
        f32 len = diff.length();
        if (lenMax > len || i == 0)
        {
            lenMax = len;
            target = brosPoint;
        }
    }
    *pLen = lenMax;
    return target;
}

CLPoint *KartTarget::GetBeforeTarget(int idx, f32 *pLen)
{
    CLPoint *brosPoint;
    KartBody *body = GetKartCtrl()->getKartBody(idx);
    CLPoint *target;

    if (GetKartCtrl()->IsMiniGame())
        return nullptr;

    JGeometry::TVec3f diff;
    JGeometry::TVec3f pntPos;

    f32 lenMax;
    int pointCnt = mCenter->getPrevPointNumber();

    for (int i = 0; i < pointCnt; i++)
    {
        brosPoint = mCenter->getPrevPoint(i);
        brosPoint->getPosition(&pntPos);
        diff.sub(body->mPos, pntPos);
        f32 len = diff.length();
        if (lenMax > len || i == 0)
        {
            lenMax = len;
            target = brosPoint;
        }
    }
    *pLen = lenMax;
    return target;
}

void KartTarget::DoSearchTarget(int idx)
{
    JGeometry::TVec3f pntPos;
    JGeometry::TVec3f diff;

    KartBody *body = GetKartCtrl()->getKartBody(idx);

    if (GetKartCtrl()->IsMiniGame())
        return;

    f32 lenNext, lenPrev, lenBros;

    CLPoint *next = GetNextTarget(idx, &lenNext);
    CLPoint *prev = GetBeforeTarget(idx, &lenPrev);
    CLPoint *bros = GetBrosTarget(idx, &lenBros);

    mCenter->getPosition(&pntPos);
    diff.sub(body->mPos, pntPos);
    f32 len = diff.length();

    if (len > lenNext && lenPrev > lenNext && lenBros > lenNext)
        mCenter = next;
    else if (len > lenPrev && lenNext > lenPrev && lenBros > lenPrev)
        mCenter = prev;
    else if (len > lenBros && lenNext > lenBros && lenPrev > lenBros)
        mCenter = bros;
}

void KartTarget::DoSearchRescueTarget(CLPoint *pnt)
{
    if (GetKartCtrl()->IsMiniGame())
        return;

    mCenter = pnt;
}
