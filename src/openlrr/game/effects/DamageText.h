// DamageText.h : 
//

#pragma once

#include "../GameCommon.h"


namespace LegoRR
{; // !<---

/**********************************************************************************
 ******** Forward Declarations
 **********************************************************************************/

#pragma region Forward Declarations

#pragma endregion

/**********************************************************************************
 ******** Constants
 **********************************************************************************/

#pragma region Constants

#pragma endregion

/**********************************************************************************
 ******** Enumerations
 **********************************************************************************/

#pragma region Enums

#pragma endregion

/**********************************************************************************
 ******** Structures
 **********************************************************************************/

#pragma region Structs

struct DamageFontData // [LegoRR/DamageFont.c|struct:0x20]
{
	/*00,4*/	Gods98::Container* ownerCont;
	/*04,4*/	Gods98::Mesh* mesh;
	/*08,4*/	uint32 groupCount;
	/*0c,4*/	real32 float_c;
	/*10,4*/	real32 float_10;
	/*14,4*/	real32 timerDown;
	/*18,4*/	real32 timerUp;
	/*1c,4*/	uint32 flags;
	/*20*/
};
assert_sizeof(DamageFontData, 0x20);


struct DamageFont_Globs // [LegoRR/DamageFont.c|struct:0x16c|tags:GLOBS]
{
	/*000,28*/	Gods98::Container_Texture* fontTextDigitsTable[10];
	/*028,4*/	Gods98::Container_Texture* fontTextMinus;
	/*02c,140*/	DamageFontData instanceTable[10];
	/*16c*/
};
assert_sizeof(DamageFont_Globs, 0x16c);

#pragma endregion

/**********************************************************************************
 ******** Globals
 **********************************************************************************/

#pragma region Globals

// <LegoRR.exe @004b9a58>
extern DamageFont_Globs & dmgfontGlobs;

#pragma endregion

/**********************************************************************************
 ******** Macros
 **********************************************************************************/

#pragma region Macros

#pragma endregion

/**********************************************************************************
 ******** Functions
 **********************************************************************************/

#pragma region Functions

// <LegoRR.exe @0040a300>
#define DamageFont_Cleanup ((void (__cdecl* )(void))0x0040a300)
//void __cdecl DamageFont_Cleanup(void);

// <LegoRR.exe @0040a330>
#define DamageFont_LoadFrames ((void (__cdecl* )(const char* dirName, const char* fileBaseName))0x0040a330)
//void __cdecl DamageFont_LoadFrames(const char* dirName, const char* fileBaseName);

// <LegoRR.exe @0040a3e0>
#define DamageFont_DisplayDamage_OverLiveObject ((void (__cdecl* )(LegoObject* liveObj, uint32 displayNumber))0x0040a3e0)
//void __cdecl DamageFont_DisplayDamage_OverLiveObject(LegoObject* liveObj, uint32 displayNumber);

// <LegoRR.exe @0040a4f0>
#define DamageFont_GetNextFree ((DamageFontData* (__cdecl* )(void))0x0040a4f0)
//DamageFontData* __cdecl DamageFont_GetNextFree(void);

// <LegoRR.exe @0040a510>
#define DamageFont_SetDisplayNumber ((void (__cdecl* )(DamageFontData* healthFont, uint32 displayNumber))0x0040a510)
//void __cdecl DamageFont_SetDisplayNumber(DamageFontData* healthFont, uint32 displayNumber);

// <LegoRR.exe @0040a670>
#define DamageFont_MeshRenderCallback ((void (__cdecl* )(Gods98::Mesh* mesh, DamageFontData* healthFont, Gods98::Viewport* view))0x0040a670)
//void __cdecl DamageFont_MeshRenderCallback(Gods98::Mesh* mesh, DamageFontData* healthFont, Gods98::Viewport* view);

// <LegoRR.exe @0040a940>
#define DamageFont_UpdateAll ((void (__cdecl* )(real32 elapsedAbs))0x0040a940)
//void __cdecl DamageFont_UpdateAll(real32 elapsedAbs);

// <LegoRR.exe @0040a970>
#define DamageFont_UpdateSingle ((void (__cdecl* )(DamageFontData* healthFont, real32 elapsedAbs))0x0040a970)
//void __cdecl DamageFont_UpdateSingle(DamageFontData* healthFont, real32 elapsedAbs);

// <LegoRR.exe @0040aa10>
#define DamageFont_LiveObject_CheckCanShowDamage_Unk ((bool32 (__cdecl* )(LegoObject* liveObj))0x0040aa10)
//bool32 __cdecl DamageFont_LiveObject_CheckCanShowDamage_Unk(LegoObject* liveObj);

#pragma endregion

}
