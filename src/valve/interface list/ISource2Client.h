#pragma once

// forward
class CEconItemSystem;
class ISource2Client
{
public:
	CEconItemSystem* GetEconItemSystem()
	{
		return Memory::CallVFunc<CEconItemSystem*, 122>(this);
	}

	void FrameStageNotify(int iStage)
	{
		return Memory::CallVFunc<void, 36U>(this, iStage);
	}

	CDebugOverlayGameSystem* GetDebugOverlay()
	{
		return Memory::CallVFunc<CDebugOverlayGameSystem*, 163 - 3U>(this);
	}

	float CalcClampedInterp()
	{
		return Memory::CallVFunc<float, 174U>(this);
	}
};