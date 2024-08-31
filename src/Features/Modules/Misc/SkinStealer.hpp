#pragma once
//
// Created by vastrakai on 8/31/2024.
//


class SkinStealer : public ModuleBase<SkinStealer> {
public:
    SkinStealer() : ModuleBase("SkinStealer", "Steal skins from other players", ModuleCategory::Misc, 0, false) {
        mNames = {
            {Lowercase, "skinstealer"},
            {LowercaseSpaced, "skin stealer"},
            {Normal, "SkinStealer"},
            {NormalSpaced, "Skin Stealer"},
        };
    }

    static std::vector<uint8_t> convToPng(const std::vector<uint8_t>& data, int width, int height);

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void saveSkin(class Actor* skin);
};