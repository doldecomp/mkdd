#include "Inagaki/GameSoundMgr.h"
#include "Kaneshige/Course/CrsGround.h"
#include "Kaneshige/RaceMgr.h"
#include "Kawano/driver.h"
#include "Osako/kartPad.h"
#include "Sato/ItemObjMgr.h"
#include "Yamamoto/kartBody.h"
#include "Yamamoto/kartCtrl.h"

#include "Jsystem/JAudio/JASFakeMatch2.h"
#include "Yamamoto/kartParams.h"
#include "kartEnums.h"

void KartCtrl::SetPadClear(KartGamePad *kartGamePad) {
    kartGamePad->mButtons.mButton = 0;
    kartGamePad->mButtons.mTrigger = 0;
    kartGamePad->mButtons.mRelease = 0;
    kartGamePad->mMainStick.mX = 0.0f;
    kartGamePad->mMainStick.mY = 0.0f;
    kartGamePad->mSubStick.mX = 0.0f;
    kartGamePad->mSubStick.mY = 0.0f;
}

void KartCtrl::SetWanWanPadClear(KartGamePad *kartGamePad) {
    kartGamePad->mButtons.mButton = 0;
    kartGamePad->mButtons.mTrigger = 0;
    kartGamePad->mButtons.mRelease = 0;
    kartGamePad->mMainStick.mY = 0.0f;
    kartGamePad->mSubStick.mX = 0.0f;
    kartGamePad->mSubStick.mY = 0.0f;
}

void KartCtrl::SetGhostPadClear(KartGamePad *kartGamePad) {
    kartGamePad->mButtons.clear();
    kartGamePad->mMainStick.clear();
    kartGamePad->mSubStick.clear();
}

void KartCtrl::PadAllClear(int gamePadIndex) {
    KartBody *kartBody = getKartBody(gamePadIndex);
    // Clear First Kart Player (Driver) Game Pad.
    SetPadClear(getPad(gamePadIndex));
    if ((kartBody->mGameStatus & 1) != 0) {
        // Clear Second Kart Player (Passenger) Game Pad.
        SetPadClear(getCoPad(gamePadIndex));
    }
}

void KartCtrl::DoContCtl(int index) {    
    KartBody *kartBody = getKartBody(index);
    if (RaceMgr::getCurrentManager()->isMirror() != 0) {
        KartGamePad *kartGamePad = GetDriveCont(index);
        kartGamePad->mMainStick.mX = -(kartGamePad->mMainStick.mX);
        if ((kartBody->mGameStatus & 1) != 0) {
            kartGamePad = GetCoDriveCont(index);
            kartGamePad->mMainStick.mX = -(kartGamePad->mMainStick.mX);
        }
    }
    DoContCopy(index);

    if (kartBody->getChecker()->CheckAllClearKey()) {
        KartGamePad *driverCont = GetDriveCont(index);
        SetPadClear(driverCont);
        KartGamePad *coDriverCont = GetCoDriveCont(index);
        SetPadClear(coDriverCont);
        getKartEnemy(index)->ContempLate();
    }

    kartBody->getHandle()->WatchFrontalCollisionForce();
    if ((kartBody->mCarStatus & 0x1000) != 0) {
        KartGamePad *driverCont = GetDriveCont(index);
        SetWanWanPadClear(driverCont);
        KartGamePad *coDriverCont = GetCoDriveCont(index);
        SetWanWanPadClear(coDriverCont);
    }
    else if (kartBody->getChecker()->CheckAllClearKeyT()) {
        KartGamePad *driverCont = GetDriveCont(index);
        SetPadClear(driverCont);
        KartGamePad *coDriverCont = GetCoDriveCont(index);
        SetPadClear(coDriverCont);
    }
}

void KartCtrl::DoContCopy(int index) {
    getKartPad(index)->mDriverButton = GetDriveCont(index)->mButtons.mButton;
    getKartPad(index)->mCoDriverButton = GetCoDriveCont(index)->mButtons.mButton;

    getKartPad(index)->mDriverTrigger = GetDriveCont(index)->mButtons.mTrigger;
    getKartPad(index)->mCoDriverTrigger = GetCoDriveCont(index)->mButtons.mTrigger;

    getKartPad(index)->mDriverRelease = GetDriveCont(index)->mButtons.mRelease;
    getKartPad(index)->mCoDriverRelease = GetCoDriveCont(index)->mButtons.mRelease;
}

void KartCtrl::DoContPaste(int index) {
    GetDriveCont(index)->mButtons.mButton = getKartPad(index)->mDriverButton;
    GetCoDriveCont(index)->mButtons.mButton = getKartPad(index)->mCoDriverButton;

    GetDriveCont(index)->mButtons.mTrigger = getKartPad(index)->mDriverTrigger;
    GetCoDriveCont(index)->mButtons.mTrigger = getKartPad(index)->mCoDriverTrigger;

    GetDriveCont(index)->mButtons.mRelease = getKartPad(index)->mDriverRelease;
    GetCoDriveCont(index)->mButtons.mRelease = getKartPad(index)->mCoDriverRelease;
}

f32 KartCtrl::GetItemStickY(int kartPadIndex) {
    return getKartPad(kartPadIndex)->mItemStickY;
}

f32 KartCtrl::GetItemStickX(int kartIndex) {
    if ((getKartBody(kartIndex)->mGameStatus & 0x1) != 0) {
        return 0.0f;
    }
    return 0.0f;
}

u32 KartCtrl::GetItemButton(int kartIndex) {
    return (getKartBody(kartIndex)->mGameStatus & 1) != 0
        ? getKartPad(kartIndex)->mFaceBtnsMask
        : getKartPad(kartIndex)->mItemMask;
}

void KartCtrl::DoLod() {
    int kartNumber = GetKartCtrl()->GetKartNumber();
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();
    JGeometry::TVec3f kartDistToCamera;

    for (u16 j = 0; j < raceMgr->getCameraNumber(); j++) {
        KartCam *kartCam = getKartCam(j);
        kartCam->SetVictoryScreenPort(j);
        for (int i = 0; i < kartNumber; i++) {
            KartBody *kartBody = getKartBody(i);
            KartDrawer *kartDrawer = RaceMgr::getManager()->getKartDrawer(i);
            raceMgr->getKartInfo(i)->getKartID();

            if (getKartBody(i)->getChecker()->CheckIndication() != 0) {
                f32 clipperScale = kartCam->GetLodLen();
                if (kartCam->GetCameraMode() == 2) {
                    clipperScale = 18000.0f;
                }

                kartDistToCamera.sub(kartBody->mPos, *kartCam->GetCameraPos());
                if (kartDistToCamera.x > clipperScale || kartDistToCamera.x < -clipperScale) {
                    kartDrawer->setLODLevel(j, 1);
                } else if (kartDistToCamera.y > clipperScale || kartDistToCamera.y < -clipperScale) {
                    kartDrawer->setLODLevel(j, 1);
                } else if (kartDistToCamera.z > clipperScale || kartDistToCamera.z < -clipperScale) {
                    kartDrawer->setLODLevel(j, 1);
                } else {
                    kartDrawer->setLODLevel(j, 0);
                }

                clipperScale = kartCam->GetClipperScale();
                kartBody->mBodyModel->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);

                clipperScale = 0.85f * kartCam->GetClipperScale();
                kartBody->mExModels[0]->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                kartBody->mExModels[1]->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);

                getKartSus(i * 4 + 0)->mWheel->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                getKartSus(i * 4 + 1)->mWheel->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                getKartSus(i * 4 + 2)->mWheel->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                getKartSus(i * 4 + 3)->mWheel->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);

                if ((kartBody->mGameStatus & 0x1000) != 0) {
                    getKartSus(i * 4 + 0)->mShock->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                    getKartSus(i * 4 + 1)->mShock->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                    getKartSus(i * 4 + 2)->mShock->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                    getKartSus(i * 4 + 3)->mShock->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);

                    getKartSus(i * 4 + 0)->mArm->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                    getKartSus(i * 4 + 1)->mArm->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                    getKartSus(i * 4 + 2)->mArm->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                    getKartSus(i * 4 + 3)->mArm->clipBySphere(j, kartCam->GetClipper(), kartCam->GetMtx(), clipperScale);
                }
            }
        }
    }
    return;
}

int KartCtrl::GetPortPtr(int portNumber) {
    u8 offset;
    switch (RaceMgr::getCurrentManager()->getCameraNumber()) {
        case 1:
            offset = 0;
            break;

        case 2:
            if (RaceMgr::getCurrentManager()->isSubScrExist()) {
                switch (portNumber) {
                    case 0: offset = 0; break;
                    case 1: offset = 7; break;
                    default: break;
                }
            } else {
                switch (portNumber) {
                    case 0: offset = 1; break;
                    case 1: offset = 2; break;
                    default: break;
                }
            }
            break;

        case 3:
        case 4:
            switch (portNumber) {
                case 0: offset = 3; break;
                case 1: offset = 4; break;
                case 2: offset = 5; break;
                case 3: offset = 6; break;
                default: break;
            }
            break;

        default:
            offset = 0;
            break;
    }

    // FIX: This return type is probably incorrect.
    return (int)&viewdata[offset];
}

void KartCtrl::GetCamFovy(int camIndex) {
    getKartCam(camIndex)->GetFovy();
}

void KartCtrl::GetCamAspect(int camIndex) {
    getKartCam(camIndex)->GetAspect();
}

CrsGround *KartCtrl::GetBodyGround(int kartIndex) {
    return &getKartBody(kartIndex)->mBodyGround;
}

CrsGround *KartCtrl::GetRRTireGround(int kartIndex) {
    return &getKartBody(kartIndex)->mKartSus[2]->mCrsGnd; // Rear Right Tire Ground
}

CrsGround *KartCtrl::GetRLTireGround(int kartIndex) {
    return &getKartBody(kartIndex)->mKartSus[3]->mCrsGnd; // Rear Left Tire Ground
}

void KartCtrl::GetBodyPos(int kartIndex, JGeometry::TVec3f *vecBodyPos) {
    KartBody *kartBody = getKartBody(kartIndex);
    vecBodyPos->x = kartBody->mPos.x;
    vecBodyPos->y = kartBody->mPos.y;
    vecBodyPos->z = kartBody->mPos.z;
}

void KartCtrl::GetBodyVel(int kartIndex, JGeometry::TVec3f *vecBodyVel) {
    KartBody *kartBody = getKartBody(kartIndex);
    vecBodyVel->x = kartBody->mVel.x * kartBody->mSpeedScale;
    vecBodyVel->y = kartBody->mVel.y * kartBody->mSpeedScale;
    vecBodyVel->z = kartBody->mVel.z * kartBody->mSpeedScale;
}

// FIX: Add return statements for the following two functions:
bool KartCtrl::GetLeftFTirePos(int kartIndex, Vec *vec) {
    getKartBody(kartIndex);
    GetTirePos(kartIndex, 1, vec);
}

bool KartCtrl::GetRightFTirePos(int kartIndex, Vec *vec) {
    getKartBody(kartIndex);
    GetTirePos(kartIndex, 0, vec);
}

void KartCtrl::GetLeftTirePos(int kartIndex, Vec *vec) {
    getKartBody(kartIndex);
    RaceMgr::getCurrentManager();
    GetTirePos(kartIndex, 3, vec);
}

void KartCtrl::GetRightTirePos(int kartIndex, Vec *vec) {
    getKartBody(kartIndex);
    RaceMgr::getCurrentManager();
    GetTirePos(kartIndex, 2, vec);
}

int KartCtrl::GetTirePos(int kartIndex, int kartTireIndex, Vec *kartTireVect) {
    KartBody* kartBody = getKartBody(kartIndex);
    RaceMgr::getCurrentManager();
    
    u32 idx = kartBody->mIdx;
    u32 kartOffset = kartTireIndex;
    
    kartTireVect->x = (kartBody->mKartSus[kartOffset]->mWheel->getBaseTRMtx()[0][3] - kartBody->_2fc.x * kartBody->mKartSus[kartOffset]->mTireRadius);
    kartTireVect->y = (kartBody->mKartSus[kartOffset]->mWheel->getBaseTRMtx()[1][3] - kartBody->_2fc.y * kartBody->mKartSus[kartOffset]->mTireRadius) 
        + tireOffsetPos[idx];
    kartTireVect->z = (kartBody->mKartSus[kartOffset]->mWheel->getBaseTRMtx()[2][3] - kartBody->_2fc.z * kartBody->mKartSus[kartOffset]->mTireRadius);
    
    return (u8)(kartBody->mKartSus[kartOffset]->_124 & 1);
}

f32 KartCtrl::GeTireG(int kartIndex) {
    return getKartBody(kartIndex)->mTireG.x;
}

f32 KartCtrl::GetCarSpeed(int kartIndex) {
    return (getKartBody(kartIndex)->mGameStatus & 0x200) != 0
        ? 0.0f
        : 2.16f * getKartBody(kartIndex)->mSpeed;
}

f32 KartCtrl::GetCarRpm(int kartIndex) {
    f32 carRpm = getKartBody(kartIndex)->mCarRPM / 5000.0f;
    if (carRpm > 1.0f) {
        carRpm = 1.0f;
    }
    return carRpm;
}


f32 KartCtrl::GetKartRpm(int kartIndex) {
    return getKartBody(kartIndex)->mKartRPM;
}

// MJB - The following two functions are the same, just with
//       the pads for the driver/co-driver inverted.
KartGamePad *KartCtrl::GetDriveCont(int index) {
    if ((getKartBody(index)->mGameStatus & 1) == 0) {
        return getPad(index);
    }
    
    return (GetKartCtrl()->WhichDriver(index) == 0)
        ? getPad(index)     // Driver
        : getCoPad(index);  // Passenger
}

KartGamePad *KartCtrl::GetCoDriveCont(int index) {
    if ((getKartBody(index)->mGameStatus & 1) == 0) {
        return getPad(index);
    }
    
    return (GetKartCtrl()->WhichDriver(index) == 0)
        ? getCoPad(index)   // Passenger
        : getPad(index);    // Driver
}

u64 KartCtrl::GetCarStatus(int idx) {
    return getKartBody(idx)->mCarStatus;
}

u32 KartCtrl::GetGameStatus(int idx) {
    return getKartBody(idx)->mGameStatus;
}

void KartCtrl::SetTireDispRound(KartBody *kartBody, KartSus *kartSus, f32 reverse) {
    f32 tireDisp = 0.02f * kartSus->_110;
    f32 speed = 2.16f * kartBody->mSpeed;

    if (kartBody->_458 < 1.0f) {
        speed = 0.0f;
        tireDisp = 0.0f;
    }

    if (reverse < 0.0f) {
        speed = -speed;
    }

    tireDisp += 0.02f * speed;

    if (kartBody->mGameStatus & 0x200) {
        tireDisp = 0.0f;
    } else if (tireDisp > 0.436111f) {
        tireDisp = 0.436111f;
    } else if (tireDisp < -0.436111f) {
        tireDisp = -0.436111f;
    }

    kartSus->_10c += tireDisp;
    if (kartSus->_10c > 3.14 || kartSus->_10c < -3.14) {
        kartSus->_10c = 0.0f;
    }
    return;
}

void KartCtrl::SetKartRpm(KartBody *kartBody, f32 unknownRPM1, f32 unknownRPM2) {
    f32 unknownMultiplier = kartBody->_458 / 60.0f;

    if (unknownMultiplier > 2.0f) {
        unknownMultiplier = 2.0f;
    }

    // Presumably this checks if there was a collision...?
    if (kartBody->getTouchNum() == 0) {
        kartBody->mCarRPM = unknownRPM1;
        unknownMultiplier = 2.0f;
    }
    else {
        kartBody->mCarRPM = unknownRPM2;
    }

    // Ensure car RPM can't be negative.
    if (kartBody->mCarRPM < 0) {
        kartBody->mCarRPM = 0;
    }

    kartBody->mCarRPM *= 40.0f;
    kartBody->mCarRPM *= unknownMultiplier;
    kartBody->mCarRPM += 300.0f;

    // Set upper bounds for car RPM.
    if (kartBody->mCarRPM > 16300.0f) {
        kartBody->mCarRPM = 16300.0f;
    }
}

bool KartCtrl::WhichDriver(int kartIndex) {
    return getKartBody(kartIndex)->mDriver != 0;
}

u8 KartCtrl::WhichNowDriver(int kartIndex) {
    return getKartBody(kartIndex)->mDriver;
}

bool KartCtrl::CheckCamera(int kartIndex) {
    KartBody *kartBody = getKartBody(kartIndex);
    return 100 - kartBody->mCameraNum != 0;
}

int KartCtrl::GetCameraNum(int kartIndex) {
    return getKartBody(kartIndex)->mCameraNum;
}

bool KartCtrl::CheckItem(const int kartIndex) {
    return u8((getKartBody(kartIndex)->mCarStatus & 0x80000000) != 0);
}

f32 KartCtrl::GetMaxSpeed(int kartIndex) {
    f32 maxSpeed;
    KartBody *kartBody = getKartBody(kartIndex);

    switch (kartBody->mBodyGround.getAttribute()) {
        case CrsGround::Attr_1:
        case CrsGround::Attr_4:
        case CrsGround::Attr_11:
            maxSpeed = kartBody->_3f0;
            if (kartBody->mBodyGround.getSplashHeight() > 0.0f && (kartBody->mPlayerPosMtx[1][3] - kartBody->mBodyGround.getSplashHeight() < 0.0f)) {
                maxSpeed *= 0.7f;
            }
            break;
        case CrsGround::Attr_12:
            maxSpeed = kartBody->_3f4;
            if (kartBody->mBodyGround.getSplashHeight() > 0.0f && (kartBody->mPlayerPosMtx[1][3] - kartBody->mBodyGround.getSplashHeight() < 0.0f)) {
                maxSpeed *= 0.7f;
            }
            break;
        case CrsGround::Attr_19:
            maxSpeed = kartBody->_3fc;
            if (kartBody->mBodyGround.getSplashHeight() > 0.0f && (kartBody->mPlayerPosMtx[1][3] - kartBody->mBodyGround.getSplashHeight() < 0.0f)) {              
                maxSpeed *= 0.7f;
            }
            break;
        case CrsGround::Attr_0:
        case CrsGround::Attr_3:
            maxSpeed = kartBody->_3f8;
            if (kartBody->mBodyGround.getSplashHeight() > 0 && (kartBody->mPlayerPosMtx[1][3] - kartBody->mBodyGround.getSplashHeight() < 0.0f)) {
                maxSpeed *= 0.7f;     
            }
            break;
        case CrsGround::Attr_Water:
            // Presumably, slow down when in water, e.g. Peach Beach...?
            maxSpeed = kartBody->_3f0;
            if ((kartBody->_1a0[1][3] - kartBody->mBodyGround.getWaterHeight()) < -50.0f) {
                maxSpeed *= 0.3f;
            }
            else if ((kartBody->_1a0[1][3] - kartBody->mBodyGround.getWaterHeight()) < 0.0f) {
                maxSpeed *= 0.7f;
            }
            break;
        default:
            maxSpeed = kartBody->_3f0;
            if (kartBody->mBodyGround.getSplashHeight() > 0.0f && (kartBody->mPlayerPosMtx[1][3] - kartBody->mBodyGround.getSplashHeight() < 0.0f)) {
                maxSpeed *= 0.7f;
            }
            break;
    }
    return maxSpeed;
}

f32 KartCtrl::GetDownSlopeAcc(int kartIndex) {
    KartBody *kartBody = getKartBody(kartIndex);
    f32 slopeNumerator = kartBody->_464;

    if (slopeNumerator < 0.0f && kartBody->_32c.y != 1.0f) {
        slopeNumerator = -slopeNumerator;
        if (slopeNumerator <= 0.0872222f) {
            return 1.0f;
        } else if (slopeNumerator >= 0.785f) {
            slopeNumerator = 0.785f;
        }

        slopeNumerator /= 0.785f;
        return 0.2f * slopeNumerator + 1.0f;
    }

    return 1.0f;
}

f32 KartCtrl::GetDownSlopeSpeed(int kartIndex) {
    KartBody *kartBody = getKartBody(kartIndex);
    f32 slopeNumerator;

    if (kartBody->_464 < 0.0f && kartBody->_32c.y != 1.0f) {
        slopeNumerator = -kartBody->_464;
        if (slopeNumerator <= 0.0523333f) {
            return 0.0f;
        } else if (slopeNumerator >= 0.174444f) {
            slopeNumerator = 0.174444f;
        }
        slopeNumerator /= 0.174444f;
        return 3.0f * slopeNumerator;
    }
    return 0.0f;
}

f32 KartCtrl::GetTireAngle(int kartIndex) {
    return getKartBody(kartIndex)->mTireAngle;
}

bool KartCtrl::GetTandemDir(int index) {
    return !!getKartAnime(index)->IsBack(index);
}

f32 KartCtrl::GetWaterHeight(int kartIndex) {
    KartBody *kartBody = getKartBody(kartIndex);

    
    if (kartBody->mBodyGround.getAttribute() == 0x11) {
        return kartBody->_1a0[1][3] - kartBody->mBodyGround.getWaterHeight();
    }
    else {
        return (kartBody->mBodyGround.getSplashHeight() > 0.0f) ? 
            kartBody->_1a0[1][3] - kartBody->mBodyGround.getSplashHeight() : 0.0f;
    }
}

bool KartCtrl::CheckJugemuSignal() { return mBitfield & 2; }

GameAudio::KartSoundMgr *KartCtrl::GetKartSoundMgr(int kartIndex) {
    KartSound *kartSound = getKartSound(kartIndex);
    return kartSound->mSoundMgr;
}

GameAudio::CharacterSoundMgr *KartCtrl::GetCharacterSoundMgr(int kartIndex) {
    // void DriverModel::getGameAudioCharacterSoundMgr() {}
    // Unusure, requires something with DriverModel
    return mKartLoaders[kartIndex]->mDriverModels[0]->getGameAudioCharacterSoundMgr();
}

f32 KartCtrl::GetKartBodyOffset(int kartIndex) {
    getKartBody(kartIndex);     // MJB - should this be used...?
    return 60.0f;
}

// const in ItemObjMgr is more likely, however that caused issues in ItemObjMgr itself, leaving it here shouldn't cause issues
bool KartCtrl::MakeChangePossible(const int index) {
    KartBody *kartBody = getKartBody(index);
    ItemObjMgr *itemObjMgr = GetItemObjMgr();
    if ((kartBody->mGameStatus & 1) != 0) {
        return false;
    }
    u8 isRollingSlot = ItemObjMgr::getNowTandemDriverNum(index);
    if (isRollingSlot == 0) {
        if (((itemObjMgr->IsRollingSlot(index, 1)) ||
            (itemObjMgr->getKartEquipItem(index, 1) != nullptr)) &&
            (itemObjMgr->getKartEquipItem(index, 0) != nullptr))
        {
            kartBody->mCarStatus |= 0x4000000000ull;       // 0x570
        }
    }
    else {
        if ((itemObjMgr->IsRollingSlot(index, 0) ||
            (itemObjMgr->getKartEquipItem(index, 0) != nullptr)) &&
            (itemObjMgr->getKartEquipItem(index, 1) != nullptr))
        {
            kartBody->mCarStatus |= 0x4000000000ull;       // 0x570
        }
    }
    
    if ((kartBody->mCarStatus & 0x4000000000ull) == 0) {
        return false;
    }
    else if (isRollingSlot == 0) {
        if ((itemObjMgr->IsRollingSlot(index, 1) ||
            (itemObjMgr->getKartEquipItem(index, 1) != nullptr)) &&
            (itemObjMgr->getKartEquipItem(index, 0) == nullptr))
        {
            return true;
        }
    } else {
        if ((itemObjMgr->IsRollingSlot(index, 0) ||
            (itemObjMgr->getKartEquipItem(index, 0) != nullptr)) &&
            (itemObjMgr->getKartEquipItem(index, 1) == nullptr))
        {
            return true;
        }
    }
    return false;
}

bool KartCtrl::CheckTandemItmGet(int kartIndex) {
    getKartBody(kartIndex);
    ItemObjMgr *itemObjMgr = GetItemObjMgr();

    u8 tandemDriverNum = ItemObjMgr::getNowTandemDriverNum(kartIndex);
    if (tandemDriverNum == 0) {
        if (itemObjMgr->getKartEquipItem(kartIndex, 0) != nullptr) {
            return true;
        }
    }
    else {
        if (itemObjMgr->getKartEquipItem(kartIndex, 1) != nullptr) {
            return true;
        }
    }
    return false;
}

bool KartCtrl::CheckTandemItmRollingGet(int kartIndex) {
    getKartBody(kartIndex);
    ItemObjMgr *itemObjMgr = GetItemObjMgr();

    u8 tandemDriverNum = ItemObjMgr::getNowTandemDriverNum(kartIndex);
    if (tandemDriverNum == 0) {
        if (itemObjMgr->IsRollingSlot(kartIndex, 0)) {
            return true;
        }
    }
    else {
        if (itemObjMgr->IsRollingSlot(kartIndex, 1)) {
            return true;
        }
    }
    return false;
}

void KartCtrl::SetObjectPos(int camIndex, JGeometry::TVec3f vecObjectPos) {
    KartDemoCam *kartDemoCam = getKartCam(camIndex)->GetDemoCam();
    kartDemoCam->_8c.set(vecObjectPos);
}

bool KartCtrl::CheckThunderBolt(int kartIndex) {
    return getKartBody(kartIndex)->getThunder()->mFlags & 1;
}

double KartCtrl::GetTireRadius(int kartIndex) {
    return getKartBody(kartIndex)->mKartSus[2]->mTireRadius;
}

bool KartCtrl::IsBurn(int kartIndex) {
    KartDamage *kartDamage = getKartBody(kartIndex)->getDamage();
    return (u8)((kartDamage->mFlags & 0x180) != 0);
}

bool KartCtrl::IsWallReact(int kartIndex) {
    KartBody *kartBody = getKartBody(kartIndex);
    KartTumble *kartTumble = kartBody->getTumble();
    
    if ((kartTumble->_0[4] & 1) != 0) {
        return true;
    }
    else {
        return u8((kartBody->mCarStatus & 0x100000) != 0);
    }
}

bool KartCtrl::HaveBalloon(int kartIndex) {
    getKartBody(kartIndex);
    return RCMGetKartChecker(kartIndex)->getBalloonNumber() == 0;
}

int KartCtrl::GetDriftCnt(int kartIndex) {
    KartBody *kartBody = getKartBody(kartIndex);
    
    if ((kartBody->mCarStatus & 3) != 0) {
        return (u8)(kartBody->mMTState + 1);
    }
    return 0;
}

bool KartCtrl::IsMiniGame() {
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();
    if (raceMgr->getRaceMode() == BALLOON_BATTLE|| raceMgr->getRaceMode() == ROBBERY_BATTLE 
        || raceMgr->getRaceMode() == BOMB_BATTLE || raceMgr->getRaceMode() == ESCAPE_BATTLE) {
        return true;
    }
    return false;
}

bool KartCtrl::IsMiniGameEnd() {
    if (GetKartCtrl()->IsMiniGame()) {
        RaceMgr *raceMgr = RaceMgr::getCurrentManager();
        if (raceMgr->isRaceEnd() != 0) {
            return true;
        }
    }
    return false;
}

u8 KartCtrl::CheckWinner() {
    int kartNumber = GetKartCtrl()->GetKartNumber();
    
    for (int kartIndex = 0; kartIndex < kartNumber; kartIndex++) {
        KartBody *kartBody = GetKartCtrl()->getKartBody(kartIndex);
        KartChecker *kartChecker = RCMGetKartChecker(kartBody->mMynum);
        int rank = kartChecker->getRank();
        if (kartChecker->getRank() == 1) {
            return kartBody->mMynum;
        }
    }
    return 1;
}

void KartCtrl::GetKartEffctVel(int kartIndex, JGeometry::TVec3f *vecKartEffectVel) {
    KartBody *kartBody = getKartBody(kartIndex);

    if ((kartBody->mGameStatus & 0x2000) != 0) {
        vecKartEffectVel->set(kartBody->mEffctVel);
    }
    else {
        vecKartEffectVel->zero();
    }
}

bool KartCtrl::CheckChange(int kartIndex) {
    KartBody* kartBody = getKartBody(kartIndex);

    if (kartBody->mDriverModels[2]->IsChange() != 0 || kartBody->mDriverModels[3]->IsChange() != 0)
        return true;
    return false;
}

bool KartCtrl::CheckMatchless(int kartIndex) {
    KartBody* kartBody = getKartBody(kartIndex);
    return kartBody->getCrash()->CheckMatchlessTimer();
}

bool KartCtrl::CheckReverse(int kartIndex) {
    KartBody* kartBody = getKartBody(kartIndex);
    return kartBody->_5c3 & 2;
}

f32 KartCtrl::GetKartScale(int kartIndex) {
    KartBody *kartBody = getKartBody(kartIndex);
    return (kartBody->getThunder()->mFlags & 1) != 0
        ? kartBody->getThunder()->mScale    // You've been... THUNDERSTRUCK!
        : 1.0f;
}

RivalKart *KartCtrl::getKartEnemy(int kartIndex) {
    return mRivalKarts[kartIndex];
}

CLPoint *KartCtrl::GetKartCenterPtr(int kartIndex) {
    return getKartTarget(kartIndex)->mCenter;
}

void KartCtrl::EnemyInit(int kartIndex) {
    getKartEnemy(kartIndex)->Init();
}

void KartCtrl::EnemyResetInit(int kartIndex) {
    getKartEnemy(kartIndex)->ResetInit();
}

bool KartCtrl::CheckZoomWinConsole() { return mBitfield & 4; }

void KartCtrl::SetUpMiniTurbo(int kartIndex, unsigned char setupMiniTurbo) {
    getKartBody(kartIndex)->mMTState = setupMiniTurbo;
}

void KartStrat::DoStepAccel() {
    KartBody *kartBody = mBody;
    int index = kartBody->mMynum;

    KartGamePad *kartGamePadDriver = GetKartCtrl()->GetDriveCont(index);
    KartGamePad *kartGamePadPassenger = GetKartCtrl()->GetCoDriveCont(index);

    if (kartGamePadDriver->testButton(GetKartCtrl()->getKartPad(index)->mAccelBtn)) {
        kartBody->_3c8 = GetKartCtrl()->fcnvge(kartBody->_3c8, kartBody->_3d0, kartBody->_3d4, kartBody->_3d4);
    } else {
        GetKartCtrl()->ChaseFnumber(&kartBody->_3c8, 0.0f, 0.8f);
    }

    if (kartGamePadPassenger->testButton(GetKartCtrl()->getKartPad(index)->mAccelBtn)) {
        GetKartCtrl()->getKartAnime(index)->DoStartRunStartAnime(index);
    } else {
        GetKartCtrl()->getKartAnime(index)->DoStartRunEndAnime(index);
    }

    f32 carRPM = 0.0f;
    for (int i = 0; i < 4; i++) {
        KartSus *kartSus = GetKartCtrl()->getKartSus(i + (index * 4));
        kartSus->_278 += kartBody->_3c8;
        kartSus->_278 -= GetKartCtrl()->LimmtNumber(kartSus->_278, 1.0f);
        kartSus->_278 = GetKartCtrl()->LimmtNumber(kartSus->_278, kartBody->_3e4);
        carRPM += kartSus->_278;
    }
    kartBody->mCarRPM = carRPM;
    if (kartBody->mCarRPM < 0.0f) {
        kartBody->mCarRPM = 0.0f;
    }
    kartBody->mCarRPM *= 40.0f;
    kartBody->mCarRPM *= 2.0f;
    kartBody->mCarRPM += 300.0f;
}

void KartStrat::DoStepSterr() {    
    KartBody *kartBody = mBody;
    int index = kartBody->mMynum;
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();

    KartSus *kartSus1 = GetKartCtrl()->getKartSus((index * 4) + 0);
    KartSus *kartSus2 = GetKartCtrl()->getKartSus((index * 4) + 1);
    GetKartCtrl()->getKartSus((index * 4) + 2);
    GetKartCtrl()->getKartSus((index * 4) + 3);

    f32 stepSterr = GetKartCtrl()->GetDriveCont(index)->getMainStickX();
    f32 stepSterrUnknown = 10.0f * stepSterr;

    if ((((kartBody->mGameStatus & 8)) && (stepSterrUnknown < 4.0f)) && (stepSterrUnknown > -4.0f)) {
        stepSterrUnknown = 0.0f;
    }

    stepSterr = 10.0f - stepSterrUnknown;

    if ((kartBody->mGameStatus & 8)) {
        kartBody->_38c = GetKartCtrl()->fcnvge(kartBody->_38c, stepSterr, 0.4f, 0.4f);
    } else {
        kartBody->_38c = GetKartCtrl()->fcnvge(kartBody->_38c, stepSterr, 1.8f, 1.8f);
    }

    kartBody->_388 = (kartBody->_38c - 10.0f) / 10.0f;
    GetKartCtrl()->RotYMatrix33(kartSus1->_188, 0.680333f * kartBody->_388);
    GetKartCtrl()->RotYMatrix33(kartSus2->_188, 0.680333f * kartBody->_388);

    if (raceMgr->getRaceMode() != 8) {
        GetKartCtrl()->getKartAnime(index)->DoDriveAnime(index);
    }
}

void KartStrat::DoSterr() {
    KartBody *kartBody = mBody;
    int myNum = kartBody->mMynum;
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();
    // Based on the variables stored below, we only need to modify the front
    // tires as they indicate the direction the kart is being steered.
    KartSus *kartSus1 = GetKartCtrl()->getKartSus(myNum * 4);
    KartSus *kartSus2 = GetKartCtrl()->getKartSus(myNum * 4 + 1);
    GetKartCtrl()->getKartSus(myNum * 4 + 2);
    GetKartCtrl()->getKartSus(myNum * 4 + 3);

    KartGamePad *kartGamePad = GetKartCtrl()->GetDriveCont(myNum);
    f32 steeringInput = 1.8f;
    
    if ((kartBody->mGameStatus & 4) == 0) {
        return;
    }
    if ((kartBody->mCarStatus & 2) != 0) {
        kartBody->_4a4 = -steeringInput;
        steeringInput = (kartGamePad->getMainStickX() - steeringInput) * 0.5f;

        if (steeringInput > -0.65f) {
            steeringInput *= 0.05f;
        } else if (steeringInput <= -1.4f) {
            steeringInput *= 1.35f;
        } else if (steeringInput <= -1.3f) {
            steeringInput *= 1.3f;
        } else if (steeringInput <= -1.0f) {
            steeringInput *= 1.2f;
        } else if (steeringInput <= -0.9f) {
            steeringInput *= 1.15f;
        }
        kartBody->mSterrFrame = steeringInput;

    } else if ((kartBody->mCarStatus & 1) != 0) {
        kartBody->_4a4 = steeringInput;
        steeringInput = (kartGamePad->getMainStickX() + steeringInput) * 0.5f;
        
        if (steeringInput < 0.65f) {
            steeringInput *= 0.05f;
        } else if (steeringInput >= 1.4f) {
            steeringInput *= 1.35f;
        } else if (steeringInput >= 1.3f) {
            steeringInput *= 1.3f;
        } else if (steeringInput >= 1.0f) {
            steeringInput *= 1.2f;
        } else if (steeringInput >= 0.9f) {
            steeringInput *= 1.15f;
        }
        kartBody->mSterrFrame = steeringInput;

    } else {
        if (kartBody->_4a4 != 0.0f) {
            GetKartCtrl()->ChaseFnumber(&kartBody->_4a4, 0.0f, 0.45f);
            steeringInput = (kartBody->_4a4 + kartGamePad->getMainStickX()) / 2.0f;
        } else {
            steeringInput = kartGamePad->getMainStickX();
        }
        
        kartBody->mSterrFrame = steeringInput;
    }

    if (steeringInput > 1.0f) {
        steeringInput = 1.0f;
    }
    if (steeringInput < -1.0f) {
        steeringInput = -1.0f;
    }

    f32 inputStrength = 0.3f;
    f32 stickX = kartGamePad->getMainStickX();
    if (stickX != 0.0f) {
        if (stickX < 0.0f) {
            stickX = -stickX;
        }
        inputStrength *= stickX;
        if (inputStrength <= 0.01f) {
            inputStrength = 0.01f;
        }
    }

    f32 steeringSensitivity = 20.0f - (kartBody->mSpeed * 0.13333334f);
    if (steeringSensitivity <= 1.0f) {
        steeringSensitivity = 1.0f;
    }
    if (steeringSensitivity >= 20.0f) {
        steeringSensitivity = 20.0f;
    }

    if ((kartBody->mCarStatus & 3) == 0) {
        steeringSensitivity = 11.0f;
        if (GetKartCtrl()->GetCarSpeed(myNum) <= 30.0f) {
            steeringSensitivity = 14.0f;
        }
    }

    GetKartCtrl()->ChaseFnumber(&kartBody->mTireAngle, (3.141f * (-steeringInput * steeringSensitivity)) / 180.0f, inputStrength);
    if (raceMgr->getRaceMode() == 8 || kartBody->getChecker()->CheckCrash() == 1) {
        kartBody->mTireAngle = 0.0f;
    }

    GetKartCtrl()->RotYMatrix33(kartSus1->_128, kartBody->mTireAngle);
    GetKartCtrl()->RotYMatrix33(kartSus2->_128, kartBody->mTireAngle);

    f32 speedAdjustment = kartGamePad->getMainStickX() * 10.0f;
    if ((((kartBody->mGameStatus & 8) != 0) && (speedAdjustment < 4.0f)) && (speedAdjustment > -4.0f)) {
        speedAdjustment = 0.0f;
    }

    f32 adjustment = 10.0f - speedAdjustment;
    if ((kartBody->mGameStatus & 8) != 0) {
        kartBody->_38c = GetKartCtrl()->fcnvge(kartBody->_38c, adjustment, 0.4f, 0.4f);
    } else {
        kartBody->_38c = GetKartCtrl()->fcnvge(kartBody->_38c, adjustment, 1.8f, 1.8f);
    }

    kartBody->_388 = (kartBody->_38c - 10.0f) / 10.0f;
    GetKartCtrl()->RotYMatrix33(kartSus1->_188, kartBody->_388 * 0.680333f);
    GetKartCtrl()->RotYMatrix33(kartSus2->_188, kartBody->_388 * 0.680333f);

    if (raceMgr->getRaceMode() != 8) {
        // Wait... what? Why...? Why duplicate code??? Why, I say... WHY?!
        if (steeringInput < 0.0f) {
            steeringInput = 10.0f - (10.0f * steeringInput);
        } else {
            steeringInput = 10.0f - (10.0f * steeringInput);
        }

        kartBody->_384 = GetKartCtrl()->fcnvge(kartBody->_384, steeringInput, 0.85f, 0.85f);
        kartBody->mFrame = (kartBody->_384 - 10.0f) / 10.0f;
    }
    
    return;
}

void KartStrat::DoAccel() {    
    KartBody *kartBody = mBody;
    int index = kartBody->mMynum;
    KartGamePad *kartGamePad = GetKartCtrl()->GetDriveCont(index);
    kartBody->_5c3 &= ~5;

    if ((kartBody->mGameStatus & 4) == 0) {
        return;
    }

    if (GetKartCtrl()->getKartBody(index)->getChecker()->CheckCrash()) {
        return;
    }

    f32 unknownRPM2 = kartBody->_3d0;
    if ((kartBody->mCarStatus & 0x8000020) != 0) {
        unknownRPM2 *= 0.2f;
    }

    if (kartGamePad->testButton(GetKartCtrl()->getKartPad(index)->mAccelBtn)) {
        kartBody->_3c8 = GetKartCtrl()->fcnvge(
            kartBody->_3c8, unknownRPM2, kartBody->_3d4, kartBody->_3d4);
    } else {
        GetKartCtrl()->ChaseFnumber(&kartBody->_3c8, 0.0f, 0.1f);
        if (kartBody->_3c8 < 0.5f) {
            kartBody->_3c8 = 0.0f;
        }
    }

    f32 unknownRPM3 = 0.0f;
    f32 unknownRPM1 = 0.0f;

    for (int i = 0; i < 4; i++) {
        unknownRPM3 += GetKartCtrl()->getKartSus(i + (index * 4))->_110;
    }

    if ((kartBody->mBodyGround.getAttribute() != CrsGround::Attr_6) && (kartBody->getTouchNum() != 0) && kartGamePad->testButton(GetKartCtrl()->getKartPad(index)->mBtnB)) {
        GetKartCtrl()->ChaseFnumber(&kartBody->_3cc, kartBody->_3d8, kartBody->_3dc);
        kartBody->_5c3 |= 4;
        if ((kartBody->_5c3 & 2) == 0) {
            GetKartCtrl()->getKartSound(index)->DoBrakeSound();
        }
    } else {
        kartBody->_3cc = 0.0f;
    }
    
    for (int i = 0; i < 4; i++) {
        KartSus *kartSus = GetKartCtrl()->getKartSus(i + (index * 4));
        kartSus->_110 += kartBody->_3c8;
        kartSus->_278 += kartBody->_3c8;
        kartSus->_110 -= GetKartCtrl()->LimmtNumber(kartSus->_110, kartBody->_3e0);
        kartSus->_278 -= GetKartCtrl()->LimmtNumber(kartSus->_278, 1.0f);
        kartSus->_110 = GetKartCtrl()->LimmtNumber(kartSus->_110, kartBody->_3e4);
        kartSus->_278 = GetKartCtrl()->LimmtNumber(kartSus->_278, kartBody->_3e4);
        kartSus->_110 -= kartBody->_3cc;
        unknownRPM1 += kartSus->_278;
    }

    JGeometry::TVec3f vecVelocity;
    GetKartCtrl()->DevMatrixByVector(&vecVelocity, (Vec *)&kartBody->mVel.x, kartBody->_110);

    if (GetKartCtrl()->GetCarSpeed(kartBody->mMynum) <= 50.0f && ((GetKartCtrl()->GetCarSpeed(kartBody->mMynum) > 10.0f) && (kartBody->_510 > 2.44222f))) {
        kartBody->_5c3 &= ~5;
        kartBody->_5c3 |= 2;
    } else {
        kartBody->_5c3 &= ~0x2;
    }

    for (int i = 0; i < 4; i++) {
        KartSus *kartSus = GetKartCtrl()->getKartSus(i + (index * 4));
        GetKartCtrl()->SetTireDispRound(kartBody, kartSus, vecVelocity.z);
    }
    
    GetKartCtrl()->SetKartRpm(kartBody, unknownRPM1, unknownRPM3);
}

void KartCtrl::HaveRabbit() {
    RaceMgr *raceMgr = RaceMgr::getCurrentManager();
    int kartNumber = GetKartNumber();
    
    if (raceMgr->getRaceMode() != ESCAPE_BATTLE) 
        return;

    KartBody *kartBody;
    bool haveRabbit = false;
    for (int i = 0; i < kartNumber; i++) {
        kartBody = getKartBody(i);
        if (kartBody->getItem()->_0[8] != 0) {
            haveRabbit = true;
        }
    }

    // MJB - Shine thief's music controls, maybe...?
    //       Rabbit might refer to the person with the shine...
    if (haveRabbit && (kartBody->getItem()->_0[9] == 0)) {
        getKartSound(kartBody->mMynum)->DoShineStartBGM();
        kartBody->getItem()->_0[9] = 1;
    } else if (!haveRabbit && (kartBody->getItem()->_0[9] != 0)) {
        getKartSound(kartBody->mMynum)->DoShineStopBGM();
        kartBody->getItem()->_0[9] = 0;
    }
}
