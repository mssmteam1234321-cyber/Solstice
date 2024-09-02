//
// Created by alteik on 02/09/2024.
//
#include <Features/Modules/Module.hpp>

class NoHurtcam : public ModuleBase<NoHurtcam> {
public:
    NoHurtcam() : ModuleBase<NoHurtcam>("NoHurtcam", "Disables hurt animation while getting damage", ModuleCategory::Visual, 0, false) {
        mNames = {
                {Lowercase, "nohurtcam"},
                {LowercaseSpaced, "no hurtcam"},
                {Normal, "NoHurtcam"},
                {NormalSpaced, "No Hurtcam"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
};