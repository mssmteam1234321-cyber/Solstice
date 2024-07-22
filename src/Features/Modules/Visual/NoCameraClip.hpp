#pragma once
//
// Created by vastrakai on 7/22/2024.
//

#include <Features/Modules/Module.hpp>

class CameraComponent; // Forward declaration

class NoCameraClip : public ModuleBase<NoCameraClip> {
public:
    NoCameraClip() : ModuleBase<NoCameraClip>("NoCameraClip", "Disables camera clipping (credit to ASM lol)", ModuleCategory::Visual, 0, false) {
        mNames = {
            {Lowercase, "nocameraclip"},
            {LowercaseSpaced, "no camera clip"},
            {Normal, "NoCameraClip"},
            {NormalSpaced, "No Camera Clip"}
        };
    }


    CameraComponent* mFirstPersonCamera = nullptr;
    CameraComponent* mThirdPersonCamera = nullptr;
    CameraComponent* mThirdPersonFrontCamera = nullptr;

    void onEnable() override;
    void onDisable() override;
    void onLookInputEvent(class LookInputEvent& event);
};