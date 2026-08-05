#ifndef COURSENAME2D_H
#define COURSENAME2D_H

#include "JSystem/JKernel/JKRArchive.h"
#include "kartEnums.h"

class CourseName2D
{
public:
    CourseName2D(JKRArchive *archive);  // 0x80190a94
    ~CourseName2D();                    // 0x80190b5c

    void *getCourseNameTexture(ECourseID courseId);  // 0x80190bb0

private:
    static CourseName2D *mCourseName2D;  // 0x80416378

    void *mTextures[16];
};

#endif  // COURSENAME2D_H
