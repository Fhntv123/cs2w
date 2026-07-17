#pragma once

#include "../Schema.h"

class CViewVectors
{
public:
	Vector m_vecView; //0x0000
	Vector m_vecHullMin; //0x000C
	Vector m_vecHullMax; //0x0018
	Vector m_vecDuckHullMin; //0x0024
	Vector m_vecDuckHullMax; //0x0030
	Vector m_vecDuckView; //0x003C
	Vector m_vecOBSHullMin; //0x0048
	Vector m_vecOBSHullMax; //0x0054
	Vector m_vecDeadView; //0x0060
}; //Size: 0x006C

class C_CSGameRules
{
public:
	CViewVectors* GetViewVectors()
	{
		return Memory::CallVFunc<CViewVectors*, 38U>(this);
	}
public:
	SCHEMA_ADD_FIELD(bool, m_bFreezePeriod, "C_CSGameRules->m_bFreezePeriod");
	SCHEMA_ADD_FIELD(bool, m_bWarmupPeriod, "C_CSGameRules->m_bWarmupPeriod");
	SCHEMA_ADD_FIELD(bool, m_bTeamIntroPeriod, "C_CSGameRules->m_bTeamIntroPeriod");
	SCHEMA_ADD_FIELD(bool, m_bIsValveDS, "C_CSGameRules->m_bIsValveDS");
	SCHEMA_ADD_FIELD(bool, m_bHasMatchStarted, "C_CSGameRules->m_bHasMatchStarted");
	SCHEMA_ADD_FIELD(float, m_fRoundStartTime, "C_CSGameRules->m_fRoundStartTime");
};