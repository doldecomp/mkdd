#include "Kaneshige/RaceMgr.h"
#include "Osako/system.h"
#include "Sato/GeographyObj.h"
#include "Sato/ObjUtility.h"
#include "Shiraiwa/LensFlare.h"
#include "Shiraiwa/SiUtil.h"
#include "Yamamoto/kartCtrl.h"

#include "mathHelper.h"

TLensFlareScrn::TLensFlareScrn() {}
TLensFlareScrn::~TLensFlareScrn() {}

void TLensFlareScrn::init(u32 param_1, const JUTColor &color, u8 alphaMin, u8 alphaMax) {
    mViewIdx = param_1;

    mColor.set(color);
    mColor.a = 0;

    mAlpha = 0;
    _24 = false;
    _1c = 0;

    mAlphaMin = alphaMin == 0 ? sAlphaMin : alphaMin;
    mAlphaMax = alphaMax == 0 ? sAlphaMax : alphaMax;
}

char *TLensFlareSupervisor::fileName[4] = {
    "/Objects/mk_LensFX01.bti",
    "/Objects/mk_LensFX02.bti",
    "/Objects/mk_LensFX03.bti",
    "/Objects/mk_LensFX04.bti",
};

TLensFlareSupervisor *TLensFlare::sSupervisor;

u8 TLensFlareScrn::sAlphaAccel = 8;
u8 TLensFlareScrn::sAlphaDecel = 4;
u8 TLensFlareScrn::sAlphaMax = 100;
u8 TLensFlareScrn::sAlphaMin = 100;

void TLensFlareScrn::calc(bool enable, f32 scale) {
    switch (_1c) {
        case 1:
            if (_24 && !enable) {
                frameOut();
                _24 = enable;
            }

            mAlpha += sAlphaAccel;
            if (mAlpha > mAlphaMax) {
                mAlpha = mAlphaMax;
                _1c = 2;
            }
            break;

        case 2:
            if (!enable) {
                frameOut();
                _24 = enable;
            }

            mAlpha -= sAlphaDecel;
            if (mAlpha < mAlphaMin) {
                mAlpha = mAlphaMin;
            }
            break;

        case 3:
            mAlpha -= sAlphaDecel;
            if (mAlpha < 0) {
                mAlpha = 0;
                _1c = 0;
            }
            break;

        case 0:
            if (!_24 && enable) {
                frameIn();
                _24 = enable;
            }
            break;

        default:
            break;
    }

    mColor.a = mAlpha * scale;
    SiUtil::getViewport(&mViewport, mViewIdx);
}

// TODO: Should `mColor.a == 0` be integrated into this inline, somehow?
void TLensFlareScrn::draw() {
    setupGX();
    
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition2f32(0.0f, 0.0f);
    GXColor4u8(mColor.r, mColor.g, mColor.b, mColor.a);

    GXPosition2f32(mViewport.getWidth(), 0.0f);
    GXColor4u8(mColor.r, mColor.g, mColor.b, mColor.a);

    GXPosition2f32(mViewport.getWidth(), mViewport.getHeight());
    GXColor4u8(mColor.r, mColor.g, mColor.b, mColor.a);

    GXPosition2f32(0.0f, mViewport.getHeight());
    GXColor4u8(mColor.r, mColor.g, mColor.b, mColor.a);
}

void TLensFlareScrn::setupGX() {
    JGeometry::TPos3f mtxTex;
    GXSetCullMode(GX_CULL_BACK);
    GXClearVtxDesc();

    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_CLR_RGB, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    GXSetNumChans(1);

    GXSetChanCtrl(GX_COLOR0A0, true, GX_SRC_REG, GX_SRC_VTX, 1, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);

    GXColor white = {0xff, 0xff, 0xff, 0xff};
    GXSetChanAmbColor(GX_COLOR0A0, white);

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP);

    PSMTXIdentity(mtxTex);
    GXLoadPosMtxImm(mtxTex, 0);
    GXSetCurrentMtx(0);
    GXLoadTexMtxImm(mtxTex, 0x3c, GX_MTX3x4);

    GXSetNumTexGens(0);
    GXSetNumTevStages(1);

    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3c, false, 0x7d);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
}

void TLensFlareScrn::frameIn() {
    if (_1c != 0) {
        return;
    }
    _1c = 1;
    mAlpha = 0;
    mColor.a = 0;
}

void TLensFlareScrn::frameOut() {
    _1c = 3;
}

TLensFlareSupervisor::TLensFlareSupervisor() {
    mList.initiate();
    mTexture = new ResTIMG *[4];
}

TLensFlareSupervisor::~TLensFlareSupervisor() {
    delete [] mTexture;
}

void TLensFlareSupervisor::entry(TLensFlare *lensFlare) {
    mList.append(&lensFlare->_164);
}

void TLensFlareSupervisor::draw(u32 viewNo) {
    KartCam *kartCam = GetKartCtrl()->getKartCam(viewNo);
    int posH = kartCam->GetPosh();
    int posV = kartCam->GetPosv();

    J2DOrthoGraph j2dOrthoGraph(posV, posH, posV + kartCam->GetWidth(), posH + kartCam->GetHeight(), -1.0f, 1.0f);
    j2dOrthoGraph.setPort();

    setupGX();

    for (JSULink<TLensFlare> *flaresList = mList.getFirst(); flaresList != mList.getEnd(); flaresList = flaresList->getNext()) {
        TLensFlare *flare = flaresList->getObject();
        if (flare->draw(viewNo)) {
            break;
        }
    }
}

void TLensFlareSupervisor::getBufferZ(u32 viewNo) {
    for (JSULink<TLensFlare> *link = mList.getFirst(); link != mList.getEnd(); link = link->getNext()) {
        link->getObject()->getBufferZ(viewNo);
    }
}

void TLensFlareSupervisor::setupGX() {
    GXSetCullMode(GX_CULL_BACK);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_CLR_RGB, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_CLR_RGBA, GX_RGB8, 0);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, true, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);

    GXColor amb_color = {0xff, 0xff, 0xff, 0xff};
    GXSetChanAmbColor(GX_COLOR0A0, amb_color);

    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3c, false, 0x7d);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_A1, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
}

void TLensFlareSupervisor::load() {
    for (int i = 0; i < 4; i++) {
        mTexture[i] = (ResTIMG *)ObjUtility::getPtrCourseArc(fileName[i]);
        #line 321
        JUT_ASSERT(mTexture[i] != 0);
    }
}

f32 TFlareLine::sScale = 1.2f;
f32 TFlareLine::sFlareInterval = 1.05f;
f32 TFlareLine::sOutRadius = 120.0f;
u8 TFlareLine::sAlpha = 40;
u8 TFlareLine::sAlphaAccel = 2;
u8 TFlareLine::sAlphaDecel = 4;
f32 TFlareLine::sScrnScaleRange = 0.7f;

FlareInfo TFlareLine::sFlareInfo[21] = {
    { 1, 10.0f,  {0x0023, 0x0046, 0x00AF, 0x0000} },
    { 5,  0.0f,  {0x0000, 0x0000, 0x0000, 0x0000} },
    { 3,  0.7f,  {0x0064, 0x0064, 0x00C8, 0x0000} },
    { 3,  1.0f,  {0x004B, 0x00AA, 0x004B, 0x0000} },
    { 0,  2.0f,  {0x007D, 0x007D, 0x0032, 0x0000} },
    { 0,  1.5f,  {0x003C, 0x001E, 0x001E, 0x0000} },
    { 5,  0.0f,  {0x0000, 0x0000, 0x0000, 0x0000} },
    { 5,  0.0f,  {0x0000, 0x0000, 0x0000, 0x0000} },
    { 1,  0.35f, {0x00F5, 0x00A0, 0x00FF, 0x0000} },
    { 0,  3.37f, {0x007D, 0x001E, 0x0000, 0x0000} },
    { 3,  0.8f,  {0x007D, 0x004B, 0x004B, 0x0000} },
    { 3,  1.0f,  {0x00AA, 0x007D, 0x004B, 0x0000} },
    { 3,  1.57f, {0x0064, 0x0096, 0x00C3, 0x0000} },
    { 0,  2.27f, {0x0000, 0x0032, 0x007D, 0x0000} },
    { 1,  0.3f,  {0x00B9, 0x00C8, 0x00FF, 0x0000} },
    { 1,  3.8f,  {0x0096, 0x0028, 0x0028, 0x0000} },
    { 0,  5.0f,  {0x002D, 0x008C, 0x0091, 0x0000} },
    { 0,  2.0f,  {0x001E, 0x0064, 0x00AF, 0x0000} },
    { 5,  0.0f,  {0x0000, 0x0000, 0x0000, 0x0000} },
    { 5,  0.0f,  {0x0000, 0x0000, 0x0000, 0x0000} },
    { 2, 10.0f,  {0x00FF, 0x00FF, 0x00FF, 0x0000} },
};

const u8 TFlareLine::cNumFlare = 0;

TFlareLine::TFlareLine(u32 viewNo) {
    mViewNo = viewNo;
    mFlares = new TFlare *[21];
    for (int i = 0; i < 21; i++) {
        if (sFlareInfo[i]._0 != 5) {
            mFlares[i] = new TFlare;
            mFlares[i]->mTexture = new JUTTexture;
        } else {
            mFlares[i] = nullptr;
        }
    }
}

TFlareLine::~TFlareLine() {
    delete [] mFlares;
}

void TFlareLine::setTexture(ResTIMG **resTIMG, int lutID) {
    FlareInfo *flareInfo = sFlareInfo;
    for (int i = 0; i < 21; i++) {
        if (mFlares[i] != nullptr) {
            if (flareInfo[i]._0 < 4) {
                mFlares[i]->mTexture->storeTIMG(resTIMG[flareInfo[i]._0], lutID);
            }
        }
    }
}

// TODO: Fix this function to get a proper byte match.
u32 TFlareLine::getBufferZ() {
    u32 bufferZ = RCMGetManager()->mRaceDrawer->getCaptureMgr()->getZValue(8, mViewNo);
    RCMGetManager()->mRaceDrawer->getCaptureMgr()->setPosition(8, mViewNo, _48.x, _48.y);
    return bufferZ;
}

void TFlareLine::reset(u8 isVisible, u8 r, u8 g, u8 b, u8 a) {
    u8 drawConsoleNum = SiUtil::getDrawConsoleNum();
    SiUtil::getViewport(&_28, mViewNo, drawConsoleNum);

    JUTColor color(r, g, b, 0);
    init(mViewNo, color, a, a);
    _5c = isVisible == 0 ? sAlpha : isVisible;

    _38 = false;
    _44 = 0;
    _48.setAll(-1.0f);
    _58 = 1.0f;
    resetFlares();
}

f32 TFlareLine::getAspectRate() {
    f32 aspectRate;
    switch (SiUtil::getDrawConsoleNum()) {
        case 1:
            aspectRate = System::getPerspAspect() / System::getPerspAspect();
            break;

        case 2:
            aspectRate = System::getPerspAspectDiv2() / System::getPerspAspect();
            break;

        case 3:
        case 4:
            aspectRate = System::getPerspAspectDiv4() / System::getPerspAspect();
            break;

        default:
            #line 471
            JUT_ASSERT_MSG(false, "illegal console Number.")
            break;
    }
    return aspectRate;
}

void TFlareLine::resetFlares() {
    f32 aspectRate = getAspectRate();

    f32 width = _28.getWidth();
    f32 xScale = width / (aspectRate * System::get2DVpW());

    f32 height = _28.getHeight();
    f32 yScale = height / System::get2DVpH();

    for (int i = 0; i < 21; i++) {
        if (mFlares[i] != nullptr) {
            mFlares[i]->_c.x = xScale * sFlareInfo[i]._4;
            mFlares[i]->_c.y = yScale * sFlareInfo[i]._4;

            mFlares[i]->_4.x = 0.0f;
            mFlares[i]->_4.y = 0.0f;

            mFlares[i]->mColor.set(
                sFlareInfo[i].color.r,
                sFlareInfo[i].color.g,
                sFlareInfo[i].color.b,
                0
            );

            if (sFlareInfo[i]._0 == 2) {
                mFlares[i]->_18 = false;
            } else {
                mFlares[i]->_18 = true;
            }
        }
    }
}

void TFlareLine::calc(const JGeometry::TVec3f &position) {
    JGeometry::TVec2f screenPos;
    _50 = SiUtil::getScreenPosCheckMirror(mViewNo, position, &screenPos);

    if (_50 == 0xffffffff) {
        KartCam *kartCam = RCMGetCamera(mViewNo);
        screenPos.set(kartCam->GetVpPosh() - 1.0f, kartCam->GetVpPosv() - 1.0f);
    }

    JGeometry::TBox2f viewport;
    SiUtil::getViewport(&viewport, mViewNo);

    bool viewportMatches =
        viewport.f.x == _28.f.x &&
        viewport.f.y == _28.f.y &&
        viewport.i.x == _28.i.x &&
        viewport.i.y == _28.i.y;

    if (!viewportMatches) {
        f32 aspectRate = getAspectRate();
        f32 widthScale = _28.getWidth() / (aspectRate * System::get2DVpW());
        f32 heightScale = _28.getHeight() / System::get2DVpH();

        for (int i = 0; i < 21; i++) {
            if (mFlares[i] != nullptr) {
                mFlares[i]->_c.x = (sFlareInfo[i]._4 * widthScale);
                mFlares[i]->_c.y = (sFlareInfo[i]._4 * heightScale);
            }
        }

        _28.set(viewport);
    }

    _48.set(screenPos);

    JGeometry::TVec2f center;
    center.x = _28.f.x + 0.5f * (_28.i.x - _28.f.x);
    center.y = _28.f.y + 0.5f * (_28.i.y - _28.f.y);

    JGeometry::TVec2f direction;
    direction.sub(center, screenPos);

    const f32 height = _28.f.y - _28.i.y;
    f32 radius = sOutRadius * (height / System::get2DVpH());
    
    if (radius != 0.0f) {
        _58 = direction.squared() / SQUARE(radius);

        radius = _58;
        if (radius > 1.0f) {
            radius = 1.0f;
        } else if (radius < 0.0f) {
            radius = 0.0f;
        }
        _58 = radius;

        direction.normalize();

        mRotRad = atan2f(-direction.x, direction.y);
        direction.scale(sFlareInterval);

        setPosition(center, direction);

        _38 = isInside(screenPos);

        if (_38) {
            if (getBufferZ() < _50) {
                _38 = false;
            }
        }

        if (!_38) {
            _44 -= sAlphaDecel;
        } else {
            _44 += sAlphaAccel;
        }

        if (_44 < 0) {
            _44 = 0;
        } else if (_44 > _5c) {
            _44 = _5c;
        }

        f32 normalRange = SiUtil::getNormalRange(JGeometry::TUtilf::abs(_48.x - center.x), 0.0f, 0.5f * _28.getWidth());
        TLensFlareScrn::calc(_38, 1.0f - sScrnScaleRange * normalRange);
    }
}

bool TFlareLine::isInside(const JGeometry::TVec2f &plane) {
    bool isInside = true;
    if ((plane.x < _28.i.x || plane.x > _28.f.x) || (plane.y < _28.i.y || plane.y > _28.f.y)) {
        isInside = false;
    }
    return isInside;
}

// Second parameter is unused.
void TFlareLine::setPosition(const JGeometry::TVec2f &param_1, const JGeometry::TVec2f &param_2) {
    JGeometry::TVec2f vec;
    vec.sub(_48, param_1);

    if (vec.y == 0.0f) { return; }

    vec.y = JGeometry::TUtilf::abs(vec.y);
    f32 height = ((sFlareInterval * vec.y) * 2.0f) / 21.0f;
    vec.normalize();
    System::get2DVpH(); // Why is this called but never assigned?
    for (int i = 0; i < 21; i++) {
        if (mFlares[i] != nullptr) {
            mFlares[i]->_4.x = 0.0f;
            mFlares[i]->_4.y = (height * i) / vec.y;
        }
    }
}

u8 TFlareLine::draw() {
    JGeometry::TPos3f mtxAxisZ;
    JGeometry::TPos3f mtxPos;

    bool isFlareDrawn = true;

    if (_44 > 0) {
        KartCam *kartCam = RCMGetCamera(mViewNo);
        PSMTXRotRad(mtxAxisZ, 'Z', mRotRad);
        PSMTXTrans(mtxPos, _48.x - kartCam->GetVpPosv(), _48.y - kartCam->GetVpPosh(), 0.0f);
        PSMTXConcat(mtxPos, mtxAxisZ, mtxPos);

        GXLoadPosMtxImm(mtxPos, 0);
        GXSetCurrentMtx(0);

        for (int i = 0; i < 21; i++) {
            TFlare *flare = mFlares[i];

            if (flare != nullptr) {
                drawFixCenter(*flare);
            }
        }
    } else {
        isFlareDrawn = false;
    }

    // This looks like it could reasonably be an inline, but every
    // attempt I've made to condense it hasn't worked. Revisit later.
    bool isFlareScrnDrawn;
    if (mColor.a == 0) {
        isFlareScrnDrawn = false;
    } else {
        TLensFlareScrn::draw();
        isFlareScrnDrawn = true;
    }

    return isFlareDrawn | isFlareScrnDrawn;
}

void TFlareLine::drawFixCenter(TFlare &flare) {
    f32 width = flare.mTexture->getWidth();
    f32 height = flare.mTexture->getHeight();

    f32 xScale = flare._c.x * width * 0.5f * sScale;
    f32 yScale = flare._c.y * height * 0.5f * sScale;

    f32 x = flare._4.x - 0.5f * xScale;
    f32 y = flare._4.y - 0.5f * yScale;

    flare.mTexture->load(GX_TEXMAP0);

    GXSetTevColor(GX_TEVREG0, JUTColor(flare.mColor.toUInt32()));
    GXSetTevColor(GX_TEVREG1, JUTColor(_44 + flare.mColor));

    if (flare._18) {
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
    } else {
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
    }
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);

    GXPosition2f32(x, y);
    GXTexCoord2s8(0, 0);

    GXPosition2f32(x + xScale, y);
    GXTexCoord2s8(2, 0);

    GXPosition2f32(x + xScale, y + yScale);
    GXTexCoord2s8(2, 2);

    GXPosition2f32(x, y + yScale);
    GXTexCoord2s8(0, 2);
}

TLensFlare::TLensFlare(const CrsData::SObject &sObject) : TMapObjHioNode(sObject), _164(this) {
    if (sSupervisor == nullptr) {
        sSupervisor = new TLensFlareSupervisor;
    }
    sSupervisor->entry(this);
    _174 = new TFlareLine[SiUtil::getConsoleNum()];
}

void TLensFlare::reset() {
    resetObject();

    for (int i = 0; i < SiUtil::getConsoleNum(); i++) {
        _174[i].mViewNo = i;
        _174[i].setTexture(sSupervisor->getTextures(), 0);

        u8 p1 = mObjData->mParam1;
        
        u8 r = mObjData->mParam2;
        if (r == 0) { r = 0xC0; }

        u8 g = mObjData->mParam3;
        if (g == 0) { g = 0xC0; }

        u8 b = mObjData->mParam4;
        if (b == 0) { b = 0xC0; }

        u8 a = mObjData->mParam5;
        _174[i].reset(p1, r, g, b, a);
    }
}

void TLensFlare::calc() {
    for (int i = 0; i < SiUtil::getConsoleNum(); i++) {
        _174[i].calc(mPos);
    }
}

u8 TLensFlare::draw(u32 idx) {
    return _174[idx].draw();
}

u32 TLensFlare::getBufferZ(u32 viewNo) {
    return _174[viewNo].getBufferZ();
}

#include "JSystem/JAudio/JASFakeMatch2.h"
