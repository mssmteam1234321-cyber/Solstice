//
// Created by vastrakai on 7/2/2024.
//

#include "PacketReceiveHook.hpp"

#include <magic_enum.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>

std::unordered_map<PacketID, std::unique_ptr<Detour>> PacketReceiveHook::mDetours;

void* PacketReceiveHook::onPacketSend(void* _this, void* networkIdentifier, void* netEventCallback, std::shared_ptr<Packet> packet)
{
    auto ofunc = PacketReceiveHook::mDetours[packet->getId()]->getOriginal<decltype(&onPacketSend)>();

    auto holder = nes::make_holder<PacketInEvent>(packet, networkIdentifier, netEventCallback);
    gFeatureManager->mDispatcher->trigger(holder);
    if (holder->isCancelled()) return nullptr;

    return ofunc(_this, networkIdentifier, netEventCallback, packet);
}

void PacketReceiveHook::init()
{
    // Go through the enum of PacketID using magic_enum, and hook the packet receive function for each packet
    // Also do this asynchonously, using futures
    std::vector<std::future<void>> futures;

    auto packetIds = magic_enum::enum_values<PacketID>();

    size_t chunkSize = packetIds.size();

    auto handlePacketChunk = [&](size_t start, size_t end) {
        for (size_t i = start; i < end; i++) {
            PacketID packetId = packetIds[i];
            auto packet = MinecraftPackets::createPacket(packetId);
            if (!packet) continue;
            uint64_t packetFunc = packet->mDispatcher->getPacketHandler();
            std::string name = std::string(magic_enum::enum_name<PacketID>(packet->getId()));
            name = "PacketHandlerDispatcherInstance<" + name + "Packet,0>::handle";
            auto detour = std::make_unique<Detour>(name, reinterpret_cast<void*>(packetFunc), &onPacketSend, true);
            detour->enable(true);
            mDetours[packetId] = std::move(detour);
        }
    };

    uint64_t start = NOW;

    for (size_t i = 0; i < packetIds.size(); i += chunkSize) {
        size_t start = i;
        size_t end = std::min(i + chunkSize, packetIds.size());
        futures.push_back(std::async(std::launch::async, handlePacketChunk, start, end));
    }

    for (auto& future : futures) {
        future.get();
    }

    uint64_t timeTaken = NOW - start;

    spdlog::info("Successfully hooked {} packets in {}ms", mDetours.size(), timeTaken);
}
