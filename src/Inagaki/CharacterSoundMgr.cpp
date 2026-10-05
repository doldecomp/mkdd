#include "Inagaki/GameAudioAnimator.h"
#include "Inagaki/GameAudioCamera.h"
#include "Inagaki/GameAudioCommon.h"
#include "Inagaki/GameAudioMain.h"
#include "Inagaki/GameSoundMgr.h"
#include "Inagaki/GameSoundTable.h"
#include "JSystem/JAudio/Interface/JAISound.h"
#include "JSystem/JAudio/Interface/JAISoundHandles.h"
#include "JSystem/JAudio/JAUSoundAnimator.h"
#include "JSystem/JAudio/JAUSoundObject.h"
#include "JSystem/JGeometry/Vec.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "kartEnums.h"
#include "types.h"

#include "JSystem/JAudio/JASFakeMatch4.h"

//JASGlobalInstance<JAWSpeAna>::sInstance // UNUSED
//JASGlobalInstance<JAWSeqEdit>::sInstance // UNUSED

namespace GameAudio {

CharacterSoundMgr::CharacterSoundMgr(Vec *pos, JKRHeap *heap, u8 p3, ECharID charId) : SoundMgr<CharacterSoundMgr>(pos, heap, 8) {
    mAnimator = new (heap, 0) CustomAnimator(this);
    mStarter = new (heap, 0) CustomAnimationSoundStarter();
    _C = 0x80; // struct size?
    _64 = p3;
    mCharID = charId;
    mKartSound = nullptr;
    getKartSound();
    init();
}

CharacterSoundMgr::~CharacterSoundMgr() {}

void CharacterSoundMgr::init() {
    _78 = 0;
    _6c = 0;
    _70 = 1.0f;
    _68 = 360;
    _65 = 0xff;
    mAnimator->startAnimation(nullptr, false, 0.0f, 0.0f);
}

void CharacterSoundMgr::loop() {
#line 152
    JUT_ASSERT_MSG(mSoundPos, "CharacterSoundMgr::loop　disable中です！！！！！\n");
    if (mKillSw)
        return;

    JAUDopplerSoundObject::process();
    checkEcho();
    if (_68 < 360) {
        _68++;
    }
}

void CharacterSoundMgr::frameWork(f32 p1, f32) {
    KartSoundMgr *kartSound = getKartSound();

    if (mKillSw != 0 || kartSound->mKartType == 2) {
        return;
    }
    mAnimator->updateAnimation(p1, *mSoundPos, mStarter);
}

void CharacterSoundMgr::setVoice(u8 voiceID) {
    KartSoundMgr *kartSound = getKartSound();

    int charIdx;
    switch (mCharID) {
    case BABY_MARIO:
        charIdx = 0;
        break;
    case BABY_LUIGI:
        charIdx = 1;
        break;
    case PARATROOPA:
        charIdx = 2;
        break;
    case KOOPATROOPA:
        charIdx = 3;
        break;
    case PEACH:
        charIdx = 4;
        break;
    case DAISY:
        charIdx = 5;
        break;
    case MARIO:
        charIdx = 6;
        break;
    case LUIGI:
        charIdx = 7;
        break;
    case WARIO:
        charIdx = 8;
        break;
    case WALUIGI:
        charIdx = 9;
        break;
    case YOSHI:
        charIdx = 10;
        break;
    case BIRDO:
        charIdx = 0xb;
        break;
    case DONKEY_KONG:
        charIdx = 0xc;
        break;
    case DIDDY_KONG:
        charIdx = 0xd;
        break;
    case BOWSER:
        charIdx = 0xe;
        break;
    case BOWSER_JR:
        charIdx = 0xf;
        break;
    case TOAD:
        charIdx = 0x10;
        break;
    case TOADETTE:
        charIdx = 0x11;
        break;
    case KING_BOO:
        charIdx = 0x13;
        break;
    case PETEY:
        charIdx = 0x12;
        break;
    default:
        JUT_REPORT_MSG("CharacterSoundMgr::setVoice 不正なキャラクターIDです\n");
        return;
    }

    u32 soundID = voiceID;
    soundID += 0x30000 + charIdx * 0x29;
    _6c = Common::changeRandomId(soundID, _6c);

    if (mKillSw || kartSound->mKartType == 2) {
        return;
    }

    CustomSoundTable *soundTable = Main::getAudio()->getSoundTable();
    u32 swBit = soundTable->getSwBit(soundID);

    if (kartSound && kartSound->mConductStatus == 3 && swBit & 0x8000000) {
        return;
    }

    if (kartSound->mKartType != 0 && (swBit & 0x80000000)) {
        return;
    }

    JAISoundStarter *starter = JAISoundStarter::getInstance();
    JAISoundHandles *handles = this;
    JAISoundHandle *handle = &(*handles)[0];
    
    if (_65 == voiceID && (swBit & 0x7000000)) {
        switch (swBit & 0x7000000) {
        case 0x1000000: {
            if (_68 < 18)
                return;
            break;
        }
        case 0x2000000: {
            if (_68 < 42)
                return;
            break;
        }
        case 0x3000000: {
            if (_68 < 66) {
                return;
            }
            break;
        }
        case 0x4000000:
            if (*handle) {
                return;
            }
            break;
        default:
#line 306
            JUT_WARNING_F2("&s", "setVoice : Iregal SwBit\n");
        }
    }

    if (*handle) {
        if (soundTable->getPriority((*handle)->getID()) < soundTable->getPriority(soundID)) {
            soundTable->getPriority(soundID);
            (*handle)->getID(); // some getter was used here, but unusd
            soundTable->getPriority((*handle)->getID());
            return;
        }
    }

    _65 = voiceID;
    _68 = 0;

    starter->startSound(_6c, handle, nullptr);

    if (!(*handle)) {
        return;
    }

    kartSound = getKartSound(); // ?
    if (kartSound) {
        setEcho(handle, kartSound->mEchoMix);            
    }

    if (Parameters::getChibiFlag(_64)) {
        JAISound *sound = handle->operator->();
        sound->getAuxiliary().movePitch(Parameters::getChibiPitch(_6c), 0);
    }

    if (!(*handle)->getAudible()) {
        u32 unk = 0;            
        u32 cam8 = Main::getAudio()->getCamera()->getSceneMax(); // number of cameras?
        u8 kartCount = kartSound->mKartIndex;
        if (cam8 > 1 && cam8 > kartCount) {
            unk = (1 << kartCount) ^ 0xf; // what
        }
        (*handle)->newAudible(JGeometry::TVec3f(*mSoundPos), &_18, unk, nullptr);

        if (kartSound->mKartType) {
            (*handle)->getAuxiliary().moveVolume(0.55f, 0);
        }
    }
}

void CharacterSoundMgr::setSe(u32 seID) {
    JAISoundHandle *handle;
    KartSoundMgr *kartSound = getKartSound();
    
    if (mKillSw != 0 || kartSound->mKartType == 2)
        return;

    if (kartSound && kartSound->mConductStatus == 3 && Main::getAudio()->getSoundTable()->getSwBit(seID) & 0x8000000) {
        return;
    }

    u32 unk = 0;
    u32 cam8 = Main::getAudio()->getCamera()->getSceneMax(); // number of cameras?
    u8 kartCount = kartSound->mKartIndex;
    if (cam8 > 1 && cam8 > kartCount) {
        unk = 1 << kartCount ^ 0xf; // what
    } 

    handle = startSoundCustom(seID, unk);

    if (Parameters::getChibiFlag(_64)) {
        JAISound *sound = handle->operator->();
        sound->getAuxiliary().movePitch(Parameters::getChibiPitch(seID), 0);
    }

    if (kartSound) {
        setEcho(handle, kartSound->mEchoMix);
    }
}

bool CharacterSoundMgr::isSpeak() {
    JAISoundHandles *handles = this;
    if ((*handles)[0]) {
        return true;
    }
    return false;
}

void CharacterSoundMgr::setVoicePitch(bool chibi) {
    JAISoundHandles *handles = this;
    JAISoundHandle &handle = (*handles)[0];
    if (handle) {
        if (chibi) {
            f32 pitch = Parameters::getChibiPitch(handle->getID());
            handle->getAuxiliary().movePitch(pitch, 20);
        }
        else {
            handle->getAuxiliary().movePitch(1.0f, 20);
        }
    }
}

KartSoundMgr *CharacterSoundMgr::getKartSound() {
    if (mKartSound && KartSoundMgr::smStart) {
        return mKartSound;
    }
    KartSoundMgr *kartSound = KartSoundMgr::smStart;
    while (kartSound) {
        if (_64 == kartSound->mKartIndex) {
            mKartSound = kartSound;
            return mKartSound;
        }
        kartSound = kartSound->mNext;
    }
    return nullptr;
}

void CharacterSoundMgr::checkEcho() {
    KartSoundMgr *kartSoundMgr = getKartSound();
    
    if (!kartSoundMgr) {
        return;
    }

    f32 echoVal = mKartSound->mEchoMix;
    if (_70 == echoVal) {
        return;
    }

    bool alternator = false;
    JAISoundHandles *handle = this;
    for (int i = 0; i < _10; i++) {
        if ((*handle)[i]) {
            ((*handle)[i])->getAuxiliary().moveFxMix(echoVal, 0);   
        }
    }
    _70 = echoVal;
}

CustomAnimationSoundStarter::CustomAnimationSoundStarter() : JAISoundStarter(false) {
    _4 = 4;
}

bool CustomAnimationSoundStarter::startSound(JAISoundID soundID, JAISoundHandle *handlePtr, const JGeometry::TVec3f *posPtr) {
    bool ret = JAISoundStarter::getInstance()->startSound(soundID, handlePtr, nullptr);
    if (*handlePtr) {
        u32 unk = 0;
    
        u32 cam8 = Main::getAudio()->getCamera()->getSceneMax(); // number of cameras?
        if (cam8 > 1 && cam8 > _4) {
            unk = 1 << _4 ^ 0xf; // what
        }
        if (!(*handlePtr)->getAudible()) {
            (*handlePtr)->newAudible(*posPtr, posPtr, unk, nullptr);
        }
    }
    
    return ret;
}

CustomAnimator::CustomAnimator(JAISoundHandles *handles) : JAUSoundAnimator(handles) {}

JAISoundHandle *CustomAnimator::getFreeHandle(const JAUSoundAnimationSound *animSound) {
    CharacterSoundMgr *charSoundMgr = (CharacterSoundMgr*)handles_;
    CustomSoundTable *soundTable = Main::getAudio()->getSoundTable();
    KartSoundMgr *kartSoundMgr = charSoundMgr->getKartSound();

    if (kartSoundMgr->mKartType && soundTable->getSwBit(animSound->mSoundId) & 0x80000000) {
        return nullptr;
    }

    if ((animSound->mSoundId.mId.mFullId & 0xf0000) == 0x30000) { // Hmmm, this might be something else
        return &(*handles_)[0];
    }

    for (int i = 1; i < handles_->getNumHandles(); i++) {
        if (!(*handles_)[i]) {
            return &(*handles_)[i];
        }
    }
    return nullptr;
}

void CustomAnimator::modifySoundParams(JASSoundParams *params, const JAUSoundAnimationSound *animSound, f32 duration) {
    JAUSoundAnimator::modifySoundParams(params, animSound, duration);
    CharacterSoundMgr *charSoundMgr = (CharacterSoundMgr*)handles_;
    if (Parameters::getChibiFlag(charSoundMgr->_64)) {
        params->mPitch *= Parameters::getChibiPitch(animSound->mSoundId);
    }
}


}
