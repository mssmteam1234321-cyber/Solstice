#pragma once
//
// Created by vastrakai on 8/16/2024.
//


class StaffAlert : public ModuleBase<StaffAlert>
{
public:
    BoolSetting mStaffOnly = BoolSetting("Staff Only", "Only show events from staff members or nicked players", true);
    BoolSetting mShowNotifications = BoolSetting("Show Notifications", "Show notifications when a staff member/nicked player joins or leaves", true);

    StaffAlert() : ModuleBase("StaffAlert", "Automatically detects staff members", ModuleCategory::Misc, 0, false)
    {
        addSettings(
            &mStaffOnly,
            &mShowNotifications
        );

        mNames = {
            {Lowercase, "staffalert"},
            {LowercaseSpaced, "staff alert"},
            {Normal, "StaffAlert"},
            {NormalSpaced, "Staff Alert"}
        };
    }

    struct PlayerInfo
    {
        std::string name = "";
        std::string rank = "";
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

    std::vector<PlayerEvent> mPlayerEvents;
    std::vector<std::pair<uint64_t, std::unique_ptr<HttpRequest>>> mRequests;
    std::vector<PlayerInfo> mPlayers;
    std::vector<std::string> mRequestedPlayers;
    uint64_t mLastRateLimit = 0;

    static void onHttpResponse(HttpResponseEvent event);
    [[nodiscard]] bool isPlayerCached(const std::string& name) const;
    const std::string& getRank(const std::string& name);
    void makeRequest(const std::string& name);

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};