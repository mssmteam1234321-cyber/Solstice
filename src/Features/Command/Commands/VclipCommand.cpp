//
// Created by alteik on 03/09/2024.
//

#include "VclipCommand.hpp"
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

void VclipCommand::execute(const std::vector<std::string>& args)
{
    if (args.size() == 2)
    {
        const std::string& amountOfBlocks = args[1];
        if(!isNumber(amountOfBlocks))
        {
            ChatUtils::displayClientMessage("Amount must be a number");
        }

        auto player = ClientInstance::get()->getLocalPlayer();
        auto pos = *player->getPos();
        auto amount = std::stof(amountOfBlocks);
        player->setPosition({pos.x, pos.y+ amount, pos.z});

        bool negative = amount < 0;
        std::string direction = negative ? "down" : "up";
        int absValue = abs(amount);
        ChatUtils::displayClientMessaget.("Teleported " + direction + " " + std::to_string(absValue) + " blocks");
    }
}

std::vector<std::string> VclipCommand::getAliases() const
{
    return {"v"};
}

std::string VclipCommand::getDescription() const
{
    return "Vclips u up or down.";
}

std::string VclipCommand::getUsage() const
{
    return "Usage .vclip <amount of blocks> (ex: 2, -2)";
}

bool VclipCommand::isNumber(std::string str) {
    try {
        return std::stof(str);
    } catch (std::invalid_argument &e) {
        return false;
    }
}