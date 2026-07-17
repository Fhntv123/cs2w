#pragma once
#include <memory>

class CFSNHandler {
public:
	void OnPreOriginal(EClientFrameStage nStage) {
		switch (nStage)
		{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		{
#ifdef DEBUG_OR_ALPHA
			CCSPlayerController* pLocalPlayerController = CCSPlayerController::GetLocalPlayerController();
			if (!pLocalPlayerController)
				return;

			C_CSPlayerPawn* pLocalPlayerPawn = pLocalPlayerController->m_hPlayerPawn().Get();
			if (!pLocalPlayerPawn)
				return;

			CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
			if (!pInventory)
				return;

			InventoryChanger::SetCustomModel(pLocalPlayerController, pLocalPlayerPawn, pInventory);
#endif
			//InventoryChanger::UpdateHUD();
			//g_WorldModulation->PrecacheSkyboxes();

			break;
		}
		case FRAME_NET_UPDATE_END:
		{
			if (g_ParticleMgr) 
				g_ParticleMgr->UpdateParticles();

			//g_fog->create_fog();
			//g_fog->fog();

			break; 
		}
		default:
			break;
		}
	}

	void OnPostOriginal(EClientFrameStage nStage) {
		switch ( nStage ) {
		case FRAME_NET_UPDATE_END:
			g_LagCompensation->Run( );
		}
	}

};

inline auto g_FSN = std::make_unique<CFSNHandler>();