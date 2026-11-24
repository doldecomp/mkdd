
#include "Inagaki/GameAudioCamera.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/JAudio/Interface/JAISe.h"
#include "JSystem/JAudio/Interface/JAISound.h"
#include "JSystem/JGeometry/Util.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"


namespace GameAudio {

JGeometry::TVec3f sShoreDummyVec(0, 0, 0);
CustomAudience<4> *ShoreSoundMgr::smAudience;

}

#include "JSystem/JAudio/JASFakeMatch15.h"

namespace GameAudio {

GameAudio::ShoreSoundMgr::ShoreSoundMgr(JKRHeap *heap) : SoundMgr<ShoreSoundMgr>(&sShoreDummyVec, heap, 4) {
    _5c = 0;
    mSoundPosition = new JGeometry::TVec3f[4];
    _68 = new PosMove[4];
    _6c = new PosMove[4];

    for (u8 i = 0; i < 4; i++) {
        mSoundPosition[i].set(0.0f, 0.0f, 0.0f);
    }
    _5d = 0;
    _60 = 0.0f;
}

GameAudio::ShoreSoundMgr::~ShoreSoundMgr() {}

void GameAudio::ShoreSoundMgr::loop() {
    #line 79
    JUT_ASSERT_MSG(mSoundPos != NULL, "ShoreSoundMgr::loop　disable中です！！！！！\n");

    if(mKillSw) {
        return;
    }

    const u32 sceneMax = GameAudio::Main::getAudio()->getCamera()->getSceneMax();

    // Forced to cast sceneMax into its own u32 type here...
    for(s32 index = 0; index < (u32)sceneMax; index++){
        if ((*this)[index] == NULL) {
            continue;
        }

        const JGeometry::TVec3f& playPos = getPlayPos(index);
        JAIAudible *audible = (*this)[index]->getAudible();
        if(audible) {
            audible->setPos(playPos);
        }
    }

    _5c = 0;
}

void GameAudio::ShoreSoundMgr::frameWork() {
    if (mKillSw)
        return;
    _5c = 1;
}

// This looks like it could be a TVec3f method...
// But too awkward, instinctive implementation like the one below does not offer
// a 100% matching.
// return JGeometry::TUtilf::epsilonEquals(a.x, b.x, epsilon) &&
//    JGeometry::TUtilf::epsilonEquals(a.y, b.y, epsilon) &&
//    JGeometry::TUtilf::epsilonEquals(a.z, b.z, epsilon);

static inline bool isSamePos(const JGeometry::TVec3f &a, const JGeometry::TVec3f &b) {
    const f32 epsilon = JGeometry::TUtilf::epsilon();
    bool same = false;
    bool sameXY = false;
    if (JGeometry::TUtilf::epsilonEquals(a.x, b.x, epsilon)
        && JGeometry::TUtilf::epsilonEquals(a.y, b.y, epsilon)) {
        sameXY = true;
    }
    if (sameXY && JGeometry::TUtilf::epsilonEquals(a.z, b.z, epsilon)) {
        same = true;
    }
    return same;
}

void GameAudio::ShoreSoundMgr::setTide(u8 camnum, JGeometry::TVec3f *pos1, JGeometry::TVec3f *pos2) {
    #line 108
    JUT_ASSERT_MSG(camnum < 4, "ShoreSoundMgr::setSe camnum over");

    if(mKillSw) {
        return;
    }

    JGeometry::TVec3f* currentPosition;
    PosMove* currentPosMove;

    for(u8 i = 0; i < 2; i++)
    {
        if(i == 0)
        {
            currentPosition = pos1;
            currentPosMove = &_68[camnum];
        }
        else {
            currentPosition = pos2;
            currentPosMove = &_6c[camnum];
        }

        if(currentPosition == NULL)
        {
            currentPosMove->mMoving = false;
        }
        else if (currentPosMove->mMoving == false)
        {
            currentPosMove->mPos = *currentPosition;
            currentPosMove->mMoving = true;
            currentPosMove->mStepsLeft = 0;
        }
        else if(!isSamePos(currentPosMove->mPos, *currentPosition)) {
            currentPosMove->mStepMove = (*currentPosition - currentPosMove->mPos) * 0.033333335f;
            currentPosMove->mTarget = *currentPosition;
            if(_5d == true)
            {
                currentPosMove->mStepsLeft = 180;
            }
            else {
                currentPosMove->mStepsLeft = 30;
            }
        }

        if(currentPosMove->mStepsLeft != 0)
        {
            currentPosMove->mStepsLeft--;
            if(currentPosMove->mStepsLeft == 0)
            {
                currentPosMove->mPos = currentPosMove->mTarget;
            }
            else {
                currentPosMove->mPos += currentPosMove->mStepMove;
            }
        }
    }

    Main *main = Main::getAudio();
    CameraMgr* camera = main->getCamera();

    const s32 index = camnum;
    if(index >= camera->mSceneMax)
    {
        return;
    }

    JGeometry::TVec3f &firstPos = _68[camnum].mPos;
    JGeometry::TVec3f &secondPos = _6c[camnum].mPos;

    if(_68[camnum].mMoving == false && _6c[camnum].mMoving == false)
    {
        return;
    }

    if(_68[camnum].mMoving == false)
    {
        mSoundPosition[camnum] = secondPos;
    }
    else if (_6c[camnum].mMoving == false)
    {
        mSoundPosition[camnum] = firstPos;
    }
    else if (!isSamePos(firstPos, secondPos)) {
        const Vec &camPos = Main::getAudio()->getCamera()->getCameraInfo(camnum)->_0;
        const Vec segment = firstPos - secondPos;
        const Vec toCam = firstPos - camPos;
        const f32 dx = firstPos.x - secondPos.x;
        const f32 dy = firstPos.y - secondPos.y;
        const f32 dz = firstPos.z - secondPos.z;
        f32 result = ((segment.x * toCam.x) + (segment.y * toCam.y) + (segment.z * toCam.z)) /
            ((dx * dx) + (dy * dy) + (dz * dz));

        if (result <= 0.f) {
            mSoundPosition[camnum] = firstPos;
        }
        else if (result >= 1.0f)
        {
            mSoundPosition[camnum] = secondPos;
        }
        else {
            const Vec closest = firstPos + ((secondPos - firstPos) * result);
            JGeometry::setTVec3f(&closest.x, &mSoundPosition[camnum].x);
        }
    }
    else {
        mSoundPosition[camnum] = firstPos;
    }
    const u32 soundID = _5d ? 0x40022 : 0x40023;
    JAISoundHandle &handle = (*this)[index];
    JAISoundStarter *starter = JAISoundStarter::getInstance();

    // Why the check here? It cannot be 0
    if (soundID) {
        starter->startSound(soundID, &handle, NULL);
        if (handle->getAudible() == NULL) {
            const u32 otherScreensMask = (1 << index) ^ 0xf;
            JAISe *se = handle.getSound()->asSe();
            se->audience_ = NULL;

            handle->newAudible(JGeometry::TVec3f(
                mSoundPosition[camnum]),
                &mSoundPosition[camnum],
                otherScreensMask,
                smAudience);
        }
    }
}

void GameAudio::ShoreSoundMgr::setStatus(bool p1, f32 p2) {
    _5d = p1;
    _60 = p2;
}

}
