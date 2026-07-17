#include "../../Precompiled.h"

constexpr int kMovementKeyBits =
IN_TURNLEFT |
IN_FORWARD |
IN_BACK |
IN_TURNRIGHT |
IN_MOVELEFT |
IN_MOVERIGHT |
IN_JUMP;

void CMovement::FixCmdButtons() {
	if (!(g_Prediction->GetPostFlags() & FL_ONGROUND))
		return;

	if (Globals::m_pBaseCmd->forwardmove() > 0)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_FORWARD, CInButtonState::EButtonState::IN_BUTTON_DOWN);
	else if (Globals::m_pBaseCmd->forwardmove() < 0)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_BACK, CInButtonState::EButtonState::IN_BUTTON_DOWN);

	if (Globals::m_pBaseCmd->leftmove() > 0)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_MOVELEFT, CInButtonState::EButtonState::IN_BUTTON_DOWN);
	else if (Globals::m_pBaseCmd->leftmove() < 0)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_MOVERIGHT, CInButtonState::EButtonState::IN_BUTTON_DOWN);
}

float ApplyFriction(Vector& vVecVelocity)
{
	auto& pawn = Globals::m_pLocalPlayerPawn;
	auto& pMoveService = pawn->m_pMovementServices();

	CONVAR(sv_friction);
	CONVAR(sv_stopspeed);

	float flSpeed = 0.0f;

	if (Interfaces::m_pGlobalVariables->m_flCurrentTime > pMoveService->m_flOffsetTickCompleteTime())
		flSpeed = vVecVelocity.Length();
	else
		flSpeed = pMoveService->m_flOffsetTickStashedSpeed();

	float flSpeedReduction = 0.0f;

	if (flSpeed >= 0.1f) {
		if (pawn->m_hGroundEntity().Get()) {
			float friction = pMoveService->m_flSurfaceFriction() * sv_friction->GetFloat();
			flSpeedReduction = std::fmax(flSpeed, sv_stopspeed->GetFloat()) * friction * pawn->m_flFriction() * Interfaces::m_pGlobalVariables->m_flIntervalPerSubTick;
		}
		float flNewSpeed = fmaxf(flSpeed - flSpeedReduction, 0.f);
		if (flNewSpeed != flSpeed)
		{
			vVecVelocity *= flNewSpeed / flSpeed;
		}
	}

	return fmaxf(-(flSpeed - flSpeedReduction), 0.0);
}

Vector CMovement::SimulatePosAtStop(Vector vPos)
{
	Vector vVecVelocity = Globals::m_pLocalPlayerPawn->m_vecVelocity();
	vVecVelocity.z = 0.f;

	if (!Config::vb(g_Variables.m_Ragebot.m_vbAutostop).at(EAutostop::AUTOSTOP_EARLY))
		return vPos;

	if (Convar::weapon_accuracy_nospread->m_value.m_i1)
		return vPos;

	if (!(Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND))
		return vPos;

	CONVAR(sv_accelerate);
	CONVAR(sv_accelerate_use_weapon_speed);
	CONVAR(sv_water_slow_amount);

	float flSurfaceFriction = Globals::m_pLocalPlayerPawn->m_pMovementServices()->m_flSurfaceFriction();

	int nMaxSimTicks = 20;

	auto& pPlayer = Globals::m_pLocalPlayerPawn;
	auto& pMoveService = pPlayer->m_pMovementServices();
	auto& pWeapon = LocalPlayerData::m_pWeapon;
	auto& pCmd = Globals::m_pCmd;

	Vector vExtrap = vPos;
	float flMaxSpeed = pMoveService->m_flMaxspeed();
	float flTargetSpeed = flMaxSpeed * 0.34f;

	float flInitialVelocity = vVecVelocity.Length2D();

	if (flInitialVelocity <= flTargetSpeed)
		return vPos;

	for (int i = 0; i < nMaxSimTicks; i++) {
		float flCurrentVelocity = vVecVelocity.Length2D();

		if (flCurrentVelocity <= flTargetSpeed)
			break;

		float flFrictionDecel = ApplyFriction(vVecVelocity);

		Vector vWishDir = -vVecVelocity;
		float flWishSpeed = vWishDir.NormalizeInPlace();
		float flCurrentSpeed = vVecVelocity.DotProduct(vWishDir);

		int nButtonFlags = pCmd->m_nButtons.m_nValue;
		bool bIsDucking = (nButtonFlags & IN_DUCK) ||
			pMoveService->m_bDucking() ||
			(pPlayer->m_fFlags() & FL_DUCKING);
		bool bIsHoldingSprintKey = (nButtonFlags & IN_SPRINT) && !bIsDucking;

		float v19_clampedWishSpeed = std::max(250.0f, flWishSpeed);
		float v20_effectiveAbsMaxSpeed = v19_clampedWishSpeed;
		float v21_accelerationFactor = 1.0f;
		bool v23_weaponCausesSlowdown = false;

		if (sv_accelerate_use_weapon_speed->GetBool() && pWeapon) {
			float flWeaponMaxSpeed = flMaxSpeed;
			auto pVData = pWeapon->GetWeaponBaseVData();

			if (pVData && pWeapon->m_zoomLevel() > 0 && pVData->m_nZoomLevels() > 1 && (flWeaponMaxSpeed * 0.51999998f) < 110.0f) {
				v23_weaponCausesSlowdown = true;
			}

			float flWeaponSpeedRatio = std::min(1.0f, flWeaponMaxSpeed / 250.0f);

			if ((!bIsDucking && !bIsHoldingSprintKey) || v23_weaponCausesSlowdown) {
				v21_accelerationFactor = flWeaponSpeedRatio;
			}

			v20_effectiveAbsMaxSpeed = v19_clampedWishSpeed * flWeaponSpeedRatio;
		}

		float v29_baseSpeedForAccelCalc = v19_clampedWishSpeed;

		unsigned int uWaterLevel = (pPlayer->m_flWaterLevel() * 4.f) + 1.f;

		if (uWaterLevel >= 2) {
			float flWaterSlowAmount = sv_water_slow_amount->GetFloat();
			v20_effectiveAbsMaxSpeed *= flWaterSlowAmount;

			if (bIsDucking) {
				v20_effectiveAbsMaxSpeed *= 0.34f;
				v21_accelerationFactor = std::min(0.34f, v21_accelerationFactor);
			}
			else {
				if (!bIsHoldingSprintKey) {
					v29_baseSpeedForAccelCalc = v19_clampedWishSpeed * flWaterSlowAmount;
				}
			}
		}
		else {
			if (bIsDucking) {
				v20_effectiveAbsMaxSpeed *= 0.34f;
				v21_accelerationFactor = std::min(0.34f, v21_accelerationFactor);
			}
		}

		float v29_finalSpeedCap = v29_baseSpeedForAccelCalc * v21_accelerationFactor;

		float flModifiedSvAccelerate = sv_accelerate->GetFloat();
		float flPositiveCurrentSpeed = std::max(0.0f, flCurrentSpeed);

		if (bIsHoldingSprintKey) {
			bool bIsCarryingHostage = pPlayer->m_pHostageServices() ? pPlayer->m_pHostageServices()->m_hCarriedHostage().Get() : false;

			if (!bIsCarryingHostage && !v23_weaponCausesSlowdown) {
				v29_finalSpeedCap *= 0.51999998f;
			}

			float flReducedAbsMaxSpeed = v20_effectiveAbsMaxSpeed * 0.51999998f;
			float flSprintSpeedThreshold = flReducedAbsMaxSpeed - 5.0f;

			if (flPositiveCurrentSpeed > flSprintSpeedThreshold) {
				float flNumerator = std::max(0.0f, flPositiveCurrentSpeed - flSprintSpeedThreshold);
				float flDenominator = std::max(0.001f, flReducedAbsMaxSpeed - flSprintSpeedThreshold);
				float flSpeedFactorRatio = flNumerator / flDenominator;
				float flAccelReductionFactor = std::min(1.0f, std::max(0.0f, 1.0f - flSpeedFactorRatio));
				flModifiedSvAccelerate *= flAccelReductionFactor;
			}
		}

		float flPotentialAccelGain = flModifiedSvAccelerate * INTERVAL_PER_TICK * v29_finalSpeedCap * flSurfaceFriction;
		float flAddSpeed = -flCurrentSpeed;
		float flAccelAmountToAdd = std::max(0.0f, std::min(flPotentialAccelGain, flAddSpeed));

		vVecVelocity.x += vWishDir.x * flAccelAmountToAdd;
		vVecVelocity.y += vWishDir.y * flAccelAmountToAdd;

		vExtrap.x += vVecVelocity.x * INTERVAL_PER_TICK;
		vExtrap.y += vVecVelocity.y * INTERVAL_PER_TICK;
	}

	return vExtrap;
}

void CMovement::StopMovement()
{
	CONVAR(weapon_accuracy_nospread);

	bool bQuickStop = (Config::b(g_Variables.m_Movement.m_bEnableQuickStop) && !Globals::m_bIsPressingMovementKey);
	if (!bQuickStop && weapon_accuracy_nospread->GetBool())
		return;

	if (bQuickStop || this->m_bWants2DMoveHalt)
	{
		const bool bAutoPeekEnabled = Config::b(g_Variables.m_Ragebot.m_bAutoPeek);
		const bool bAutoPeekBound = Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_kbAutoPeek));

		if (bAutoPeekEnabled && bAutoPeekBound && (quickpeek_retract || quickpeek_retract_shoot))
			return;

		bool bOnGround = Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND;

		if (!bOnGround && !Config::vb(g_Variables.m_Ragebot.m_vbAutostop).at(EAutostop::AUTOSTOP_IN_AIR))
			return;

		CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;

		Vector vVelocity = Globals::m_pLocalPlayerPawn->m_vecVelocity();

		pBaseCmd->set_leftmove(0.0f);
		pBaseCmd->set_forwardmove(vVelocity.Length2D() > 20.0f ? 1.0f : 0.0f);

		QAngle aViewAngles = this->m_angCameraAngles;
		float flYaw = Math::CalcAngle(Vector(0.0f, 0.0f, 0.0f), vVelocity).y + 180.0f;
		float flRotation = M_DEG2RAD(aViewAngles.y - flYaw);

		float flCosRotation = cos(flRotation);
		float flSinRotation = sin(flRotation);

		float flNewForwardMove = flCosRotation * pBaseCmd->forwardmove() - flSinRotation * pBaseCmd->leftmove();
		float flNewSideMove = flSinRotation * pBaseCmd->forwardmove() + flCosRotation * pBaseCmd->leftmove();

		pBaseCmd->set_forwardmove(flNewForwardMove);
		pBaseCmd->set_leftmove(-flNewSideMove);

		Globals::m_pCmd->m_nButtons.m_nValue &= ~kMovementKeyBits;
		Globals::m_pCmd->m_nButtons.m_nValueChanged &= ~kMovementKeyBits;
		Globals::m_pCmd->m_nButtons.m_nValueScroll &= ~kMovementKeyBits;
	}
}

bool IsValidMovementTrace(GameTrace_t& tr, BBox_t bounds, TraceFilter_t* filter)
{
	GameTrace_t stuck;

	if (tr.m_bAllSolid)
		return false;

	if (tr.m_flFraction < 1.0f && fabs(tr.m_vecNormal.x) < FLT_EPSILON && fabs(tr.m_vecNormal.y) < FLT_EPSILON && fabs(tr.m_vecNormal.z) < FLT_EPSILON)
		return false;

	if (fabs(tr.m_vecNormal.x) > 1.0f || fabs(tr.m_vecNormal.y) > 1.0f || fabs(tr.m_vecNormal.z) > 1.0f)
		return false;

	Interfaces::m_pVPhys2World->TracePlayerBBox(&tr.m_vecEndPos, &tr.m_vecEndPos, &bounds, filter, &stuck);
	if (stuck.m_bAllSolid || stuck.m_flFraction < 1.0f - FLT_EPSILON)
		return false;

	Interfaces::m_pVPhys2World->TracePlayerBBox(&tr.m_vecEndPos, &tr.m_vecStartPos, &bounds, filter, &stuck);

	if (stuck.m_bAllSolid)
		return false;

	return true;
}

void CMovement::BunnyHop()
{
	CONVAR(sv_autobunnyhopping);

	if (!Config::b(g_Variables.m_Movement.m_bEnableBunnyHop))
		return;

	auto pMovementServices = Globals::m_pLocalPlayerPawn->m_pMovementServices();
	if (!pMovementServices)
		return;

	if (sv_autobunnyhopping->GetBool() || !(Globals::m_pCmd->m_nButtons.m_nValue & IN_JUMP))
		return;

	if ((g_Prediction->GetPostFlags() & FL_ONGROUND))
		Globals::m_pCmd->m_nButtons.m_nValue &= ~IN_JUMP;
}

void CMovement::JumpBug()
{
	if (!(Globals::m_pCmd->m_nButtons.m_nValue & IN_JUMP))
		return;

	if (!Config::b(g_Variables.m_Movement.m_bEnableJumpBug))
		return;

	CONVAR(sv_autobunnyhopping);
	if (!sv_autobunnyhopping->GetBool())
		return;

	CUserCmd* cmd = Globals::m_pCmd;
	CBaseUserCmdPB* base_cmd = Globals::m_pBaseCmd;

	auto pawn = Globals::m_pLocalPlayerPawn;
	if (!pawn)
		return;

	auto movement = pawn->m_pMovementServices();
	if (!movement)
		return;

	bool edge_detected = false;
	if ((pawn->m_fFlags() & FL_ONGROUND) && !(Interfaces::m_pGameRules->m_fRoundStartTime() > Interfaces::m_pGlobalVariables->m_flCurrentTime))
	{
		Vector origin = pawn->m_pGameSceneNode()->m_vecAbsOrigin();

		auto collision = pawn->m_pCollision();
		if (collision)
		{
			BBox_t bounds;
			pawn->m_pMovementServices()->GetPlayerCollisionBound(&bounds);

			TraceFilter_t filter;
			Interfaces::m_pVPhys2World->InitPlayerMovementTraceFilter(&filter, pawn, 0x1C3003, COLLISION_GROUP_PLAYER_MOVEMENT);

			Vector start = origin;
			Vector end = origin - Vector(0, 0, 32.0f);

			GameTrace_t trace;

			if (Interfaces::m_pVPhys2World->TracePlayerBBox(&start, &end, &bounds, &filter, &trace))
			{
				edge_detected = trace.m_flFraction >= 1.0f || !trace.m_pHitEntity;
			}
		}
	}

	if (((pawn->m_fFlags() & FL_ONGROUND) && !(Interfaces::m_pGameRules->m_fRoundStartTime() > Interfaces::m_pGlobalVariables->m_flCurrentTime)) || edge_detected)
	{
		cmd->m_nButtons.m_nValue |= IN_DUCK;

		base_cmd->clear_subtick_moves();

		// Create subtick moves properly to avoid type conflicts
		auto duck_subtick = base_cmd->add_subtick_moves();
		if (duck_subtick)
		{
			duck_subtick->set_when(0.f);
			duck_subtick->set_button(IN_DUCK);
			duck_subtick->set_pressed(true);
		}

		auto jump_subtick = base_cmd->add_subtick_moves();
		if (jump_subtick)
		{
			jump_subtick->set_when(0.25f);
			jump_subtick->set_button(IN_JUMP);
			jump_subtick->set_pressed(true);
		}

		auto unduck_subtick = base_cmd->add_subtick_moves();
		if (unduck_subtick)
		{
			unduck_subtick->set_when(0.999f);
			unduck_subtick->set_button(IN_DUCK);
			unduck_subtick->set_pressed(false);
		}

		auto unjump_subtick = base_cmd->add_subtick_moves();
		if (unjump_subtick)
		{
			unjump_subtick->set_when(0.999f);
			unjump_subtick->set_button(IN_JUMP);
			unjump_subtick->set_pressed(false);
		}
	}
	else if (!(pawn->m_fFlags() & FL_ONGROUND))
	{
		cmd->m_nButtons.m_nValue &= ~IN_DUCK;
	}
}

void CMovement::DirectionalAirStrafe(Vector vVelocity, float flCurrentSpeed, float flFrameTime, bool bSubtickCalling) {
	if (!Config::b(g_Variables.m_Movement.m_bEnableAutoStrafe))
		return;

	CONVAR(sv_airaccelerate);
	CONVAR(sv_air_max_wishspeed);

	auto pBase = Globals::m_pBaseCmd;
	auto pMoveService = Globals::m_pLocalPlayerPawn->m_pMovementServices();

	static uint64_t uLastPressed = 0;
	static uint64_t uLastButtons = 0;

	const uint64_t uCurrentButtons = Globals::m_pCmd->m_nButtons.m_nValue;

	const auto CheckButton = [&](const uint64_t uButton)
		{
			if (uCurrentButtons & uButton && (!(uLastButtons & uButton)
				|| (uButton & IN_MOVELEFT && !(uLastPressed & IN_MOVERIGHT))
				|| (uButton & IN_MOVERIGHT && !(uLastPressed & IN_MOVELEFT))
				|| (uButton & IN_FORWARD && !(uLastPressed & IN_BACK))
				|| (uButton & IN_BACK && !(uLastPressed & IN_FORWARD))))
			{
				if (uButton & IN_MOVELEFT)
					uLastPressed &= ~IN_MOVERIGHT;
				else if (uButton & IN_MOVERIGHT)
					uLastPressed &= ~IN_MOVELEFT;
				else if (uButton & IN_FORWARD)
					uLastPressed &= ~IN_BACK;
				else if (uButton & IN_BACK)
					uLastPressed &= ~IN_FORWARD;

				uLastPressed |= uButton;
			}
			else if (!(uCurrentButtons & uButton))
				uLastPressed &= ~uButton;
		};

	CheckButton(IN_MOVELEFT);
	CheckButton(IN_MOVERIGHT);
	CheckButton(IN_FORWARD);
	CheckButton(IN_BACK);

	uLastButtons = uCurrentButtons;

	if (uCurrentButtons & IN_SPRINT || Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_LADDER ||
		Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_NOCLIP ||
		g_Prediction->GetPostFlags() & FL_ONGROUND ||
		g_Prediction->GetPreFlags() & FL_ONGROUND)
		return;

	float flOffset = 0.f;
	if (uLastPressed & IN_MOVELEFT)
		flOffset += 90.f;

	if (uLastPressed & IN_MOVERIGHT)
		flOffset -= 90.f;

	if (uLastPressed & IN_FORWARD)
		flOffset *= 0.5f;

	else if (uLastPressed & IN_BACK)
		flOffset = -flOffset * 0.5f + 180.f;

	float flYaw = Math::FloatNormalize(Interfaces::m_pInput->GetViewAngles().y);

	flYaw += flOffset;

	pBase->set_forwardmove(0.f);
	pBase->set_leftmove(0.f);

	const float flVelocityAngle = Math::FloatNormalize(M_RAD2DEG(atan2f(vVelocity.y, vVelocity.x)));
	const float flSpeed = vVelocity.Length2D();
	const auto flIdeal = std::clamp(
		M_RAD2DEG(
			atan(
				fmax(
					15.f,
					30.f - (
						flCurrentSpeed *
						sv_airaccelerate->GetFloat() *
						pMoveService->m_flSurfaceFriction() *
						flFrameTime
						)
				) / flSpeed
			)
		),
		0.f,
		45.f
	);

	const float flVelocityDelta = Math::FloatNormalize(flYaw - flVelocityAngle);

	auto RotateMovement = [](CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, float flTargetYaw)
		{
			const float flRotation = M_DEG2RAD(pBaseCmd->viewangles().y() - flTargetYaw);

			const float flNewForwardMove = cos(flRotation) * pBaseCmd->forwardmove() - sin(flRotation) * pBaseCmd->leftmove();
			const float flNewSideMove = sin(flRotation) * pBaseCmd->forwardmove() + cos(flRotation) * pBaseCmd->leftmove();

			pBaseCmd->set_forwardmove(std::clamp(flNewForwardMove, -1.f, 1.f));
			pBaseCmd->set_leftmove(std::clamp(flNewSideMove * -1.f, -1.f, 1.f));
		};

	if ((fabsf(flVelocityDelta) > 170.f && flSpeed > 80.f) || (flVelocityDelta > flIdeal && flSpeed > 80.f))
	{
		flYaw = flIdeal + flVelocityAngle;
		pBase->set_leftmove(-1.f);
		RotateMovement(Globals::m_pCmd, pBase, Math::FloatNormalize(flYaw));

		return;
	}

	static bool bSideSwitch = false;
	bSideSwitch = bSubtickCalling ? !bSideSwitch : Globals::nSequence % 2 == 0;

	if (-flIdeal <= flVelocityDelta || flSpeed <= 80.f) {
		if (bSideSwitch) {
			flYaw = flYaw - flIdeal;
			pBase->set_leftmove(-1.f);
		}
		else {
			flYaw = flIdeal + flYaw;
			pBase->set_leftmove(1.f);
		}
	}
	else {
		flYaw = flVelocityAngle - flIdeal;
		pBase->set_leftmove(1.f);
	}

	RotateMovement(Globals::m_pCmd, pBase, flYaw);

	if (!bSubtickCalling && Config::b(g_Variables.m_Movement.m_bEnableAutoStrafe)) {
		this->SubtickAirStrafe();
	}
}

void CMovement::AirAccelerate(Vector& vVelocity, Vector vMoveImpulse, float& flStamina, float flFrameTime, float flYaw)
{
	CONVAR(sv_airaccelerate);
	CONVAR(sv_staminarecoveryrate);
	CONVAR(sv_air_max_wishspeed);

	float flCurrentSpeed = vVelocity.Length2D();
	if (flStamina > 0)
	{
		float flSpeedScale = std::clamp(1.0f - (flStamina / 100.f), 0.f, 1.f);
		flCurrentSpeed *= flSpeedScale * flSpeedScale;
		flStamina = fmaxf(flStamina - (flFrameTime * sv_staminarecoveryrate->GetFloat()), 0.f);
	}

	QAngle angViewAngles = { 0, flYaw, 0 };

	Vector vecForward, vecLeft, vecUp;
	angViewAngles.ToDirections(&vecForward, &vecLeft, &vecUp);
	vecForward.NormalizeInPlace();
	vecLeft.NormalizeInPlace();

	Vector vecWishDir(
		((vecForward.x * vMoveImpulse.x) * flCurrentSpeed) - ((vecLeft.x * vMoveImpulse.y) * flCurrentSpeed),
		((vecForward.y * vMoveImpulse.x) * flCurrentSpeed) - ((vecLeft.y * vMoveImpulse.y) * flCurrentSpeed),
		0.f
	);

	float flWishSpeed = vecWishDir.NormalizeInPlace();
	flWishSpeed = fminf(flWishSpeed, flCurrentSpeed);

	const float flSpeedAdd = fminf(flWishSpeed, sv_air_max_wishspeed->GetFloat() - vVelocity.DotProduct(vecWishDir));

	if (flSpeedAdd < 0.f)
		return;

	const float flAccelSpeed = fminf(flSpeedAdd, flWishSpeed * flFrameTime * sv_airaccelerate->GetFloat());
	vVelocity += (vecWishDir * flAccelSpeed) * 0.5f;
}

void CMovement::SubtickAirStrafe()
{
	CONVAR(sv_gravity);

	if (Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_LADDER ||
		Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_NOCLIP ||
		(g_Prediction->GetPostFlags() & FL_ONGROUND) ||
		(g_Prediction->GetPreFlags() & FL_ONGROUND) ||
		(Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND))
		return;

	auto pBase = Globals::m_pBaseCmd;
	auto pLocal = Globals::m_pLocalPlayerPawn;
	auto pMoveService = pLocal->m_pMovementServices();

	pBase->clear_subtick_moves();

	float flFriction = pMoveService->m_flSurfaceFriction();
	float flStamina = pMoveService->m_flStamina();

	Vector vAbsVelocity = pLocal->m_vecAbsVelocity();
	Vector vLastImpulses = pMoveService->m_vecLastMovementImpulses();
	Vector vCMDMoveBackup = { pBase->forwardmove(), pBase->leftmove(), pBase->upmove() };

	const int nTicks = std::clamp(12 - pBase->subtick_moves_size(), 0, 12);
	const float flFrameTime = INTERVAL_PER_TICK / static_cast<float>(nTicks);
	float flMovementYaw = this->m_angModelAngles.y;

	for (int i = 0; i < nTicks; ++i)
	{
		const auto& pSubtick = Interfaces::m_pInput->CreateNewSubTickMoveStep(pBase->mutable_subtick_moves());
		if (!pSubtick)
			continue;

		pBase->set_forwardmove(vCMDMoveBackup.x);
		pBase->set_leftmove(vCMDMoveBackup.y);
		pBase->set_upmove(vCMDMoveBackup.z);

		this->AirAccelerate(vAbsVelocity, vLastImpulses, flStamina, flFrameTime, flMovementYaw);

		if (this->m_bWants2DMoveHalt) {
			this->StopMovement();
			this->MovementCorrection();
		}
		else {
			this->DirectionalAirStrafe(vAbsVelocity, vAbsVelocity.Length2D(), flFrameTime, true);
		}

		pSubtick->set_when(static_cast<float>(i) / static_cast<float>(nTicks));
		pSubtick->set_analog_forward_delta(pBase->forwardmove() - vLastImpulses.x);
		pSubtick->set_analog_left_delta(pBase->leftmove() - vLastImpulses.y);
		pSubtick->set_button(0);
		pSubtick->set_pressed(false);

		vLastImpulses.x += pSubtick->analog_forward_delta();
		vLastImpulses.y += pSubtick->analog_left_delta();

		pBase->mutable_subtick_moves()->AddAllocated(pSubtick);
	}
}

void CMovement::HandleMoveStop(bool bHaltMovement) {

	this->m_bWants2DMoveHalt = bHaltMovement;

	if (Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND)
	{
		this->StopMovement();
	}
}

void CMovement::HandleDirectionalStrafe()
{
	if (!Config::b(g_Variables.m_Movement.m_bEnableAutoStrafe))
		return;

	this->SubtickAirStrafe();

	this->DirectionalAirStrafe(Globals::m_pLocalPlayerPawn->m_vecAbsVelocity(), LocalPlayerData::m_pWeapon->GetMaxSpeed(), 1.f, false);
}

void CMovement::AutoRetreat(CUserCmd* cmd, C_CSPlayerPawn* pawn)
{
	active_retract = false;

	if (!Config::b(g_Variables.m_Ragebot.m_bAutoPeek) || !pawn || pawn->m_iHealth() <= 0)
		return;

	const uint64_t movementKeys = IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;
	quickpeek_retract = !(cmd->m_nButtons.m_nValue & movementKeys);

	if (cmd->m_nButtons.m_nValue & IN_ATTACK)
		quickpeek_retract_shoot = true;

	if (!Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_kbAutoPeek)))
	{
		quickpeek_retract = false;
		quickpeek_retract_shoot = false;
		retractPos = Vector{};
		return;
	}

	if (!pawn || pawn->m_iHealth() <= 0)
	{
		quickpeek_retract = false;
		quickpeek_retract_shoot = false;
		retractPos = Vector{};
		return;
	}

	if (retractPos.IsZero())
		retractPos = pawn->m_pGameSceneNode()->m_vecAbsOrigin();

	const bool shouldRetract = (quickpeek_retract && Config::b(g_Variables.m_Ragebot.m_bRetreatOnRelease)) || quickpeek_retract_shoot;

	if (shouldRetract && !retractPos.IsZero())
	{
		Vector delta = retractPos - pawn->m_pGameSceneNode()->m_vecAbsOrigin();
		float distance = delta.Length2D();

		if (distance <= 1.0f)
		{
			quickpeek_retract = false;
			quickpeek_retract_shoot = false;
			return;
		}

		QAngle viewAngles = this->m_angCameraAngles;
		Vector forward, right, up;
		Math::AngleVectors(viewAngles, forward, right, up);

		forward.z = right.z = 0.f;
		forward.NormalizeInPlace();
		right.NormalizeInPlace();

		Vector wishdir = delta.Normalized();
		wishdir.z = 0.f;
		wishdir.NormalizeInPlace();

		float forwardmove = wishdir.DotProduct(forward);
		float sidemove = wishdir.DotProduct(right);

		CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;
		pBaseCmd->set_forwardmove(forwardmove * 450.f);
		pBaseCmd->set_leftmove(sidemove * -450.f);

		cmd->m_nButtons.m_nValue &= ~(IN_DUCK | IN_JUMP | movementKeys);

		if (pBaseCmd->forwardmove() > 0)
			cmd->m_nButtons.m_nValue |= IN_FORWARD;
		else if (pBaseCmd->forwardmove() < 0)
			cmd->m_nButtons.m_nValue |= IN_BACK;

		if (pBaseCmd->leftmove() > 0)
			cmd->m_nButtons.m_nValue |= IN_MOVERIGHT;
		else if (pBaseCmd->leftmove() < 0)
			cmd->m_nButtons.m_nValue |= IN_MOVELEFT;

		active_retract = true;

		if (distance <= 1.0f)
			quickpeek_retract_shoot = false;
	}
	else
		quickpeek_retract = false;
}

void Circle3DGradient(const Vector& pos, float radius, const Color& center_color, const Color& edge_color)
{
	if (radius <= 0.f)
		return;

	ImVec2 center;
	if (!Draw::WorldToScreen(pos, center))
		return;

	constexpr int min_segments = 12;
	constexpr int max_segments = 64;
	const int segments = std::clamp(static_cast<int>(radius * 2.f), min_segments, max_segments);
	const float step = 2.0f * 3.141592654f / segments;

	std::vector<ImVec2> points;
	points.reserve(segments + 1);
	points.push_back(center);

	for (int i = 0; i <= segments; i++)
	{
		float angle = step * i;
		Vector world_point(pos.x + radius * std::cos(angle), pos.y + radius * std::sin(angle), pos.z);

		ImVec2 screen_point;
		if (Draw::WorldToScreen(world_point, screen_point))
			points.push_back(screen_point);
	}

	if (points.size() < 3)
		return;

	auto draw_list = ImGui::GetBackgroundDrawList();
	const ImU32 center_col = center_color.GetU32();
	const ImU32 edge_col = edge_color.GetU32();

	for (size_t i = 1; i < points.size() - 1; ++i)
		draw_list->AddTriangleFilledMulticolor(points[0], points[i], points[i + 1], center_col, edge_col, edge_col);
}

void CMovement::RenderAutoRetreat() {
	if (!retractPos.IsZero()) {
		Color col = Config::c(g_Variables.m_Ragebot.m_AutoPeekColor);

		Color transparent_edge(col);
		transparent_edge[3] = 0;

		Circle3DGradient(retractPos, 23.f, col, transparent_edge);
	}
}

void CMovement::MovementCorrection()
{
	auto& pBaseCmd = Globals::m_pBaseCmd;

	int sign = this->m_angModelAngles.x > 89.f ? -1.f : 1.f;
	this->m_angModelAngles.Normalize();

	Vector forward, right, up, old_forward, old_right, old_up;

	Math::AngleVectors(this->m_angModelAngles, forward, right, up);

	forward.z = right.z = up.x = up.y = 0.f;

	forward.NormalizeInPlace();
	right.NormalizeInPlace();
	up.NormalizeInPlace();

	Math::AngleVectors(this->m_angCameraAngles, old_forward, old_right, old_up);

	old_forward.z = old_right.z = old_up.x = old_up.y = 0.f;

	old_forward.NormalizeInPlace();
	old_right.NormalizeInPlace();
	old_up.NormalizeInPlace();

	forward *= pBaseCmd->forwardmove();
	right *= pBaseCmd->leftmove();
	up *= pBaseCmd->upmove();

	float fixed_forward_move = old_forward.DotProduct(right) + old_forward.DotProduct(forward) + old_forward.DotProduct(up, true);
	float fixed_side_move = old_right.DotProduct(right) + old_right.DotProduct(forward) + old_right.DotProduct(up, true);
	float fixed_up_move = old_up.DotProduct(right, true) + old_up.DotProduct(forward, true) + old_up.DotProduct(up);

	pBaseCmd->set_forwardmove(fixed_forward_move);
	pBaseCmd->set_leftmove(fixed_side_move);
	pBaseCmd->set_upmove(fixed_up_move);

	fixed_forward_move = sign * (old_forward.DotProduct(right) + old_forward.DotProduct(forward));
	fixed_side_move = old_right.DotProduct(right) + old_right.DotProduct(forward);

	pBaseCmd->set_forwardmove(std::clamp(fixed_forward_move, -1.f, 1.f));
	pBaseCmd->set_leftmove(std::clamp(fixed_side_move, -1.f, 1.f));

	FixCmdButtons();
}

void CMovement::EdgeJump(CUserCmd* pCmd) {
	if (!Input::HandleInput(Config::kb(g_Variables.m_Movement.m_kEdgeJump)))
		return;

	if (!(g_Prediction->GetPostFlags() & FL_ONGROUND))
		pCmd->m_nButtons.SetButtonState(IN_JUMP, CInButtonState::EButtonState::IN_BUTTON_UP_DOWN);
	JumpBug();
}

//void CMovement::AirDuck()
//{
//	if (!Config::b(g_Variables.m_Movement.m_bAirDuck))
//		return;
//
//	if (Globals::m_pLocalPlayerPawn->m_fFlags() & IN_JUMP) {
//		Globals::m_pCmd->m_nButtons.m_nValue &= ~IN_DUCK;
//	}
//}