#include "../../Precompiled.h"

CCSInventoryManager* CCSInventoryManager::GetInventoryManager()
{
#ifdef CS_PARANOID
	CS_ASSERT(Functions::fnGetInventoryManager != nullptr);
#endif

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnGetInventoryManager);
}