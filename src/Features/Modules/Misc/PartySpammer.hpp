#pragma once
//
// Created by vastrakai on 8/23/2024.
//


class PartySpammer : public ModuleBase<PartySpammer> {
public:
    BoolSetting mActive = BoolSetting("Active", "Whether the party spammer is active", false);
    NumberSetting mDelay = NumberSetting("Delay", "The delay in milliseconds between each party invite", 1000, 0, 10000, 100);

    PartySpammer() : ModuleBase("PartySpammer", "Spam invites players to your party", ModuleCategory::Misc, 0, false) {
        addSetting(&mActive);
        addSetting(&mDelay);

        mNames = {
            {Lowercase, "partyspammer"},
            {LowercaseSpaced, "party spammer"},
            {Normal, "PartySpammer"},
            {NormalSpaced, "Party Spammer"}
        };
    }

    std::string mPlayerToSpam = "Sinister4458";
    std::vector<int> mOpenFormIds;
    std::map<int, std::string> mFormJsons;
    std::map<int, std::string> mFormTitles;
    bool mSentInvite = false;
    bool mInteractable = false;
    uint64_t mLastInteract = 0;

    void onEnable() override;
    void onDisable() override;
    void submitForm(int buttonId, int formId);
    void submitBoolForm(bool buttonId, int formId);
    void closeForm(int formId);
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
};