#ifndef GEOSTARTLINE_H
#define GEOSTARTLINE_H

#include "Sato/GeographyObj.h"

class GeoStartLine : public GeographyObj {
public:
    GeoStartLine();                              // 0x801b4860
    virtual ~GeoStartLine() {}                   // 0x801b4bd0
    virtual void reset();                        // 0x801b49c8
    virtual void createColModel(J3DModelData *); // 0x801b4ba0
    virtual const char *getBmdFileName();        // 0x801b4ba4
    virtual void calc();                         // 0x801b4bcc
};

#endif
