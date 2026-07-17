#include "../../Precompiled.h"

constexpr inline const char* s_szSoundPaths[] = {
	"sounds/metallic.vsnd_c",
	"sounds/sound.vsnd_c",
	"sounds/ui/coin_pickup_01.vsnd_c",
	"sounds/ui/panorama/mainmenu_press_shop_01.vsnd_c",
	"sounds/ui/item_drop.vsnd_c"
};

void CShotHandler::RenderHitMarker(const ImVec2& pos, float size)
{
	auto clr = Config::c(g_Variables.m_Visuals.m_colHitmarker);

	float flLength = 4;    
	float flGap = 4;      
	float flThickness = 1.0f;  

	ImU32 color = ImGui::GetColorU32(ImVec4(clr.rBase(), clr.gBase(), clr.bBase(), 1.f));
	ImU32 outline = IM_COL32(0, 0, 0, 100);  

	auto draw = ImGui::GetBackgroundDrawList();

	auto DrawPair = [&](ImVec2 start, ImVec2 end)
		{
			draw->AddLine(start, end, color, flThickness);
		};

	DrawPair(ImVec2(pos.x - (flGap + flLength), pos.y - (flGap + flLength)),
		ImVec2(pos.x - flGap, pos.y - flGap));

	DrawPair(ImVec2(pos.x + (flGap + flLength), pos.y - (flGap + flLength)),
		ImVec2(pos.x + flGap, pos.y - flGap));

	DrawPair(ImVec2(pos.x - (flGap + flLength), pos.y + (flGap + flLength)),
		ImVec2(pos.x - flGap, pos.y + flGap));

	DrawPair(ImVec2(pos.x + (flGap + flLength), pos.y + (flGap + flLength)),
		ImVec2(pos.x + flGap, pos.y + flGap));
}


void CShotHandler::OnPlayerDeath(CGameEvent* pEvent) {
	if (!Interfaces::m_pEngine->IsConnected() || !Interfaces::m_pEngine->IsInGame())
		return;

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;

	CCSPlayerController* pAttacker = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("attacker")));
	CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("userid")));
	C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>(pController->m_hPawn().Get());

	C_CSPlayerPawn* localPawn = Globals::m_pLocalPlayerPawn;
	if (!localPawn)
		return;

	CCSPlayerController* localController = Globals::m_pLocalPlayerController;
	if (!localController)
		return;

	C_CSPlayerPawn* pHurtPawn = Interfaces::m_pGameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pController->m_hPawn());
	if (pHurtPawn != localPawn && pHurtPawn->m_iTeamNum() != localPawn->m_iTeamNum() && pAttacker == localController) {

	}
}

void CShotHandler::OnPlayerHurt(CGameEvent* pEvent)
{
	if (!Interfaces::m_pEngine->IsConnected() || !Interfaces::m_pEngine->IsInGame())
		return;

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;

	CCSPlayerController* pAttacker = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("attacker")));
	CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("userid")));
	C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>(pController->m_hPawn().Get());

	if (!pAttacker || !pController || !pAttacker->m_bIsLocalPlayerController())
		return;

	int dmg = pEvent->GetInt(X("dmg_health"));
	if (dmg <= 0)
		return;

	if (Config::b(g_Variables.m_Visuals.m_bShockEffect))
		pPawn->CreateShockEffect();

	if (Config::b(g_Variables.m_Misc.m_bHitSound))
		Utilities::PlayVSound(s_szSoundPaths[Config::i(g_Variables.m_Misc.m_iHitSoundIndex)], 0.9f);

	int nHitGroup = pEvent->GetInt("hitgroup");
	std::string strChannelMessage =
		"Hit " + std::string(pController->m_sSanitizedPlayerName()) + " for " +
		std::to_string(dmg) + " in " +
		Utilities::GetHitgroupName(nHitGroup) + " with " +
		pEvent->GetString(X("weapon")) + " (remaining: " +
		std::to_string(pEvent->GetInt(X("health"))) + ")";

	if (Config::b(g_Variables.m_Misc.m_bHitlogs))
		AddHitLog(strChannelMessage);

	const float flBacktrackTime = (NRagebot::g_Rage->m_Data.m_LastTarget.HasValidTarget() &&
		NRagebot::g_Rage->m_Data.m_LastTarget.m_pLagRecord &&
		NRagebot::g_Rage->m_Data.m_LastTarget.m_pLagRecord->m_pPawn == pPawn)
		? NRagebot::g_Rage->m_Data.m_LastTarget.m_pLagRecord->m_flSimulationTime
		: pPawn->m_flSimulationTime();

	PlayerESP::m_hit_markers.push_back({ ImGui::GetTime(), 1.0f, 1.0f });

	auto& arr = g_Chams->m_mapOnShotChams[pPawn];

	std::rotate(arr.begin(), arr.begin() + 1, arr.end());
	arr.back() = flBacktrackTime;
}

void CShotHandler::OnBulletImpact(CGameEvent* pEvent)
{
	CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("userid")));
	if (!pController || !pController->m_bIsLocalPlayerController())
		return;

	g_ParticleMgr->OnEvent(pEvent);
}

void CShotHandler::OnPresent()
{
	if (!Interfaces::m_pGlobalVariables)
		return;

#ifdef DEBUG_OR_ALPHA
	float flNow = Interfaces::m_pGlobalVariables->m_flCurrentTime;
	float flMaxDuration = static_cast<float>(Config::i(g_Variables.m_Visuals.m_nHitEffectsDurration)); // seconds

	if (Config::b(g_Variables.m_Visuals.m_bHitmarker))
	{
		m_HitRecords.erase(
			std::remove_if(
				m_HitRecords.begin(),
				m_HitRecords.end(),
				[flNow, flMaxDuration](const PlayerHurtRecord_t& record)
				{
					return (flNow - record.m_flTime) > flMaxDuration;
				}
			),
			m_HitRecords.end()
		);

		for (const auto& record : m_HitRecords)
		{
			ImVec2 screenPos;
			if (Draw::WorldToScreen(record.m_vecImpactPosition, screenPos))
			{
				this->RenderHitMarker(screenPos);
			}
		}
	}
#endif
}