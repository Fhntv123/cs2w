#pragma once

#define MULTIPLAYER_BACKUP 150

class CCSInputMessage
{
public:
	std::int32_t m_nFrameTickCount; //0x0000
	float m_flFrameTickFraction; //0x0004
	std::int32_t m_nPlayerTickCount; //0x0008
	float m_flPlayerTickFraction; //0x000C
	QAngle m_angViewAngles; //0x0010
	Vector m_vecShootPosition; //0x001C 
	std::int32_t m_nTargetIndex; //0x0028
	Vector m_vecTargetHeadPosition; //0x002C What's this???
	Vector m_vecTargetAbsOrigin; //0x0038
	Vector m_vecTargetAngle; //0x0044
	std::int32_t m_nsvShowHitRegistration; //0x0050
	std::int32_t m_nEntryIndexMax; //0x0054
	std::int32_t m_nIndexUnk; //0x0058
	int idekjustpadding;
};
static_assert(sizeof(CCSInputMessage) == 0x60);

class CTinyMoveStepData
{
public:
	float m_flWhen; //0x0000
	MEM_PAD(0x4); //0x0004
	std::uint64_t m_nButton; //0x0008
	bool m_bPressed; //0x0010
	MEM_PAD(0x7); //0x0011
}; //Size: 0x0018

class CMoveStepButtons
{
public:
	std::uint64_t m_nKeyboardPressed; //0x0000
	std::uint64_t m_nMouseWheelheelPressed; //0x0008
	std::uint64_t m_nUnPressed; //0x0010
	std::uint64_t m_nKeyboardCopy; //0x0018
}; //Size: 0x0020

class CExtendedMoveData : public CMoveStepButtons
{
public:
	float m_flForwardMove; //0x0020
	float m_flSideMove; //0x0024
	float m_flUpMove; //0x0028
	std::int32_t m_nMouseDeltaX; //0x002C
	std::int32_t m_nMouseDeltaY; //0x0030
	std::int32_t m_nAdditionalStepMovesCount; //0x0034
	CTinyMoveStepData m_TinyMoveStepData[12]; //0x0038
	Vector m_vecViewAngle; //0x0158
	std::int32_t m_nTargetHandle; //0x0164
}; //Size:0x0168
class c_button_data
{
public:
	std::uint64_t flag;
	char* name;
	std::byte m_pad0[0x8];
	float value;
	std::byte m_pad1[0x4];
	float when_start;
	float when_end;
	std::byte m_pad2[0x4];
	int command_count;
	std::byte m_pad3[0x60];
}; //Size: 0x0048

class CSubtickInput {
public:
	float m_flWhen;
	float m_flDelta;
	uint64_t m_nButton;
	bool m_bPressed;
	char pad_0011[3];
	int m_nMouseInput;
	Vector angles;
};

class CCSGOInput
{
public:
	char pad_0000[16]; //0x0000
	c_button_data* buttons_data[63]; //0x0010
	char pad_0208[72]; //0x0208
	bool bBlockShot;
	bool m_bInThirdPerson;
	MEM_PAD(0x6);
	QAngle angThirdPersonAngles;
	MEM_PAD(0x14);
	uint64_t nKeyboardPressed;
	uint64_t nMouseWheelPressed;
	uint64_t nUnPressed;
	uint64_t nKeyboardCopy;
	float flForwardMove;
	float flSideMove;
	float flUpMove;
	Vector2D nMousePos;
	int32_t SubticksCount;
	CSubtickInput Subticks[32];
	Vector vecViewAngle;
	int32_t nTargetHandle;
	MEM_PAD(0x230);
	int32_t nAttackStartHistoryIndex1;
	int32_t nAttackStartHistoryIndex2;
	int32_t nAttackStartHistoryIndex3;
	char pad_061C[4]; //0x061C
	int32_t m_nMessageSize; //0x0620
	char pad_0624[6]; //0x0624
	CCSInputMessage* m_pMessage; //0x0630

	void SetViewAngle(const QAngle angView)
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnSetViewAngle != nullptr);
#endif

		Functions::fnSetViewAngle(this, 0, angView);
	}

	QAngle GetViewAngles()
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnGetViewAngles != nullptr);
#endif

		return *reinterpret_cast<QAngle*>(Functions::fnGetViewAngles(this, 0));
	}

	inline CSubtickMoveStep* CreateNewSubTickMoveStep(google::protobuf::RepeatedPtrField<CSubtickMoveStep>* pMutableSubtickMoveStep)
	{
		if (!pMutableSubtickMoveStep)
			return nullptr; 

		auto* arena = pMutableSubtickMoveStep->GetArena();
		return Functions::fnCreateNewSubtickMoveStep(arena);
	}


	CUserCmd* GetUserCmd()
	{
		typedef uintptr_t(__fastcall* GetLocalControllerByInternalIdFn)(int);
		static GetLocalControllerByInternalIdFn GetLocalControllerByInternalId = (GetLocalControllerByInternalIdFn)Memory::FindPattern(CLIENT_DLL, "48 83 ec ?? 83 f9 ?? 75 ?? 48 8b 0d ?? ?? ?? ?? 48 8d 54 24 ?? 48 8b 01 ff 90 ?? ?? ?? ?? 8b 08 48 63 c1 48 8d 0d ?? ?? ?? ?? 48 8b 04 c1 48 83 c4 ?? c3 cc cc cc cc cc cc cc cc cc cc cc cc cc 48 83 ec ?? 83 f9");
		typedef uintptr_t(__fastcall* SetupCmdFn)(uintptr_t);
		static SetupCmdFn SetupCmd = (SetupCmdFn)Memory::FindPattern(CLIENT_DLL, "48 83 EC 28 E8 ?? ?? ?? ?? 8B 80");
		typedef uintptr_t(__fastcall* GetControllerCmdFn)(uintptr_t, uintptr_t);
		static GetControllerCmdFn GetControllerCmd = (GetControllerCmdFn)Memory::FindPattern(CLIENT_DLL, "40 53 48 83 EC 20 8B DA E8 ?? ?? ?? ?? 4C");

		auto Controller = GetLocalControllerByInternalId(0);
		return (CUserCmd*)GetControllerCmd(Controller, SetupCmd(Controller));
	}
};
//static_assert(sizeof(CCSGOInput) == 0x5FD0);