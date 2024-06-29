//
// Created by vastrakai on 6/28/2024.
//

#include "TestCommand.hpp"

#include <Utils/ChatUtils.hpp>

void TestCommand::execute(const std::vector<std::string>& args)
{
    ChatUtils::displayClientMessage("Test command executed!");
}

std::vector<std::string> TestCommand::getAliases() const
{
    return {"t"};
}

std::string TestCommand::getDescription() const
{
    return "A test command";
}

std::string TestCommand::getUsage() const
{
    return "Usage: .test";
}
