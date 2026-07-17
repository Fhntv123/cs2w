#pragma once
#include "../../../memory/Memory.h"
#include "utlmap.h"
#include "CEconItem.h"

class C_EconItemSchema {
public:
	auto& GetSortedItemDefinitionMap() {
		return *reinterpret_cast<CUtlMap<int, C_EconItemDefinition*>*>(
			(uintptr_t)(this) + 0x128);
	}

	//auto& GetMusicKits() {
	//	return *reinterpret_cast<CUtlMap<int, CMusicKit*>*>(
	//		(uintptr_t)(this) + 0x4D8);
	//}

	//auto& GetAlternateIconsMap() {
	//	return *reinterpret_cast<CUtlMap<uint64_t, AlternateIconData_t>*>(
	//		(uintptr_t)(this) + 0x280);
	//}

	auto& GetPaintKits() {
		return *reinterpret_cast<CUtlMap<int, C_PaintKit*>*>((uintptr_t)(this) +
			0x2F0);
	}
	auto& GetStickers() {
		return *reinterpret_cast<CUtlMap<int, CSkiterKit*>*>((uintptr_t)(this) + 0x318);
	}
	auto GetAttributeDefinitionInterface(int iAttribIndex)
	{
		return Memory::CallVFunc<void*, 27>(this, iAttribIndex);
	}

};

class C_EconItemSystem {
public:
	C_EconItemSchema* get_econ_item_schema() {
		return *reinterpret_cast<C_EconItemSchema**>((uintptr_t)(this) + 0x8);
	}
};

class i_client {
public:
	C_EconItemSystem* get_econ_item_system() {
		return Memory::CallVFunc<C_EconItemSystem*, 122>(this);
	}

};