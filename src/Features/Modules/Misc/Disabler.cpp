//
// Created by vastrakai on 8/9/2024.
//

#include "Disabler.hpp"

#include <Features/Events/PacketOutEvent.hpp>
#include <Features/Events/RunUpdateCycleEvent.hpp>
#include <Features/Events/PingUpdateEvent.hpp>
#include <Features/Events/SendImmediateEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>

void Disabler::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &Disabler::onPacketOutEvent, nes::event_priority::VERY_LAST>(this);
    gFeatureManager->mDispatcher->listen<RunUpdateCycleEvent, &Disabler::onRunUpdateCycleEvent, nes::event_priority::VERY_FIRST>(this);
    gFeatureManager->mDispatcher->listen<SendImmediateEvent, &Disabler::onSendImmediateEvent>(this);
    gFeatureManager->mDispatcher->listen<PingUpdateEvent, &Disabler::onPingUpdateEvent, nes::event_priority::FIRST>(this);

    mShouldUpdateClientTicks = true;
}

void Disabler::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &Disabler::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<RunUpdateCycleEvent, &Disabler::onRunUpdateCycleEvent>(this);
    gFeatureManager->mDispatcher->deafen<SendImmediateEvent, &Disabler::onSendImmediateEvent>(this);
    gFeatureManager->mDispatcher->deafen<PingUpdateEvent, &Disabler::onPingUpdateEvent>(this);
}

/*glm::vec2 MathUtils::getMovement() {
    glm::vec2 ret = glm::vec2(0, 0);
    float forward = 0.0f;
    float side = 0.0f;

    bool w = Keyboard::mPressedKeys['W'];
    bool a = Keyboard::mPressedKeys['A'];
    bool s = Keyboard::mPressedKeys['S'];
    bool d = Keyboard::mPressedKeys['D'];

    if (!w && !a && !s && !d)
        return ret;

    static constexpr float forwardF = 1;
    static constexpr float sideF = 0.7071067691f;

    if (w) {
        if (!a && !d)
            forward = forwardF;
        if (a) {
            forward = sideF;
            side = sideF;
        }
        else if (d) {
            forward = sideF;
            side = -sideF;
        }
    }
    else if (s) {
        if (!a && !d)
            forward = -forwardF;
        if (a) {
            forward = -sideF;
            side = sideF;
        }
        else if (d) {
            forward = -sideF;
            side = -sideF;
        }
    }
    else if (!w && !s) {
        if (!a && d) side = -forwardF;
        else side = forwardF;
    }

    ret.x = side;
    ret.y = forward;
    return ret;
}*/

void Disabler::onPacketOutEvent(PacketOutEvent& event) {
#ifdef __DEBUG__
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (mMode.mValue == Mode::Flareon && mDisablerType.mValue == DisablerType::MoveFix) {
        if (event.mPacket->getId() != PacketID::PlayerAuthInput) return;

        // I hate math so much

        auto pkt = event.getPacket<PlayerAuthInputPacket>();
        glm::vec2 moveVec = pkt->mMove;
        glm::vec2 xzVel = { pkt->mPosDelta.x, pkt->mPosDelta.z };
        float yaw = pkt->mRot.y;
        yaw = -yaw;

        if (moveVec.x == 0 && moveVec.y == 0 && xzVel.x == 0 && xzVel.y == 0) return;

        float moveVecYaw = atan2(moveVec.x, moveVec.y);
        moveVecYaw = glm::degrees(moveVecYaw);

        float movementYaw = atan2(xzVel.x, xzVel.y);
        float movementYawDegrees = movementYaw * (180.0f / M_PI);

        float yawDiff = movementYawDegrees - yaw;

        float newMoveVecX = sin(glm::radians(yawDiff));
        float newMoveVecY = cos(glm::radians(yawDiff));
        glm::vec2 newMoveVec = { newMoveVecX, newMoveVecY };

        if (abs(newMoveVec.x) < 0.001) newMoveVec.x = 0;
        if (abs(newMoveVec.y) < 0.001) newMoveVec.y = 0;
        if (moveVec.x == 0 && moveVec.y == 0) newMoveVec = { 0, 0 };

        // Remove all old flags
        pkt->mInputData &= ~AuthInputAction::UP;
        pkt->mInputData &= ~AuthInputAction::DOWN;
        pkt->mInputData &= ~AuthInputAction::LEFT;
        pkt->mInputData &= ~AuthInputAction::RIGHT;
        pkt->mInputData &= ~AuthInputAction::UP_RIGHT;
        pkt->mInputData &= ~AuthInputAction::UP_LEFT;

        pkt->mMove = newMoveVec;
        pkt->mVehicleRotation = newMoveVec; // ???? wtf mojang
        pkt->mInputMode = InputMode::MotionController;

        // Get the move direction
        bool forward = newMoveVec.y > 0;
        bool backward = newMoveVec.y < 0;
        bool left = newMoveVec.x < 0;
        bool right = newMoveVec.x > 0;

        static bool isSprinting = false;
        bool startedThisTick = false;
        // if the flags contain isSprinting, set the flag
        if (pkt->hasInputData(AuthInputAction::START_SPRINTING)) {
            isSprinting = true;
            startedThisTick = true;
        } else if (pkt->hasInputData(AuthInputAction::STOP_SPRINTING)) {
            isSprinting = false;
        }

        spdlog::info("Forward: {}, Backward: {}, Left: {}, Right: {}", forward ? "true" : "false", backward ? "true" : "false", left ? "true" : "false", right ? "true" : "false");

        if (!forward)
        {
            // Remove all sprint flags
            pkt->mInputData &= ~AuthInputAction::START_SPRINTING;
            if (isSprinting && !startedThisTick) {
                pkt->mInputData |= AuthInputAction::STOP_SPRINTING;
                spdlog::info("Stopping sprint");
            }

            spdlog::info("Not moving forward");

            pkt->mInputData &= ~AuthInputAction::SPRINTING;
            pkt->mInputData &= ~AuthInputAction::START_SNEAKING;

            spdlog::info("Removed sprinting and sneaking flags");

            // Stop the player from sprinting
            player->getMoveInputComponent()->setmIsSprinting(false);
        }

        return;
    }
#endif
    if (mMode.mValue != Mode::Sentinel) return;

    if (event.mPacket->getId() == PacketID::PlayerAuthInput) {
        static bool desyncY = false;
        auto pkt = event.getPacket<PlayerAuthInputPacket>();
        if (mShouldUpdateClientTicks) mClientTicks = pkt->mClientTick;
        pkt->mInputData |= AuthInputAction::START_GLIDING | AuthInputAction::JUMPING | AuthInputAction::WANT_UP | AuthInputAction::JUMP_DOWN | AuthInputAction::NORTH_JUMP_DEPRECATED | AuthInputAction::START_JUMPING;
        pkt->mInputData &= ~AuthInputAction::STOP_GLIDING;
        pkt->mInputData |= AuthInputAction::SPRINT_DOWN | AuthInputAction::SPRINTING | AuthInputAction::START_SPRINTING;
        pkt->mInputData &= ~AuthInputAction::STOP_SPRINTING;
        pkt->mInputData &= ~AuthInputAction::SNEAK_DOWN;
        pkt->mInputData &= ~AuthInputAction::SNEAKING;
        pkt->mInputData &= ~AuthInputAction::START_SNEAKING;
        glm::vec3 pos = pkt->mPos;
        glm::vec3 deltaPos = pkt->mPosDelta;
        glm::vec3 lastPos = pkt->mPos - deltaPos;

        // send tampered packet
        pkt->mClientTick = mClientTicks;
        pkt->mPos = { INT_MAX, INT_MAX, INT_MAX };
        ClientInstance::get()->getPacketSender()->sendToServer(pkt);
        mClientTicks++;

        pkt->mClientTick = mClientTicks;
        pkt->mPos = { -INT_MAX, -INT_MAX, -INT_MAX };
        ClientInstance::get()->getPacketSender()->sendToServer(pkt);
        mClientTicks++;

        // send normal packet
        pkt->mClientTick = mClientTicks;
        pkt->mPos = pos;
        mClientTicks++;
    }
    else if (event.mPacket->getId() == PacketID::NetworkStackLatency) {
        event.mCancelled = true;
    }
    else if (event.mPacket->getId() == PacketID::InventoryTransaction)
    {
        auto packet = event.getPacket<InventoryTransactionPacket>();

        auto* ait = reinterpret_cast<ItemUseOnActorInventoryTransaction*>(packet->mTransaction.get());
        if (ait->mActionType == ItemUseOnActorInventoryTransaction::ActionType::Attack)
        {
            auto actor = ActorUtils::getActorFromUniqueId(ait->mActorId);
            if (!actor) return;
            if (mFirstAttackedActor && actor != mFirstAttackedActor) {
                event.mCancelled = true;
                return;
            }
            mFirstAttackedActor = actor;
        }
    }
}

void Disabler::onRunUpdateCycleEvent(RunUpdateCycleEvent& event) {
    if (mMode.mValue != Mode::Sentinel) return;

    if (event.isCancelled() || event.mApplied) return;

    Sleep(101);
    mFirstAttackedActor = nullptr;
}

static constexpr unsigned char newTimestamp[9] = { 0x3, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF };

int64_t Disabler::getDelay() const
{
    if (mRandomizeDelay.mValue)
    {
        return MathUtils::random(mMinDelay.mValue, mMaxDelay.mValue);
    }
    return mDelay.mValue;
}

void Disabler::onPingUpdateEvent(PingUpdateEvent& event)
{
    if (mMode.mValue != Mode::Flareon) return;
    if (mDisablerType.mValue == DisablerType::PingSpoof)
    {
        //event.mPing = 0; // hide ping visually
    }
}

void Disabler::onSendImmediateEvent(SendImmediateEvent& event)
{

    if (mMode.mValue != Mode::Flareon) return;
#ifdef __DEBUG__
    if (mDisablerType.mValue == DisablerType::MoveFix) return;
#endif
    uint8_t packetId = event.send[0];
    if (packetId == 0)
    {
        if (mDisablerType.mValue == DisablerType::PingHolder) {
            event.cancel();
            return;
        }

        uint64_t timestamp = *reinterpret_cast<uint64_t*>(&event.send[1]);
        timestamp = _byteswap_uint64(timestamp);
        timestamp -= getDelay();
        uint64_t now = NOW;
        timestamp = _byteswap_uint64(timestamp);
        *reinterpret_cast<uint64_t*>(&event.send[1]) = timestamp;

        event.mModified = true;
    }
}

