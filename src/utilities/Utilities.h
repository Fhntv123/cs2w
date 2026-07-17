#pragma once

// Used to sort Vectors in ccw order about a pivot.
static float CCW(const Vector& a, const Vector& b, const Vector& c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

struct ccwSorter
{
	const Vector& pivot;

	ccwSorter(const Vector& inPivot) : pivot(inPivot) { }

	bool operator()(const Vector& a, const Vector& b) {
		return CCW(pivot, a, b) < 0;
	}
};

static bool IsLeftOfPoint(const Vector& a, const Vector& b)
{
	return (a.x < b.x || (a.x == b.x && a.y < b.y));
}

namespace Utilities
{
	// copy data to clipboard
	void CopyToClipboard(std::string strData);
	// calculate gift wrapping
	std::vector<Vector> GiftWrappingAlgorithm(std::vector<Vector> vecVector);
	// check if line goes through smoke
	bool LineGoesThroughSmoke(const Vector& vecStart, const Vector& vecEnd, const float& flMaxDensity);
	// set local player ready
	void SetLocalPlayerReady();
	// flash window
	void FlashWindow(HWND pWindow);
	// load files from game
	void* LoadFileForMe(const std::string_view strPath, int& nFileSize);
	// find HUD elements
	std::uintptr_t* FindHudElement(const char* szName);
	// print a message to hud
	void HudChatPrint(const char* szString);
	// force hold knife
	bool ForceHoldKnife();
	// find parent hitgroup from hitbox id
	int HitboxToHitgroup(EHitBoxes index);

	std::string GetHitgroupName(int nHitgroupIndex);

	void PlayVSound(const char* szSoundPath, float flVolume);

	// event listener pointer
	inline CEventListener m_EventListener;
}