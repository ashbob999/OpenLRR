// Flocks.h : 
//

#pragma once

#include "../GameCommon.h"


namespace LegoRR
{; // !<---

/**********************************************************************************
 ******** Forward Declarations
 **********************************************************************************/

#pragma region Forward Declarations

struct Flocks;
struct FlocksItem;

#pragma endregion

/**********************************************************************************
 ******** Constants
 **********************************************************************************/

#pragma region Constants

#pragma endregion

/**********************************************************************************
 ******** Function Typedefs
 **********************************************************************************/

#pragma region Function Typedefs

typedef void (__cdecl* FlocksCallback)(Flocks* flocks, FlocksItem*, void* data);

#pragma endregion

/**********************************************************************************
 ******** Enumerations
 **********************************************************************************/

#pragma region Enums

enum FlocksFlags : uint32
{
	FLOCKS_FLAG_NONE  = 0,
	FLOCKS_FLAG_UNK_1 = 0x1,
};
flags_end(FlocksFlags, 0x4);

#pragma endregion

/**********************************************************************************
 ******** Structures
 **********************************************************************************/

#pragma region Structs

struct FlocksItem // [LegoRR/Flocks.c|struct:0xa0]  An individual item flying in a single Flocks unit.
{
	/*00,c*/	Vector3F vector_0;
	/*0c,c*/	Vector3F vector_c;
	/*18,c*/	Vector3F vector_18;
	/*24,4*/	real32 rotationY_24;
	/*28,4*/	real32 rotationX_28;
	/*2c,4*/	real32 rotationZ_2c;
	/*30,4*/	real32 float_30;
	/*34,4*/	real32 float_34;
	/*38,4*/	real32 GoalUpdate1;
	/*3c,4*/	real32 Turn1;
	/*40,4*/	real32 Speed1;
	/*44,4*/	real32 Tightness1;
	/*48,4*/	real32 GoalUpdate2;
	/*4c,4*/	real32 Turn2;
	/*50,4*/	real32 Speed2;
	/*54,4*/	real32 Tightness2;
	/*58,40*/	Matrix4F matrix;
	/*98,4*/	Gods98::Container* cont;
	/*9c,4*/	FlocksItem* subdataNext;
	/*a0*/
};
assert_sizeof(FlocksItem, 0xa0);


struct Flocks // [LegoRR/Flocks.c|struct:0x28] The singular flocks unit, which holds all items flying within it.
{
	/*00,4*/	FlocksItem* flocksSubdata1; // (is this a doubly-linked list?)
	/*04,4*/	FlocksItem* flocksSubdata2;
	/*08,4*/	uint32 numSubdata;
	/*0c,4*/	bool32 hasVector1;
	/*10,4*/	bool32 hasVector2;
	/*14,4*/	undefined4 field_14;
	/*18,4*/	undefined4 field_18;
	/*1c,4*/	undefined4 field_1c;
	/*20,4*/	undefined4 field_20;
	/*24,4*/	FlocksFlags flags;
	/*28*/
};
assert_sizeof(Flocks, 0x28);


struct Flocks_Globs // [LegoRR/Flocks.c|struct:0x10|tags:GLOBS]
{
	/*00,4*/	real32 Turn;
	/*04,4*/	real32 Speed;
	/*08,4*/	real32 Tightness;
	/*0c,4*/	real32 GoalUpdate;
	/*10*/
};
assert_sizeof(Flocks_Globs, 0x10);

#pragma endregion

/**********************************************************************************
 ******** Globals
 **********************************************************************************/

#pragma region Globals

// <LegoRR.exe @00558850>
extern Flocks_Globs & flocksGlobs;

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


// <LegoRR.exe @0040f5f0>
#define Flocks_LiveObject_Flocks_CreateSubdata ((FlocksItem* (__cdecl* )(OPTIONAL const Vector3F* vector_1, OPTIONAL const Vector3F* vector_2, real32 turn, real32 speed, real32 tightness, real32 goalUpdate, Gods98::Container* cont))0x0040f5f0)
//FlocksItem* __cdecl Flocks_LiveObject_Flocks_CreateSubdata(OPTIONAL const Vector3F* vector_1, OPTIONAL const Vector3F* vector_2, real32 turn, real32 speed, real32 tightness, real32 goalUpdate, Gods98::Container* cont);

// <LegoRR.exe @0040f700>
#define Flocks_LiveObject_Flocks_CreateData ((Flocks* (__cdecl* )(OPTIONAL const Vector3F* vector_1, OPTIONAL const Vector3F* vector_2, real32 turn, real32 speed, real32 tightness, Gods98::Container* cont))0x0040f700)
//Flocks* __cdecl Flocks_LiveObject_Flocks_CreateData(OPTIONAL const Vector3F* vector_1, OPTIONAL const Vector3F* vector_2, real32 turn, real32 speed, real32 tightness, Gods98::Container* cont);

// <LegoRR.exe @0040f780>
#define Flocks_FreeData ((void (__cdecl* )(Flocks* flocksData))0x0040f780)
//void __cdecl Flocks_FreeData(Flocks* flocksData);

// <LegoRR.exe @0040f7c0>
#define Flocks_Subdata_MathChangeRotationX ((void (__cdecl* )(FlocksItem* flockSubdata, const Vector3F* vector))0x0040f7c0)
//void __cdecl Flocks_Subdata_MathChangeRotationX(FlocksItem* flockSubdata, const Vector3F* vector);

// <LegoRR.exe @0040f850>
#define Flocks_Subdata_MathChangeRotationsYZ ((void (__cdecl* )(FlocksItem* subdata, IN OUT Vector3F* vector))0x0040f850)
//void __cdecl Flocks_Subdata_MathChangeRotationsYZ(FlocksItem* subdata, IN OUT Vector3F* vector);

// <LegoRR.exe @0040fa30>
#define Flocks_Subdata_Matrix_MultRotXYZ ((void (__cdecl* )(FlocksItem* subdata))0x0040fa30)
//void __cdecl Flocks_Subdata_Matrix_MultRotXYZ(FlocksItem* subdata);

// <LegoRR.exe @0040fac0>
#define Flocks_Callback_SubdataMoveAndTurn ((void (__cdecl* )(Flocks* unused_flocksData, FlocksItem* subdata, real32* pElapsed))0x0040fac0)
//void __cdecl Flocks_Callback_SubdataMoveAndTurn(Flocks* unused_flocksData, FlocksItem* subdata, real32* pElapsed);

// <LegoRR.exe @0040fbf0>
#define Flocks_MathX_Vector3DRandomMultiply ((void (__cdecl* )(OUT Vector3F* vector, real32 x, real32 y, real32 z))0x0040fbf0)
//void __cdecl Flocks_MathX_Vector3DRandomMultiply(OUT Vector3F* vector, real32 x, real32 y, real32 z);

// <LegoRR.exe @0040fc30>
#define Flocks_Callback_SubdataMoveTightness ((void (__cdecl* )(Flocks* flocksData, FlocksItem* subData, real32* pElapsed))0x0040fc30)
//void __cdecl Flocks_Callback_SubdataMoveTightness(Flocks* flocksData, FlocksItem* subData, real32* pElapsed);

// <LegoRR.exe @0040fce0>
#define Flocks_SetVector2 ((void (__cdecl* )(Flocks* flocksData, const Vector3F* vector2))0x0040fce0)
//void __cdecl Flocks_SetVector2(Flocks* flocksData, const Vector3F* vector2);

// <LegoRR.exe @0040fd10>
#define Flocks_Subdata_MoveAndTurn ((void (__cdecl* )(FlocksItem* subdata, real32 elapsed))0x0040fd10)
//void __cdecl Flocks_Subdata_MoveAndTurn(FlocksItem* subdata, real32 elapsed);

// <LegoRR.exe @0040fd30>
#define Flocks_Subdata_UpdateMove ((void (__cdecl* )(Flocks* flocksData, real32 elapsed))0x0040fd30)
//void __cdecl Flocks_Subdata_UpdateMove(Flocks* flocksData, real32 elapsed);

// <LegoRR.exe @0040fd70>
#define Flocks_LiveObject_Flocks_AddSubdata ((void (__cdecl* )(Flocks* flockData, FlocksItem* subdata))0x0040fd70)
//void __cdecl Flocks_LiveObject_Flocks_AddSubdata(Flocks* flockData, FlocksItem* subdata);

// <LegoRR.exe @0040fd90>
#define Flocks_Free_SubdataRecurse ((FlocksItem* (__cdecl* )(Flocks* flocksData))0x0040fd90)
//FlocksItem* __cdecl Flocks_Free_SubdataRecurse(Flocks* flocksData);

// <LegoRR.exe @0040fdc0>
#define Flocks_CallbackForAllSubdata ((void (__cdecl* )(Flocks* flockData, FlocksCallback callback, void* data))0x0040fdc0)
//void __cdecl Flocks_CallbackForAllSubdata(Flocks* flockData, FlocksCallback callback, void* data);

// <LegoRR.exe @0040fdf0>
#define Flocks_MathX_RandRangeOne ((real32 (__cdecl* )(void))0x0040fdf0)
//real32 __cdecl Flocks_MathX_RandRangeOne(void);

// <LegoRR.exe @0040fe00>
#define Flocks_Callback_FUN_0040fe00 ((void (__cdecl* )(Flocks* flocksData, FlocksItem* subdata, real32* param_3))0x0040fe00)
//void __cdecl Flocks_Callback_FUN_0040fe00(Flocks* flocksData, FlocksItem* subdata, real32* param_3);

// <LegoRR.exe @0040fe80>
#define Flocks_FUN_0040fe80 ((void (__cdecl* )(Flocks* flocksData, real32 randomness))0x0040fe80)
//void __cdecl Flocks_FUN_0040fe80(Flocks* flocksData, real32 randomness);

// <LegoRR.exe @0040fea0>
#define Flocks_SetParameters1 ((void (__cdecl* )(Flocks* flocksData, real32 turn, real32 speed, real32 tightness))0x0040fea0)
//void __cdecl Flocks_SetParameters1(Flocks* flocksData, real32 turn, real32 speed, real32 tightness);

// <LegoRR.exe @0040fec0>
#define Flocks_Callback_SetSubdataParametersFromGlobals ((void (__cdecl* )(Flocks* flocksData, FlocksItem* subdata))0x0040fec0)
//void __cdecl Flocks_Callback_SetSubdataParametersFromGlobals(Flocks* flocksData, FlocksItem* subdata);

// <LegoRR.exe @0040ff10>
#define Flocks_SetGlobal_AndAllSubdataParameters ((void (__cdecl* )(Flocks* flockData, real32 turn, real32 speed, real32 tightness, real32 goalUpdate))0x0040ff10)
//void __cdecl Flocks_SetGlobal_AndAllSubdataParameters(Flocks* flockData, real32 turn, real32 speed, real32 tightness, real32 goalUpdate);

// <LegoRR.exe @0040ff50>
#define Flocks_SetVector1 ((void (__cdecl* )(Flocks* flocksData, const Vector3F* vector1))0x0040ff50)
//void __cdecl Flocks_SetVector1(Flocks* flocksData, const Vector3F* vector1);

// DATA: const Vector3F*
// <LegoRR.exe @0040ff80>
#define Flocks_Callback_SetSubdataVectorC ((void (__cdecl* )(Flocks* flocksData, FlocksItem* subdata, void* vector))0x0040ff80)
//void __cdecl Flocks_Callback_SetSubdataVectorC(Flocks* flocksData, FlocksItem* subdata, void* vector);

// <LegoRR.exe @0040ffa0>
#define Flocks_SetAllSubdataVectorC ((void (__cdecl* )(Flocks* flocksData, const Vector3F* vector))0x0040ffa0)
//void __cdecl Flocks_SetAllSubdataVectorC(Flocks* flocksData, const Vector3F* vector);

// DATA: const Vector3F*
// <LegoRR.exe @0040ffc0>
#define Flocks_Callback_SetSubdataVector0 ((void (__cdecl* )(Flocks* flocksData, FlocksItem* subdata, void* vector))0x0040ffc0)
//void __cdecl Flocks_Callback_SetSubdataVector0(Flocks* flocksData, FlocksItem* subdata, void* vector);

// <LegoRR.exe @0040ffe0>
#define Flocks_SetAllSubdataVector0 ((void (__cdecl* )(Flocks* flocksData, const Vector3F* vector))0x0040ffe0)
//void __cdecl Flocks_SetAllSubdataVector0(Flocks* flocksData, const Vector3F* vector);

// <LegoRR.exe @00410000>
#define Flocks_Callback_FUN_00410000 ((void (__cdecl* )(Flocks* flocksData, FlocksItem* subdata, real32* pElapsed))0x00410000)
//void __cdecl Flocks_Callback_FUN_00410000(Flocks* flocksData, FlocksItem* subdata, real32* pElapsed);

// <LegoRR.exe @00410160>
#define Flocks_Callback_CompareVecs_0_c ((void (__cdecl* )(Flocks* flocksData, FlocksItem* subdata, bool32* pSuccess))0x00410160)
//void __cdecl Flocks_Callback_CompareVecs_0_c(Flocks* flocksData, FlocksItem* subdata, bool32* pSuccess);

// <LegoRR.exe @004101b0>
#define Flocks_CompareAllVecs_0_c ((bool32 (__cdecl* )(Flocks* flocksData))0x004101b0)
//bool32 __cdecl Flocks_CompareAllVecs_0_c(Flocks* flocksData);

#pragma endregion

}
