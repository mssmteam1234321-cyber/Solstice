#pragma once
//
// Created by vastrakai on 7/7/2024.
//

#include <Features/Modules/Module.hpp>



class BlockESP : public ModuleBase<BlockESP>
{
public:
    NumberSetting mRadius = NumberSetting("Radius", "The radius of the block esp", 10.f, 1.f, 100.f, 0.01f);
    NumberSetting mUpdateFrequency = NumberSetting("Update Frequency", "The frequency of the block update (in ticks)", 5.f, 1.f, 40.f, 0.01f);
    BoolSetting mRenderFilled = BoolSetting("Render Filled", "Renders the block esp filled", true);
    BoolSetting mEmerald = BoolSetting("Emerald", "Draws around emerald ore", true);
    BoolSetting mDiamond = BoolSetting("Diamond", "Draws around diamond ore", true);
    BoolSetting mGold = BoolSetting("Gold", "Draws around gold ore", true);
    BoolSetting mIron = BoolSetting("Iron", "Draws around iron ore", true);
    BoolSetting mCoal = BoolSetting("Coal", "Draws around coal ore", true);
    BoolSetting mRedstone = BoolSetting("Redstone", "Draws around redstone ore", true);
    BoolSetting mLapis = BoolSetting("Lapis", "Draws around lapis ore", true);

    BlockESP() : ModuleBase("BlockESP", "Draws a box around selected blocks", ModuleCategory::Visual, 0, false) {
        addSettings(&mRadius, &mUpdateFrequency, &mRenderFilled, &mDiamond, &mEmerald, &mGold, &mIron, &mCoal, &mRedstone, &mLapis);

        mNames = {
            {Lowercase, "blockesp"},
            {LowercaseSpaced, "block esp"},
            {Normal, "BlockESP"},
            {NormalSpaced, "Block ESP"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaesTickEvent(class BaseTickEvent& event) const;
    void onRenderEvent(class RenderEvent& event);
};