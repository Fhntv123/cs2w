#include "../Precompiled.h"

bool Functions::Setup()
{
	fnSetViewAngle = reinterpret_cast<decltype(fnSetViewAngle)>(Memory::FindPattern(CLIENT_DLL, X("85 D2 75 3D 48 63 81 ? ? ? ?")));
	fnGetViewAngles = reinterpret_cast<decltype(fnGetViewAngles)>(Memory::FindPattern(CLIENT_DLL, X("4C 8B C1 85 D2 74 08 48 8D 05 ? ? ? ? C3")));
	fnGetBaseEntity = reinterpret_cast<decltype(fnGetBaseEntity)>(Memory::FindPattern(CLIENT_DLL, X("4C 8D 49 ? 81 FA")));
	fnCalculateWorldSpaceBones = reinterpret_cast<decltype(fnCalculateWorldSpaceBones)>(Memory::FindPattern(CLIENT_DLL, X("48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8D 6C 24 ? 48 8B 81 ? ? ? ? 44 8B FA 48 8B F1 8B DA 48 85 C0 0F 84 ? ? ? ? 48 8B 08 48 85 C9 0F 84 ? ? ? ?"))); //40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC D0
	fnTraceShape = reinterpret_cast<decltype(fnTraceShape)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 4C 24 ? 55 57")));
	//fnClipTraceToPlayers = reinterpret_cast<decltype(fnClipTraceToPlayers)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 0F 29 44 24")));//need fix
	fnClipRayToEntity = reinterpret_cast<decltype(fnClipRayToEntity)>(Memory::FindPattern(CLIENT_DLL, X("48 8B C4 48 89 58 ? 55 56 57 41 54 41 56 48 8D 68 ? 48 81 EC ? ? ? ? 48 8B 5D")));
	fnGetSurfaceData = reinterpret_cast<decltype(fnGetSurfaceData)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 80 78 18 00")), 0x1, 0x0));
	fnFindKeyVar = reinterpret_cast<decltype(fnFindKeyVar)>(Memory::FindPattern(PARTICLES_DLL, X("48 89 5C 24 ? 57 48 81 EC ? ? ? ? 33 C0 8B DA")));
	fnLoadKeyValues = reinterpret_cast<decltype(fnLoadKeyValues)>(Memory::GetAbsoluteAddress(Memory::FindPattern(TIER0_DLL, X("E8 ? ? ? ? 8B 4C 24 34 0F B6 D8")), 0x1, 0x0));
	fnSetTypeKV3 = reinterpret_cast<decltype(fnSetTypeKV3)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC 30 4C 8B 11 41 B9 ? ? ? ? 49 83 CA 01 0F B6 C2 80 FA 06 48 8B D9 44 0F 45 C8")));
	fnCreateParticleEffect = reinterpret_cast<decltype(fnCreateParticleEffect)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? F3 0F 10 1D ? ? ? ? 41 8B F8 8B DA 4C 8D 05")));
	fnUnknownParticleFunction = reinterpret_cast<decltype(fnUnknownParticleFunction)>(Memory::FindPattern(CLIENT_DLL, X("48 89 74 24 ? 57 48 83 EC ? 4C 8B D9 49 8B F9 33 C9 41 8B F0 83 FA ? 0F 84")));
	fnSetPlayerReady = reinterpret_cast<decltype(fnSetPlayerReady)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC 20 48 8B DA 48 8D 15 ? ? ? ? 48 8B CB FF 15 ? ? ? ? 85 C0 75 14 BA")));
	fnCreateMaterial = reinterpret_cast<decltype(fnCreateMaterial)>(Memory::FindPattern(MATERIAL_SYSTEM2_DLL, X("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05")));
	fnCacheParticleEffect = reinterpret_cast<decltype(fnCacheParticleEffect)>(Memory::FindPattern(CLIENT_DLL, X("4C 8B DC 53 48 81 EC ? ? ? ? F2 0F 10 05"))); //fixed
	fnGetEntityHandle = reinterpret_cast<decltype(fnGetEntityHandle)>(Memory::FindPattern(CLIENT_DLL, X("48 85 C9 74 32 48 8B 49 10 48 85 C9 74 29 44 8B 41 10 BA")));
	//fnInterpolateShootPosition = reinterpret_cast<decltype(fnInterpolateShootPosition)>(Memory::FindPattern(CLIENT_DLL, X("40 53 55 41 55 41 56 41 57 48 81 EC ? ? ? ? 4C 8B EA 4C 8B F1 41 8B D0 48 8D 8C 24")));
	fnLookupBone = reinterpret_cast<decltype(fnLookupBone)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8B 8D ? ? ? ? B3")), 0x1));
	fnTraceSmokeDensity = reinterpret_cast<decltype(fnTraceSmokeDensity)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 0F 28 F8 44 0F 28 54 24 ?")), 0x1, 0x0));
	fnGetInventoryManager = reinterpret_cast<decltype(fnGetInventoryManager)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8B D3 48 8B C8 4C 8B 00 41 FF 90 ? ? ? ? 48 8B CB")), 0x1, 0x0));
	fnUpdateCompositeMaterial = reinterpret_cast<decltype(fnUpdateCompositeMaterial)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 41 56 41 57 48 83 EC 20 44 0F B6 F2 48 8B F1 E8")));
	fnRegenerateWeaponSkin = reinterpret_cast<decltype(fnRegenerateWeaponSkin)>(Memory::FindPattern(CLIENT_DLL, X("40 55 53 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 44 0F B6 FA 48 8B D9 BA ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? ")));
	fnSetModel = reinterpret_cast<decltype(fnSetModel)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC ? 48 8B D9 4C 8B C2 48 8B 0D ? ? ? ? 48 8D 54 24")));
	fnSetMeshGroupMask = reinterpret_cast<decltype(fnSetMeshGroupMask)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8D 99 ? ? ? ? 48 8B 71")));
	//fnAddStatTrackEntity = reinterpret_cast<decltype(fnAddStatTrackEntity)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B DA")));
	fnAddNametagEntity = reinterpret_cast<decltype(fnAddNametagEntity)>(Memory::FindPattern(CLIENT_DLL, X("40 55 53 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B DA")));
	fnCreateSharedObjectSubclassEconItem = reinterpret_cast<decltype(fnCreateSharedObjectSubclassEconItem)>(Memory::FindPattern(CLIENT_DLL, X("48 83 EC 28 B9 48 00 00 00 E8 ? ? ? ? 48 85")));
	fnCreateBaseTypeCache = reinterpret_cast<decltype(fnCreateBaseTypeCache)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC ? 4C 8B 49 ? 44 8B D2")));
	fnGetClientSystem = reinterpret_cast<decltype(fnGetClientSystem)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 8B D8 85 C0 74 33")), 0x1, 0x0));
	fnSetDynamicAttributeValue = reinterpret_cast<decltype(fnSetDynamicAttributeValue)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 49 8B C0 48 8B CA 48 8B D0")), 0x1, 0x0));
	fnCAttributeStringInit = reinterpret_cast<decltype(fnCAttributeStringInit)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 7D ? 48 89 45 ? 49 8D 4F")), 0x1, 0x0));
	fnCAttributeStringFill = reinterpret_cast<decltype(fnCAttributeStringFill)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 41 83 CF 08")), 0x1, 0x0));
	fnSetDynamicAttributeValueString = reinterpret_cast<decltype(fnSetDynamicAttributeValueString)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8B 5C 24 ? 48 8B 6C 24 ? 48 8B 74 24 ? 48 83 C4 40")), 0x1, 0x0));
	fnCBufferStringInit = reinterpret_cast<decltype(fnCBufferStringInit)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 83 EC ? 8B 41 ? 48 8D 79")));
	fnDispatchEffect = reinterpret_cast<decltype(fnDispatchEffect)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 48 8B DA 48 8D 4C 24")));
	fnLoadFileForMe = reinterpret_cast<decltype(fnLoadFileForMe)>(Memory::FindPattern(CLIENT_DLL, X("40 55 57 41 56 48 83 EC 20 4C")));
	fnUpdateSubClass = reinterpret_cast<decltype(fnUpdateSubClass)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC 30 48 8B 41 10 48 8B D9 8B 50 30")));
	fnCreateNewSubtickMoveStep = reinterpret_cast<decltype(fnCreateNewSubtickMoveStep)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8B D0 48 8B CE E8 ? ? ? ? 48 8B C8")), 0x1, 0x0));
	//fnC_CSWeaponBase_UpdateSkin = reinterpret_cast<decltype(fnC_CSWeaponBase_UpdateSkin)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 8B DA 48 8B F9 E8")));
	//fnC_CSWeaponBase_UpdateCompositeMaterial = reinterpret_cast<decltype(fnC_CSWeaponBase_UpdateCompositeMaterial)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8D 8B ? ? ? ? 48 89 BC 24")), 0x1, 0x0));
	//fnCEconItemSchema_GetAttributeDefinitionInterface = reinterpret_cast<decltype(fnCEconItemSchema_GetAttributeDefinitionInterface)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 85 C0 74 ? E8 ? ? ? ? 0F B7 14 3B 48 8B C8 E8 ? ? ? ? 0F B6 48")), 0x1, 0x0));
	//fnCGCClientSharedObjectCache_FindTypeCache = reinterpret_cast<decltype(fnCGCClientSharedObjectCache_FindTypeCache)>(Memory::FindPattern(CLIENT_DLL, X("4C 8B 49 18 44 8B D2 4C 63 41 10 4F 8D 1C C1 49 8B C3")));

	return true;
}

bool ReturnAddressSpoofGadgets::FindGadgets()
{
	m_pClientGadet = Memory::FindPattern(CLIENT_DLL, X("FF 27"));
	if (!m_pClientGadet)
		return false;

	m_pServerGadet = Memory::FindPattern(SERVER_DLL, X("FF 27"));
	if (!m_pServerGadet)
		return false;

	m_pEngineGadet = Memory::FindPattern(ENGINE2_DLL, X("FF 27"));
	if (!m_pEngineGadet)
		return false;

	m_pSchemaSystemGadget = Memory::FindPattern(SCHEMASYSTEM_DLL, X("FF 27"));
	if (!m_pSchemaSystemGadget)
		return false;

	m_pAnimationSystemGadget = Memory::FindPattern(ANIMATIONSYSTEM_DLL, X("FF 27"));
	if (!m_pAnimationSystemGadget)
		return false;	
	
	m_pSDL3Gadget = Memory::FindPattern(SDL3_DLL, X("FF 27"));
	if (!m_pSDL3Gadget)
		return false;

	m_pTier0Gadget = Memory::FindPattern(TIER0_DLL, X("FF 27"));
	if (!m_pTier0Gadget)
		return false;
	
	m_pFileSystemGadget = Memory::FindPattern(FILESYSTEM_STDIO_DLL, X("FF 27"));
	if (!m_pFileSystemGadget)
		return false;

	m_pDbgHelpGadget = Memory::FindPattern(DBGHELP_DLL, X("FF 27"));
	if (!m_pDbgHelpGadget)
		return false;

	m_pParticlesGadget = Memory::FindPattern(PARTICLES_DLL, X("FF 27"));
	if (!m_pParticlesGadget)
		return false;

	m_pSceneSystemGadget = Memory::FindPattern(SCENESYSTEM_DLL, X("FF 27"));
	if (!m_pSceneSystemGadget)
		return false;

	m_pMaterialSystemGadget = Memory::FindPattern(MATERIAL_SYSTEM2_DLL, X("FF 27"));
	if (!m_pMaterialSystemGadget)
		return false;

	m_pSoundSystemGadget = Memory::FindPattern(SOUNDSYSTEM_DLL, X("FF 27"));
	if (!m_pSoundSystemGadget)
		return false;

	return true;
}