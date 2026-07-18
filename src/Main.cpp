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
    AllocConsole();
    SetConsoleTitleA("vexium debug log");
    FILE* f = nullptr;
    freopen_s(&f, "CONOUT$", "w", stdout);

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
    VexLog("OnDllAttach entered");

    // Wait for engine DLLs
    VexLog("waiting for navsystem...");
    while (!Memory::GetModuleBaseHandle(NAVSYSTEM_DLL))
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    VexLog("navsystem ready");

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    VexLog("extra wait done");

    try
    {
        VexLog("Config::Setup...");
        if (!Config::Setup(X("Default.json")))
            throw std::runtime_error(X("failed to setup config"));
        VexLog("Config OK");

        VexLog("Memory::Setup...");
        if (!Memory::Setup())
            throw std::runtime_error(X("failed to setup memory"));
        VexLog("Memory OK");

        // Interfaces: only need m_pSwapChain for vtable[8] hook
        // partial failure is OK — we only throw if SwapChain is null
        VexLog("Interfaces::Setup...");
        Interfaces::Setup();
        if (Interfaces::m_pSwapChain == nullptr)
            throw std::runtime_error(X("m_pSwapChain null — cannot hook Present"));
        VexLog("Interfaces OK (SwapChain found)");

        // Input::Setup() removed — WndProc is now set inside hkPresent
        // from pSwapChain->GetDesc().OutputWindow (guaranteed valid)

        VexLog("Hooks::Setup...");
        if (!Hooks::Setup())
            throw std::runtime_error(X("failed to setup hooks"));
        VexLog("Hooks OK");

        VexLog("=== MENU-ONLY MODE LOADED ===");
    }
    catch (const std::exception& ex)
    {
        VexLog("EXCEPTION:");
        VexLog(ex.what());
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), EXIT_FAILURE);
        return EXIT_FAILURE;
    }

    // Wait for unload key
    while (!Input::IsKeyDown(Config::i(g_Variables.m_Gui.m_iUnloadKey)))
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    VexLog("unload key pressed, detaching...");
    Hooks::Destroy();
    Input::Restore();
    Draw::Destroy();

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), EXIT_SUCCESS);
    return EXIT_SUCCESS;
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
