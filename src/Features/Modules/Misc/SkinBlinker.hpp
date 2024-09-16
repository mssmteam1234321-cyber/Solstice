#pragma once
//
// Created by Tozic on 9/16/2024.
//

class SkinBlinker : public ModuleBase<SkinBlinker> {
public:
    BoolSetting mStealCapes = BoolSetting("Steal Capes", "weither or not to equip other player's capes.", false);

    SkinBlinker() : ModuleBase("SkinBlinker", "Equips skins from other players", ModuleCategory::Misc, 0, false) {
        //addSetting(&mStealCapes);

        mNames = {
            {Lowercase, "skinblinker"},
            {LowercaseSpaced, "skin blinker"},
            {Normal, "SkinBlinker"},
            {NormalSpaced, "Skin Blinker"},
        };
    }

    std::chrono::high_resolution_clock::time_point mTimerDelay = std::chrono::high_resolution_clock::now();
    static inline std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timerMap;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};