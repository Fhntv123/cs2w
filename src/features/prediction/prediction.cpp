#include "../../Precompiled.h"

void ClientSidePredict(CNetworkGameClient* pNetworkGameClient, int nPredictionReason) {
	if (pNetworkGameClient->m_bShouldPredict && pNetworkGameClient->m_iDeltaTick > 0 && pNetworkGameClient->m_iServerTick > 0) {
		// TODO: rebuild this bit, not that much code
		// mostly just calling functions
		Interfaces::m_pClient->FrameStageNotify(FRAME_NET_UPDATE_POSTDATAUPDATE_START);

		const bool bInPredictionBackup = pNetworkGameClient->m_bInPrediction;
		//	pNetworkGameClient->m_bInPrediction = true;
		Interfaces::m_pPrediction->Update(nPredictionReason, pNetworkGameClient->m_iServerTick);
		//	pNetworkGameClient->m_bInPrediction = bInPredictionBackup;
	}
}

void CEnginePrediction::Begin(CUserCmd* pCmd)
{
	if (pCmd->m_nCommandNumber == this->m_nLastSequenceProcessed)
		return;

	CCSPlayer_MovementServices* pMovementServices = Globals::m_pLocalPlayerPawn->m_pMovementServices();
	if (!pMovementServices)
		return;

	CNetworkGameClient* pNetworkGameClient = Interfaces::m_pNetworkClientService->GetNetworkGameClient();
	if (!pNetworkGameClient)
		return;
	//broke
	CUserCmdManager* pCmdManager = Globals::m_pLocalPlayerController->GetUserCmdManager();
	if (!pCmdManager)
		return;

	CCSPlayer_MovementServices* pMoveService = Globals::m_pLocalPlayerPawn->m_pMovementServices();
	if (!pMoveService)
		return;

	if (!Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return

		LocalPlayerData::m_pWeapon->UpdateAccuracyPenalty_Rebuilt();

	m_PredictionData.m_vAbsVelocityBackup = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity();
	m_PredictionData.m_vVelocityBackup = Globals::m_pLocalPlayerPawn->m_vecVelocity();
	m_PredictionData.m_vPreAbsOrigin = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity();

	m_PredictionData.m_flIntervalPerSubTick = Interfaces::m_pGlobalVariables->m_flIntervalPerSubTick;
	m_PredictionData.m_flCurrentTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;
	m_PredictionData.m_flCurrentTime2 = Interfaces::m_pGlobalVariables->m_flCurrentTime2;
	m_PredictionData.m_nTickCount = Interfaces::m_pGlobalVariables->m_nTickCount;
	m_PredictionData.m_flFrameTime = Interfaces::m_pGlobalVariables->m_flFrameTime;
	m_PredictionData.m_flFrameTime2 = Interfaces::m_pGlobalVariables->m_flFrameTime2;
	m_PredictionData.m_nPrePredictionFlags = Globals::m_pLocalPlayerPawn->m_fFlags();

	m_PredictionData.m_nTickBase = LocalPlayerData::m_nTickBase;

	m_PredictionData.m_bInPrediction = Interfaces::m_pPrediction->m_bInPrediction;
	m_PredictionData.m_bFirstPrediction = Interfaces::m_pPrediction->m_bFirstPrediction;
	m_PredictionData.m_bHasBeenPredicted = pCmd->m_bHasBeenPredicted;
	m_PredictionData.m_bShouldPredict = pNetworkGameClient->m_bShouldPredict;

	//printf("%llx\n", Globals::m_pLocalPlayerPawn->m_pMovementServices());
	m_PredictionData.m_pMovementServicesBackup = Interfaces::m_pMemAlloc->Alloc(0x5e0);
	memcpy(m_PredictionData.m_pMovementServicesBackup, Globals::m_pLocalPlayerPawn->m_pMovementServices(), 0x5e0);

	pCmd->m_bHasBeenPredicted = false;
	pNetworkGameClient->m_bShouldPredict = true;
	Interfaces::m_pPrediction->m_bFirstPrediction = false;
	Interfaces::m_pPrediction->m_bInPrediction = true;

	//printf("calling SetPredictionCommand\n");

	pMovementServices->SetPredictionCommand(pCmd);

	//Globals::m_pLocalPlayerController->GetCurrentCommand( ) = pCmd;///broke

	//printf("calling RunCommand\n");

	pMoveService->RunCommand(pCmd);

	//printf("calling ClientSidePredict\n");

	//ClientSidePredict( pNetworkGameClient, PREDICTION_REASON_CREATEMOVE );

	Vector vNonInterpolatedShootPos = LocalPlayerData::m_vecEyePosition;
	//xref sv: ERROR: Player %s calculated shoot time [ #%d + %.3f ] does not match time from cmd [ #%d + %.3f ]\n
	using fnCalculateShootPosition = void(__fastcall*)(Vector*, C_CSPlayerPawn*, TimeStamp_t*, void*, void*);
	static fnCalculateShootPosition CalculateShootPosition = (fnCalculateShootPosition)(Memory::FindPattern(CLIENT_DLL, "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 44 8B 92 ? ? ? ?"));
	TimeStamp_t objTimeStamp = { (int)Globals::m_pLocalPlayerController->m_nTickBase(), 0.f };
	//printf("calling CalculateShootPosition %llx\n", Globals::m_pLocalPlayerPawn);
	CalculateShootPosition(&vNonInterpolatedShootPos, Globals::m_pLocalPlayerPawn, &objTimeStamp, nullptr, nullptr);
	//printf("called CalculateShootPosition\n");
	NRagebot::m_vShootPos = vNonInterpolatedShootPos;

	//printf("calling ResetPredictionCommand\n");

	pMovementServices->ResetPredictionCommand();

	memcpy(Globals::m_pLocalPlayerPawn->m_pMovementServices(), m_PredictionData.m_pMovementServicesBackup, 0x5e0);

	Interfaces::m_pMemAlloc->Free(m_PredictionData.m_pMovementServicesBackup);

	Globals::m_pLocalPlayerPawn->m_vecAbsVelocity() = m_PredictionData.m_vAbsVelocityBackup;
	Globals::m_pLocalPlayerPawn->m_vecVelocity() = m_PredictionData.m_vVelocityBackup;
}

void CEnginePrediction::End(CUserCmd* pCmd)
{
	if (pCmd->m_nCommandNumber == this->m_nLastSequenceProcessed)
		return;

	CCSPlayer_MovementServices* pMovementServices = Globals::m_pLocalPlayerPawn->m_pMovementServices();
	if (!pMovementServices)
		return;

	CNetworkGameClient* pNetworkGameClient = Interfaces::m_pNetworkClientService->GetNetworkGameClient();
	if (!pNetworkGameClient)
		return;

	Interfaces::m_pGlobalVariables->m_flIntervalPerSubTick = m_PredictionData.m_flIntervalPerSubTick;
	Interfaces::m_pGlobalVariables->m_flCurrentTime = m_PredictionData.m_flCurrentTime;
	Interfaces::m_pGlobalVariables->m_flCurrentTime2 = m_PredictionData.m_flCurrentTime2;
	Interfaces::m_pGlobalVariables->m_nTickCount = m_PredictionData.m_nTickCount;
	Interfaces::m_pGlobalVariables->m_flFrameTime = m_PredictionData.m_flFrameTime;
	Interfaces::m_pGlobalVariables->m_flFrameTime2 = m_PredictionData.m_flFrameTime2;

	Globals::m_pLocalPlayerController->m_nTickBase() = m_PredictionData.m_nTickBase;

	Interfaces::m_pPrediction->m_bFirstPrediction = m_PredictionData.m_bFirstPrediction;
	Interfaces::m_pPrediction->m_bInPrediction = m_PredictionData.m_bInPrediction;
	pCmd->m_bHasBeenPredicted = m_PredictionData.m_bHasBeenPredicted;
	pNetworkGameClient->m_bShouldPredict = m_PredictionData.m_bShouldPredict;

	m_PredictionData.m_nPostPredictionFlags = Globals::m_pLocalPlayerPawn->m_fFlags();

	//Globals::m_pLocalPlayerController->GetCurrentCommand( ) = nullptr;

	this->m_nLastSequenceProcessed = pCmd->m_nCommandNumber;
}