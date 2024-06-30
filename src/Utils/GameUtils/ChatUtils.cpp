//
// Created by vastrakai on 6/25/2024.
//

#include "ChatUtils.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Rendering/GuiData.hpp>

void ChatUtils::displayClientMessage(const std::string& msg)
{
    ClientInstance::get()->getGuiData()->displayClientMessage("§7[§asolstice§7] §r" + msg);
}

void ChatUtils::displayClientMessage(const std::string& subcaption, const std::string& msg)
{
    ClientInstance::get()->getGuiData()->displayClientMessage("§7[§asolstice§7] §7[" + subcaption + "§7] §r" + msg);
}


