#pragma once

using GameTime_t = std::float_t;
using GameTick_t = std::int32_t;

using CFiringModeFloat = std::float_t[2];
using CSkillFloat = std::float_t[4];

struct BoneData_t
{
	Vector vecPosition;
	float flScale;
	Vector4D vecRotation;
};

class CEntitySubClassVDataBase
{
public:
	template <typename T> requires std::derived_from<T, CEntitySubClassVDataBase>
	inline T* As()
	{
		return (T*)this;
	}
};

struct PlayerContext_t
{
	int32_t m_nPlayerRenderTick;//0x0000
	float_t m_flRenderTickFraction; // 0x0004
	MEM_PAD(0x4); //0x0008
	int32_t m_nSomeTickCount; //0x000C
	Vector m_vecSomeVector; //0x0010
	MEM_PAD(971);
};

class c_gradient_fog {
public:
	SCHEMA_ADD_FIELD(bool, m_bIsEnabled, "C_GradientFog->m_bIsEnabled");
	SCHEMA_ADD_FIELD(float, m_flFogStartDistance, "C_GradientFog->m_flFogStartDistance");
	SCHEMA_ADD_FIELD(float, m_flFogEndDistance, "C_GradientFog->m_flFogEndDistance");
	SCHEMA_ADD_FIELD(float, m_flFogStrength, "C_GradientFog->m_flFogStrength");
	SCHEMA_ADD_FIELD(float, m_flFogFalloffExponent, "C_GradientFog->m_flFogFalloffExponent");
	SCHEMA_ADD_FIELD(float, m_flFogMaxOpacity, "C_GradientFog->m_flFogMaxOpacity");
	SCHEMA_ADD_FIELD(Color, m_fogColor, "C_GradientFog->m_fogColor");
};

class c_fog
{
private:
	c_gradient_fog* p_fog;
public:
	void create_fog();
	void fog();
};
inline std::unique_ptr<c_fog> g_fog = std::make_unique<c_fog>();

class CCollisionProperty;
class CEntityInstance;
class CEntityIdentity
{
public:
	// @note: handle index is not entity index
	SCHEMA_ADD_OFFSET(std::uint32_t, nIndex, 0x10);
	SCHEMA_ADD_FIELD(const char*, m_designerName, "CEntityIdentity->m_designerName");
	SCHEMA_ADD_FIELD(std::uint32_t, m_flags, "CEntityIdentity->m_flags");

	[[nodiscard]] bool IsValid()
	{
		return nIndex() != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex()
	{
		if (!IsValid())
			return ENT_ENTRY_MASK;

		return nIndex() & ENT_ENTRY_MASK;
	}

	[[nodiscard]] int GetSerialNumber()
	{
		if (nIndex() == INVALID_EHANDLE_INDEX || nIndex() == 0)
			return 0;

		return nIndex() >> NUM_SERIAL_NUM_SHIFT_BITS;
	}

	CEntityInstance* m_pInstance; // 0x00
};

class CEntityInstance
{
public:
	void GetSchemaClassInfo(SchemaClassInfoData_t** pReturn)
	{
		return Memory::CallVFunc<void, 42>(this, pReturn);
	}

	[[nodiscard]] CBaseHandle GetRefEHandle()
	{
		CEntityIdentity* pIdentity = m_pEntity();
		if (pIdentity == nullptr)
			return CBaseHandle();

		return CBaseHandle(pIdentity->GetEntryIndex(), pIdentity->GetSerialNumber() - (pIdentity->m_flags() & 1));
	}

	void OnPreDataChanged(EDataUpdateType nUpdateType)
	{
		return Memory::CallVFunc<void, 4U>(this, nUpdateType);
	}

	void OnDataChanged(EDataUpdateType nUpdateType)
	{
		return Memory::CallVFunc<void, 5U>(this, nUpdateType);
	}

	void PreDataUpdate(EDataUpdateType nUpdateType)
	{
		return Memory::CallVFunc<void, 6U>(this, nUpdateType);
	}

	void PostDataUpdate(EDataUpdateType nUpdateType)
	{
		return Memory::CallVFunc<void, 7U>(this, nUpdateType);
	}

	SCHEMA_ADD_FIELD(bool, m_bVisibleinPVS, "CEntityInstance->m_bVisibleinPVS");

	SCHEMA_ADD_FIELD(CEntityIdentity*, m_pEntity, "CEntityInstance->m_pEntity");
};

class VPhysicsCollisionAttribute_t
{
	MEM_PAD(0x10);
	uint64 m_nInteractsWith;
};

class CCollisionProperty
{
public:
	std::uint16_t CollisionMask()
	{
		return *reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x38);
	}

	void SetCollisionBounds(Vector& vecMins, Vector& vecMaxs)
	{
		static auto fnSetCollisionBounds = reinterpret_cast<void(__fastcall*)(CCollisionProperty*, Vector*, Vector*)>(Memory::FindPattern(CLIENT_DLL, "48 83 EC ? F2 0F 10 02 8B 42 08"));
		fnSetCollisionBounds(this, &vecMins, &vecMaxs);
	}

public:
	SCHEMA_ADD_FIELD(VPhysicsCollisionAttribute_t, m_collisionAttribute, "CCollisionProperty->m_collisionAttribute");
	SCHEMA_ADD_FIELD(float, m_flCapsuleRadius, "CCollisionProperty->m_flCapsuleRadius");

	SCHEMA_ADD_FIELD(std::uint8_t, m_usSolidFlags, "CCollisionProperty->m_usSolidFlags");
	SCHEMA_ADD_FIELD(std::uint8_t, m_CollisionGroup, "CCollisionProperty->m_CollisionGroup");

	SCHEMA_ADD_FIELD(Vector, m_vecMins, "CCollisionProperty->m_vecMins");
	SCHEMA_ADD_FIELD(Vector, m_vecMaxs, "CCollisionProperty->m_vecMaxs");
	SCHEMA_ADD_FIELD(Vector, m_vCapsuleCenter1, "CCollisionProperty->m_vCapsuleCenter1");
	SCHEMA_ADD_FIELD(Vector, m_vCapsuleCenter2, "CCollisionProperty->m_vCapsuleCenter2");
};

class CTransform
{
public:
	VectorAligned vecPosition;
	QuaternionAligned quatOrientation;
};
static_assert(alignof(CTransform) == 0x10);


struct HitboxData_t {
	Vector m_vMins;
	Vector m_vMaxs;
	Vector m_vCenter;
	float m_flRadius;
	int m_nHitboxIndex;
	int m_nShapeType;
	bool m_bMultipoint;


	bool SegmentIntersectsCapsule(Vector vStart, Vector vPoint) {

		Vector capsuleStart = m_vMins;
		Vector capsuleEnd = m_vMaxs;

		Vector d1 = vPoint - vStart;
		Vector d2 = capsuleEnd - capsuleStart;
		Vector r = vStart - capsuleStart;

		float a = d1.DotProduct(d1);
		float e = d2.DotProduct(d2);
		float f = d2.DotProduct(r);

		float s, t;
		float c = d1.DotProduct(r);
		float b = d1.DotProduct(d2);
		float denom = a * e - b * b;

		if (denom != 0.0f) {
			s = (b * f - c * e) / denom;
		}
		else {
			s = 0.0f;
		}

		s = std::clamp(s, 0.0f, 1.0f);

		t = (b * s + f) / e;
		t = std::clamp(t, 0.0f, 1.0f);

		Vector closestPoint1 = vStart + d1 * s;
		Vector closestPoint2 = capsuleStart + d2 * t;

		float distSqr = (closestPoint1 - closestPoint2).LengthSqr();

		return distSqr <= (m_flRadius * m_flRadius);
		// idgaf, im not adding degenerate checks nor AABB handling because thats never the case.
	}

};

class CHitBox
{
public:
	const char* m_name;				// 0x0000
	const char* m_sSurfaceProperty;	// 0x0008
	const char* m_sBoneName;		// 0x0010
	Vector m_vecMinBounds;			// 0x0018
	Vector m_vecMaxBounds;			// 0x0024
	float m_flShapeRadius;			// 0x0030
	std::uint32_t m_nBoneNameHash;	// 0x0034
	std::int32_t m_nGroupId;		// 0x0038
	std::uint8_t m_nShapeType;		// 0x003C
	bool m_bTranslationOnly; 		// 0x003D
	uint8_t __pad003e[0x2]; // 0x3e
	std::uint32_t m_CRC;			// 0x0040
	Color m_cRenderColor;			// 0x0044
	std::uint16_t m_nHitboxIndex;	// 0x0048
	MEM_PAD(0x26);

	HitboxData_t ConstructHitboxData(BoneData_t boneData, bool multipoint) {
		HitboxData_t returnData = {};
		returnData.m_nShapeType = this->m_nShapeType;
		returnData.m_flRadius = this->m_flShapeRadius;
		returnData.m_nHitboxIndex = this->m_nHitboxIndex;
		returnData.m_vMins = boneData.vecRotation.RotateVector(this->m_vecMinBounds * boneData.flScale) + boneData.vecPosition;
		returnData.m_vMaxs = boneData.vecRotation.RotateVector(this->m_vecMaxBounds * boneData.flScale) + boneData.vecPosition;
		returnData.m_vCenter = (returnData.m_vMins + returnData.m_vMaxs) / 2.f;
		returnData.m_bMultipoint = multipoint;
		return returnData;
	}
};

class CHitBoxSets
{
public:
	MEM_PAD(0x20);				// 0x0000
	std::uint32_t m_nNameHash;	// 0x0020
	MEM_PAD(0x4);				// 0x0024
	std::int32_t m_nHitboxCount;// 0x0028
	MEM_PAD(0x4);				// 0x002C
	CHitBox* m_pHitBox;			// 0x0030
	MEM_PAD(0x18);				// 0x0038
};

//struct CHitBoxSets
//{
//	const char* m_name;           // 0x0
//	uint32_t m_nNameHash;         // 0x8
//	CUtlVector<CHitBox> m_HitBoxes; // 0x10
//	const char* m_SourceFilename; // 0x28
//};

class CRenderMesh
{
public:
	MEM_PAD(0x140);						// 0x0000
	CHitBoxSets* m_pHitboxSets;		// 0x108
	std::int32_t	m_nHitboxSets;		// 0x110
}; // Size=0x118

class CRenderMeshes
{
public:
	CRenderMesh* m_pMeshes;
};

class CModel
{
public:
	CHitBox* GetHitBox(const std::int32_t nIndex);
	const char* GetHitBoxName(const std::int32_t nIndex);
	int GetHitBoxCount();
public:
	MEM_PAD(0x70);					// 0x0000
	std::int32_t m_nRendermeshCount;// 0x0070
	MEM_PAD(0x4);					// 0x0074
	CRenderMeshes* m_pRenderMeshes;	// 0x0078
};

class CCSPlayer_CameraServices {
public:

};

class CModelState
{
public:
	MEM_PAD(0x80);
	BoneData_t* m_pBoneArray;
	MEM_PAD(0x18);
	CStrongHandle<CModel> m_modelHandle;
public:
	SCHEMA_ADD_FIELD(CUtlSymbolLarge, m_ModelName, "CModelState->m_ModelName");
	SCHEMA_ADD_FIELD(CModel*, m_hModel, "CModelState->m_hModel");
	SCHEMA_ADD_FIELD_OFFSET(CModel*, m_hNewModel, "CModelState->m_hModel", 0x10);
};

class CSkeletonInstance;
class CGameSceneNode
{
public:
	CSkeletonInstance* GetSkeletonInstance()
	{
		return Memory::CallVFunc<CSkeletonInstance*, 8U>(this);
	}

	void SetMeshGroupMask(std::uint64_t uMeshGroup)
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnSetMeshGroupMask != nullptr);
#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnSetMeshGroupMask, this, uMeshGroup);
	}

	bool GetBonePositionAndRotation(const EBones eBoneIndex, Vector& vecBonePositionOut, Vector4D& vecBoneRotationOut);
	Vector GetBonePosition(const std::int32_t eBoneIndex);
public:
	SCHEMA_ADD_FIELD(CTransform, m_nodeToWorld, "CGameSceneNode->m_nodeToWorld");
	SCHEMA_ADD_FIELD(CEntityInstance*, m_pOwner, "CGameSceneNode->m_pOwner");

	SCHEMA_ADD_FIELD(bool, m_bDormant, "CGameSceneNode->m_bDormant");

	SCHEMA_ADD_FIELD(CNetworkOriginCellCoordQuantizedVector, m_vecOrigin, "CGameSceneNode->m_vecOrigin");
	SCHEMA_ADD_FIELD(Vector, m_vecAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
	SCHEMA_ADD_FIELD(Vector, m_vRenderOrigin, "CGameSceneNode->m_vRenderOrigin");

	SCHEMA_ADD_FIELD(QAngle, m_angRotation, "CGameSceneNode->m_angRotation");
	SCHEMA_ADD_FIELD(QAngle, m_angAbsRotation, "CGameSceneNode->m_angAbsRotation");

	struct InterpolationInformation_t
	{
		int m_nSrcTick;
		int m_nDstTick;
		float m_flFraction;
	};

	struct InterpolationTickFraction_t
	{
		int m_nTick;
		float m_flFraction;
	};

	bool CalculateInterpolationInfo(InterpolationInformation_t* pClientInterp, InterpolationInformation_t* pServerInterp0, InterpolationInformation_t* pServerInterp1, InterpolationTickFraction_t* pTickFraction)
	{
		using fnCalculateInterpolation = bool(__fastcall*)(CGameSceneNode*, InterpolationInformation_t*, InterpolationInformation_t*, InterpolationInformation_t*, InterpolationTickFraction_t*);
		static fnCalculateInterpolation oCalculateInterpolation = reinterpret_cast<fnCalculateInterpolation>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 8B 45 EF 3B 45 57")), 0x1, 0x0));
		return oCalculateInterpolation(this, pClientInterp, pServerInterp0, pServerInterp1, pTickFraction);
	}
};

class CSkeletonInstance : public CGameSceneNode
{
public:
	void CalculateWorldSpaceBones(std::uint32_t uBoneMask)
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnCalculateWorldSpaceBones != nullptr);
#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnCalculateWorldSpaceBones, this, uBoneMask);
	}

	void CalculateAnimationState(std::uint32_t nFlags)
	{
		static auto fn = reinterpret_cast<__int64(__fastcall*)(CSkeletonInstance*, int)>(Memory::FindPattern(CLIENT_DLL, "40 55 56 57 41 54 41 55 41 56 41 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8D 6C 24 ? 48 8B F1 48 C7 45 ? ? ? ? ? 48 8D 0D ? ? ? ? 48"));
		fn(this, nFlags);
	}

public:
	MEM_PAD(0x210); //0x01EC 
	Matrix4x2_t* m_pBoneCache; //0x01F0  //mb 0x210

	SCHEMA_ADD_FIELD(CModelState, m_modelState, "CSkeletonInstance->m_modelState");
	SCHEMA_ADD_FIELD(std::uint8_t, m_nHitboxSet, "CSkeletonInstance->m_nHitboxSet");
};

class CBodyComponent;
class C_BaseEntity : public CEntityInstance
{
public:
	[[nodiscard]] bool IsPlayerPawn()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSPlayerPawn");
	}

	[[nodiscard]] bool IsBasePlayerController()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSPlayerController");
	}

	[[nodiscard]] bool IsWeapon()
	{
		static SchemaClassInfoData_t* pWeaponBaseClass = nullptr;
		if (!pWeaponBaseClass)
			Interfaces::m_pSchemaSystem->FindTypeScopeForModule(X("client.dll"))->FindDeclaredClass(&pWeaponBaseClass, X("C_CSWeaponBase"));

		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return (pClassInfo->InheritsFrom(pWeaponBaseClass));
	}

	[[nodiscard]] bool IsViewModel()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo || !pClassInfo->szName)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSGOViewModel");
	}

	[[nodiscard]] bool IsViewModelAttachment()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo || !pClassInfo->szName)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_ViewmodelAttachmentModel");
	}

	[[nodiscard]] bool IsKnife()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo || !pClassInfo->szName)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_Knife");
	}

	[[nodiscard]] int LookupBone(const char* szBone)
	{
#		ifdef CS_PARANOID
		CS_ASSERT(Functions::fnLookupBone != nullptr);
#endif

		return Functions::fnLookupBone(this, szBone);
	}

public:
	SCHEMA_ADD_FIELD_OFFSET(CEntitySubClassVDataBase*, m_pVDataBase, "C_BaseEntity->m_nSubclassID", 0x8);
	SCHEMA_ADD_FIELD(CGameSceneNode*, m_pGameSceneNode, "C_BaseEntity->m_pGameSceneNode");
	SCHEMA_ADD_FIELD(CCollisionProperty*, m_pCollision, "C_BaseEntity->m_pCollision");
	SCHEMA_ADD_FIELD(CBodyComponent*, m_CBodyComponent, "C_BaseEntity->m_CBodyComponent");

	SCHEMA_ADD_FIELD(CUtlStringToken, m_nSubclassID, "C_BaseEntity->m_nSubclassID");

	SCHEMA_ADD_FIELD(bool, m_bTakesDamage, "C_BaseEntity->m_bTakesDamage");

	SCHEMA_ADD_FIELD(std::float_t, m_flFriction, "C_BaseEntity->m_flFriction");
	SCHEMA_ADD_FIELD(std::float_t, m_flWaterLevel, "C_BaseEntity->m_flWaterLevel");
	SCHEMA_ADD_FIELD(std::float_t, m_flSimulationTime, "C_BaseEntity->m_flSimulationTime");
	SCHEMA_ADD_FIELD(int, m_nSimulationTick, "C_BaseEntity->m_nSimulationTick");

	SCHEMA_ADD_FIELD(std::float_t, m_flAnimTime, "C_BaseModelEntity->m_flAnimTime");
	SCHEMA_ADD_FIELD(std::float_t, m_flGravityScale, "C_BaseEntity->m_flGravityScale");

	SCHEMA_ADD_FIELD(std::int32_t, m_iEFlags, "C_BaseEntity->m_iEFlags");
	SCHEMA_ADD_FIELD(int, m_iHealth, "C_BaseEntity->m_iHealth");
	SCHEMA_ADD_FIELD(int, m_iMaxHealth, "C_BaseEntity->m_iMaxHealth");

	SCHEMA_ADD_FIELD(std::uint8_t, m_nActualMoveType, "C_BaseEntity->m_nActualMoveType");
	SCHEMA_ADD_FIELD(std::uint8_t, m_lifeState, "C_BaseEntity->m_lifeState");
	SCHEMA_ADD_FIELD(std::uint8_t, m_iTeamNum, "C_BaseEntity->m_iTeamNum");

	SCHEMA_ADD_FIELD(std::uint32_t, m_fFlags, "C_BaseEntity->m_fFlags");

	SCHEMA_ADD_FIELD(GameTick_t, m_nNextThinkTick, "C_BaseEntity->m_nNextThinkTick");

	SCHEMA_ADD_FIELD(Vector, m_vecBaseVelocity, "C_BaseEntity->m_vecBaseVelocity");
	SCHEMA_ADD_FIELD(Vector, m_vecAbsVelocity, "C_BaseEntity->m_vecAbsVelocity");
	SCHEMA_ADD_FIELD(Vector, m_vecVelocity, "C_BaseEntity->m_vecVelocity");

	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hGroundEntity, "C_BaseEntity->m_hGroundEntity");
	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hOwnerEntity, "C_BaseEntity->m_hOwnerEntity");
};

class CBaseAnimGraphController {
public:
	SCHEMA_ADD_FIELD(bool, m_bSequenceFinished, "CBaseAnimGraphController->m_bSequenceFinished");
	SCHEMA_ADD_FIELD(float, m_flSoundSyncTime, "CBaseAnimGraphController->m_flSoundSyncTime");
	SCHEMA_ADD_FIELD(std::uintptr_t, m_nActiveIKChainMask, "CBaseAnimGraphController->m_nActiveIKChainMask");
	SCHEMA_ADD_FIELD(int, m_hSequence, "CBaseAnimGraphController->m_hSequence");
	SCHEMA_ADD_FIELD(int, m_flSeqStartTime, "CBaseAnimGraphController->m_flSeqStartTime");
	SCHEMA_ADD_FIELD(int, m_flSeqFixedCycle, "CBaseAnimGraphController->m_flSeqFixedCycle");
	SCHEMA_ADD_FIELD(int, m_nAnimLoopMode, "CBaseAnimGraphController->m_nAnimLoopMode");
	SCHEMA_ADD_FIELD(float, m_flPlaybackRate, "CBaseAnimGraphController->m_flPlaybackRate");
	SCHEMA_ADD_FIELD(bool, m_bNetworkedAnimationInputsChanged, "CBaseAnimGraphController->m_bNetworkedAnimationInputsChanged");
	SCHEMA_ADD_FIELD(bool, m_bNetworkedSequenceChanged, "CBaseAnimGraphController->m_bNetworkedSequenceChanged");
	SCHEMA_ADD_FIELD(bool, m_bLastUpdateSkipped, "CBaseAnimGraphController->m_bLastUpdateSkipped");

	SCHEMA_ADD_OFFSET(float, m_flCachedSeqCycleRate, 0x14CC);

	void WriteNewSequence(int& sequence) {
		sequence = m_hSequence();
	}
};


class CBodyComponent
{
public:
	SCHEMA_ADD_FIELD(CGameSceneNode*, m_pSceneNode, "CGlowProperty->m_pSceneNode");
public:
	CBaseAnimGraphController* GetBaseAnimGraphController()
	{
		return reinterpret_cast<CBaseAnimGraphController*>(std::uintptr_t(this) + 0x460);
	}
};

class CGlowProperty
{
public:
	SCHEMA_ADD_FIELD(bool, m_bFlashing, "CGlowProperty->m_bFlashing");
	SCHEMA_ADD_FIELD(bool, m_bGlowing, "CGlowProperty->m_bGlowing");

	SCHEMA_ADD_FIELD(std::int32_t, m_iGlowType, "CGlowProperty->m_iGlowType");
	SCHEMA_ADD_FIELD(std::int32_t, m_iGlowTeam, "CGlowProperty->m_iGlowTeam");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowRange, "CGlowProperty->m_nGlowRange");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowRangeMin, "CGlowProperty->m_nGlowRangeMin");

	SCHEMA_ADD_FIELD(std::float_t, m_flGlowTime, "CGlowProperty->m_flGlowTime");
	SCHEMA_ADD_FIELD(std::float_t, m_flGlowStartTime, "CGlowProperty->m_flGlowStartTime");

	SCHEMA_ADD_FIELD(Vector, m_fGlowColor, "CGlowProperty->m_fGlowColor");

	SCHEMA_ADD_FIELD(Color, m_glowColorOverride, "CGlowProperty->m_glowColorOverride");

	//class CGlowProperty
	//{
	//public:
	//    char pad_0000[24]; //0x0000
	//    void* m_pOwner; //0x0018
	//    char pad_0020[16]; //0x0020
	//    int32_t m_iGlowType; //0x0030
	//    int32_t m_iGlowTeam; //0x0034
	//    int32_t m_nGlowRange; //0x0038
	//    int32_t m_nGlowRangeMin; //0x003C
	//    uint8_t red; //0x0040
	//    uint8_t green; //0x0041
	//    uint8_t blue; //0x0042
	//    uint8_t alpha; //0x0043
	//    bool m_bFlashing; //0x0044
	//    char pad_0045[3]; //0x0045
	//    float m_flGlowTime; //0x0048
	//    float m_flGlowStartTime; //0x004C
	//    bool m_bEligibleForScreenHighlight; //0x0050
	//    bool m_bGlowing; //0x0051
	//}; //Size: 0x0080

	MEM_PAD(0x18);
	C_CSPlayerPawn* m_pOwner; //0x0018
};

class C_BaseModelEntity : public C_BaseEntity
{
public:
	void SetModel(const char* szName)
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnSetModel != nullptr);
#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnSetModel, this, szName);
	}

	void SetAbsOrigin(const Vector& vecOrigin)
	{
		using fnSetAbsOrigin = void(__fastcall*)(C_BaseEntity*, Vector);
		static const auto oSetAbsOrigin = reinterpret_cast<fnSetAbsOrigin>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 08 57 48 83 EC 40 48 8B 01 48 8B FA")));

#ifdef CS_PARANOID
		CS_ASSERT(oSetAbsOrigin != nullptr);
#endif

		return oSetAbsOrigin(this, vecOrigin);
	}
public:
	SCHEMA_ADD_FIELD(Vector, m_vecViewOffset, "C_BaseModelEntity->m_vecViewOffset");

	SCHEMA_ADD_FIELD(GameTime_t, m_flCreateTime, "C_BaseModelEntity->m_flCreateTime");
	SCHEMA_ADD_FIELD(GameTick_t, m_nCreationTick, "C_BaseModelEntity->m_nCreationTick");

	SCHEMA_ADD_FIELD(CGlowProperty, m_Glow, "C_BaseModelEntity->m_Glow");

	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hOldMoveParent, "C_BaseModelEntity->m_hOldMoveParent");
};

struct PhysicsRagdollPose_t
{
	MEM_PAD(0x48);
	CHandle<C_BaseEntity> m_hOwner; // 0x48
	MEM_PAD(0x20);
	bool m_bDirty; // 0x68
};

class CBaseAnimGraph : public C_BaseModelEntity
{
public:
	SCHEMA_ADD_FIELD(bool, m_bInitiallyPopulateInterpHistory, "CBaseAnimGraph->m_bInitiallyPopulateInterpHistory");
	SCHEMA_ADD_FIELD(bool, m_bShouldAnimateDuringGameplayPause, "CBaseAnimGraph->m_bShouldAnimateDuringGameplayPause");
	SCHEMA_ADD_FIELD(bool, m_bSuppressAnimEventSounds, "CBaseAnimGraph->m_bSuppressAnimEventSounds");
	SCHEMA_ADD_FIELD(bool, m_bAnimGraphUpdateEnabled, "CBaseAnimGraph->m_bAnimGraphUpdateEnabled");
	SCHEMA_ADD_FIELD(bool, m_bBuiltRagdoll, "CBaseAnimGraph->m_bBuiltRagdoll");
	SCHEMA_ADD_FIELD(bool, m_bClientRagdoll, "CBaseAnimGraph->m_bClientRagdoll");
	SCHEMA_ADD_FIELD(bool, m_bHasAnimatedMaterialAttributes, "CBaseAnimGraph->m_bHasAnimatedMaterialAttributes");

	SCHEMA_ADD_FIELD(std::float_t, m_flMaxSlopeDistance, "CBaseAnimGraph->m_flMaxSlopeDistance");

	SCHEMA_ADD_FIELD(std::int32_t, m_nForceBone, "CBaseAnimGraph->m_nForceBone");

	SCHEMA_ADD_FIELD(Vector, m_vecForce, "CBaseAnimGraph->m_vecForce");
	SCHEMA_ADD_FIELD(Vector, m_vLastSlopeCheckPos, "CBaseAnimGraph->m_vLastSlopeCheckPos");

	SCHEMA_ADD_FIELD(CBaseAnimGraph*, m_pClientsideRagdoll, "CBaseAnimGraph->m_pClientsideRagdoll");
	SCHEMA_ADD_FIELD(PhysicsRagdollPose_t*, m_pRagdollPose, "CBaseAnimGraph->m_pRagdollPose");
};

class C_BaseFlex : public CBaseAnimGraph
{

};

class C_BaseToggle : public C_BaseModelEntity
{
public:
};

class C_BaseTrigger : public C_BaseToggle
{
	SCHEMA_ADD_FIELD(bool, m_bDisabled, "C_BaseTrigger->m_bDisabled");
	SCHEMA_ADD_FIELD(bool, m_bClientSidePredicted, "C_BaseTrigger->m_bClientSidePredicted");
};

class C_PostProcessingVolume : public C_BaseTrigger
{
public:
	SCHEMA_ADD_FIELD(bool, m_bExposureControl, "C_PostProcessingVolume->m_bExposureControl");

	SCHEMA_ADD_FIELD(std::float_t, m_flFadeDuration, "C_PostProcessingVolume->m_flFadeDuration");
	SCHEMA_ADD_FIELD(std::float_t, m_flMinExposure, "C_PostProcessingVolume->m_flMinExposure");
	SCHEMA_ADD_FIELD(std::float_t, m_flMaxExposure, "C_PostProcessingVolume->m_flMaxExposure");

	SCHEMA_ADD_FIELD(std::float_t, m_flExposureFadeSpeedUp, "C_PostProcessingVolume->m_flExposureFadeSpeedUp");
	SCHEMA_ADD_FIELD(std::float_t, m_flExposureFadeSpeedDown, "C_PostProcessingVolume->m_flExposureFadeSpeedDown");
};

class CBaseProp : public CBaseAnimGraph
{
public:
	SCHEMA_ADD_FIELD(bool, m_bModelOverrodeBlockLOS, "CBaseProp->m_bModelOverrodeBlockLOS");
	SCHEMA_ADD_FIELD(bool, m_bConformToCollisionBounds, "CBaseProp->m_bConformToCollisionBounds");

	SCHEMA_ADD_FIELD(std::int32_t, m_iShapeType, "CBaseProp->m_iShapeType");

	SCHEMA_ADD_FIELD(Matrix3x4_t, m_mPreferredCatchTransform, "CBaseProp->m_mPreferredCatchTransform");
};

class C_BreakableProp : public CBaseProp
{
public:
};

class C_DynamicProp : public C_BreakableProp
{
public:
	SCHEMA_ADD_FIELD(bool, m_bUseHitboxesForRenderBox, "C_BreakableProp->m_bUseHitboxesForRenderBox");
	SCHEMA_ADD_FIELD(bool, m_bUseAnimGraph, "C_BreakableProp->m_bUseAnimGraph");
	SCHEMA_ADD_FIELD(bool, m_bAnimateOnServer, "C_BreakableProp->m_bAnimateOnServer");
	SCHEMA_ADD_FIELD(bool, m_bRandomizeCycle, "C_BreakableProp->m_bRandomizeCycle");
	SCHEMA_ADD_FIELD(bool, m_bStartDisabled, "C_BreakableProp->m_bStartDisabled");
	SCHEMA_ADD_FIELD(bool, m_bScriptedMovement, "C_BreakableProp->m_bScriptedMovement");
	SCHEMA_ADD_FIELD(bool, m_bFiredStartEndOutput, "C_BreakableProp->m_bFiredStartEndOutput");
	SCHEMA_ADD_FIELD(bool, m_bForceNpcExclude, "C_BreakableProp->m_bForceNpcExclude");
	SCHEMA_ADD_FIELD(bool, m_bCreateNonSolid, "C_BreakableProp->m_bCreateNonSolid");
	SCHEMA_ADD_FIELD(bool, m_bIsOverrideProp, "C_BreakableProp->m_bIsOverrideProp");

	SCHEMA_ADD_FIELD(std::int32_t, m_iInitialGlowState, "C_BreakableProp->m_iInitialGlowState");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowRange, "C_BreakableProp->m_nGlowRange");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowRangeMin, "C_BreakableProp->m_nGlowRangeMin");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowTeam, "C_BreakableProp->m_nGlowTeam");
	SCHEMA_ADD_FIELD(std::int32_t, m_iCachedFrameCount, "C_BreakableProp->m_iCachedFrameCount");

	SCHEMA_ADD_FIELD(Vector, m_vecCachedRenderMins, "C_BreakableProp->m_vecCachedRenderMins");
	SCHEMA_ADD_FIELD(Vector, m_vecCachedRenderMaxs, "C_BreakableProp->m_vecCachedRenderMaxs");

	SCHEMA_ADD_FIELD(Color, m_glowColor, "C_BreakableProp->m_glowColor");
};

class C_Chicken : public C_DynamicProp
{
public:
	//constexpr std::ptrdiff_t m_hHolidayHatAddon = 0x10F0; // CHandle<CBaseAnimGraph>
	//constexpr std::ptrdiff_t m_jumpedThisFrame = 0x10F4; // bool
	//constexpr std::ptrdiff_t m_leader = 0x10F8; // CHandle<C_CSPlayerPawnBase>
	//constexpr std::ptrdiff_t m_AttributeManager = 0x1100; // C_AttributeContainer
	//constexpr std::ptrdiff_t m_OriginalOwnerXuidLow = 0x15A8; // uint32_t
	//constexpr std::ptrdiff_t m_OriginalOwnerXuidHigh = 0x15AC; // uint32_t
	//constexpr std::ptrdiff_t m_bAttributesInitialized = 0x15B0; // bool
	//constexpr std::ptrdiff_t m_hWaterWakeParticles = 0x15B4; // ParticleIndex_t
};

class C_RagdollProp : public CBaseAnimGraph
{
	SCHEMA_ADD_FIELD(std::float_t, m_flBlendWeight, "C_RagdollProp->m_flBlendWeight");
	SCHEMA_ADD_FIELD(std::float_t, m_flBlendWeightCurrent, "C_RagdollProp->m_flBlendWeightCurrent");

	SCHEMA_ADD_FIELD(CUtlVector<std::int32_t>, m_parentPhysicsBoneIndices, "C_RagdollProp->m_parentPhysicsBoneIndices");
	SCHEMA_ADD_FIELD(CUtlVector<std::int32_t>, m_worldSpaceBoneComputationOrder, "C_RagdollProp->m_worldSpaceBoneComputationOrder");

	SCHEMA_ADD_FIELD(C_NetworkUtlVectorBase<Vector>, m_ragPos, "C_RagdollProp->m_ragPos");
	SCHEMA_ADD_FIELD(C_NetworkUtlVectorBase<QAngle>, m_ragAngles, "C_RagdollProp->m_ragAngles");

	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hRagdollSource, "C_RagdollProp->m_hRagdollSource");
};

class C_ClientRagdoll : public CBaseAnimGraph
{
public:
	//constexpr std::ptrdiff_t m_bFadeOut = 0xE80; // bool
	//constexpr std::ptrdiff_t m_bImportant = 0xE81; // bool
	//constexpr std::ptrdiff_t m_flEffectTime = 0xE84; // GameTime_t
	//constexpr std::ptrdiff_t m_gibDespawnTime = 0xE88; // GameTime_t
	//constexpr std::ptrdiff_t m_iCurrentFriction = 0xE8C; // int32_t
	//constexpr std::ptrdiff_t m_iMinFriction = 0xE90; // int32_t
	//constexpr std::ptrdiff_t m_iMaxFriction = 0xE94; // int32_t
	//constexpr std::ptrdiff_t m_iFrictionAnimState = 0xE98; // int32_t
	//constexpr std::ptrdiff_t m_bReleaseRagdoll = 0xE9C; // bool
	//constexpr std::ptrdiff_t m_iEyeAttachment = 0xE9D; // AttachmentHandle_t
	//constexpr std::ptrdiff_t m_bFadingOut = 0xE9E; // bool
	//constexpr std::ptrdiff_t m_flScaleEnd = 0xEA0; // float[10]
	//constexpr std::ptrdiff_t m_flScaleTimeStart = 0xEC8; // GameTime_t[10]
	//constexpr std::ptrdiff_t m_flScaleTimeEnd = 0xEF0; // GameTime_t[10]
};

class CCSPlayer_ItemServices
{
public:
	SCHEMA_ADD_FIELD(bool, m_bHasDefuser, "CCSPlayer_ItemServices->m_bHasDefuser");
	SCHEMA_ADD_FIELD(bool, m_bHasHelmet, "CCSPlayer_ItemServices->m_bHasHelmet");
	//SCHEMA_ADD_FIELD(bool, m_bHasHeavyArmor, "CCSPlayer_ItemServices->m_bHasHeavyArmor");
};

class CPlayer_ObserverServices
{
public:
	SCHEMA_ADD_FIELD(std::uint8_t, m_iObserverMode, "CPlayer_ObserverServices->m_iObserverMode");

	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hObserverTarget, "CPlayer_ObserverServices->m_hObserverTarget");
};

class CCSObserver_ObserverServices : public CPlayer_ObserverServices
{
public:
};

class CPlayer_WeaponServices
{
public:
	[[nodiscard]] Vector GetShootPosition()
	{
		Vector vecTemp = Vector(0.0f, 0.0f, 0.0f);
		Memory::CallVFunc<void, 26>(this, &vecTemp);
		return vecTemp;
	}
public:
	SCHEMA_ADD_FIELD(CHandle<C_CSWeaponBase>, m_hActiveWeapon, "CPlayer_WeaponServices->m_hActiveWeapon");
	SCHEMA_ADD_FIELD(C_NetworkUtlVectorBase<CHandle<C_BasePlayerWeapon>>, m_hMyWeapons, "CPlayer_WeaponServices->m_hMyWeapons");
};

class C_EconWearable;
class C_BaseCombatCharacter : public C_BaseFlex
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_bloodColor, "C_BaseCombatCharacter->m_bloodColor");
};

class CPlayer_WeaponServices;
class CPlayer_CameraServices
{
public:
	SCHEMA_ADD_FIELD(CBaseHandle, m_hActivePostProcessingVolume, "CPlayer_CameraServices->m_hActivePostProcessingVolume");
	SCHEMA_ADD_FIELD(Vector, m_vecCsViewPunchAngle, "CPlayer_CameraServices->m_vecCsViewPunchAngle");
	SCHEMA_ADD_FIELD(Vector, m_vClientScopeInaccuracy, "CCSPlayer_CameraServices->m_vClientScopeInaccuracy");

};

class CBasePlayerController;
class CCSPlayer_MovementServices;
class C_CSWeaponBase;
class C_BasePlayerPawn : public C_BaseCombatCharacter
{
public:
	SCHEMA_ADD_FIELD(std::float_t, m_flOldSimulationTime, "C_BasePlayerPawn->m_flOldSimulationTime");

	SCHEMA_ADD_FIELD(CPlayer_WeaponServices*, m_pWeaponServices, "C_BasePlayerPawn->m_pWeaponServices");
	SCHEMA_ADD_FIELD(CCSPlayer_ItemServices*, m_pItemServices, "C_BasePlayerPawn->m_pItemServices");
	SCHEMA_ADD_FIELD(CCSObserver_ObserverServices*, m_pObserverServices, "C_BasePlayerPawn->m_pObserverServices");
	SCHEMA_ADD_FIELD(CPlayer_CameraServices*, m_pCameraServices, "C_BasePlayerPawn->m_pCameraServices");
	SCHEMA_ADD_FIELD(CCSPlayer_MovementServices*, m_pMovementServices, "C_BasePlayerPawn->m_pMovementServices");
	SCHEMA_ADD_FIELD(C_CSWeaponBase*, m_pClippingWeapon, "C_CSPlayerPawn->m_pClippingWeapon");
	SCHEMA_ADD_FIELD(Vector, v_angle, "C_BasePlayerPawn->v_angle");

	SCHEMA_ADD_FIELD(CHandle<CBasePlayerController>, m_hController, "C_BasePlayerPawn->m_hController");

	// client.dll @ 48 89 74 24 10 57 48 83 EC 30 48 8B F9 48 8B
	[[nodiscard]] Vector GetEyePosition(bool bLocal = false)
	{
		Vector vecEyePosition = Vector(0, 0, 0);

		if (bLocal)
		{
			if (!Globals::m_NetClientInfo.m_pLocalData)
				return vecEyePosition;

			vecEyePosition = Globals::m_NetClientInfo.m_pLocalData->m_vecShootPostion;
		}
		else
		{
			Memory::CallVFunc<void, 175U>(this, &vecEyePosition);
		}

		return vecEyePosition;
	}
};

class CAnimGraphNetworkedVariables;
class CAnimationGraphInstance
{
public:
	MEM_PAD(0x2E0); //0x0000
	CAnimGraphNetworkedVariables* m_pAnimGraphNetworkedVariables; //0x02E0 
};

class C_BaseViewModel : public CBaseAnimGraph
{
public:
	SCHEMA_ADD_FIELD(CHandle<C_BasePlayerWeapon>, m_hWeapon, "C_BaseViewModel->m_hWeapon");
};

enum EMaterialMagicNumber : std::uint32_t
{
	MATERIAL_MAGIC_NUMBER_GLOVE = 0xF143B82A,
};

struct MaterialRecord_t
{
	std::uint32_t m_uUnknown;
	EMaterialMagicNumber m_eIdentifier;
	std::uint32_t m_uHandle;
	MEM_PAD(0x4);
	std::uint32_t m_uTypeIndex;
};

struct MaterialInfo_t
{
	MaterialRecord_t* m_pMaterialRecords;
	std::uint32_t m_nCount;
};

class C_CSGOViewModel : public C_BaseViewModel
{
public:
	MEM_PAD(0xD08); //0x0000
	CAnimationGraphInstance* m_pAnimationGraphInstance; //0x0D08

	SCHEMA_ADD_PFIELD_OFFSET(MaterialInfo_t, m_MaterialInfo, "C_CSGOViewModel->m_bNeedToQueueHighResComposite", 0x8);
};

class CCSPlayer_ViewModelServices
{
public:
	SCHEMA_ADD_FIELD(CHandle<C_CSGOViewModel>[3], m_hViewModel, "CCSPlayer_ViewModelServices->m_hViewModel");
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
	//SCHEMA_ADD_FIELD(CCSPlayer_ViewModelServices*, m_pViewModelServices, "C_CSPlayerPawnBase->m_pViewModelServices"); remover from game

	SCHEMA_ADD_FIELD(std::float_t, m_flLowerBodyYawTarget, "C_CSPlayerPawnBase->m_flLowerBodyYawTarget");
	SCHEMA_ADD_FIELD_OFFSET(std::float_t, m_flFlashAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha", -0x8);
	SCHEMA_ADD_FIELD(std::float_t, m_flFlashMaxAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha");
	SCHEMA_ADD_FIELD(std::float_t, m_flFlashDuration, "C_CSPlayerPawnBase->m_flFlashDuration");

	SCHEMA_ADD_FIELD(GameTime_t, m_flLastSpawnTimeIndex, "C_CSPlayerPawnBase->m_flLastSpawnTimeIndex");

	SCHEMA_ADD_FIELD(int, m_nSurvivalTeam, "C_CSPlayerPawnBase->m_nSurvivalTeam"); // danger zone

	SCHEMA_ADD_FIELD(Vector, m_vLastSmokeOverlayColor, "C_CSPlayerPawnBase->m_vLastSmokeOverlayColor");

	SCHEMA_ADD_FIELD(QAngle, m_angEyeAngles, "C_CSPlayerPawnBase->m_angEyeAngles");
public:
	bool IsPlayerOutOfView()
	{
		return Memory::CallVFunc<bool, 296U>(this);
	}
};

class C_CSObserverPawn : public C_CSPlayerPawnBase
{
public:
};

class C_EconItemView;
class CCSPlayer_BulletServices;


class CCSPlayer_HostageServices {
public:
	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hCarriedHostage, "CCSPlayer_HostageServices->m_hCarriedHostage");
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
	[[nodiscard]] bool IsEnemy(C_CSPlayerPawn* pOther);
	[[nodiscard]] bool IsVisible(C_CSPlayerPawn* pPawn, Vector vecPoint, bool bSmokeCheck = true, float flMaxDensity = 1.0f);
	[[nodiscard]] int GetAssociatedTeam();

	[[nodiscard]] std::uint16_t GetCollisionMask();
	[[nodiscard]] std::uint32_t GetOwnerHandle();
	[[nodiscard]] std::uint32_t GetEntityHandle();
	[[nodiscard]] void CreateShockEffect(const Color& col = Color(255, 0, 0, 255));
	[[nodiscard]] void ApplyRagdollGravity(float gravityScale);
	[[nodiscard]] void LaunchRagdoll(float force);
	[[nodiscard]] bool IsArmored(int nHitGroup);
	OFFSET(float, m_flMaxPlayerMovementSpeed, 0x1254);

	void RunPreThink() {
		Memory::CallVFunc<void, 322>(this);
	}

	void PhysicsRunThink() {
		using fn_physics_run_think = void(__fastcall*)(void*);
		static fn_physics_run_think fn = (fn_physics_run_think)(Memory::FindPattern(CLIENT_DLL, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 8B 81 ? ? ? ? 48 8B F9"));
		fn(this);
	}

	void RunPostThink() {
		Memory::CallVFunc<void, 333>(this);
	}

	// Note: Outdated
	Vector GetWorldSpaceCenter()
	{
		Vector vecResult;
		Memory::CallVFunc<Vector, 77U>(this, &vecResult);
		return vecResult;
	}

	auto GetLocalActiveWeapon() -> C_CSWeaponBase*
	{
		CPlayer_WeaponServices* pWeaponService = m_pWeaponServices();

		if (pWeaponService)
		{
			C_CSWeaponBase* pLocalWeapon = pWeaponService->m_hActiveWeapon().Get();

			if (pLocalWeapon)
				return pLocalWeapon;
		}

		return nullptr;
	}

	//std::vector<C_CS2HudModelWeapon*> GetViewModels()
	//{
	//	auto* pC_CS2HudModelArms = m_hHudModelArms().Get();

	//	std::vector<C_CS2HudModelWeapon*> vecViewModels;

	//	if (pC_CS2HudModelArms)
	//	{
	//		auto* pCGameSceneNode = pC_CS2HudModelArms->m_pGameSceneNode();

	//		if (pCGameSceneNode)
	//		{
	//			for (auto* pChild = pCGameSceneNode->m_pChild(); pChild; pChild = pChild->m_pNextSibling())
	//			{
	//				auto* pOwner = reinterpret_cast<C_BaseEntity*>(pChild->m_pOwner());

	//				if (pOwner && pOwner->IsCS2HudModelWeapon())
	//				{
	//					vecViewModels.push_back(reinterpret_cast<C_CS2HudModelWeapon*>(pOwner));
	//				}
	//			}
	//		}
	//	}

	//	return vecViewModels;
	//}

	//C_CS2HudModelWeapon* GetViewModel()
	//{
	//	auto* pC_CSWeaponBaseGun = GetLocalActiveWeapon();

	//	if (pC_CSWeaponBaseGun)
	//	{
	//		const auto& ViewModels = GetViewModels();

	//		if (!ViewModels.empty())
	//		{
	//			for (auto pViewModel : ViewModels)
	//			{
	//				auto* pOwnerEntity = pViewModel->m_hOwnerEntity().Get();

	//				if ((uintptr_t)pOwnerEntity == (uintptr_t)pC_CSWeaponBaseGun)
	//					return pViewModel;
	//			}
	//		}
	//	}

	//	return nullptr;
	//}

	float GetSomeTiming(int a1, int a2) {
		static auto fnSomeTimingFromPawn = reinterpret_cast<float(__fastcall*)(void*, int, int)>((Memory::FindPattern(CLIENT_DLL, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 49 63 D8 48 8B F1")));
		return fnSomeTimingFromPawn(this, a1, a2);
	}

	void SetAbsOrigin(const Vector& vecOrigin)
	{
		using fnSetAbsOrigin = void(__fastcall*)(C_BaseEntity*, Vector);
		static const auto oSetAbsOrigin = reinterpret_cast<fnSetAbsOrigin>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 83 EC ? ? ? ? 48 8B FA 48 8B D9 FF 90 ? ? ? ? 84 C0 0F 85")));

#ifdef CS_PARANOID
		CS_ASSERT(oSetAbsOrigin != nullptr);
#endif

		return oSetAbsOrigin(this, vecOrigin);
	}

public:
	SCHEMA_ADD_FIELD(bool, m_bNeedToReApplyGloves, "C_CSPlayerPawn->m_bNeedToReApplyGloves");
	SCHEMA_ADD_FIELD(bool, m_bHasFemaleVoice, "C_CSPlayerPawn->m_bHasFemaleVoice");
	SCHEMA_ADD_FIELD(bool, m_bIsScoped, "C_CSPlayerPawn->m_bIsScoped");
	SCHEMA_ADD_FIELD(bool, m_bResumeZoom, "C_CSPlayerPawn->m_bResumeZoom");
	SCHEMA_ADD_FIELD(bool, m_bIsWalking, "C_CSPlayerPawn->m_bIsWalking");
	SCHEMA_ADD_FIELD(bool, m_bIsDefusing, "C_CSPlayerPawn->m_bIsDefusing");
	SCHEMA_ADD_FIELD(bool, m_bIsGrabbingHostage, "C_CSPlayerPawn->m_bIsGrabbingHostage");
	SCHEMA_ADD_FIELD(bool, m_bOnGroundLastTick, "C_CSPlayerPawn->m_bOnGroundLastTick");
	SCHEMA_ADD_FIELD(bool, m_bGunGameImmunity, "C_CSPlayerPawn->m_bGunGameImmunity");
	SCHEMA_ADD_FIELD(std::int32_t, m_iShotsFired, "C_CSPlayerPawn->m_iShotsFired");
	SCHEMA_ADD_FIELD(std::int32_t, m_ArmorValue, "C_CSPlayerPawn->m_ArmorValue");
	SCHEMA_ADD_FIELD(float, m_flVelocityModifier, "C_CSPlayerPawn->m_flVelocityModifier");
	//SCHEMA_ADD_FIELD(CHandle<C_CS2HudModelArms>, m_hHudModelArms, "C_CSPlayerPawn->m_hHudModelArms");

	SCHEMA_ADD_FIELD(GameTime_t, m_flHealthShotBoostExpirationTime, "C_CSPlayerPawn->m_flHealthShotBoostExpirationTime");

	SCHEMA_ADD_FIELD(CUtlVector<QAngle>, m_aimPunchCache, "C_CSPlayerPawn->m_aimPunchCache");

	SCHEMA_ADD_PFIELD(C_EconItemView, m_EconGloves, "C_CSPlayerPawn->m_EconGloves");

	SCHEMA_ADD_FIELD(CCSPlayer_BulletServices*, m_pBulletServices, "C_CSPlayerPawn->m_pBulletServices");
	SCHEMA_ADD_FIELD(CCSPlayer_HostageServices*, m_pHostageServices, "C_CSPlayerPawn->m_pHostageServices")

	QAngle GetRemovedAimPunch()
	{
		QAngle aimPunch = {};
		static const auto getAimPunchfn = reinterpret_cast<void(__fastcall*)(void*, QAngle*, bool)>(Memory::GetAbsoluteAddress(Memory::FindPattern(L"client.dll", "E8 ?? ?? ?? ?? 4C 8B C0 48 8D 55 ?? 48 8B CB E8 ?? ?? ?? ?? 48 8D 0D"), 1));
		getAimPunchfn(this, &aimPunch, true);
		return aimPunch;
	}

	struct WeaponData {
		int weaponId;      // offset 0
		float weaponTime;  // offset 4
	};

};

class CUserCmdManager
{
public:
	CUserCmd m_arrCommands[150];
	CUserCmd m_arrGlobalCmd;
	int32_t nSequence;
	char pad_59AC[4];
	double m_dRealTime;
	char pad_59B8[2080];

	CUserCmd* GetCmd() {
		return &m_arrCommands[nSequence & 150];
	}

	CUserCmd* GetCommandBySequence(int nSequence)
	{
		return &m_arrCommands[nSequence % 150];
	}
};


class CBasePlayerController : public C_BaseModelEntity
{
public:
	SCHEMA_ADD_FIELD(bool, m_bIsLocalPlayerController, "CBasePlayerController->m_bIsLocalPlayerController");

	SCHEMA_ADD_FIELD(std::uint64_t, m_steamID, "CBasePlayerController->m_steamID");
	SCHEMA_ADD_FIELD(std::uint32_t, m_nFinalPredictedTick, "CBasePlayerController->m_nFinalPredictedTick");

	SCHEMA_ADD_FIELD(std::uint32_t, m_nTickBase, "CBasePlayerController->m_nTickBase");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iDesiredFOV, "CBasePlayerController->m_iDesiredFOV");

	SCHEMA_ADD_FIELD(CHandle<C_BasePlayerPawn>, m_hPawn, "CBasePlayerController->m_hPawn");

	SCHEMA_ADD_FIELD_OFFSET(CUserCmd*, GetCurrentCommand, "CBasePlayerController->m_steamID", -0x8);//broke

	CUserCmdManager* GetUserCmdManager() {
		static auto fn = reinterpret_cast<CUserCmdManager * (__fastcall*)(CBasePlayerController*)>(
			Memory::FindPattern(CLIENT_DLL, "41 56 41 57 48 83 EC ? 48 8D 54 24")
			);
		return fn(this);
	};

	float GetPlayerInterp() {
		static auto fnGetPlayerInterp = reinterpret_cast<float(__fastcall*)(CBasePlayerController*)>((Memory::FindPattern(CLIENT_DLL, "40 53 48 83 EC ? 48 8B D9 48 8B 0D ? ? ? ? 48 83 C1")));
		return fnGetPlayerInterp(this);
	}
};

class C_CSWeaponBaseGun;
class CCSPlayerController_InGameMoneyServices;
class CCSPlayerController : public CBasePlayerController
{
public:
	[[nodiscard]] static CCSPlayerController* GetLocalPlayerController();
	[[nodiscard]] C_CSWeaponBase* GetPlayerWeapon(C_CSPlayerPawn* pPlayer);
	[[nodiscard]] bool CanShoot(C_CSWeaponBaseGun* pBaseWeapon);
	[[nodiscard]] bool IsFiring(C_BasePlayerWeapon* pBaseWeapon);
	bool IsThrowingGrenade(C_CSWeaponBase* pBaseWeapon);
public:
	SCHEMA_ADD_FIELD(CCSPlayerController_InGameMoneyServices*, m_pInGameMoneyServices, "CCSPlayerController->m_pInGameMoneyServices")

	SCHEMA_ADD_FIELD(bool, m_bPawnHasDefuser, "CCSPlayerController->m_bPawnHasDefuser");
	SCHEMA_ADD_FIELD(bool, m_bPawnHasHelmet, "CCSPlayerController->m_bPawnHasHelmet");
	SCHEMA_ADD_FIELD(bool, m_bPawnIsAlive, "CCSPlayerController->m_bPawnIsAlive");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iPing, "CCSPlayerController->m_iPing");

	SCHEMA_ADD_FIELD(const char*, m_sSanitizedPlayerName, "CCSPlayerController->m_sSanitizedPlayerName");

	SCHEMA_ADD_FIELD(CHandle<C_CSPlayerPawn>, m_hPlayerPawn, "CCSPlayerController->m_hPlayerPawn");
	SCHEMA_ADD_FIELD(CHandle<C_CSObserverPawn>, m_hObserverPawn, "CCSPlayerController->m_hObserverPawn");

	void PhysicsRunThink() {
		static auto fn = reinterpret_cast<void* (__fastcall*)(void*)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 81 EC ? ? ? ? ? ? ? 48 8B F9 FF 90")));
		fn(this);
	}
};

class C_PlantedC4 : public CBaseAnimGraph
{
public:
	__forceinline float GetTimer(const float flServerTime)
	{
		return std::clamp(this->m_flC4Blow() - flServerTime, 0.0f, this->m_flTimerLength());
	}

	__forceinline float GetDefuseTimer(const float flServerTime)
	{
		return std::clamp(this->m_flDefuseCountDown() - flServerTime, 0.0f, this->m_flDefuseLength());
	}
public:
	SCHEMA_ADD_FIELD(bool, m_bBombTicking, "C_PlantedC4->m_bBombTicking");
	SCHEMA_ADD_FIELD(bool, m_bCannotBeDefused, "C_PlantedC4->m_bCannotBeDefused");
	SCHEMA_ADD_FIELD(bool, m_bBeingDefused, "C_PlantedC4->m_bBeingDefused");

	SCHEMA_ADD_FIELD(std::int32_t, m_nBombSite, "C_PlantedC4->m_nBombSite");

	SCHEMA_ADD_FIELD(std::float_t, m_flTimerLength, "C_PlantedC4->m_flTimerLength");
	SCHEMA_ADD_FIELD(std::float_t, m_flDefuseLength, "C_PlantedC4->m_flDefuseLength");

	SCHEMA_ADD_FIELD(GameTime_t, m_flC4Blow, "C_PlantedC4->m_flC4Blow");
	SCHEMA_ADD_FIELD(GameTime_t, m_flDefuseCountDown, "C_PlantedC4->m_flDefuseCountDown");

	SCHEMA_ADD_FIELD(CHandle<C_CSPlayerPawn>, m_hBombDefuser, "C_PlantedC4->m_hBombDefuser");
};

class CAttributeManager
{
public:
};

class IEconItemInterface
{
public:
};

enum EStickerAttributeType : int
{
	STICKER_ATTRIBUTE_INDEX,
	STICKER_ATTRIBUTE_WEAR,
	STICKER_ATTRIBUTE_SCALE,
	STICKER_ATTRIBUTE_ROTATION
};

class C_EconItemView : public IEconItemInterface
{
public:
	CEconItem* GetSOCData()
	{
		CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
		if (!pInventory)
			return nullptr;

		return pInventory->GetSOCDataForItem(m_iItemID());
	}

	[[nodiscard]] int GetCustomPaintKitIndex()
	{
		return Memory::CallVFunc<int, 2U>(this);
	}

	[[nodiscard]] int GetCustomPaintKitSeed()
	{
		return Memory::CallVFunc<int, 3U>(this);
	}

	[[nodiscard]] float GetCustomPaintKitWear(float flWearDefault = 0.0f)
	{
		return Memory::CallVFunc<float, 4U>(this, flWearDefault);
	}

	[[nodiscard]] int GetStickerAttributeBySlotIndexFloat(int nSlot, EStickerAttributeType eStickerAttribute, int nDefinitionIndex)
	{
		return Memory::CallVFunc<int, 6U>(this, nSlot, eStickerAttribute, nDefinitionIndex);
	}

	[[nodiscard]] CEconItemDefinition* GetStaticData()
	{
		return Memory::CallVFunc<CEconItemDefinition*, 13U>(this);
	}
public:
	SCHEMA_ADD_FIELD(bool, m_bInitialized, "C_EconItemView->m_bInitialized");
	SCHEMA_ADD_FIELD(bool, m_bDisallowSOC, "C_EconItemView->m_bDisallowSOC");
	SCHEMA_ADD_FIELD(bool, m_bRestoreCustomMaterialAfterPrecache, "C_EconItemView->m_bRestoreCustomMaterialAfterPrecache");

	SCHEMA_ADD_FIELD(char[161], m_szCustomName, "C_EconItemView->m_szCustomName");
	SCHEMA_ADD_FIELD(char[161], m_szCustomNameOverride, "C_EconItemView->m_szCustomNameOverride");

	SCHEMA_ADD_FIELD(std::int32_t, m_iEntityQuality, "C_EconItemView->m_iEntityQuality");
	SCHEMA_ADD_FIELD(std::int32_t, m_iQualityOverride, "C_EconItemView->m_iQualityOverride");

	SCHEMA_ADD_FIELD(std::uint16_t, m_iItemDefinitionIndex, "C_EconItemView->m_iItemDefinitionIndex");

	SCHEMA_ADD_FIELD(std::uint32_t, m_iItemIDHigh, "C_EconItemView->m_iItemIDHigh");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iItemIDLow, "C_EconItemView->m_iItemIDLow");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iAccountID, "C_EconItemView->m_iAccountID");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iInventoryPosition, "C_EconItemView->m_iInventoryPosition");

	SCHEMA_ADD_FIELD(std::uint64_t, m_iItemID, "C_EconItemView->m_iItemID");
};

class C_AttributeContainer : public CAttributeManager
{
public:
	SCHEMA_ADD_PFIELD(C_EconItemView, m_Item, "C_AttributeContainer->m_Item");
};

class C_EconEntity : public C_BaseFlex
{
public:
	[[nodiscard]] std::uint64_t GetOriginalOwnerXuid()
	{
		return (static_cast<std::uint64_t>(m_OriginalOwnerXuidHigh()) << 32) | m_OriginalOwnerXuidLow();
	}
public:
	SCHEMA_ADD_PFIELD(C_AttributeContainer, m_AttributeManager, "C_EconEntity->m_AttributeManager");

	SCHEMA_ADD_FIELD(std::int32_t, m_nFallbackPaintKit, "C_EconEntity->m_nFallbackPaintKit");
	SCHEMA_ADD_FIELD(std::int32_t, m_nFallbackSeed, "C_EconEntity->m_nFallbackSeed");
	SCHEMA_ADD_FIELD(std::int32_t, m_nFallbackStatTrak, "C_EconEntity->m_nFallbackStatTrak");

	SCHEMA_ADD_FIELD(std::uint32_t, m_OriginalOwnerXuidLow, "C_EconEntity->m_OriginalOwnerXuidLow");
	SCHEMA_ADD_FIELD(std::uint32_t, m_OriginalOwnerXuidHigh, "C_EconEntity->m_OriginalOwnerXuidHigh");

	SCHEMA_ADD_FIELD(std::float_t, m_flFallbackWear, "C_EconEntity->m_flFallbackWear");
};

class CBasePlayerWeaponVData : public CEntitySubClassVDataBase
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_iMaxClip1, "CBasePlayerWeaponVData->m_iMaxClip1");
};

class CCSWeaponBaseVData : public CBasePlayerWeaponVData
{
public:
	SCHEMA_ADD_FIELD(EWeaponType, m_WeaponType, "CCSWeaponBaseVData->m_WeaponType");
	SCHEMA_ADD_FIELD(EWeaponSilencerType, m_eSilencerType, "CCSWeaponBaseVData->m_eSilencerType");

	SCHEMA_ADD_FIELD(bool, m_bIsFullAuto, "CCSWeaponBaseVData->m_bIsFullAuto");
	SCHEMA_ADD_FIELD(bool, m_bIsRevolver, "CCSWeaponBaseVData->m_bIsRevolver");
	SCHEMA_ADD_FIELD(float, m_flSpread, "CCSWeaponBaseVData->m_flSpread");

	SCHEMA_ADD_FIELD(const char*, m_szName, "CCSWeaponBaseVData->m_szName");
	SCHEMA_ADD_FIELD(std::int32_t, m_nZoomLevels, "CCSWeaponBaseVData->m_nZoomLevels");
	SCHEMA_ADD_FIELD(std::int32_t, m_nDamage, "CCSWeaponBaseVData->m_nDamage");
	SCHEMA_ADD_FIELD(int32_t, m_nNumBullets, "CCSWeaponBaseVData->m_nNumBullets");

	SCHEMA_ADD_FIELD(std::float_t, m_flRange, "CCSWeaponBaseVData->m_flRange");
	SCHEMA_ADD_FIELD(std::float_t, m_flRangeModifier, "CCSWeaponBaseVData->m_flRangeModifier");
	SCHEMA_ADD_FIELD(std::float_t, m_flHeadshotMultiplier, "CCSWeaponBaseVData->m_flHeadshotMultiplier");
	SCHEMA_ADD_FIELD(std::float_t, m_flArmorRatio, "CCSWeaponBaseVData->m_flArmorRatio");
	SCHEMA_ADD_FIELD(std::float_t, m_flPenetration, "CCSWeaponBaseVData->m_flPenetration");
	SCHEMA_ADD_FIELD(std::float_t, m_flThrowVelocity, "CCSWeaponBaseVData->m_flThrowVelocity");

	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flCycleTime, "CCSWeaponBaseVData->m_flCycleTime");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flMaxSpeed, "CCSWeaponBaseVData->m_flMaxSpeed");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flInaccuracyStand, "CCSWeaponBaseVData->m_flInaccuracyStand");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flInaccuracyCrouch, "CCSWeaponBaseVData->m_flInaccuracyCrouch");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flInaccuracyJump, "CCSWeaponBaseVData->m_flInaccuracyJump");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flInaccuracyMove, "CCSWeaponBaseVData->m_flInaccuracyMove");
	SCHEMA_ADD_FIELD(float, m_flInaccuracyReload, "CCSWeaponBaseVData->m_flInaccuracyReload");

	const bool IsGun() noexcept
	{
		switch (this->m_WeaponType())
		{
		case EWeaponType::WEAPONTYPE_PISTOL:
		case EWeaponType::WEAPONTYPE_SUBMACHINEGUN:
		case EWeaponType::WEAPONTYPE_RIFLE:
		case EWeaponType::WEAPONTYPE_SHOTGUN:
		case EWeaponType::WEAPONTYPE_SNIPER_RIFLE:
		case EWeaponType::WEAPONTYPE_MACHINEGUN:
			return true;
		default:
			return false;
		}
	}

	[[nodiscard]] const bool IsKnifeOrTaser() noexcept
	{
		if (CRT::StringCompare(this->m_szName(), X("weapon_knife")) == 0 || CRT::StringCompare(this->m_szName(), X("weapon_taser")) == 0)
			return true;
		else
			return false;
	}

	[[nodiscard]] const bool IsTaser() noexcept
	{
		if (CRT::StringCompare(this->m_szName(), X("weapon_taser")) == 0)
			return true;
		else
			return false;
	}

	void UpdateWeaponData()
	{
		Memory::CallVFunc<void*, 183U>(this);
	}
};

class C_BasePlayerWeapon : public C_EconEntity
{
public:
	CCSWeaponBaseVData* GetWeaponBaseVData() noexcept
	{
		return this->m_pVDataBase()->As<CCSWeaponBaseVData>();
	}

	SCHEMA_ADD_FIELD(std::int32_t, m_iClip1, "C_BasePlayerWeapon->m_iClip1");
	SCHEMA_ADD_FIELD(std::int32_t, m_pReserveAmmo, "C_BasePlayerWeapon->m_pReserveAmmo");

	SCHEMA_ADD_FIELD(GameTick_t, m_nNextPrimaryAttackTick, "C_BasePlayerWeapon->m_nNextPrimaryAttackTick");
	SCHEMA_ADD_FIELD(GameTick_t, m_nNextSecondaryAttackTick, "C_BasePlayerWeapon->m_nNextSecondaryAttackTick");

	SCHEMA_ADD_FIELD(std::float_t, m_flNextPrimaryAttackTickRatio, "C_BasePlayerWeapon->m_flNextPrimaryAttackTickRatio");
};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices
{
public:
	SCHEMA_ADD_FIELD(bool, m_bIsLookingAtWeapon, "CCSPlayer_WeaponServices->m_bIsLookingAtWeapon");
	SCHEMA_ADD_FIELD(bool, m_bIsHoldingLookAtWeapon, "CCSPlayer_WeaponServices->m_bIsHoldingLookAtWeapon");

	SCHEMA_ADD_FIELD(GameTime_t, m_flNextAttack, "CCSPlayer_WeaponServices->m_flNextAttack");
};

class CPlayer_MovementServices
{
public:
	//todo: create pattern for all
	void SetPredictionCommand(CUserCmd* pCmd)
	{
		Memory::CallVFunc<void, /*36U*/39>(this, pCmd);
	}

	void RunCommand(CUserCmd* pCmd)
	{
		using fn_RunCommand = void(__fastcall*)(void*, CUserCmd*);
		static fn_RunCommand RunCommand = (fn_RunCommand)(Memory::FindPattern(CLIENT_DLL, X("48 8B C4 48 81 EC C8 00 00 00 48 89 58 10")));//(xref: runcommand: % 04d, tick : % u)
		fn_RunCommand(this);
	}

	void ResetPredictionCommand()
	{
		Memory::CallVFunc<void, /*37U*/40>(this); 
	}

	//void SetPreditionCommand(CUserCmd* pCmd)
	//{
	//	Memory::CallVFunc<void, 36U>(this, pCmd);
	//}

	//void RunCommand(CUserCmd* pCmd)
	//{
	//	Memory::CallVFunc<void, 25U>(this, pCmd);
	//}

	//void ResetPredictionCommand()
	//{
	//	Memory::CallVFunc<void, 37U>(this);
	//}

	bool UnkVFunc()
	{
		return Memory::CallVFunc<bool, 39U>(this);
	}

	__int64 ProcessMovement(CUserCmd* pCmd)
	{
		static auto fn = reinterpret_cast<__int64(__fastcall*)(CPlayer_MovementServices*, CUserCmd*)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 48 8B 06 48 8B CE FF 90 ? ? ? ? 48 85 DB"), 1));
		return fn(this, pCmd);
	}

	void ParachuteAccelerate(Vector& vecOutVelo, float& flStamina, float& flFriction, Vector vecMoveImpulses, float flFrametime);
	void RunCommand_Rebuilt(CUserCmd* pCmd);
public:
	SCHEMA_ADD_FIELD(float, m_flMaxspeed, "CPlayer_MovementServices->m_flMaxspeed");
	SCHEMA_ADD_FIELD(Vector2D, m_vecLastMovementImpulses, "CPlayer_MovementServices->m_vecLastMovementImpulses");
};

// entries 12 | size 0x218
class CPlayer_MovementServices_Humanoid : public CPlayer_MovementServices
{
public:
	void PostThink()
	{
		Memory::CallVFunc<void, 5U>(this);
	}
	SCHEMA_ADD_FIELD(float, m_flSurfaceFriction, "CPlayer_MovementServices_Humanoid->m_flSurfaceFriction");
	SCHEMA_ADD_FIELD(bool, m_bDucking, "CPlayer_MovementServices_Humanoid->m_bDucking");
	SCHEMA_ADD_FIELD(bool, m_bDucked, "CPlayer_MovementServices_Humanoid->m_bDucked");
};

struct TouchList_t {
	Vector deltavelocity;
	//GameTrace_t trace;
	MEM_PAD(0x108);
};

class C_SubtickMove_
{
public:
	float m_flWhen;
	uint64_t m_uButton;

	union
	{
		bool m_bPressed;
		struct
		{
			float m_flAnalogForwardDelta;
			float m_flAnalogLeftDelta;
			float m_flAnalogPitchDelta;
			float m_flAnalogYawDelta;
		} m_AnalogMove;
	};
};

class CMoveData
{
public:
	uint8_t m_nFlags;
	int32_t m_nSomeEntIndex;
	Vector m_vAbsViewAngles;
	Vector m_vViewAngles;
	Vector m_vLastMovementImpulses;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;
	Vector m_vAbsVelocity;
	Vector m_vAngles;
	CUtlVector<C_SubtickMove_> m_SubtickMoves;
	CUtlVector<C_SubtickMove_> m_AttackSubtickMoves;
	bool m_bHasSubtickInputs;
	float m_flLatestSubtickFraction;
	CUtlVector<void*> m_TouchList;
	Vector m_vCollisionNormal;
	Vector m_vGroundNormal;
	Vector m_vAbsOrigin;
	int32_t m_nTickCount;
	int32_t m_nTargetTick;
	float m_flSubtickEndFraction;
	float m_flSubtickStartFraction;
	Vector m_vOutWishVelocity;
	Vector m_vOldAngles;
	float m_flMaxSpeed;
	float m_flClientMaxSpeed;
	float m_flFrictionDecel;
	bool m_bJumpedThisTick;
	bool m_bGameCodeMovedPlayer;
};

class CMoveData;
class CCSPlayer_MovementServices : public CPlayer_MovementServices_Humanoid
{
public:
	SCHEMA_ADD_OFFSET(C_CSPlayerPawn*, m_pPawn, 0x30);
	SCHEMA_ADD_OFFSET(CInButtonState&, m_ButtonState, 0x50);

	CMoveData* GetMoveData()
	{
		return Memory::CallVFunc<CMoveData*, 30>(this);
	}

	void UpdateButtonState(CUserCmd* cmd) {
		Memory::CallVFunc<void, 25>(this, cmd);
	}

	void CheckMovingGround(float time) {
		Memory::CallVFunc<void, 40>(this, time);
	}

	CMoveData* ResetMoveData() {
		return Memory::CallVFunc<CMoveData*, 31>(this);
	}

	void GetPlayerCollisionBound(BBox_t* bnds) {
		using fn = void* (__fastcall*)();
		static fn x = (fn)(Memory::FindPattern(CLIENT_DLL, "48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 48 FF A0 ? ? ? ? 48 8D 05"));

		char* bbox = {};

		if (this->m_bDucked()) {
			bbox = ((char*)x() + 36);
		}
		else {
			bbox = (char*)x() + 12;
		}

		bnds = (BBox_t*)bbox;
	}

	void ForceButtonsDown(__int64 a2)
	{
		using fnForceButtonsDown = void* (__fastcall*)(void*, __int64);
		static fnForceButtonsDown ForceButtonsDown_ = (fnForceButtonsDown)(Memory::FindPattern(CLIENT_DLL, "40 53 57 41 56 48 81 EC ? ? ? ? 48 83 79"));

		ForceButtonsDown_(this, a2);
	}

	void SetupMovementMoves(CMoveData* pMoveData)//broke
	{
		using fnSetupMovementMoves = void* (__fastcall*)(void*, CMoveData*);
		//static fnSetupMovementMoves SetupMovementMoves_ = ( fnSetupMovementMoves )( Memory::FindPattern( CLIENT_DLL, "48 89 5C 24 ? 57 48 83 EC ? 48 8B FA 48 8B D9 E8 ? ? ? ? 44 0F B6 C0" ) );
		static fnSetupMovementMoves SetupMovementMoves_ = (fnSetupMovementMoves)(Memory::FindPattern(CLIENT_DLL, "48 8B CB E8 ? ? ? ? 48 8B 5C 24 ? 48 8B 6C 24 ? 48 83 C4 30")); //it got inlined idk if it works

		SetupMovementMoves_(this, pMoveData);
	}
	
	bool ShouldUseViewanglesForSubtick(CUserCmd* a) {
		return Memory::CallVFunc<bool, 34>(this, a);
	}

	void SetupMoveNew(CUserCmd* cmd, CMoveData* move) {
		Memory::CallVFunc<void, 31>(this, cmd, move);
	}

	void ProcessImpacts(CUserCmd* cmd, CMoveData* move_data)
	{
		using fn_process_impacts = void* (__fastcall*)(void*, CUserCmd*, CMoveData*);
		static fn_process_impacts process_impacts_ = (fn_process_impacts)(Memory::FindPattern(CLIENT_DLL, "48 8B C4 53 56 41 55"));

		process_impacts_(this, cmd, move_data);
	}

	void QuantizeMovement(CMoveData* mov) {
		Memory::CallVFunc<void, 33>(this, mov);
	}

	void SetupMove(CUserCmd* pCmd, CMoveData* pMoveData);
	void FinishMove(CUserCmd* pCmd, CMoveData* pMoveData)
	{
		Memory::CallVFunc<void, 34>(this, pCmd, pMoveData);
	}
	bool IsNoclipDuringPauseDisallowed()
	{
		return Memory::CallVFunc<bool, 39>(this);
	}

	void GetBBoxBounds(BBox_t* bounds)
	{
		/*bounds->m_vMin = { -16.0f, -16.0f, 0.0f };
		bounds->m_vMax = { 16.0f, 16.0f, 72.0f };
		if ( this->m_bDucking( ) )
		{
			bounds->m_vMax.z = 54.0f;
		}*/

		/*
		if ( sub_1805A8760(*(_QWORD *)(a1 + 0x30)) )  // is in observer check
		{
		v21 = (char *)GetBBox() + 72;
		}
		else if ( *(_BYTE *)(a1 + 0x1EC) )            // m_bDucked
		{
		v21 = (char *)GetBBox() + 36;
		}
		else
		{
		v21 = (char *)GetBBox() + 12;
		}
		*/

		using fnGetBBox = BBox_t * (__fastcall*)(void);
		static fnGetBBox GetBBox = (fnGetBBox)(Memory::FindPattern(CLIENT_DLL, "48 8B 0D ? ? ? ? 48 85 C9 74 ? ? ? ? 48 FF A0 ? ? ? ? 48 8D 05"));

		char* bbox = { };

		if (this->m_bDucked())
		{
			bbox = ((char*)GetBBox() + 36);
		}
		else
		{
			bbox = (char*)GetBBox() + 12;
		}

		bounds = (BBox_t*)bbox;
	}

	Vector GetGroundPos(CMoveData*, float* outf) {
		Vector start = m_pPawn()->m_pGameSceneNode()->m_vecAbsOrigin();
		Vector end = start;
		end.z -= 200.f;

		Ray_t ray{};
		GameTrace_t trace{};
		TraceFilter_t filter{0x1c3003, Globals::m_pLocalPlayerPawn, nullptr, 4};

		Interfaces::m_pVPhys2World->TraceShape(&ray, start, end, &filter, &trace);

		if (outf)
			*outf = trace.m_flFraction;

		if (trace.DidHitWorld()) {
			return trace.m_vecEndPos;
		}

		return start;
	}

	void CalculateJumpHeight(CMoveData* mov) {
		if (this->m_pPawn()->m_fFlags() & FL_ONGROUND) 
			this->m_flMaxJumpHeightThisJump() = std::fmaxf(mov->m_vAbsOrigin.z - this->m_flHeightAtJumpStart(), 0.0f);

		if (this->m_nButtonDownMaskPrev() != this->m_ButtonState().m_nValue)
			this->m_nButtonDownMaskPrev() = this->m_ButtonState().m_nValue;
	}

	float GetGroundPosition(CMoveData* mv);

	void GetBBoxBounds2(BBox_t* bounds)
	{
		using fnGetBBoxFromMovementServices = BBox_t * (__fastcall*)(void*);
		static fnGetBBoxFromMovementServices GetBBoxFromMovementServices = (fnGetBBoxFromMovementServices)(Memory::FindPattern(CLIENT_DLL, " 40 56 48 83 EC ? 48 83 79 ? ? 48 8B F1 0F 29 7C 24 ? 75"));

		bounds = GetBBoxFromMovementServices(this);
	}
public:
	SCHEMA_ADD_FIELD(std::float_t, m_flDuckAmount, "CCSPlayer_MovementServices->m_flDuckAmount");
	SCHEMA_ADD_FIELD(float, m_flStamina, "CCSPlayer_MovementServices->m_flStamina");
	SCHEMA_ADD_FIELD(bool, m_bOldJumpPressed, "CCSPlayer_MovementServices->m_bOldJumpPressed");
	SCHEMA_ADD_FIELD(float&, m_flForwardMove, ("CPlayer_MovementServices->m_flForwardMove"));
	SCHEMA_ADD_FIELD(float&, m_flLeftMove, ("CPlayer_MovementServices->m_flLeftMove"));
	SCHEMA_ADD_FIELD(float&, m_flUpMove, ("CPlayer_MovementServices->m_flUpMove"));
	//SCHEMA_ADD_FIELD(Vector& m_vecLastFinishTickViewAngles, "CPlayer_MovementServices->m_vecLastFinishTickViewAngles");
	SCHEMA_ADD_FIELD(float, m_flOffsetTickCompleteTime, "CCSPlayer_MovementServices->m_flOffsetTickCompleteTime");
	SCHEMA_ADD_FIELD(float, m_flOffsetTickStashedSpeed, "CCSPlayer_MovementServices->m_flOffsetTickStashedSpeed");
	SCHEMA_ADD_FIELD(int, m_nGameCodeHasMovedPlayerAfterCommand, "CCSPlayer_MovementServices->m_nGameCodeHasMovedPlayerAfterCommand");
	SCHEMA_ADD_FIELD(uint64_t&, m_nQueuedButtonDownMask, "CCSPlayer_MovementServices->m_nGameCodeHasMovedPlayerAfterCommand");
	SCHEMA_ADD_FIELD(uint64_t&, m_nQueuedButtonChangeMask, "CCSPlayer_MovementServices->m_nGameCodeHasMovedPlayerAfterCommand");
	SCHEMA_ADD_FIELD(uint64_t&, m_nButtonDoublePressed, "CCSPlayer_MovementServices->m_nGameCodeHasMovedPlayerAfterCommand");
	SCHEMA_ADD_FIELD(uint64_t&, m_nToggleButtonDownMask, "CCSPlayer_MovementServices->m_nGameCodeHasMovedPlayerAfterCommand");
	SCHEMA_ADD_FIELD(uint64_t&, m_nButtonDownMaskPrev, "CCSPlayer_MovementServices->m_nGameCodeHasMovedPlayerAfterCommand");
	SCHEMA_ADD_FIELD(float&, m_flMaxJumpHeightThisJump, "CCSPlayer_MovementServices->m_flMaxJumpHeightThisJump");
	SCHEMA_ADD_FIELD(float&, m_flHeightAtJumpStart, "CCSPlayer_MovementServices->m_flHeightAtJumpStart");
	SCHEMA_ADD_FIELD(Vector&, m_vecLastFinishTickViewAngles, "CPlayer_MovementServices->m_vecLastFinishTickViewAngles");
};

struct BulletInfo_t
{

	BulletInfo_t()
	{
		this->m_vecPosition = Vector(0, 0, 0);
		this->m_flTimeStamp = 0.0f;
		this->m_flExpireTime = -1.0f;
	}

	BulletInfo_t(Vector vecPosition, float flTimeStamp, float flExpireTime)
	{
		this->m_vecPosition = vecPosition;
		this->m_flTimeStamp = flTimeStamp;
		this->m_flExpireTime = flExpireTime;
	}

	Vector	m_vecPosition = Vector(0, 0, 0);
	float	m_flTimeStamp = 0.0f;
	float	m_flExpireTime = 0.0f;
};

class CCSPlayer_BulletServices
{
public:
	[[nodiscard]] CUtlVector<BulletInfo_t>& GetBulletData()
	{
		return *reinterpret_cast<CUtlVector<BulletInfo_t>*>(reinterpret_cast<std::uintptr_t>(this) + 0x48);
	}
};

class C_CSWeaponBase : public C_BasePlayerWeapon
{
public:
	void AddStattrakEntity()
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnAddStatTrackEntity != nullptr);
#endif

		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnAddStatTrackEntity, this, this->m_AttributeManager()->m_Item());
	}

	void AddNametagEntity()
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnAddNametagEntity != nullptr);
#endif

		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnAddNametagEntity, this, this->m_AttributeManager()->m_Item());
	}

	[[nodiscard]] float GetSpread()
	{
		return Memory::CallVFunc<float, /*364*/373>(this);//48 83 EC ? 48 63 91
	}

	void* Shoot()
	{
		return Memory::CallVFunc<void*, 392 + 2>(this); //idk
	}

	struct CSBaseGunFireData {
		char pad00[0x14];
		Vector m_vecShootPos;
		char pad01[52]; // stack corruption
	};

	CSBaseGunFireData GetWeaponAttackTime() {

		static auto fn = reinterpret_cast<CSBaseGunFireData * (__fastcall*)(C_CSWeaponBase*, CSBaseGunFireData*, int)>(Memory::FindPattern(CLIENT_DLL, "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 ? 48 81 EC ? ? ? ? 4C 8B F1"));
		CSBaseGunFireData data = {};
		fn(this, &data, 0);
		return data;
	}

	struct WeaponInaccuracyData_t {
		float flTotalInaccuracy;
		float flMoveInaccuracy;
		float flAirInaccuracy;
	};

	[[nodiscard]] WeaponInaccuracyData_t GetInaccuracy()
	{
		WeaponInaccuracyData_t ret{};
		ret.flTotalInaccuracy = Memory::CallVFunc<float, /*411*/410>(this, &ret.flMoveInaccuracy, &ret.flAirInaccuracy);
		return ret;
	}

	float GetWeaponInAccuracyRecoveryTime()
	{
		using fnGetWeaponInAccuracyRecoveryTime = float(__fastcall*)(C_CSWeaponBase*);
		static fnGetWeaponInAccuracyRecoveryTime GetWeaponInAccuracyRecoveryTime = (fnGetWeaponInAccuracyRecoveryTime)(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? F3 0F 10 B7 ? ? ? ? F3 0F 5E F8"), 1));
		return GetWeaponInAccuracyRecoveryTime(this);
	}

	void UpdateTurningInAccuracy()
	{
		using fnUpdateTurningInAccuracy = void(__fastcall*)(C_CSWeaponBase*);
		static fnUpdateTurningInAccuracy UpdateTurningInAccuracy = (fnUpdateTurningInAccuracy)(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? F3 0F 10 87 ? ? ? ? 44 0F 2F C8"), 1));
		UpdateTurningInAccuracy(this);
	}

	//idk 100% if theyre right, i think theyre, looks identical
	float GetCycleTime(int nWeaponMode)
	{
		return Memory::CallVFunc<float, /*352*/354>(this, nWeaponMode);
	}

	float GetDuckInAccuracy(int nWeaponMode)
	{
		return Memory::CallVFunc<float, /*354*/356>(this, nWeaponMode);
	}

	float GetStandInAccuracy(int nWeaponMode)
	{
		return Memory::CallVFunc<float, /*355*/357>(this, nWeaponMode); //
	}

	float GetJumpInAccuracy(int nWeaponMode)
	{
		return Memory::CallVFunc<float, /*356*/358>(this, nWeaponMode);
	}

	float GetLadderInAccuracy(int nWeaponMode)
	{
		return Memory::CallVFunc<float, /*358*/360>(this, nWeaponMode);
	}
	//till here

	[[nodiscard]] float GetMaxSpeed()
	{
		return Memory::CallVFunc<float, /*366*/368>(this);
	}

	void UpdateAccuracyPenalty()
	{
		Memory::CallVFunc<void, /*410*/411>(this);
	}

	void UpdateAccuracyPenalty_Rebuilt();

	void UpdateSubClass()
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnUpdateSubClass != nullptr);
#endif

		return Functions::fnUpdateSubClass(this);
	}

public:
	SCHEMA_ADD_FIELD(bool, m_bInReload, "C_CSWeaponBase->m_bInReload");
	SCHEMA_ADD_FIELD(bool, m_bBurstMode, "C_CSWeaponBase->m_bBurstMode");
	SCHEMA_ADD_FIELD(bool, m_bUIWeapon, "C_CSWeaponBase->m_bUIWeapon");
	SCHEMA_ADD_FIELD(int, m_weaponMode, "C_CSWeaponBase->m_weaponMode");

	SCHEMA_ADD_FIELD(std::int32_t, m_iOriginalTeamNumber, "C_CSWeaponBase->m_iOriginalTeamNumber");
	SCHEMA_ADD_FIELD(float, m_flRecoilIndex, "C_CSWeaponBase->m_flRecoilIndex");
	SCHEMA_ADD_FIELD(float, m_fAccuracyPenalty, "C_CSWeaponBase->m_fAccuracyPenalty");
	SCHEMA_ADD_FIELD(float, m_flLastAccuracyUpdateTime, "C_CSWeaponBase->m_flLastAccuracyUpdateTime");
	//SCHEMA_ADD_OFFSET(float, m_flRecoilIndex, 0x17E4);

	SCHEMA_ADD_FIELD(std::float_t, m_flPostponeFireReadyFrac, "C_CSWeaponBase->m_flPostponeFireReadyFrac");
	SCHEMA_ADD_FIELD(std::float_t, m_flWatTickOffset, "C_CSWeaponBase->m_flWatTickOffset");

	SCHEMA_ADD_FIELD(GameTick_t, m_nPostponeFireReadyTicks, "C_CSWeaponBase->m_nPostponeFireReadyTicks");
	SCHEMA_ADD_FIELD(GameTime_t, m_fLastShotTime, "C_CSWeaponBase->m_fLastShotTime");
	SCHEMA_ADD_FIELD(std::int32_t, m_zoomLevel, "C_CSWeaponBaseGun->m_zoomLevel");

	SCHEMA_ADD_PFIELD_OFFSET(void, m_hStattrakAttachment, "C_CSWeaponBase->m_iNumEmptyAttacks", 0x4);
	SCHEMA_ADD_PFIELD_OFFSET(void, m_hNametagAttachment, "C_CSWeaponBase->m_iNumEmptyAttacks", 0x14);
};

class C_CSWeaponBaseGun : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_zoomLevel, "C_CSWeaponBaseGun->m_zoomLevel");
	SCHEMA_ADD_FIELD(std::int32_t, m_iBurstShotsRemaining, "C_CSWeaponBaseGun->m_iBurstShotsRemaining");

	bool CanPrimaryAttack(const int nWeaponType, const float flServerTime);
};

class C_C4 : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(char[32], m_szScreenText, "C_C4->m_szScreenText");
	SCHEMA_ADD_FIELD(bool, m_bBombPlanted, "C_C4->m_bBombPlanted");

};

class C_BaseCSGrenade : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(bool, m_bIsHeldByPlayer, "C_BaseCSGrenade->m_bIsHeldByPlayer");
	SCHEMA_ADD_FIELD(bool, m_bPinPulled, "C_BaseCSGrenade->m_bPinPulled");
	SCHEMA_ADD_FIELD(bool, m_bJumpThrow, "C_BaseCSGrenade->m_bJumpThrow");
	SCHEMA_ADD_FIELD(bool, m_bJustPulledPin, "C_BaseCSGrenade->m_bJustPulledPin");

	SCHEMA_ADD_FIELD(GameTime_t, m_fThrowTime, "C_BaseCSGrenade->m_fThrowTime");

	SCHEMA_ADD_FIELD(std::float_t, m_flThrowStrength, "C_BaseCSGrenade->m_flThrowStrength");
	SCHEMA_ADD_FIELD(std::float_t, m_flThrowStrengthApproach, "C_BaseCSGrenade->m_flThrowStrengthApproach");
};

class C_BaseGrenade : public C_BaseFlex
{
public:
	SCHEMA_ADD_FIELD(CHandle< C_CSPlayerPawn >, m_hThrower, "C_BaseGrenade->m_hThrower");
	SCHEMA_ADD_FIELD(bool, m_bIsLive, "C_BaseGrenade->m_bIsLive");
	SCHEMA_ADD_FIELD(float, m_DmgRadius, "C_BaseGrenade->m_DmgRadius");
	SCHEMA_ADD_FIELD(float, m_flDamage, "C_BaseGrenade->m_flDamage");
	SCHEMA_ADD_FIELD(float, m_flDetonateTime, "C_BaseGrenade->m_flDetonateTime");
};

class C_BaseCSGrenadeProjectile : public C_BaseGrenade
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_nBounces, "C_BaseCSGrenadeProjectile->m_nBounces");
	SCHEMA_ADD_FIELD(std::int32_t, m_nExplodeEffectTickBegin, "C_BaseCSGrenadeProjectile->m_nExplodeEffectTickBegin");

	SCHEMA_ADD_FIELD(GameTime_t, m_flSpawnTime, "C_BaseCSGrenadeProjectile->m_flSpawnTime");
	SCHEMA_ADD_FIELD(bool, m_bCanCreateGrenadeTrail, "C_BaseCSGrenadeProjectile->m_bCanCreateGrenadeTrail");

	SCHEMA_ADD_FIELD(Vector, m_vInitialPosition, "C_BaseCSGrenadeProjectile->m_vInitialPosition");
	SCHEMA_ADD_FIELD(Vector, m_vInitialVelocity, "C_BaseCSGrenadeProjectile->m_vInitialVelocity");
	SCHEMA_ADD_FIELD(Vector, m_vecExplodeEffectOrigin, "C_BaseCSGrenadeProjectile->m_vecExplodeEffectOrigin");

	SCHEMA_ADD_FIELD(CUtlVector<Vector>, m_arrTrajectoryTrailPoints, "C_BaseCSGrenadeProjectile->m_arrTrajectoryTrailPoints");
	SCHEMA_ADD_FIELD(CUtlVector<std::float_t>, m_arrTrajectoryTrailPointCreationTimes, "C_BaseCSGrenadeProjectile->m_arrTrajectoryTrailPointCreationTimes");
};

class C_SmokeGrenadeProjectile : public C_BaseCSGrenadeProjectile
{
public:
	SCHEMA_ADD_FIELD(bool, m_bDidSmokeEffect, "C_SmokeGrenadeProjectile->m_bDidSmokeEffect");
	SCHEMA_ADD_FIELD(bool, m_bSmokeEffectSpawned, "C_SmokeGrenadeProjectile->m_bSmokeEffectSpawned");

	SCHEMA_ADD_FIELD(std::int32_t, m_nSmokeEffectTickBegin, "C_SmokeGrenadeProjectile->m_nSmokeEffectTickBegin");

	SCHEMA_ADD_FIELD(Vector, m_vSmokeColor, "C_SmokeGrenadeProjectile->m_vSmokeColor");
	SCHEMA_ADD_FIELD(Vector, m_vSmokeDetonationPos, "C_SmokeGrenadeProjectile->m_vSmokeDetonationPos");

	[[nodiscard]] inline constexpr std::float_t GetMaxTime()
	{
		return 18.f;
	}
};

class C_Inferno : public C_BaseModelEntity
{
public:
	SCHEMA_ADD_FIELD(bool[64], m_bFireIsBurning, "C_Inferno->m_bFireIsBurning");

	SCHEMA_ADD_FIELD(std::int32_t, m_fireCount, "C_Inferno->m_fireCount");
	SCHEMA_ADD_FIELD(std::int32_t, m_nFireEffectTickBegin, "C_Inferno->m_nFireEffectTickBegin");

	SCHEMA_ADD_FIELD(std::float_t, m_nFireLifetime, "C_Inferno->m_nFireLifetime");

	SCHEMA_ADD_FIELD(Vector[64], m_firePositions, "C_Inferno->m_firePositions");
	SCHEMA_ADD_FIELD(Vector[64], m_fireParentPositions, "C_Inferno->m_fireParentPositions");
	SCHEMA_ADD_FIELD(Vector, m_minBounds, "C_Inferno->m_minBounds");
	SCHEMA_ADD_FIELD(Vector, m_maxBounds, "C_Inferno->m_maxBounds");

	[[nodiscard]] inline std::float_t GetMaxTime()
	{
		return Convar::inferno_flame_lifetime->GetFloat();
	}

	/*const char* GetInfernoType()
	{
		const char* szInfernoType = Memory::CallVFunc<const char*, 0x1F8>(this);
		return szInfernoType;
	}*/
};

class CCSPlayerController_InGameMoneyServices
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_iAccount, "CCSPlayerController_InGameMoneyServices->m_iAccount");
	SCHEMA_ADD_FIELD(std::int32_t, m_iCashSpentThisRound, "CCSPlayerController_InGameMoneyServices->m_iCashSpentThisRound");
	SCHEMA_ADD_FIELD(std::int32_t, m_iStartAccount, "CCSPlayerController_InGameMoneyServices->m_iStartAccount");
	SCHEMA_ADD_FIELD(std::int32_t, m_iTotalCashSpent, "CCSPlayerController_InGameMoneyServices->m_iTotalCashSpent");
	SCHEMA_ADD_FIELD(std::int32_t, m_nPreviousAccount, "CCSPlayerController_InGameMoneyServices->m_nPreviousAccount");
};

class C_EnvSky : public C_BaseModelEntity
{
public:
	SCHEMA_ADD_FIELD(bool, m_bStartDisabled, "C_EnvSky->m_bStartDisabled");
	SCHEMA_ADD_FIELD(bool, m_bEnabled, "C_EnvSky->m_bEnabled");

	SCHEMA_ADD_FIELD(std::float_t, m_flBrightnessScale, "C_EnvSky->m_flBrightnessScale");
	SCHEMA_ADD_FIELD(std::float_t, m_flFogMinStart, "C_EnvSky->m_flFogMinStart");
	SCHEMA_ADD_FIELD(std::float_t, m_flFogMinEnd, "C_EnvSky->m_flFogMinEnd");
	SCHEMA_ADD_FIELD(std::float_t, m_flFogMaxStart, "C_EnvSky->m_flFogMaxStart");
	SCHEMA_ADD_FIELD(std::float_t, m_flFogMaxEnd, "C_EnvSky->m_flFogMaxEnd");

	SCHEMA_ADD_FIELD(std::int32_t, m_nFogType, "C_EnvSky->m_nFogType");

	SCHEMA_ADD_FIELD(Color, m_vTintColor, "C_EnvSky->m_vTintColor");
	SCHEMA_ADD_FIELD(Color, m_vTintColorLightingOnly, "C_EnvSky->m_vTintColorLightingOnly");

	SCHEMA_ADD_FIELD(CStrongHandle<CMaterial2>, m_hSkyMaterial, "C_EnvSky->m_hSkyMaterial");
	SCHEMA_ADD_FIELD(CStrongHandle<CMaterial2>, m_hSkyMaterialLightingOnly, "C_EnvSky->m_hSkyMaterialLightingOnly");
};

class C_GradientFog : public C_BaseEntity
{
public:
	SCHEMA_ADD_FIELD(float, m_flFogMaxOpacity, "C_GradientFog->m_flFogMaxOpacity");
	SCHEMA_ADD_FIELD(Color, m_fogColor, "C_GradientFog->m_fogColor");
	SCHEMA_ADD_FIELD(bool, m_bIsEnabled, "C_GradientFog->m_bIsEnabled");
};

class C_EnvCubemapFog : public C_BaseEntity
{
public:
	SCHEMA_ADD_FIELD(float, m_flFogMaxOpacity, "C_EnvCubemapFog->m_flFogMaxOpacity");
};

enum EExplosionType : uint8_t
{
	EXPLOSION_TYPE_DEFAULT,
	EXPLOSION_TYPE_GRENADE,
	EXPLOSION_TYPE_MOLOTOV,
	EXPLOSION_TYPE_FIREWORKS,
	EXPLOSION_TYPE_GASCAN,
	EXPLOSION_TYPE_GASCYLINDER,
	EXPLOSION_TYPE_EXPLOSIVEBARREL,
	EXPLOSION_TYPE_ELECTRICAL,
	EXPLOSION_TYPE_EMP,
	EXPLOSION_TYPE_SHRAPNEL,
	EXPLOSION_TYPE_SMOKEGRENADE,
	EXPLOSION_TYPE_FLASHBANG,
	EXPLOSION_TYPE_TRIPMINE,
	EXPLOSION_TYPE_ICE,
	EXPLOSION_TYPE_NONE,
	EXPLOSION_TYPE_CUSTOM,
	EXPLOSION_TYPE_COUNT
};

class CEffectData
{
public:
	void* m_pVTable;			// 0x0
	Vector m_vecOrigin;			// 0x8
	Vector m_vecStart;			// 0x14
	Vector m_vecNormal;			// 0x20
	QAngle m_angAngles;			// 0x2C
	uint32_t m_hEntity;			// 0x38
	uint32_t m_hOtherEntity;	// 0x3C
	float m_flScale;			// 0x40
	float m_flMagnitude;		// 0x44
	float m_flRadius;			// 0x48
	int m_nSurfaceProp; 		// 0x4C
	void* pEffectIndex; 		// 0x50
	uint32_t m_nEffectIndex;	// 0x58
	uint8_t m_nPenetrate;		// 0x5C
	MEM_PAD(0x1);				// 0x5D
	uint16_t m_nMaterial;		// 0x5E
	uint16_t m_nHitBox;			// 0x60
	uint8_t m_nColor;			// 0x62
	uint8_t m_fFlags;			// 0x63
	uint8_t m_nAttachmentIndex;	// 0x64
	MEM_PAD(0x3);				// 0x65
	int m_nAttachmentName;		// 0x68
	uint16_t m_iEffectName;		// 0x6C
	uint8_t m_nExplosionType;	// 0x6E
	MEM_PAD(0x9);				// 0x6F
public:
	void CreateEffect(const char* szEffectName)
	{
		// @ida:
		/*
			if ( v59 != 1 )
			{
			v13 = &CEffectData::`vftable';
		*/
		static std::uint8_t* pVTable = Memory::FindPattern(CLIENT_DLL, X("4C 8B DC 53 48 81 EC ? ? ? ? F2 0F 10 05"));
		if (!pVTable)
			return;

#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnDispatchEffect != nullptr);
#endif

		// set the vtable
		this->m_pVTable = pVTable;
		// call effect
		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnDispatchEffect, szEffectName, this);
	};
};