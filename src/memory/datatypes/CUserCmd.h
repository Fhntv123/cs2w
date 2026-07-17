#pragma once
#include "../../valve/protobufs/cs_usercmd.pb.h"

// enums server.dll
enum ECommandButtons : std::uint64_t
{
	IN_ATTACK = 1 << 0,
	IN_JUMP = 1 << 1,
	IN_DUCK = 1 << 2,
	IN_FORWARD = 1 << 3,
	IN_BACK = 1 << 4,
	IN_USE = 1 << 5,
	IN_TURNLEFT = 1 << 7,
	IN_TURNRIGHT = 1 << 8,
	IN_MOVELEFT = 1 << 9,
	IN_MOVERIGHT = 1 << 10,
	IN_ATTACK2 = 1 << 11,
	IN_RELOAD = 1 << 13,
	IN_SPRINT = 1 << 16,
	IN_SHOWSCORES = 1ULL << 33,
	IN_ZOOM = 1ULL << 34,
	IN_LOOKATWEAPON = 1ULL << 35
};

struct CInButtonState
{
public:
	void* pVTable; // 0x0
	std::uint64_t m_nValue; // 0x8
	std::uint64_t m_nValueChanged; // 0x10
	std::uint64_t m_nValueScroll; // 0x18

	enum EButtonState : int8_t {
		IN_BUTTON_UP = 0,
		IN_BUTTON_DOWN = 1,
		IN_BUTTON_DOWN_UP = 2,
		IN_BUTTON_UP_DOWN = 3,
		IN_BUTTON_UP_DOWN_UP = 4,
		IN_BUTTON_DOWN_UP_DOWN = 5,
		IN_BUTTON_DOWN_UP_DOWN_UP = 6,
		IN_BUTTON_UP_DOWN_UP_DOWN = 7
	};

    void SetButtonState(const uint64_t uValue, EButtonState eButtonState) {
        // 0 deðeri ile crash oluþmasýný önle
        if (!uValue)
            return;

        switch (eButtonState) {
        case EButtonState::IN_BUTTON_UP:
            m_nValue &= ~uValue;
            m_nValueChanged &= ~uValue;
            m_nValueScroll &= ~uValue;
            break;

        case EButtonState::IN_BUTTON_DOWN:
            m_nValue |= uValue;
            m_nValueChanged &= ~uValue;
            m_nValueScroll &= ~uValue;
            break;

        case EButtonState::IN_BUTTON_DOWN_UP:
            m_nValue &= ~uValue;
            m_nValueChanged |= uValue;
            m_nValueScroll &= ~uValue;
            break;

        case EButtonState::IN_BUTTON_UP_DOWN:
            m_nValue |= uValue;
            m_nValueChanged |= uValue;
            m_nValueScroll &= ~uValue;
            break;

        case EButtonState::IN_BUTTON_UP_DOWN_UP:
            m_nValue &= ~uValue;
            m_nValueChanged &= ~uValue;
            m_nValueScroll |= uValue;
            break;

        case EButtonState::IN_BUTTON_DOWN_UP_DOWN:
            m_nValue |= uValue;
            m_nValueChanged &= ~uValue;
            m_nValueScroll |= uValue;
            break;

        case EButtonState::IN_BUTTON_DOWN_UP_DOWN_UP:
            m_nValue &= ~uValue;
            m_nValueChanged |= uValue;
            m_nValueScroll |= uValue;
            break;

        case EButtonState::IN_BUTTON_UP_DOWN_UP_DOWN:
            m_nValue |= uValue;
            m_nValueChanged |= uValue;
            m_nValueScroll |= uValue;
            break;

        default:
            // bilinmeyen enum deðerlerinden korun
            break;
        }
    }
   
};
static_assert(sizeof(CInButtonState) == 0x20);
class CUserCmd
{
public:
    void* __vfptr; //0x0
	int64  m_nCommandNumber; //0x8
    CSGOUserCmdPB m_csgoUserCmd; // 0xC
	CInButtonState m_nButtons; // 0x58
	MEM_PAD(0x8);
	double m_dbLastServerTime;
	bool m_bHasBeenPredicted; // 0x88
	int m_iFlag;
	int m_iCommandType;

};
static_assert(sizeof(CUserCmd) == 0x98);