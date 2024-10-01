#pragma once
//
// Created by vastrakai on 9/30/2024.
//


class Jesus : public ModuleBase<Jesus>
{
public:
    enum class Mode
    {
        Solid
    };

    EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of jesus", Mode::Solid, "Solid");
    BoolSetting mShift = BoolSetting("Allow Shift", "Allows you to shift to go down", true);

    Jesus() : ModuleBase("Jesus", "Lets you walk on water", ModuleCategory::Movement, 0, false)
    {
        addSettings(
            &mMode,
            &mShift
        );

        mNames = {
            {Lowercase, "jesus"},
            {LowercaseSpaced, "jesus"},
            {Normal, "Jesus"},
            {NormalSpaced, "Jesus"}
        };
    }

    BlockLegacy* mWaterBlock = nullptr;

    bool sdjReplace(bool patch);

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);

    std::string getSettingDisplay() override {
        return mMode.mValues[mMode.as<int>()];
    }
};