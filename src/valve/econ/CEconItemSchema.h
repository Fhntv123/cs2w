#pragma once

class CEconItemDefinition;

inline constexpr std::uint64_t Helper_GetAlternateIconKeyForWeaponPaintWearItem(std::uint16_t nDefIdx, std::uint32_t nPaintId, std::uint32_t nWear)
{
    return (nDefIdx << 16) + (nPaintId << 2) + nWear;
}

struct AlternateIconData_t
{
    const char* m_szSimpleName;
    const char* m_szLargeSimpleName;
    MEM_PAD(0x10);
};

#pragma pack(push, 1)
struct CPaintKit
{
    std::uint64_t m_nID; // 0x0
    const char* m_szName; // 0x8
    const char* m_szDescriptionString; // 0x10
    const char* m_szDescriptionTag; // 0x18
    MEM_PAD(0x8);
    const char* m_szPattern; // 0x28
    const char* m_szNormal; // 0x30
    const char* m_szLogoMaterial; // 0x38
    MEM_PAD(0x4);
    std::uint32_t m_nRarity; // 0x44
    std::uint32_t m_nStyle; // 0x48
    Color m_rgbaColor[4]; // 0x4c
    Color m_rgbaLogoColor[4]; // 0x5c
    float m_flWearDefault; // 0x6c
    float m_flWearRemapMin; // 0x70
    float m_flWearRemapMax; // 0x74
    std::uint8_t m_nFixedSeed; // 0x78
    std::uint8_t m_uchPhongExponent; // 0x79
    std::uint8_t m_uchPhongAlbedoBoost; // 0x7a
    std::uint8_t m_uchPhongIntensity; // 0x7b
    float m_flPatternScale; // 0x7c
    float m_flPatternOffsetXStart; // 0x80
    float m_flPatternOffsetXEnd; // 0x84
    float m_flPatternOffsetYStart; // 0x88
    float m_flPatternOffsetYEnd; // 0x8c
    float m_flPatternRotateStart; // 0x90
    float m_flPatternRotateEnd; // 0x94
    float m_flLogoScale; // 0x98
    float m_flLogoOffsetX; // 0x9c
    float m_flLogoOffsetY; // 0xa0
    float m_flLogoRotation; // 0xa4
    bool m_bIgnoreWeaponSizeScale; // 0xa8
    MEM_PAD(0x3);
    std::uint32_t m_nViewModelExponentOverrideSize; // 0xac
    bool m_bOnlyFirstMaterial; // 0xb0
    bool m_bUseNormalModel; // 0xb1
    bool m_bUseLegacyModel; // 0xb2
    MEM_PAD(0x1);
    float m_flPearlesent; // 0xb4
    const char* m_szVmtPath; // 0xb8
    MEM_PAD(0x8);
    const char* m_szCompositeMaterialPath; // 0xc8
    void* m_pkvVmtOverrides; // 0xd0
    MEM_PAD(0x8);

    bool uses_old_model()
    {
        return *reinterpret_cast<bool*>((uintptr_t)(this) + 0xAE);
    }
}; // Size: 0xe0
#pragma pack(pop)

//class CPaintKit
//{
//public:
//    bool IsUsingLegacyModel() const
//    {
//        return static_cast<bool>(reinterpret_cast<std::uintptr_t>(this) + 0xB2);
//    }
//public:
//    int m_nID;
//    const char* m_szName;
//    const char* m_szDescriptionString;
//    const char* m_szDescriptionTag;
//    MEM_PAD(0x24);
//    int m_nRarity;
//    int m_nStyle;
//    Color m_rgbaColor[4];
//    Color m_rgbaLogoColor[4];
//    float m_flWearDefault;
//    float m_flWearRemapMin;
//    float m_flWearRemapMax;
//    unsigned char m_nFixedSeed;
//    unsigned char m_uchPhongExponent;
//    unsigned char m_uchPhongAlbedoBoost;
//    unsigned char m_uchPhongIntensity;
//    float m_flPatternScale;
//    float m_flPatternOffsetXStart;
//    float m_flPatternOffsetXEnd;
//    float m_flPatternOffsetYStart;
//    float m_flPatternOffsetYEnd;
//    float m_flPatternRotateStart;
//    float m_flPatternRotateEnd;
//    float m_flLogoScale;
//    float m_flLogoOffsetX;
//    float m_flLogoOffsetY;
//    float m_flLogoRotation;
//    bool m_bIgnoreWeaponSizeScale;
//    MEM_PAD(0x3);
//    int m_nViewModelExponentOverrideSize;
//    bool m_bOnlyFirstMaterial;
//    bool m_bIsLegacyModel;
//    MEM_PAD(0x3);
//    float m_flPearlescent;
//};

class CStickerKit
{
public:
    int m_nID;
    int m_nRarity;
    const char* m_szName;
    const char* m_szDescription;
    const char* m_szItemName;
    const char* m_szMaterialPath;
    const char* m_szMaterialPathNoDrips;
    const char* m_szInventoryImage;
    int m_nTournamentID;
    int m_nTournamentTeamID;
    int m_nTournamentPlayerID;
    bool m_bMaterialPathIsAbsolute;
    bool m_bCannotTrade;
    MEM_PAD(0x2);
    float m_flRotateStart;
    float m_flRotateEnd;
    float m_flScaleMin;
    float m_flScaleMax;
    float m_flWearMin;
    float m_flWearMax;
    const char* m_szIconURLSmall;
    const char* m_szIconURLLarge;
    void* m_pKVItem;
};

//class CEconItemSchema {
//public:
//    char pad_0008[120];
//    CUtlSimpleMap<int, CEconItemRarityDefinition> mapRarities;
//    CUtlSimpleMap<int, CEconItemQualityDefinition> mapQualities;
//    char pad_00C8[88];
//    CUtlSimpleMap<int, CEconItemDefinition*> mapItemsSorted;
//    char pad_0148[296];
//    CUtlSimpleMap<int, CAlternateIconData> mapAlternateIcons;
//    char pad_0298[80];
//    CUtlSimpleMap<int, CPaintKit*> mapPaintKits;
//    CUtlSimpleMap<int, CStickerKit*> mapStickerKits;
//    CUtlSimpleMap<const char*, CStickerKit*> mapStickerKitsByName;
//    char pad_0360[112];
//    CUtlSimpleMap<const char*, KeyValues*> mapPrefabs;
//    char pad_03F8[24];
//    CUtlVectorSimple<CEconColorDefinition*> vecColors;
//    char pad_0420[168];
//    CUtlSimpleMap<int, CEconMusicDefinition*> mapMusic;
//    char pad_04F0[1112];
//    CUtlVectorSimple<const char*> vecTeams;
//    char pad_0958[88];
//    void* GetAttributeDefinitionInterface(int iAttribIndex) {
//        return CallClassFn<void*, 27>(this, iAttribIndex);
//    }

class CEconMusicDefinition;
class CEconColorDefinition
{
    //public:
    //    const char* GetName(void) const { return m_strName.Get(); }
    //    const char* GetColorName(void) const { return m_strColorName.Get(); }		// meant for passing into VGUI styles, etc.
    //    const char* GetHexColor(void) const { return m_strHexColor.Get(); }
    //
    //private:
    //    // The English name of this color. Only used for lookup.
    //    CUtlConstString m_strName;
    //    // The VGUI name of the color in our schema. This will be used to set values
    //    // for VGUI controls.
    //    CUtlConstString m_strColorName;
    //    // The hex string value of this color. This will be used for Web display.
    //    CUtlConstString m_strHexColor;
};

struct ItemListEntry_t
{
    int m_nItemDefinition;
    int m_nPaintKit;
    MEM_PAD(20)

        EItemDefinitionIndexes GetWeaponDefinition() const noexcept
    {
        return static_cast<EItemDefinitionIndexes>(m_nItemDefinition);
    }
};

class EconLootListDefinition
{
public:
    const char* GetName()
    {
        return Memory::CallVFunc<const char*, 0U>(this);
    }

    const CUtlVector<ItemListEntry_t>& GetLootListContents()
    {
        return Memory::CallVFunc<const CUtlVector<ItemListEntry_t>&, 1U>(this);
    }
};

class EconItemSetDefinition
{
public:
    const char* GetLockKey()
    {
        return Memory::CallVFunc<const char*, 1U>(this);
    }

    int GetItemCount()
    {
        return Memory::CallVFunc<int, 4U>(this);
    }

    int GetItemDefinition(int nIndex)
    {
        return Memory::CallVFunc<int, 5U>(this, nIndex);
    }

    int GetPaintKit(int nIndex)
    {
        return Memory::CallVFunc<int, 6U>(this, nIndex);
    }
};

class CEconItemSchema
{
public:
    MEM_PAD(0x128);
    MEM_PAD(0x130);
    CUtlMap<int, CEconItemDefinition*> m_SortedItemDefinitionMap;
    MEM_PAD(0x128);
    CUtlMap<uint64_t, AlternateIconData_t> m_AlternateIconsMap;
    MEM_PAD(0x50);
    CUtlMap<int, CPaintKit*> m_PaintKits;
    CUtlMap<int, CStickerKit*> m_StickerKits;
    MEM_PAD(0xD8);
    CUtlVector<CEconColorDefinition*> m_Colors;
    MEM_PAD(0xA8);
    CUtlMap<int, CEconMusicDefinition*> m_MusicKits;

public:
    CEconItemDefinition* GetItemDefinitionInterface(int nID)
    {
        return Memory::CallVFunc<CEconItemDefinition*, 4U>(this, nID);
    }

    auto& GetSortedItemDefinitionMap()
    {
        return *reinterpret_cast<CUtlMap<int, CEconItemDefinition*>*>(
            (uintptr_t)(this) + 0x130);
    }

    auto& GetPaintKits()
    {
        return *reinterpret_cast<CUtlMap<int, CPaintKit*>*>((uintptr_t)(this) +
            0x2F8);
    }

    const char* GetRarityName(int nRarity)
    {
        return Memory::CallVFunc<const char*, 19U>(this, nRarity);
    }

    void* GetAttributeDefinitionInterface(int nAttributeIndex)
    {
        return Memory::CallVFunc<void*, 27U>(this, nAttributeIndex);
    }

    int GetItemSetCount()
    {
        return Memory::CallVFunc<int, 28U>(this);
    }

    EconItemSetDefinition* GetItemSet(int nIndex)
    {
        return Memory::CallVFunc<EconItemSetDefinition*, 29U>(this, nIndex);
    }

    EconLootListDefinition* GetLootList(int nIndex)
    {
        return Memory::CallVFunc<EconLootListDefinition*, 32U>(this, nIndex);
    }

    int GetLootListCount()
    {
        return Memory::CallVFunc<int, 34U>(this);
    }

    CEconItemDefinition* GetItemDefinitionByName(const char* szName)
    {
        return Memory::CallVFunc<CEconItemDefinition*, 42U>(this, szName);
    }
};