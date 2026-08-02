#include "Kawano/driver.h"
#include "Osako/animator.h"

DriverModel::DriverModel(KartLoader *, int, u16, bool, int) {}

void DriverModel::convCharIDToDataID(ECharID) {}

void DriverModel::init_ik(int) {}

void DriverModel::init(int) {}

void DriverModel::initMtxCalc() {}

void DriverModel::initAnmTrans_BossPakkun(J3DAnmTransform *) {}

void DriverModel::InitMpaMtxCalc(DriverModel::StateEnum, J3DAnmTransform *, J3DAnmTransform *, J3DAnmTransform *, J3DAnmTransform *) {}

void DriverModel::restart(int) {}

void DriverModel::setDriveAnm(f32) {}

void DriverModel::setTandemAnm(f32, f32) {}

void DriverModel::Start1ShotAnm_base(DriversAnimator *, DriverModel::StateEnum) {}

void DriverModel::StartAnimation2() {}

void DriverModel::StartAnimation(DriverModel::StateEnum) {}

void DriverModel::updateFrame() {}

void DriverModel::updateFrame_base(DriversAnimator *, DriverModel::StateEnum) {}

void DriverModel::updateSetFrame() {}

void DriverModel::updateLoad_Animation() {}

void DriverModel::updateLoad() {}

void DriverModel::updateCheckRenew_Animation() {}

void DriverModel::updateCheckRenew() {}

void DriverModel::updateBlendCount() {}

void DriverModel::updateSetBlendWeight_GetBlendTime(DriverModel::StateEnum) {}

void DriverModel::updateSetBlendWeight() {}

void DriverModel::update(u16) {}

bool DriverModel::createModel(JKRSolidHeap *, u32, u32) {}

void DriverModel::setCurrentViewNo(u32) {}

void DriverModel::setItem(ItemObj *) {}

void DriverModel::resetItem() {}

void DriverModel::getMtxP_HandR() {}

void DriverModel::getMtxP_HandR2() {}

void DriverModel::getMtxP(int) {}

void DriverModel::getAnmTransformPtr(int) {}

void DriverModel::IsAnmResInMRAM(int) {}

void DriverModel::IsAnmSoundResInMRAM(int) {}

#include "JSystem/JAudio/JASFakeMatch2.h"
