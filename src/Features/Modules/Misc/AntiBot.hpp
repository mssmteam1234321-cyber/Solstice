#pragma once
//
// Created by vastrakai on 7/8/2024.
//
#include <Features/Modules/Module.hpp>

class AntiBot : public ModuleBase<AntiBot>
{
public:
    enum class Mode {
        Simple,
        Custom
    };
    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of the AntiBot module.", Mode::Simple, "Simple", "Custom");
    BoolSetting mHitboxCheck = BoolSetting("Hitbox Check", "Whether or not to check the hitbox of the entity.", true);
    BoolSetting mPlayerCheck = BoolSetting("Player Check", "Whether or not to check if the entity is a player.", true);

    AntiBot() : ModuleBase("AntiBot", "Filters out bots from the entity list", ModuleCategory::Misc, 0, true) {
        addSettings(&mMode, &mHitboxCheck, &mPlayerCheck);

        VISIBILITY_CONDITION(mHitboxCheck, mMode.mValue == Mode::Custom);
        VISIBILITY_CONDITION(mPlayerCheck, mMode.mValue == Mode::Custom);

        mNames = {
            {Lowercase, "antibot"},
            {LowercaseSpaced, "anti bot"},
            {Normal, "AntiBot"},
            {NormalSpaced, "Anti Bot"}
        };
    }



    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    bool isBot(class Actor* actor) const;

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.as<int>()];
    }
};