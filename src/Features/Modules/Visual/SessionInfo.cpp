//
// Created by alteik on 02/10/2024.
//

#include "SessionInfo.hpp"
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Network/PacketID.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <Hook/Hooks/RenderHooks/D3DHook.hpp>

std::vector<std::string> gamemodesToGetStatsFor = { "sky", "sky-duos", "sky-squads", "sky-mega", "ctf", "bed", "bed-duos", "bed-squads", "sg", "sg-duos" };

static int mTotalKills = 0;
static int mTotalDeaths = 0;
static int mTotalPlayed = 0;
static int mCompletedRequests = 0;
static bool mShouldUpdate = false;

void SessionInfo::resetStatistics() {
    mTotalKills = 0;
    mTotalDeaths = 0;
    mTotalPlayed = 0;
    mCompletedRequests = 0;
}

void SessionInfo::makeRequestsForAllGamemodes(const std::string &gamertag) {
    resetStatistics();

    for (const auto& gamemode : gamemodesToGetStatsFor) {
        auto request = std::make_unique<HttpRequest>(
                HttpMethod::GET,
                "https://api.playhive.com/v0/game/all/" + gamemode + "/" + gamertag,
                "",
                "",
                &SessionInfo::onHttpResponse,
                this
        );
        int64_t now = NOW;
        mRequests.emplace_back(now, std::move(request));
    }
}

void SessionInfo::onHttpResponse(HttpResponseEvent event) {
    auto sender = static_cast<SessionInfo *>(event.mSender);

    if (event.mStatusCode == 200) {
        nlohmann::json json = nlohmann::json::parse(event.mResponse);

        int kills = 0, deaths = 0, played = 0;

        if (json.contains("kills")) {
            kills = json["kills"];
        }
        if (json.contains("deaths")) {
            deaths = json["deaths"];
        }
        if (json.contains("played")) {
            played = json["played"];
        }

        mTotalKills += kills;
        mTotalDeaths += deaths;
        mTotalPlayed += played;

        spdlog::info("[SessionInfo] Stats updated from gamemode!");

    } else if (event.mStatusCode == 404) {
        spdlog::info("[SessionInfo] Stats not found for this gamemode!");
    } else {
        spdlog::error("[SessionInfo] Request failed with status code: {}", event.mStatusCode);
    }

    mCompletedRequests++;

    if (mCompletedRequests == gamemodesToGetStatsFor.size()) {
        ChatUtils::displayClientMessage("Stats:");
        ChatUtils::displayClientMessage(
                "Kills: " + std::to_string(mTotalKills) +
                ", Deaths: " + std::to_string(mTotalDeaths) +
                ", Games Played: " + std::to_string(mTotalPlayed)
        );
        mShouldUpdate = false;
        spdlog::info("[SessionInfo] All stats updated!");
    }
}

void SessionInfo::onEnable() {
    resetStatistics();
    mShouldUpdate = true;
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &SessionInfo::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &SessionInfo::onPacketInEvent>(this);
}

void SessionInfo::onDisable() {
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &SessionInfo::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &SessionInfo::onPacketInEvent>(this);
}

void SessionInfo::onRenderEvent(RenderEvent &event) {

}

void SessionInfo::onBaseTickEvent(BaseTickEvent& event) {
    auto player = event.mActor;
    if (!player) return;

    mPlayerName = player->getLocalName();

    if(!mShouldUpdate) return;

    for (auto it = mRequests.begin(); it != mRequests.end();) {
        if (it->second->isDone()) {
            it = mRequests.erase(it);
        } else {
            ++it;
        }
    }

    if (!mRequests.empty()) {
        auto &request = mRequests.front();
        if (!request.second->mRequestSent) {
            request.second->sendAsync();
            spdlog::trace("[SessionInfo] Sent request [uri: {}]", request.second->mUrl);
        }
    }
}

void SessionInfo::onPacketInEvent(PacketInEvent &event) {
    auto player = ClientInstance::get()->getLocalPlayer();
    if(!player) return;

    if (event.mPacket->getId() == PacketID::Text) {
        auto tp = event.getPacket<TextPacket>();
        if (tp->mMessage == "§c§l» §r§c§lGame OVER!") {
            mShouldUpdate = true;
            makeRequestsForAllGamemodes(mPlayerName);
        }
    }
}