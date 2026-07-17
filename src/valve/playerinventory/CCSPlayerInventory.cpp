#include "../../Precompiled.h"

CCSPlayerInventory* CCSPlayerInventory::GetPlayerInventory()
{
	CCSInventoryManager* pInventoryManager = CCSInventoryManager::GetInventoryManager();
	if (!pInventoryManager)
		return nullptr;

	return pInventoryManager->GetLocalInventory();
}

static CGCClientSharedObjectTypeCache* CreateBaseTypeCache(CCSPlayerInventory* pInventory)
{
	CGCClientSystem* pGCClientSystem = CGCClientSystem::GetClientSystem();
	if (!pGCClientSystem)
		return nullptr;

	CGCClient* pGCClient = pGCClientSystem->GetCGCClient();
	if (!pGCClient)
		return nullptr;

	CGCClientSharedObjectCache* pSOCache = pGCClient->FindSOCache(pInventory->GetOwner());
	if (!pSOCache)
		return nullptr;

	return pSOCache->CreateBaseTypeCache(k_EEconTypeItem);
}

bool CCSPlayerInventory::AddEconItem(CEconItem* pItem)
{
	// Helper function to aid in adding items.
	if (!pItem)
		return false;

	CGCClientSharedObjectTypeCache* pSOTypeCache = ::CreateBaseTypeCache(this);
	if (!pSOTypeCache || !pSOTypeCache->AddObject(reinterpret_cast<CSharedObject*>(pItem)))
		return false;

	SOCreated(GetOwner(), reinterpret_cast<CSharedObject*>(pItem), eSOCacheEvent_Incremental);
	return true;
}

void CCSPlayerInventory::RemoveEconItem(CEconItem* pItem)
{
	// Helper function to aid in removing items.
	if (!pItem)
		return;

	CGCClientSharedObjectTypeCache* pSOTypeCache = ::CreateBaseTypeCache(this);
	if (!pSOTypeCache)
		return;

	const CUtlVector<CEconItem*>& pSharedObjects = pSOTypeCache->GetVecObjects<CEconItem*>();
	if (!pSharedObjects.Exists(pItem))
		return;

	SODestroyed(GetOwner(), reinterpret_cast<CSharedObject*>(pItem), eSOCacheEvent_Incremental);
	pSOTypeCache->RemoveObject(reinterpret_cast<CSharedObject*>(pItem));

	pItem->Destruct();
}

std::pair<uint64_t, uint32_t> CCSPlayerInventory::GetHighestIDs()
{
	std::uint64_t uMaxItemID = 0;
	std::uint32_t uMaxInventoryID = 0;

	CGCClientSharedObjectTypeCache* pSOTypeCache = ::CreateBaseTypeCache(this);
	if (pSOTypeCache)
	{
		const CUtlVector<CEconItem*>& vecItems = pSOTypeCache->GetVecObjects<CEconItem*>();
		for (CEconItem* it : vecItems)
		{
			if (!it)
				continue;

			// Checks if item is default.
			if ((it->m_uID & 0xF000000000000000) != 0)
				continue;

			uMaxItemID = std::max(uMaxItemID, it->m_uID);
			uMaxInventoryID = std::max(uMaxInventoryID, it->m_uInventory);
		}
	}

	return std::make_pair(uMaxItemID, uMaxInventoryID);
}

C_EconItemView* CCSPlayerInventory::GetItemViewForItem(uint64_t itemID)
{
	C_EconItemView* pEconItemView = nullptr;
	const CUtlVector<C_EconItemView*>& pItems = GetItemVector();
	for (C_EconItemView* it : pItems)
	{
		if (it && it->m_iItemID() == itemID)
		{
			pEconItemView = it;
			break;
		}
	}

	return pEconItemView;
}

CEconItem* CCSPlayerInventory::GetSOCDataForItem(uint64_t itemID)
{
	CEconItem* pSOCData = nullptr;
	CGCClientSharedObjectTypeCache* pSOTypeCache = ::CreateBaseTypeCache(this);
	if (pSOTypeCache)
	{
		const CUtlVector<CEconItem*>& vecItems = pSOTypeCache->GetVecObjects<CEconItem*>();
		for (CEconItem* it : vecItems)
		{
			if (it && it->m_uID == itemID)
			{
				pSOCData = it;
				break;
			}
		}
	}

	return pSOCData;
}