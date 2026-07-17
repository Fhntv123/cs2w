#pragma once

class CNetworkGameClient
{
public:
	MEM_PAD(0x90); // 0x0000
	float m_flRealTime; //0x0090
	int m_iFrameCount; //0x0094
	float m_flFrameTime; //0x0098
	float m_flFrameTime2; //0x009C
	int m_iMaxClients; //0x00A0
	MEM_PAD(0x8); // 0x00A4
	float m_flPlayerInterp; //0x00AC
	double m_dbSomeTimer; //0x00B0
	MEM_PAD(0x8); // 0x00B8
	float m_flIntervalPerSubTick; //0x00C0
	float m_flCurrentTime; //0x00C4
	float m_flCurrentTIme2; //0x00C8
	MEM_PAD(0x8); //0x00CC
	bool m_bInPrediction; //0x00D4
	MEM_PAD(0x3); //0x00D5
	int m_iTickCount; //0x00D8
	int m_iTickCount2; //0x00DC
	float m_flSomeTimer; //0x00E0
	MEM_PAD(0x4); //0x00E4
	CNetChannelInfo* m_pNetChannelInfo; // 0x00E8
	MEM_PAD(0x8); // 0x00F0
	bool m_bShouldPredict; // 0x00F8
	MEM_PAD(0x14B); // 0x00F9
	int m_iDeltaTick; // 0x0244
	MEM_PAD(0x124); // 0x0248
	int m_iServerTick; // 0x036C
	MEM_PAD(2642556);
	int m_iClientTick;

	void FullUpdate()
	{
		Memory::CallVFunc<void, 28U>(this, X("unk"));
	}

	void SendStringCommand(int nCount, const char* szCommand)
	{
		Memory::CallVFunc<void, 34U>(this, nCount, szCommand);
	}

	[[nodiscard]] void* GetNetChannel(int nSplitScreenSlot = 0)
	{
		return Memory::CallVFunc<void*, 37U>(this, nSplitScreenSlot);
	}

	[[nodiscard]] float GetClientInterpAmount()
	{
		return Memory::CallVFunc<float, 61U>(this);
	}

	void GetPredictionTick(int* pPredictionTick)
	{
		int unk = 0;
		return Memory::CallVFunc<void, 67U>(this, pPredictionTick, &unk);
	}

	void SetPlayerInterpFraction()
	{
		Memory::CallVFunc<void, 70U>(this);
	}

	void SentTickSync(bool some_bool)
	{
		Memory::CallVFunc<void, 74U>(this, some_bool);
	}

	void RunPrediction(CNetworkGameClient* info, int nPredictionReason)
	{
		using fn = void(__fastcall*)(CNetworkGameClient*, int);
		static fn x = reinterpret_cast<fn>(Memory::FindPattern(ENGINE2_DLL, "40 55 41 56 48 83 EC ? 80 B9"));

		x(info, nPredictionReason);
	}

	void FullConnect(void* pAddress)
	{
		Memory::CallVFunc<void, 120U>(this, pAddress);
	}
};

class INetworkClientService
{
public:
	INetworkClientService() = delete;
	INetworkClientService(INetworkClientService&&) = delete;
	INetworkClientService(const INetworkClientService&) = delete;

	[[nodiscard]] CNetworkGameClient* GetNetworkGameClient()
	{
		return Memory::CallVFunc<CNetworkGameClient*, 23U>(this);
	}

	void DisconnectGameNow(int nUnk)
	{
		Memory::CallVFunc<void, 27U>(this, nUnk);
	}

	[[nodiscard]] bool IsMultiPlayer()
	{
		return Memory::CallVFunc<bool, 33U>(this);
	}

};