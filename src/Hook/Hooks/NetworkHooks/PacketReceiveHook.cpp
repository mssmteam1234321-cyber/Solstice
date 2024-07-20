//
// Created by vastrakai on 7/2/2024.
//

#include "PacketReceiveHook.hpp"

#include <magic_enum.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/GameSession.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <omp.h>

std::unordered_map<PacketID, std::unique_ptr<Detour>> PacketReceiveHook::mDetours;

void* PacketReceiveHook::onPacketSend(void* _this, void* networkIdentifier, void* netEventCallback, std::shared_ptr<Packet> packet)
{
    auto ofunc = PacketReceiveHook::mDetours[packet->getId()]->getOriginal<decltype(&onPacketSend)>();

    NetworkIdentifier = networkIdentifier;

    auto holder = nes::make_holder<PacketInEvent>(packet, networkIdentifier, netEventCallback);
    gFeatureManager->mDispatcher->trigger(holder);
    if (holder->isCancelled()) return nullptr;

    return ofunc(_this, networkIdentifier, netEventCallback, packet);
}

void PacketReceiveHook::handlePacket(std::shared_ptr<Packet> packet)
{
    if (!NetworkIdentifier) return;
    auto ofunc = mDetours[packet->getId()]->getOriginal<decltype(&onPacketSend)>();
    ofunc(packet->mDispatcher, NetworkIdentifier, ClientInstance::get()->getMinecraftSim()->getGameSession()->getEventCallback(), packet);
}

void PacketReceiveHook::init()
{
    static bool called = false;
    if (called) return;
    called = true;

    auto packetIds = magic_enum::enum_values<PacketID>();

    uint64_t start = NOW;
    spdlog::info("Hooking {} packets", packetIds.size());

    std::vector<std::string> packetNames(packetIds.size());
    std::vector<uint64_t> packetFuncs(packetIds.size(), 0);

    // Precompute packet names and functions outside of the parallel loop
#pragma omp parallel for
    for (int i = 0; i < packetIds.size(); i++) {
        PacketID packetId = packetIds[i];
        auto packet = MinecraftPackets::createPacket(packetId);
        if (!packet) continue;

        packetFuncs[i] = packet->mDispatcher->getPacketHandler();
        if (packetFuncs[i]) {
            packetNames[i] = "PacketHandlerDispatcherInstance<" + std::string(magic_enum::enum_name<PacketID>(packet->getId())) + "Packet,0>::handle";
        } else {
            spdlog::warn("Failed to hook packet: {}", magic_enum::enum_name<PacketID>(packet->getId()));
        }
    }

    // Create detours
#pragma omp parallel for
    for (int i = 0; i < packetIds.size(); i++) {
        if (!packetFuncs[i]) continue;

        auto detour = std::make_unique<Detour>(packetNames[i], reinterpret_cast<void*>(packetFuncs[i]), &onPacketSend, true);
#pragma omp critical
        {
            mDetours[packetIds[i]] = std::move(detour);
        }
    }

    uint64_t timeTaken = NOW - start;

    spdlog::info("Successfully hooked {} packets in {}ms", mDetours.size(), timeTaken);
}