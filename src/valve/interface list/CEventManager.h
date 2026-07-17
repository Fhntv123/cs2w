#pragma once

#define EVENT_DEBUG_ID_INIT			42
#define EVENT_DEBUG_ID_SHUTDOWN		13

class IGameEventListener2
{
public:
	virtual			~IGameEventListener2() {}
	virtual void	FireGameEvent(CGameEvent* pEvent) = 0;
	virtual int		GetEventDebugID()
	{
		return nDebugID;
	}
public:
	int	nDebugID = 0;
};

// https://github.com/Wend4r/hl2sdk/blob/5d5e84e031673a6c3bb576f63b141e15bb1248f8/public/igameevents.h#L146-L206
class CEventListener : public IGameEventListener2
{
public:
	void Setup(const std::deque<const char*>& arrEvents);
	void Destroy();

	virtual void FireGameEvent(CGameEvent* pEvent) override;
	virtual int GetEventDebugID() override
	{
		return EVENT_DEBUG_ID_INIT;
	}
};

class IGameEventManager2
{
public:
	enum
	{
		SERVERSIDE = 0, // this is a server side listener, event logger etc
		CLIENTSIDE, // this is a client side listenet, HUD element etc
		CLIENTSTUB, // this is a serverside stub for a remote client listener (used by engine only)
		SERVERSIDE_OLD, // legacy support for old server event listeners
		CLIENTSIDE_OLD // legacy support for old client event listeners
	};

	virtual ~IGameEventManager2() {}

	/// load game event descriptions from a file e.g. "resource\gameevents.res"
	///  @Returns: count of loaded game events
	int LoadEventsFromFile(const char* szFileName)
	{
		return Memory::CallVFunc<int, 1U>(this, szFileName);
	}

	// remove all and anything
	void Reset()
	{
		Memory::CallVFunc<void, 2U>(this);
	}

	/// add a listener for a particular event
	///  @Returns: true if listener was successfully added for given event or was already there, false otherwise
	bool AddListener(IGameEventListener2* pListener, const char* szName, bool bServerSide = false)
	{
		return Memory::CallVFunc<bool, 3U>(this, pListener, szName, bServerSide);
	}

	///  @Returns: true if given listener is listens to given event, false otherwise
	bool FindListener(IGameEventListener2* pListener, const char* szName)
	{
		return Memory::CallVFunc<bool, 4U>(this, pListener, szName);
	}

	// remove a listener
	void RemoveListener(IGameEventListener2* pListener)
	{
		Memory::CallVFunc<void, 5U>(this, pListener);
	}
};