#ifndef MAPOBJNOMOVE_H
#define MAPOBJNOMOVE_H

#include "Kaneshige/Course/CrsData.h"
#include "Sato/GeographyObj.h"


class TMapObjNoMove : public GeographyObj {
public:
    TMapObjNoMove(const CrsData::SObject &);    // 0x8029c390
    virtual ~TMapObjNoMove() {}                 // 0x8029c3d8
    virtual void reset();                       // 0x8029c458
    virtual const char *getBmdFileName();       // 0x8029c4a4
    virtual void calc() {}                      // 0x8029c4f0

    static GeoAnmTableEntry sObjList[4];        // 0x803a4d40
    
}; // class TMapObjNoMove
#endif // MAPOBJNOMOVE_H
