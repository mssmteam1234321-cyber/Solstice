//
// Created by alteik on 04/09/2024.
//

#include <Features/Modules/Setting.hpp>
#include <Features/Modules/Module.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>

class AutoClicker : public ModuleBase<AutoClicker>
{
public:
    NumberSetting mCPS = NumberSetting("CPS", "clicks per second", 16, 1, 20, 1);

    AutoClicker() : ModuleBase<AutoClicker>("AutoClicker", "automaticly clicks while holding mouse button", ModuleCategory::Combat, 0, false) {
        mNames = {
                {Lowercase, "autoclicker"},
                {LowercaseSpaced, "auto clicker"},
                {Normal, "AutoClicker"},
                {NormalSpaced, "Auto Clicker"}
        };

        addSettings(&mCPS);
    }

    Actor* GetActorFromEntityId(EntityId entityId);
    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};