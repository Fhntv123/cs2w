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

    // Wait for engine DLLs to be ready
    VexLog("waiting for navsystem...");
    while (!Memory::GetModuleBaseHandle(NAVSYSTEM_DLL))
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    VexLog("navsystem ready");

    try
    {
        // --- MENU-ONLY MODE ---
        // Only init what's needed to show the menu without crashing.
        // Re-enable other systems one by one after menu works.

        VexLog("Config::Setup...");
        if (!Config::Setup(X("Default.json")))
            throw std::runtime_error(X("failed to setup config"));

        VexLog("Memory::Setup...");
        if (!Memory::Setup())
            throw std::runtime_error(X("failed to setup memory"));

        VexLog("Interfaces::Setup...");
        if (!Interfaces::Setup())
            throw std::runtime_error(X("failed to get interfaces"));

        VexLog("Input::Setup...");
        if (!Input::Setup())
            throw std::runtime_error(X("failed to setup inputs"));

        // Draw::Setup is called inside hkPresent (guaranteed DX11 ready)
        VexLog("Hooks::Setup...");
        if (!Hooks::Setup())
            throw std::runtime_error(X("failed to setup hooks"));

        VexLog("=== MENU-ONLY MODE LOADED OK ===");

        /*
        // --- DISABLED UNTIL MENU WORKS ---
        VexLog("Math::Setup...");
        if (!Math::Setup())
            throw std::runtime_error(X("failed to setup math"));

        VexLog("Functions::Setup...");
        if (!Functions::Setup())
            throw std::runtime_error(X("failed to setup functions"));

        VexLog("ReturnAddressSpoofGadgets::FindGadgets...");
        if (!ReturnAddressSpoofGadgets::FindGadgets())
            throw std::runtime_error(X("failed to find gadgets"));

        VexLog("Schema::Setup...");
        if (!Schema::Setup())
            throw std::runtime_error(X("failed to dump schema"));

        VexLog("Convar::Setup...");
        if (!Convar::Setup())
            throw std::runtime_error(X("failed to setup convars"));

        VexLog("Chams::Init...");
        g_Chams->Init();

        VexLog("EventListener::Setup...");
        Interfaces::m_pCvar->UnlockHiddenCVars();
        Utilities::m_EventListener.Setup({ X("round_start"), X("add_bullet_hit_marker"), X("bullet_impact"), X("player_hurt"), X("player_death"), X("weapon_fire"), X("vote_cast"), X("vote_started"), X("item_purchase"), X("bomb_defused"), X("bomb_begindefuse"), X("bomb_planted"), X("bomb_beginplant") });
        */
    }
    catch (const std::exception& ex)
    {
        char buf[512];
        wsprintfA(buf, "EXCEPTION: %s", ex.what());
        VexLog(buf);

        Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
        Logging::Print(X("[error] {}"), ex.what());
        Logging::PopConsoleColor();

#ifdef _DEBUG
        _RPT0(_CRT_ERROR, ex.what());
#else
        LI_FN(FreeLibraryAndExitThread)(static_cast<HMODULE>(lpParameter), EXIT_FAILURE);
#endif
    }

    return EXIT_FAILURE;
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
