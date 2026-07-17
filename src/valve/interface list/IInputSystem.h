#pragma once

class IInputSystem
{
public:
	bool IsRelativeMouseMode()
	{
		// @ida: 'IInputSystem::SetRelativeMouseMode'
		// @ida: #STR: SDL is %s relative mouse\n
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + 0x4D);
	}

	double GetCurrentTickTime()
	{
		return *reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(this) + 0x2538);
	}

	void* GetSDLWindow()
	{
		// @ida: IInputSystem::DebugSpew -> #STR: "Current coordinate bias %s: %g,%g scale %g,%g\n"
		// find Plat_WindowToOsSpecificHandle first param
		return *reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(this) + 0x26A8);
	}
};