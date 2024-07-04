#pragma once
//
// Created by vastrakai on 7/4/2024.
//

#include <Solstice.hpp>
#include <Features/FeatureManager.hpp>
#include <Features/Events/NotifyEvent.hpp>


class Notifications : public ModuleBase<Notifications> {
public:
    BoolSetting mShowOnToggle = BoolSetting("Show on toggle", "Show a notification when a module is toggled", true);


    Notifications() : ModuleBase("Notifications", "Shows notifications on module toggle and other events", ModuleCategory::Visual, 0, false) {
        addSetting(&mShowOnToggle);

        mNames = {
            {Lowercase, "notifications"},
            {LowercaseSpaced, "notifications"},
            {Normal, "Notifications"},
            {NormalSpaced, "Notifications"}
        };

        gFeatureManager->mDispatcher->listen<RenderEvent, &Notifications::onRenderEvent>(this);
    }

    std::vector<Notification> mNotifications;

    void onEnable() override;
    void onDisable() override;
    void onRenderEvent(class RenderEvent& event);
    void onModuleStateChange(ModuleStateChangeEvent& event);
    void onNotifyEvent(class NotifyEvent& event);
};
