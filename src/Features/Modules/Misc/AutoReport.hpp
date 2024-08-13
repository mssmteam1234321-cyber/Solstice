#pragma once
//
// Created by vastrakai on 8/13/2024.
//


class AutoReport : public ModuleBase<AutoReport> {
public:
    NumberSetting mDelay = NumberSetting("Delay", "The delay in seconds between each report", 5, 1, 10, 1);

    AutoReport() : ModuleBase("AutoReport", "Automatically reports players", ModuleCategory::Misc, 0, false) {
        addSettings(
            &mDelay
        );

        mNames = {
            {Lowercase, "autoreport"},
            {LowercaseSpaced, "auto report"},
            {Normal, "AutoReport"},
            {NormalSpaced, "Auto Report"}
        };
    }

    unsigned int mLastFormId = 0;
    bool mHasFormOpen = false;
    bool mIsReportMenu = false;
    std::string mJson;
    std::vector<std::pair<uintptr_t, std::string>> mQueuedCommands;
    std::string mLastFormTitle;
    bool mFinishedReporting = true;
    std::string mLastPlayer = "";

    int mReportStage = 0;

    uint64_t mLastExecTime = 0;

    uint64_t mLastCommandTime = 0;
    uint64_t mLastReportTime = 0;
    uint64_t mLastFormTime = 0;

    uint64_t mLastDimensionChange = 0;
    uint64_t mLastTeleport = 0;

    void onEnable() override;
    void onDisable() override;
    void submitForm(int buttonId);
    void closeForm();
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);


    std::vector<std::string> mReportedPlayers;
};
