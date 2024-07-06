//
// Created by vastrakai on 7/5/2024.
//

#include "PacketUtils.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/MobEquipmentPacket.hpp>

void PacketUtils::spoofSlot(int slot)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    auto mep = MinecraftPackets::createPacket<MobEquipmentPacket>();

    auto itemStack = player->getSupplies()->getContainer()->getItem(slot);

    mep->mRuntimeId = player->getRuntimeID();
    mep->mItem = NetworkItemStackDescriptor(*itemStack);
    mep->mSlot = slot;
    mep->mSelectedSlot = slot;
    mep->mContainerId = MobEquipmentPacket::ContainerID::Inventory;
    mep->mContainerIdByte = static_cast<unsigned char>(MobEquipmentPacket::ContainerID::Inventory);
    mep->mSelectedSlotByte = static_cast<unsigned char>(slot);
    mep->mSlotByte = static_cast<unsigned char>(slot);
    ClientInstance::get()->getPacketSender()->sendToServer(mep.get());
}
