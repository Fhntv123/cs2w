struct EventHash_t
{
	EventHash_t(const char* szString)
	{
		m_szString = szString;
		m_uHash = MurmurHash2(szString, static_cast<int>(CRT::StringLength(szString)), STRINGTOKEN_MURMURHASH_SEED);
	}

	std::uint32_t m_uHash;
	const char* m_szString;
};

// https://github.com/Wend4r/hl2sdk/blob/5d5e84e031673a6c3bb576f63b141e15bb1248f8/public/igameevents.h#L82-L141
// forward
class CBasePlayerController;
class C_BasePlayerPawn;
class CEntityInstace;
class CGameEvent
{
public:
	virtual ~CGameEvent() {}

	[[nodiscard]] const char* GetName() const
	{
		//   @ida: client.dll -> U8["48 8B CE FF 50 ? 48 8D 0D" + 0x5]   @XREF: "show_freeze_panel"
		return Memory::CallVFunc<const char*, 1U>(this);
	}

	[[nodiscard]] bool GetBool(const char* szEvent, bool bDefault = false) const //   @Todo: unverified
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<bool, 6U>(this, uKeyHash, bDefault);
	}

	[[nodiscard]] int GetInt(const char* szEvent, int iDefault = 0) const
	{
		//   @ida CGameEvent::GetInt_Old(): client.dll -> ABS["E8 ? ? ? ? 0F B6 4D 77" + 0x1] 
		//   @ida: client.dll -> U8["E8 ? ? ? ? 45 33 C0 48 89 74 24 ? 48 8D 54 24 ? 49 8B CF FF D3 85 C0 8B 44 24 48" - 0x1] / sizeof(std::uintptr_t)   @XREF: "headshot"
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<int, 7U>(this, uKeyHash, iDefault);
	}

	[[nodiscard]] std::uint64_t GetUint64(const char* szEvent, std::uint64_t ullDefault = 0ULL) const //   @Todo: unverified
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<std::uint64_t, 8U>(this, uKeyHash, ullDefault);
	}

	[[nodiscard]] float GetFloat(const char* szEvent, const float flDefault = 0.0f) const
	{
		//   @ida CGameEvent::GetFloat_Old(): client.dll -> ABS["E8 ? ? ? ? 0F 28 D8 89 5C 24 20" + 0x1]
		//   @ida: client.dll -> U8["4C 8B 60 ? 4D 8B C6" + 0x3] / sizeof(std::uintptr_t)   @XREF: "theta"
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<float, 9U>(this, uKeyHash, flDefault);
	}

	[[nodiscard]] const char* GetString(const char* szEvent, const char* szDefault = "") const
	{
		//   @ida: client.dll -> U8["48 8B 78 ? 48 8D 4D 80" + 0x3] / sizeof(std::uintptr_t)   @XREF: "weapon"
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<const char*, 10U>(this, uKeyHash, szDefault);
	}

	[[nodiscard]] const void* GetPtr(const char* szEvent) const //   @Todo: unverified
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<const void*, 11U>(this, uKeyHash);
	}

	int GetPlayerIndex(const char* szEvent)
	{
		//   @ida: client.dll -> U8["4C 8B 70 ? 4C 89 7C 24" + 0x3] / sizeof(std::uintptr_t)   @XREF: "attacker", "assister", "userid"
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		int nOutIndex;
		Memory::CallVFunc<void, 14U>(this, uKeyHash, &nOutIndex);
		return nOutIndex;
	}

	CBasePlayerController* GetPlayerController(const char* szEvent)
	{
		//   @ida: client.dll -> ["89 74 24 40 4C 8B A8 ? ? ? ? 40" + 0x7] / sizeof(std::uintptr_t)   @XREF: "killer_controller"
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<CBasePlayerController*, 16U>(this, uKeyHash);
	}

	C_BasePlayerPawn* GetPlayerPawn(const char* szEvent)
	{
		//   @ida CGameEvent::GetPlayerPawn_Old(): server.dll -> ABS["E8 ? ? ? ? 48 85 C0 74 0B 48 8B C8 E8 ? ? ? ? 4C 8B F0 41 8B 46 08" + 0x1]   @XREF: "userid"
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<C_BasePlayerPawn*, 17U>(this, uKeyHash);
	}

	void SetBool(const char* szEvent, const bool bValue)
	{
		//   @ida CGameEvent::SetBool_Old(): server.dll -> ABS["E8 ? ? ? ? 48 8B 0D ? ? ? ? 45 33 C0 48 8B D3 48 8B 01 FF 50 38 48 8B 46 10" + 0x1]   @XREF: "canbuy"'
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 20U>(this, uKeyHash, bValue);
	}

	void SetInt(const char* szEvent, const int nValue)
	{
		//   @ida: server.dll -> ["48 8D 4D A7 4C 8B B0 ? ? ? ? FF" + 0x7] / sizeof(std::uintptr_t)
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 21U>(this, uKeyHash, nValue);
	}

	void SetUint64(const char* szEvent, const std::uint64_t ullValue) //   @Todo: unverified
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 22U>(this, uKeyHash, ullValue);
	}

	void SetFloat(const char* szEvent, const float flValue)
	{
		//   @ida: server.dll -> ["48 8B B0 ? ? ? ? 33 D2 44 89 6C 24" + 0x3] / sizeof(std::uintptr_t)   @XREF: "inferno_expire"
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 23U>(this, uKeyHash, flValue);
	}

	void SetString(const char* szEvent, const char* szValue)
	{
		//   @ida: server.dll -> ["48 8D 4D A7 48 8B B8 ? ? ? ? 33" + 0x7] / sizeof(std::uintptr_t)   @XREF: "weapon"
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 24U>(this, uKeyHash, szValue);
	}

	void SetPtr(const char* szEvent, const void* pValue) //   @Todo: unverified
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 25U>(this, uKeyHash, pValue);
	}
public:
	std::byte pad0[0x60]; // 0x08
};
static_assert(sizeof(CGameEvent) == 0x68); // size verify   @ida: (CGameEventManager::CreateEvent) client.dll -> ["B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 23 4C 8B C5" + 0x1]