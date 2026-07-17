#include "Precompiled.h"
#include <fstream>
#include "features/visuals/StringForSkyboxes.h"

// Pure WinAPI log — works even before CRT/static init, writes to %TEMP%\vexium_log.txt
static void VexLog(const char* msg)
{
    char path[MAX_PATH];
    DWORD len = GetTempPathA(MAX_PATH, path);
    if (len == 0 || len >= MAX_PATH) return;
    // append filename
    const char* fname = "vexium_log.txt";
    if (len + strlen(fname) >= MAX_PATH) return;
    memcpy(path + len, fname, strlen(fname) + 1);

    HANDLE hFile = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;
    SetFilePointer(hFile, 0, nullptr, FILE_END);
    DWORD written = 0;
    WriteFile(hFile, msg, (DWORD)strlen(msg), &written, nullptr);
    WriteFile(hFile, "\n", 1, &written, nullptr);
    CloseHandle(hFile);
}

DWORD WINAPI OnDllAttach(LPVOID lpParameter)
{
    VexLog("[vexium] OnDllAttach entered");

    VexLog("[vexium] waiting for navsystem...");
    while (!Memory::GetModuleBaseHandle(NAVSYSTEM_DLL))
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    VexLog("[vexium] navsystem ready");

    try
    {
#ifdef DEBUG_CONSOLE
        if (!Logging::Attach(X("vexium.cc")))
            throw std::runtime_error(X("failed to attach console"));
#endif        
        //Downloader->Run();

        VexLog("[vexium] Config::Setup...");
        if (!Config::Setup(X("Default.json")))
            throw std::runtime_error(X("failed to setup config"));

        VexLog("[vexium] Memory::Setup...");
        if (!Memory::Setup())
            throw std::runtime_error(X("failed to setup memory"));

        VexLog("[vexium] Math::Setup...");
        if (!Math::Setup())
            throw std::runtime_error(X("failed to setup"));

        VexLog("[vexium] Functions::Setup...");
        if (!Functions::Setup())
            throw std::runtime_error(X("failed to setup functions"));

        VexLog("[vexium] ReturnAddressSpoofGadgets::FindGadgets...");
        if (!ReturnAddressSpoofGadgets::FindGadgets())
            throw std::runtime_error(X("failed to find gadgets"));

        VexLog("[vexium] Interfaces::Setup...");
        if (!Interfaces::Setup())
            throw std::runtime_error(X("failed to get interfaces"));

        VexLog("[vexium] Schema::Setup...");
        if (!Schema::Setup())
            throw std::runtime_error(X("failed to dump schema"));

        //#ifdef _DEBUG
        //if (!Convar::Dump(X(L"Convars.txt")))
        //   throw std::runtime_error(X("failed to dump convars"));
        //#endif

        VexLog("[vexium] Convar::Setup...");
        if (!Convar::Setup())
            throw std::runtime_error(X("failed to setup convars"));

        VexLog("[vexium] Input::Setup...");
        if (!Input::Setup())
            throw std::runtime_error(X("failed to setup inputs"));

        VexLog("[vexium] Draw::Setup...");
        if (!Draw::m_bInitialized)
            Draw::Setup(Interfaces::m_pDevice, Interfaces::m_pDeviceContext);

        //STR::CreateDirectories();

        //g_WorldModulation->LoadDefaultSkyboxes();

        VexLog("[vexium] Chams::Init...");
        g_Chams->Init();

        //if (!InventoryChanger::DumpAllSkins())
        //    throw std::runtime_error(X("failed to dump all skins"));

        VexLog("[vexium] Hooks::Setup...");
        if (!Hooks::Setup())
            throw std::runtime_error(X("failed to setup hooks"));

        VexLog("[vexium] EventListener::Setup...");
        Interfaces::m_pCvar->UnlockHiddenCVars();

        Utilities::m_EventListener.Setup({ X("round_start"), X("add_bullet_hit_marker"), X("bullet_impact"), X("player_hurt"), X("player_death"), X("weapon_fire"), X("vote_cast"), X("vote_started"), X("item_purchase"), X("bomb_defused"), X("bomb_begindefuse"), X("bomb_planted"), X("bomb_beginplant") });

        VexLog("[vexium] ALL DONE — cheat loaded successfully");
    }
    catch (const std::exception& ex)
    {
        VexLog("[vexium] EXCEPTION:");
        VexLog(ex.what());

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

    Utilities::m_EventListener.Destroy();

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
        VexLog("[vexium] DllMain DLL_PROCESS_ATTACH");

        LI_FN(DisableThreadLibraryCalls)(hModule);

        Globals::m_hDll = hModule;

        VexLog("[vexium] CreateThread -> OnDllAttach");
        if (HANDLE hThread = LI_FN(CreateThread)(nullptr, 0U, OnDllAttach, hModule, 0UL, nullptr); hThread != nullptr)
            LI_FN(CloseHandle)(hThread);

        return TRUE;
    }

    return FALSE;
}
