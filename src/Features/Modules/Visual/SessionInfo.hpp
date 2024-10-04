//
// Created by alteik on 02/10/2024.
//

class SessionInfo : public ModuleBase<SessionInfo> {
public:
    SessionInfo() : ModuleBase("SessionInfo", "Custom chat!", ModuleCategory::Visual, 0, false) {
        mNames = {
                {Lowercase, "sessioninfo"},
                {LowercaseSpaced, "session info"},
                {Normal, "SessionInfo"},
                {NormalSpaced, "Session Info"},
        };
    }

    static void onHttpResponse(HttpResponseEvent event);
    void makeRequestsForAllGamemodes(const std::string& gamertag);
    std::vector<std::pair<uint64_t, std::unique_ptr<HttpRequest>>> mRequests;
    std::string mPlayerName = "";
    void resetStatistics();
    uint64_t lastUpdate = 0;

    int style = 0;

    float siY = 350;
    float siX = 10;

    float alphaAnim = 1.f;

    void onEnable() override;
    void onDisable() override;
    void onRenderEvent(class RenderEvent& event);
    void onBaseTickEvent(class BaseTickEvent& event);
};