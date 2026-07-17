#pragma once

enum ERayType : std::uint8_t
{
    RAY_TYPE_LINE = 0,
    RAY_TYPE_SPHERE,
    RAY_TYPE_HULL,
    RAY_TYPE_CAPSULE,
    RAY_TYPE_MESH,
};

enum ECollisionGroup
{
    COLLISION_GROUP_NONE = 0,
    COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
    COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
    COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
    COLLISION_GROUP_INTERACTIVE,	// Collides with everything except interactive debris or debris
    COLLISION_GROUP_PLAYER,
    COLLISION_GROUP_BREAKABLE_GLASS,
    COLLISION_GROUP_VEHICLE,
    COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
    // TF2, this filters out other players and CBaseObjects
    COLLISION_GROUP_NPC,			// Generic NPC group
    COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
    COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
    COLLISION_GROUP_VEHICLE_CLIP,	// vehicle clip brush to restrict vehicle movement
    COLLISION_GROUP_PROJECTILE,		// Projectiles!
    COLLISION_GROUP_DOOR_BLOCKER,	// Blocks entities not permitted to get near moving doors
    COLLISION_GROUP_PASSABLE_DOOR,	// Doors that the player shouldn't collide with
    COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
    COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code

    COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
    COLLISION_GROUP_NPC_SCRIPTED,	// USed for NPCs in scripts that should not collide with each other

    LAST_SHARED_COLLISION_GROUP
};

class Ray_t
{
public:
    Ray_t() { Init(Vector(0.0f, 0.0f, 0.0f)); }
    Ray_t(const Vector& vStartOffset) { Init(vStartOffset); }
    Ray_t(const Vector& vCenter, float flRadius) { Init(vCenter, flRadius); }
    Ray_t(const Vector& vMins, const Vector& vMaxs) { Init(vMins, vMaxs); }
    Ray_t(const Vector& vCenterA, const Vector& vCenterB, float flRadius) { Init(vCenterA, vCenterB, flRadius); }
    Ray_t(const Vector& vMins, const Vector& vMaxs, const Vector* pVertices, int nNumVertices) { Init(vMins, vMaxs, pVertices, nNumVertices); }

    void Init(const Vector& vStartOffset)
    {
        m_Line.m_vStartOffset = vStartOffset;
        m_Line.m_flRadius = 0.0f;
        m_eType = RAY_TYPE_LINE;
    }

    void Init(const Vector& vCenter, float flRadius)
    {
        if (flRadius > 0.0f)
        {
            m_Sphere.m_vCenter = vCenter;
            m_Sphere.m_flRadius = flRadius;
            m_eType = RAY_TYPE_SPHERE;
        }
        else
            Init(vCenter);
    }

    void Init(const Vector& vMins, const Vector& vMaxs)
    {
        if (vMins != vMaxs)
        {
            m_Hull.m_vMins = vMins;
            m_Hull.m_vMaxs = vMaxs;
            m_eType = RAY_TYPE_HULL;
        }
        else
            Init(vMins);
    }

    void Init(const Vector& vCenterA, const Vector& vCenterB, float flRadius)
    {
        if (vCenterA != vCenterB)
        {
            if (flRadius > 0.0f)
            {
                m_Capsule.m_vCenter[0] = vCenterA;
                m_Capsule.m_vCenter[1] = vCenterB;
                m_Capsule.m_flRadius = flRadius;
                m_eType = RAY_TYPE_CAPSULE;
            }
            else
                Init(vCenterA, vCenterB);
        }
        else
            Init(vCenterA, flRadius);
    }

    void Init(const Vector& vMins, const Vector& vMaxs, const Vector* pVertices, int nNumVertices)
    {
        m_Mesh.m_vMins = vMins;
        m_Mesh.m_vMaxs = vMaxs;
        m_Mesh.m_pVertices = pVertices;
        m_Mesh.m_nNumVertices = nNumVertices;
        m_eType = RAY_TYPE_MESH;
    }

    struct Line_t
    {
        Vector m_vStartOffset;
        float m_flRadius;
    };

    struct Sphere_t
    {
        Vector m_vCenter;
        float m_flRadius;
    };

    struct Hull_t
    {
        Vector m_vMins;
        Vector m_vMaxs;
    };

    struct Capsule_t
    {
        Vector m_vCenter[2];
        float m_flRadius;
    };

    struct Mesh_t
    {
        Vector m_vMins;
        Vector m_vMaxs;
        const Vector* m_pVertices;
        int m_nNumVertices;
    };

    union
    {
        Line_t         m_Line;
        Sphere_t     m_Sphere;
        Hull_t         m_Hull;
        Capsule_t     m_Capsule;
        Mesh_t         m_Mesh;
    };

    ERayType m_eType;
};

struct SurfaceData_t
{
public:
    MEM_PAD(0x8)
        float m_flPenetrationModifier;
    float m_flDamageModifier;
    MEM_PAD(0x4)
        int m_iMaterial;
};
static_assert(sizeof(SurfaceData_t) == 0x18);

struct TraceHitboxData_t
{
public:

    const char* m_name;				// 0x0000
    const char* m_sSurfaceProperty;	// 0x0008
    const char* m_sBoneName;		// 0x0010
    Vector m_vecMinBounds;			// 0x0018
    Vector m_vecMaxBounds;			// 0x0024
    float m_flShapeRadius;			// 0x0030
    std::uint32_t m_nBoneNameHash;	// 0x0034
    std::int32_t m_nHitGroup;		// 0x0038
    std::uint8_t m_nShapeType;		// 0x003C
    bool m_bTranslationOnly; 		// 0x003D
    std::uint32_t m_CRC;			// 0x0040
    Color m_cRenderColor;			// 0x0044
    std::uint16_t m_nHitboxId;		// 0x0048
    MEM_PAD(0x22);
};
static_assert(sizeof(TraceHitboxData_t) == 0x70);

struct GameTrace_t
{
public:
    GameTrace_t() = default;
    SurfaceData_t* GetSurfaceData();

    int GetHitboxId();
    int GetHitgroup();
    bool DidHitWorld() const;

    bool IsVisible() const;

    void* m_pSurface; // 0x00
    C_CSPlayerPawn* m_pHitEntity; // 0x08
    TraceHitboxData_t* m_pHitboxData; // 0x10
    char pad02[0x38]; // 0x18
    std::uint32_t m_uContents; // 0x50
    MEM_PAD(0x24); // 0x54
    Vector m_vecStartPos; // 0x78
    Vector m_vecEndPos; // 0x84
    Vector m_vecNormal; // 0x90
    Vector m_vecPosition; // 0x9C
    MEM_PAD(0x4); // 0xA8
    float m_flFraction; // 0xAC
    MEM_PAD(0x5);
    ERayType m_eRayType;
    bool m_bAllSolid;
    char pad05[0x4D]; // 0xB7
}; // Size: 0x108
static_assert(sizeof(GameTrace_t) == 0x108);

struct TraceFilter_t
{
public:
    void* __vfptr;
    uint64_t m_uTraceMask;
    std::array<int64_t, 2> m_v1{};
    std::array<int32_t, 4> m_arrSkipHandles{};
    std::array<int16_t, 2> m_arrCollisions{};
    uint8_t m_v2{};
    uint8_t m_v3{};
    uint8_t m_v4{};
    uint8_t m_v5{};
    uint8_t collision{};

    TraceFilter_t() = default;
    TraceFilter_t(uint64_t mask, C_CSPlayerPawn* entity, C_CSPlayerPawn* player, int layer);
};
static_assert(sizeof(TraceFilter_t) == 0x40);

struct BBox_t
{
    Vector m_vMin, m_vMax;
};

struct UpdateValue_t {
    float m_PreviousLengthMod;
    float m_CurrentLengthMod;
    MEM_PAD(0x8);
    std::int16_t m_HandleIdx;
    MEM_PAD(0x6);
};

struct TraceArrElement_t
{
    MEM_PAD(0x30)
};

class CTraceInfo
{
public:
    float m_flPreviousLengthMod;
    float m_flCurrentLengthMod;
    float m_flDamage;
    std::uint32_t m_nPenetrationCount;
    std::uint32_t m_uHandle;
    std::uint32_t m_uHandle2;
};

struct trace_array_element_t {
    std::byte pad_001[0x30];
};

struct TraceData_t2
{
    std::int32_t m_uk1{ };
    float m_uk2{ 52.0f };
    void* m_ArrPointer{ };
    std::int32_t m_uk3{ 128 };
    std::int32_t m_uk4{ static_cast<std::int32_t>(0x80000000) };
    std::array< TraceArrElement_t, 0x80 > m_Arr = { };
    void* m_pUnk;
    int m_nSurfaces; // 0x1820
    CTraceInfo* m_pTraceInfo; // 0x1828
    int m_iUnk; // 0x1830
    void* pointer_update_value; // 0x1838
    MEM_PAD(0xB8); // 0x1840
    Vector m_vecStart; // 0x18F8
    Vector m_vecEnd; // 0x1904
    MEM_PAD(0x50);
};

struct penetration_data_t {
    float m_damage{ };
    uint8_t m_hitgroup{ };

    int m_hitbox{ };

    bool m_penetrated{ true };
};

struct TraceData_t {
    int uk1{ };
    float uk2{ 52.0f };
    void* m_ArrPointer{ };
    int uk3{ 128 };
    int uk4{ static_cast<int>(0x80000000) };
    trace_array_element_t m_Arr[0x80] = {};
    std::byte pad_001[0x8];
    std::int64_t m_nSurfaces{ };
    void* pointer_update_value{ };
    std::byte pad_002[0xC8];
    Vector m_vecStart{ }, m_vecEnd{ };
    std::byte pad_003[0x50];
};