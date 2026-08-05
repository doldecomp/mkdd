#include "Kameda/CourseName2D.h"

#include "JSystem/JAudio/JASFakeMatch2.h"  // For static initializer
#include "Osako/ResMgr.h"
#include "kartEnums.h"

CourseName2D *CourseName2D::mCourseName2D;

CourseName2D::CourseName2D(JKRArchive *archive)
{
    mCourseName2D = this;

    ECourseID courseIds[16] = {
        BABY_PARK,
        PEACH_BEACH,
        DAISY_CRUISER,
        LUIGI_CIRCUIT,
        MARIO_CIRCUIT,
        YOSHI_CIRCUIT,
        MUSHROOM_BRIDGE,
        MUSHROOM_CITY,
        WALUIGI_STADIUM,
        WARIO_COLOSSEUM,
        DINO_DINO_JUNGLE,
        DK_MOUNTAIN,
        BOWSER_CASTLE,
        RAINBOW_ROAD,
        DRY_DRY_DESERT,
        SHERBET_LAND,
    };

    for (int i = 0; i < 16; ++i)
    {
        mTextures[i] = nullptr;

        char buffer[0x40];
        snprintf(buffer, sizeof(buffer), "timg/%s_nameS.bti", ResMgr::getCrsArcName(courseIds[i]));

        mTextures[i] = archive->getResource(buffer);
    }
}

CourseName2D::~CourseName2D()
{
    if (mCourseName2D)
        mCourseName2D = nullptr;
}

void *CourseName2D::getCourseNameTexture(ECourseID courseId)
{
    void *texture = nullptr;

    switch (courseId)
    {
    case BABY_PARK:
        texture = mTextures[0];
        break;
    case PEACH_BEACH:
        texture = mTextures[1];
        break;
    case DAISY_CRUISER:
        texture = mTextures[2];
        break;
    case LUIGI_CIRCUIT:
        texture = mTextures[3];
        break;
    case MARIO_CIRCUIT:
        texture = mTextures[4];
        break;
    case YOSHI_CIRCUIT:
        texture = mTextures[5];
        break;
    case MUSHROOM_BRIDGE:
        texture = mTextures[6];
        break;
    case MUSHROOM_CITY:
        texture = mTextures[7];
        break;
    case WALUIGI_STADIUM:
        texture = mTextures[8];
        break;
    case WARIO_COLOSSEUM:
        texture = mTextures[9];
        break;
    case DINO_DINO_JUNGLE:
        texture = mTextures[10];
        break;
    case DK_MOUNTAIN:
        texture = mTextures[11];
        break;
    case BOWSER_CASTLE:
        texture = mTextures[12];
        break;
    case RAINBOW_ROAD:
        texture = mTextures[13];
        break;
    case DRY_DRY_DESERT:
        texture = mTextures[14];
        break;
    case SHERBET_LAND:
        texture = mTextures[15];
        break;
    default:
        break;
    }

    return texture;
}
