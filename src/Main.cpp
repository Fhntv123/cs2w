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

    // Wait for all engine DLLs to be ready
    VexLog("waiting for navsystem...");
    while (!Memory::GetModuleBaseHandle(NAVSYSTEM_DLL))
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    VexLog("navsystem ready");

    // Extra wait — give the game time to fully init DX11 and all modules
    VexLog("extra wait 2s for DX11 init...");
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    VexLog("wait done");

    try
    {
        // ── Config ───────────────────────────────────────────────────────────
        VexLog("Config::Setup...");
        if (!Config::Setup(X("Default.json")))
            throw std::runtime_error(X("failed to setup config"));
        VexLog("Config OK");

        // ── Memory ───────────────────────────────────────────────────────────
        VexLog("Memory::Setup...");
        if (!Memory::Setup())
            throw std::runtime_error(X("failed to setup memory"));
        VexLog("Memory OK");

        // ── Interfaces ───────────────────────────────────────────────────────
        // NOTE: Interfaces::Setup() scans many patterns. Some may fail on
        // certain game versions. We do NOT throw on failure — we only require
        // the DX11 interfaces (m_pSwapChain, m_pDevice) for the menu.
        VexLog("Interfaces::Setup...");
        bool ifacesOk = Interfaces::Setup();
        if (ifacesOk)
            VexLog("Interfaces OK (all)");
        else
            VexLog("Interfaces PARTIAL — some patterns failed, continuing for menu");

        // Critical check: DX11 must be available for the menu
        if (Interfaces::m_pSwapChain == nullptr)
            throw std::runtime_error(X("m_pSwapChain is null — DX11 not ready"));
        if (Interfaces::m_pDevice == nullptr)
            throw std::runtime_error(X("m_pDevice is null — DX11 not ready"));
        VexLog("DX11 interfaces OK");

        // ── Input (WndProc hook) ──────────────────────────────────────────────
        VexLog("Input::Setup...");
        if (!Input::Setup())
            throw std::runtime_error(X("failed to setup inputs"));
        VexLog("Input OK");

        // ── Hooks (DX11 Present/Resize + mouse only) ──────────────────────────
        // Draw::Setup is called inside hkPresent — guaranteed DX11 ready there
        VexLog("Hooks::Setup...");
        if (!Hooks::Setup())
            throw std::runtime_error(X("failed to setup hooks"));
        VexLog("Hooks OK");

        VexLog("=== MENU-ONLY MODE LOADED OK ===");

        /*
        // ── DISABLED UNTIL MENU WORKS ─────────────────────────────────────────
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
        Utilities::m_EventListener.Setup({ X("round_start"), X("add_bullet_hit_marker"),
            X("bullet_impact"), X("player_hurt"), X("player_death"), X("weapon_fire"),
            X("vote_cast"), X("vote_started"), X("item_purchase"), X("bomb_defused"),
            X("bomb_begindefuse"), X("bomb_planted"), X("bomb_beginplant") });
        */
    }
    catch (const std::exception& ex)
    {
        VexLogFmt("EXCEPTION: %s", ex.what());

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
