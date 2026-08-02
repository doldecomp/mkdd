#ifndef RIVALBODYCTRL_H
#define RIVALBODYCTRL_H

#include <JSystem/JKernel/JKRDisposer.h>

#include "Kaneshige/Course/CrsGround.h"
#include "Sato/RivalCtrlBase.h"

class RivalBodyCtrl : public RivalCtrlBase, JKRDisposer
{
public:
    RivalBodyCtrl(RivalKart *);                    // 0x802804f4
    void init();                                   // 0x802805e4
    static void reset();                           // 0x80280724
    static void setRankArray();                    // 0x80280864
    static void chkPlayerKartCondition();          // 0x80280aec
    void chkBodyCondition();                       // 0x80280bec
    void getKartIndexByRank(int);                  // 0x80280ca0
    void calcTargetPosDrift();                     // 0x80280ea4
    void calcTargetPosNormal(float);               // 0x80280ea8
    void getFwPointPos(JGeometry::TVec3f *);       // 0x80281068
    void getPointRSideDir(JGeometry::TVec3f *);    // 0x802810ac
    void getLengthFCenter();                       // 0x802811b4
    void isNeighborhoodCenter(float);              // 0x8028135c
    void getDriftTargetDir(JGeometry::TVec3f *);   // 0x802813d0
    void getTargetCurveLR(float *, short);         // 0x80281558
    void chkDriftPoint();                          // 0x802818a8
    void chkArrivePoint(CLPoint *);                // 0x80281a54
    void initDrift(bool, CLPoint *);               // 0x80281b24    
    void reviseAfterDriftVtTargetPos();            // 0x80282364    
    void doColWallCtrl();                          // 0x80282888
    void calcMaxChangeWaitCnt();                   // 0x80282cb4    
    void comebackRescure(CLPoint *, bool);         // 0x80282cf8
    void resetBodyData();                          // 0x80283054
    void isOneUpperLowerPlayer(bool);              // 0x802830e4
    void getLowestPlayerDist(float *);             // 0x802831f4
    void findMyRankOnlyCom();                      // 0x802832cc
    int getTopComKartIndex(RivalBodyCtrl *);       // 0x80283324
    void getMyFowardComKartIndex();                // 0x802833e0
    void getMyBackwardComKartIndex(unsigned char); // 0x802834e0
    void setDriftInfo();                           // 0x802835f4
    void doRotPose(JGeometry::TVec3f, float);      // 0x80283858

    // Vtable
    virtual ~RivalBodyCtrl();                              // 0x8028056c, 14
    virtual void doSterrCtrl();                            // 0x8028251c, 18
    virtual void doDriftCtrl();                            // 0x80282070, 1c
    virtual void doStateFlgCtrl();                         // 0x80282cf4, 20
    virtual void doChangeDriverCtrl();                     // 0x80282aa8, 24
    virtual void doRotPosSterr(const JGeometry::TVec3f &); // 0x80282650, 28
    virtual void doNextPoint(bool);                        // 0x80280d5c, 2c
    virtual void resetPointData();                         // 0x80282dd4, 30

    // void sKartRankArray;                                 // 0x804138d4
    // void sComKartRankArray;                              // 0x80413994
    // void sKartTotalDist;                                 // 0x804139b4
    // void sPlayerRank;                                    // 0x804139d4
    // void sTopPlayerIndex;                                // 0x8041503c
    // void sLowestPlayerKartIndex;                         // 0x80415040
    // void sLowestComKartIndex;                            // 0x80415044
    // void sCrsStdDistRatio;                               // 0x80415048
    // void sEngineBParamPtr;                               // 0x80416d20
    static u32 sCommonFlgs;                                    // 0x80416d24
    // void sCheckTopDistRatio;                             // 0x80416d28

    // void getNthCLPoint(CLPoint *, short);
    // void getVtLengthFCenter();
    // void calcDriftTargetDir();
    // void getPointDir(const CLPoint *, JGeometry::TVec3f *);
    // void getLRKartTargetDir();
    // void getAngleKartZdirTargetDir();
    // void doDriftStageCount();
    // void setDriftStageWaitFrame();
    // void doRotTurnAndChangeState(RivalCtrlBase::RivalMoveState);
    // void isRank1st();
    // void isLowerPlayer(const float &);
    // void isLowestRankOfCom();
    // void getFowardPlayerDist();
    // void getBackwardPlayerDist();
    // void getTopPlayerDist();
    // void findRivalDistanceFrontAngle(float, float);
    // void findRivalDistanceRearAngle(float, float);
    
    static bool isPlayerFirst() { return (sCommonFlgs & 2) != 0; }


    u8 _20[0xc8 - 0x24];
}; // Size: 0xc8

class CLPointControllerRivalKart
{
public:
    CLPointControllerRivalKart(RivalBodyCtrl *);
    virtual int findNextPointNo(CLPoint *);
};

class RivalAwardBodyCtrl : public RivalBodyCtrl
{
public:
    RivalAwardBodyCtrl(RivalKart *);
    virtual void doNextPoint(bool);
    virtual void doRotPosSterr(const JGeometry::TVec3f &);
    virtual ~RivalAwardBodyCtrl() {} // 0x8028416c
    virtual void doDriftCtrl();       // 0x80284438
    virtual void doChangeDriverCtrl(); // 0x8028443c
};

class RivalAward2ndBodyCtrl : public RivalAwardBodyCtrl
{
public:
    RivalAward2ndBodyCtrl(RivalKart *);
    virtual void doSterrCtrl();
    void calcVtTargetPos();
    void doNextPoint(bool);
    virtual ~RivalAward2ndBodyCtrl() {} // 0x80284440
};

#endif // RIVALBODYCTRL_H
