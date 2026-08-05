#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "Kameda/Scene.h"
#include "kartEnums.h"

class SceneMenu : public Scene
{
public:
    struct BckData
    {
    };

    struct BrkData
    {
    };

    enum Cursor
    {

    };

    SceneMenu(JKRArchive *, JKRHeap *);  // 0x8016f78c
    virtual ~SceneMenu();                // 0x80173908
    virtual void init();                 // 0x80172e6c
    virtual void draw();                 // 0x80173960
    virtual void calc();                 // 0x80173f08
    
    void reset();                        // 0x801725b8
    void calcModeAnm();                  // 0x801750b0
    void caleEngine(BckData *);          // 0x801751ac
    void calcEngineMoji(BckData *);      // 0x801751ec
    void calcEnginePic(BckData *);       // 0x801752f0
    void calcEngineBrkAnm(BrkData *);    // 0x801753f4
    void calcModeOrder();                // 0x80175434
    void entry_init();                   // 0x801755b4
    void entry();                        // 0x80175970
    void entry_end();                    // 0x80176268
    void vs_init();                      // 0x801762ec
    void vs();                           // 0x80176c5c
    void vs_end();                       // 0x801780bc
    void mode_init();                    // 0x8017836c
    void mode();                         // 0x801797d0
    void mode_end();                     // 0x8017b78c
    void battle_init();                  // 0x8017c834
    void battle();                       // 0x8017cd14
    void battle_end();                   // 0x8017d39c
    void character_init();               // 0x8017d4a0
    void character();                    // 0x8017e3b8
    void character_end();                // 0x8017e86c
    void nextScene();                    // 0x8017ebf4
    void selectA(int);                   // 0x8017ece8
    void selectB(int);                   // 0x8017eefc
    void selectUp(int);                  // 0x8017f17c
    void selectDown(int);                // 0x8017f28c
    void selectLeft(int);                // 0x8017f39c
    void selectRight(int);               // 0x8017f4a0
    void selectZ(int);                   // 0x8017f5a4
    void isSelectAnm(int);               // 0x8017f6f0
    void moveCursor(int, Cursor);        // 0x8017f754
    void isRndRoulette();                // 0x8017fb78
    void rndRoulette();                  // 0x8017fc50
    void rndSelect();                    // 0x8017fce0
    void setCharacterFlag();             // 0x8017fe00
    void getKart(int, int);              // 0x8017ff04
    void changeKart(int, int, int, int); // 0x80180060
    void characterAnm();                 // 0x8018029c
    void characterOkWaitAnm();           // 0x80180ef8
    void setChar1Plus();                 // 0x80181450
    void setRaceData();                  // 0x801817f0
    void setCharSelFlag();               // 0x801828f8
    void useCharSel(int, int);           // 0x80182954
    void getCharSel(int &, int &);       // 0x8018296c
    void getRndCharSel(int &, int &);    // 0x80182d14
    void changeText();                   // 0x80182e8c
private:
    static const u32 mCharVoice[20];             // 0x80371338
    static const u32 mCharGoVoice[20];           // 0x80371388
    static const int mCharIndex[20];             // 0x803713d8
    static const int mKartIndex[21];             // 0x803715d4
    static const u8 mKartPerf[63];               // 0x80371628
    static const ERaceLevel mLevelID[4];         // 0x80371668
    static const u64 mPlayerIconBckTag[33];      // 0x80371678
    static const u64 mHowManyDriversBckTag[13];  // 0x803717b0
    static const u64 mHowManyDriversBrkTag[22];  // 0x80371818
    static const u64 mSingleTagSelectBckTag[43]; // 0x803718c8
    static const u64 mSingleTagSelectBtkTag[24]; // 0x80371a20
    static const u64 mSingleTagSelectBrkTag[78]; // 0x80371ae0
    static const u64 mSingleTagSelectBpkTag[7];  // 0x80371d50
    static const u64 mModeBckTag[75];            // 0x80371d88
    static const u64 mModeBtkTag[26];            // 0x80371fe0
    static const u64 mModeBrkTag[30];            // 0x803720b0
    static const u64 mModeBpkTag[6];             // 0x803721a0
    static const u64 mBattleBckTag[15];          // 0x803722a0
    static const u64 mBattleBrkTag[3];           // 0x80372318
    static const u64 mCharCCBckTag[2];           // 0x80372330
    static const u64 mCharCCBrkTag[4];           // 0x80372340
    static const u64 mCharCCBpkTag[6];           // 0x80372360
    static const u64 mChar1BckTag[9];            // 0x803723f0
    static const u64 mChar1PlusBckTag[19];       // 0x80372438
    static const u64 mChar1PlusBrkTag[2];        // 0x803724d0
    static const u64 mChar2BckTag[20];           // 0x803724e0
    static const u64 mChar2BrkTag[5];            // 0x80372580
    static const u64 mChar3BckTag[27];           // 0x803725a8
    static const u64 mChar3BrkTag[8];            // 0x80372680
    static const u64 mChar4BckTag[26];           // 0x803726c0
    static const char *mCharName[20];            // 0x80395648
    static const char *mPlayerIconPlayerName[4]; // 0x80395698
    static const char *mPowerMojiName[4];        // 0x803956a8
    static const char *mPowerPicName[4];         // 0x803956b8
    static const char *mBattleName[3];           // 0x803956c8
    static const char *mCharCCName[8];           // 0x803956d4
}; // Size: 0x22c0

#endif
