//
// Created by vastrakai on 8/16/2024.
//

#include "StaffAlert.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/Minecraft/World/Level.hpp>

// example: (URL: https://api.playhive.com/v0/game/all/main/FlareonRapier, Method: GET)
/*
{
  "main": {
    "UUID": "2abd2e6a-8127-36da-bbc4-516452f7e65f",
    "xuid": 2535418481650255,
    "username": "flareonrapier",
    "username_cc": "FlareonRapier",
    "rank": "REGULAR",
    "first_played": 1659627061,
    "hub_title_count": 1,
    "hub_title_unlocked": [
      "&eThe &bSky Warrior"
    ],
    "avatar_count": 0,
    "costume_count": 0,
    "hat_count": 0,
    "backbling_count": 0,
    "pets": [],
    "mounts": []
  }
}*/

static constexpr uint64_t rateLimitMs = 15 * 1000; // 15 seconds

void StaffAlert::onHttpResponse(HttpResponseEvent event)
{
    auto sender = static_cast<StaffAlert*>(event.mSender);
    auto originalRequest = static_cast<HttpRequest*>(event.mOriginalRequest);

    if (event.mStatusCode == 200)
    {
        nlohmann::json json = nlohmann::json::parse(event.mResponse);
        if (json.contains("main"))
        {
            auto main = json["main"];
            PlayerInfo player;
            player.name = main["username_cc"];
            player.rank = main["rank"];
            sender->mPlayers.push_back(player);
            spdlog::info("[StaffAlert] Player found: {} ({})", player.name, player.rank);
        }
    } else if (event.mStatusCode == 404) {
        // Extract the name from the URL
        std::string name = originalRequest->mUrl.substr(originalRequest->mUrl.find_last_of('/') + 1);
        // Name is not found, so we add it to the cache with the rank "NICKED"
        PlayerInfo player;
        player.name = name;
        player.rank = "NICKED";
        sender->mPlayers.push_back(player);
        spdlog::info("[StaffAlert] Player not found: {}, adding as rank 'NICKED'", name);
    } else if (event.mStatusCode == 429) {
        // Rate limited
        spdlog::warn("[StaffAlert] Rate limited, waiting {} seconds", rateLimitMs / 1000);
        ChatUtils::displayClientMessageRaw("§c§l» §r§cRate limited, waiting {} seconds", rateLimitMs / 1000);
        sender->mLastRateLimit = NOW;
    } else {
        spdlog::error("[StaffAlert] Request failed with status code: {}", event.mStatusCode);
    }

    // Don't delete the request here, it will be deleted by BaseTickEvent when the future is done
}


bool StaffAlert::isPlayerCached(const std::string& name) const
{
    return std::ranges::any_of(mPlayers, [name](const auto& player) {
        return player.name == name;
    });
}


const std::string& StaffAlert::getRank(const std::string& name)
{
    static const std::string unknown = "Unknown";

    for (const auto& player : mPlayers)
    {
        if (player.name == name)
        {
            return player.rank;
        }
    }
    return unknown;
}

// Default ranks: REGULAR, PLUS
// This is not inclusive of YOUTUBE, TWITCH, HIVE_TEAM, etc.
static constexpr std::array ranks = { "REGULAR", "PLUS" };

void StaffAlert::makeRequest(const std::string& name)
{
    if (std::ranges::find(mRequestedPlayers, name) != mRequestedPlayers.end() || isPlayerCached(name))
    {
        return;
    }
    mRequestedPlayers.push_back(name);

    auto request = std::make_unique<HttpRequest>(HttpMethod::GET, "https://api.playhive.com/v0/game/all/main/" + name, "", "", &StaffAlert::onHttpResponse, this);
    int64_t now = NOW;
    mRequests.emplace_back( now, std::move(request) );
}

void StaffAlert::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &StaffAlert::onBaseTickEvent>(this);
}

void StaffAlert::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &StaffAlert::onBaseTickEvent>(this);
}

void StaffAlert::onBaseTickEvent(BaseTickEvent& event)
{
    //spdlog::info("[StaffAlert] Players: {}, Requests: {}, Events: {}", mPlayers.size(), mRequests.size(), mPlayerEvents.size()); // Debugging
    for (auto it = mRequests.begin(); it != mRequests.end();)
    {
        if (it->second->isDone())
        {
            it = mRequests.erase(it);
        } else {
            ++it;
        }
    }

    if (mLastRateLimit != 0 && NOW - mLastRateLimit < rateLimitMs)
    {
        return;
    }

    auto player = event.mActor;
    if (!player) return;

    // ---- Request handling ----
    static uint64_t lastSend = 0;
    static uint64_t timeBetweenRequests = 100;

    uint64_t now = NOW;

    if (now - lastSend > timeBetweenRequests)
    {
        lastSend = now;
        if (!mRequests.empty())
        {
            auto& request = mRequests.front();
            if (!request.second->mRequestSent)
            {
                request.second->sendAsync();
                spdlog::trace("[StaffAlert] Sent request [uri: {}]", request.second->mUrl);
            }
        }
    }

    // ---- Event handling ----
    std::unordered_map<mce::UUID, PlayerListEntry>* playerList = player->getLevel()->getPlayerList();
    std::vector<std::string> playerNames;
    for (auto& entry : *playerList | std::views::values)
    {
        playerNames.emplace_back(entry.name);
    }
    static std::vector<std::string> lastPlayerNames = playerNames;

    for (auto& playerName : playerNames)
    {
        if (std::ranges::find(lastPlayerNames, playerName) == lastPlayerNames.end())
        {
            auto je = PlayerEvent{ PlayerEvent::Type::JOIN, playerName };
            mPlayerEvents.push_back(je);
        }
    }

    for (auto& playerName : lastPlayerNames)
    {
        if (std::ranges::find(playerNames, playerName) == playerNames.end())
        {
            auto le = PlayerEvent{ PlayerEvent::Type::LEAVE, playerName };
            mPlayerEvents.push_back(le);
        }
    }

    // Store the last player list
    lastPlayerNames = playerNames;

    // Process the events of cached players
    for (auto it = mPlayerEvents.begin(); it != mPlayerEvents.end();)
    {
        auto& daEvent = *it;
        if (isPlayerCached(daEvent.name))
        {
            std::string rank = getRank(daEvent.name);

            bool isDefaultRank = std::ranges::find(ranks, rank) != ranks.end();

            if (!isDefaultRank && mStaffOnly.mValue || !mStaffOnly.mValue)
            {
                std::string nameColor = isDefaultRank ? "§7" : "§c"; // Gray color for default ranks, Yellow color for custom ranks
                std::string spz = rank;
                if (spz == "NICKED") spz = "§cNICKED"; // Red color for nicked players
                if (daEvent.type == PlayerEvent::Type::JOIN)
                {
                    ChatUtils::displayClientMessageRaw("§a§l» §r§6[{}§6]{} {} §ajoined.", spz, nameColor, daEvent.name);

                    if (mShowNotifications.mValue) NotifyUtils::notify("[" + rank + "] " + daEvent.name + " joined", 5.0f, Notification::Type::Info);
                } else if (daEvent.type == PlayerEvent::Type::LEAVE) {
                    ChatUtils::displayClientMessageRaw("§c§l» §r§6[{}§6]{} {} §cleft.", spz, nameColor, daEvent.name);

                    if (mShowNotifications.mValue) NotifyUtils::notify("[" + rank + "] " + daEvent.name + " left", 5.0f, Notification::Type::Info);
                }
            }
            it = mPlayerEvents.erase(it);
        } else {
            makeRequest(daEvent.name);
            ++it;
        }
    }
};