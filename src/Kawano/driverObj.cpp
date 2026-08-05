#include "Kawano/driverObj.h"
#include "JSystem/JAudio/JASFakeMatch2.h"
#include "JSystem/J3D/J3DModelLoader.h"
#include "Kaneshige/ExModel.h"
#include "Kaneshige/RaceMgr.h"
#include "Kaneshige/TexLODControl.h"

DriverObj::DriverObj() {}

DriverObj::~DriverObj() {}

J3DModelData* DriverObj::createModelData(char* name) {
    J3DModelData* modelData = J3DModelLoaderDataBase::load(ResMgr::getPtr(ResMgr::mcArcMRAM, name), 0x011020030);
    ExModel::patchModelData(modelData);
    ExModel::setLightMask(modelData, GX_LIGHT1);
    ExModel::setLODBias(modelData, TexLODControl::getCharacterLODBias());
    RCMGetCourse()->setFogInfo(modelData);
    modelData->newSharedDisplayList(0x40000);
    return modelData;
}
