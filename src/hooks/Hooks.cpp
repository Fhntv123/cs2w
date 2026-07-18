#include "../Precompiled.h"
#include "Hooks.h"
#include "../features/threading/Threader.h"
#include "../features/misc/ShotHandler.h"
#include "../features/misc/FSN handler.h"

#include <windows.h>
#include <psapi.h>
#include <iostream>
#include "../features/misc/Miscellaneous.h"
#include "../gui/new/new_menu.h"

using namespace NRagebot;

void* GetModuleBaseAddress(void* address)
{
	HMODULE hMods[1024];
	HANDLE hProcess = GetCurrentProcess();
	DWORD cbNeeded;

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			MODULEINFO modInfo;
			if (GetModuleInformation(hProcess, hMods[i], &modInfo, sizeof(modInfo)))
			{
				if ((uintptr_t)address >= (uintptr_t)modInfo.lpBaseOfDll &&
					(uintptr_t)address < (uintptr_t)modInfo.lpBaseOfDll + modInfo.SizeOfImage)
				{
					return modInfo.lpBaseOfDll;
				}
			}
		}
	}
	return nullptr;
}


bool Hooks::Setup()
{
	if (MH_Initialize() != MH_OK)
		throw std::runtime_error(X("failed initialize minhook"));

	// MENU-ONLY: Present + ResizeBuffers only
	SubscribeHook(Detours::Present.Init(&hkPresent,
		Memory::GetVFunc(Interfaces::m_pSwapChain->pDXGISwapChain, 8U)));

	SubscribeHook(Detours::ResizeBuffers.Init(&hkResizeBuffers,
		Memory::GetVFunc(Interfaces::m_pSwapChain->pDXGISwapChain, 13U)));

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		throw std::runtime_error(X("failed enable hooks"));

	return true;
}

void Hooks::Destroy()
{
	for (auto& it : vecDetours)
		it.Restore();

	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	MH_Uninitialize();
}

void* Hooks::hkDrawFirstPersonLegs(void* a1, void* a2, void* a3, void* a4, void* a5)
{
	static auto og = Detours::FirstPersonLegs.GetOriginal< decltype(&hkDrawFirstPersonLegs) >();

	if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(ERemovals::REMOVE_LOCAL_LEGS))
		return nullptr;

	return og(a1, a2, a3, a4, a5);
}

void Hooks::hkHandleTeamIntro(void* a1, void* a2, bool* pbTeamIntroEnd)
{
	static const auto oOriginal = Detours::HandleTeamIntro.GetOriginal<decltype(&hkHandleTeamIntro)>();

	if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(ERemovals::REMOVE_TEAM_INTRO))
		return;

	return oOriginal(a1, a2, pbTeamIntroEnd);
}

float Hooks::hkPrepareSceneMaterial(CMaterial2* pMaterial, void* pData, float flNull)
{
	static const auto oOriginal = Detours::PrepareSceneMaterial.GetOriginal<decltype(&hkPrepareSceneMaterial)>();

	static int currIndexBrightness = 0;
	static int currIndexRenderOnly = 0;
	static float lastBrightness = 0.f;
	static float lastBrightnessRenderOnly = 0.f;
	static CMaterial2* lastMaterial = 0;
	uint64_t pParameters = *(uint64_t*)((uint64_t)pMaterial + 0x18);

	if (pMaterial != lastMaterial)
	{
		bool found1 = false, found2 = false;

		uint32_t count = *(uint32_t*)((uint64_t)pMaterial + 0x10);

		for (int i = 0; i < count; i++)
		{
			uint64_t pParameter = pParameters + (i * 0x40);

			if (strcmp(*(const char**)(pParameter + 0x28), "g_flBrightnessExposureBias") == 0)
			{
				currIndexBrightness = i;
				lastBrightness = *(float*)(pParameter);
				if (found2)
					break;
				found1 = true;
			}
			else if (strcmp(*(const char**)(pParameter + 0x28), "g_flRenderOnlyExposureBias") == 0)
			{
				currIndexRenderOnly = i;
				lastBrightnessRenderOnly = *(float*)(pParameter);
				if (found1)
					break;
				found2 = true;
			}
		}

		lastMaterial = pMaterial;
	}

	if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(REMOVE_FOG_BRIGHTNESS))
	{
		*(float*)(pParameters + (currIndexBrightness * 0x40)) = -6.f;
		*(float*)(pParameters + (currIndexRenderOnly * 0x40)) = -6.f;
	}
	else
	{
		*(float*)(pParameters + (currIndexBrightness * 0x40)) = lastBrightness;
		*(float*)(pParameters + (currIndexRenderOnly * 0x40)) = lastBrightnessRenderOnly;
	}


	return oOriginal(pMaterial, pData, flNull);
}

bool Hooks::hkDrawCrosshair(void* a1) {
	static auto og = Detours::DrawCrosshair.GetOriginal< decltype(&hkDrawCrosshair) >();

	if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(ERemovals::REMOVE_CROSSHAIR))
		return false;

	return og(a1);
}

static __int64 Hooks::hkCAnimatableSceneObjectDesc__FindOrAllocateTransforms(__int64 CAnimatableSceneObjectDesc, cs2::animatable_scene_object* animatable_scene_object, cs2::mesh_instance* CMeshInstance_Imp, __int64 a4, __int64 a5, int a6, const char* a7, __int64* a8, __int64* a9)
{
	const auto og = Detours::CAnimatableSceneObjectDesc__FindOrAllocateTransforms.GetOriginal< decltype(&hkCAnimatableSceneObjectDesc__FindOrAllocateTransforms) >();

	static std::shared_mutex mutex{ };
	std::unique_lock lock{ mutex };

	C_BaseEntity* entity{ };
	if (!animatable_scene_object->m_owner_handle.IsValid() || !(entity = animatable_scene_object->m_owner_handle.Get()) || !entity->IsPlayerPawn())
		return og(CAnimatableSceneObjectDesc, animatable_scene_object, CMeshInstance_Imp, a4, a5, a6, a7, a8, a9);

	CMeshInstance_Imp->m_transforms_cache->m_mutex = 0;

	auto ret = og(CAnimatableSceneObjectDesc, animatable_scene_object, CMeshInstance_Imp, a4, a5, a6, a7, a8, a9);

	CMeshInstance_Imp->m_transforms_cache->m_mutex = 0;

	return ret;
}

bool FASTCALL Hooks::hkDrawOverHead(C_CSPlayerPawn* a1) {

	static auto og = Detours::DrawOverHead.GetOriginal<decltype(&hkDrawOverHead)>();

	if (!a1) return og(a1);

	if (a1 == Globals::m_pLocalPlayerPawn) {
		if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(ERemovals::REMOVE_LOCAL_OVERHEAD_INFO))
			return false;

		return true;
	}

	if (!a1->IsEnemy(Globals::m_pLocalPlayerPawn) && Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(ERemovals::REMOVE_TEAM_OVERHEAD_INFO))
		return false;

	return true;
}

struct ReportHit_t
{
	MEM_PAD(0x18);
	Vector m_vecPosition;
	float m_flTime;
};

_QWORD* FASTCALL Hooks::hkReportHit(__int64 a1)
{
	const auto oOriginal = Detours::ReportHit.GetOriginal< decltype(&hkReportHit) >();

	if (!Interfaces::m_pGlobalVariables)
		return oOriginal(a1);

	const ReportHit_t* pReportHit = (ReportHit_t*)a1;

	CShotHandler::PlayerHurtRecord_t objHurtRec;
	objHurtRec.m_flTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;
	objHurtRec.m_vecImpactPosition = pReportHit->m_vecPosition;

	g_ShotHandler->m_HitRecords.push_back(objHurtRec);

	return oOriginal(a1);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
long Hooks::hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Input::Process(uMsg, wParam, lParam);

	bool bMouseOverrideActive = Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_bMouseOverride));
	Input::m_bMouseOverrideActive = bMouseOverrideActive;

	if (bMouseOverrideActive && !Gui::m_bOpen)
	{
		if (uMsg == WM_MOUSEMOVE)
		{
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);

			POINT topLeft = { 0, 0 };
			POINT bottomRight = { clientRect.right, clientRect.bottom };

			ClientToScreen(hWnd, &topLeft);
			ClientToScreen(hWnd, &bottomRight);

			RECT clipRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
			ClipCursor(&clipRect);

			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
			return true;
		}
	}
	else
	{
		ClipCursor(nullptr);
	}

	if (Config::i(g_Variables.m_Gui.m_iMenuKey) > 0 && Input::IsKeyReleased(Config::i(g_Variables.m_Gui.m_iMenuKey)))
	{
		LocalPlayerData::m_bRageNeedsConfigUpdate = true;
		Gui::m_bOpen = !Gui::m_bOpen;

		const auto oIsRelativeMouseMode = Detours::IsRelativeMouseMode.GetOriginal<decltype(&hkIsRelativeMouseMode)>();
		if (oIsRelativeMouseMode)
			oIsRelativeMouseMode(Interfaces::m_pInputSystem, (Gui::m_bOpen || Input::m_bMouseOverrideActive) ? false : Gui::m_bInputActive);
	}

	if (Gui::m_bOpen)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

		if (wParam != 'R' && wParam != '1' && wParam != '2' && wParam != '3' && wParam != '4' && wParam != '5' && wParam != 'W' && wParam != 'A' && wParam != 'S' && wParam != 'D' && wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_TAB && wParam != VK_SPACE || ImGui::GetIO().WantTextInput)
			return true;

		if (Input::IsKeyDown(VK_ESCAPE))
			Gui::m_bOpen = false;
	}

	if ((Gui::m_bOpen || bMouseOverrideActive) && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE))
		return false;

	return static_cast<long>(CallWindowProcW(Input::m_pOldWndProc, hWnd, uMsg, wParam, lParam));
}

HRESULT WINAPI Hooks::hkPresent(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags)
{
	const auto oPresent = Detours::Present.GetOriginal< decltype(&hkPresent) >();

	// Init Draw here — guaranteed DX11 is ready at this point
	if (!Draw::m_bInitialized && Interfaces::m_pDevice != nullptr && Interfaces::m_pDeviceContext != nullptr)
		Draw::Setup(Interfaces::m_pDevice, Interfaces::m_pDeviceContext);

	if (Interfaces::m_pRenderTargetView == nullptr)
		Interfaces::CreateRenderTarget();

	if (Interfaces::m_pRenderTargetView != nullptr && Interfaces::m_pDeviceContext != nullptr)
		Interfaces::m_pDeviceContext->OMSetRenderTargets(1, &Interfaces::m_pRenderTargetView, nullptr);

	SEH_START

	if (Draw::m_bInitialized)
	{
		if (!Gui::m_bInitialized)
			Gui::Initialize();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// --- MENU-ONLY MODE: only render the menu, no features ---
		menu->render();

		/*
		// Disabled until menu works without crash:
		g_Movement->RenderAutoRetreat();
		g_AntiAim->DrawMouseOverrideIndicator();
		g_Overlay->Watermark();
		g_Overlay->SpectatorsList();
		g_Overlay->Indicators();
		Gui::DrawHitLogs();
		PlayerESP::DrawDamageIndicators();
		PlayerESP::HitMarker();
		g_WorldModulation->Exposure(Globals::m_pLocalPlayerPawn);
		g_ShotHandler->OnPresent();
		Scope::Run();
		*/

		Draw::pBackgroundDrawList = ImGui::GetBackgroundDrawList();
		Draw::RenderDrawData(Draw::pBackgroundDrawList);
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	SEH_END

	return oPresent(Interfaces::m_pSwapChain->pDXGISwapChain, uSyncInterval, uFlags);
}

HRESULT FASTCALL Hooks::hkResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags)
{
	const auto oResizeBuffer = Detours::ResizeBuffers.GetOriginal< decltype(&hkResizeBuffers) >();

	HRESULT hResult = oResizeBuffer(pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags); //ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pGameOverlayGadget, oResizeBuffer, pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags);
	if (SUCCEEDED(hResult))
		Interfaces::CreateRenderTarget();

	return hResult;
}

HRESULT WINAPI Hooks::hkCreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	const auto oCreateSwapChain = Detours::CreateSwapChain.GetOriginal< decltype(&hkCreateSwapChain) >();
	Interfaces::DestroyRenderTarget();
	return oCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain); // TODO: Spoof
}

class CSubtickBackup
{
public:
	google::protobuf::RepeatedPtrField< std::string > m_LastSubtickMoves;

	void CreateBackup(CUserCmd* pCmd)
	{
		m_LastSubtickMoves.Clear();
		for (int i = 0; i < pCmd->m_csgoUserCmd.mutable_base()->subtick_moves_size(); i++)
		{
			auto& objSubtickMoveStep = pCmd->m_csgoUserCmd.mutable_base()->subtick_moves().at(i);
			objSubtickMoveStep.AppendToString(m_LastSubtickMoves.Add());
		}
	}

	void RestoreFromBackup(CUserCmd* pCmd)
	{
		if (m_LastSubtickMoves.size())
		{
			pCmd->m_csgoUserCmd.mutable_base()->clear_subtick_moves();
			for (const auto& strLasSubtickMove : m_LastSubtickMoves)
			{
				if (const auto pSubtickMoveStep = Interfaces::m_pInput->CreateNewSubTickMoveStep(pCmd->m_csgoUserCmd.mutable_base()->mutable_subtick_moves()))
				{
					pSubtickMoveStep->MergeFromString(strLasSubtickMove);

					pCmd->m_csgoUserCmd.mutable_base()->mutable_subtick_moves()->AddAllocated(pSubtickMoveStep);
				}
			}
		}
	}
};
const inline auto g_SubtickBackup = std::make_unique<CSubtickBackup>();

int s_nOldSeqNum = 0;

	void Hooks::hkCreateMovePrePrediction(CCSGOInput* pInput, int nSlot, bool bActive)
	{
		static auto oCreateMove = Detours::CreateMovePrePrediction.GetOriginal< decltype(&hkCreateMovePrePrediction) >();

		oCreateMove(pInput, nSlot, bActive);

		if (!LocalPlayerData::Update())
			return;

		if (g_Entities->m_vecEntities.empty())
			g_Entities->ForceUpdateEntityList();

	g_Rage->HandleKeybinds();

	g_Rage->SetConfig();

	//Doubletap::Process();

	//if (Globals::m_pCmd->m_nButtons.m_nValue & IN_ATTACK)
	//	Doubletap::SetupInputHistory();

	g_Prediction->Begin(Globals::m_pCmd);
	{
		g_Movement->BunnyHop();

		g_Movement->AutoRetreat(Globals::m_pCmd, Globals::m_pLocalPlayerPawn);

		g_Movement->StoreCameraAngles();

		g_AntiAim->OnCreateMove();

		g_Movement->StoreModelAngles();

		g_Movement->HandleDirectionalStrafe();

		g_Movement->EdgeJump(Globals::m_pCmd);

		g_Rage->ConsiderAttack();

		g_Rage->OnCreateMove();

		//g_Movement->JumpBug();

		g_Misc->AutoPistol();

		//g_Misc->StraightThrow(pInput->GetViewAngles());

		g_Misc->AccurateWalk(Globals::m_pCmd);

		g_Movement->HandleMoveStop(g_Rage->m_Data.m_bWants2DMoveHalt);

		// Handle ragdoll gravity
		if (Config::b(g_Variables.m_Visuals.m_bRagdollGravity))
		{
			constexpr float kMin = -1000.0f;
			constexpr float kMax = -1.0f;
			float gravityScale = Config::f(g_Variables.m_Visuals.m_flRagdollGravityScale);

			if (gravityScale > kMax)
				gravityScale = kMax;
			else if (gravityScale < kMin)
				gravityScale = kMin;

			// Set ragdoll gravity using convar
			Interfaces::m_pCvar->Find(FNV1A::Hash("ragdoll_gravity_scale"))->m_value.m_fl = gravityScale;
		}
	}
	g_Prediction->End(Globals::m_pCmd);

	
	if (Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled))
	{
		
		if (Config::b(g_Variables.m_Ragebot.m_bClientNoSpread))
		{
			Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_cheats"))->m_value.m_i32 = 1;
			Interfaces::m_pCvar->Find(FNV1A::HashConst("weapon_accuracy_nospread"))->m_value.m_i32 = 1;
		}
	}

	g_Rage->OverrideInputHistory();

	Interfaces::m_pCvar->Find(FNV1A::Hash("snd_toolvolume"))->m_value.m_fl = Config::f(g_Variables.m_Misc.m_flHitsoundVolume);

	g_Movement->MovementCorrection();

	if (s_nOldSeqNum == Globals::m_pCmd->m_nCommandNumber)
	{
		// Skip subtick backup restore when nospread is enabled to prevent protobuf crash
		if (!Config::b(g_Variables.m_Ragebot.m_bClientNoSpread))
			g_SubtickBackup->RestoreFromBackup(Globals::m_pCmd);
	}

	// Skip subtick backup creation when nospread is enabled to prevent protobuf crash
	if (!Config::b(g_Variables.m_Ragebot.m_bClientNoSpread))
		g_SubtickBackup->CreateBackup(Globals::m_pCmd);

	s_nOldSeqNum = Globals::m_pCmd->m_nCommandNumber;
}

void* FASTCALL Hooks::hkInputParser(CCSInputMessage* pInputMessage, CSGOInputHistoryEntryPB* pInputHistory, bool bSomeBool, TimeStamp_t a4, TimeStamp_t a5, C_CSPlayerPawn* pLocalPlayer)
{
	static auto oInputParser = Detours::InputParser.GetOriginal<decltype(&hkInputParser)>();

	auto res = oInputParser(pInputMessage, pInputHistory, bSomeBool, a4, a5, pLocalPlayer);

	if (!Globals::m_pCmd)
		return res;

	if (!Globals::m_pBaseCmd)
		return res;

	return res;
}

bool FASTCALL Hooks::hkCreateMovePostPrediction(CCSGOInput* pInput, unsigned int nSlot, CUserCmd* pCmd)
{
	const auto oCreateMove = Detours::CreateMovePostPrediction.GetOriginal< decltype(&hkCreateMovePostPrediction) >();

	const bool bResult = ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oCreateMove, pInput, nSlot, pCmd);

	return bResult;
}

bool FASTCALL Hooks::hkMouseInputEnabled(void* pRcx)
{
	const auto oMouseInputEnabled = Detours::MouseInputEnabled.GetOriginal< decltype(&hkMouseInputEnabled) >();

	bool bMouseOverrideActive = Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_bMouseOverride));

	return (Gui::m_bOpen || bMouseOverrideActive) ? false : ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oMouseInputEnabled, pRcx);
}

void FASTCALL Hooks::hkFrameStageNotify(void* pRcx, EClientFrameStage nFrameStage)
{
	static auto oFrameStageNotify = Detours::FrameStageNotify.GetOriginal< decltype(&hkFrameStageNotify) >();
	g_FSN->OnPreOriginal(nFrameStage);
	ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oFrameStageNotify, pRcx, nFrameStage);
	g_FSN->OnPostOriginal(nFrameStage);
}

void STDCALL Hooks::hkLevelInit(IClientMode* pClientModeShared, const char* szNewMap)
{
	const auto oLevelInit = Detours::LevelInit.GetOriginal< decltype(&hkLevelInit) >();

	if (Interfaces::m_pGlobalVariables == nullptr)
		Interfaces::m_pGlobalVariables = *reinterpret_cast<IGlobalVariables**>(Memory::ResolveRelativeAddress(Memory::FindPattern(CLIENT_DLL, X("48 89 15 ? ? ? ? 48 89 42")), 0x3, 0x7));

	Interfaces::m_pGameRules = *reinterpret_cast<C_CSGameRules**>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("48 8B 1D ? ? ? ? 48 8D 54 24 ? 0F 28 D0 48 8D 4C 24 ?")), 0x3));
	Interfaces::m_pParticleManager = Interfaces::GetGameParticleManager();

	//if (FNV1A::HashConst("C_GradientFog")) {
	//	Interfaces::m_pGameResourceService->pGameEntitySystem->CreateEntityByClassName<c_gradient_fog>("C_GradientFog");
	//}

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oLevelInit, pClientModeShared, szNewMap);
}

void STDCALL Hooks::hkLevelShutDown(IClientMode* pClientModeShared)
{
	static auto oLevelShutdown = Detours::LevelShutDown.GetOriginal< decltype(&hkLevelShutDown) >();

	Interfaces::m_pGlobalVariables = nullptr;

	Globals::m_pLocalPlayerController = nullptr;
	Globals::m_pLocalPlayerPawn = nullptr;

	LocalPlayerData::Reset();

	//g_WorldModulation->vecSkyboxes.clear();

	//g_WorldModulation->UpdateSkyboxes();
	//g_WorldModulation->LoadDefaultSkyboxes();

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oLevelShutdown, pClientModeShared);
}

void FASTCALL Hooks::hkOverrideView(void* pClientModeCSNormal, CViewSetup* pSetup)
{
	const auto oOverrideView = Detours::OverrideView.GetOriginal< decltype(&hkOverrideView) >();

	if (!Globals::m_pLocalPlayerController)
		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oOverrideView, pClientModeCSNormal, pSetup);

	if (!Globals::m_pLocalPlayerPawn/* && Globals::m_pLocalPlayerPawn->m_iHealth() > 0*/)
		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oOverrideView, pClientModeCSNormal, pSetup);

	if (!Interfaces::m_pEngine->IsConnected() || !Interfaces::m_pEngine->IsInGame() && !Interfaces::m_pGameRules->m_bTeamIntroPeriod())
		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oOverrideView, pClientModeCSNormal, pSetup);

	oOverrideView(pClientModeCSNormal, pSetup);

	static float flOriginalAspectRatio = pSetup->m_flAspectRatio;

	int iAspectRatioValue = Config::i(g_Variables.m_WorldEffects.m_iAspectRatio);
	if (iAspectRatioValue != 0)
	{
		pSetup->m_flAspectRatio = static_cast<float>(iAspectRatioValue) / 10.f;
		pSetup->m_nSomeFlags |= 2;
	}
	else
	{
		pSetup->m_flAspectRatio = flOriginalAspectRatio;
		pSetup->m_nSomeFlags &= ~2;
	}

	if ((Globals::m_pLocalPlayerPawn->m_iHealth() >= 1) && Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson) && Input::HandleInput(Config::kb(g_Variables.m_Visuals.m_iThirdPersonKeybind)))
	{
		Vector vecFinalEyePos = Globals::m_pLocalPlayerPawn->GetEyePosition();
		if (Config::b(g_Variables.m_Ragebot.m_bDuckPeekAssist) && Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_kbDuckPeekAssist)) && (Globals::m_pCmd->m_nButtons.m_nValue & IN_DUCK))
		{
			vecFinalEyePos.z = Globals::m_pLocalPlayerPawn->GetEyePosition().z + (18 * Globals::m_pLocalPlayerPawn->m_pMovementServices()->m_flDuckAmount());
		}

		QAngle angAdjustedCameraViewAngle = Interfaces::m_pInput->GetViewAngles();/*Interfaces::m_pInput->m_CurrentMoveData.m_vecViewAngle;*/
		angAdjustedCameraViewAngle.x = -angAdjustedCameraViewAngle.x;
		pSetup->m_vecOrigin = Math::CalculateCameraPosition(vecFinalEyePos, -(float)Config::i(g_Variables.m_Visuals.m_iThirdPersonDistance), angAdjustedCameraViewAngle); // calculates the camera position based on the view angles, distance and anchor point

		Ray_t ray{};
		GameTrace_t trace{};
		TraceFilter_t filter{ 0x1C3003, Globals::m_pLocalPlayerPawn, NULL, 7 };//4
		if (Interfaces::m_pVPhys2World->TraceShape(&ray, vecFinalEyePos, pSetup->m_vecOrigin, &filter, &trace)) {
			if (trace.DidHitWorld())
			{
				pSetup->m_vecOrigin = trace.m_vecPosition;
			}
		}

		pSetup->m_angView = Math::CalcAngle(pSetup->m_vecOrigin, vecFinalEyePos).Normalize();
	}

	if (Config::i(g_Variables.m_Visuals.m_iRemoveRecoil) == RECOIL_ALL)
		pSetup->m_angView = Interfaces::m_pInput->GetViewAngles();

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oOverrideView, pClientModeCSNormal, pSetup);
}

void* FASTCALL Hooks::hkOnAddEntity(void* pRcx, CEntityInstance* pInstance, CBaseHandle uHandle)
{
	const auto oOnAddEntity = Detours::OnAddEntity.GetOriginal< decltype(&hkOnAddEntity) >();
	g_Entities->OnAddEntity(pInstance, uHandle);
	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pEngineGadet, oOnAddEntity, pRcx, pInstance, uHandle);
}

void* FASTCALL Hooks::hkOnRemoveEntity(void* pRcx, CEntityInstance* pInstance, CBaseHandle uHandle)
{
	const auto oOnRemoveEntity = Detours::OnRemoveEntity.GetOriginal< decltype(&hkOnRemoveEntity) >();
	g_Entities->OnRemoveEntity(pInstance, uHandle);
	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pEngineGadet, oOnRemoveEntity, pRcx, pInstance, uHandle);
}

ViewMatrix_t* FASTCALL Hooks::hkGetMatrixForView(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels)
{
	const auto oGetMatrixForView = Detours::GetMatrixForView.GetOriginal< decltype(&hkGetMatrixForView) >();
	ViewMatrix_t* matResult = ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oGetMatrixForView, pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);
	// get view matrix
	Globals::m_ViewMatrix = *pOutWorldToProjection;

	return matResult;
}

bool FASTCALL Hooks::hkIsLoadoutAllowed()
{
	const auto oIsLoadoutAllowed = Detours::IsLoadoutAllowed.GetOriginal<decltype(&hkIsLoadoutAllowed)>();

	if (Config::b(g_Variables.m_Misc.m_bUnlockLoadout))
		return true;

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oIsLoadoutAllowed);
}

cs2::mesh_instance* Hooks::hkCreateMeshInstance(__int64 CMeshSystem, __int64 CBaseSceneObjectDesc, int maybe_spawn_tick, __int64 model_instance_thing, __int16 zero, __int64 null_mem_addr, __int64 CSceneObject, __int64 minus_one, char minus_one2, __int64 zero_val)
{
	const auto oOriginal = Detours::CreateMeshInstance.GetOriginal<decltype(&hkCreateMeshInstance)>();

	cs2::mesh_instance* pReturn = oOriginal(CMeshSystem, CBaseSceneObjectDesc, maybe_spawn_tick, model_instance_thing, zero, null_mem_addr, CSceneObject, minus_one, minus_one2, zero_val);

	cs2::unk_mesh_cache* cache = *(cs2::unk_mesh_cache**)model_instance_thing;
	std::string str = cache->m_model_imp->m_path;
	if (str.find("characters/") != std::string::npos) {

		for (int i = 0; i < g_Chams->m_mapMeshInstances[pReturn].size(); i++) {
			g_Chams->m_mapMeshInstances[pReturn][i] = oOriginal(CMeshSystem, CBaseSceneObjectDesc, maybe_spawn_tick, model_instance_thing, zero, null_mem_addr, CSceneObject, minus_one, minus_one2, zero_val);
		}

	}

	return pReturn;
}

void* __fastcall Hooks::hkFreeMeshInstance(void* a1, char a2)
{
	const auto oOriginal = Detours::FreeMeshInstance.GetOriginal<decltype(&hkFreeMeshInstance)>();
	auto* original_instance = static_cast<cs2::mesh_instance*>(a1);

	auto it = g_Chams->m_mapMeshInstances.find(original_instance);
	if (it != g_Chams->m_mapMeshInstances.end())
	{
		for (auto* instance : it->second)
		{
			oOriginal(instance, a2);
		}

		g_Chams->m_mapMeshInstances.erase(it);
	}
	return oOriginal(a1, a2);
}

void FASTCALL Hooks::hkQuantizeMovement(uintptr_t a1, uintptr_t a2)
{
	const auto oQuantizeMovement = Detours::QuantizeMovement.GetOriginal<decltype(&hkQuantizeMovement)>();

	*(float*)(a2 + 0x0) = Globals::m_pBaseCmd->forwardmove(); // forward_move
	*(float*)(a2 + 0x0) = Globals::m_pBaseCmd->leftmove();// left_move
	*(float*)(a2 + 0x0) = Globals::m_pBaseCmd->upmove();// up_move

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oQuantizeMovement, a1, a2);
}

int FASTCALL Hooks::hkSendMessageGC(ISteamGameCoordinator* thisptr, std::uint32_t uMsgType, const void* pData, std::uint32_t uData)
{
	const auto oSendMessage = Detours::SendMessageGC.GetOriginal<decltype(&hkSendMessageGC)>();
	const std::uint32_t uMessageType = (uMsgType & 0x7FFFFFFF);

	if (uMessageType == 9144)
	{
		return k_EGCResultOK;
	}

	const int iStatus = ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oSendMessage, thisptr, uMsgType, pData, uData);

	return iStatus;
}

int FASTCALL Hooks::hkRetrieveMessageGC(ISteamGameCoordinator* thisptr, std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize)
{
	const auto oRetrieveMessage = Detours::RetrieveMessageGC.GetOriginal<decltype(&hkRetrieveMessageGC)>();
	const int iStatus = ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oRetrieveMessage, thisptr, puMsgType, pDest, uDest, puMsgSize);

	if (iStatus == k_EGCResultNotLoggedOn)
		return k_EGCResultOK;

	if (iStatus != k_EGCResultOK)
		return iStatus;

	std::uint32_t uMessageType = (*puMsgType & 0x7FFFFFFF);
#ifdef DEBUG_CONSOLE
	Logging::PushConsoleColor(FOREGROUND_INTENSE_GREEN | FOREGROUND_RED);
	Logging::Print(X("[->] Message received from GC {:d}!"), uMessageType);
	Logging::PopConsoleColor();
#endif

	return iStatus;
}

__int64 __fastcall Hooks::hkPanoramaEvent(void* pUnk, const char* szEventName, void* pUnk1, float flUnk)
{
	const auto oPanoramaEvent = Detours::PanoramaEvent.GetOriginal< decltype(&hkPanoramaEvent) >();

	if (FNV1A::Hash(szEventName) == FNV1A::HashConst("popup_accept_match_found"))
	{
		if (Config::b(g_Variables.m_Misc.m_bAutoAccept))
		{
			Utilities::SetLocalPlayerReady();
			Utilities::FlashWindow(Input::m_hWindow);
		}
	}

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oPanoramaEvent, pUnk, szEventName, pUnk1, flUnk);
}

void* FASTCALL Hooks::hkCVarGetValue(void* pRcx, const char* szCVar, int iDefaultValue)
{
	const auto oCVarGetValue = Detours::CVarGetValue.GetOriginal<decltype(&hkCVarGetValue)>();
	static std::uint8_t* pCAM_ThinkReturn = Memory::FindPattern(CLIENT_DLL, X("48 8B 01 BA 04 00 00 00"));
	static bool bWantedValue = true;

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oCVarGetValue, pRcx, szCVar, iDefaultValue);
}

void Hooks::DrawAggregateSceneObject(void* a1, void* a2, c_aggregate_object_array* a3) {
	static auto original = Detours::DrawAggregateSceneObject.GetOriginal< decltype(&DrawAggregateSceneObject) >();
	original(a1, a2, a3);

	if (!a3->data || !Interfaces::m_pSceneSystem || !Interfaces::m_pSceneSystem->light_data_queue || !Interfaces::m_pSceneSystem->light_data_queue->light_data)
		return;

	for (int i = 0; i < a3->data->count; i++)
	{
		int index = (a3->data->index + i) << 5;
		*reinterpret_cast<Color*>(reinterpret_cast<uintptr_t>(Interfaces::m_pSceneSystem->light_data_queue->light_data) + index) = Config::c(g_Variables.m_WorldVisuals.m_colWorldColor);
	}
}

void* Hooks::hkDrawAggeregateObject(void* a1, void* a2, CDrawMesh* a3, int a4, int a5, void* a6, void* a7, void* a8)
{
	const auto oOriginal = Detours::DrawAggeregateObject.GetOriginal<decltype(&hkDrawAggeregateObject)>();

	Color color = Color(a3->m_colValue);

	constexpr auto GetModelType = [&](uint64_t uHash) {
		constexpr std::array<uint64_t, 5> arrClouds = {
			FNV1A::HashConst("materials/effects/clouds_001.vmat"), FNV1A::HashConst("materials/de_vertigo/vertigo_clouds_001.vmat"), FNV1A::HashConst("materials/models/props/de_nuke/hr_nuke/nuke_skydome_001/nuke_clouds_003.vmat"),
			FNV1A::HashConst("materials/models/props/de_nuke/hr_nuke/nuke_skydome_001/nuke_clouds_002.vmat"), FNV1A::HashConst("materials/models/props/de_nuke/hr_nuke/nuke_skydome_001/nuke_clouds_001.vmat") };

		constexpr std::array<uint64_t, 5> arrSuns = { FNV1A::HashConst("materials/sun/overlay.vmat"), FNV1A::HashConst("materials/effects/glows/sun_disc_glow_001.vmat"), FNV1A::HashConst("materials/effects/glows/sun_glow_001.vmat"), 0, 0 };

		for (int i = 0; i < 5; i++) {
			if (uHash == arrClouds[i])
				return 1;
			if (uHash == arrSuns[i])
				return 2;
		}
		return 3;
		};

	int iType = GetModelType(FNV1A::Hash(a3->m_pMaterial->GetName()));
	switch (iType) {
	case 1: { color = Config::c(g_Variables.m_WorldVisuals.m_colCloudColor); break; }
	case 2: { color = Config::c(g_Variables.m_WorldVisuals.m_colSunColor); break; }
	case 3: { color = Config::c(g_Variables.m_WorldVisuals.m_colWorldColor); break; }
	}
	for (int i = 0; i < a4; ++i)
	{
		CDrawMesh* pMaterialData = &a3[i];
		if (pMaterialData)
		{
			pMaterialData->m_colValue = Color(color.r(), color.g(), color.b(), color.a());
		}
	}

	return oOriginal(a1, a2, a3, a4, a5, a6, a7, a8);
}

CStrongHandle<CMaterial2> CreateMaterial_TEST(const char* szMaterialName, const char szVmatBuffer[])
{
	CKeyValues3* pKeyValues3 = CKeyValues3::CreateMaterialResource();
	pKeyValues3->LoadFromBuffer(szVmatBuffer);

	CStrongHandle<CMaterial2> pCustomMaterial = {};
	Functions::fnCreateMaterial(nullptr, &pCustomMaterial, szMaterialName, pKeyValues3, 0, 1);

	return pCustomMaterial;
}

void* Hooks::hkShouldUpdateSequences(void* a1, void* a2, void* a3) {

	static auto original = Detours::ShouldUpdateSequences.GetOriginal<decltype(&hkShouldUpdateSequences)>();

	const char* model_name = *reinterpret_cast<const char**>(reinterpret_cast<std::uintptr_t>(a1) + 0x8);
	if (model_name == nullptr)
		return original(a1, a2, a3);

	std::string str_model_name = std::string(model_name);
	if (str_model_name.starts_with("weapons/models/knife/"))
		*reinterpret_cast<__int64*>(reinterpret_cast<std::uintptr_t>(a3) + 0x30) = 0;

	return original(a1, a2, a3);
}

struct CSceneLayer // sizeof=0xB10
{
	_BYTE gap0[72];
	_DWORD m_nDrawFlag;
	_BYTE gap4C[1108];
	const char char4A0;
	_BYTE gap4A1[559];
	__int64(__fastcall*** pppfunc6D0)(_QWORD);
	_BYTE gap6D8[1072];
	void(__fastcall* pfuncB08)(__int64, _QWORD);
};

void Hooks::hkSortSceneObjects(__int64 a1, __int64 _pSceneLayer, __int64 _pDrawMesh, unsigned int nDataCount)
{
	static const char szInvisLatexVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
    shader = "csgo_character.vfx"
    F_DISABLE_Z_BUFFERING = 1
    F_DISABLE_Z_PREPASS = 1
    F_DISABLE_Z_WRITE = 1
    F_BLEND_MODE = 1
	F_RENDER_BACKFACES = 0

    g_vColorTint = [1.0, 1.0, 1.0, 1.0]
    g_bFogEnabled = 0
    g_flMetalness = 0.000
    g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
    g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
    g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
    g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
} )";

	static const char szLatexVmatBuffer[] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
    shader = "csgo_character.vfx"
    F_DISABLE_Z_PREPASS = 1
    F_DISABLE_Z_WRITE = 1
    F_BLEND_MODE = 1
	F_RENDER_BACKFACES = 0

    g_vColorTint = [1.0, 1.0, 1.0, 1.0]
    g_bFogEnabled = 0
    g_flMetalness = 0.000
    g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
    g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
    g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
    g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
} )";

	const auto oOriginal = Detours::SortSceneObjects.GetOriginal<decltype(&hkSortSceneObjects)>();

	CSceneLayer* pSceneLayer = (CSceneLayer*)_pSceneLayer;
	CDrawMesh* pDrawMesh = (CDrawMesh*)_pDrawMesh;

	if (pSceneLayer->pfuncB08)
		pSceneLayer->pfuncB08(_pDrawMesh, nDataCount);

	for (int i = 0; i < nDataCount; ++i)
	{
		CDrawMesh& mesh = pDrawMesh[i];

		if (!mesh.m_pSceneAnimatableObject)
			continue;

		auto pOwner = mesh.m_pSceneAnimatableObject->m_hOwnerHandle.Get();
		if (!pOwner)
			continue;

		if (pOwner == Globals::m_pLocalPlayerPawn)
			continue;

		if (mesh.m_pMaterial) {
			if (CRT::StringCompare(mesh.m_pMaterial->GetName(), X("materials/dev/glowproperty.vmat")) == 0)
				return;
		}

		bool bIsBulletBeam = pSceneLayer->m_nDrawFlag & 1 || HIBYTE(pSceneLayer->m_nDrawFlag) & 1;
		if (bIsBulletBeam)
			continue;

		SchemaClassInfoData_t* pSchemaData;
		pOwner->GetSchemaClassInfo(&pSchemaData);
		if (!pSchemaData)
			continue;

		if (FNV1A::Hash(pSchemaData->szName) == FNV1A::HashConst("C_CSPlayerPawn"))
		{
			static const auto material = g_Chams->CreateMaterial("testmat", szInvisLatexVmatBuffer);
			mesh.m_pMaterial = material;
			mesh.m_colValue = (pSceneLayer->m_nDrawFlag & 2) ? Color(255, 0, 0) : Color(0, 255, 0);;
		}
	}

	return oOriginal(a1, _pSceneLayer, _pDrawMesh, nDataCount);
}

void Hooks::hkGeneratePrimitives(CAnimatableSceneObjectDesc* pDesc, CSceneAnimatableObject* pObject, void* a3, CMeshPrimitiveOutputBuffer* pRenderBuffer)
{
	const auto oOriginal = Detours::GeneratePrimitives.GetOriginal<decltype(&hkGeneratePrimitives)>();

	if (Interfaces::m_pEngine->IsInGame() && Interfaces::m_pEngine->IsConnected())
		if (g_Chams->OnGeneratePrimitives(pDesc, pObject, a3, pRenderBuffer))
			return;

	oOriginal(pDesc, pObject, a3, pRenderBuffer);
}

void FASTCALL Hooks::hkDrawArray(CAnimatableSceneObjectDesc* pAnimatableSceneObjectDesc, void* pDx11, CDrawMesh* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	const auto oDrawObject = Detours::DrawArray.GetOriginal<decltype(&hkDrawArray)>();
	if (!Interfaces::m_pEngine->IsConnected() || !Interfaces::m_pEngine->IsInGame())
		return 	ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pSceneSystemGadget, oDrawObject, pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerPawn)
		return 	ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pSceneSystemGadget, oDrawObject, pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pSceneSystemGadget, oDrawObject, pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
}

void Hooks::hkDrawArrayLight(void* pLightBinnerGPU, CAggregateSceneObject* pAggregateSceneObject, void* pUnk)
{
	const auto oOriginal = Detours::DrawArrayLight.GetOriginal<decltype(&hkDrawArrayLight)>();

	if (!pAggregateSceneObject)
		return oOriginal(pLightBinnerGPU, pAggregateSceneObject, pUnk);

	if (!Interfaces::m_pEngine->IsInGame() || !Interfaces::m_pEngine->IsConnected())
		return oOriginal(pLightBinnerGPU, pAggregateSceneObject, pUnk);

	//pAggregateSceneObject->red = Config::c(g_Variables.m_WorldVisuals.m_colLighting).r() / 255.f;
	//pAggregateSceneObject->green = Config::c(g_Variables.m_WorldVisuals.m_colLighting).g() / 255.f;
	//pAggregateSceneObject->blue = Config::c(g_Variables.m_WorldVisuals.m_colLighting).b() / 255.f;
	//pAggregateSceneObject->alpha = Config::c(g_Variables.m_WorldVisuals.m_colLighting).a() / 255.f;

	Color cColor = Config::c(g_Variables.m_WorldVisuals.m_colLighting);

	reinterpret_cast<CAggregateSceneObject*>(pAggregateSceneObject)->red = (cColor.r() / 255.f) * (cColor.a() / 64.f);
	reinterpret_cast<CAggregateSceneObject*>(pAggregateSceneObject)->green = (cColor.g() / 255.f) * (cColor.a() / 64.f);
	reinterpret_cast<CAggregateSceneObject*>(pAggregateSceneObject)->blue = (cColor.b() / 255.f) * (cColor.a() / 64.f);

	return oOriginal(pLightBinnerGPU, pAggregateSceneObject, pUnk);
}

void Hooks::hkCreateAnimatableSceneObject(CModelState* pModelState, void* a2, void* a3)
{
	const auto oOriginal = Detours::CreateAnimatableSceneObject.GetOriginal<decltype(&hkCreateAnimatableSceneObject)>();
	oOriginal(pModelState, a2, a3);

	void* pAnimSceneObject = (void*)((__int64)pModelState + 0xd8);

	int breakpointhere = 0;
	return;
}

void Hooks::hkSkyboxDrawarray(__int64 pObjectDesc, __int64 a2, __int64 pSceneData, int iDataCount, int a5, __int64 a6, __int64 a7)
{
	static auto original = Detours::SkyboxDrawarray.GetOriginal<decltype(&hkSkyboxDrawarray)>();

	struct CSkyboxSceneObject {
		MEM_PAD(0x110);
		__int64*** m_pFaceData;
		MEM_PAD(0x30);
	};
	static_assert(sizeof(CSkyboxSceneObject) == 0x148);

	if (iDataCount > 0) {
		auto pSceneSkyboxObject = *(CSkyboxSceneObject**)(0x68 * (iDataCount)+(pSceneData)-0x50);
		if (pSceneSkyboxObject) {

			Color c = Config::c(g_Variables.m_WorldEffects.m_colSkybox);
			Vector skycol = Vector(
				static_cast<float>(c.r()) / 255.f,
				static_cast<float>(c.g()) / 255.f,
				static_cast<float>(c.b()) / 255.f
			);
			*(Vector*)((__int64)pSceneSkyboxObject + 0x128) = skycol;
		}
	}

	struct CSceneSkyboxObject {
	public:
		std::byte pad_0000[0x110]; //0x0
		void** pSkyMaterial; // 0x110
		std::byte pad_0118[0x10]; //0x118
		Color colorSky;
	};

	if (!Config::b(g_Variables.m_WorldEffects.m_bCustomSkybox))
		return original(pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7);

	CSceneSkyboxObject* pSkyBoxObject = *reinterpret_cast<CSceneSkyboxObject**>((__int64)pSceneData + 0x18);
	if (!pSkyBoxObject)
		return original(pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7);

	if (!pSkyBoxObject->pSkyMaterial)
		return original(pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7);

	if (g_WorldModulation->nSelectedSkybox == ~1U)
		return original(pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7);

	if (g_WorldModulation->vecSkyboxes.size() <= g_WorldModulation->nSelectedSkybox)
		return original(pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7);

	if (Config::b(g_Variables.m_WorldEffects.m_bCustomSkybox))
		*pSkyBoxObject->pSkyMaterial = g_WorldModulation->vecSkyboxes[g_WorldModulation->nSelectedSkybox].pSkyboxMaterial;

	original(pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7);
}

class CParticleMgrr {
private:
	void* m_pParticles = nullptr;
public:
	void SetColor(const char* szType, Color colParticle, int nIndex) {
		if (m_pParticles) {
			const auto pParticle = (__int64)m_pParticles + 0x50 + nIndex * 0x68;
			const auto pParticleParent = *(__int64*)(pParticle - 0x50);
			const auto pParticleNamePointer = *(__int64*)(pParticleParent + 0x18);

			char* szParticleName = (*(char**)(*(void***)(pParticleNamePointer + 0x8)));

			if (strstr(szParticleName, szType)) {
				*(Color*)(pParticle) = colParticle;
			}
		}
	}

	CParticleMgrr(void* pParticles) {
		m_pParticles = pParticles;
	}
};

void* Hooks::hkParticleDrawArray(void* pParticleObjectDesc, void* a2, void* a3, int nDataCount, void* a5, void* a6, void* a7)
{
	const auto pOriginal = Detours::ParticleDrawArray.GetOriginal<decltype(&hkParticleDrawArray)>();

	if (!Interfaces::m_pEngine->IsInGame() || !Interfaces::m_pEngine->IsConnected())
		return pOriginal(pParticleObjectDesc, a2, a3, nDataCount, a5, a6, a7);

	if (!Globals::m_pLocalPlayerPawn)
		return pOriginal(pParticleObjectDesc, a2, a3, nDataCount, a5, a6, a7);

	if (!a3 || nDataCount <= 0)
		return pOriginal(pParticleObjectDesc, a2, a3, nDataCount, a5, a6, a7);

	CParticleMgrr mgr(a3);
	for (int i = 0; i < nDataCount; i++) {
		if (Config::b(g_Variables.m_WorldVisuals.m_bParticleModulation)) {
			Color colInferno = Config::c(g_Variables.m_WorldVisuals.m_colParticleColor);
			mgr.SetColor(X("molotov"), colInferno, i);
			mgr.SetColor(X("incendiary"), colInferno, i);
		}

		if (Config::b(g_Variables.m_Visuals.m_bShockEffect)) {
			mgr.SetColor(X("impact_taser_bodyfx"), Config::c(g_Variables.m_Visuals.m_colShockEffect), i);
		}

		if (Config::b(g_Variables.m_WorldVisuals.m_bGrenadeModulation)) {
			mgr.SetColor(X("explosion_basic"), Config::c(g_Variables.m_WorldVisuals.m_colGrenade), i);
		}

		if (Config::b(g_Variables.m_WorldVisuals.m_bTaserStimModulation)) {
			Color colStim = Config::c(g_Variables.m_WorldVisuals.m_colTaserStim);
			mgr.SetColor(X("weapon_tracers_taser"), colStim, i);
			mgr.SetColor(X("weapon_taser_glow"), colStim, i);
			mgr.SetColor(X("weapon_taser_flash"), colStim, i);
			mgr.SetColor(X("weapon_muzzle_flash_taser"), colStim, i);
		}
	}

	return pOriginal(pParticleObjectDesc, a2, a3, nDataCount, a5, a6, a7);
}

uint64_t ValveHash(const char* szVar, int32_t iSeed) {
	return reinterpret_cast<uint64_t(__fastcall*)(const char*, int32_t)>(Memory::ResolveRelativeAddress(Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 8B D0 48 8D 0D ? ? ? ? E8 ? ? ? ? 0F 10 45 ? 69 C8"), 0x1, 0x4))(szVar, iSeed);
}

void Hooks::hkSetPostProcessVec(void* a1, unsigned int uHash, Vector4D* vVal)
{
	const auto pOriginal = Detours::SetPostProcessVec.GetOriginal<decltype(&hkSetPostProcessVec)>();
	{
		int v120 = ValveHash(X("DofRanges"), 0x3141592F);
		unsigned int v121 = (0x5BD1E995 * (v120 ^ 0x73)) ^ ((0x5BD1E995 * (v120 ^ 0x73u)) >> 13);
		unsigned int uDOFParamHash = (0x5BD1E995 * v121) ^ ((0x5BD1E995 * v121) >> 15);
		auto vValue = Vector4D(0.f, 180.f, -100.f, 2000.f);
		pOriginal(a1, uDOFParamHash, &vValue);
	}

	return pOriginal(a1, uHash, vVal);;
}

int Hooks::hkPlayGunSound(int a1, void* a2, int a3, __int64 a4, int a5, int a6, int a7)
{
	const auto oOriginal = Detours::PlayGunSound.GetOriginal<decltype(&hkPlayGunSound)>();

	return oOriginal(a1, a2, a3, a4, a5, a6, a7);
}

class EmitSound_t
{
public:
	MEM_PAD(0x8);
	const char* m_szSoundName;
	float m_flVolume;
};

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() {}

	virtual bool	IsReliable(void) const = 0;
	virtual bool	IsInitMessage(void) const = 0;

	virtual int		GetRecipientCount(void) const = 0;
	virtual int		GetRecipientIndex(int slot) const = 0;
};

_DWORD* Hooks::hkEmitSound(_DWORD* a1, __int64 a2, unsigned int iEntIndex, __int64 pEmitSound)
{
	const auto oOriginal = Detours::EmitSound.GetOriginal<decltype(&hkEmitSound)>();

	IRecipientFilter* pFilter = (IRecipientFilter*)a2;
	EmitSound_t* pESound = (EmitSound_t*)pEmitSound;

	std::cout << "-------------" << std::endl;
	std::cout << "GetRecipientCount: " << pFilter->GetRecipientCount() << std::endl;
	std::cout << "Entity Index: " << iEntIndex << std::endl;
	std::cout << "Sound Name: " << pESound->m_szSoundName << std::endl;
	std::cout << "Sound Volume: " << pESound->m_flVolume << std::endl;
	std::cout << "-------------" << std::endl;

	std::string strSoundName = pESound->m_szSoundName;

	if (strSoundName.contains("flash"))
	{
		pESound->m_szSoundName = "";
		pESound->m_flVolume = 0.f;
	}

	return oOriginal(a1, a2, iEntIndex, (__int64)pESound);
}

__int64 Hooks::hkSomeUtlSymbolFunc(__int64 pVSoundTypeManager, int a2)
{
	const auto oOriginal = Detours::SomeUtlSymbolFunc.GetOriginal<decltype(&hkSomeUtlSymbolFunc)>();

	const __int64 result = oOriginal(pVSoundTypeManager, a2);

	const char* szResult = (const char*)result;

	if (strstr(szResult, "sounds/weapons/ssg08/ssg08_01.vsnd") && Config::b(g_Variables.m_Misc.m_bDistantScoutSound))
	{
		szResult = "sounds/weapons/ssg08/distant.vsnd";
	}

	return (__int64)szResult;
}

double Hooks::hkBloodEffectHit(void* pUnk)
{
	return -1.0;
}

void Hooks::hkSomeRainShit(void* pPrecipitation, void* pLocalEntity, int nSomethingAlways0)
{
	Detours::SomeRainShit.GetOriginal<decltype(&hkSomeRainShit)>()(nullptr, nullptr, 0);
}

void FASTCALL Hooks::hkPlayVSound(const char* szSound)
{
	const auto oOriginal = Detours::PlayVSound.GetOriginal<decltype(&hkPlayVSound)>();
	Logging::Print(szSound);
}

void Hooks::hkDecreaseSpeed(void* pMovementServices, void* pUnk)
{

}

void* Hooks::hkDrawParticleChildrenSeparateFromParents(void* unkptr, void* pDrawArray)
{
	const auto pOriginal = Detours::DrawParticleChildrenSeparateFromParents.GetOriginal<decltype(&hkDrawParticleChildrenSeparateFromParents)>();

	return pOriginal(unkptr, pDrawArray);
}

CParticleColor Hooks::hkGetParticleColor(CParticleColor pOutputColor, unsigned __int8* pOriginalParticleColor)
{
	const auto pOriginal = Detours::GetParticleColor.GetOriginal<decltype(&hkGetParticleColor)>();

	if (!Config::b(g_Variables.m_WorldVisuals.m_bParticleModulation))
		return pOriginal(pOutputColor, pOriginalParticleColor);

	return pOriginal(pOutputColor, pOriginalParticleColor);
}

void* Hooks::hkAODrawArray(void* pAmbientOcclusionProxySceneObjectDesc, void* a2, void* a3, int nDataCount, void* a5, void* a6, void* a7)
{
	const auto pOriginal = Detours::AODrawArray.GetOriginal<decltype(&hkAODrawArray)>();

	if (!Config::b(g_Variables.m_WorldVisuals.m_bParticleModulation))
		return pOriginal(pAmbientOcclusionProxySceneObjectDesc, a2, a3, nDataCount, a5, a6, a7);

	if (!a3 || nDataCount <= 0)
		return pOriginal(pAmbientOcclusionProxySceneObjectDesc, a2, a3, nDataCount, a5, a6, a7);

	int objectCount = nDataCount;

	for (int i = 0; i < objectCount; ++i)
	{
		uintptr_t objectData = reinterpret_cast<uintptr_t>(a3) + i * 104;
	}

	return pOriginal(pAmbientOcclusionProxySceneObjectDesc, a2, a3, nDataCount, a5, a6, a7);
}

void* FASTCALL Hooks::hkFlashOverlay(__int64 a1, int a2, __int64* a3, __int64 a4, float a5[4])
{
	const auto oFlashOverlay = Detours::FlashOverlay.GetOriginal<decltype(&hkFlashOverlay)>();

	if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(ERemovals::REMOVE_FLASH))
		return nullptr;

	return oFlashOverlay(a1, a2, a3, a4, a5);
}

void* Hooks::hkCalcViewmodel(float* flUnk, float* flOffsets, float* flFov)
{
	const auto pOriginal = Detours::CalcViewmodel.GetOriginal<decltype(&hkCalcViewmodel)>();

	void* pReturn = pOriginal(flUnk, flOffsets, flFov);

	if (!Config::b(g_Variables.m_Visuals.m_bViewmodelChanger))
		return pOriginal(flUnk, flOffsets, flFov);

	flOffsets[0] += Config::f(g_Variables.m_Visuals.flXOffset);
	flOffsets[1] += Config::f(g_Variables.m_Visuals.flYOffset);
	flOffsets[2] += Config::f(g_Variables.m_Visuals.flZOffset);

	*flFov = Config::i(g_Variables.m_Visuals.iViewmodelFov);

	QAngle* rot = (QAngle*)((__int64)flOffsets + 0x20);

	rot->x += Config::f(g_Variables.m_Visuals.flXRotation);
	rot->y += Config::f(g_Variables.m_Visuals.flYRotation);
	rot->z += Config::f(g_Variables.m_Visuals.flZRotation);

	rot->Normalize();

	return pReturn;
}

__int64 Hooks::hkGetInterpolatedShootPosition(CPlayer_WeaponServices* pWeaponServices, Vector* vecEyePosition, int nShootTick, float flFraction)
{
	static auto oOriginal = Detours::GetInterpolatedShootPosition.GetOriginal<decltype(&hkGetInterpolatedShootPosition)>();

	auto result = oOriginal(pWeaponServices, vecEyePosition, nShootTick, flFraction);;

	return result;
}

static bool FASTCALL Hooks::hkRemoveLegs(C_CSPlayerPawn pPawn)
{
	const auto oRemoveLegs = Detours::RemoveLegs.GetOriginal<decltype(&hkRemoveLegs)>();

	if (!Interfaces::m_pEngine->IsConnected() || !Interfaces::m_pEngine->IsInGame())
		return oRemoveLegs(pPawn);;

	if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(ERemovals::REMOVE_LOCAL_LEGS) && !Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson))
		return false;

	return oRemoveLegs(pPawn);
}

void FASTCALL Hooks::hkAllowCameraAngleChange(CCSGOInput* pCSGOInput, int a2)
{
	const auto Original = Detours::AllowCameraAngleChange.GetOriginal<decltype(&hkAllowCameraAngleChange)>();

	QAngle angViewAngles = pCSGOInput->GetViewAngles();
	Original(pCSGOInput, a2);
	pCSGOInput->SetViewAngle(angViewAngles);
}

void* FASTCALL Hooks::hkIsRelativeMouseMode(void* pThisptr, bool bActive)
{
	const auto oIsRelativeMouseMode = Detours::IsRelativeMouseMode.GetOriginal<decltype(&hkIsRelativeMouseMode)>();

	bool bMouseOverrideActive = Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_bMouseOverride));

	Gui::m_bInputActive = bActive;
	return oIsRelativeMouseMode(pThisptr, (Gui::m_bOpen || bMouseOverrideActive) ? false : bActive);
}

float s_flLastUpdatedRealTime = 0.f;
float s_flOldSmoothedValue = 0.f;

void FASTCALL Hooks::hkOnRenderStart(CViewRender* pViewRender)
{
	const auto oOnRenderStart = Detours::OnRenderStart.GetOriginal<decltype(&hkOnRenderStart)>();

	oOnRenderStart(pViewRender);

	if (!Interfaces::m_pGlobalVariables)
		return;

	{
		static CTimer TimerObj(true);
		static int nFrameCount = 0;
		nFrameCount++;

		float flElapsed = TimerObj.Elapsed();
		if (flElapsed > 1.f)
			Gui::m_iAverageFPS = static_cast<int>(nFrameCount / (flElapsed / 1000.0f));
		if (flElapsed > 1000.0f)
		{
			nFrameCount = 0;
			TimerObj.Reset();
		}
	}

	return Features::OnRenderStart(pViewRender);
}

float Hooks::hkSetWorldFov(void* pRcx) {
	static auto original = Detours::SetWorldFov.GetOriginal<decltype(&hkSetWorldFov)>();
	float flOriginalFOV = original(pRcx);

	if (!Interfaces::m_pEngine->IsInGame() || !Interfaces::m_pEngine->IsConnected())
		return flOriginalFOV;

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerPawn)
		return flOriginalFOV;

	CPlayer_WeaponServices* pWeaponServices = Globals::m_pLocalPlayerPawn->m_pWeaponServices();
	if (!pWeaponServices)
		return flOriginalFOV;

	C_BasePlayerWeapon* pActiveWeapon = pWeaponServices->m_hActiveWeapon().Get();
	if (!pActiveWeapon)
		return flOriginalFOV;

	C_CSWeaponBaseGun* pWeaponGun = static_cast<C_CSWeaponBaseGun*>(pActiveWeapon);
	if (!pWeaponGun)
		return flOriginalFOV;

	C_AttributeContainer* pAtts = pActiveWeapon->m_AttributeManager();
	if (!pAtts)
		return flOriginalFOV;

	C_EconItemView* pItem = pAtts->m_Item();
	if (!pItem)
		return flOriginalFOV;

	if (Globals::m_pLocalPlayerPawn->m_bIsScoped() && Config::b(g_Variables.m_Visuals.m_bForceZoom)) {
		if (pWeaponGun && pWeaponGun->m_zoomLevel() == 2)
			return 40.f;
	}

	if (!Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(REMOVE_ZOOM)) {
		if (Globals::m_pLocalPlayerPawn->m_bIsScoped())
			return flOriginalFOV;
	}

	return Config::i(g_Variables.m_Visuals.m_iWorldFov);
}

void* Hooks::hkDrawParticleEffect(const char* szEffect, __int64 a2, __int64* angSomeAngles_1, __int64 a4, char a5, unsigned int a6, __int64 a7, bool a8, __int64 a9, __int64 a10, __int64 a11, __int64 a12)
{
	auto oOriginal = Detours::DrawParticleEffect.GetOriginal<decltype(&hkDrawParticleEffect)>();

	if (szEffect == "fx_blood_bullet_impact")
		return nullptr;

	return oOriginal(szEffect, a2, angSomeAngles_1, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

void* FASTCALL Hooks::hkDrawSmokeVertex(void* a1, void* a2, int a3, int a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10)
{
	const auto oOriginal = Detours::DrawSmokeVertex.GetOriginal<decltype(&hkDrawSmokeVertex)>();

	return Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(ERemovals::REMOVE_SMOKE) ? nullptr : oOriginal(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

const char* FASTCALL Hooks::hkGetInfernoType(__int64 a1)
{
	return "";
}

void Hooks::hkMergeBones(CSkeletonInstance* pSkeletonInstance, uint32_t iUnk) /* iUnk = EBoneFlags */
{
	const auto oMergeBones = Detours::MergeBones.GetOriginal<decltype(&hkMergeBones)>();

	if (!Globals::m_pLocalPlayerPawn)
		return oMergeBones(pSkeletonInstance, iUnk);

	bool bIsLocal = pSkeletonInstance->m_pOwner() == Globals::m_pLocalPlayerPawn->m_pGameSceneNode()->GetSkeletonInstance()->m_pOwner();


	if (iUnk == EBoneFlags::FLAG_HITBOX)
		return;

	return oMergeBones(pSkeletonInstance, iUnk);
}

void Hooks::hkRunCommand(CCSPlayer_MovementServices* pMovementServices, CUserCmd* pUserCmd)
{
	const auto oRunCommand = Detours::RunCommand.GetOriginal<decltype(&hkRunCommand)>();

	oRunCommand(pMovementServices, pUserCmd);
}

void* Hooks::hkUpdateSkybox(ISceneSystem* pSceneSystem, CMaterial2** pMaterial, void* pSceneWorld)
{
	return Detours::UpdateSkybox.GetOriginal<decltype(&hkUpdateSkybox)>()(pSceneSystem, pMaterial, pSceneSystem);
}

void FASTCALL Hooks::hkNiggerKickFunction(__int64 a1, unsigned int a2)
{
	const auto original = Detours::NiggerKickFunction.GetOriginal<decltype(&hkNiggerKickFunction)>();

	if (Config::b(g_Variables.m_Misc.m_bRemoveNoSteamLogonKick))
		return;

	return original(a1, a2);
}

class CAnimGraphModelBinding
{
public:
	void* vtbl; //0x0000
	const char* m_szModelName; //0x0008
	void* m_pAnimUpdateSharedData; //0x0010
	void* m_pAnimUpdateInstanceData; //0x0018
	void* m_pAnimNetVarRegistry; //0x0020
};

void* FASTCALL Hooks::hkDisableServerAnimation(void* a1, void* a2, void* a3)
{
	const auto oDisableServerAnimation = Detours::DisableServerAnimation.GetOriginal<decltype(&hkDisableServerAnimation)>();

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pAnimationSystemGadget, oDisableServerAnimation, a1, a2, a3);

	CAnimGraphModelBinding* pAnimGraphModelBinding = (CAnimGraphModelBinding*)a1;

	const char* szModelPath = pAnimGraphModelBinding->m_szModelName;

	if (CRT::StringCompareN(X("weapons/models/knife/"), szModelPath, 21) == 0)
		*(__int64*)((uintptr_t)a3 + 0x30) = 0;

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pAnimationSystemGadget, oDisableServerAnimation, a1, a2, a3);
}

bool FASTCALL Hooks::hkIsGlowing(CGlowProperty* pGlow)
{
	const auto oIsGlowing = Detours::IsGlowing.GetOriginal<decltype(&hkIsGlowing)>();

	if (!pGlow || !pGlow->m_pOwner || !pGlow->m_pOwner->IsPlayerPawn() || pGlow->m_pOwner == Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return oIsGlowing(pGlow);

	if (pGlow->m_pOwner->m_iHealth() <= 0)
		return oIsGlowing(pGlow);

	if (pGlow->m_pOwner->m_iTeamNum() == Globals::m_pLocalPlayerPawn->m_iTeamNum())
		return oIsGlowing(pGlow);

	return Config::b(g_Variables.m_Visuals.m_bGlow);
}

void FASTCALL Hooks::hkGetGlowColor(CGlowProperty* pGlow, float* pColor)
{
	const auto oGetGlowColor = Detours::GetGlowColor.GetOriginal<decltype(&hkGetGlowColor)>();

	if (!pGlow || !pColor || !Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return oGetGlowColor(pGlow, pColor);

	if (pGlow->m_pOwner == Globals::m_pLocalPlayerPawn)
		return oGetGlowColor(pGlow, pColor);

	Color colColor = Config::c(g_Variables.m_Visuals.m_colGlow);
	pColor[0] = colColor.r() / 255.f;
	pColor[1] = colColor.g() / 255.f;
	pColor[2] = colColor.b() / 255.f;
	pColor[3] = colColor.a() / 255.f;
}

void* Hooks::hkDrawViewPunch(CPlayer_CameraServices* a1, float* a2, float* a3, float* a4) {
	const auto oDrawViewPunch = Detours::DrawViewPunch.GetOriginal<decltype(&hkDrawViewPunch)>();

	if (Config::i(g_Variables.m_Visuals.m_iRemoveRecoil) == RECOIL_PUNCH)
		a1->m_vecCsViewPunchAngle() = Vector();

	return oDrawViewPunch(a1, a2, a3, a4);
}

void Hooks::hkDrawScopeOverlay(void* a1, void* a2) {
	const auto original = Detours::DrawScopeOverlay.GetOriginal<decltype(&hkDrawScopeOverlay)>();

	if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_DEFAULT)
		return original(a1, a2);

	else if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_OVERLAY)
		return;

	else if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_GRADIENT)
		return;

	else if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_ALL)
		return;

	return original(a1, a2);
}

void Hooks::hkUpdatePostProcessing(C_PostProcessingVolume* a1, int a2) {
	const static auto original = Detours::UpdatePostProcessing.GetOriginal<decltype(&hkUpdatePostProcessing)>();

	original(a1, a2);

	g_WorldModulation->Exposure(a1);
}

void Hooks::hkSetupMove(CCSPlayer_MovementServices* service, CUserCmd* cmd, CMoveData* data) {
	const static auto og = Detours::SetupMove.GetOriginal<decltype(&hkSetupMove)>();

	og(service, cmd, data);
}