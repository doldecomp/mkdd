#ifndef RACE2D_H
#define RACE2D_H

#include <JSystem/JKernel/JKRHeap.h>
#include <JSystem/J2D/J2DPicture.h>
#include "Kaneshige/KartInfo.h"

class Race2D
{ 
public:
    Race2D(JKRHeap *);                                                                            // 0x801531b8
    void init();                                                                                  // 0x80157714
    void drawPlayerMark();                                                                        // 0x8015838c
    void drawMask();                                                                              // 0x801588b0
    void draw();                                                                                  // 0x80158998
    void drawAlarm();                                                                             // 0x80159a90
    void anmTA(int);                                                                              // 0x80159f00
    void getAlphaAnmTA(int, u8 &);                                                     // 0x8015a688
    void getBestTimeColor(int, JUtility::TColor &, J2DPicture::TCornerColor &);                   // 0x8015a758
    void getBestLapColor(int, JUtility::TColor &);                                                // 0x8015a874
    void drawMiniGame();                                                                          // 0x8015a90c
    void drawBomb();                                                                              // 0x8015a978
    void getBombScale(int, int, int);                                                             // 0x8015af3c
    void getReachBombScale(int, int);                                                             // 0x8015b13c
    void drawEscape();                                                                            // 0x8015b1e8
    void getStartShineTimeScale(int);                                                             // 0x8015b42c
    void drawCourse();                                                                            // 0x8015b4b8
    void calc();                                                                                  // 0x8015c7ec
    void calcPlayerMark();                                                                        // 0x8015cfdc
    void calcParticle();                                                                          // 0x8015d394
    void setLayoutData();                                                                         // 0x8015da18
    void calcMiniGame();                                                                          // 0x8015f688
    void calcBomb();                                                                              // 0x8015f6f4
    void getBombColor(JUtility::TColor, JUtility::TColor);                                        // 0x80160590
    void isBombReach(int);                                                                        // 0x80160708
    void isBombWinner(int);                                                                       // 0x80160864
    void setEffectPos(int);                                                                       // 0x80160980
    void calcEscape();                                                                            // 0x80160c14
    void calcTimeFrame(bool, int);                                                                // 0x80161114
    void getCornerColor(int);                                                                     // 0x8016152c
    void setEscapeColor(int);                                                                     // 0x80161894
    void getTimeColor(JUtility::TColor, JUtility::TColor);                                        // 0x80162844
    void startBombDemo();                                                                         // 0x801629bc
    void isAlarm(int);                                                                            // 0x801629e0
    void getMapPos(int, JGeometry::TVec3f &, JGeometry::TVec2f &);                    // 0x80162a40
    void getCharacterInfo(int, int, f32 &, f32 &, f32 &);                                   // 0x80162dac
    void getCharacterClr(int, int, int, JUtility::TColor &, JUtility::TColor &, u8 &); // 0x80162ec8
    void getItemInfo(int, int, int, f32 &, f32 &, f32 &);                                   // 0x801635d0
    void decideItem(int, u8, int &);                                                   // 0x8016374c
    static void calcLapInit();                                                         // 0x80163b74
    static void calcLap();                                                                        // 0x80163c00
    static void lapInit();                                                                        // 0x80164128
    static void lapMain();                                                                        // 0x80164168
    static void lapTimeInit();                                                                           // 0x80164188
    static void lapTimeMain();                                                                           // 0x8016451c
    static void calcItem();                                                                              // 0x801651b4
    static void itemStartInit();                                                                         // 0x801655d0
    static void itemStart();                                                                             // 0x80165620
    static void itemBlinkInit();                                                                         // 0x80166348
    static void itemBlink();                                                                             // 0x80166398
    static void itemGetInit();                                                                    // 0x80166dd8
    static void itemGetMain();                                                                    // 0x80166e2c
    static void itemSpecialInit();                                                                // 0x80167464
    static void itemSpecialMain();                                                                // 0x801674a4
    void setUsisi(int, ECharID);                                                                  // 0x80167b08
    void alarmX(int, f32 &);                                                                    // 0x80167b0c
    void getStartScaleA(int, f32 &);                                                            // 0x80167b5c
    void getStartScaleB(int, f32 &);                                                            // 0x80167c50
    void getStartSpeedMeterScale(int, int, f32 &);                                              // 0x80167d4c
    void getGoalSpeedMeterPos(int, f32 &);                                                      // 0x80167df8
    void getStartLapPos(int, int, f32 &);                                                       // 0x80167e48
    void getGoalLapPos(int, f32 &);                                                             // 0x8016804c
    void getStartCharPos(int, int, f32 &);                                                      // 0x8016809c
    void getGoalCharPos(int, int, f32 &);                                                       // 0x8016819c
    void getThunderCharScale(int);                                                                // 0x80168214
    void getStartRankPos(int, int, JGeometry::TVec2f &);                                    // 0x801682ec
    void getRaceRankScale(int);                                                                   // 0x8016851c
    void getGoalRankPos(int, int, JGeometry::TVec2f &);                                     // 0x801685e4
    void getGoalRankScale(int, int, f32 &);                                                     // 0x801689e4
    void getGoalRankRot(int, f32 &);                                                            // 0x80168cb4
    void getStartLapTimePos(int, int, f32 &);                                                   // 0x80168d1c
    void getGoalLapTimePos(int, int, f32 &);                                                    // 0x80168e30
    //void mLapTag;                                                                                 // 0x8036fb10
    //void mItemExTag;                                                                              // 0x8036fb90
    //void mItemExDrawTag;                                                                          // 0x8036fc10
    //void mRankTag;                                                                                // 0x8036ff80
    //void mRankGoalTag;                                                                            // 0x80370080
    //void mTimeDrawTag;                                                                            // 0x80370100
    //void mLapTimeDrawTag;                                                                         // 0x803701a0
    //void mSpeedTag;                                                                               // 0x803707d0
    //void mSpeedChipTag;                                                                           // 0x803707e8
    //void mSpeedDrawTag;                                                                           // 0x80370820
    //void mShineTimeTag;                                                                           // 0x803708f0
    //void mPlayerNumberName;                                                                       // 0x803950b8
    //void mLANPlayerNumberName;                                                                    // 0x803950d8
    //void mDriverName;                                                                             // 0x803950f8
    //void mPlayerName;                                                                             // 0x80395148
    //void mLANPlayerName;                                                                          // 0x80395168
    //void mCharacterName;                                                                          // 0x80395188
    //void mItemName;                                                                               // 0x803951d8
    //void mSpeedName;                                                                              // 0x80395254
    static JUtility::TColor mPlayerNumberColor[10]; // 0x803fbecc
    // void mTimeLapName;                                                                            // 0x804145d8
    static Race2D *mThis;                                                                         // 0x80416300
    //void mTask;                                                                                   // 0x80416304
    // Inline/Unused
    // void Race2DParam::Race2DParam();
    // void ~Race2D();
    // void getItemPos(int, int, JGeometry::TVec2<f32> &);
    // void itemUseInit();
    // void itemUseMain();
};

#endif // RACE2D_H
