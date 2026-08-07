#include "Inagaki/GameSoundTable.h"
#include "JSystem/JAudio/Interface/JAISound.h"
#include "JSystem/JAudio/JAUSoundTable.h"

#include "JSystem/JAudio/JASFakeMatch11.h"
#include "JSystem/JAudio/System/JASGadget.h"
#include "JSystem/JUtility/JUTAssert.h"

namespace GameAudio {

u16 CustomSoundTable::getBgmSeqResourceID(JAISoundID soundID) const {
    bool resourceNotNull = false;
    if(JASGlobalInstance<JAUSoundTable>::sInstance != NULL
        && JASGlobalInstance<JAUSoundTable>::sInstance->getResource() != NULL)
    {
        resourceNotNull = true;
    }

    // I don't really know where this "isValid" call could come from
    #line 27
    JUT_ASSERT_MSG(resourceNotNull, "isValid()");

    JAUSoundTableItem* tableItem = JASGlobalInstance<JAUSoundTable>::sInstance->getData(soundID);
    const u8 typeId = JASGlobalInstance<JAUSoundTable>::sInstance->getTypeID(soundID);

    if(tableItem != NULL) {
        // Did not succeed to get 100% without this switch...
        switch(typeId) case 0x20:
            return tableItem->_10;
    }
    return -1;
}

u32 CustomSoundTable::getSoundType(JAISoundID soundID) const {}

int CustomSoundTable::getCategory(JAISoundID soundID) const {}

void CustomSoundTable::getAudibleSw(JAISoundID soundID) const {}

u8 CustomSoundTable::getPTrack(JAISoundID soundID) const {}

u32 CustomSoundTable::getSwBit(JAISoundID soundID) const {}

u8 CustomSoundTable::getLevelFlag(JAISoundID soundID) const {}

u32 CustomSoundTable::getPriority(JAISoundID soundID) const {}

void CustomSoundTable::getSeInfo(JAISoundID soundID, JAISe *se) const {}

void CustomSoundTable::getSeqInfo(JAISoundID soundID, JAISeq *seg) const {}

void CustomSoundTable::getStreamInfo(JAISoundID soundID, JAIStream *stream) const {}

JAISoundInfo *CustomSoundTable::getSoundInfo_(JAISoundID soundID, JAISound *sound) const {}

s32 CustomSoundTable::getStreamFileEntry(JAISoundID soundID) {}

void CustomSoundTable::initAudibleSw() {}

}
