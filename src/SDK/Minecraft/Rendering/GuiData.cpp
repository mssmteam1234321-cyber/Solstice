//
// Created by vastrakai on 6/25/2024.
//

#include "GuiData.hpp"

#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <Utils/MemUtils.hpp>

void GuiData::displayClientMessage(const std::string& msg)
{
    if (!ClientInstance::get()->getLocalPlayer()) return;
    static int lol = 0;
    MemUtils::callFastcall<void>(SigManager::GuiData_displayClientMessage, this, msg, &lol, false);
}
