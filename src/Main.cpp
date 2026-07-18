#include "Precompiled.h"
#include <fstream>
#include "features/visuals/StringForSkyboxes.h"

// ── debug log ────────────────────────────────────────────────────────────────
static HANDLE g_hLogFile = INVALID_HANDLE_VALUE;

static void VexLog(const char* msg)
{
    if (HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE); hCon && hCon != INVALID_HANDLE_VALUE)
    {
        DWORD w = 0;
        WriteConsoleA(hCon, msg, (DWORD)strlen(msg), &w, nullptr);
        WriteConsoleA(hCon, "\n", 1, &w, nullptr);
    }
    if (g_hLogFile != INVALID_HANDLE_VALUE)
    {
        DWORD w = 0;
        WriteFile(g_hLogFile, msg, (DWORD)strlen(msg), &w, nullptr);
        WriteFile(g_hLogFile, "\n", 1, &w, nullptr);
    }
}

static void VexLogFmt(const char* fmt, const char* val)
{
    char buf[512];
    wsprintfA(buf, fmt, val);
    VexLog(buf);
}

static void VexLogInit()
{
    // NO AllocConsole — it corrupts the process on repeated inject
    // Write to file only

    char path[MAX_PATH];
    DWORD len = GetTempPathA(MAX_PATH, path);
    if (len > 0 && len < MAX_PATH - 20)
    {
        lstrcatA(path, "vexium_log.txt");
        g_hLogFile = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ,
            nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    }
    VexLog("=== vexium debug log started ===");
    VexLog("DllMain -> DLL_PROCESS_ATTACH");
}

// ─────────────────────────────────────────────────────────────────────────────

DWORD WINAPI OnDllAttach(LPVOID lpParameter)
{
    // Guard against double-inject
    static volatile LONG s_bAttached = 0;
    if (InterlockedCompareExchange(&s_bAttached, 1, 0) != 0)
    {
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 0);
        return 0;
    }

    VexLog("OnDllAttach entered");

    VexLog("waiting for navsystem...");
    int waitCount = 0;
    while (!Memory::GetModuleBaseHandle(NAVSYSTEM_DLL))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (++waitCount > 60)  // 30s timeout
        {
            VexLog("navsystem timeout — aborting");
            FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 1);
            return 1;
        }
    }
    VexLog("navsystem ready");

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    VexLog("extra wait done");

    // ── All setup wrapped in SEH ─────────────────────────────────────────────
    bool ok = false;
    __try
    {
        VexLog("Config::Setup...");
        if (!Config::Setup(X("Default.json")))
        {
            VexLog("Config FAILED");
            __leave;
        }
        VexLog("Config OK");

        VexLog("Memory::Setup...");
        if (!Memory::Setup())
        {
            VexLog("Memory FAILED");
            __leave;
        }
        VexLog("Memory OK");

        VexLog("Interfaces::Setup...");
        // Interfaces::Setup does many pattern scans that can throw or return
        // partial data. We do NOT throw — we just check m_pSwapChain after.
        bool ifacesOk = false;
        __try
        {
            ifacesOk = Interfaces::Setup();
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            VexLog("Interfaces::Setup threw SEH exception (caught)");
            ifacesOk = false;
        }
        VexLogFmt("Interfaces returned: %s", ifacesOk ? "true" : "false");

        if (Interfaces::m_pSwapChain == nullptr)
        {
            VexLog("m_pSwapChain is NULL — cannot hook Present");
            __leave;
        }
        VexLog("m_pSwapChain OK");

        VexLog("Hooks::Setup...");
        bool hooksOk = false;
        __try
        {
            hooksOk = Hooks::Setup();
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            VexLog("Hooks::Setup threw SEH (caught)");
            hooksOk = false;
        }
        if (!hooksOk)
        {
            VexLog("Hooks FAILED");
            __leave;
        }
        VexLog("Hooks OK");

        VexLog("=== MENU-ONLY MODE LOADED ===");
        ok = true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        VexLog("OUTER SEH exception caught in OnDllAttach");
        ok = false;
    }

    if (!ok)
    {
        VexLog("attach failed — unloading");
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 1);
        return 1;
    }

    // Idle loop — wait for unload key (INSERT by default)
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (GetAsyncKeyState(VK_INSERT) & 0x8000)
            break;
    }

    VexLog("unload key pressed, detaching...");
    __try { Hooks::Destroy(); } __except (EXCEPTION_EXECUTE_HANDLER) {}
    __try { Input::Restore(); } __except (EXCEPTION_EXECUTE_HANDLER) {}

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 0);
    return 0;
}

DWORD WINAPI OnDllDetach(LPVOID lpParameter)
{
    while (!Input::IsKeyDown(Config::i(g_Variables.m_Gui.m_iUnloadKey)))
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    Globals::m_bIsUnloading = true;
    Gui::m_bOpen = false;
    Gui::m_bInitialized = false;

    // Utilities::m_EventListener.Destroy(); // disabled in menu-only mode
    Draw::Destroy();
    Input::Restore();
    Hooks::Destroy();

#ifdef DEBUG_CONSOLE
    Logging::Detach();
#else
    if (Logging::ofsFile.is_open())
        Logging::ofsFile.close();
#endif
    LI_FN(FreeLibraryAndExitThread)((HMODULE)lpParameter, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        VexLogInit();

        LI_FN(DisableThreadLibraryCalls)(hModule);
        Globals::m_hDll = hModule;

        VexLog("CreateThread -> OnDllAttach");
        if (HANDLE hThread = LI_FN(CreateThread)(nullptr, 0U, OnDllAttach, hModule, 0UL, nullptr); hThread != nullptr)
            LI_FN(CloseHandle)(hThread);

        return TRUE;
    }

    return FALSE;
}
