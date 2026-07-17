#pragma once

class CGameEvent;

struct OnShotSkeleton_t
{
	C_CSPlayerPawn* m_pPawn;
	CCSPlayerController* m_pController;
	BoneData_t m_Matrix[128];
	float m_flTimeAdded;
};

class CShotHandler {
public:
	struct PlayerHurtRecord_t {
		int m_iHitGroup;
		Vector m_vecBoneOrigin;
		Vector m_vecImpactPosition;
		float m_flTime;
	};
public:
	std::deque<PlayerHurtRecord_t> m_HitRecords;
public:
	void RenderHitMarker(const ImVec2& pos, float size = 6.0f);
	void OnPlayerHurt(CGameEvent* pEvent);
	void OnPlayerDeath(CGameEvent* pEvent);
	void OnBulletImpact(CGameEvent* pEvent);
	void OnPresent();
};

inline auto g_ShotHandler = std::make_unique<CShotHandler>();
