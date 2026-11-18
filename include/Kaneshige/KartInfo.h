
#ifndef KARTINFO_H
#define KARTINFO_H

#include <dolphin/os.h>
#include <JSystem/JUtility/JUTDbg.h>

#include "JSystem/JUtility/JUTAssert.h"
#include "Osako/kartPad.h"
#include "types.h"

#include "kartEnums.h"

class KartInfo 
{
public:
    struct SCharDB
    {
        u16 id;
        u16 defaultPartnerID;
        u16 weight;
        u16 defaultKartID;
        u8 _8;
        u8 _9;
        //u8 _9[3]; // this is padding data most likely
    };

    struct SKartDB
    {
        EKartID id;
        EKartWeight weight;
        u16 wheelCount;
        s8 _a;
        u16 defaultDriverID;
        u16 _e;
    };

    class KartCharacter {
    public:
        KartCharacter() {
            reset();
        }
        ~KartCharacter() {};
        void reset() {
            mKartGamePad = nullptr;
            mCharDB = nullptr;
        }

        bool isComPlayer() const { return mKartGamePad == nullptr; }
        KartGamePad *getPad() { return mKartGamePad; }
        void setPad(KartGamePad * gamepad);
        void setCharDB(const SCharDB  * charDB) { mCharDB = charDB; }
        ECharID getCharID() const{ return (mCharDB) ? (ECharID)mCharDB->id : cCharIDNone; }
        ECharID getPartnerID() const { return (mCharDB) ? (ECharID)mCharDB->defaultPartnerID : cCharIDNone; }
        
        int getPlayerKind() const { return convPlayerKind(mKartGamePad); }
        
        // MJB - Why these are declared here I have absolutely no clue...
        int getWeight() const { return (mCharDB) ? mCharDB->weight : 0; }
        bool isAvailable() const;
        static int convPlayerKind(KartGamePad *);
    private:
        KartGamePad* mKartGamePad; // inherited from JUTGamePad
        const SCharDB* mCharDB;
    };
    KartInfo();
    ~KartInfo();

    void reset();

    static const SCharDB * getCharDB(ECharID charID);
    void setDriver(int driverNo, ECharID charID, KartGamePad * gamePad);
    static const SKartDB * getKartDB(EKartID kartID);
    static EKartWeight getKartWeight(EKartID);
    static ECharID getDefaultDriver(EKartID);
    static ECharID getDefaultPartner(ECharID);
    static int getDriverWeight(ECharID);
    static EKartID getDefaultKartID(ECharID);
    static EKartID getPartnerKartID(ECharID);
    bool isDefaultCharCombi();
    KartGamePad * getYoungestPad();

    void setGhostKind(EGhostKind kind) { mGhostKind = kind; }
    EGhostKind getGhostKind() { return mGhostKind; }

    bool isAvailableKart() const { return isAvailableDriver(0); }
    bool isAvailableDriver(int driverNo) const {
#line 107
        JUT_MINMAX_ASSERT(0, driverNo, 2);
        return mKartCharacters[driverNo].isAvailable();
    }

    bool isComDriver(int driverNo) const {
#line 113
        JUT_MINMAX_ASSERT(0, driverNo, 2);
        return mKartCharacters[driverNo].isComPlayer();
    }

    // Enum?
    int getPlayerKind(int driverNo) const { 
        JUT_MINMAX_ASSERT(0, driverNo, 2);
        return mKartCharacters[driverNo].getPlayerKind();
    }

    KartGamePad *getPad(int driverNo) {
#line 126
        JUT_MINMAX_ASSERT(0, driverNo, 2);
        return mKartCharacters[driverNo].getPad();
    }

    KartGamePad *getCoPad() { return mKartCharacters[1].getPad(); }

    ECharID getDriverCharID(int driverNo) const {
#line 142
        JUT_MINMAX_ASSERT(0, driverNo, 2);
        return mKartCharacters[driverNo].getCharID();
    }

    int getDriverWeight(int driverNo) const {
#line 148
        JUT_MINMAX_ASSERT(0, driverNo, 2);
        return mKartCharacters[driverNo].getWeight();
    }

    EKartID getKartID() const { return (mKartDB) ? mKartDB->id : cKartIDNone; }
    EKartWeight getKartWeight() const { return mKartDB ? mKartDB->weight : UNK_3; }
    int getWheelNumber() const { return mKartDB ? mKartDB->wheelCount : 0; }
    bool isComKart() const { return isComDriver(0); }
    void setKartID(EKartID kartID) { mKartDB = getKartDB(kartID); }
    bool isRealPlayerKart() const { return getPlayerKind(0) == 1; }
    bool isGhostKart() const { return getPlayerKind(0) == 4; }
    bool isPlayerKart() const { return !isComKart(); }
    bool isTagMode() const { return (mKartCharacters) != 0; }
    int getWeight() const { return (mKartDB) ? mKartDB->weight : 3; }

    static const SCharDB cBabyMarioCharDB;
    static const SCharDB cBabyLuigiCharDB;
    static const SCharDB cPatapataCharDB;
    static const SCharDB cNokonokoCharDB;
    static const SCharDB cPeachCharDB;
    static const SCharDB cDaisyCharDB;
    static const SCharDB cMarioCharDB;
    static const SCharDB cLuigiCharDB;
    static const SCharDB cWarioCharDB;
    static const SCharDB cWaluigiCharDB;
    static const SCharDB cDonkeyCharDB;
    static const SCharDB cDonkeyJrCharDB;
    static const SCharDB cYoshiCharDB;
    static const SCharDB cCatherineCharDB;
    static const SCharDB cKoopaCharDB;
    static const SCharDB cKoopaJrCharDB;
    static const SCharDB cKinopioCharDB;
    static const SCharDB cKinopicoCharDB;
    static const SCharDB cKingTeresaCharDB;
    static const SCharDB cBossPakkunCharDB;
    static const SKartDB cMustacheKartDB;
    static const SKartDB cMonkeyKartDB;
    static const SKartDB cLizardKartDB;
    static const SKartDB cSmallFishKartDB;
    static const SKartDB cPrincessKartDB;
    static const SKartDB cChildKartDB;
    static const SKartDB cBadKartDB;
    static const SKartDB cSatanKartDB;
    static const SKartDB cLuigiKartDB;
    static const SKartDB cDiddyKartDB;
    static const SKartDB cCatherineKartDB;
    static const SKartDB cPatapataKartDB;
    static const SKartDB cDaisyKartDB;
    static const SKartDB cBabyLuigiKartDB;
    static const SKartDB cWaluigiKartDB;
    static const SKartDB cKoopaJrKartDB;
    static const SKartDB cKinopioKartDB;
    static const SKartDB cKinopicoKartDB;
    static const SKartDB cKingTeresaKartDB;
    static const SKartDB cBossPakkunKartDB;
    static const SKartDB cBonusKartDB;

private:
    const SKartDB *mKartDB;
    KartCharacter mKartCharacters[2]; // one for the driver, other for the one doing nothing
    EGhostKind mGhostKind;            // if this is set to 1 this means the driver is a ghost, 2 is also used for ghost but for the pad that gets recorded, so that means 2 is invisible?
};

#endif // !KARTINFO_H
