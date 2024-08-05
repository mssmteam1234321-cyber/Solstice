//
// Created by vastrakai on 7/10/2024.
//

#include "Speed.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RunUpdateCycleEvent.hpp>
#include <Features/Modules/Player/Scaffold.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <SDK/Minecraft/Network/Packets/MobEffectPacket.hpp>
#include <SDK/Minecraft/Network/Packets/SetActorMotionPacket.hpp>

void Speed::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Speed::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Speed::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<RunUpdateCycleEvent, &Speed::onRunUpdateCycleEvent>(this);
    mDamageBoostVal = 1.f;
    mDamageTimerApplied = false;
}

void Speed::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Speed::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Speed::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<RunUpdateCycleEvent, &Speed::onRunUpdateCycleEvent>(this);
    ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);
    mDamageBoostVal = 1.f;
    mDamageTimerApplied = false;
}

void Speed::onRunUpdateCycleEvent(RunUpdateCycleEvent& event)
{
    if (event.isCancelled() || event.mApplied) return;

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    static auto scaffold = gFeatureManager->mModuleManager->getModule<Scaffold>();
    if (scaffold->mEnabled) return;

    bool applyNetskip = false;

    if (mMode.mValue != Mode::Friction || mApplyNetskip.mValue) {
        applyNetskip = true;
    }

    if (!applyNetskip) return;
    Sleep(101);
}

bool Speed::tickSwiftness()
{
    auto player = ClientInstance::get()->getLocalPlayer();
    bool hasSpeed = mEffectTimers.contains(EffectType::Speed);
    for (auto& [effect, time] : mEffectTimers)
    {
        // if time is expired, clear the effect
        if (time < NOW) mEffectTimers.erase(effect);
        if (time > NOW && effect == EffectType::Speed)
        {
            if (time - NOW < 100) hasSpeed = false;
            else hasSpeed = true;
        }
    }

    static bool lastSpace = false;
    bool space = Keyboard::mPressedKeys[VK_SPACE];

    int spellbook = ItemUtils::getSwiftnessSpellbook(mSwiftnessHotbar.mValue);
    if (spellbook == -1 && !hasSpeed) return false;

    static auto scaffold = gFeatureManager->mModuleManager->getModule<Scaffold>();

    if (space && !lastSpace && !hasSpeed && spellbook != -1 && !scaffold->mEnabled)
    {
        ItemUtils::useItem(spellbook);
        NotifyUtils::notify("Using swiftness!", 5.f, Notification::Type::Info);
    }

    if (space && hasSpeed || !mHoldSpace.mValue && hasSpeed)
    {
        auto input = player->getMoveInputComponent();
        input->mIsJumping = false;

        glm::vec3 velocity = player->getStateVectorComponent()->mVelocity;
        float movementSpeed = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);

        float movementYaw = atan2(velocity.z, velocity.x);
        float movementYawDegrees = movementYaw * (180.0f / M_PI) - 90.f;

        float playerYawDegrees = player->getActorRotationComponent()->mYaw + MathUtils::getRotationKeyOffset();

        float yawDifference = playerYawDegrees - movementYawDegrees;
        float yawDifferenceRadians = yawDifference * (M_PI / 180.0f);
        float newMovementYaw = movementYaw + yawDifferenceRadians;
        static float fric = 1.f;
        if (!player->isOnGround()) fric *= mSwiftnessFriction.as<float>();
        else fric = 1.f;

        if (Keyboard::isUsingMoveKeys())
        {
            movementSpeed = mSwiftnessSpeed.mValue * fric;
        }
        glm::vec3 newVelocity = {cos(newMovementYaw) * movementSpeed, velocity.y, sin(newMovementYaw) * movementSpeed};
        player->getStateVectorComponent()->mVelocity = newVelocity;

        return true;
    }

    lastSpace = space;
    return false;
}

void Speed::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    float dmgSlowdown = mDamageBoostSlowdown.as<float>();
    mDamageBoostVal = MathUtils::lerp(mDamageBoostVal, 1.f, dmgSlowdown);

    if (mDamageTimer.mValue && mDamageBoostVal > 1.04f)
    {
        mDamageTimerApplied = true;
        ClientInstance::get()->getMinecraftSim()->setSimTimer(mDamageTimerSpeed.as<float>());
    } else mDamageTimerApplied = false;
    if (player->getFlag<RedirectCameraInput>()) return;

    if (mSwiftness.mValue)
    {
        if (tickSwiftness()) return;
    }

    if (mMode.mValue == Mode::Friction)
    {
        tickFriction(player);
    }
    else if (mMode.mValue == Mode::FlareonV1)
    {
        FrictionPreset preset = {};
        switch (mFlareonPreset.mValue)
        {
            // float speed, float friction, bool timerBoost, float timerSpeed, bool fastFall, int fallTicks, float fallSpeed, JumpType jumpType, float jumpHeight)
            case FlareonPreset::FastFall:
                preset = FrictionPreset(3.75, 0.98, true, 20.20, FastfallMode::Predict, 4, 3.00, false, 0, 0, JumpType::Vanilla, 0.42f);
                break;
            case FlareonPreset::Normal:
                preset = FrictionPreset(3.66, 0.98, false, 20.20, FastfallMode::Predict, 5, 1.00, false, 0, 0, JumpType::Vanilla, 0.42f);
                break;
            case FlareonPreset::Low1:
                preset = FrictionPreset(3.78, 0.98, true, 20.30, FastfallMode::Predict, 1, 2.00, false, 0, 0, JumpType::Vanilla, 0.42f);
                break;
            case FlareonPreset::Low2:
                preset = FrictionPreset(3.75, 0.98, true, 21.00, FastfallMode::Predict, 4, 1.00, false, 0, 0, JumpType::Velocity, 0.29f);
                break;
        }
        tickFrictionPreset(preset);
    }
}

void Speed::onPacketInEvent(PacketInEvent& event)
{
    if (event.mPacket->getId() == PacketID::SetActorMotion && mDamageBoost.mValue)
    {
        auto sam = event.getPacket<SetActorMotionPacket>();
        auto player = ClientInstance::get()->getLocalPlayer();
        if (!player) return;

        if (sam->mRuntimeID == player->getRuntimeID())
        {
            mDamageBoostVal = mDamageBoostSpeed.as<float>();
        }
    }
    if (event.mPacket->getId() == PacketID::MobEffect)
    {
        auto player = ClientInstance::get()->getLocalPlayer();
        if (!player) return;

        auto mep = event.getPacket<MobEffectPacket>();

        if (mep->mRuntimeId != player->getRuntimeID()) return;

        auto effect = std::string(mep->getEffectName());
        auto eventName = std::string(mep->getEventName());
        uint64_t time = mep->mEffectDurationTicks * 50;

        if (mep->mEventId == MobEffectPacket::Event::Add) {
            mEffectTimers[mep->mEffectId] = NOW + time;
        } else if (mep->mEventId == MobEffectPacket::Event::Remove) {
            mEffectTimers.erase(mep->mEffectId);
        }

        spdlog::info("Effect: {} Event: {} Time: {}", effect, eventName, time);
    }

    if (!mEnabled) return;
}

void Speed::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = event.getPacket<PlayerAuthInputPacket>();
        auto player = ClientInstance::get()->getLocalPlayer();
            if (Keyboard::isUsingMoveKeys()) paip->mInputData |= AuthInputAction::JUMPING | AuthInputAction::WANT_UP | AuthInputAction::JUMP_DOWN;
            if (!player->isOnGround() && player->wasOnGround() && Keyboard::isUsingMoveKeys()) {
                paip->mInputData |= AuthInputAction::START_JUMPING;
            }
    }
}

void Speed::tickFriction(Actor* player)
{
    if (!mDamageTimerApplied)
    {
        if (mTimerBoost.mValue) ClientInstance::get()->getMinecraftSim()->setSimTimer(mTimerSpeed.as<float>());
        else ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);
    }

    static auto friction = mFriction.as<float>();
    static int tick = 0;

    if (player->isOnGround())
    {
        friction = 1.f;
        tick = 0;
    }
    else
    {
        friction *= mFriction.as<float>();
        tick++;
    }

    // Predicts the next y velocity and applies it
    if (mFastFall.mValue == FastfallMode::Predict)
    {
        if (tick == mFallTicks.as<int>())
        {
            auto fallSpeed = mFallSpeed.as<float>();
            auto stateVector = player->getStateVectorComponent();
            for (int i = 0; i < fallSpeed; i++)
            {
                stateVector->mVelocity.y = (stateVector->mVelocity.y - 0.08f) * 0.98f;
            }
        }
        if (mFastFall2.mValue && tick == mFallTicks2.as<int>())
        {
            auto fallSpeed = mFallSpeed2.as<float>();
            auto stateVector = player->getStateVectorComponent();
            for (int i = 0; i < fallSpeed; i++)
            {
                stateVector->mVelocity.y = (stateVector->mVelocity.y - 0.08f) * 0.98f;
            }
        }
    } // Sets the velocity directly
    else if (mFastFall.mValue == FastfallMode::SetVel)
    {
        if (tick == mFallTicks.as<int>())
        {
            auto fallSpeed = mFallSpeed.as<float>() / 10;
            auto stateVector = player->getStateVectorComponent();
            stateVector->mVelocity.y = -fallSpeed;
        }
        if (mFastFall2.mValue && tick == mFallTicks2.as<int>())
        {
            auto fallSpeed = mFallSpeed2.as<float>() / 10;
            auto stateVector = player->getStateVectorComponent();
            stateVector->mVelocity.y = -fallSpeed;
        }
    }

    glm::vec2 motion = MathUtils::getMotion(player->getActorRotationComponent()->mYaw, ((mSpeed.as<float>() * mDamageBoostVal) / 10) * friction);
    auto stateVector = player->getStateVectorComponent();
    stateVector->mVelocity = {motion.x, stateVector->mVelocity.y, motion.y};

    bool usingMoveKeys = Keyboard::isUsingMoveKeys();
    if (usingMoveKeys && mJumpType.mValue == JumpType::Vanilla) {
        if (player->isOnGround())
        {
            player->jumpFromGround();
            player->getStateVectorComponent()->mVelocity.y = mJumpHeight.as<float>();
        }
    } else if (usingMoveKeys && mJumpType.mValue == JumpType::Velocity) {
        if (player->isOnGround()) {
            player->getStateVectorComponent()->mVelocity.y = mJumpHeight.as<float>();
        }
    }
};

// same as above but all settings are passed as arguments
void Speed::tickFrictionPreset(FrictionPreset& preset)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;
    bool timerBoostSettting = preset.timerBoost;
    float timerSpeedSetting = preset.timerSpeed;
    float frictionSetting = preset.friction;
    float speedSetting = preset.speed;
    FastfallMode fastFallSetting = preset.fastFall;
    int fallTicksSetting = preset.fallTicks;
    float fallSpeedSetting = preset.fallSpeed;
    bool fastFall2Setting = preset.fastFall2;
    int fallTicks2Setting = preset.fallTicks2;
    float fallSpeed2Setting = preset.fallSpeed2;
    JumpType jumpTypeSetting = preset.jumpType;
    float jumpHeightSetting = preset.jumpHeight;

    if (!mDamageTimerApplied)
    {
        if (timerBoostSettting) ClientInstance::get()->getMinecraftSim()->setSimTimer(timerSpeedSetting);
        else ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);
    }

    static float friction = frictionSetting;
    static int tick = 0;

    if (player->isOnGround())
    {
        friction = 1.f;
        tick = 0;
    }
    else
    {
        friction *= frictionSetting;
        tick++;
    }

    // Predicts the next y velocity and applies it
    if (fastFallSetting == FastfallMode::Predict)
    {
        if (tick == fallTicksSetting)
        {
            auto fallSpeed = fallSpeedSetting;
            auto stateVector = player->getStateVectorComponent();
            for (int i = 0; i < fallSpeed; i++)
            {
                stateVector->mVelocity.y = (stateVector->mVelocity.y - 0.08f) * 0.98f;
            }
        }
        if (fastFall2Setting && tick == fallTicks2Setting)
        {
            auto fallSpeed = fallSpeed2Setting;
            auto stateVector = player->getStateVectorComponent();
            for (int i = 0; i < fallSpeed; i++)
            {
                stateVector->mVelocity.y = (stateVector->mVelocity.y - 0.08f) * 0.98f;
            }
        }
    } // Sets the velocity directly
    else if (fastFallSetting == FastfallMode::SetVel)
    {
        if (tick == fallTicksSetting)
        {
            auto fallSpeed = fallSpeedSetting / 10;
            auto stateVector = player->getStateVectorComponent();
            stateVector->mVelocity.y = -fallSpeed;
        }
        if (fastFall2Setting && tick == fallTicks2Setting)
        {
            auto fallSpeed = fallSpeed2Setting / 10;
            auto stateVector = player->getStateVectorComponent();
            stateVector->mVelocity.y = -fallSpeed;
        }
    }

    glm::vec2 motion = MathUtils::getMotion(player->getActorRotationComponent()->mYaw, ((speedSetting / 10) * mDamageBoostVal) * friction);
    auto stateVector = player->getStateVectorComponent();
    stateVector->mVelocity = {motion.x, stateVector->mVelocity.y, motion.y};

    bool usingMoveKeys = Keyboard::isUsingMoveKeys();
    if (usingMoveKeys && jumpTypeSetting == JumpType::Vanilla) {
        if (player->isOnGround())
        {
            player->jumpFromGround();
            player->getStateVectorComponent()->mVelocity.y = jumpHeightSetting;
        }
    } else if (usingMoveKeys && jumpTypeSetting == JumpType::Velocity) {
        if (player->isOnGround()) {
            player->getStateVectorComponent()->mVelocity.y = jumpHeightSetting;
        }
    }
};