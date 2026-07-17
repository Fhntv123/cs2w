#pragma once

namespace GrenadeESP
{
	void Draw(EntityObject_t& object);
	int CalculateGrenadeDamage(C_CSPlayerPawn* pLocal, C_BaseGrenade* pGrenade, float distance);
}