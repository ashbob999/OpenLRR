// Draw.h : 
//
/// APIS: IDirectDrawSurface4
/// DEPENDENCIES: DirectDraw, Maths, (Errors)
/// DEPENDENTS: Main, ...

#pragma once

#include "../../platform/windows.h"

#include "../../common.h"
#include "../colour.h"
#include "../geometry.h"


/**********************************************************************************
 ******** Forward Global Namespace Declarations
 **********************************************************************************/

#pragma region Forward Declarations

struct IDirectDrawSurface4;

#pragma endregion


namespace Gods98
{; // !<---

/**********************************************************************************
 ******** Function Typedefs
 **********************************************************************************/

#pragma region Function Typedefs

typedef void (__cdecl* DrawPixelFunc)(sint32 x, sint32 y, uint32 value);

#pragma endregion

/**********************************************************************************
 ******** Constants
 **********************************************************************************/

#pragma region Constants

#define DRAW_MAXLINES				200

#pragma endregion

/**********************************************************************************
 ******** Enumerations
 **********************************************************************************/

#pragma region Enums

#define DRAW_VERTEXFLAGS			(D3DFVF_DIFFUSE|D3DFVF_XYZ)

flags_scoped(Draw_GlobFlags) : uint32
{
	DRAW_GLOB_FLAG_NONE        = 0,

	DRAW_GLOB_FLAG_INITIALISED = 0x1,
};
flags_scoped_end(Draw_GlobFlags, 0x4);


enum class DrawEffect : uint32
{
	None      = 0,
	XOR       = 1,
	HalfTrans = 2,
};
assert_sizeof(DrawEffect, 0x4);

#pragma endregion

/**********************************************************************************
 ******** Structures
 **********************************************************************************/

#pragma region Structs

struct Draw_Rect
{
	/*00,10*/ Area2F rect;
	/*10,c*/ union {
		struct {
			/*10,4*/ real32 r;
			/*14,4*/ real32 g;
			/*18,4*/ real32 b;
		};
		/*10,c*/ ColourRGBF colour;
	};
	/*1c*/
};
assert_sizeof(Draw_Rect, 0x1c);


struct DrawLineVertex
{
	/*00,c*/ Vector3F position;
	/*0c,4*/ uint32 colour;
	/*10*/
};
assert_sizeof(DrawLineVertex, 0x10);


struct Draw_Globs
{
	// [globs: start]
	/*00,4*/ DrawPixelFunc drawPixelFunc;
	/*04,8*/ Point2F clipStart;
	/*0c,8*/ Point2F clipEnd;
	/*14,10*/ Rect2I lockRect;
	/*24,4*/ void* buffer;
	/*28,4*/ uint32 pitch;
	/*2c,4*/ uint32 bpp;
	/*30,4*/ uint32 redMask;
	/*34,4*/ uint32 greenMask;
	/*38,4*/ uint32 blueMask;
	/*3c,4*/ uint32 redBits;
	/*40,4*/ uint32 greenBits;
	/*44,4*/ uint32 blueBits;
	//DrawLineVertex vertexList[DRAW_MAXLINES * 2];
	/*48,4*/ Draw_GlobFlags flags;
	// [globs: end]
	/*4c*/
};
assert_sizeof(Draw_Globs, 0x4c);

#pragma endregion

/**********************************************************************************
 ******** Globals
 **********************************************************************************/

#pragma region Globals

// <LegoRR.exe @005417e8>
extern Draw_Globs & drawGlobs;

#pragma endregion

/**********************************************************************************
 ******** Functions
 **********************************************************************************/

#pragma region Functions

#define Draw_PixelList(p,c,r,g,b)					Draw_PixelListEx((p),(c),(r),(g),(b),Gods98::DrawEffect::None)
#define Draw_LineList(f,t,c,r,g,b)					Draw_LineListEx((f),(t),(c),(r),(g),(b),Gods98::DrawEffect::None)
#define Draw_WorldLineList(v,f,t,c,r,g,b,a)			Draw_WorldLineListEx((v),(f),(t),(c),(r),(g),(b),(a),Gods98::DrawEffect::None)
#define Draw_RectList(a,c,r,g,b)					Draw_RectListEx((a),(c),(r),(g),(b),Gods98::DrawEffect::None)

#define Draw_PixelListXOR(p,c,r,g,b)				Draw_PixelListEx((p),(c),(r),(g),(b),Gods98::DrawEffect::XOR)
#define Draw_LineListXOR(f,t,c,r,g,b)				Draw_LineListEx((f),(t),(c),(r),(g),(b),Gods98::DrawEffect::XOR)
	//#define Draw_WorldLineListXOR(v,f,t,c,r,g,b,a)		Draw_WorldLineListEx((v),(f),(t),(c),(r),(g),(b),(a),Gods98::DrawEffect::XOR)
#define Draw_RectListXOR(a,c,r,g,b)					Draw_RectListEx((a),(c),(r),(g),(b),Gods98::DrawEffect::XOR)

#define Draw_PixelListHalfTrans(p,c,r,g,b)			Draw_PixelListEx((p),(c),(r),(g),(b),Gods98::DrawEffect::HalfTrans)
#define Draw_LineListHalfTrans(f,t,c,r,g,b)			Draw_LineListEx((f),(t),(c),(r),(g),(b),Gods98::DrawEffect::HalfTrans)
//#define Draw_WorldLineListHalfTrans(v,f,t,c,r,g,b,a)	Draw_WorldLineListEx((v),(f),(t),(c),(r),(g),(b),(a),Gods98::DrawEffect::HalfTrans)
#define Draw_RectListHalfTrans(a,c,r,g,b)			Draw_RectListEx((a),(c),(r),(g),(b),Gods98::DrawEffect::HalfTrans)

/// CUSTOM: Gets if the Draw module rendering is enabled.
bool Draw_IsRenderEnabled();

/// CUSTOM: Sets if the Draw module rendering is enabled. For testing performance.
void Draw_SetRenderEnabled(bool enabled);


/// CUSTOM: Returns true if the drawing surface is currently locked, i.e. after calling Draw_Begin().
bool Draw_IsLocked();

/// CUSTOM: Locks the drawing surface and waits for Draw_End() to be called before unlocking it.
bool Draw_Begin();

/// CUSTOM: Unlocks the drawing surface after Draw_Begin() was called.
void Draw_End();

/// CUSTOM: Gets the current effect while the drawing surface has been locked with Draw_Begin().
DrawEffect Draw_GetEffect();

/// CUSTOM: Sets the current effect while the drawing surface has been locked with Draw_Begin().
void Draw_SetEffect(DrawEffect effect);

/// CUSTOM:
void Draw_AssertUnlocked(const char* caller);


// <LegoRR.exe @00486140>
void __cdecl Draw_Initialise(const Area2F* window);

// <LegoRR.exe @00486160>
void __cdecl Draw_SetClipWindow(const Area2F* window);

// <LegoRR.exe @00486270>
void __cdecl Draw_GetClipWindow(OUT Area2F* window);

// <unused>
void __cdecl Draw_PixelListEx(const Point2F* pointList, uint32 count, real32 r, real32 g, real32 b, DrawEffect effect);

// <LegoRR.exe @004862b0>
void __cdecl Draw_LineListEx(const Point2F* fromList, const Point2F* toList, uint32 count, real32 r, real32 g, real32 b, DrawEffect effect);

// <unused>
//void __cdecl Draw_WorldLineListEx(Viewport* vp, const Vector3F* fromList, const Vector3F* toList, uint32 count, real32 r, real32 g, real32 b, real32 a, DrawEffect effect);

// <LegoRR.exe @00486350>
void __cdecl Draw_RectListEx(const Area2F* rectList, uint32 count, real32 r, real32 g, real32 b, DrawEffect effect);

// <LegoRR.exe @004864d0>
void __cdecl Draw_RectList2Ex(const Draw_Rect* rectList, uint32 count, DrawEffect effect);

// <LegoRR.exe @00486650>
void __cdecl Draw_DotCircle(const Point2F* pos, uint32 radius, uint32 dots, real32 r, real32 g, real32 b, DrawEffect effect);

// <LegoRR.exe @00486790>
uint32 __cdecl Draw_GetColour(real32 r, real32 g, real32 b);

// <LegoRR.exe @00486810>
bool32 __cdecl Draw_LockSurface(IDirectDrawSurface4* surf, DrawEffect effect);

// <LegoRR.exe @00486910>
void __cdecl Draw_UnlockSurface(IDirectDrawSurface4* surf);

// <LegoRR.exe @00486950>
bool32 __cdecl Draw_SetDrawPixelFunc(DrawEffect effect);

// Bresenham's algorithm (line drawing)
// see: <http://www.brackeen.com/vga/shapes.html#shapes3.0>
// also see: <https://github.com/trigger-segfault/AsciiArtist/blob/75c75467d4f5fa2da02c1ba9712deb16529bbbde/PowerConsoleLib/PowerConsole/Drawing/Graphics.cpp#L459-L499>
// <LegoRR.exe @004869e0>
void __cdecl Draw_LineActual(sint32 x1, sint32 y1, sint32 x2, sint32 y2, uint32 colour);

/// CUSTOM:
uint32 _Draw_ConvertHalfTrans(uint32 pixel, uint32 value);

/// CUSTOM:
void __cdecl Draw_PixelDummy(sint32 x, sint32 y, uint32 value);

// <LegoRR.exe @00486b40>
void __cdecl Draw_Pixel8(sint32 x, sint32 y, uint32 value);

// <LegoRR.exe @00486b60>
void __cdecl Draw_Pixel16(sint32 x, sint32 y, uint32 value);

// <LegoRR.exe @00486b90>
void __cdecl Draw_Pixel16XOR(sint32 x, sint32 y, uint32 value);

// <LegoRR.exe @00486bc0>
void __cdecl Draw_Pixel16HalfTrans(sint32 x, sint32 y, uint32 value);

// <LegoRR.exe @00486c60>
void __cdecl Draw_Pixel24(sint32 x, sint32 y, uint32 value);

/// CUSTOM:
void __cdecl Draw_Pixel24XOR(sint32 x, sint32 y, uint32 value);

/// CUSTOM:
void __cdecl Draw_Pixel24HalfTrans(sint32 x, sint32 y, uint32 value);

// <LegoRR.exe @00486c90>
void __cdecl Draw_Pixel32(sint32 x, sint32 y, uint32 value);

/// CUSTOM:
void __cdecl Draw_Pixel32XOR(sint32 x, sint32 y, uint32 value);

/// CUSTOM:
void __cdecl Draw_Pixel32HalfTrans(sint32 x, sint32 y, uint32 value);

#pragma endregion

}
