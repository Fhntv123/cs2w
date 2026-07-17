#pragma once

namespace Tabs
{
	inline int m_iCurrentTab = 4;
	inline int m_iCurrentlegitbotTab = 4;

	inline const char* HitboxNames[] = 
	{
		"Head",
		"Chest",
		"Stomach",
		"Pelvis",
		"Arms",
		"Legs",
		"Feet"
	};

	inline const char* LegitbotConditions[] =
	{
		"Visible",
		"No smoke",
		"No flash"
	};

	inline const char* szRemovals[] =
	{
		"Smoke",
		"Flash",
		"Torso",
		"Crosshair",
		"Overhead local",
		"Overhead team",
		"Team intro",
		"Fog brightness",
		"Scope zoom",
	};

	inline const char* szOverlay[] =
	{
		"Keybind list",
		"Spectators list",
		"Watermark",
	};

	inline const char* szAutostopModes[ ] =
	{
		"Early",
		"In air",
	};

	inline const char* szHistoryAimModes[] =
	{
		"Enable backtrack",
		"Backtrack only",
	};

	inline const char* WeaponNames[] =
	{
		"Pistols",
		"Deagle",
		"Revolver",
		"SMGs",
		"LMGs",
		"Assault Rifles",
		"Shotguns",
		"SSG-08",
		"Auto snipers",
		"AWP"
	};

	inline const char* TargetTypes[] =
	{
		"Damage",
		"Hitchance ",
		"Health"
	};

}

namespace Fonts
{
	inline ImFont* Default;
	inline ImFont* GunIcons;
	inline ImFont* ESP;
	inline ImFont* Verdana;
	inline ImFont* VerdanaBold;
	inline ImFont* VerdanaEsp;
	inline ImFont* SegoeUIBold;
	inline ImFont* SegoeUIBold18;
	inline ImFont* SegoeUIBold25;
	inline ImFont* SegoeUIBold18_ITALIC;
	inline ImFont* icons[3];
	inline ImFont* logo;
	inline ImFont* inter[3];
	inline ImFont* overlay;
	inline ImFont* overlay2;
	inline ImFont* overlay3;
	inline ImFont* imFA = nullptr;
}

namespace Gui
{
	inline bool m_bInitialized = false;
	inline bool m_bOpen = false;
	inline bool m_bInputActive = false;
	inline ImVec2 m_vecScreenSize = ImVec2(0.0f, 0.0f);

	inline int m_iAverageFPS = 0;

	inline ImColor colThemeColor = ImColor(160, 125, 255);

	void DrawHitLogs();
	void Initialize(unsigned int uFontFlags = 0x0);
	void Update(ImGuiIO& io);
	void RenderSpectatorList();
	void RenderWatermark();
	void Render();
}