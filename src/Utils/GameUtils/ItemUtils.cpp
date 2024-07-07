//
// Created by vastrakai on 7/6/2024.
//

#include "ItemUtils.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/Item.hpp>
#include <SDK/Minecraft/Inventory/ItemStack.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>

int ItemUtils::getItemValue(ItemStack* item) {
    int value = 0;
    if (!item->mItem) return -1;
    switch (item->getItem()->getItemType())
    {
    case SItemType::Helmet:
    case SItemType::Chestplate:
    case SItemType::Leggings:
    case SItemType::Boots:
        value = item->getEnchantValue(Enchant::PROTECTION) + item->getEnchantValue(Enchant::FIRE_PROTECTION);
        break;
    case SItemType::Sword:
        value = item->getEnchantValue(Enchant::SHARPNESS);
        break;
    case SItemType::Pickaxe:
    case SItemType::Axe:
    case SItemType::Shovel:
        value = item->getEnchantValue(Enchant::EFFICIENCY);
        break;
    case SItemType::None:
        break;
    }

    // If the item is armor, add the getArmorValue function
    if (item->getItem()->getItemType() >= SItemType::Helmet && item->getItem()->getItemType() <= SItemType::Boots)
        value += item->getItem()->mProtection;

    // If the item is a weapon, add the getItemTier function
    if (item->getItem()->getItemType() == SItemType::Sword || item->getItem()->getItemType() == SItemType::Pickaxe || item->getItem()->getItemType() == SItemType::Axe || item->getItem()->getItemType() == SItemType::Shovel)
        value += item->getItem()->getItemTier();

    return value;
}

int ItemUtils::getBestItem(SItemType type)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    auto supplies = player->getSupplies();
    auto container = supplies->getContainer();

    int bestSlot = -1;
    int bestValue = 0;

    for (int i = 0; i < 36; i++)
    {
        auto item = container->getItem(i);
        if (!item->mItem) continue;

        if (item->getItem()->getItemType() == type)
        {
            int value = getItemValue(item);
            if (value > bestValue)
            {
                bestValue = value;
                bestSlot = i;
            }
        }
    }

    return bestSlot;
}

std::unordered_map<SItemType, int> ItemUtils::getBestItems()
{
    std::unordered_map<SItemType, int> bestItemsResult;
    std::vector<std::pair<SItemType, int>> bestItems;

    // instead, you need to iterate over the inventory once manually.
    auto player = ClientInstance::get()->getLocalPlayer();
    auto supplies = player->getSupplies();
    auto container = supplies->getContainer();

    for (int i = 0; i < 36; i++)
    {
        auto item = container->getItem(i);
        if (!item->mItem) continue;

        switch (item->getItem()->getItemType())
        {
        case SItemType::Helmet:
        case SItemType::Chestplate:
        case SItemType::Leggings:
        case SItemType::Boots:
        case SItemType::Sword:
        case SItemType::Pickaxe:
        case SItemType::Axe:
        case SItemType::Shovel:
            bestItems.emplace_back(item->getItem()->getItemType(), i);
            break;
        }
    }

    for (const auto& [type, slot] : bestItems)
    {
        auto item = container->getItem(slot);
        if (!item->mItem) continue;

        int value = getItemValue(item);

        if (value > getItemValue(container->getItem(bestItemsResult[type])))
        {
            bestItemsResult[type] = slot;
        }
    }

    return bestItemsResult;
}

