#include "Precompiled.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
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

        VexLog("Skipping Interfaces::Setup (unsafe pattern scans)");
        // Bypass Interfaces::Setup entirely — we build a dummy IDXGISwapChain
        // via D3D11CreateDeviceAndSwapChain and hook its vtable[8]/vtable[13].
        // This works on ANY CS2 version — no pattern scans.
        {
            HWND hDummyWnd = CreateWindowExA(0, "STATIC", "dummy",
                WS_OVERLAPPEDWINDOW, 0, 0, 100, 100,
                nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
            if (!hDummyWnd)
            {
                VexLog("CreateWindowExA failed");
                __leave;
            }

            DXGI_SWAP_CHAIN_DESC scd{};
            scd.BufferCount = 1;
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            scd.OutputWindow = hDummyWnd;
            scd.SampleDesc.Count = 1;
            scd.Windowed = TRUE;
            scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            D3D_FEATURE_LEVEL fl[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
            IDXGISwapChain* pDummySwap = nullptr;
            ID3D11Device*   pDummyDev  = nullptr;
            ID3D11DeviceContext* pDummyCtx = nullptr;
            D3D_FEATURE_LEVEL flOut;

            HRESULT hr = D3D11CreateDeviceAndSwapChain(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                0, fl, ARRAYSIZE(fl), D3D11_SDK_VERSION,
                &scd, &pDummySwap, &pDummyDev, &flOut, &pDummyCtx);

            if (FAILED(hr) || !pDummySwap)
            {
                VexLogFmt("D3D11CreateDeviceAndSwapChain failed hr=0x%08X", hr);
                DestroyWindow(hDummyWnd);
                __leave;
            }
            VexLog("dummy swapchain created OK");

            // Build a fake ISwapChainDx11 shim. Layout: 0x170 padding + IDXGISwapChain*
            static uint8_t s_fakeShim[0x180]{};
            *reinterpret_cast<IDXGISwapChain**>(s_fakeShim + 0x170) = pDummySwap;
            Interfaces::m_pSwapChain = reinterpret_cast<ISwapChainDx11*>(s_fakeShim);

            // Clean up dummy device/context — swapchain vtable is what we need
            if (pDummyCtx) pDummyCtx->Release();
            if (pDummyDev) pDummyDev->Release();
            // Don't destroy hDummyWnd yet — swapchain may hold reference
            // Don't release pDummySwap — we need its vtable alive

            VexLog("Interfaces::m_pSwapChain shim OK");
        }

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
