//
// Created by vastrakai on 7/6/2024.
//

#include "InvManager.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/PingUpdateEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/InteractPacket.hpp>
#include <SDK/Minecraft/Network/Packets/ContainerClosePacket.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>

void InvManager::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &InvManager::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &InvManager::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &InvManager::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<PingUpdateEvent, &InvManager::onPingUpdateEvent>(this);
}

void InvManager::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &InvManager::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &InvManager::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &InvManager::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<PingUpdateEvent, &InvManager::onPingUpdateEvent>(this);
}

bool gIsOpen = false;
bool gIsChestOpen = false;

void sendOpen()
{
    if (gIsOpen) return;
    auto player = ClientInstance::get()->getLocalPlayer();

    auto interactPacket = MinecraftPackets::createPacket<InteractPacket>();
    interactPacket->mTargetId = player->getRuntimeID();
    interactPacket->mPos = *player->getPos();
    interactPacket->mAction = InteractPacket::Action::OpenInventory;
    ClientInstance::get()->getPacketSender()->sendToServer(interactPacket.get());
    gIsOpen = true;
}

void sendClose(bool force = false)
{
    if (!gIsOpen && !force) return;
    auto player = ClientInstance::get()->getLocalPlayer();
    auto closePacket = MinecraftPackets::createPacket<ContainerClosePacket>();
    closePacket->mContainerId = ContainerID::Inventory;
    closePacket->mServerInitiatedClose = false;
    ClientInstance::get()->getPacketSender()->sendToServer(closePacket.get());

    auto closePacket2 = MinecraftPackets::createPacket<ContainerClosePacket>();
    closePacket2->mContainerId = ContainerID::First;
    closePacket2->mServerInitiatedClose = false;
    ClientInstance::get()->getPacketSender()->sendToServer(closePacket2.get());


    gIsOpen = false;
}

void InvManager::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    auto armorContainer = player->getArmorContainer();
    auto supplies = player->getSupplies();
    auto container = supplies->getContainer();

    if (gIsChestOpen && mSpoofOpen.mValue)
    {
        spdlog::debug("Chest is open [condition: gIsChestOpen && mSpoofOpen.mValue]");
        return; // Don't do anything if the chest is open
    }

    if (mCloseNext)
    {
        sendClose();
        mCloseNext = false;
    }

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
    bool isInstant = mMode.mValue == Mode::Instant;
    if (mLastAction + static_cast<uint64_t>(mDelay.mValue) > NOW)
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
        if (mSpoofOpen.mValue) sendOpen();
        supplies->getContainer()->dropSlot(item);
        if (mSpoofOpen.mValue) mCloseNext = true;

        mLastAction = NOW;
        if (!isInstant)
        {
            return;
        }
    }

    if (mPreferredSlots.mValue)
    {
        if (mPreferredSwordSlot.mValue != 0)
        {
            if (bestSwordSlot != -1 && bestSwordSlot != mPreferredSwordSlot.mValue - 1)
            {
                if (mSpoofOpen.mValue) sendOpen();
                supplies->getContainer()->swapSlots(bestSwordSlot, mPreferredSwordSlot.mValue - 1);
                if (mSpoofOpen.mValue) mCloseNext = true;

                mLastAction = NOW;
                if (!isInstant)
                {
                    return;
                }
            }
        }
        if (mPreferredPickaxeSlot.mValue != 0)
        {
            if (bestPickaxeSlot != -1 && bestPickaxeSlot != mPreferredPickaxeSlot.mValue - 1)
            {
                if (mSpoofOpen.mValue) sendOpen();
                supplies->getContainer()->swapSlots(bestPickaxeSlot, mPreferredPickaxeSlot.mValue - 1);
                if (mSpoofOpen.mValue) mCloseNext = true;

                mLastAction = NOW;
                if (!isInstant)
                {
                    return;
                }
            }
        }
        if (mPreferredAxeSlot.mValue != 0)
        {
            if (bestAxeSlot != -1 && bestAxeSlot != mPreferredAxeSlot.mValue - 1)
            {
                if (mSpoofOpen.mValue) sendOpen();
                supplies->getContainer()->swapSlots(bestAxeSlot, mPreferredAxeSlot.mValue - 1);
                if (mSpoofOpen.mValue) mCloseNext = true;

                mLastAction = NOW;
                if (!isInstant)
                {
                    return;
                }
            }
        }
        if (mPreferredShovelSlot.mValue != 0)
        {
            if (bestShovelSlot != -1 && bestShovelSlot != mPreferredShovelSlot.mValue - 1)
            {
                if (mSpoofOpen.mValue) sendOpen();
                supplies->getContainer()->swapSlots(bestShovelSlot, mPreferredShovelSlot.mValue - 1);
                if (mSpoofOpen.mValue) mCloseNext = true;

                mLastAction = NOW;
                if (!isInstant)
                {
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
        if (mSpoofOpen.mValue) sendOpen();
        supplies->getContainer()->equipArmor(item);
        if (mSpoofOpen.mValue) mCloseNext = true;
        mLastAction = NOW;
        if (!isInstant)
        {
            break;
        }
    }
}

void InvManager::onPacketInEvent(PacketInEvent& event)
{
    if ((event.mPacket->getId() == PacketID::ContainerOpen || event.mPacket->getId() == PacketID::ContainerClose || event.mPacket->getId() == PacketID::Interact) && mSpoofOpen.mValue)
    {
        // Return if the LastAction was more than 400ms ago
        if (mLastAction + 200 + mLastPing < NOW) return;

        if (event.mPacket->getId() == PacketID::ContainerOpen)
        {
            auto openPacket = event.getPacket<ContainerOpenPacket>();
            if (openPacket->mContainerId != ContainerID::Inventory && openPacket->mContainerId != ContainerID::First) return;
            if (openPacket->mContainerId == ContainerID::Chest) gIsChestOpen = true;
        } else if (event.mPacket->getId() == PacketID::ContainerClose)
        {
            auto closePacket = event.getPacket<ContainerClosePacket>();
            if (closePacket->mContainerId != ContainerID::Inventory && closePacket->mContainerId != ContainerID::First) return;
            if (closePacket->mContainerId == ContainerID::Chest) gIsChestOpen = false;
        } else if (event.mPacket->getId() == PacketID::Interact)
        {
            auto interactPacket = event.getPacket<InteractPacket>();
            if (interactPacket->mAction != InteractPacket::Action::OpenInventory) return;
        }

        spdlog::warn("Cancelling packet [in:{}Packet]", std::string(magic_enum::enum_name(event.mPacket->getId())));

        event.cancel();
    }
}

void InvManager::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() == PacketID::ContainerClose)
    {
        auto packet = event.getPacket<ContainerClosePacket>();
        if (packet->mContainerId == ContainerID::Chest) gIsChestOpen = false;
    }
    if (event.mPacket->getId() == PacketID::Interact && mSpoofOpen.mValue)
    {
        // Return if the LastAction was more than 400ms+ping ago
        if (mLastAction + 200 + mLastPing < NOW) return;

        auto packet = event.getPacket<InteractPacket>();
        if (packet->mAction == InteractPacket::Action::OpenInventory)
        {
            spdlog::warn("Cancelling packet [out:InteractPacket]");
            event.cancel();
        }
    } else if (event.mPacket->getId() == PacketID::InventoryTransaction && mSpoofOpen.mValue)
    {
        auto packet = event.getPacket<InventoryTransactionPacket>();
        /*std::string actionType = std::string(magic_enum::enum_name(packet->mTransaction->type));
        spdlog::debug("InventoryTransactionPacket type: {}", actionType);*/
        if (packet->mTransaction->type == ComplexInventoryTransaction::Type::ItemUseTransaction)
        {
            auto iut = reinterpret_cast<ItemUseInventoryTransaction*>(packet->mTransaction.get());
            if (iut->actionType == ItemUseInventoryTransaction::ActionType::Use || iut->actionType == ItemUseInventoryTransaction::ActionType::Place)
            {
                std::string actionType = std::string(magic_enum::enum_name(iut->actionType));
                auto block = ClientInstance::get()->getBlockSource()->getBlock(iut->blockPos);
                std::string name = block->toLegacy()->mName;

                if (name.ends_with("_chest"))
                {
                    // Force a close
                    sendClose(true);
                }

            }
        }
    }
}

void InvManager::onPingUpdateEvent(PingUpdateEvent& event)
{
    mLastPing = event.mPing;
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
