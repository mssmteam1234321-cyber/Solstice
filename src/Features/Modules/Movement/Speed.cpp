//
// Created by vastrakai on 7/10/2024.
//

#include "Speed.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RunUpdateCycleEvent.hpp>
#include <Features/Modules/Player/Scaffold.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/MinecraftSim.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include <Utils/Keyboard.hpp>
#include <Utils/MiscUtils/EasingUtil.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>



void Speed::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Speed::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Speed::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<RunUpdateCycleEvent, &Speed::onRunUpdateCycleEvent>(this);


}

void Speed::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Speed::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Speed::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<RunUpdateCycleEvent, &Speed::onRunUpdateCycleEvent>(this);
    ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);
}

void Speed::onRunUpdateCycleEvent(RunUpdateCycleEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    static auto scaffold = gFeatureManager->mModuleManager->getModule<Scaffold>();
    if (scaffold->mEnabled) return;
    if (mMode.as<Mode>() != Mode::Friction) {
        std::this_thread::sleep_for(std::chrono::milliseconds(101));
    } else if (mApplyNetskip.mValue) {
        std::this_thread::sleep_for(std::chrono::milliseconds(101));
    }
}

void Speed::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    if (mMode.as<Mode>() == Mode::Friction)
    {
        tickFriction(player);
    }
    else if (mMode.as<Mode>() == Mode::FlareonV1)
    {
        FrictionPreset preset = {};
        switch (mFlareonPreset.as<FlareonPreset>())
        {
            // float speed, float friction, bool timerBoost, float timerSpeed, bool fastFall, int fallTicks, float fallSpeed, JumpType jumpType, float jumpHeight)
            case FlareonPreset::FastFall:
                preset = FrictionPreset(3.75, 0.98, true, 20.20, true, 4, 3.00, JumpType::Vanilla, 0.42f);
                break;
        }
        tickFrictionPreset(preset);
    }
}

void Speed::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.packet->getId() == PacketID::PlayerAuthInput)
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
    if (mTimerBoost.mValue) ClientInstance::get()->getMinecraftSim()->setSimTimer(mTimerSpeed.as<float>());
    else ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);

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

    if (mFastFall.mValue)
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
    }

    glm::vec2 motion = MathUtils::getMotion(player->getActorRotationComponent()->mYaw, (mSpeed.as<float>() / 10) * friction);
    auto stateVector = player->getStateVectorComponent();
    stateVector->mVelocity = {motion.x, stateVector->mVelocity.y, motion.y};

    bool usingMoveKeys = Keyboard::isUsingMoveKeys();
    if (usingMoveKeys && mJumpType.as<JumpType>() == JumpType::Vanilla) {
        if (player->isOnGround()) player->jumpFromGround();
    } else if (usingMoveKeys && mJumpType.as<JumpType>() == JumpType::Velocity) {
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
    bool fastFallSetting = preset.fastFall;
    int fallTicksSetting = preset.fallTicks;
    float fallSpeedSetting = preset.fallSpeed;
    JumpType jumpTypeSetting = preset.jumpType;
    float jumpHeightSetting = preset.jumpHeight;

    if (timerBoostSettting) ClientInstance::get()->getMinecraftSim()->setSimTimer(timerSpeedSetting);
    else ClientInstance::get()->getMinecraftSim()->setSimTimer(20.f);

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

    if (fastFallSetting)
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
    }

    glm::vec2 motion = MathUtils::getMotion(player->getActorRotationComponent()->mYaw, (speedSetting / 10) * friction);
    auto stateVector = player->getStateVectorComponent();
    stateVector->mVelocity = {motion.x, stateVector->mVelocity.y, motion.y};

    bool usingMoveKeys = Keyboard::isUsingMoveKeys();
    if (usingMoveKeys && jumpTypeSetting == JumpType::Vanilla) {
        if (player->isOnGround()) player->jumpFromGround();
    } else if (usingMoveKeys && jumpTypeSetting == JumpType::Velocity) {
        if (player->isOnGround()) {
            player->getStateVectorComponent()->mVelocity.y = jumpHeightSetting;
        }
    }
};