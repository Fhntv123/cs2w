#pragma once

#include "../../Precompiled.h"

class CMisc
{
public:
	void AutoPurchase();
	void AutoPistol();
	void StraightThrow(const QAngle& pViewAngles);
	float GetMaxPlayerSpeed();
	void AccurateWalk(CUserCmd* pCmd);
	void PenetrationCrosshair();
};

const auto g_Misc = std::make_unique<CMisc>();