// Priorities.h : 
//

#pragma once

#include "../GameCommon.h"
#include "../audio/SFX.h"


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

enum Priorities_GlobFlags : uint32
{
	PRIORITIES_GLOB_FLAG_NONE    = 0,
	PRIORITIES_GLOB_FLAG_HOVER   = 0x1,
	PRIORITIES_GLOB_FLAG_PRESSED = 0x2,
};
flags_end(Priorities_GlobFlags, 0x4);

#pragma endregion

/**********************************************************************************
 ******** Structures
 **********************************************************************************/

#pragma region Structs

struct Priorities_Globs // [LegoRR/Priorities.c|struct:0x4c0|tags:GLOBS]
{
	/*000,6c*/	char* langPriorityName[AI_Priority_Count];
	/*06c,6c*/	Gods98::Image* priorityImage[AI_Priority_Count];
	/*0d8,6c*/	Gods98::Image* priorityPressImage[AI_Priority_Count];
	/*144,6c*/	Gods98::Image* priorityOffImage[AI_Priority_Count];
	/*1b0,6c*/	SFX_ID sfxPriorityName[AI_Priority_Count];
	/*21c,6c*/	AI_Priority buttonTypes[AI_Priority_Count]; // Priority types for each button at index.
	/*288,d8*/	Point2F buttonPoints[AI_Priority_Count]; // Positions for each button at index.
	/*360,6c*/	AI_Priority initialTypes[AI_Priority_Count]; // Initial button positions on level start, and when reset.
	/*3cc,6c*/	uint32 initialValues[AI_Priority_Count]; // Initial values set by AITask_Game_SetPriorityValue for each button at index
	/*438,6c*/	bool32 initialOff[AI_Priority_Count]; // Initial OFF state for each button at index
	/*4a4,4*/	uint32 count; // Number of priority buttons for level.
	/*4a8,10*/	Area2F hoverArea;
	/*4b8,4*/	uint32 pressIndex;
	/*4bc,4*/	Priorities_GlobFlags flags;
	/*4c0*/
};
assert_sizeof(Priorities_Globs, 0x4c0);

#pragma endregion

/**********************************************************************************
 ******** Globals
 **********************************************************************************/

#pragma region Globals

// <LegoRR.exe @004a9ee8>
extern AI_Priority & s_PriorityList_Type;

// <LegoRR.exe @004a9eec>
extern const char* (& s_PriorityList_Name);

// <LegoRR.exe @00501f00>
extern Priorities_Globs & prioritiesGlobs;

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

// USAGE: <PriorityType>    <LangName>[|LangSFX]:<NormalImage>:<Pressedmage>:<OffImage>
// <LegoRR.exe @0045d080>
#define Priorities_LoadImages ((void (__cdecl* )(const Gods98::Config* config, const char* gameName))0x0045d080)

// <LegoRR.exe @0045d1c0>
#define Priorities_Reset ((void (__cdecl* )(void))0x0045d1c0)

// <LegoRR.exe @0045d210>
#define Priorities_LoadLevel ((bool32 (__cdecl* )(const Gods98::Config* config, const char* gameName, const char* levelName))0x0045d210)

// <LegoRR.exe @0045d320>
#define Priorities_LoadPositions ((void (__cdecl* )(const Gods98::Config* config, const char* gameName))0x0045d320)

// <LegoRR.exe @0045d3d0>
#define Priorities_HandleInput ((bool32 (__cdecl* )(uint32 mouseX, uint32 mouseY, bool32 leftButton, bool32 leftButtonLast, bool32 leftReleased))0x0045d3d0)

// <LegoRR.exe @0045d5c0>
#define Priorities_ChangePriorityPressed ((bool32 (__cdecl* )(uint32 orderIndex, bool32 leftButton, bool32 leftButtonLast))0x0045d5c0)

// Sets the area used to draw a green rectangle around the current moused-over priority.
// Hover is only drawn when PRIORITIES_GLOB_FLAG_HOVER flag is set.
// <LegoRR.exe @0045d630>
#define Priorities_SetHoverArea ((void (__cdecl* )(const Point2F* point, uint32 widthM1, uint32 heightM1))0x0045d630)

// <LegoRR.exe @0045d680>
#define Priorities_MovePriorityUpOrTop ((void (__cdecl* )(uint32 orderIndex))0x0045d680)

// <LegoRR.exe @0045d6b0>
#define Priorities_MovePriorityTop ((void (__cdecl* )(uint32 orderIndex))0x0045d6b0)

// <LegoRR.exe @0045d730>
#define Priorities_MovePriorityUp ((void (__cdecl* )(uint32 orderIndex))0x0045d730)

// <LegoRR.exe @0045d7b0>
#define Priorities_TurnPriorityOff ((void (__cdecl* )(uint32 orderIndex))0x0045d7b0)

// <LegoRR.exe @0045d810>
#define Priorities_MoveCursorToPriorityUpButton ((void (__cdecl* )(uint32 orderIndex))0x0045d810)

// <LegoRR.exe @0045d900>
#define Priorities_MoveCursorToPriorityImage ((void (__cdecl* )(uint32 orderIndex))0x0045d900)

// <LegoRR.exe @0045d990>
#define Priorities_Draw ((void (__cdecl* )(void))0x0045d990)

#pragma endregion

}
