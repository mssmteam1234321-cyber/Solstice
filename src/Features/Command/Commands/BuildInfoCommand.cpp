//
// Created by vastrakai on 7/29/2024.
//

#include "BuildInfoCommand.hpp"

#include <build_info.h>

static uint64_t UID1 = 0x1F1F1F1F1F1F1F1F; // replaced at fetch time
static uint64_t UID2 = 0x2F2F2F2F2F2F2F2F; // replaced at fetch time
static uint64_t UID3 = 0x3F3F3F3F3F3F3F3F; // replaced at fetch time



struct IdHolder
{
    uint64_t id1;
    uint64_t id2;
    uint64_t id3;

    explicit IdHolder(const uint64_t id1, const uint64_t id2, const uint64_t id3) : id1(id1), id2(id2), id3(id3) {}
};

void BuildInfoCommand::execute(const std::vector<std::string>& args)
{
    ChatUtils::displayClientMessage("§6Solstice build info");
    ChatUtils::displayClientMessage("§6- §eBuild date§7: {}", __DATE__);
    ChatUtils::displayClientMessage("§6- §eBuild time§7: {}", __TIME__);
    ChatUtils::displayClientMessage("§6- §eSolstice version§7: {}", std::string(SOLSTICE_VERSION));
    ChatUtils::displayClientMessage("§6- §eMinecraft version§7: {}", ProcUtils::getVersion());
    ChatUtils::displayClientMessage("§6- §eIntended Minecraft version§7: {}", std::string(SOLSTICE_INTENDED_VERSION != "" ? SOLSTICE_INTENDED_VERSION : "N/A"));
    ChatUtils::displayClientMessage("§6- §eCommit message§7: {}", std::string(SOLSTICE_BUILD_COMMIT_MESSAGE));
    ChatUtils::displayClientMessage("§6- §eCommit hash§7: {}", std::string(SOLSTICE_BUILD_VERSION));
    ChatUtils::displayClientMessage("§6- §eCompiled by§7: {}", std::string(SOLSTICE_PC_USERNAME));
    ChatUtils::displayClientMessage("§6- §eBranch§7: {}", std::string(SOLSTICE_BUILD_BRANCH));

    // so it doesn't get optimized out
    auto idHolder = std::make_unique<IdHolder>(UID1, UID2, UID3);
    idHolder.reset();
#ifdef __DEBUG__
    ChatUtils::displayClientMessage("§6- §eBuild type§7: Debug");
#else
    ChatUtils::displayClientMessage("§6- §eBuild type§7: Release");
#endif
    ChatUtils::displayClientMessage("§e{}§6 files changed locally compared to the last commit", STRING(SOLSTICE_FILES_CHANGED_COUNT));
}

std::vector<std::string> BuildInfoCommand::getAliases() const
{
    return {"bi"};
}

std::string BuildInfoCommand::getDescription() const
{
    return "Displays information about the current client build";
}

std::string BuildInfoCommand::getUsage() const
{
    return "Usage: .buildinfo";
}
