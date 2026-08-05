#include "Inagaki/GameAudioMain.h"

#include "JSystem/JAudio/System/JASAudioThread.h"
#include "JSystem/JAudio/System/JASWaveArcLoader.h"
#include "JSystem/JAudio/System/JASWaveBank.h"

namespace GameAudio {

Main *Main::msBasic;
static void *mLastBaaPointer;

Main::Main() {

}

void Main::init(JKRSolidHeap *, u32, void *, void *, u32) {}

void Main::bootDSP() {
    JASAudioThread::bootDSP();
}

bool Main::isActive() {
    return mState & 0x10;
}

void Main::initRaceSound() {}

void Main::frameWork() {}

void Main::changeSection(u32) {}

void Main::checkSection() {}

bool Main::isWaveLoaded(WS_ID id) {
    JAUSectionHeap* sectionHeap = JASGlobalInstance<JAUSectionHeap>::sInstance;
    if(sectionHeap != nullptr)
    {
        JASWaveBank* bank = sectionHeap->sectionHeapData_.waveBankTable.getWaveBank(id);

        if(bank != nullptr)
        {
            JASWaveArc* arc = bank->getWaveArc(0);
            if (arc->_4c == 2) {
                return true;
            }
        }
    }

    return false;
}

void Main::checkCourceOnRace() {}

void Main::setPause(PAUSE_STATUS) {}

void Main::startSequenceBgm(u32) {}

void Main::stopSequenceBgm(u32) {}

u32 Main::getPlayingSequenceID() {}

void Main::prepareStreamBgm(u32) {}

void Main::startStreamBgm(u32) {}

void Main::stopStreamBgm(u32) {}

u32 Main::getPlayingStreamID() {}

void Main::setInvincibleBgm(u8, u8) {}

void Main::clearInvincibleBgm(u8, u8) {}

void Main::startInvincibleBgm(u8, bool) {}

void Main::checkBgm() {}

void Main::startSystemSe(u32) {}

void Main::stopSystemSe(u32) {}

void Main::startRaceSystemSe(u32) {}

void Main::startKartSystemSe(u8, u32) {}

void Main::checkThunderMode() {}

void Main::setThunderMode(u8) {}

void Main::clearThunderMode(u8) {}

void Main::startBombPointSe(u8, u8) {}

void Main::startBombReachSe(u8, u8) {}

void Main::setMasterVolume(s8) {}

f32 Main::getMasterVolumeValue() {}

void Main::setBgmVolume(f32) {}

void Main::updateBgmVolume(u32) {}

void Main::fadeOutAll(u32) {}

void Main::setRaceSeVolume(f32, u32) {}

void Main::setOutputMode(u32) {}

void Main::resetAudio(u32) {}

void Main::resumeAudio() {}

void Main::checkResetInner() {}

void Main::setGoalProcess() {}

void Main::processSeqCallback(JASTrack *, u16) {}


}
