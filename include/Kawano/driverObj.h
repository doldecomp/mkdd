#ifndef DRIVER_OBJ_H
#define DRIVER_OBJ_H

#include "JSystem/JKernel/JKRDisposer.h"

class J3DModelData;

class DriverObj : public JKRDisposer {
public:
    DriverObj();

    virtual ~DriverObj();
    J3DModelData* createModelData(char*);
};

#endif // DRIVER_OBJ_H
