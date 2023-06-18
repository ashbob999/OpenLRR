// Erosion.h : 
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

struct Erode_Globs // [LegoRR/Erode.c|struct:0xfa14|tags:GLOBS]
{
	/*0000,3e80*/	Point2I UnkBlocksList[2000];
	/*3e80,4*/	uint32 UnkBlocksCount;
	/*3e84,3e80*/	Point2I activeBlocks[2000];
	/*7d04,1f40*/	bool32 activeStates[2000];
	/*9c44,1f40*/	real32 activeTimers[2000]; // (countdown timers)
	/*bb84,1f40*/	Point2I lockedBlocks[1000];
	/*dac4,fa0*/	real32 lockedTimers[1000]; // (countdown timers)
	/*ea64,fa0*/	bool32 lockedStates[1000];
	/*fa04,4*/	real32 elapsedTimer; // (count-up elapsed timer)
	/*fa08,4*/	real32 ErodeTriggerTime; // (init: Lego.cfg)
	/*fa0c,4*/	real32 ErodeErodeTime; // (init: Lego.cfg)
	/*fa10,4*/	real32 ErodeLockTime; // (init: Lego.cfg)
	/*fa14*/
};
assert_sizeof(Erode_Globs, 0xfa14);

#pragma endregion

/**********************************************************************************
 ******** Globals
 **********************************************************************************/

#pragma region Globals

// <LegoRR.exe @004c8eb0>
extern Erode_Globs & erodeGlobs;

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

// <LegoRR.exe @0040e860>
#define Erode_Initialise ((void (__cdecl* )(real32 triggerTime, real32 erodeTime, real32 lockTime))0x0040e860)
//void __cdecl Erode_Initialise(real32 triggerTime, real32 erodeTime, real32 lockTime);

// <LegoRR.exe @0040e8c0>
#define Erode_GetFreeActiveIndex ((bool32 (__cdecl* )(OUT sint32* index))0x0040e8c0)
//bool32 __cdecl Erode_GetFreeActiveIndex(OUT sint32* index);

// <LegoRR.exe @0040e8f0>
#define Erode_GetBlockErodeRate ((real32 (__cdecl* )(const Point2I* blockPos))0x0040e8f0)
//real32 __cdecl Erode_GetBlockErodeRate(const Point2I* blockPos);

// <LegoRR.exe @0040e940>
#define Erode_AddActiveBlock ((void (__cdecl* )(const Point2I* blockPos, sint32 unkModulusNum))0x0040e940)
//void __cdecl Erode_AddActiveBlock(const Point2I* blockPos, sint32 unkModulusNum);

// <LegoRR.exe @0040e9e0>
#define Erode_Update ((void (__cdecl* )(real32 elapsedGame))0x0040e9e0)
//void __cdecl Erode_Update(real32 elapsedGame);

// <LegoRR.exe @0040ed30>
#define Erode_AddLockedBlock ((void (__cdecl* )(const Point2I* blockPos))0x0040ed30)
//void __cdecl Erode_AddLockedBlock(const Point2I* blockPos);

// <LegoRR.exe @0040ed80>
#define Erode_Block_FUN_0040ed80 ((void (__cdecl* )(const Point2I* blockPos, bool32 doState2_else_add3))0x0040ed80)
//void __cdecl Erode_Block_FUN_0040ed80(const Point2I* blockPos, bool32 doState2_else_add3);

// <LegoRR.exe @0040eee0>
#define Erode_IsBlockLocked ((bool32 (__cdecl* )(const Point2I* blockPos))0x0040eee0)
//bool32 __cdecl Erode_IsBlockLocked(const Point2I* blockPos);

// <LegoRR.exe @0040ef30>
#define Erode_FindAdjacentBlockPos ((bool32 (__cdecl* )(const Point2I* blockPos, OUT Point2I* adjacentblockPos))0x0040ef30)
//bool32 __cdecl Erode_FindAdjacentBlockPos(const Point2I* blockPos, OUT Point2I* adjacentblockPos);

#pragma endregion

}
