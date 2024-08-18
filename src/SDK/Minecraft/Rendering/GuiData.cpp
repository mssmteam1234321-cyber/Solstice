//
// Created by vastrakai on 6/25/2024.
//

#include "GuiData.hpp"

#include <Hook/Hooks/NetworkHooks/PacketReceiveHook.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/GameSession.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <Utils/MemUtils.hpp>

void GuiData::displayClientMessage(const std::string& msg)
{
    if (!ClientInstance::get()->getLocalPlayer()) return;
    static uintptr_t func = 0;

    auto textPacket = MinecraftPackets::createPacket<TextPacket>();
    textPacket->mMessage = msg;
    textPacket->mType = TextPacketType::Raw;

    static bool once = false;
    if (!once || !func)
    {
        func = textPacket->mDispatcher->getPacketHandler();
        once = true;
    }

    if (!func) return;
    MemUtils::callFastcall<void>(func, textPacket->mDispatcher, PacketReceiveHook::NetworkIdentifier, ClientInstance::get()->getMinecraftSim()->getGameSession()->getEventCallback(), textPacket);
};
