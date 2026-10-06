#ifndef JPEFFECTMGR_H
#define JPEFFECTMGR_H

#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JParticle/JPAResource.h"

class JPEffectMgr
{
public:
    JPEffectMgr(JKRHeap *, bool, u8);                                           // 0x8022fda0
    void reset();                                                               // 0x8022fe58
    void reset_forMenu();                                                       // 0x802301c0
    static u16 getHashValue(const char *);                                      // 0x80230224
    void create2DEmtMgr(JPAResourceManager *, u16, u16);                        // 0x80230248
    void create_forMenu();                                                      // 0x8023038c
    JPABaseEmitter *createEmt(u16, const JGeometry::TVec3f &);                  // 0x8023050c
    JPABaseEmitter *createEmt(const char *, const JGeometry::TVec3f &);         // 0x80230544
    JPABaseEmitter *createEmtCameraClip(const char *, const JGeometry::TVec3f &); // 0x80230590
    JPABaseEmitter *createEmt2D(const char *, const JGeometry::TVec3f &);       // 0x80230600
    JPABaseEmitter *createEmt(const char *, const JGeometry::TVec3f &, Mtx);    // 0x8023064c
    void createEmtThunder(const char *, const JGeometry::TVec3f &);             // 0x802306c0
    JPABaseEmitter *createEmtCam(const char *, const JGeometry::TVec3f &, u8);  // 0x8023070c
    void createEmtBefore2DMulti(const char *, const JGeometry::TVec3f &, u8);   // 0x80230764
    void deleteEmt(JPABaseEmitter *);                                           // 0x802307bc
    void deleteEmtThunder(JPABaseEmitter *);                                    // 0x802307e0
    void deleteEmt2D(JPABaseEmitter *);                                         // 0x80230804
    void deleteEmtCam(JPABaseEmitter *, u8);                                    // 0x80230828
    void deleteEmtBefore2DMulti(JPABaseEmitter *, u8);                          // 0x80230854
    void clipEmitterCameraNumFlg(JPABaseEmitter *);                             // 0x80230880
    void calc();                                                                // 0x802308dc
    void calc_forMenu();                                                        // 0x8023095c
    void draw(int);                                                             // 0x802309c0
    void draw_thunder(int);                                                     // 0x80230ae0
    void draw_for2D();                                                          // 0x80230bf0
    void draw_forBefore2D();                                                    // 0x80230cc0
    void draw_forBefore2D(const u8);                                            // 0x80230d90
    void createSnowEmt();                                                       // 0x80230f30
    void createAwardDemoEmt(const bool);                                        // 0x80231084
    void createRainbowEmt();                                                    // 0x80231130
    void createCupStartDemoEmt();                                               // 0x80231190
    void resetSnowCamVel();                                                     // 0x80231254
    void calcSnowCamVel();                                                      // 0x802312b0

    JPAEmitterManager *getEm2DMgr() { return mEm2DMgr; }

    static void createMgr(JKRHeap *heap, bool p2)
    {
        mThis = new JPEffectMgr(heap, p2, 1);
    }

    static JPEffectMgr *getEffectMgr()
    {
        return mThis;
    }

    static JPEffectMgr *mThis;

private:
    u8 _00[0x18];
    JPAEmitterManager *mEm2DMgr; // 18
    u8 _1C[0x38 - 0x1C];
};

inline void CreateJPAMgr(JKRHeap *heap, bool p2) { JPEffectMgr::createMgr(heap, p2); };

inline JPEffectMgr *GetJPAMgr() { return JPEffectMgr::getEffectMgr(); }

#define JPADeleteEffect(effect) \
    GetJPAMgr()->deleteEmt(effect); \
    effect = nullptr;

#endif
