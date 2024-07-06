//
// Created by vastrakai on 7/5/2024.
//

#include "PlayerInventory.hpp"

#include <libhat/Access.hpp>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>

ItemStack* Inventory::getItem(int slot)
{
    static auto vFunc = OffsetProvider::Inventory_getItem;
    return MemUtils::callVirtualFunc<ItemStack*, int>(vFunc, this, slot);
}

Inventory* PlayerInventory::getContainer()
{
    return hat::member_at<Inventory*>(this, OffsetProvider::PlayerInventory_mContainer);
}
