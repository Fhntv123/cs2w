#include "Miscellaneous.h"

void CMisc::AutoPurchase() {
	if (!Config::b(g_Variables.m_Misc.m_bAutoPurchase))
		return;

	std::string buy;

	switch (Config::i(g_Variables.m_Misc.m_iAutoPurchase1))
	{
	case 0:
	default:
		break;
	case 1:
		buy += X("buy g3sg1; buy scar20; ");
		break;
	case 2:
		buy += X("buy awp; ");
		break;
	case 3:
		buy += X("buy ssg08; ");
		break;
	}

	switch (Config::i(g_Variables.m_Misc.m_iAutoPurchase2))
	{
	case 0:
	default:
		break;
	case 1:
		buy += X("buy elite; ");
		break;
	case 2:
		buy += X("buy fiveseven; buy tec9; ");
		break;
	case 3:
		buy += X("buy deagle; ");
		break;
	case 4:
		buy += X("buy revolver; ");
		break;
	}

	if (Config::vb(g_Variables.m_Misc.m_uAutoPurchase3).at(EAutoPurchase::AUTOBUY_ARMOR))
		buy += X("buy vesthelm; buy vest; ");
	if (Config::vb(g_Variables.m_Misc.m_uAutoPurchase3).at(EAutoPurchase::AUTOBUY_TASER))
		buy += X("buy taser; ");
	if (Config::vb(g_Variables.m_Misc.m_uAutoPurchase3).at(EAutoPurchase::AUTOBUY_GRENADES))
		buy += X("buy molotov; buy hegrenade; buy smokegrenade; ");
	if (Config::vb(g_Variables.m_Misc.m_uAutoPurchase3).at(EAutoPurchase::AUTOBUY_DEFUSER))
		buy += X("buy defuser; ");

	Interfaces::m_pEngine->ExecuteClientCmdUnrestricted(buy.c_str());
}

void CMisc::AutoPistol() {
	if (!Config::b(g_Variables.m_Misc.m_bAutoPistol))
		return;

	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerPawn->m_pWeaponServices())
		return;

	C_BasePlayerWeapon* pWeapon = Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get();
	if (!pWeapon || !pWeapon->GetWeaponBaseVData())
		return;

	C_AttributeContainer* pAttrib = pWeapon->m_AttributeManager();
	if (!pAttrib)
		return;

	C_EconItemView* pItem = pAttrib->m_Item();
	if (!pItem)
		return;

	int nWeaponID = pItem->m_iItemDefinitionIndex();
	if (!nWeaponID)
		return;

	if (nWeaponID == WEAPON_R8_REVOLVER)
		return;

	if (!(Globals::m_pCmd->m_nButtons.m_nValue & IN_ATTACK))
		return;

	bool bIsPistol = pWeapon->GetWeaponBaseVData()->m_WeaponType() == WEAPONTYPE_PISTOL;
	bool bIsSSG08 = nWeaponID == WEAPON_SSG_08;

	if (!bIsPistol && !bIsSSG08)
		return;

	bool bCanShoot = (Interfaces::m_pGlobalVariables->m_nTickCount >= pWeapon->m_nNextPrimaryAttackTick());

	static bool bShouldFire = false;
	if (bCanShoot) {
		CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;

		if (bShouldFire) {
			auto shoot_cmd = pBaseCmd->add_subtick_moves();
			shoot_cmd->set_button(IN_ATTACK);
			shoot_cmd->set_pressed(true);
			shoot_cmd->set_when(0.999f);

			auto release_cmd = pBaseCmd->add_subtick_moves();
			release_cmd->set_button(IN_ATTACK);
			release_cmd->set_pressed(false);
			release_cmd->set_when(0.999f);

			bShouldFire = false;
		}
		else {
			bShouldFire = true;
		}
	}
}

void CMisc::StraightThrow(const QAngle& pViewAngles) {
	//if (!Interfaces::m_pGameRules && Interfaces::m_pGameRules->m_bIsValveDS())
	//	return;

	//if (!Config::b(g_Variables.m_Movement.m_bStraigthThrow))
	//	return;

	//C_CSPlayerPawn* pawn = Globals::m_pLocalPlayerPawn;
	//if (!pawn)
	//	return;

	//C_CSWeaponBase* pWeapon = pawn->m_pWeaponServices()->m_hActiveWeapon().Get();
	//if (!pWeapon)
	//	return;

	//CUserCmd* cmd = Globals::m_pCmd;
	//if (!cmd)
	//	return;

	//auto pNade = pawn->m_pWeaponServices()->m_hActiveWeapon().Get();
	//if (!pNade || !pNade->IsNade())
	//	return;

	//const QAngle old_view_angles = {
	//  cmd->m_csgoUserCmd.mutable_base()->viewangles().x(),
	//  cmd->m_csgoUserCmd.mutable_base()->viewangles().y(),
	//  cmd->m_csgoUserCmd.mutable_base()->viewangles().z()
	//};

	//Vector mThrowPos{};
	//Math::AngleVectors(pViewAngles, &mThrowPos, nullptr, nullptr);

	//const float ThrowStrength = std::clamp(reinterpret_cast<C_BaseCSGrenade*>(pNade)->m_flThrowStrength(), 0.f, 1.f);
	//Vector newDir = mThrowPos * (std::clamp(pWeapon->GetWeaponBaseVData()->m_flThrowVelocity() * 0.9f, 15.f, 750.f) * (ThrowStrength * 0.7f + 0.3f)) + pawn->m_vecAbsVelocity() * 1.25f;

	//QAngle newViewangles = newDir.ToAngles();
	//QAngle currentViewangles = old_view_angles;
	//currentViewangles.y += (currentViewangles.y - newViewangles.y);

	//if (!reinterpret_cast<C_BaseCSGrenade*>(pNade)->m_bPinPulled() || cmd->m_nButtons.m_nValue & (IN_ATTACK | IN_ATTACK2)) {
	//	if (reinterpret_cast<C_BaseCSGrenade*>(pNade)->m_fThrowTime() > 0.f || reinterpret_cast<C_BaseCSGrenade*>(pNade)->m_bJumpThrow()) {
	//		cmd->m_csgoUserCmd.mutable_base()->mutable_viewangles()->set_x(currentViewangles.x);
	//		cmd->m_csgoUserCmd.mutable_base()->mutable_viewangles()->set_y(currentViewangles.y);
	//		cmd->m_csgoUserCmd.mutable_base()->mutable_viewangles()->set_z(currentViewangles.z);
	//	}
	//}
}

constexpr int MovementKeyBits =
IN_TURNLEFT |
IN_FORWARD |
IN_BACK |
IN_TURNRIGHT |
IN_MOVELEFT |
IN_MOVERIGHT |
IN_JUMP;

float CMisc::GetMaxPlayerSpeed() {
	if (!Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get() || !Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get()->GetWeaponBaseVData())
		return 260.f;

	return Globals::m_pLocalPlayerPawn->m_bIsScoped() ? Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get()->GetWeaponBaseVData()->m_flMaxSpeed()[1] : Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get()->GetWeaponBaseVData()->m_flMaxSpeed()[0];
}

void CMisc::AccurateWalk(CUserCmd* pCmd) {
	if (!(Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND))
		return;

	if (!Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_bAccurateWalk)))
		return;

	const float maxSpeed = GetMaxPlayerSpeed();
	const float speedMultipler = 0.15f;
	const float targetSpeed = maxSpeed * speedMultipler;
	const float currentSpeed = Globals::m_pLocalPlayerPawn->m_vecVelocity().Length2D();
	
	if (currentSpeed <= targetSpeed)
		return;

	const float slowDownFactor = targetSpeed / currentSpeed;

	CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;
	if (!pBaseCmd)
		return;

	float currentForward = pBaseCmd->forwardmove();
	float currentLeft = pBaseCmd->leftmove();

	pBaseCmd->set_forwardmove(currentForward * slowDownFactor);
	pBaseCmd->set_leftmove(currentLeft * slowDownFactor);

	Globals::m_pCmd->m_nButtons.m_nValue &= ~MovementKeyBits;

	if (pBaseCmd->forwardmove() > 0)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_FORWARD, CInButtonState::EButtonState::IN_BUTTON_DOWN);
	else if (pBaseCmd->forwardmove() < 0)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_BACK, CInButtonState::EButtonState::IN_BUTTON_DOWN);

	if (pBaseCmd->leftmove() > 0)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_MOVELEFT, CInButtonState::EButtonState::IN_BUTTON_DOWN);
	else if (pBaseCmd->leftmove() < 0)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_MOVERIGHT, CInButtonState::EButtonState::IN_BUTTON_DOWN);
}

//void CMisc::PenetrationCrosshair() {
//	//if (!Config::b(g_Variables.m_Misc.m_bPenetrationCrosshair))
//	//	return;
//
//}