#pragma once

#define DRAW_DEBUG Config::b(g_Variables.m_Misc.m_bDebug)

enum ECustomWorldParticles : unsigned int
{
	WORLD_PARTICLES_NONE = 0,
	WORLD_PARTICLES_ASHES,
	WORLD_PARTICLES_SNOW,
	WORLD_PARTICLES_STARS
};


enum EVisualsPlayer : unsigned int
{
	PLAYER_ENEMY = 0,
	PLAYER_TEAM,
	PLAYER_LOCAL,
	PLAYER_MAX
};

enum EVisualsChamMaterials
{
	VISUAL_MATERIAL_LATEX = 0,
	VISUAL_MATERIAL_BLOOM,
	VISUAL_MATERIAL_GLOW,
	VISUAL_MATERIAL_FLAT,
	VISUAL_MATERIAL_MAX
};

enum EVisualsInfoFlags : unsigned int
{
	FLAG_MONEY = 0,
	FLAG_ARMOR,
	FLAG_KIT,
	FLAG_ZOOM,
	FLAG_DEFUSING,
	FLAG_PING,
	INFO_FLAG_MAX
};

enum EKeyInputType : int
{
	HOLD,
	TOGGLE,
	ALWAYS_ON
};

enum ELegitbotConditions : unsigned int
{
	CONDITION_VISIBLE = 0,
	CONDITION_NO_SMOKE,
	CONDITION_NO_FLASH,
	CONDITION_MAX
};

enum ERemovals : unsigned int
{
	REMOVE_SMOKE = 0,
	REMOVE_FLASH,
	REMOVE_LOCAL_LEGS,
	REMOVE_CROSSHAIR,
	REMOVE_LOCAL_OVERHEAD_INFO,
	REMOVE_TEAM_OVERHEAD_INFO,
	REMOVE_TEAM_INTRO,
	REMOVE_FOG_BRIGHTNESS,
	REMOVE_ZOOM,
	REMOVE_MAX
};

enum EOverlay : unsigned int
{
	OVERLAY_INDICATORS = 0,
	OVERLAY_SPECTATORS,
	OVERLAY_WATERMARK,
	OVERLAY_MAX
};

enum ERemoveRecoil {
	RECOIL_NONE = 0,
	RECOIL_PUNCH,
	RECOIL_ALL,
	RECOIL_MAX = RECOIL_NONE
};

enum ERemoveScope {
	SCOPE_DEFAULT = 0,
	SCOPE_OVERLAY,
	SCOPE_GRADIENT,
	SCOPE_ALL,
	SCOPE_MAX = SCOPE_DEFAULT
};

enum ERageHistoryAimModes : unsigned int
{
	RAGE_HISTORY_BT_ENABLED = 0,
	RAGE_HISTORY_BT_ONLY,
	RAGE_HISTORY_MAX
};

enum EWatermark : unsigned int
{
	WATERMARK_FPS = 0,
	WATERMARK_PING,
	WATERMARK_SPEED,
	WATERMARK_MAX
};

enum EAutostop : unsigned int
{
	AUTOSTOP_EARLY = 0,
	AUTOSTOP_IN_AIR,
	AUTOSTOP_MAX
};

enum ELegitbotHitboxes : unsigned int
{
	LEGITHITBOX_HEAD = 0,
	LEGITHITBOX_CHEST,
	LEGITHITBOX_STOMACH,
	LEGITHITBOX_PELVIS,
	LEGITHITBOX_ARMS,
	LEGITHITBOX_LEGS,
	LEGITHITBOX_FEET,
	LEGITHITBOX_MAX
};

enum EAutoPurchase : unsigned int {
	AUTOBUY_NONE = 0,
	AUTOBUY_ARMOR,
	AUTOBUY_TASER,
	AUTOBUY_GRENADES,
	AUTOBUY_DEFUSER,
	AUTOBUY
};

struct ActiveConfig_t {
	int m_iMinDamage = -1;
	int m_iMinDamageOverride = -1;
	int m_iHitchance = -1;
	int m_iHitchanceOverride = -1;
	int m_iTargettingType = -1;
	int m_iMaxPointscale = -1;
	std::vector<MenuRageHitbox_t> m_Hitboxes = {};
};

class CVariables
{
public:

	struct GuiVariables_t
	{
		C_ADD_VARIABLE(int, m_iMenuKey, VK_INSERT);
		C_ADD_VARIABLE(int, m_iUnloadKey, VK_DELETE);

		C_ADD_VARIABLE(bool, m_bWatermark, false);
		C_ADD_VARIABLE(bool, m_bSpectatorsList, false);
		C_ADD_VARIABLE(bool, m_bKeybindList, false);
		C_ADD_VARIABLE(bool, m_bIndicators, false);

		C_ADD_VARIABLE_VECTOR(bool, EOverlay::OVERLAY_MAX, m_vbOverlay, false);

	}; GuiVariables_t m_Gui;

	struct RagebotVariables_t
	{
		ActiveConfig_t m_ActiveConfig;

		// Light Pistol
		C_ADD_VARIABLE(int, m_LIGHT_PISTOL_iHitchance, 0);
		C_ADD_VARIABLE(int, m_LIGHT_PISTOL_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_LIGHT_PISTOL_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_LIGHT_PISTOL_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_LIGHT_PISTOL_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_LIGHT_PISTOL_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_LIGHT_PISTOL_iMaxPointscale, 0);

		// Deagle
		C_ADD_VARIABLE(int, m_DEAGLE_iHitchance, 0);
		C_ADD_VARIABLE(int, m_DEAGLE_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_DEAGLE_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_DEAGLE_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_DEAGLE_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_DEAGLE_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_DEAGLE_iMaxPointscale, 0);

		// Revolver
		C_ADD_VARIABLE(int, m_REVOLVER_iHitchance, 0);
		C_ADD_VARIABLE(int, m_REVOLVER_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_REVOLVER_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_REVOLVER_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_REVOLVER_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_REVOLVER_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_REVOLVER_iMaxPointscale, 0);

		// SMG
		C_ADD_VARIABLE(int, m_SMG_iHitchance, 0);
		C_ADD_VARIABLE(int, m_SMG_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_SMG_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_SMG_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_SMG_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_SMG_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_SMG_iMaxPointscale, 0);

		// LMG
		C_ADD_VARIABLE(int, m_LMG_iHitchance, 0);
		C_ADD_VARIABLE(int, m_LMG_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_LMG_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_LMG_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_LMG_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_LMG_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_LMG_iMaxPointscale, 0);

		// AR
		C_ADD_VARIABLE(int, m_AR_iHitchance, 0);
		C_ADD_VARIABLE(int, m_AR_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_AR_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_AR_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_AR_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_AR_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_AR_iMaxPointscale, 0);

		// Shotgun
		C_ADD_VARIABLE(int, m_SHOTGUN_iHitchance, 0);
		C_ADD_VARIABLE(int, m_SHOTGUN_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_SHOTGUN_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_SHOTGUN_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_SHOTGUN_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_SHOTGUN_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_SHOTGUN_iMaxPointscale, 0);

		// Scout
		C_ADD_VARIABLE(int, m_SCOUT_iHitchance, 0);
		C_ADD_VARIABLE(int, m_SCOUT_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_SCOUT_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_SCOUT_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_SCOUT_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_SCOUT_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_SCOUT_iMaxPointscale, 0);

		// Auto Sniper
		C_ADD_VARIABLE(int, m_AUTOSNIPER_iHitchance, 0);
		C_ADD_VARIABLE(int, m_AUTOSNIPER_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_AUTOSNIPER_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_AUTOSNIPER_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_AUTOSNIPER_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_AUTOSNIPER_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_AUTOSNIPER_iMaxPointscale, 0);

		// AWP
		C_ADD_VARIABLE(int, m_AWP_iHitchance, 0);
		C_ADD_VARIABLE(int, m_AWP_iHitchanceOverride, 0);
		C_ADD_VARIABLE(int, m_AWP_iMinDamage, 0);
		C_ADD_VARIABLE(int, m_AWP_iMinDamageOverride, 0);
		C_ADD_VARIABLE(int, m_AWP_iTargetPriority, 0);
		C_ADD_VECTOR_HITBOX(m_AWP_vecHitBoxes, {});
		C_ADD_VARIABLE(int, m_AWP_iMaxPointscale, 0);

		C_ADD_VARIABLE(bool, m_bAntiaim, false);
		C_ADD_VARIABLE(KeyBind_t, m_bAccurateWalk, KeyBind_t(0, HOLD));
		C_ADD_VARIABLE(KeyBind_t, m_bFreestanding, KeyBind_t(0, HOLD));
		C_ADD_VARIABLE(KeyBind_t, m_bMouseOverride, KeyBind_t(0, HOLD));
		C_ADD_VARIABLE(bool, m_bUnhittableYaw, false);
		C_ADD_VARIABLE(bool, m_bAtTarget, false);
		C_ADD_VARIABLE(bool, m_bYawJitter, false);
		C_ADD_VARIABLE(bool, m_bPitchJitter, false);
		C_ADD_VARIABLE(KeyBind_t, m_kbEnableSpinAA, KeyBind_t(0, HOLD));
		C_ADD_VARIABLE(bool, m_bSpinAA, false);
		C_ADD_VARIABLE(int, m_iSpinAASpeed, 0);
		C_ADD_VARIABLE(bool, m_bRandomizeYaw, false);
		C_ADD_VARIABLE(float, m_flRandomizeYawRange, 39.f);
		C_ADD_VARIABLE(int, m_iPitch, 0);
		C_ADD_VARIABLE(int, m_iYawJitter, 0);
		C_ADD_VARIABLE(int, m_iPitchJitter, 0);
		C_ADD_VARIABLE(int, m_iYaw, 0);
		C_ADD_VARIABLE(int, m_iYawValue, 0);
		C_ADD_VARIABLE(int, m_iPitchValue, 0);
		C_ADD_VARIABLE(int, m_iRoll, 0);
		C_ADD_VARIABLE(KeyBind_t, m_kbDuckPeekAssist, KeyBind_t(0, HOLD));
		C_ADD_VARIABLE(KeyBind_t, m_iMinDamageOverrideKey, KeyBind_t(0, HOLD));
		C_ADD_VARIABLE(KeyBind_t, m_iHitchanceOverrideKey, KeyBind_t(0, HOLD));
		C_ADD_VARIABLE(KeyBind_t, m_iLeftAntiaimKeybind, KeyBind_t(0, TOGGLE));
		C_ADD_VARIABLE(KeyBind_t, m_iRightAntiaimKeybind, KeyBind_t(0, TOGGLE));
		C_ADD_VARIABLE(KeyBind_t, m_iNoSpreadKey, KeyBind_t(0, ALWAYS_ON));
		C_ADD_VARIABLE(bool, m_bKnifeBot, false);
		C_ADD_VARIABLE(bool, m_bTaserBot, false);
		C_ADD_VARIABLE(bool, m_bAutoRevolver, false);

		C_ADD_VARIABLE(bool, m_bRagebotEnabled, false);
		C_ADD_VARIABLE(bool, m_bDoubleTap, false);
		C_ADD_VARIABLE(bool, m_bNoSpread, false);

		C_ADD_VARIABLE(bool, m_bSilentAim, false);
		C_ADD_VARIABLE(bool, m_bAutoShoot, false);
		C_ADD_VARIABLE(bool, m_bAutoWall, true);

		C_ADD_VARIABLE(float, m_flFov, 1.f);

		C_ADD_VARIABLE(bool, m_bAutostop, false);
		C_ADD_VARIABLE_VECTOR( bool, EAutostop::AUTOSTOP_MAX, m_vbAutostop, false );

		C_ADD_VARIABLE(bool, m_bAutoscope, false);
		C_ADD_VARIABLE(bool, m_bClientNoSpread, false);
		C_ADD_VARIABLE(bool, m_bDuckPeekAssist, false);
		C_ADD_VARIABLE(KeyBind_t, m_iFakePitch, KeyBind_t(0, TOGGLE))

		C_ADD_VARIABLE_VECTOR( bool, ERageHistoryAimModes::RAGE_HISTORY_MAX, m_vbRageHistoryAim, false );

		C_ADD_VARIABLE(bool, m_bAutoPeek, false);
		C_ADD_VARIABLE(bool, m_bRetreatOnRelease, false);
		C_ADD_VARIABLE(KeyBind_t, m_kbAutoPeek, KeyBind_t(0, HOLD));
		C_ADD_VARIABLE(Color, m_AutoPeekColor, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(KeyBind_t, m_iForceHeadshotKey, KeyBind_t(0, TOGGLE));
		C_ADD_VARIABLE(KeyBind_t, m_iForceBodyaimKey, KeyBind_t(0, TOGGLE));

		C_ADD_VARIABLE(bool, m_bZeusBot, false);
		C_ADD_VARIABLE(KeyBind_t, m_kbForceShoot, KeyBind_t(0, TOGGLE));

	}; RagebotVariables_t m_Ragebot;

	struct LegitbotVariables_t
	{
		C_ADD_VARIABLE(bool, m_bEnabled, false);
		C_ADD_VARIABLE(int, m_iSmoothness, 5);
		C_ADD_VARIABLE(int, m_iFov, 4);
		C_ADD_VARIABLE_VECTOR(bool, ELegitbotConditions::CONDITION_MAX, m_vbConditions, false);
		C_ADD_VARIABLE_VECTOR(bool, ELegitbotHitboxes::LEGITHITBOX_MAX, m_vbHitboxes, false);

	}; LegitbotVariables_t m_Legitbot;

	struct VisualsVariables_t
	{
		C_ADD_VARIABLE(KeyBind_t, m_iThirdPersonKeybind, KeyBind_t(0, TOGGLE));
		C_ADD_VARIABLE(bool, m_bEnableThirdPerson, false);
		C_ADD_VARIABLE(bool, m_bForceThirdPerson, false);
		C_ADD_VARIABLE(bool, m_bTransparencyInScope, false);
		C_ADD_VARIABLE(int, m_iThirdPersonDistance, 120);
		C_ADD_VARIABLE(bool, m_bHitmarker, false);
		C_ADD_VARIABLE(Color, m_colHitmarker, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(bool, m_bShockEffect, false);
		C_ADD_VARIABLE(Color, m_colShockEffect, Color(255, 255, 255));
		C_ADD_VARIABLE(int, m_nHitEffectsDurration, 4);
		C_ADD_VARIABLE(bool, m_bRagdoll, false);
		C_ADD_VARIABLE(bool, m_bLaunchRagdoll, false);
		C_ADD_VARIABLE(int, m_iRagdollGravity, 10);
		C_ADD_VARIABLE(int, m_iRemoveScope, SCOPE_MAX);
		C_ADD_VARIABLE(int, m_iRemoveRecoil, RECOIL_MAX);

		C_ADD_VARIABLE(bool, m_bEnableOnShotSkeletonESP, false);
		C_ADD_VARIABLE(Color, m_colOnShotSkeletonBones, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bEnableESP, true);
		C_ADD_VARIABLE(bool, m_bEnableBoxESP, false);
		C_ADD_VARIABLE(bool, m_bEnableSkeletonESP, false);
		C_ADD_VARIABLE(bool, m_bEnableBacktrackSkeletonESP, false);
		C_ADD_VARIABLE(bool, m_bEnableNameESP, false);
		C_ADD_VARIABLE(bool, m_bEnableWeaponESP, false);
		C_ADD_VARIABLE(bool, m_bEnableArmorESP, false);
		C_ADD_VARIABLE(bool, m_bEnableHealthESP, false);
		C_ADD_VARIABLE(bool, m_bEnableHeadDotESP, false);
		C_ADD_VARIABLE(bool, m_bEnableWeaponIconESP, false);

		C_ADD_VARIABLE(bool, m_bEnableEnemyChams, false);
		C_ADD_VARIABLE(bool, m_bEnableEnemyChamsIgnoreZ, false);
		C_ADD_VARIABLE(bool, m_bEnableTeamChams, false);
		C_ADD_VARIABLE(bool, m_bEnableTeamChamsIgnoreZ, false);
		C_ADD_VARIABLE(bool, m_bEnableSelfChams, false);
		C_ADD_VARIABLE(bool, m_bEnableSelfWeaponChams, false);
		C_ADD_VARIABLE(bool, m_bEnableSelfAttachmentChams, false);
		C_ADD_VARIABLE(bool, m_bEnableSelfArmsChams, false);
		C_ADD_VARIABLE(bool, m_bRagdollChamsEnemy, false);
		C_ADD_VARIABLE(bool, m_bEnableDroppedWeaponChams, false);

		C_ADD_VARIABLE(bool, m_bEnableOOFArrows, false);
		C_ADD_VARIABLE(Color, m_colOOFArrows, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(int, m_bOOFArrowsWidth, 100);
		C_ADD_VARIABLE(int, m_bOOFArrowsHeight, 100);

		C_ADD_VARIABLE(bool, m_bCustomModelChanger, false);

		C_ADD_VARIABLE(bool, m_bViewmodelChanger, false);

		C_ADD_VARIABLE(float, flXOffset, 1.f);
		C_ADD_VARIABLE(float, flYOffset, 1.f);
		C_ADD_VARIABLE(float, flZOffset, 1.f);
		C_ADD_VARIABLE(float, flXRotation, 0.f);
		C_ADD_VARIABLE(float, flYRotation, 0.f);
		C_ADD_VARIABLE(float, flZRotation, 0.f);
		C_ADD_VARIABLE(int, iViewmodelFov, 68);

		C_ADD_VARIABLE_VECTOR(bool, INFO_FLAG_MAX, m_vecFlags, false)

		C_ADD_VARIABLE(Color, m_colEnemyChams, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colEnemyChamsIgnoreZ, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(Color, m_colTeamChams, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colTeamChamsIgnoreZ, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(Color, m_colSelfChams, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colAttachmentChams, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colSelfWeaponChams, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colSelfArmsChams, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(Color, m_colDroppedWeaponChams, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(Color, m_colBoxESP, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colESPOutline, Color(0, 0, 0, 150));

		C_ADD_VARIABLE(Color, m_colSkeletonEsp, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colBacktrackSkeletonEsp, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bArmorBarGradient, false);

		C_ADD_VARIABLE(Color, m_colArmorBarGradient, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colNameEsp, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colWeaponEsp, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colWeaponIcon, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colArmorBar, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colHealthBarEsp, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colHealthBarGradient, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(int, m_iWorldFov, 90);
		C_ADD_VARIABLE(bool, m_bForceZoom, false);

		C_ADD_VARIABLE(bool, m_bBacktrackChams, false);
		C_ADD_VARIABLE(bool, m_bBacktrackChamsIgnoreZ, false);
		C_ADD_VARIABLE(Color, m_colBacktrackChams, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colBacktrackChamsIgnoreZ, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(int, m_iBacktrackChamMaterialEnemy, VISUAL_MATERIAL_LATEX);

		C_ADD_VARIABLE(bool, m_bGlow, false);
		C_ADD_VARIABLE(bool, m_bGlowOnRagdoll, false);
		C_ADD_VARIABLE(Color, m_colGlow, Color(255, 255, 255, 25));

		C_ADD_VARIABLE(bool, m_bRagdollGravity, false);
		C_ADD_VARIABLE(float, m_flRagdollGravityScale, -500.0f);

		C_ADD_VARIABLE(Color, m_colScope, Color(255, 255, 255, 0));
		C_ADD_VARIABLE(Color, m_colScopeOutSide, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(int, m_iScopeLength, 120);
		C_ADD_VARIABLE(int, m_iScopeThickness, 1);
		C_ADD_VARIABLE(int, m_iScopeGap, 30);

		C_ADD_VARIABLE(int, m_iChamMaterialEnemy, VISUAL_MATERIAL_LATEX);
		C_ADD_VARIABLE(int, m_iChamMaterialEnemyIgnoreZ, VISUAL_MATERIAL_LATEX);
		C_ADD_VARIABLE(int, m_iChamMaterialLocal, VISUAL_MATERIAL_LATEX);
		C_ADD_VARIABLE(int, m_iChamMaterialLocalWeapon, VISUAL_MATERIAL_LATEX);
		C_ADD_VARIABLE(int, m_iChamMaterialLocalArms, VISUAL_MATERIAL_LATEX);
		C_ADD_VARIABLE(int, m_iChamMaterialLocalAttachments, VISUAL_MATERIAL_LATEX);
		C_ADD_VARIABLE(int, m_iChamMaterialDroppedWeapons, VISUAL_MATERIAL_LATEX);

		bool m_bThirdPersonEnabled = false;

		C_ADD_VARIABLE(bool, m_bEditFlags, false);

		C_ADD_VARIABLE(bool, m_bDamageIndicators, false);
		C_ADD_VARIABLE(Color, m_colDamageIndicator, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bEnableOnShotChams, false);
		C_ADD_VARIABLE(bool, m_bOnShotChamsIgnoreZ, false);
		C_ADD_VARIABLE(int, m_iOnShotChamMaterial, VISUAL_MATERIAL_LATEX);
		C_ADD_VARIABLE(int, m_iOnShotChamMaterialIgnoreZ, VISUAL_MATERIAL_LATEX);
		C_ADD_VARIABLE(Color, m_colOnShotChamsIgnoreZ, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colOnShotChams, Color(255, 255, 255, 255));
	}; VisualsVariables_t m_Visuals;


	struct MovementVariables_t
	{
		C_ADD_VARIABLE(bool, m_bEnableBunnyHop, false);
		C_ADD_VARIABLE(bool, m_bEnableJumpBug, false);
		C_ADD_VARIABLE(bool, m_bAirDuck, false)
		C_ADD_VARIABLE(bool, m_bEnableAutoStrafe, false);
		C_ADD_VARIABLE(bool, m_bEnableAutoStrafeExploit, false);
		C_ADD_VARIABLE(bool, m_bStraigthThrow, false);
		C_ADD_VARIABLE(bool, m_bEnableQuickStop, false);
		C_ADD_VARIABLE(KeyBind_t, m_kEdgeJump, KeyBind_t(0, HOLD));
	}; MovementVariables_t m_Movement;
	
	struct MiscVariables_t
	{
		C_ADD_VARIABLE(bool, m_bEnableMisc, false);
		C_ADD_VARIABLE(bool, m_bAntiUntrusted, false);
		C_ADD_VARIABLE(bool, m_bAutoAccept, false);
		C_ADD_VARIABLE(bool, m_bUnlockLoadout, false);
		C_ADD_VARIABLE(bool, m_bPerformanceMode, false);
		C_ADD_VARIABLE(bool, m_bRemoveNoSteamLogonKick, true);
		C_ADD_VARIABLE(bool, m_bDebug, false);
		C_ADD_VARIABLE(bool, m_bHitlogs, false);
		C_ADD_VARIABLE(bool, m_bDistantScoutSound, false);
		C_ADD_VARIABLE(bool, m_bHitSound, false);
		C_ADD_VARIABLE(float, m_flHitsoundVolume, 1.f);
		C_ADD_VARIABLE(int, m_iHitSoundIndex, 0);
		C_ADD_VARIABLE_VECTOR( bool, EWatermark::WATERMARK_MAX, m_vbWatermark, false );
		C_ADD_VARIABLE(bool, m_bHitMarker, false);

		C_ADD_VARIABLE(bool, m_bAutoPurchase, false);
		C_ADD_VARIABLE(int, m_iAutoPurchase1, 0);
		C_ADD_VARIABLE(int, m_iAutoPurchase2, 0);
		C_ADD_VARIABLE_VECTOR(bool, EAutoPurchase::AUTOBUY, m_uAutoPurchase3, false);

		C_ADD_VARIABLE(bool, m_bAutoPistol, false);
		C_ADD_VARIABLE(bool, m_bUnlockInventory, false);
		C_ADD_VARIABLE(Color, m_colHitMarker, Color(255, 255, 255, 255));
	}; MiscVariables_t m_Misc;

	struct WorldEffects_t
	{
		C_ADD_VARIABLE(bool, m_bDrawImpacts, false);
		C_ADD_VARIABLE(Color, m_colDrawImpactsColor, Color(255,255,255,255));

		C_ADD_VARIABLE(float, m_fUnknown, 1);

		C_ADD_VARIABLE(bool, m_bLocalBulletTracers, false);
		C_ADD_VARIABLE(bool, m_bkillEffectMode, false);
		C_ADD_VARIABLE(Color, m_colLocalBulletTracers, Color(255,255,255,255));

		C_ADD_VARIABLE(bool, m_bTeamBulletTracers, false);
		C_ADD_VARIABLE(Color, m_colTeamBulletTracers, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bEnemyBulletTracers, false);
		C_ADD_VARIABLE(Color, m_colEnemyBulletTracers, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bCustomSkybox, false);
		C_ADD_VARIABLE(bool, m_bRemoveSkybox, false);
		C_ADD_VARIABLE(int, m_iSelectedSkybox, 0);
		C_ADD_VARIABLE(Color, m_colSkybox, Color(255,255,255,255));

		C_ADD_VARIABLE(float, m_flExposure, 20);
		C_ADD_VARIABLE(bool, m_bCustomWorldParticles, false);
		C_ADD_VARIABLE(int, m_iSelectedCustomWorldParticles, ECustomWorldParticles::WORLD_PARTICLES_NONE);
		C_ADD_VARIABLE(int, m_iSelectedCustomWorldParticleDensity, 75);

		C_ADD_VARIABLE(bool, m_bAspectRatioChanger, false);
		C_ADD_VARIABLE(int, m_iAspectRatio, 0);

		C_ADD_VARIABLE(bool, m_bHitEffect, false);

	}; WorldEffects_t m_WorldEffects;

	struct WorldVisuals_t
	{
		C_ADD_VARIABLE(bool, m_bDrawInferno, false);
		C_ADD_VARIABLE(Color, m_colInferno, Color(255,0,0,120));
		C_ADD_VARIABLE(bool, m_bDrawInfernoIndicators, false);

		C_ADD_VARIABLE(Color, m_colCloudColor, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colSunColor, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_colWorldColor, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bParticleModulation, false);
		C_ADD_VARIABLE(Color, m_colParticleColor, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bTaserStimModulation, false);
		C_ADD_VARIABLE(Color, m_colTaserStim, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bGrenadeModulation, false);
		C_ADD_VARIABLE(Color, m_colGrenade, Color(255, 255, 255, 255));
		
		C_ADD_VARIABLE(int, m_nWorldExposure, 100);

		C_ADD_VARIABLE(Color, m_colLighting, Color(255, 255, 255, 41));

		C_ADD_VARIABLE(bool, m_bWorldModulation, false);

		C_ADD_VARIABLE(bool, m_bWeaponNames, false);
		C_ADD_VARIABLE(bool, m_bDrawBombName, false);
		C_ADD_VARIABLE(bool, m_bDrawGrenadeNames, false);
		C_ADD_VARIABLE(bool, m_bDrawGrenadeTracers, false);
		C_ADD_VARIABLE(bool, m_bGrenadePrediction, false);
		C_ADD_VARIABLE(bool, m_bGrenadeProximityWarning, false);
		C_ADD_VARIABLE(bool, m_bGrenadePredictionEffect, false);
		C_ADD_VARIABLE(Color, m_cGrenadeProximityWraning, Color(255, 255, 255, 255));
		C_ADD_VARIABLE(Color, m_cGrenadePrediction, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(Color, m_cColorFog, Color(255, 255, 255, 255));

		C_ADD_VARIABLE(bool, m_bEnableFog, false);

		C_ADD_VARIABLE(float, m_fStartFog, 50.f);
		C_ADD_VARIABLE(float, m_fEndFog, 1000.f);
		C_ADD_VARIABLE(float, m_fFogFallof, 1.f);

		C_ADD_VARIABLE_VECTOR(bool, ERemovals::REMOVE_MAX, m_vbRemovals, false);

		C_ADD_VARIABLE(bool, m_bWeaponEsp, false);
	}; WorldVisuals_t m_WorldVisuals;
};

inline CVariables g_Variables;