//
// Created by vastrakai on 7/5/2024.
//

#include "PlayerInventory.hpp"

#include <libhat/Access.hpp>
#include <SDK/OffsetProvider.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/Item.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/InventoryTransactionPacket.hpp>

#include "ContainerManagerModel.hpp"



void Inventory::dropSlot(int slot)
{
    ItemStack* itemStack = getItem(slot);

    if (!itemStack->mItem) return;

    static ItemStack blankStack = ItemStack();

    InventoryAction action = InventoryAction(slot, itemStack, &blankStack);
    InventoryAction action2 = InventoryAction(0, &blankStack, itemStack);
    action.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);
    action2.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);
    action.mSource.mType = InventorySourceType::ContainerInventory;
    action2.mSource.mType = InventorySourceType::WorldInteraction;

    auto pkt = MinecraftPackets::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ComplexInventoryTransaction>();
    cit->data.addAction(action);
    cit->data.addAction(action2);

    pkt->mTransaction = std::move(cit);
    ClientInstance::get()->getPacketSender()->sendToServer(pkt.get());

    //setItem(slot, blankStack);
}

void Inventory::swapSlots(int from, int to)
{
    ItemStack* item1 = getItem(from);
    ItemStack* item2 = getItem(to);

    auto action1 = InventoryAction(from, item1, item2);
    auto action2 = InventoryAction(to, item2, item1);

    action1.mSource.mType = InventorySourceType::ContainerInventory;
    action2.mSource.mType = InventorySourceType::ContainerInventory;
    action1.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);
    action2.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);

    auto pkt = MinecraftPackets::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ComplexInventoryTransaction>();

    cit->data.addAction(action1);
    cit->data.addAction(action2);

    //setItem(from, *item2);
    //setItem(to, *item1);

    pkt->mTransaction = std::move(cit);
    ClientInstance::get()->getPacketSender()->sendToServer(pkt.get());
}

void Inventory::equipArmor(int slot)
{
    auto player = ClientInstance::get()->getLocalPlayer();

    ItemStack* itemStack = getItem(slot);

    if (!itemStack->mItem) return;

    Item* item = *itemStack->mItem;
    // Get the current item stack in the armor slot
    ItemStack* armorStack = player->getArmorContainer()->getItem(item->getArmorSlot());

    static ItemStack blankStack = ItemStack();

    InventoryAction action = InventoryAction(slot, itemStack, armorStack);
    action.mSource.mType = InventorySourceType::ContainerInventory;
    action.mSource.mContainerId = static_cast<int>(ContainerID::Inventory);

    InventoryAction action2 = InventoryAction(item->getArmorSlot(), armorStack, itemStack);
    action2.mSource.mType = InventorySourceType::ContainerInventory;
    action2.mSource.mContainerId = static_cast<int>(ContainerID::Armor);


    auto pkt = MinecraftPackets::createPacket<InventoryTransactionPacket>();

    auto cit = std::make_unique<ComplexInventoryTransaction>();
    cit->data.addAction(action);
    cit->data.addAction(action2);

    pkt->mTransaction = std::move(cit);
    ClientInstance::get()->getPacketSender()->sendToServer(pkt.get());

    //setItem(slot, blankStack);
}

Inventory* PlayerInventory::getContainer()
{
    return hat::member_at<Inventory*>(this, OffsetProvider::PlayerInventory_mContainer);
}
