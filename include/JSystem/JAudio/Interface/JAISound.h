#ifndef JAUDIO_JAISOUND_H
#define JAUDIO_JAISOUND_H

#include "JSystem/JUtility/JUTAssert.h"
#include "JSystem/JUtility/JUTDbg.h"
#include "JSystem/JAudio/Interface/JAIAudible.h"
#include "JSystem/JAudio/Interface/JAISoundParams.h"
#include "JSystem/JAudio/System/JASGadget.h"
#include "JSystem/JAudio/System/JASTrack.h"

#include "types.h"

class JAISound;

class JAISoundHandle
{
public:
    JAISoundHandle() { sound_ = NULL; }
    ~JAISoundHandle() {
        releaseSound();
    }

    void releaseSound();

    JAISound *getSound()
    {
#line 41
        JUT_ASSERT(sound_ != 0);
        return sound_;
    }

    JAISound *operator->()
    {
#line 58
        JUT_ASSERT(sound_ != 0)
        return sound_;
    }

    operator bool() const { return isSoundAttached(); }

    bool isSoundAttached() const { return sound_ != NULL; }

    JAISound *sound_;
};

class JAISoundID
{
public:
    operator u32() const { return mId.mFullId; }
    //void operator=(const JAISoundID &other) { mId.mFullId = other.mId.mFullId; };

    JAISoundID(u32 pId) { mId.mFullId = pId; };
    JAISoundID(const JAISoundID &other) { mId.mFullId = other.mId.mFullId; };

    JAISoundID() {}

    bool isAnonymous() const { return mId.mFullId == 0xffffffffUL; }
    void setAnonymous() { mId.mFullId = 0xffffffffUL; }

    mutable union // mutable... this smells fake but so far matches every file
    // if something doesn't match because of this, mark JAUSeqDataBlockMgr.cpp as Equivalent
    {
        u32 mFullId;
        struct
        {
            u8 mSectionId;
            u8 mGroupId;
            u8 b2;
            u8 b3;
        } mBytes;
        struct
        {
            u16 mSoundType;
            u16 mShortId;
        } mAdvancedId; // Debug doesn't have an inline for referencing the short ID so I assume
                       // it's similar to this
    } mId;
};

class JAISe;
class JAISeq;
class JAIStream;

struct JAISoundInfo : public JASGlobalInstance<JAISoundInfo>
{
    JAISoundInfo(bool);

    virtual u32 getSoundType(JAISoundID soundID) const = 0;
    virtual int getCategory(JAISoundID soundID) const = 0;
    virtual u32 getPriority(JAISoundID soundID) const = 0;
    virtual void getSeInfo(JAISoundID soundID, JAISe *se) const = 0;
    virtual void getSeqInfo(JAISoundID soundID, JAISeq *seq) const = 0;
    virtual void getStreamInfo(JAISoundID soundID, JAIStream *stream) const = 0;
    virtual ~JAISoundInfo();
};

struct JAISoundStarter : public JASGlobalInstance<JAISoundStarter>
{
    JAISoundStarter(bool);

    virtual ~JAISoundStarter();
    virtual bool startSound(JAISoundID soundID, JAISoundHandle *handlePtr, const JGeometry::TVec3f *pos) = 0;
    bool startLevelSound(JAISoundID soundID, JAISoundHandle *handlePtr, const JGeometry::TVec3f *pos);
};

struct JAISoundStatus_
{
    void init()
    {
        _0.pack = 0;
        _1.pack = 0;
        packedState = 0;
        user_data = 0;
    }

    bool isAlive() const { return state.unk != 6; }
    bool isDead() { return state.unk == 6; }
    bool isPlaying() { return state.unk == 5; }
    bool isMute() { return _0.flags.mute; }
    bool isPaused() { return _0.flags.paused; }
    void pauseWhenOut() { _1.flags.flag6 = 1; }

    s32 lockWhenPrepared();
    s32 unlockIfLocked();

    union
    {
        struct
        {
            u8 mute : 1;
            u8 paused : 1;
            u8 flag3 : 1;
            u8 flag4 : 1;
            u8 flag5 : 1;
            u8 flag6 : 1;
            u8 flag7 : 1;
            u8 flag8 : 1;
        } flags;
        u8 pack;
    } _0;

    union
    {
        struct
        {
            u8 flag1 : 1;
            u8 flag2 : 1;
            u8 flag3 : 1;
            u8 flag4 : 1;
            u8 flag5 : 1;
            u8 flag6 : 1;
            u8 flag7 : 1;
            u8 flag8 : 1;
        } flags;
        u8 pack;
    } _1;

    union 
    {
        struct
        {
            u8 unk;
            struct
            {
                u8 flag1 : 1;
                u8 calcedOnce : 1;
                u8 animationState : 2;
                u8 flag5 : 1;
                u8 flag6 : 1;
                u8 flag7 : 1;
                u8 flag8 : 1;
            } flags;
        } state;
        u16 packedState;
    };
    
    u32 user_data; // 4
};

class JAISoundActivity
{
public:
    void init() { _0.value = 0; }

    union
    {
        u8 value;
        struct
        {
            u8 flag1 : 1;
            u8 flag2 : 1;
            u8 flag3 : 1;
            u8 flag4 : 1;
            u8 flag5 : 1;
            u8 flag6 : 1;
            u8 flag7 : 1;
            u8 flag8 : 1;
        } flags;
    } _0;
};

struct JAISoundFader
{
    void forceIn()
    {
        mIntensity = 1.0f;
        mTransition.zero();
    }
    void forceOut()
    {
        mIntensity = 0.0f;
        mTransition.zero();
    }
    void fadeIn(u32 fadeCount)
    {
        if (fadeCount == 0)
            forceIn();
        else
            mTransition.set(1.0f, mIntensity, fadeCount);
    }
    void fadeOut(u32 fadeCount)
    {
        if (fadeCount != 0)
            mTransition.set(0.0f, mIntensity, fadeCount);
        else
            forceOut();
    }
    void fadeInFromOut(u32 fadeCount)
    {
        mIntensity = 0.0f;
        fadeIn(fadeCount);
    }

    // Same as fadeInFromOut but branching order is inversed
    // Needed by KartSoundMgr...
    void fadeInFromOut2(u32 fadeCount)
    {
        mIntensity = 0.f;
        if (fadeCount != 0) {
            mTransition.set(1.0f, mIntensity, fadeCount);
        }
        else
        {
            forceIn();
        }
    }
    bool isOut() { return (mTransition.mCount == 0 && mIntensity < 0.01f); }
    void calc() { mIntensity = mTransition.apply(mIntensity); }
    f32 getIntensity() { return mIntensity; }

    f32 mIntensity;
    JAISoundParamsTransition::TTransition mTransition;
}; // Size: 0x10

template <typename A0>
struct JAISoundStrategyMgr__unknown
{
    virtual void virtual2();
    virtual void calc(A0 *);
    virtual void virtual4(A0 *, const JASSoundParams &);
};

template <typename A0>
struct JAISoundStrategyMgr
{
    virtual void virtual2();
    virtual JAISoundStrategyMgr__unknown<A0> *calc(JAISoundID);
    virtual void virtual4(JAISoundStrategyMgr__unknown<A0> *);
};

class JAITempoMgr
{
public:
    f32 mTempo;                                        // 00
    JAISoundParamsTransition::TTransition mTransition; // 04

    JAITempoMgr() { init(); }
    void init() { setTempo(1.0f); }
    void setTempo(f32 param_0)
    {
        mTempo = param_0;
        mTransition.zero();
    }
    f32 getTempo() { return mTempo; }
    void calc() { mTempo = mTransition.apply(mTempo); }
};

class JAISoundChild : JASPoolAllocObject<JAISoundChild>
{
public:
    void init();
    void mixOut(JASTrack *track);
    void calc();

    JAISoundChild() { init(); }

    JAISoundParamsMove mMove; // 00
    JASSoundParams mParams;   // 50
}; // Size: 0x64

class JAIAudible;
class JAIAudience;
class JAISe;
class JAISeq;
class JAIStream;

class JAISound
{
public:
    void releaseHandle();
    void attachHandle(JAISoundHandle *handle);
    JAISound();
    void start_JAISound_(JAISoundID soundID, const JGeometry::TVec3f *pos, JAIAudience *audience);
    bool acceptsNewAudible() const;
    void newAudible(const JGeometry::TVec3f &, const JGeometry::TVec3f *, u32,
                    JAIAudience *audience);
    void stop();
    void stop(u32 fadeout);
    void die_JAISound_();
    void increasePrepareCount_JAISound_();
    bool calc_JAISound_();
    void initTrack_JAISound_(JASTrack *track);

    virtual s32 getNumChild() const = 0;
    virtual JAISoundChild *getChild(int childNo) = 0;
    virtual void releaseChild(int) = 0;
    virtual JAISe *asSe();
    virtual JAISeq *asSeq();
    virtual JAIStream *asStream();
    virtual JASTrack *getTrack() = 0;
    virtual JASTrack *getChildTrack(int) = 0;
    virtual JAITempoMgr *getTempoMgr() = 0;
    virtual bool JAISound_tryDie_() = 0;

    const JAISoundID &getID() const { return soundID_; }
    u8 getAnimationState() const { return status_.state.flags.animationState; }
    bool isAnimated() const { return getAnimationState() != 0; }
    void setAnimationState(u8 state) { status_.state.flags.animationState = state; }
    u32 getUserData() const { return status_.user_data; }
    void setUserData(u32 userData) { status_.user_data = userData; }
    JAIAudible *getAudible() const { return audible_; }
    bool isHandleAttached() const { return handle_ != NULL; }
    bool hasLifeTime() const { return status_._1.flags.flag2; }
    void removeLifeTime_() { status_._1.flags.flag1 = false; }

    void setLifeTime(u32 newLifeTime, bool pause)
    {
#line 333
        JUT_ASSERT(status_.state.flags.calcedOnce == 0);
        lifeTime_ = newLifeTime;
        setComesBack(pause);
        status_._1.flags.flag2 = 1;
    }

    void setComesBack(bool pause)
    {
#line 354
        JUT_ASSERT(status_.state.flags.calcedOnce == 0)
        status_._1.flags.flag1 = 1;
        if (pause)
        {
            status_.pauseWhenOut();
        }
    }

    bool setPos(const JGeometry::TVec3f &pos)
    {
        if (audible_ != NULL)
        {
            audible_->setPos(pos);
        }
        return audible_ != NULL;
    }

    void stop_JAISound_()
    {
        status_.state.flags.flag5 = 0;
        status_.state.flags.flag1 = 1;
    }
    bool isStopping()
    {
        return status_.state.flags.flag1 && (!status_.state.flags.flag5 || fader_.isOut());
    }

    void pause(bool doPause)
    {
        status_._0.flags.paused = doPause;
    }

    void updateLifeTime(u32 newLifeTime)
    {
        if (lifeTime_ < newLifeTime)
        {
            lifeTime_ = newLifeTime;
        }
    }

    void fadeIn(u32 fadeCount)
    {
        fader_.fadeInFromOut(fadeCount);
        return;
    }

    s32 getCount() const { return count_; }

    JAISoundParamsMove &getAuxiliary() { return params_.mMove; }

    // private:
    JAISoundHandle *handle_; // 04
    JAIAudible *audible_; // 08
    JAIAudience *audience_; // 0c
    u32 lifeTime_; // 10
    s32 prepareCount_; // 14
    JAISoundID soundID_; // 18
    JAISoundStatus_ status_; // 1c
    JAISoundFader fader_; // 24
    u32 priority_; // 34
    s32 count_; // 38
    JAISoundParams params_; // 3a
}; // Size: 0x98

#endif
