
#include "Inagaki/GameAudioCamera.h"
#include "Inagaki/GameMapSoundMgr.h"

#include "Inagaki/GameSoundMgr.h"
#include "JSystem/JAudio/JASFakeMatch4.h"

namespace GameAudio {

MapSoundObjectMgr::MapSoundObjectMgr(Vec *pos, Type type, JKRHeap *heap) : SoundWithEchoMgr<MapSoundObjectMgr>(pos, heap, 2) {
    _68 = 0;
    mType = type;
}

MapSoundObjectMgr::~MapSoundObjectMgr() {}

void MapSoundObjectMgr::frameWork() {
    if (mKillSw) {
        return;
    }
    
    switch (mType) {
    case MapSound_Type0:
        setSe(0x40053);
        break;
    case MapSound_Type1: {
        int rank = getPlayerGoalRank();
        if (rank == 2) {
            setSe(0x40054);
        }
        else if (rank == 0) {
            setSe(0x40066);
        }
        else {
            setSe(0x40069);
        }
        break;
    }
    case MapSound_Type2: {
        int rank = getPlayerGoalRank();
        if (rank == 2) {
            setSe(0x40055);
        }
        else if (rank == 0) {
            setSe(0x40067);
        }
        else {
            setSe(0x4006a);
        }
        break;
    }
    case MapSound_Type3: {
        int rank = getPlayerGoalRank();
        if (rank == 2) {
            setSe(0x40056);
        }
        else if (rank == 0) {
            setSe(0x40068);
        }
        else {
            setSe(0x4006b);
        }
        break;
    }
    case MapSound_Type4: {
        int rank = getPlayerGoalRank();
        if (rank == 2) {
            setSe(0x4006c);
        }
        else if (rank == 0) {
            setSe(0x4006d);
        }
        else {
            setSe(0x4006e);
        }
        break;
    }
    case MapSound_Type13:
        setSe(0x4002a);
        break;
    case MapSound_Type14:
        setSe(0x4006f);
        break;
    case MapSound_Type5:
        setSe(0x40083);
        break;
    case MapSound_Type6:
        setSe(0x40084);
        break;
    case MapSound_Type7:
        setSe(0x40087);
        break;
    case MapSound_Type8:
        setSe(0x40088);
        break;
    case MapSound_Type9:
        setSe(0x40085);
        break;
    case MapSound_Type10:
        setSe(0x40086);
        break;
    case MapSound_Type11:
        setSe(0x40089);
        break;
    case MapSound_Type12:
        setSe(0x4008a);
        break;
    case MapSound_Type15:
        setSe(0x4008d);
        break;
    case MapSound_Type16:
        setSe(0x4008c);
    }
    _68 = 1;
}

int MapSoundObjectMgr::getPlayerGoalRank() {
    u32 n = Main::getAudio()->getCamera()->getSceneMax();
    KartSoundMgr *soundMgr = KartSoundMgr::smStart;
    for (int i = 0; soundMgr && i < n; i++, soundMgr = soundMgr->mNext) {
        if (soundMgr->mPrevConductStatus == 3) {
            return (soundMgr->mRank >= 5) ? 1 : 0;
        }
    }
    return 2;
}
}
