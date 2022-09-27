// Vehicle.cpp : 
//

#include "MeshLOD.h"
#include "Object.h"
#include "Upgrade.h"
#include "Weapons.h"

#include "Vehicle.h"


/**********************************************************************************
 ******** Functions
 **********************************************************************************/

#pragma region Functions

/// CUSTOM: Properly cleanup Container references to nulls.
void LegoRR::_Vehicle_RemoveNulls(VehicleModel* vehicle)
{
	/// FIXME: Properly remove Container references that were being leaked.
	if (vehicle->drillNull) {
		Gods98::Container_RemoveReference(vehicle->drillNull);
		vehicle->drillNull = nullptr;
	}
	if (vehicle->depositNull) {
		Gods98::Container_RemoveReference(vehicle->depositNull);
		vehicle->depositNull = nullptr;
	}
	if (vehicle->driverNull) {
		Gods98::Container_RemoveReference(vehicle->driverNull);
		vehicle->driverNull = nullptr;
	}

	for (uint32 i = 0; i < VEHICLE_MAXCARRYS; i++) {
		if (vehicle->carryNulls[i]) {
			Gods98::Container_RemoveReference(vehicle->carryNulls[i]);
			vehicle->carryNulls[i] = nullptr;
		}
	}
	for (uint32 i = 0; i < VEHICLE_MAXCAMERAS; i++) {
		if (vehicle->cameraNulls[i]) {
			Gods98::Container_RemoveReference(vehicle->cameraNulls[i]);
			vehicle->cameraNulls[i] = nullptr;
		}
	}
}

/// CUSTOM: Properly cleanup Container references to nulls.
void LegoRR::_Vehicle_RemoveWeaponNulls(VehicleModel* vehicle)
{
	/// FIXME: Properly remove Container references that were being leaked.
	/// CODE DUPLICATION: Identical to _Building_RemoveWeaponNulls (assuming we use WeaponsModel* as the parameter).
	for (uint32 i = 0; i < WEAPON_MAXWEAPONS; i++) {
		for (uint32 j = 0; j < WEAPON_MAXFIRES; j++) {
			if (vehicle->weapons.fireNullPairs[i][j]) {
				Gods98::Container_RemoveReference(vehicle->weapons.fireNullPairs[i][j]);
				vehicle->weapons.fireNullPairs[i][j] = nullptr;
			}
		}

		if (vehicle->weapons.xPivotNulls[i]) {
			Gods98::Container_RemoveReference(vehicle->weapons.xPivotNulls[i]);
			vehicle->weapons.xPivotNulls[i] = nullptr;
		}
		if (vehicle->weapons.yPivotNulls[i]) {
			Gods98::Container_RemoveReference(vehicle->weapons.yPivotNulls[i]);
			vehicle->weapons.yPivotNulls[i] = nullptr;
		}
	}
}

/// CUSTOM: Properly cleanup Container references to nulls.
void LegoRR::_Vehicle_RemoveWheelNulls(VehicleModel* vehicle)
{
	/// FIXME: Properly remove Container references that were being leaked.
	for (uint32 i = 0; i < VEHICLE_MAXWHEELS; i++) {
		if (vehicle->wheelNulls[i]) {
			Gods98::Container_RemoveReference(vehicle->wheelNulls[i]);
			vehicle->wheelNulls[i] = nullptr;
		}
	}
}



// <LegoRR.exe @0046c690>
bool32 __cdecl LegoRR::Vehicle_IsCameraFlipDir(VehicleModel* vehicle)
{
	return vehicle->cameraFlipDir == BoolTri::BOOL3_TRUE;
}

// <LegoRR.exe @0046c6b0>
bool32 __cdecl LegoRR::Vehicle_SetActivity(VehicleModel* vehicle, const char* activityName, real32 elapsed)
{
	/// FIX APPLY: Remove Container references that were being leaked after every SetActivity.
	_Vehicle_RemoveNulls(vehicle);
	_Vehicle_RemoveWeaponNulls(vehicle);
	_Vehicle_RemoveWheelNulls(vehicle);

	MeshLOD_RemoveTargets(vehicle->polyMedium1);
	/// FIXME: Should we also be calling RemoveTargets for polyMedium2??
	if (vehicle->polyMedium2 != nullptr) {
		MeshLOD_RemoveTargets(vehicle->polyMedium2);
	}

	bool success2 = false;
	bool success1 = Gods98::Container_SetActivity(vehicle->contAct1, activityName);
	vehicle->flags &= ~VEHICLE_FLAG_NOACTIVITY1;

	if (!success1 && (vehicle->flags & VEHICLE_FLAG_HOLDMISSING)) {
		if (!Gods98::Container_IsCurrentActivity(vehicle->contAct1, activityName)) {
			vehicle->flags |= VEHICLE_FLAG_NOACTIVITY1;
		}
	}
	if (vehicle->contAct2 != nullptr) {
		success2 = Gods98::Container_SetActivity(vehicle->contAct2, activityName);
	}

	if (!(vehicle->flags & VEHICLE_FLAG_NOACTIVITY1)) {
		Gods98::Container_SetAnimationTime(vehicle->contAct1, elapsed);
	}
	if (vehicle->contAct2 != nullptr) {
		Gods98::Container_SetAnimationTime(vehicle->contAct2, elapsed);
	}
	Vehicle_PopulateWheelNulls(vehicle);
	Vehicle_SetUpgradeActivity(vehicle, activityName);
	return (success1 || success2);

}

// <LegoRR.exe @0046c7d0>
void __cdecl LegoRR::Vehicle_SetUpgradeActivity(VehicleModel* vehicle, const char* activityName)
{
	/// CODE DUPLICATION: Almost identical to _Building_SetUpgradeActivity.

	/// FIX APPLY: Remove Container references that were being leaked after every SetUpgradeActivity.
	_Vehicle_RemoveWeaponNulls(vehicle);

	for (uint32 i = 0; i < WEAPON_MAXWEAPONS; i++) {
		//for (uint32 j = 0; j < WEAPON_MAXFIRES; j++) {
		//	vehicle->weapons.fireNullPairs[i][j] = nullptr;
		//}
		//vehicle->weapons.xPivotNulls[i] = nullptr;
		//vehicle->weapons.yPivotNulls[i] = nullptr;

		vehicle->weapons.weaponParts[i] = nullptr;
	}

	vehicle->weapons.weaponCount = 0;

	for (LegoObject* upg = vehicle->upgrades.firstObject; upg != nullptr; upg = upg->upgradePart->nextObject) {

		Upgrade_PartModel* upgradePart = upg->upgradePart;

		Gods98::Container* partParentCont = Vehicle_FindNull(vehicle, upgradePart->partInfo->nullObjectName, upgradePart->partInfo->nullFrameNo);
		Gods98::Container* partChildCont = Upgrade_Part_GetActivityContainer(upgradePart);
		Gods98::Container_SetParent(partChildCont, partParentCont);
		Gods98::Container_ClearTransform(partChildCont);
		if (activityName == nullptr) {
			// Sort-of inlined.
			//Gods98::Container_SetActivity(upgradePart->cont, objectGlobs.activityName[Activity_Stand]);
			Upgrade_Part_SetActivity(upgradePart, objectGlobs.activityName[Activity_Stand]);
		}
		else {
			Upgrade_Part_SetActivity(upgradePart, activityName);
		}

		if (upgradePart->partInfo->weaponName != nullptr) {
			// Shorthand to reduce line length. Sort-of a loop variable, it's just that
			//  the loop isn't guaranteed to be incremented each cycle in the outer loop.
			const uint32 i = vehicle->weapons.weaponCount;

			Error_FatalF(i == WEAPON_MAXWEAPONS,
						 "Building (ID %i) has too many upgrade parts with weapons", static_cast<sint32>(vehicle->objID));

			vehicle->weapons.fireNullPairFrames[i] = 0;

			// Unlike Vehicle_FindNull, these always format the part name based on vehicle->contAct1, but maybe both names would be the same(?)

			for (uint32 j = 0; j < WEAPON_MAXFIRES; j++) {
				vehicle->weapons.fireNullPairs[i][j] = nullptr;

				const char* firePartName = Gods98::Container_FormatPartName(vehicle->contAct1, vehicle->fireNullName, &j);
				vehicle->weapons.fireNullPairs[i][j] = Gods98::Container_SearchTree(partChildCont, firePartName, Gods98::Container_SearchMode::FirstMatch, nullptr);
				
				/// TODO: Yes, this is assigned before the null check. But is that what *should* happen?
				vehicle->weapons.fireNullPairFrames[i]++;
				if (vehicle->weapons.fireNullPairs[i][j] == nullptr) {
					break;
				}
			}

			const uint32 zero = 0;

			const char* xPartName = Gods98::Container_FormatPartName(vehicle->contAct1, vehicle->xPivot, &zero);
			vehicle->weapons.xPivotNulls[i] = Gods98::Container_SearchTree(partChildCont, xPartName, Gods98::Container_SearchMode::FirstMatch, nullptr);

			const char* yPartName = Gods98::Container_FormatPartName(vehicle->contAct1, vehicle->yPivot, &zero);
			vehicle->weapons.yPivotNulls[i] = Gods98::Container_SearchTree(partChildCont, yPartName, Gods98::Container_SearchMode::FirstMatch, nullptr);

			vehicle->weapons.weaponParts[i] = upgradePart->partInfo;
			vehicle->weapons.weaponCount++;
		}
	}
}


// Similar to `Vehicle_Load`, this does not free the passed pointer.
// <LegoRR.exe @0046d0d0>
void __cdecl LegoRR::Vehicle_Remove(VehicleModel* vehicle)
{
	// Remove upgrade model part objects.
	Vehicle_SetUpgradeLevel(vehicle, -1);

	// Remove MeshLODs.
	/// FIX APPLY: Free MeshLODs.
	MeshLOD_Free(vehicle->polyMedium1);
	vehicle->polyMedium1 = nullptr;

	if (vehicle->polyMedium2 != nullptr) {
		MeshLOD_Free(vehicle->polyMedium2);
		vehicle->polyMedium2 = nullptr;
	}

	// Remove source string allocations.
	if (vehicle->flags & VEHICLE_FLAG_SOURCE) {
		/// FIX APPLY: Free depositNullName, driverNullName, yPivot, xPivot, and carryNullName.
		if (vehicle->wheelNullName) {
			Gods98::Mem_Free(vehicle->wheelNullName);
			vehicle->wheelNullName = nullptr;
		}

		if (vehicle->drillNullName) {
			Gods98::Mem_Free(vehicle->drillNullName);
			vehicle->drillNullName = nullptr;
		}
		if (vehicle->depositNullName) {
			Gods98::Mem_Free(vehicle->depositNullName);
			vehicle->depositNullName = nullptr;
		}
		if (vehicle->fireNullName) {
			Gods98::Mem_Free(vehicle->fireNullName);
			vehicle->fireNullName = nullptr;
		}
		if (vehicle->driverNullName) {
			Gods98::Mem_Free(vehicle->driverNullName);
			vehicle->driverNullName = nullptr;
		}

		if (vehicle->yPivot) {
			Gods98::Mem_Free(vehicle->yPivot);
			vehicle->yPivot = nullptr;
		}
		if (vehicle->xPivot) {
			Gods98::Mem_Free(vehicle->xPivot);
			vehicle->xPivot = nullptr;
		}

		if (vehicle->carryNullName) {
			Gods98::Mem_Free(vehicle->carryNullName);
			vehicle->carryNullName = nullptr;
		}
		if (vehicle->cameraNullName) {
			Gods98::Mem_Free(vehicle->cameraNullName);
			vehicle->cameraNullName = nullptr;
		}
	}

	// Remove null containers.
	/// FIX APPLY: Properly remove Container references that were being leaked.
	_Vehicle_RemoveNulls(vehicle);
	_Vehicle_RemoveWeaponNulls(vehicle);
	_Vehicle_RemoveWheelNulls(vehicle);

	// Remove wheel mesh containers.
	for (uint32 i = 0; i < VEHICLE_MAXWHEELS; i++) {
		// Note: Removing wheel nulls handled by _Vehicle_RemoveWheelNulls.
		/// CHANGE: Should probably be removing wheel nulls before the container they're likely referencing.
		//if (vehicle->wheelNulls[i]) {
		//	Gods98::Container_Remove(vehicle->wheelNulls[i]);
		//	vehicle->wheelNulls[i] = nullptr;
		//}

		if (vehicle->contWheels[i]) {
			Gods98::Container_Remove(vehicle->contWheels[i]);
			vehicle->contWheels[i] = nullptr;
		}
	}

	// Remove activity containers.
	Gods98::Container_Remove(vehicle->contAct1);
	vehicle->contAct1 = nullptr;

	if (vehicle->contAct2 != nullptr) {
		Gods98::Container_Remove(vehicle->contAct2);
		vehicle->contAct2 = nullptr;
	}
}


// <LegoRR.exe @0046d250>
void __cdecl LegoRR::Vehicle_SetUpgradeLevel(VehicleModel* vehicle, uint32 objLevel)
{
	/// FIX APPLY: Remove nulls before they can be changed by Upgrade_SetUpgradeLevel.
	_Vehicle_RemoveNulls(vehicle);
	_Vehicle_RemoveWeaponNulls(vehicle);
	_Vehicle_RemoveWheelNulls(vehicle);

	Upgrade_SetUpgradeLevel(&vehicle->upgrades, objLevel);
	Vehicle_SetUpgradeActivity(vehicle, nullptr);
}

// <LegoRR.exe @0046d280>
void __cdecl LegoRR::Vehicle_HideWheels(VehicleModel* vehicle, bool32 hide)
{
	for (uint32 i = 0; i < VEHICLE_MAXWHEELS; i++) {
		Gods98::Container_Hide(vehicle->contWheels[i], hide);
	}
}

// <LegoRR.exe @0046d2b0>
bool32 __cdecl LegoRR::Vehicle_Clone(IN VehicleModel* srcVehicle, OUT VehicleModel* destVehicle)
{
	// Directly copy data from source model.
	*destVehicle = *srcVehicle;

	// Dissociate nulls from source model.
	/// FIX APPLY: We don't want to be referencing the nulls of another container.
	destVehicle->drillNull   = nullptr;
	destVehicle->depositNull = nullptr;
	destVehicle->driverNull  = nullptr;

	for (uint32 i = 0; i < VEHICLE_MAXCARRYS; i++) {
		destVehicle->carryNulls[i] = nullptr;
	}
	for (uint32 i = 0; i < VEHICLE_MAXCAMERAS; i++) {
		destVehicle->cameraNulls[i] = nullptr;
	}

	for (uint32 i = 0; i < WEAPON_MAXWEAPONS; i++) {
		for (uint32 j = 0; j < WEAPON_MAXFIRES; j++) {
			destVehicle->weapons.fireNullPairs[i][j] = nullptr;
		}
		destVehicle->weapons.xPivotNulls[i] = nullptr;
		destVehicle->weapons.yPivotNulls[i] = nullptr;

		destVehicle->weapons.weaponParts[i] = nullptr;
	}

	// This is a cloned model, and doesn't own string memory allocations.
	destVehicle->flags &= ~VEHICLE_FLAG_SOURCE;

	// Clone activity containers.
	destVehicle->contAct1 = Gods98::Container_Clone(srcVehicle->contAct1);
	if (destVehicle->contAct1 != nullptr) {
		// Clone and parent second activity container.
		if (srcVehicle->contAct2 != nullptr) {
			destVehicle->contAct2 = Gods98::Container_Clone(srcVehicle->contAct2);
			Gods98::Container_SetParent(destVehicle->contAct2, destVehicle->contAct1);
			Gods98::Container_SetPosition(destVehicle->contAct2, destVehicle->contAct1, 0.0f, 0.0f, 0.0f);
			Gods98::Container_SetOrientation(destVehicle->contAct2, destVehicle->contAct1, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
			Gods98::Container_SetPerspectiveCorrection(destVehicle->contAct2, true);
		}
		/// REFACTOR: Moved above MeshLOD_Clone.
		Gods98::Container_SetPerspectiveCorrection(destVehicle->contAct1, true);

		// Clone MeshLODs.
		destVehicle->polyMedium1 = MeshLOD_Clone(srcVehicle->polyMedium1);
		if (srcVehicle->polyMedium2 != nullptr) {
			destVehicle->polyMedium2 = MeshLOD_Clone(srcVehicle->polyMedium2);
		}

		// Clone wheel mesh containers.
		Vehicle_PopulateWheelNulls(destVehicle);
		/// TODO: Should we be cloning and parenting contWheels before populating Nulls!??

		for (uint32 i = 0; i < VEHICLE_MAXWHEELS; i++) {
			destVehicle->contWheels[i] = nullptr;
		}
		/// JANK: Source vehicle models can never be used directly because only the first wheel is ever assigned.
		///       This is also why cloning is passing srcVehicle->contWheels[0], but assigning to index i.
		if (srcVehicle->contWheels[0] != nullptr) {
			/// TODO: Does cloning more than the first contWheel have any effect?
			for (uint32 i = 0; i < destVehicle->wheelNullFrames; i++) {
				destVehicle->contWheels[i] = Gods98::Container_Clone(srcVehicle->contWheels[0]);
				if (destVehicle->contWheels[i] != nullptr) {
					Gods98::Container_SetParent(destVehicle->contWheels[i], destVehicle->contAct1);
					Gods98::Container_SetPerspectiveCorrection(destVehicle->contWheels[i], true);
				}
			}
		}

		return true;
	}
	return false;
}


// <LegoRR.exe @0046d520>
void __cdecl LegoRR::Vehicle_PopulateWheelNulls(VehicleModel* vehicle)
{
	_Vehicle_RemoveWheelNulls(vehicle);

	vehicle->wheelNullFrames = 0;

	for (uint32 i = 0; i < VEHICLE_MAXWHEELS; i++) {
		const char* partName = Gods98::Container_FormatPartName(vehicle->contAct1, vehicle->wheelNullName, &i);
		vehicle->wheelNulls[i] = Gods98::Container_SearchTree(vehicle->contAct1, partName, Gods98::Container_SearchMode::FirstMatch, nullptr);
		if (vehicle->wheelNulls[i] != nullptr) {
			vehicle->wheelNullFrames++;
		}
	}
}

#pragma endregion
