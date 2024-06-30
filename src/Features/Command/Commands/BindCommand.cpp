//
// Created by vastrakai on 6/29/2024.
//

#include "BindCommand.hpp"

#include <Utils/GameUtils/ChatUtils.hpp>
#include <Utils/Keyboard.hpp>

void BindCommand::execute(const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        ChatUtils::displayClientMessage("§c" + getUsage());
        return;
    }

    const auto& moduleName = args[1];
    const auto& key = args[2];

    int keyId = Keyboard::getKeyId(key);

    auto module = gFeatureManager->mModuleManager->getModule(moduleName);

    if (!module)
    {
        return;
    }

    module->mKey = keyId;

    ChatUtils::displayClientMessage("§6" + module->mName + "§a is now bound to §6" + key + "§a!");
}

std::vector<std::string> BindCommand::getAliases() const
{
    return {"b"};
}

std::string BindCommand::getDescription() const
{
    return "Binds a module to a key";
}

std::string BindCommand::getUsage() const
{
    return "Usage: .bind <module> <key>";
}
