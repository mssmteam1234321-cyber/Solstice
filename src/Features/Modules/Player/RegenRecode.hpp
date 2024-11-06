#pragma once
//
// Created by alteik on 06/11/2024.
//

// all things i removed was useless / #deceted

class RegenRecode : public ModuleBase<RegenRecode>
{
public:

    struct PathFindingResult {
        glm::ivec3 blockPos;
        float time;
    };

    enum class CalcMode {
        Minecraft
#ifdef __PRIVATE_BUILD__
        ,Fastest
#endif
    };

    enum class UncoverMode {
        PathFinding
#ifdef __PRIVATE_BUILD__
        ,UnderGround
#endif
    };

#ifdef __PRIVATE_BUILD__
    enum class StealPriority {
        Mine,
        Steal
    };

    enum class ConfuseMode {
        Always,
        Auto
    };

    enum class AntiConfuseMode {
        Redstone,
        Exposed
    };

    enum class OreSelectionMode {
        Normal,
        Closest
    };
#endif

    EnumSettingT<CalcMode> mCalcMode = EnumSettingT<CalcMode>("Calc Mode", "The calculation mode for destroy speed", CalcMode::Minecraft, "Minecraft", "Fastest");

    NumberSetting mRange = NumberSetting("Range", "The max range for destroying blocks", 5, 0, 10, 0.01);
    NumberSetting mDestroySpeed = NumberSetting("Redstone Destroy Speed", "The destroy speed for Regen", 1, 0.01, 1, 0.01);
    NumberSetting mOtherDestroySpeed = NumberSetting("Other Destroy Speed", "The other destroy speed for Regen", 1, 0.01, 1, 0.01);

#ifdef __PRIVATE_BUILD__
    BoolSetting mDynamicDestroySpeed = BoolSetting("Dynamic Destroy Speed", "use faster destroy speed to specified block", false);
    BoolSetting mOnGroundOnly = BoolSetting("OnGround Only", "use dynamic destroy speed only on ground", false);
    BoolSetting mNuke = BoolSetting("Nuke", "destroy block instantly", false);
#endif

    BoolSetting mUncover = BoolSetting("Uncover", "Uncover redstone if nothing around you is already exposed", false);
    EnumSettingT<UncoverMode> mUncoverMode = EnumSettingT<UncoverMode>("Uncover Mode", "The mode to uncover", UncoverMode::PathFinding, "PathFinding"
#ifdef __PRIVATE_BUILD__
    ,"UnderGround"
#endif
    );
#ifdef __PRIVATE_BUILD__
    NumberSetting mUncoverRange = NumberSetting("Uncover Range", "The max range for uncovering blocks", 3, 1, 8, 1);
#else
    NumberSetting mUncoverRange = NumberSetting("Uncover Range", "The max range for uncovering blocks", 3, 1, 3, 1);
#endif

#ifdef __PRIVATE_BUILD__
    BoolSetting mDynamicUncover = BoolSetting("Dynamic Uncover", "Disables uncover if enemy uncovering ores", false);
    NumberSetting mDisableDuration = NumberSetting("Disable Duration", "The time for dynamic uncover", 3, 1, 10, 1);
#endif

    BoolSetting mQueueRedstone = BoolSetting("Queue Redstone", "Queue redstone blocks to break when max absorption is reached", false);

#ifdef __PRIVATE_BUILD__
    BoolSetting mSteal = BoolSetting("Steal", "Steal the enemy's ore", false);
    NumberSetting mStealerTimeout = NumberSetting("Stealer Timeout", "The max duration for stealer", 1500, 500, 5000, 250);
    EnumSettingT<StealPriority> mStealPriority = EnumSettingT<StealPriority>("Steal Priority", "Ore stealing priority", StealPriority::Mine, "Mine", "Steal");
    BoolSetting mDelayedSteal = BoolSetting("Delayed Steal", "Add delay in steal", false);
    NumberSetting mOpponentDestroySpeed = NumberSetting("Opponent Speed", "enemy's destroy speed", 1, 0.01, 1, 0.01);
    BoolSetting mAlwaysSteal = BoolSetting("Always Steal", "Steal the enemy's ore when max absorption is reached", false);
#ifdef __DEBUG__
    BoolSetting mReplace = BoolSetting("Replacer", "kicks other hackers while hvh", false);
#endif
    BoolSetting mStealerDetecter = BoolSetting("Stealer Detector", "Does some funnies if stealer detected :>", false);
    NumberSetting mAmountOfBlocksToDetect = NumberSetting("Stolen Blocks to Detect", "amount of blocks that should be stolen in past 5 seconds to detect stealer", 4, 1, 10, 1);
    BoolSetting mDisableUncover = BoolSetting("Disable Uncover", "Disables uncover for some seconds", false);
    NumberSetting mDisableSeconds = NumberSetting("Disable Duration", "amount of seconds uncover will be disabled for", 5, 1, 15, 1);

    BoolSetting mAntiSteal = BoolSetting("Anti Steal", "Stop mining if enemy tried to steal ore", false);

    BoolSetting mOreFaker = BoolSetting("Ore Faker", "Fakes targetting ore", false);
    BoolSetting mExposed = BoolSetting("Exposed", "Include exposed ore", false);
    BoolSetting mUnexposed = BoolSetting("Unexposed", "Include unexposed ore", false);

    BoolSetting mConfuse = BoolSetting("Confuse", "Confuse stealer", false);
    EnumSettingT<ConfuseMode> mConfuseMode = EnumSettingT<ConfuseMode>("Confuse Mode", "The mode for confuser", ConfuseMode::Always, "Always", "Auto");
    NumberSetting mConfuseDuration = NumberSetting("Confuse Duration", "The time for confuse", 3000, 1000, 10000, 500);

    BoolSetting mAntiConfuse = BoolSetting("Anti Confuse", "Ignore confused blocks due to false stealing", false);
    EnumSettingT<AntiConfuseMode> mAntiConfuseMode = EnumSettingT<AntiConfuseMode>("Anti Confuse Mode", "The anti confuser mode", AntiConfuseMode::Redstone, "Redstone", "Exposed");

    BoolSetting mBlockOre = BoolSetting("Block Ore", "Cover opponent targetting ore", false);
    NumberSetting mBlockRange = NumberSetting("Block Range", "The max range for ore blocker", 5, 0, 10, 0.01);
    BoolSetting mMulti = BoolSetting("Multi", "Allows placing multiple blocks", false);

    EnumSettingT<OreSelectionMode> mOreSelectionMode = EnumSettingT<OreSelectionMode>("Ore Selection Mode", "The mode for ore selection", OreSelectionMode::Normal, "Normal", "Closest");
#endif

    BoolSetting mInfiniteDurability = BoolSetting("Infinite Durability", "Infinite durability for tools (may cause issues!)", false);

    BoolSetting mAlwaysMine = BoolSetting("Always mine", "Keep mining ore", false);

    BoolSetting mSwing = BoolSetting("Swing", "Swings when destroying blocks", false);

    BoolSetting mRenderBlock = BoolSetting("Render Block", "Renders the block you are currently breaking", true);
    NumberSetting mOffset = NumberSetting("Offeset From Center", "render pos offset from center", 20, 0, 200, 0.1);
    BoolSetting mRenderProgressBar = BoolSetting("Render Progress Bar", "Renders the progress bar", true);

#ifdef __PRIVATE_BUILD__
    BoolSetting mDebug = BoolSetting("Debug", "Send debug message in chat", false);
    BoolSetting mStealNotify = BoolSetting("Steal Notify", "Send message in chat when u stole ore / ur ore was stolen", true);
    BoolSetting mConfuseNotify = BoolSetting("Confuse Notify", "Send message in chat when confused stealer", true);
    BoolSetting mBlockNotify = BoolSetting("Block Notify", "Send message in chat when you blocked ore/ore got covered", true);
#endif

    RegenRecode() : ModuleBase("RegenRecode", "regen but better", ModuleCategory::Player, 0, false)
    {
        addSetting(&mRange);
        addSettings(&mCalcMode, &mDestroySpeed, &mOtherDestroySpeed);
#ifdef __PRIVATE_BUILD__
        addSettings(&mDynamicDestroySpeed, &mOnGroundOnly, &mNuke);
#endif
        addSettings(&mUncover, &mUncoverMode, &mUncoverRange);
#ifdef __PRIVATE_BUILD__
        addSettings(&mDynamicUncover, &mDisableDuration);
#endif
        addSetting(&mQueueRedstone);
#ifdef __PRIVATE_BUILD__
        addSettings(&mSteal, &mStealerTimeout, &mStealPriority, &mDelayedSteal, &mOpponentDestroySpeed, &mAlwaysSteal
#ifdef __DEBUG__
        ,&mReplace
#endif
        );
        addSettings(&mStealerDetecter, &mAmountOfBlocksToDetect, &mDisableUncover, &mDisableSeconds);
        addSetting(&mAntiSteal);
        addSettings(&mOreFaker, &mExposed, &mUnexposed);
        addSettings(&mConfuse, &mConfuseMode, &mConfuseDuration);
        addSettings(&mAntiConfuse, &mAntiConfuseMode);
        addSettings(&mBlockOre, &mBlockRange, &mMulti);
        addSetting(&mOreSelectionMode);
#endif
        addSetting(&mInfiniteDurability);
        addSetting(&mAlwaysMine);
        addSetting(&mSwing);
        addSettings(&mRenderBlock, &mRenderProgressBar, &mOffset);
#ifdef __PRIVATE_BUILD__
        addSettings(&mDebug, &mStealNotify, &mConfuseNotify, &mBlockNotify);
#endif

        VISIBILITY_CONDITION(mDestroySpeed, mCalcMode.mValue == CalcMode::Minecraft);
        VISIBILITY_CONDITION(mOtherDestroySpeed, mCalcMode.mValue == CalcMode::Minecraft);

        VISIBILITY_CONDITION(mUncoverMode, mUncover.mValue);
        VISIBILITY_CONDITION(mUncoverRange, mUncover.mValue && mUncoverMode.mValue == UncoverMode::PathFinding);

#ifdef __PRIVATE_BUILD__
        VISIBILITY_CONDITION(mOnGroundOnly, mDynamicDestroySpeed.mValue);
        VISIBILITY_CONDITION(mNuke, mDynamicDestroySpeed.mValue && mOnGroundOnly.mValue);

        VISIBILITY_CONDITION(mDisableDuration, mDynamicUncover.mValue);

        VISIBILITY_CONDITION(mStealerTimeout, mSteal.mValue);
        VISIBILITY_CONDITION(mStealPriority, mSteal.mValue);
        VISIBILITY_CONDITION(mDelayedSteal, mSteal.mValue);
        VISIBILITY_CONDITION(mOpponentDestroySpeed, mSteal.mValue && mDelayedSteal.mValue);
        VISIBILITY_CONDITION(mAlwaysSteal, mSteal.mValue);
        VISIBILITY_CONDITION(mReplace, mSteal.mValue);

        VISIBILITY_CONDITION(mAmountOfBlocksToDetect, mStealerDetecter.mValue);
        VISIBILITY_CONDITION(mDisableUncover, mStealerDetecter.mValue);
        VISIBILITY_CONDITION(mDisableSeconds, mStealerDetecter.mValue && mDisableUncover.mValue);

        VISIBILITY_CONDITION(mExposed, mOreFaker.mValue);
        VISIBILITY_CONDITION(mUnexposed, mOreFaker.mValue);

        VISIBILITY_CONDITION(mConfuseMode, mConfuse.mValue);
        VISIBILITY_CONDITION(mConfuseDuration, mConfuse.mValue && mConfuseMode.mValue == ConfuseMode::Auto);

        VISIBILITY_CONDITION(mAntiConfuseMode, mAntiConfuse.mValue);

        VISIBILITY_CONDITION(mBlockRange, mBlockOre.mValue);
        VISIBILITY_CONDITION(mMulti, mBlockOre.mValue);

        VISIBILITY_CONDITION(mOffset, mRenderProgressBar.mValue);

        VISIBILITY_CONDITION(mStealNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mConfuseNotify, mDebug.mValue);
        VISIBILITY_CONDITION(mBlockNotify, mDebug.mValue);
#endif

        mNames = {
            {Lowercase, "regenrecode"},
            {LowercaseSpaced, "regen recode"},
            {Normal, "RegenRecode"},
            {NormalSpaced, "Regen Recode"},
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onSendImmediateEvent(class SendImmediateEvent& event);
    void onPingUpdateEvent(class PingUpdateEvent& event);
};


