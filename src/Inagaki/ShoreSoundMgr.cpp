
#include "Inagaki/GameAudioCamera.h"
#include "Inagaki/GameSoundMgr.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"


namespace GameAudio {

JGeometry::TVec3f sShoreDummyVec(0, 0, 0);

GameAudio::ShoreSoundMgr::ShoreSoundMgr(JKRHeap *heap) : SoundMgr<ShoreSoundMgr>(&sShoreDummyVec, heap, 4) {
    _5c = 0;
    _64 = new JGeometry::TVec3f[4];
    _68 = new PosMove[4];
    _6c = new PosMove[4];

    for (u8 i = 0; i < 4; i++) {
        _64[i].set(0.0f, 0.0f, 0.0f);
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

void GameAudio::ShoreSoundMgr::setTide(u8, JGeometry::TVec3f *, JGeometry::TVec3f *) {}

void GameAudio::ShoreSoundMgr::setStatus(bool p1, f32 p2) {
    _5d = p1;
    _60 = p2;
}
    
}
#include "JSystem/JAudio/JASFakeMatch.h"

#ifdef MATCHING // TODO

// template <>
// JASMemPool< JAUDopplerAudible<4> > JASPoolAllocObject< JAUDopplerAudible<4> >::memPool_;
// template <>
// JASMemPool<JAISe> JASPoolAllocObject<JAISe>::memPool_;

#endif
