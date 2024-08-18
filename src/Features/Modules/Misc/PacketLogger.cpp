//
// Created by vastrakai on 7/1/2024.
//

#include "PacketLogger.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/Network/Packets/ModalFormResponsePacket.hpp>

void PacketLogger::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &PacketLogger::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &PacketLogger::onPacketInEvent>(this);
}

void PacketLogger::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &PacketLogger::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &PacketLogger::onPacketInEvent>(this);
}

std::vector<PacketID> ignored = { PacketID::LevelChunk, PacketID::MovePlayer, PacketID::Animate, PacketID::PlayerAuthInput, PacketID::SetActorMotion, PacketID::MoveActorAbsolute, PacketID::MoveActorDelta, PacketID::UpdateAttributes, PacketID::SetActorData };

void PacketLogger::onPacketOutEvent(PacketOutEvent& event)
{
    if (!mOutgoing.mValue) return;
    if (std::find(ignored.begin(), ignored.end(), event.mPacket->getId()) != ignored.end()) return;

    // Log the packet
    spdlog::info("[out] Packet: {}", event.mPacket->getName());
    if (event.mPacket->getId() == PacketID::Login)
    {
        auto packet = event.getPacket<Packet>();
        spdlog::info("Packet: {}", "Login");
    };

    if (event.mPacket->getId() == PacketID::InventoryTransaction)
    {
        auto packet = event.getPacket<InventoryTransactionPacket>();

        spdlog::info("Packet: InventoryTransaction, TransactionType: {}", magic_enum::enum_name(packet->mTransaction->type));
    }

    if (event.mPacket->getId() == PacketID::ModalFormResponse)
    {
        auto packet = event.getPacket<ModalFormResponsePacket>();
        std::string json = "";
        std::string* output = nullptr;
        if (packet->mJSONResponse.has_value())
            output = packet->mJSONResponse.value().toStyledString();
        else
            output = nullptr;
        spdlog::info("Packet: ModalFormResponse, FormId: {}, FormJson: {}", packet->mFormId, output ? *output : "output was null and could not be read!");
    }
}


void PacketLogger::onPacketInEvent(PacketInEvent& event)
{
    if (!mIncoming.mValue) return;
    if (std::find(ignored.begin(), ignored.end(), event.mPacket->getId()) != ignored.end()) return;
    spdlog::info("[in] Packet: {}", event.mPacket->getName());

}
