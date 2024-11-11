#pragma once
//
// Created by vastrakai on 11/2/2024.
//


class Freelook : public ModuleBase<Freelook> {
public:
    Freelook() : ModuleBase<Freelook>("Freelook", "Allows you to freelook", ModuleCategory::Visual, 0, false) {
        mNames = {
            {Lowercase, "freelook"},
            {LowercaseSpaced, "free look"},
            {Normal, "Freelook"},
            {NormalSpaced, "Free Look"}
        };

        mEnableWhileHeld = true;
    }

    int mLastCameraState = 0;
    glm::vec2 mLookingAngles = glm::vec2(0.0f, 0.0f);
    bool mHadComponent = false;
    std::vector<class CameraDirectLookComponent*> mCameraDirectLookComponents;
    std::unordered_map<CameraDirectLookComponent*, CameraDirectLookComponent> mOriginalRots;

    static void patchUpdates(bool);

    void onEnable() override;
    void onDisable() override;

};