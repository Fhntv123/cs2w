#pragma once

class CCSPlayerInventory;
class CCSInventoryManager
{
public:
    static CCSInventoryManager* GetInventoryManager();

    bool EquipItemInLoadout(int nTeam, int nSlot, uint64_t nItemID)
    {
        return Memory::CallVFunc<bool, 60U>(this, nTeam, nSlot, nItemID);
    }

    CCSPlayerInventory* GetLocalInventory()
    {
        return Memory::CallVFunc < CCSPlayerInventory*, 63U>(this);
    }
};