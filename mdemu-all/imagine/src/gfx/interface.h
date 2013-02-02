#pragma once

#include <engine-globals.h>
#include <util/bits.h>
#include <util/rectangle2.h>
#include <util/time/sys.hh>
#include <gfx/defs.hh>
#include <gfx/viewport.hh>
#include <gfx/Projector.hh>
#include <util/pixel.h>

namespace Gfx
{

// init & control
    CallResult init();// ATTRS(cold);
    CallResult setOutputVideoMode(uint x, uint y);// ATTRS(cold);
void resizeDisplay(uint x, uint y);

// commit/sync
void renderFrame();
void waitVideoSync();
void setVideoInterval(uint interval);
void updateFrameTime();
extern uint frameTime, frameTimeRel;

enum { TRIANGLE = 1, TRIANGLE_STRIP, QUAD, };

extern bool preferBGRA, preferBGR;

// render states

enum { BLEND_MODE_OFF = 0, BLEND_MODE_ALPHA, BLEND_MODE_INTENSITY };
void setBlendMode(uint mode);

enum { IMG_MODE_MODULATE = 0, IMG_MODE_BLEND, IMG_MODE_REPLACE };
void setImgMode(uint mode);

enum { BLEND_EQ_ADD, BLEND_EQ_SUB, BLEND_EQ_RSUB };
void setBlendEquation(uint mode);

void setActiveTexture(GfxTextureHandle tex);

void setImgBlendColor(GColor r, GColor g, GColor b, GColor a);

#ifdef CONFIG_BASE_ANDROID
	void setDither(uint on);
#else
	static void setDither(uint on) { }
#endif

void setZTest(bool on);

enum { BOTH_FACES, FRONT_FACES, BACK_FACES };
void setVisibleGeomFace(uint sides);

void setClipRect(bool on);
void setClipRectBounds(int x, int y, int w, int h);
static void setClipRectBounds(Rect2<int> r)
{
	setClipRectBounds(r.x, r.y, r.xSize(), r.ySize());
}

void setZBlend(bool on);
void setZBlendColor(GColor r, GColor g, GColor b);

void setClear(bool on);
void setClearColor(GColor r, GColor g, GColor b, GColor a = 1.);

void setColor(GColor r, GColor g, GColor b, GColor a = 1.);

enum { COLOR_WHITE, COLOR_BLACK };
static void setColor(int colConst)
{
	switch(colConst)
	{
		bcase COLOR_WHITE: setColor(1., 1., 1.);
		bcase COLOR_BLACK: setColor(0., 0., 0.);
	}
}

static const PixelFormatDesc &ColorFormat = PixelFormatRGBA8888;
uint color();

// state shortcuts
static void shadeMod()
{
	setActiveTexture(0);
	setImgMode(IMG_MODE_MODULATE);
	setBlendMode(BLEND_MODE_OFF);
}

static void shadeModAlpha()
{
	setActiveTexture(0);
	setImgMode(IMG_MODE_MODULATE);
	setBlendMode(BLEND_MODE_ALPHA);
}

// transforms

void applyTranslate(TransformCoordinate x, TransformCoordinate y, TransformCoordinate z);
static void applyTranslate(TransformCoordinate x, TransformCoordinate y) { applyTranslate(x, y, Gfx::proj.focal); }
void loadTranslate(TransformCoordinate x, TransformCoordinate y, TransformCoordinate z);
static void loadTranslate(TransformCoordinate x, TransformCoordinate y) { loadTranslate(x, y, Gfx::proj.focal); }
void applyScale(TransformCoordinate sx, TransformCoordinate sy, TransformCoordinate sz);
static void applyScale(TransformCoordinate sx, TransformCoordinate sy) { applyScale(sx, sy, 1.); }
static void applyScale(TransformCoordinate s) { applyScale(s, s, 1.); }
static void applyScale(GfxPoint p) { applyScale(p.x, p.y, 1.); }
static void applyYScale2D(TransformCoordinate sy, TransformCoordinate aR) { applyScale(sy * aR, sy, 1.); }
void applyPitchRotate(Angle t);
void applyRollRotate(Angle t);
void applyYawRotate(Angle t);
static void resetTransforms()
{
	loadTranslate(0., 0., proj.focal);
}
void loadIdentTransform();

static void loadTransforms(const Rect2<int> &r, _2DOrigin o)
{
	loadTranslate(gXPos(r, o), gYPos(r, o));
	applyScale(gXSize(r), gYSize(r));
}

/*static GC adjustZScales(GC pos, GC origPosZ, GC newPosZ)
{
	return pos * (newPosZ / origPosZ);
}

static GC adjustZScalesInv(GC pos, GC origPosZ, GC newPosZ)
{
	return pos / (newPosZ / origPosZ);
}*/

extern GC mmToPixelXScaler, mmToPixelYScaler;

static int xMMSizeToPixel(GC mm) { return int(mm * mmToPixelXScaler); }
static int yMMSizeToPixel(GC mm) { return int(mm * mmToPixelYScaler); }

static Rect2<int> viewportRect()
{
	return Rect2<int>(0, 0, (int)viewPixelWidth(), (int)viewPixelHeight());
}

struct GfxViewState
{
	Coordinate width, height, aspectRatio;
	uint pixelWidth, pixelHeight;
};

}

// callbacks
typedef void(*DrawHandlerFunc)(void*);
void gfx_drawHandler(DrawHandlerFunc handler, void *userPtr = 0);
typedef void(*ViewChangeFunc)(void*, Gfx::GfxViewState *oldState);
void gfx_viewChangeHandler(ViewChangeFunc func, void *userPtr = 0);
