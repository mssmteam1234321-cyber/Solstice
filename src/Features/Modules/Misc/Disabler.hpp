#pragma once
//
// Created by vastrakai on 8/9/2024.
//

class Disabler : public ModuleBase<Disabler> {
public:
    enum class Mode {
        FlareonOld,
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode to use for the disabler.", Mode::FlareonOld, "FlareonOld");

    Disabler() : ModuleBase<Disabler>("Disabler", "Attempts to disable Anti-Cheat checks by exploiting them.", ModuleCategory::Misc, 0, false){
        addSetting(&mMode);

        mNames = {
            {Lowercase, "disabler"},
            {LowercaseSpaced, "disabler"},
            {Normal, "Disabler"},
            {NormalSpaced, "Disabler"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onSendImmediateEvent(class SendImmediateEvent& event);
};