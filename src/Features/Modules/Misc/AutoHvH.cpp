//
// Created by alteik on 29/09/2024.
//

#include "AutoHvH.hpp"
#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include <Utils/MiscUtils/DataStore.hpp>
#include <SDK/Minecraft/Network/PacketID.hpp>
#include <Features/Events/PacketInEvent.hpp>

static constexpr uint64_t rateLimitMs = 15 * 1000; // 15 seconds

void AutoHvH::onHttpResponse(HttpResponseEvent event)
{
    auto sender = static_cast<AutoHvH*>(event.mSender);
    auto originalRequest = static_cast<HttpRequest*>(event.mOriginalRequest);

    if (event.mStatusCode == 200)
    {
        nlohmann::json json = nlohmann::json::parse(event.mResponse);
        if (json.contains("main"))
        {
            auto main = json["main"];
            PlayerInfo player = PlayerInfo(main["username_cc"], main["first_played"]);
            sender->mPlayerStore.mObjects.push_back(player);
            spdlog::info("[AutoHvH] Player found: {} (first played: {})", player.name, player.firstPlayed);
        }
    } else if (event.mStatusCode == 429) {
        spdlog::warn("[AutoHvH] Rate limited, waiting {} seconds", rateLimitMs / 1000);
        ChatUtils::displayClientMessageRaw("§c§l» §r§cRate limited, waiting {} seconds", rateLimitMs / 1000);
        sender->mLastRateLimit = NOW;
    } else {
        spdlog::error("[AutoHvH] Request failed with status code: {}", event.mStatusCode);
    }
}

uint64_t AutoHvH::getFirstPlayed(const std::string &name) {

    for (const auto& player : mPlayerStore.mObjects)
    {
        if (player.name == name)
        {
            return player.firstPlayed;
        }
    }
    return 0;
}

void AutoHvH::makeRequest(const std::string& name)
{
    if (std::ranges::find(mRequestedPlayers, name) != mRequestedPlayers.end())
    {
        return;
    }
    mRequestedPlayers.push_back(name);

    auto request = std::make_unique<HttpRequest>(HttpMethod::GET, "https://api.playhive.com/v0/game/all/main/" + name, "", "", &AutoHvH::onHttpResponse, this);
    int64_t now = NOW;
    mRequests.emplace_back( now, std::move(request) );
}

bool AutoHvH::isPlayerCached(const std::string& name) const
{
    return std::ranges::find_if(mPlayerStore.mObjects, [&name](const PlayerInfo& player) { return player.name == name; }) != mPlayerStore.mObjects.end();
}

std::string formatTime(int64_t time, const std::string &format) {
    time_t rawtime = time;
    struct tm* timeinfo;
    char buffer[80];

    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);
    std::string str(buffer);
    return str;
}

std::string relativeTime(int64_t ms)
{
    int64_t hours = ms / 3600;
    int64_t minutes = (ms % 3600) / 60;
    int64_t seconds = ms % 60;

    std::stringstream ss;

    if (hours > 0) {
        ss << hours << "h ";
    }

    if (minutes > 0 || hours > 0) {
        ss << minutes << "m ";
    }

    ss << seconds << "s";

    return ss.str();
}

void AutoHvH::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AutoHvH::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &AutoHvH::onPacketInEvent>(this);
}

void AutoHvH::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AutoHvH::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &AutoHvH::onPacketInEvent>(this);
}

void AutoHvH::onBaseTickEvent(BaseTickEvent& event) {

    for (auto it = mRequests.begin(); it != mRequests.end();) {
        if (it->second->isDone()) {
            it = mRequests.erase(it);
        } else {
            ++it;
        }
    }

    if (mLastRateLimit != 0 && NOW - mLastRateLimit < rateLimitMs) {
        return;
    }

    auto player = event.mActor;
    if (!player) return;

    std::unordered_map<mce::UUID, PlayerListEntry>* playerList = player->getLevel()->getPlayerList();
    std::vector<std::string> playerNames;

    for (auto& entry : *playerList | std::views::values) {
        playerNames.emplace_back(entry.mName);
    }

    for (auto& playerName : playerNames) {
        if (std::find(checkedPlayers.begin(), checkedPlayers.end(), playerName) != checkedPlayers.end()) {
            continue;
        }

        if (isPlayerCached(playerName)) {
            uint64_t firstPlayed = getFirstPlayed(playerName);
            if (firstPlayed < (time(0) - (60 * 60 * 24 * 1))) {
                spdlog::info("skipping " + playerName);
            } else {
                std::string firstPlayedTime = formatTime(firstPlayed, "%m/%d/%Y %H:%M:%S");
                int64_t msSinceFirstPlayed = time(0) - firstPlayed;
                std::string relativeFirstPlayedTime = relativeTime(msSinceFirstPlayed);
                ChatUtils::displayClientMessage(
                        "§cpossible cheater detected:§7 " + playerName + "§r. First played time: §d" +
                        firstPlayedTime + "§r (§d" + relativeFirstPlayedTime + "§5 ago§r)");

                checkedPlayers.push_back(playerName);
            }
        } else {
            makeRequest(playerName);
        }
    }
}

void AutoHvH::onPacketInEvent(PacketInEvent& event) {
    if (event.mPacket->getId() == PacketID::ChangeDimension || event.mPacket->getId() == PacketID::Disconnect) {
        checkedPlayers.clear();
    }
}