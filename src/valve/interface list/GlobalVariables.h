#pragma once

class IGlobalVariables
{
public:
	float m_flRealTime;
	int32_t m_nFrameCount;
	float m_flFrameTime;
	float m_flFrameTime2;
	int32_t m_nMaxClients;
	int16_t m_nTickBase;
	char pad_1a[26 - 4];
	float m_flIntervalPerSubTick;
	float m_flCurrentTime;
	float m_flCurrentTime2;
	char pad_c[4];
	float m_flSomeTickFrac;
	char pad_d[4];
	int32_t m_nTickCount;
	float m_flTickFraction;
	char pad[4];
	_DWORD m_nUnkFlag;
	char pad_134[300];
	char* m_szCurrentMap;
	char* m_szCurrentMapName;
};