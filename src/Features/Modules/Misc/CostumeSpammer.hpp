#pragma once
//
// Created by vastrakai on 11/11/2024.
//


class CostumeSpammer : public ModuleBase<CostumeSpammer>
{
public:
    NumberSetting mDelay = NumberSetting("Delay", "The delay between each costume change.", 1.f, 0.01f, 10.f, 0.01f);

    CostumeSpammer() : ModuleBase("CostumeSpammer", "Spams costumes.", ModuleCategory::Misc, 0, false)
    {
        addSettings(&mDelay);

        mNames = {
            {Lowercase, "costumespammer"},
            {LowercaseSpaced, "costume spammer"},
            {Normal, "CostumeSpammer"},
            {NormalSpaced, "Costume Spammer"}
        };
    }

    std::vector<int> mOpenFormIds;
    std::map<int, std::string> mFormJsons;
    std::map<int, std::string> mFormTitles;
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