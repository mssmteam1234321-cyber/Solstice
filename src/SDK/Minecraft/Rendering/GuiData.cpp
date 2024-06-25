//
// Created by vastrakai on 6/25/2024.
//

#include "GuiData.hpp"

#include <string>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <Utils/MemUtils.hpp>

void GuiData::displayClientMessage(const std::string& msg)
{
    if (!ClientInstance::get()->getLocalPlayer()) return;
    MemUtils::callFastcall<void>(SigManager::GuiData_displayClientMessage, this, msg, false);
}
