#pragma once
//
// 8/30/2024.
//

#include <Features/Modules/Module.hpp>

class  Spammer : public ModuleBase<Spammer> {
public:
    enum class Mode {
        Custom
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of the spammer", Mode::Custom, "Custom");
    NumberSetting mDelayMs = NumberSetting("Delay", "The delay in ms to send message", 1000, 0, 5000, 50);
    Spammer() : ModuleBase("Spammer", "Automatically sends a chat message in specified delay", ModuleCategory::Misc, 0, false)
    {
        addSetting(&mMode);
        addSetting(&mDelayMs);

        mNames = {
            {Lowercase, "spammer"},
            {LowercaseSpaced, "spammer"},
            {Normal, "Spammer"},
            {NormalSpaced, "Spammer"}
        };
    }

    MessageTemplate mSpammerMessageTemplate = MessageTemplate("spammerMessageTemplate", "Good game!");
    
    uint64_t mLastMessageSent = 0;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};