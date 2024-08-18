//
// Created by vastrakai on 6/25/2024.
//

#include "GuiData.hpp"

#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Network/MinecraftPackets.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <Utils/MemUtils.hpp>

class DummyData
{
public:
    PAD(0x120);
};

void GuiData::displayClientMessage(const std::string& msg)
{
    if (!ClientInstance::get()->getLocalPlayer()) return;

    // This is stupid.
    static std::unique_ptr<DummyData> dummyData = std::make_unique<DummyData>();
    MemUtils::callFastcall<void>(SigManager::GuiData_displayClientMessage, this, msg, dummyData.get(), false);
}
