#pragma once

/*
 * detour hooking implementation using minhook
 * @credits: https://github.com/TsudaKageyu/minhook
 */
class CDetourHook
{
public:
	/* constructor */
	CDetourHook() = default;

	CDetourHook(std::string szName)
		: strName(szName) { }

	explicit CDetourHook(void* pFunction, void* pDetour)
		: pBaseFn(pFunction), pReplaceFn(pDetour) { }

	CDetourHook& Init(void* pDetour, void* pInterface, const size_t nIndex)
	{
		if (bInit)
			return *this;

		pBaseFn = Memory::GetVFunc(pInterface, nIndex);
		pReplaceFn = pDetour;

		if (pBaseFn == nullptr || pReplaceFn == nullptr)
		{
			// Log orange warning
			Logging::PushConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN);
			Logging::Print(X("[warning] failed to get virtual function -> {}\n"), strName);
			Logging::PopConsoleColor();

			bInit = false;
		}
		else bInit = true;
		return *this;
	}

	CDetourHook& Init(void* pDetour, void* pBase)
	{
		if (bInit)
			return *this;

		pBaseFn = pBase;
		pReplaceFn = pDetour;

		if (pBaseFn == nullptr || pReplaceFn == nullptr)
		{
			// Log orange warning
			Logging::PushConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN);
			Logging::Print(
				X("[warning] failed to get virtual function at base addr -> {}\n"),
				reinterpret_cast<std::uintptr_t>(pBaseFn));
			Logging::PopConsoleColor();

			bInit = false;
		}
		else bInit = true;
		return *this;
	}

	/* first hook setup */
	void Create()
	{
		// check is hook be initialized
		if (!bInit || !bCanCreate || bIsHooked)
			return;

		if (pBaseFn == nullptr || pReplaceFn == nullptr)
			return;

		if (const MH_STATUS status = MH_CreateHook(pBaseFn, pReplaceFn, &pOriginalFn); status != MH_OK)
		{
			const char* szStatus = MH_StatusToString(status);
			uintptr_t pBase = reinterpret_cast<uintptr_t>(pBaseFn);
			throw std::runtime_error(std::vformat(
				X("failed to create hook function, status: {}\nbase function addr -> {:#08X}"),
				std::make_format_args(szStatus, pBase)));
		}
			
		if (!this->Replace())
			return;

		Logging::PushConsoleColor(FOREGROUND_INTENSE_GREEN | FOREGROUND_RED);
		Logging::Print(X("[->] Hooked {}!"), strName);
		Logging::PopConsoleColor();
	}

	/* replace function to our modified */
	bool Replace()
	{
		// check is hook be created
		if (pBaseFn == nullptr)
			return false;

		// check is function not already hooked
		if (bIsHooked)
			return false;

		if (const MH_STATUS status = MH_EnableHook(pBaseFn); status != MH_OK)
		{
			const char* szStatus = MH_StatusToString(status);
			uintptr_t pBase = reinterpret_cast<uintptr_t>(pBaseFn);
			throw std::runtime_error(std::vformat(
				X("failed to enable hook function, status: {}\nbase function addr -> {:#08X}"),
				std::make_format_args(szStatus, pBase)));
		}

		// switch hook state
		bIsHooked = true;
		return true;
	}

	/* remove hook fully */
	bool Remove()
	{
		// restore it at first
		if (!this->Restore())
			return false;

		if (const MH_STATUS status = MH_RemoveHook(pBaseFn); status != MH_OK)
		{
			const char* szStatus = MH_StatusToString(status);
			uintptr_t pBase = reinterpret_cast<uintptr_t>(pBaseFn);
			throw std::runtime_error(std::vformat(
				X("failed to remove hook, status: {}\n base function addr -> {:#08X}"),
				std::make_format_args(szStatus, pBase)));
		}

		return true;
	}

	/* replace swaped modified function back to original */
	bool Restore()
	{
		// check is function hooked
		if (!bIsHooked)
			return false;
		
		if (const MH_STATUS status = MH_DisableHook(pBaseFn); status != MH_OK)
		{
			const char* szStatus = MH_StatusToString(status);
			uintptr_t pBase = reinterpret_cast<uintptr_t>(pBaseFn);
			throw std::runtime_error(std::vformat(
				X("failed to restore hook, status: {}\n base function addr -> {:#08X}"),
				std::make_format_args(szStatus, pBase)));
		}

		// switch hook state
		bIsHooked = false;
		return true;
	}

	/* get original function pointer (not a call!) */
	template <typename Fn>
	Fn GetOriginal()
	{
		return static_cast<Fn>(pOriginalFn);
	}

	/* function base address */
	void* pBaseFn = nullptr;
	/* in future that is being modified and replace original */
	void* pReplaceFn = nullptr;
	/* saved function to get available restore hook later */
	void* pOriginalFn = nullptr;

	bool bInit = false;
	bool bIsHooked = false;
	bool bCanCreate = true;

	std::string strName = "";
};