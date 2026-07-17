#pragma once

//class CEconItemDefinition
//{
//public:
//	bool IsWeapon()
//	{
//		// Every gun supports at least 4 stickers.
//		return GetStickersSupportedCount() >= 4;
//	}
//
//	bool IsKnife(bool bExcludeDefault)
//	{
//		static constexpr FNV1A_t CSGO_Type_Knife = FNV1A::HashConst("#CSGO_Type_Knife");
//		if (FNV1A::Hash(m_pszItemTypeName) != CSGO_Type_Knife)
//			return false;
//
//		return bExcludeDefault ? m_uDefinitionIndex >= 500 : true;
//	}
//
//	bool IsGlove(bool bExcludeDefault)
//	{
//		static constexpr auto Type_Hands = FNV1A::HashConst("#Type_Hands");
//		if (FNV1A::Hash(m_pszItemTypeName) != Type_Hands)
//			return false;
//
//		const bool bDefaultGlove = m_uDefinitionIndex == WEAPON_DEFAULT_T_GLOVES || m_uDefinitionIndex == WEAPON_DEFAULT_CT_GLOVES;
//		return bExcludeDefault ? !bDefaultGlove : true;
//	}
//
//	bool IsWeaponCase()
//	{
//		static constexpr FNV1A_t Type_WeaponCase = FNV1A::HashConst("#CSGO_Type_WeaponCase");
//		return FNV1A::Hash(m_pszItemTypeName) == Type_WeaponCase;
//	}
//
//	bool IsKey()
//	{
//		static constexpr FNV1A_t Tool_WeaponCase_KeyTag = FNV1A::HashConst("#CSGO_Tool_WeaponCase_KeyTag");
//		return FNV1A::Hash(m_pszItemTypeName) == Tool_WeaponCase_KeyTag;
//	}
//
//	bool IsAgent(bool bExcludeDefault)
//	{
//		static constexpr FNV1A_t Type_CustomPlayer = FNV1A::HashConst("#Type_CustomPlayer");
//		if (FNV1A::Hash(m_pszItemTypeName) != Type_CustomPlayer)
//			return false;
//
//		const bool bDefaultAgent = m_uDefinitionIndex == WEAPON_DEFAULT_CT_AGENT || m_uDefinitionIndex == WEAPON_DEFAULT_T_AGENT;
//		return bExcludeDefault ? !bDefaultAgent : true;
//	}	
//
//	const char* GetModelName()
//	{
//		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0xD8);
//	}
//
//	int GetStickersSupportedCount()
//	{
//		return *reinterpret_cast<int*>((uintptr_t)(this) + 0x100);
//	}
//	
//	const char* GetSimpleWeaponName()
//	{
//		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x1F0);
//	}
//
//	int GetLoadoutSlot()
//	{
//		return *reinterpret_cast<int*>((uintptr_t)(this) + 0x2E8);
//	}
//public:
//	MEM_PAD(0x8);  // vtable
//	void* m_pKVItem;
//	std::uint16_t m_uDefinitionIndex;
//	CUtlVector<std::uint16_t> m_nAssociatedItemsDefIndexes;
//	bool m_bEnabled;
//	const char* m_szPrefab;
//	std::uint8_t m_unMinItemLevel;
//	std::uint8_t m_unMaxItemLevel;
//	std::uint8_t m_nItemRarity;
//	std::uint8_t m_nItemQuality;
//	std::uint8_t m_nForcedItemQuality;
//	std::uint8_t m_nDefaultDropItemQuality;
//	std::uint8_t m_nDefaultDropQuantity;
//	CUtlVector<void*> m_vecStaticAttributes;
//	std::uint8_t m_nPopularitySeed;
//	void* m_pPortraitsKV;
//	const char* m_pszItemBaseName;
//	bool m_bProperName;
//	const char* m_pszItemTypeName;
//	std::uint32_t m_unItemTypeID;
//	const char* m_pszItemDesc;
//};

#pragma pack(push, 1)
class CEconItemDefinition
{
public:
	bool IsWeapon()
	{
		// Every gun supports at least 4 stickers.
		return GetStickersSupportedCount() >= 4;
	}

	bool IsKnife(bool bExcludeDefault)
	{
		static constexpr FNV1A_t CSGO_Type_Knife = FNV1A::HashConst("#CSGO_Type_Knife");
		if (FNV1A::Hash(m_pszItemTypeName) != CSGO_Type_Knife)
			return false;

		return bExcludeDefault ? m_uDefinitionIndex >= 500 : true;
	}

	bool IsGlove(bool bExcludeDefault)
	{
		static constexpr auto Type_Hands = FNV1A::HashConst("#Type_Hands");
		if (FNV1A::Hash(m_pszItemTypeName) != Type_Hands)
			return false;

		const bool bDefaultGlove = m_uDefinitionIndex == WEAPON_DEFAULT_T_GLOVES || m_uDefinitionIndex == WEAPON_DEFAULT_CT_GLOVES;
		return bExcludeDefault ? !bDefaultGlove : true;
	}

	bool IsWeaponCase()
	{
		static constexpr FNV1A_t Type_WeaponCase = FNV1A::HashConst("#CSGO_Type_WeaponCase");
		return FNV1A::Hash(m_pszItemTypeName) == Type_WeaponCase;
	}

	bool IsKey()
	{
		static constexpr FNV1A_t Tool_WeaponCase_KeyTag = FNV1A::HashConst("#CSGO_Tool_WeaponCase_KeyTag");
		return FNV1A::Hash(m_pszItemTypeName) == Tool_WeaponCase_KeyTag;
	}

	bool IsAgent(bool bExcludeDefault)
	{
		static constexpr FNV1A_t Type_CustomPlayer = FNV1A::HashConst("#Type_CustomPlayer");
		if (FNV1A::Hash(m_pszItemTypeName) != Type_CustomPlayer)
			return false;

		const bool bDefaultAgent = m_uDefinitionIndex == WEAPON_DEFAULT_CT_AGENT || m_uDefinitionIndex == WEAPON_DEFAULT_T_AGENT;
		return bExcludeDefault ? !bDefaultAgent : true;
	}

	const char* GetModelName()
	{
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0xD8);
	}

	int GetStickersSupportedCount()
	{
		return *reinterpret_cast<int*>((uintptr_t)(this) + 0xF8);
	}

	const char* GetSimpleWeaponName()
	{
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x200);
	}

	int GetLoadoutSlot()
	{
		return *reinterpret_cast<int*>((uintptr_t)(this) + 0x2D8);
	}
public:
	MEM_PAD(0x8);  // vtable
	void* m_pKVItem; // 0x8
	std::uint16_t m_uDefinitionIndex; // 0x10
	MEM_PAD(0x1e)
		bool m_bEnabled; // 0x30
	MEM_PAD(0xf);
	std::uint8_t m_unMinItemLevel; // 0x40
	std::uint8_t m_unMaxItemLevel; // 0x41
	std::uint8_t m_nItemRarity; // 0x42
	std::uint8_t m_nItemQuality; // 0x43
	std::uint8_t m_nForcedItemQuality; // 0x44
	std::uint8_t m_nDefaultDropItemQuality; // 0x45
	std::uint8_t m_nDefaultDropQuantity; // 0x46
	MEM_PAD(0x19);
	std::uint8_t m_nPopularitySeed; // 0x60
	MEM_PAD(0x7);
	void* m_pPortraitsKV; // 0x68
	char* m_pszItemBaseName; // 0x70
	bool m_bProperName; // 0x78
	MEM_PAD(0x7);
	char* m_pszItemTypeName; // 0x80
	MEM_PAD(0x8);
	char* m_pszItemDesc; // 0x90
	std::uint32_t m_iExpirationTimeStamp; // 0x98
	std::uint32_t m_iCreationTimeStamp; // 0x9c
	char* m_pszInventoryModel; // 0xa0
	char* m_pszInventoryImage; // 0xa8
	MEM_PAD(0x18);
	int m_iInventoryImagePosition[2]; // 0xc8
	int m_iInventoryImageSize[2]; // 0xd0
	char* m_pszBaseDisplayModel; // 0xd8
	bool m_bLoadOnDemand; // 0xe0
	MEM_PAD(0x1);
	bool m_bHideBodyGroupsDeployedOnly; // 0xe2
	MEM_PAD(0x5);
	char* m_pszWorldDisplayModel; // 0xe8
	char* m_pszHolsteredModel; // 0xf0
	char* m_pszWorldExtraWearableModel; // 0xf8
	std::uint32_t m_iStickerSlots; // 0x100
	MEM_PAD(0x4);
	char* m_pszIconDefaultImage; // 0x108
	bool m_bAttachToHands; // 0x110
	bool m_bAttachToHandsVMOnly; // 0x111
	bool m_bFlipViewModel; // 0x112
	bool m_bActAsWearable; // 0x113
	MEM_PAD(0x24);
	std::uint32_t m_iItemType; // 0x138
	MEM_PAD(0x4);
	char* m_pszBrassModelOverride; // 0x140
	char* m_pszZoomInSoundPath; // 0x148
	char* m_pszZoomOutSoundPath; // 0x150
	MEM_PAD(0x18);
	std::uint32_t m_nSoundMaterialID; // 0x170
	bool m_bDisableStyleSelection; // 0x174
	MEM_PAD(0x13);
	char* m_pszParticleFile; // 0x188
	char* m_pszParticleSnapshotFile; // 0x190
	MEM_PAD(0x40);
	char* m_pszItemClassname; // 0x1d8
	char* m_pszItemLogClassname; // 0x1e0
	char* m_pszItemIconClassname; // 0x1e8
	char* m_pszDefinitionName; // 0x1f0
	bool m_bHidden; // 0x1f8
	bool m_bShouldShowInArmory; // 0x1f9
	bool m_bBaseItem; // 0x1fa
	bool m_bFlexibleLoadoutDefault; // 0x1fb
	bool m_bImported; // 0x1fc
	bool m_bOnePerAccountCDKEY; // 0x1fd
	MEM_PAD(0xa);
	char* m_pszArmoryDesc; // 0x208
	MEM_PAD(0x8);
	char* m_pszArmoryRemap; // 0x218
	char* m_pszStoreRemap; // 0x220
	char* m_pszClassToken; // 0x228
	char* m_pszSlotToken; // 0x230
	std::uint32_t m_iDropType; // 0x238
	MEM_PAD(0x4);
	char* m_pszHolidayRestriction; // 0x240
	std::uint32_t m_iSubType; // 0x248
	MEM_PAD(0xc)
		std::uint32_t m_unEquipRegionMask; // 0x258
	std::uint32_t m_unEquipRegionConflictMask; // 0x25c
	MEM_PAD(0x50);
	bool m_bPublicItem; // 0x2b0
	bool m_bIgnoreInCollectionView; // 0x2b1
	MEM_PAD(0x36);
	int m_iLoadoutSlot; // 0x2e8
	MEM_PAD(0x94);
}; // Size: 0x380
#pragma pack(pop)