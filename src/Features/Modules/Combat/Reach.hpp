//
// Created by alteik on 04/09/2024.
//

#include <Features/Modules/Module.hpp>
#include <Features/Modules/Setting.hpp>

class Reach : public ModuleBase<Reach> {
public:

    NumberSetting mDistance = NumberSetting("Distance", "attack range.", 3.0f, 3.f, 7.0f, 0.01f);

    Reach() : ModuleBase("Reach", "changes attack range.", ModuleCategory::Combat, 0, false) {
        addSetting(&mDistance);

        mNames = {
                {Lowercase, "reach"},
                {LowercaseSpaced, "reach"},
                {Normal, "Reach"},
                {NormalSpaced, "Reach"}
        };
    }

    static float* reachPtr;

    void onEnable() override;
    void onDisable() override;
    void onInit() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};