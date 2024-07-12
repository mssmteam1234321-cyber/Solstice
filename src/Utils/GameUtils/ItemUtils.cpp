//
// Created by vastrakai on 7/6/2024.
//

#include "ItemUtils.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/Inventory/Item.hpp>
#include <SDK/Minecraft/Inventory/ItemStack.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/World/Block.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>
#include <Utils/StringUtils.hpp>

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

    int bestSlot = supplies->mSelectedSlot;
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

// Blacklisted block name
constexpr std::array<const char*, 5> blacklistedBlocks = {
    "netherreactor",
    "boombox",
    "lilypad",
    "torch",
    "fence"
};

int ItemUtils::getAllPlaceables(bool hotbarOnly)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    int placeables = 0;

    for (int i = 0; i < 36; i++)
    {
        ItemStack* stack = player->getSupplies()->getContainer()->getItem(i);
        if (!stack->mItem) continue;
        Item* item = stack->getItem();
        if (hotbarOnly && i > 8) continue;
        if (stack->mBlock)
        {
            if (std::ranges::find(blacklistedBlocks, stack->mBlock->toLegacy()->mName) != blacklistedBlocks.end()) continue;

            placeables += stack->mCount;
        }
    }

    return placeables;
}

int ItemUtils::getPlaceableItemOnBlock(glm::vec3 blockPos, bool hotbarOnly, bool prioHighest)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return -1;
    auto supplies = player->getSupplies();

    int slot = -1;
    // slot, count
    std::map<int, int> placeables;
    for (int i = 0; i < 36; i++)
    {
        ItemStack* stack = supplies->getContainer()->getItem(i);
        if (!stack->mItem) continue;
        Item* item = stack->getItem();
        if (hotbarOnly && i > 8) continue;
        if (stack->mBlock)
        {
            if (std::ranges::find(blacklistedBlocks, stack->mBlock->toLegacy()->mName) != blacklistedBlocks.end()) continue;
            Block* block = stack->mBlock;
            if (!block->mLegacy->mayPlaceOn(blockPos)) continue;

            if (!prioHighest)
            {
                slot = i;
                break;
            }

            placeables[i] = stack->mCount;
        }
    }

    if (prioHighest)
    {
        int highest = 0;
        for (const auto& [pSlot, count] : placeables)
        {
            if (count > highest)
            {
                highest = count;
                slot = pSlot;
            }
        }
    }

    return slot;
}

int ItemUtils::getSwiftnessSpellbook()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return -1;

    int slot = -1;

    for (int i = 0; i < 36; i++)
    {
        ItemStack* stack = player->getSupplies()->getContainer()->getItem(i);
        if (!stack->mItem) continue;
        Item* item = stack->getItem();
        if (StringUtils::containsIgnoreCase(stack->getCustomName(), "Spell of Swiftness"))
        {
            slot = i;
            break;
        }
    }

    return slot;
}

void ItemUtils::useItem(int slot)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    ItemStack* stack = player->getSupplies()->getContainer()->getItem(slot);
    if (!stack->mItem) return;

    auto supplies = player->getSupplies();

    int currentSlot = supplies->mSelectedSlot;
    supplies->mSelectedSlot = slot;
    player->getGameMode()->baseUseItem(stack);
    supplies->mSelectedSlot = currentSlot;
}