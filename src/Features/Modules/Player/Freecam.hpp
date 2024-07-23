#pragma once
//
// Created by vastrakai on 7/22/2024.
//

#include <Features/Modules/Module.hpp>
#include <SDK/Minecraft/Actor/Components/ActorHeadRotationComponent.hpp>
#include <SDK/Minecraft/Actor/Components/ActorRotationComponent.hpp>
#include <SDK/Minecraft/Actor/Components/MobBodyRotationComponent.hpp>

class Freecam : public ModuleBase<Freecam> {
public:
    NumberSetting mSpeed = NumberSetting("Speed", "Speed of the freecam", 5.5f, 0.1f, 10.0f, 0.1f);
    BoolSetting mDisableOnLagback = BoolSetting("Disable On Lagback", "Disable freecam if you get lagback", true);

    Freecam() : ModuleBase<Freecam>("Freecam", "Move independently of your player", ModuleCategory::Player, 0, false) {
        addSetting(&mSpeed);
        addSetting(&mDisableOnLagback);

        mNames = {
            {Lowercase, "freecam"},
            {LowercaseSpaced, "freecam"},
            {Normal, "Freecam"},
            {NormalSpaced, "Freecam"}
        };
    }

    ActorRotationComponent mLastRot;
    ActorHeadRotationComponent mLastHeadRot;
    MobBodyRotationComponent mLastBodyRot;
    glm::vec3 mAABBMin;
    glm::vec3 mAABBMax;
    glm::vec3 mSvPos;
    glm::vec3 mSvPosOld;
    glm::vec3 mOldPos;

    void onEnable() override;
    void onDisable() override;
    void onPacketInEvent(class PacketInEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
    void onBaseTickEvent(class BaseTickEvent& event);
    void onActorRenderEvent(class ActorRenderEvent& event);
    void onLookInputEvent(class LookInputEvent& event);

};
