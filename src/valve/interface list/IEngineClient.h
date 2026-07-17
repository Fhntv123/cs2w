#pragma once

/*
 * functions used to verify entries:
 * 0, 1:
 * @ida CNetworkGameClient::ClientPollNetworking(): engine2.dll -> "40 53 48 83 EC 20 83 B9 ? ? ? ? ? 48 8B D9 0F 8C"
 * 3:
 * @ida CNetworkGameClient::CL_RunPrediction(): engine2.dll -> ABS["E8 ? ? ? ? 48 85 F6 74 67" + 0x1]
 * 2, [4 .. 7]:
 * @ida CNetworkGameClient::CL_ProcessPacketEntities(): engine2.dll -> ABS["E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 83 BB" + 0x1] @XREF: "Packet Entities has a larger spawn group creation sequence (%u) than we received, forcing to max %u\n"
 * 9:
 * @ida CNetworkGameClient::OnClientFrameSimulate(): engine2.dll -> "40 57 48 83 EC 70 0F 29 74 24"
 */
enum EClientFrameStage : int
{
	FRAME_NET_UPDATE_POSTDATAUPDATE_START = 2,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE,
	FRAME_RENDER_START,
	FRAME_RENDER_END,
	FRAME_NET_UPDATE_END,
	FRAME_NET_CREATION,
	FRAME_RESTORE_SERVER_STATE,
	FRAME_SIMULATE_END
};

enum EFlow : int
{
	FLOW_OUTGOING = 0,
	FLOW_INCOMING = 1,
};

class CNetChannelInfo
{
public:
	[[nodiscard]] const char* GetName()
	{
		return Memory::CallVFunc<const char*, 0U>(this);
	}

	[[nodiscard]] const char* GetAddress()
	{
		return Memory::CallVFunc<const char*, 1U>(this);
	}

	[[nodiscard]] float GetTime()
	{
		return Memory::CallVFunc<float, 2U>(this);
	}

	[[nodiscard]] float GetTimeConnected()
	{
		return Memory::CallVFunc<float, 3U>(this);
	}

	[[nodiscard]] float GetBufferSize()
	{
		return Memory::CallVFunc<float, 4U>(this);
	}

	[[nodiscard]] float GetDataRate()
	{
		return Memory::CallVFunc<float, 5U>(this);
	}

	[[nodiscard]] float GetLatency(int nFlow)
	{
		return Memory::CallVFunc<float, 9U>(this, nFlow);
	}

	[[nodiscard]] int GetAverageLatency(int nFlow)
	{
		return Memory::CallVFunc<int, 10U>(this, nFlow);
	}

	[[nodiscard]] float GetNetLatency()
	{
		return Memory::CallVFunc<float, 10>(this);
	}
	[[nodiscard]] float GetEngineLatency()
	{
		return Memory::CallVFunc<float, 11>(this);
	}

};

class IEngineClient
{
public:
	[[nodiscard]] int GetMaxClients()
	{
		return Memory::CallVFunc<int, 34U>(this);
	}

	[[nodiscard]] bool IsInGame()
	{
		return Memory::CallVFunc<bool, 38>(this);
	}

	[[nodiscard]] bool IsConnected()
	{
		return Memory::CallVFunc<bool, 39>(this);
	}

	[[nodiscard]] CNetChannelInfo* GetNetChannelInfo(int nSplitScreenSlot = 0)
	{
		return Memory::CallVFunc<CNetChannelInfo*, 40>(this, nSplitScreenSlot);
	}

	void ExecuteClientCmdUnrestricted(const char* szCommand)
	{
		Memory::CallVFunc<void, 49>(this, 0, szCommand, 0x7FFEF001);
	}

	// return CBaseHandle index
	[[nodiscard]] int GetLocalPlayer()
	{
		int nIndex = -1;

		Memory::CallVFunc<void, 53>(this, std::ref(nIndex), 0);

		return nIndex + 1;
	}

	void GetScreenSize(int& iWidth, int& iHeight)
	{
		Memory::CallVFunc<void, 60>(this, std::ref(iWidth), std::ref(iHeight));
	}

	[[nodiscard]] const char* GetLevelName()
	{
		return Memory::CallVFunc<const char*, 62>(this);
	}

	[[nodiscard]] const char* GetLevelNameShort()
	{
		return Memory::CallVFunc<const char*, 63>(this);
	}

	//[[nodiscard]] const char* GetProductVersionString()
	//{
	//	return Memory::CallVFunc<const char*, 89>(this);
	//}

	[[nodiscard]] void GetClientNetworkData(CClientNetworkData* pClientNetworkData)
	{
		Memory::CallVFunc<void, 178U>(this, pClientNetworkData);
	}
};