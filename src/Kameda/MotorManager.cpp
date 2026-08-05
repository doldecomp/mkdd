#include "Kameda/MotorManager.h"
#include "mathHelper.h"

int MotorManager::mCountFrame[0x19];
MotorManager *MotorManager::mThis;

MotorManager::MotorManager(JKRHeap *) {}

MotorManager::~MotorManager() {}

void MotorManager::reset() {}

void MotorManager::exec() {}

void MotorManager::setMotor(MotorManager::MotorType, int) {}

void MotorManager::setAwardMotor(MotorManager::MotorType) {}

void MotorManager::setPowerMotor(int, f32, u8, u8) {}

#include "JSystem/JAudio/JASFakeMatch2.h"
