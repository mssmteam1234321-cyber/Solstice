//
// Created by alteik on 29/09/2024.
//
#pragma once
#include <Features/Modules/Module.hpp>
#include <Features/Modules/Setting.hpp>

class AutoHvH : public ModuleBase<AutoHvH>
{
public:

    AutoHvH() : ModuleBase("AutoHvH", "automaticly detetcts potentialy cheaters", ModuleCategory::Misc, 0, false)
    {

        mNames = {
            {Lowercase, "autohvh"},
            {LowercaseSpaced, "auto hvh"},
            {Normal, "AutoHvH"},
            {NormalSpaced, "Auto HvH"}
        };
    }

    class PlayerInfo : public DataObject
    {
    public:
        std::string name = "";
        uint64_t firstPlayed = 0;
        uint64_t storedAt = 0;

        PlayerInfo(std::string name, uint64_t firstPlayed) : name(std::move(name)), firstPlayed(std::move(firstPlayed)), storedAt(NOW) {}
        PlayerInfo() = default;
    };

    struct PlayerEvent
    {
        enum class Type {
            JOIN,
            LEAVE
        };

        Type type;
        std::string name{};

        PlayerEvent(const Type type, const std::string& name) : type(type), name(name) {}
    };

    std::vector<std::string> checkedPlayers;

    std::vector<std::pair<uint64_t, std::unique_ptr<HttpRequest>>> mRequests;
    std::vector<std::string> mRequestedPlayers;
    uint64_t mLastRateLimit = 0;
    DataStore<PlayerInfo> mPlayerStore = DataStore<PlayerInfo>("Cheaters");

    [[nodiscard]] bool isPlayerCached(const std::string& name) const;
    static void onHttpResponse(HttpResponseEvent event);
    uint64_t getFirstPlayed(const std::string& name);
    void makeRequest(const std::string& name);

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
};