//
// Created by vastrakai on 7/6/2024.
//

#include "InvManager.hpp"

#include <magic_enum.hpp>
#include <Features/FeatureManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>
#include <Utils/GameUtils/ItemUtils.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>

void InvManager::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &InvManager::onBaseTickEvent>(this);
}

void InvManager::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &InvManager::onBaseTickEvent>(this);
}

void InvManager::onBaseTickEvent(BaseTickEvent& event) const
{
    auto player = ClientInstance::get()->getLocalPlayer();
    auto armorContainer = player->getArmorContainer();
    auto supplies = player->getSupplies();
    auto container = supplies->getContainer();

    // Check how many free slots we have
    int freeSlots = 0;
    for (int i = 0; i < 36; i++)
    {
        if (!container->getItem(i)->mItem) freeSlots++;
    }

    // If we are in a container, don't do anything
    if (ClientInstance::get()->getMouseGrabbed() && player && freeSlots > 0)
    {
        return;
    }

    std::vector<int> itemsToEquip;
    bool isInstant = mMode.mValue == static_cast<int>(Mode::Instant);
    static uint64_t lastAction = 0;
    if (lastAction + static_cast<uint64_t>(mDelay.mValue) > NOW)
    {
        return;
    }

    int bestHelmetSlot = -1;
    int bestChestplateSlot = -1;
    int bestLeggingsSlot = -1;
    int bestBootsSlot = -1;
    int bestSwordSlot = -1;
    int bestPickaxeSlot = -1;
    int bestAxeSlot = -1;
    int bestShovelSlot = -1;

    int bestHelmetValue = 0;
    int bestChestplateValue = 0;
    int bestLeggingsValue = 0;
    int bestBootsValue = 0;
    int bestSwordValue = 0;
    int bestPickaxeValue = 0;
    int bestAxeValue = 0;
    int bestShovelValue = 0;

    int equippedHelmetValue = ItemUtils::getItemValue(armorContainer->getItem(0));
    int equippedChestplateValue = ItemUtils::getItemValue(armorContainer->getItem(1));
    int equippedLeggingsValue = ItemUtils::getItemValue(armorContainer->getItem(2));
    int equippedBootsValue = ItemUtils::getItemValue(armorContainer->getItem(3));

    for (int i = 0; i < 36; i++)
    {
        auto item = container->getItem(i);
        if (!item->mItem) continue;
        auto itemType = item->getItem()->getItemType();
        auto itemValue = ItemUtils::getItemValue(item);
        if (itemType == SItemType::Helmet && itemValue > bestHelmetValue)
        {
            if (equippedHelmetValue >= itemValue)
            {
                bestHelmetSlot = -1;
                continue;
            }

            bestHelmetSlot = i;
            bestHelmetValue = itemValue;
        }
        else if (itemType == SItemType::Chestplate && itemValue > bestChestplateValue)
        {
            if (equippedChestplateValue >= itemValue)
            {
                bestChestplateSlot = -1;
                continue;
            }

            bestChestplateSlot = i;
            bestChestplateValue = itemValue;
        }
        else if (itemType == SItemType::Leggings && itemValue > bestLeggingsValue)
        {
            if (equippedLeggingsValue >= itemValue)
            {
                bestLeggingsSlot = -1;
                continue;
            }

            bestLeggingsSlot = i;
            bestLeggingsValue = itemValue;
        }
        else if (itemType == SItemType::Boots && itemValue > bestBootsValue)
        {
            if (equippedBootsValue >= itemValue)
            {
                bestBootsSlot = -1;
                continue;
            }

            bestBootsSlot = i;
            bestBootsValue = itemValue;
        }
        else if (itemType == SItemType::Sword && itemValue > bestSwordValue)
        {
            bestSwordSlot = i;
            bestSwordValue = itemValue;
        }
        else if (itemType == SItemType::Pickaxe && itemValue > bestPickaxeValue)
        {
            bestPickaxeSlot = i;
            bestPickaxeValue = itemValue;
        }
        else if (itemType == SItemType::Axe && itemValue > bestAxeValue)
        {
            bestAxeSlot = i;
            bestAxeValue = itemValue;
        }
        else if (itemType == SItemType::Shovel && itemValue > bestShovelValue)
        {
            bestShovelSlot = i;
            bestShovelValue = itemValue;
        }
    }

    // Go through and get items to drop
    std::vector<int> itemsToDrop;
    for (int i = 0; i < 36; i++)
    {
        auto item = container->getItem(i);
        if (!item->mItem) continue;
        auto itemType = item->getItem()->getItemType();
        auto itemValue = ItemUtils::getItemValue(item);
        if (itemType == SItemType::Sword && i != bestSwordSlot)
        {
            itemsToDrop.push_back(i);
        }
        else if (itemType == SItemType::Pickaxe && i != bestPickaxeSlot)
        {
            itemsToDrop.push_back(i);
        }
        else if (itemType == SItemType::Axe && i != bestAxeSlot)
        {
            itemsToDrop.push_back(i);
        }
        else if (itemType == SItemType::Shovel && i != bestShovelSlot)
        {
            itemsToDrop.push_back(i);
        }
        else if (itemType == SItemType::Helmet && i != bestHelmetSlot)
        {
            itemsToDrop.push_back(i);
        }
        else if (itemType == SItemType::Chestplate && i != bestChestplateSlot)
        {
            itemsToDrop.push_back(i);
        }
        else if (itemType == SItemType::Leggings && i != bestLeggingsSlot)
        {
            itemsToDrop.push_back(i);
        }
        else if (itemType == SItemType::Boots && i != bestBootsSlot)
        {
            itemsToDrop.push_back(i);
        }
    }

    for (auto& item : itemsToDrop)
    {
        supplies->getContainer()->dropSlot(item);
        if (!isInstant)
        {
            lastAction = NOW;
            return;
        }
    }

    if (mPreferredSlots.mValue)
    {
        if (mPreferredSwordSlot.mValue != 0)
        {
            if (bestSwordSlot != -1 && bestSwordSlot != mPreferredSwordSlot.mValue - 1)
            {
                supplies->getContainer()->swapSlots(bestSwordSlot, mPreferredSwordSlot.mValue - 1);
                if (!isInstant)
                {
                    lastAction = NOW;
                    return;
                }
            }
        }
        if (mPreferredPickaxeSlot.mValue != 0)
        {
            if (bestPickaxeSlot != -1 && bestPickaxeSlot != mPreferredPickaxeSlot.mValue - 1)
            {
                supplies->getContainer()->swapSlots(bestPickaxeSlot, mPreferredPickaxeSlot.mValue - 1);
                if (!isInstant)
                {
                    lastAction = NOW;
                    return;
                }
            }
        }
        if (mPreferredAxeSlot.mValue != 0)
        {
            if (bestAxeSlot != -1 && bestAxeSlot != mPreferredAxeSlot.mValue - 1)
            {
                supplies->getContainer()->swapSlots(bestAxeSlot, mPreferredAxeSlot.mValue - 1);
                if (!isInstant)
                {
                    lastAction = NOW;
                    return;
                }
            }
        }
        if (mPreferredShovelSlot.mValue != 0)
        {
            if (bestShovelSlot != -1 && bestShovelSlot != mPreferredShovelSlot.mValue - 1)
            {
                supplies->getContainer()->swapSlots(bestShovelSlot, mPreferredShovelSlot.mValue - 1);
                if (!isInstant)
                {
                    lastAction = NOW;
                    return;
                }
            }
        }
    }

    if (bestHelmetSlot != -1) itemsToEquip.push_back(bestHelmetSlot);
    if (bestChestplateSlot != -1) itemsToEquip.push_back(bestChestplateSlot);
    if (bestLeggingsSlot != -1) itemsToEquip.push_back(bestLeggingsSlot);
    if (bestBootsSlot != -1) itemsToEquip.push_back(bestBootsSlot);

    for (auto& item : itemsToEquip)
    {
        supplies->getContainer()->equipArmor(item);
        if (!isInstant)
        {
            lastAction = NOW;
            break;
        }
    }
}

bool InvManager::isItemUseless(ItemStack* item, int slot)
{
    if (!item->mItem) return true;
    auto player = ClientInstance::get()->getLocalPlayer();
    SItemType itemType = item->getItem()->getItemType();
    auto itemValue = ItemUtils::getItemValue(item);
    // if the item is a piece of armor
    if (itemType == SItemType::Helmet || itemType == SItemType::Chestplate || itemType == SItemType::Leggings || itemType == SItemType::Boots)
    {
        int equippedItemValue = ItemUtils::getItemValue(player->getArmorContainer()->getItem(static_cast<int>(itemType)));
        return equippedItemValue >= itemValue;
    }
    if (itemType == SItemType::Sword || itemType == SItemType::Pickaxe || itemType == SItemType::Axe || itemType == SItemType::Shovel)
    {
        int bestSlot = ItemUtils::getBestItem(itemType);
        int bestValue = ItemUtils::getItemValue(player->getSupplies()->getContainer()->getItem(bestSlot));
        return bestValue >= itemValue && bestSlot != slot;
    }

    return false;
}
