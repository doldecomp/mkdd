#include "Kaneshige/Objects/GeoCar.h"
#include "JSystem/JGeometry/Matrix.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JMath/JMath.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kaneshige/Course/CrsArea.h"
#include "Kaneshige/Course/CrsData.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/DarkAnmMgr.h"
#include "Kaneshige/Objects/GeoSignal.h"
#include "Kaneshige/RaceMgr.h"
#include "Sato/GeographyObj.h"
#include "Sato/ObjUtility.h"
#include "Sato/StateObserver.h"
#include "Yamamoto/kartCtrl.h"
#include "math.h"
#include "types.h"

u16 GeoCar::sSupervisorCreateNum;
GeoCarSupervisor *GeoCar::sSupervisor;

f32 GeoCar::sHornDistance = 1200.0f;
f32 GeoCar::sHornDot = 0.5f;
f32 GeoCar::sAccValue = 3.0f;
f32 GeoCar::sGravity = 10.0f;
f32 GeoCar::sDamageGravity = 2.0f;
f32 GeoCar::sAirResistance = 0.95f; // 95% rule!!
f32 GeoCar::sHandlePower = 3.0f;
f32 GeoCar::sTargetRadius = 1000.0f;
f32 GeoCar::sTargetAmp = 1000.0f;
f32 GeoCar::sSafeDistance = 2000.0f;
f32 GeoCar::sDamageVel0 = 60.0f;
f32 GeoCar::sDamageShakeAmp = 0.07f;
s16 GeoCar::sDamageShakeVel = 3400;

StateObserver::StateFuncSet<GeoCar> GeoCar::sTable[6] = {
    {0, &GeoCar::initFuncParking, &GeoCar::doFuncParking},
    {1, &GeoCar::initFuncMove, &GeoCar::doFuncMove},
    {2, &GeoCar::initFuncDamage, &GeoCar::doFuncDamage},
    {3, &GeoCar::initFuncRecover, &GeoCar::doFuncRecover},
    {4, &GeoCar::initFuncDeath, &GeoCar::doFuncDeath},
    {5, &GeoCar::initFuncWait, &GeoCar::doFuncWait},
};

GeoCarSupervisor::GeoCarSupervisor() {
    mEntryId = 0;
    mList.initiate();
}

GeoCarSupervisor::~GeoCarSupervisor() {
    GeoCar::sSupervisor = nullptr;
}

void GeoCarSupervisor::reset() {}

void GeoCarSupervisor::calc() {
    for (JSUListIterator<GeoCar> it(mList.getFirst()); it != mList.getEnd(); it++) {
        it->mStateReq = 0;
    }

    for (JSUListIterator<GeoCar> it(mList.getFirst()); it != mList.getEnd(); it++) {
#line 454
        JUT_ASSERT_MSG(it->mCarID != -1, "NONSET CARID");
        if (it->tstCarCollision()) {
            JSUListIterator<GeoCar> it2(it);
            it2.goNext();
            for (; it2 != mList.getEnd(); it2++) {
                if (it->mCarID != it2->mCarID) {
                    if (it2->tstCarCollision()) {
                        if (it->getPathID() == it2->getPathID()) {
                            JGeometry::TVec3f dist;
                            dist.x = it2->mPos.x - it->mPos.x;
                            dist.y = it2->mPos.y - it->mPos.y;
                            dist.z = it2->mPos.z - it->mPos.z;
                            if (dist.squared() < (GeoCar::sSafeDistance * GeoCar::sSafeDistance)) {
                                dist.normalize();
                                if (it->mFrDir.dot(dist) >= 0.0f) {
                                    it->sendStateReqSlowdown();
                                }
                                else {
                                    it2->sendStateReqSlowdown();
                                }
                            }
                        }
                        if (it2->IsHitObject(it->mPos, it->getBounds(0))) {
                            if (it->isBlast()) {
                                it2->sendStateReqExplosion();
                            }
                            if (it2->isBlast()) {
                                it->sendStateReqExplosion();
                            }
                        }
                    }
                }
            }
        }
    }
}

void GeoCarSupervisor::entryCar(GeoCar *car, bool p2) {
    mList.append(&car->mLink);
    if (p2) {
        car->mCarID = mEntryId;
        mEntryId++; 
    }
}

int GeoCarSupervisor::getCarNumber() {
    int num = 0;
    for (JSUListIterator<GeoCar> it(mList.getFirst()); it != mList.getEnd(); it++) {
        num++;
    }
    return num;
}

void GeoCarSupervisor::hideAllCar() {
    for (JSUListIterator<GeoCar> it(mList.getFirst()); it != mList.getEnd(); it++) {
        it->setObjFlagHidding();
    }
}

GeoCar::GeoCar(u32 objID, bool p2) : GeographyObj(objID), mLink(this) {
    init(p2);
}

GeoCar::GeoCar(const CrsData::SObject &obj) : GeographyObj(obj), mLink(this) {
    init(true);
}

void GeoCar::init(bool p1) {
    if (sSupervisorCreateNum == 0) {
        sSupervisor = new GeoCarSupervisor();
        sSupervisorCreateNum++;
    }

    switch (GeographyObj::getKind()) {
    case 0xfa1:
    case 0xfa5:
    case 0xfa6: {
        mNpcSoundMgr = new GameAudio::NpcCarSoundMgr(&mPos, 0, nullptr);
        break;
    }
    case 0xfa3: {
        mNpcSoundMgr = new GameAudio::NpcCarSoundMgr(&mPos, 1, nullptr);
        break;
    }
    case 0xfa2: {
        mNpcSoundMgr = new GameAudio::NpcCarSoundMgr(&mPos, 2, nullptr);
        break;
    }
    case 0xfa9: {
        mNpcSoundMgr = new GameAudio::NpcCarSoundMgr(&mPos, 3, nullptr);
        break;
    }
    default: {
        mNpcSoundMgr = nullptr;
        break;
    }
    }

    switch (GeographyObj::getKind()) {
    case 0xfa9:
    case 0xfaa: {
        GeographyObj::setItemColReaction(7, 2);
        break;
    }
    default: {
        GeographyObj::setItemColReaction(7, 0);
        break;
    }
    }
    GeographyObj::setItemColReaction(5, 0);

    _158 = 0.0f;
    mZigZagRate = 0.0f;
    if (mObjData) {
#line 641
        JUT_ASSERT_F(mObjData->mParam2 >= 0 && mObjData->mParam2 <= 100, "ZIGZAG RATE OVER(%d)", mObjData->mParam2)
        mZigZagRate = mObjData->mParam2 / 100.0f;
    }
    _160 = 0.0f;
    mFrScale.zero();
    mBcScale.zero();
    mCarID = -1;
    mAccValue = 0.0f;
    _1e0 = 0;
    mStateReq = 0;
    switch (GeographyObj::getKind()) {
    case 0xfaa:
        mShadowArea = nullptr;
        break;
    default: {
        mShadowArea = new CrsArea();
    }
    }
    mDarkAnm = new ObjDarkAnmPlayer(GeographyObj::getKind());


    sSupervisor->entryCar(this, p1);
}

static void calcOffsetPosition(JGeometry::TVec3f *out, JGeometry::TVec3f &basePos, JGeometry::TVec3f &scale, JGeometry::TVec3f &dirX, JGeometry::TVec3f &dirY, JGeometry::TVec3f &dirZ) {
    out->scaleAdd(scale.x, dirX, basePos);
    out->scaleAdd(scale.y, dirY, *out);
    out->scaleAdd(scale.z, dirZ, *out);
}

void GeoCar::calcFrPos() {
    calcOffsetPosition(&mFrPos, mBasePos, mFrScale, mLfDir, mUpDir, mFrDir);
}

void GeoCar::calcBcPos() {
    calcOffsetPosition(&mBcPos, mBasePos, mBcScale, mLfDir, mUpDir, mFrDir);
}

void GeoCar::reset() {
    GeographyObj::resetObject();
    StateObserver::ResetState();
    _58 = 1;
    _1e0 |= 1;
    if (mObjData) {
        mFrDir.set(mObjData->zNorm);
        mFrDir.scale(0.0001f);
        mUpDir.set(mObjData->yNorm);
        mUpDir.scale(0.0001f);
    }
    else {
        mFrDir.set(0.0f, 0.0f, 1.0f);
        mUpDir.set(0.0f, 1.0f, 0.0f);
    }
    mLfDir.cross(mUpDir, mFrDir);
    _1b0.zero();
    mAccValue = sAccValue;

    if (mObjData && mObjData->mParam5 != 0) {
        mTargetRadius = mObjData->mParam5;
    }
    else {
        mTargetRadius = sTargetRadius;
    }

    if (mObjData && mObjData->mParam6 != 0) {
        mHandlePower = mObjData->mParam6;
        
    }
    else {
        mHandlePower = sHandlePower;
    }

    _1e0 = 0;
    mStateReq = 0;
    mBasePos.set(mPos);
    calcFrPos();
    calcBcPos();
    _188 = 0.0f;
    mMoveAccValue = 0.0f;
    _200.zero();
    for (int i = 0; i < 4; i++) {
        _20c[i].zero();
    }

    mPointIdx = 0;
    _232 = 0;
    mDamageShakeVel = 0;
    mDamageShakeAmp = 0.0f;
    _23c.set(mFrDir);
    mDamagePos.zero();
    _254 = 0;
    setAllObjectCollision(true);
    setCarCollision();
    if (mObjData && GeographyObj::IsPathMove()) {
#line 789
        StateObserver::setState(1);
        JUT_ASSERT_MSG(mObjData->pointIdx != 0xffff, "CAR UNLINK POINT");
        setTargetPoint(mObjData->pointIdx);
    }
    else {
        StateObserver::setState(0);
    }

    if (mShadowArea) {
        mShadowArea->reset();
    }
    mDarkAnm->reset();
}

void GeoCar::setTargetPoint(u16 targetId) {
    CrsData *crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    u16 pointId = targetId; // probably fake
    CrsData::PathData *pathData = crsData->getPathData(mObjData->mPathID);
   
    if (pointId == 0xffff) {
        pointId = 0; 
    }
    else if (pointId == pathData->getPointNumber() - 1) {
        pointId--;
    }

    u16 nextPointId = pointId + 1;
    mPointIdx = nextPointId;

    mBasePos = crsData->getPointData(mObjData->mPathID, pointId)->pos;
    
    JGeometry::TVec3f v;
    v = crsData->getPointData(mObjData->mPathID, nextPointId)->pos;

    CrsGround ground(nullptr);
    ground.search(mBasePos);
    mBasePos.y = ground.getHeight();
    f32 tirePosY = -getTirePosOffset(0).y;
    mBasePos.y += tirePosY;

    mFrDir.sub(v, mBasePos);
    mFrDir.normalize();
    ground.getNormal(&mUpDir);

    mLfDir.cross(mUpDir, mFrDir);
    mLfDir.normalize();
    resetDrivePower();
    calcFrontAndBackPosition();
    _1b0.set(mFrDir);
    _1b0.scale(_160);
}

void GeoCar::resetDrivePower() {
    mMoveAccValue = 0.0f;
    _200.set(mFrDir);
    _200.zero(); // code so big brain only Kaneshige understands
}

void GeoCar::calcFrontAndBackPosition() {
    calcFrPos();
    calcBcPos();
}

void GeoCar::createModel(JKRSolidHeap *slid, u32 p2) {
    mModel.createModel(slid, p2, 0);
}

bool GeoCar::isHorn() {
    bool honk = false;
    for (int i = 0; i < RaceMgr::getManager()->getKartNumber(); i++) {
        if (RaceMgr::getManager()->getKartInfo(i)->isRealPlayerKart()) {
            JGeometry::TVec3f kartPos;
            ObjUtility::getKartPos(i, &kartPos);
            JGeometry::TVec3f dist;
            dist.sub(kartPos, mPos);
            f32 sq = dist .squared();
            if (sq > 0.0f && sq < (sHornDistance * sHornDistance)) {
                dist.normalize();
                if (dist.dot(mFrDir) >= sHornDot) {
                    honk = true;
                    break;
                }
            }
        }
    }
    return honk;
}

void GeoCar::calc() {
    if (!isDeath()) {
        recvStateRequest();
        checkKartCollision();
        checkItemCollision();
    }

    StateObserver::ExecuteState();
    if (GeographyObj::IsPathMove()) {
        checkTargetPoint();
    }
    mPos.set(mBasePos);
    mPos.scaleAdd(_188, mUpDir, mPos);
    if (mShadowArea) {
        mShadowArea->searchShadow(mPos);
        mDarkAnm->calc(*mShadowArea);
    }

    setRMtx();
    if (mNpcSoundMgr) {
        f32 l = _1b0.length();
        bool honk = isHorn();
        mNpcSoundMgr->frameWork(l, honk);
        if (_254 > 0) {
            _254--;
        }
    }
}

void GeoCar::setRMtx() {
    mRotMtx.setXYZDir(mLfDir, mUpDir, mFrDir);
}

void GeoCar::setTevColor() {
    mDarkAnm->setTevColor(&mModel);
}

void GeoCar::recvStateRequest() {
    if (recvStateReqExplosion()) {
        hitItemBomb();
    }
}

void GeoCar::checkKartCollision() {
    for (int i = 0; i < RaceMgr::getManager()->getKartNumber(); i++) {
        if (GeographyObj::tstIsHitKartFlg(i)) {
            hitKart(i);
            if (isHitVibrationOn()) {
                getVibrationLimitSpeed();
                GetKartCtrl()->GetCarSpeed(i);
            } 
        }
    }
}

void GeoCar::hitKart(int kartId) {
    if (KartCtrl::getKartCtrl()->GetKartStatus(kartId) & 0x40000) {
        StateObserver::setState(2);
    }
    RaceMgr::getManager()->getKartInfo(kartId)->isPlayerKart();
}

u32 GeoCar::getSoundID() const { return 0x10017; }

void GeoCar::setCrushSE(u32 soundId) {
    if (mNpcSoundMgr && _254 <= 0) {
        mNpcSoundMgr->setSe(soundId);
        _254 = 10;
    }
}

void GeoCar::checkItemCollision() {
    if (GeographyObj::tstItemHitting()) {
        switch (GeographyObj::getColItemObj()->getKind()) {
        case 8: {
            if (GeographyObj::getColItemObj()->getKartReaction() == 8) {
                hitItemBomb();
            }
            break;
        }
        case 7: {
            hitItemWanWan();
            break;
        }
        case 1: {
            hitItemGTurtleBig();
            break;
        }
        case 0: {
            hitItemGTurtle();
            break;
        }
        case 2: {
            hitItemRTurtle();
            break;
        }
        case 9: {
            hitItemFireBall();
            break;
        }
        case 11: {
            hitItemYoshiEgg();
            break;
        }
        }
    }
}


void GeoCar::hitItemBomb() {
    StateObserver::setState(2);
}

void GeoCar::hitItemFireBall() {}

void GeoCar::hitItemWanWan() {
    StateObserver::setState(2);
}

void GeoCar::hitItemGTurtleBig() {
    StateObserver::setState(2);
}

void GeoCar::InitExec() { Observer_FindAndInit(GeoCar, 6); }

void GeoCar::MoveExec() { Observer_FindAndExec(GeoCar, 6); }

void GeoCar::getTargetPosition(JGeometry::TVec3f *out) {
    CrsData *crsData = RaceMgr::getManager()->getCourse()->getCrsData();
    CrsData::PointData *point = crsData->getPointData(mObjData->mPathID, mPointIdx);
    out->set(point->pos);
    if (mPointIdx != 0) {
        CrsData::PointData *prevPoint = crsData->getPointData(mObjData->mPathID, mPointIdx - 1);
        JGeometry::TVec3f up(0.0f, 1.0f, 0.0f);
        JGeometry::TVec3f dist;
        dist.sub(point->pos, prevPoint->pos);
        dist.normalize();

        JGeometry::TVec3f lateralDir;
        lateralDir.cross(up, dist);
        lateralDir.normalize();
        out->scaleAdd(sTargetAmp * mZigZagRate * JMASSin(_232), lateralDir, *out);
    }
}

void GeoCar::getPointLinkSignal() {
    GeoSignal::getSupervisor();
}

void GeoCar::setAllObjectCollision(bool doSet) {
    if (doSet) {
        GeographyObj::setObjFlagCheckItemHitting();
        GeographyObj::setAllCheckKartHitFlag();
    }
    else {
        GeographyObj::clrObjFlagCheckItemHitting();
        GeographyObj::clrAllCheckKartHitFlag();
    }
}

u16 GeoCar::getPathID() {
    u16 ret = 0xffff;
    if (mObjData)
        ret = mObjData->mPathID;
    return ret;
}

f32 GeoCar::getTireAnmRate() {
    return getTireAnmRate(_1b0.length());
}

f32 GeoCar::getTireAnmRate(f32 length) {
    return (length / 60.0f) * 2.0f;
}

void GeoCar::initFuncParking() {}

void GeoCar::doFuncParking() {}

void GeoCar::initFuncMove() {
    resetDrivePower();
    calcFrontAndBackPosition();
}

void GeoCar::doFuncMove() {
    _232 += 0x200;
    JGeometry::TVec3f targetPos;
    getTargetPosition(&targetPos);

    JGeometry::TVec3f targetDiff;
    targetDiff.sub(targetPos, mFrPos);

    f32 zLen = mLfDir.dot(targetDiff);
    f32 xLen = mFrDir.dot(targetDiff);
    f32 xzVel = JMAFastSqrt((zLen*zLen)+(xLen*xLen));

    if (xzVel != 0.0f) {
        zLen *= (1.0f/xzVel);
    }
    f32 absZLen = JMAAbs(zLen);
    _200.zero();
    _200.scaleAdd(zLen * mHandlePower, mLfDir, _200);

    f32 len = _1b0.length();
    f32 someScale = 1.0f;

    if (absZLen < 0.4f) {
        if (len > _160) {
            mMoveAccValue = 0.0f;
        }
        else if (!recvStateReqSlowdown()) {
            mMoveAccValue = sAccValue;
        }
        else {
            someScale = 0.95f; // 95% rule
            mMoveAccValue = 0.0f;
        }
    }
    else if (!recvStateReqSlowdown()) {
        mMoveAccValue = sAccValue * 0.5f;
    }
    else {
        someScale = 0.95f; // 95% rule
        mMoveAccValue = 0.0f;
    }

    _200.scaleAdd(mMoveAccValue, mFrDir, _200);
    _1b0.scale(sAirResistance);
    _1b0.scale(someScale);
    _1b0.add(_200);

    f32 len2 = _1b0.length();
    if (len2 > 0.0f && len2 > _160) {
        _1b0.normalize();
        _1b0.scale(_160);
    }

    JGeometry::TVec3f frPos;
    frPos.add(mFrPos, _1b0);

    JGeometry::TVec3f frDir;
    frDir.sub(frPos, mBcPos);
    frDir.normalize();
    
    JGeometry::TVec3f lfDir;
    lfDir.cross(mUpDir, frDir);
    lfDir.normalize();

    JGeometry::TVec3f frScale(mFrScale);
    frScale.scale(-1.0f);

    JGeometry::TVec3f basePos;
    calcOffsetPosition(&basePos, frPos, frScale, lfDir, mUpDir, frDir);

    JGeometry::TVec3f bcPos;
    calcOffsetPosition(&bcPos, basePos, mBcScale, lfDir, mUpDir, frDir);
    calcNewPose(&frDir, &mUpDir, &lfDir, &basePos);
    calcOffsetPosition(&frPos, basePos, mFrScale, lfDir, mUpDir, frDir);
    calcOffsetPosition(&bcPos, basePos, mBcScale, lfDir, mUpDir, frDir);

    mBasePos.set(basePos);
    mFrPos.set(frPos);
    mBcPos.set(bcPos);
    mLfDir.set(lfDir);
    mFrDir.set(frDir);
}

void GeoCar::checkTargetPoint() {
    CrsData *crsData = RaceMgr::getManager()->getCourse()->getCrsData();

    JGeometry::TVec3f pointPos;
    pointPos.set(crsData->getPointData(mObjData->mPathID, mPointIdx)->pos);

    JGeometry::TVec3f dist;
    dist.sub(pointPos, mPos);
    if (dist.length() <= mTargetRadius) {
        CrsData::PathData *path = RCMGetCourse()->getCrsData()->getPathData(mObjData->mPathID);
        u16 pointCount = path->pointCount;
        mPointIdx++;
        if (mPointIdx < pointCount)
            return;

        mPointIdx = 0;
    
        if (path->isClosed())
            return;
    
        CrsData::PointData *lastPoint = RCMGetCourse()->getCrsData()->getPointData(mObjData->mPathID, pointCount - 1);
        
        if (lastPoint->linkPoint) {
            u16 linkPointId = 0xffff;            
            for (u16 i = 0; i < (u16)pointCount; i++) {
                //CrsData::PointData *point = RCMGetCourse()->getCrsData()->getPointData(mObjData->mPathID, i);
                if (RCMGetCourse()->getCrsData()->getPointData(mObjData->mPathID, i)->linkPoint == lastPoint->linkPoint) {
                    linkPointId = i;
                    break;
                }                
            }
#line 1609
            JUT_ASSERT_F(linkPointId != 0xffff, "GEOCAR:CANNOT FIND LINKPOINT:%d", lastPoint->linkPoint);
            mPointIdx = linkPointId;
        }
        else {
            setTargetPoint(mPointIdx);
        }
    }
}


void GeoCar::calcNewPose(JGeometry::TVec3f *pFrDir, JGeometry::TVec3f *pUpDir, JGeometry::TVec3f *pLfDir, JGeometry::TVec3f *pPos) {
    JGeometry::TVec3f positions[3];
    // Apply forces
    for (int i = 0, offset = 0; i < 3; i++, offset += 12) {
        _20c[i].scale(sAirResistance);
        _20c[i].y -= sGravity;
        
        positions[i].scaleAdd(getTirePosOffset(i).x, *pLfDir, *pPos);
        positions[i].scaleAdd(getTirePosOffset(i).y, *pUpDir, positions[i]);
        positions[i].scaleAdd(getTirePosOffset(i).z, *pFrDir, positions[i]);
        positions[i].add(_20c[i]);
    }
    
    CrsGround grounds[3];
    // Ground collision handling
    for (int i = 0; i < 3; i++) {
        grounds[i].search(positions[i], positions[i]);
        if (positions[i].y < grounds[i].getHeight()) {
            positions[i].y = grounds[i].getHeight();
            _20c[i].y = 0.0f;
        }
    }
    
    // Compute new direction vectors
    pLfDir->sub(positions[0], positions[1]);
    pLfDir->normalize();
    pFrDir->sub(positions[0], positions[2]);
    pFrDir->normalize();
    pUpDir->cross(*pFrDir, *pLfDir);
    pUpDir->normalize();
    
    // Compute new position
    pPos->x = (positions[0].x - (pLfDir->x * getTirePosOffset(0).x) - pUpDir->x * getTirePosOffset(0).y - pFrDir->x * getTirePosOffset(0).z);
    pPos->y = (positions[0].y - (pLfDir->y * getTirePosOffset(0).x) - pUpDir->y * getTirePosOffset(0).y - pFrDir->y * getTirePosOffset(0).z);
    pPos->z = (positions[0].z - (pLfDir->z * getTirePosOffset(0).x) - pUpDir->z * getTirePosOffset(0).y - pFrDir->z * getTirePosOffset(0).z);

    // Compute front and back midpoints
    JGeometry::TVec3f front;
    front.add(getTirePosOffset(0), getTirePosOffset(1));
    front.scale(0.5f);

    JGeometry::TVec3f back;
    back.add(getTirePosOffset(2), getTirePosOffset(3));
    back.scale(0.5f);
}

void GeoCar::initFuncDamage() {
    setAllObjectCollision(false);
    mDamagePos.set(0.0f, sDamageVel0, 0.0f);
    mDamageShakeVel = GeographyObj::getGeoRnd()->getRandom();

    if (mDamageShakeVel & 1) {
        mDamageShakeAmp = sDamageShakeAmp;
    }
    else {
        mDamageShakeAmp = -sDamageShakeAmp;
    }
    _23c.set(mFrDir);
}

void GeoCar::doFuncDamage() {
    f32 yScale = mDamageShakeAmp * JMASSin(mDamageShakeVel);
    f32 zScale = mDamageShakeAmp * JMASCos(mDamageShakeVel);

    mDamageShakeVel += sDamageShakeVel;

    mFrDir.scaleAdd(yScale, mUpDir, mFrDir);
    mFrDir.scaleAdd(zScale, mLfDir, mFrDir);
    mFrDir.normalize();
    mUpDir.cross(mFrDir, mLfDir);
    mUpDir.normalize();
    mLfDir.cross(mUpDir, mFrDir);
    //mLfDir.normalize();

    mDamagePos.y -= sDamageGravity;
    JGeometry::TVec3f newBasePos;
    newBasePos.add(mBasePos, mDamagePos);

    CrsArea area;
    area.searchRoof(newBasePos);
    if (area.isInside()) {
        JGeometry::TVec3f roofPos;
        area.getRoofPosition(&roofPos);
        if (newBasePos.y + _158 > roofPos.y) {
            mDamagePos.y = 0.0f;
            newBasePos.y = roofPos.y - _158;
        }
    }

    CrsGround ground(nullptr);
    ground.exceptValley(true);
    ground.search(newBasePos, mBasePos);
    if (newBasePos.y <= ground.getHeight()) {
        newBasePos.y = ground.getHeight() + 50.0f;
        mFrDir.set(_23c);
        ground.getNormal(&mUpDir);
        mLfDir.cross(mUpDir, mFrDir);
        mLfDir.normalize();
        mFrDir.cross(mLfDir, mUpDir);
        mFrDir.normalize();
        StateObserver::setState(3);
    }
    mBasePos.set(newBasePos);
}

void GeoCar::initFuncRecover() {
    calcFrontAndBackPosition();
}

void GeoCar::doFuncRecover() {
    if (GeographyObj::IsPathMove()) {
        StateObserver::setState(1);
        setAllObjectCollision(true);
    }
    else {
        GeographyObj::setObjFlagCheckItemHitting();
        StateObserver::setState(0);
    }
}

void GeoCar::initFuncDeath() {
    initCallBackDeath();
}

void GeoCar::doFuncDeath() {
    doCallBackDeath();
}

void GeoCar::initFuncWait() {}

void GeoCar::doFuncWait() {
    if (mVel.length() > 5.0f) {
        mMoveAccValue = -0.1f;
    }
    else {
        mMoveAccValue = 0.0f;
    }
}

#include "JSystem/JAudio/JASFakeMatch2.h"
