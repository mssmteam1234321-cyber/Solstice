//
// Created by vastrakai on 7/4/2024.
//

#include "Notifications.hpp"
#include <Features/Events/NotifyEvent.hpp>
#include <Features/Events/RenderEvent.hpp>
#include <Utils/FontHelper.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>

#include "spdlog/spdlog.h"

void Notifications::onEnable()
{
    gFeatureManager->mDispatcher->listen<NotifyEvent, &Notifications::onNotifyEvent>(this);
    gFeatureManager->mDispatcher->listen<ModuleStateChangeEvent, &Notifications::onModuleStateChange>(this);
}

void Notifications::onDisable()
{
    gFeatureManager->mDispatcher->deafen<NotifyEvent, &Notifications::onNotifyEvent>(this);
    gFeatureManager->mDispatcher->deafen<ModuleStateChangeEvent, &Notifications::onModuleStateChange>(this);
}

void Notifications::onRenderEvent(RenderEvent& event)
{
    ImGui::PushFont(FontHelper::Fonts["mojangles_large"]);
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();
    float y = displaySize.y - 10.0f;
    float delta = ImGui::GetIO().DeltaTime;

    // First, remove any notifications that have expired
    std::erase_if(mNotifications, [](const Notification& notification) { return notification.mIsTimeUp; });

    for (auto& notification : mNotifications)
    {
        notification.mTimeShown += delta;
        notification.mIsTimeUp = notification.mTimeShown >= notification.mDuration;
        notification.mCurrentDuration = MathUtils::lerp(notification.mCurrentDuration, notification.mDuration, delta * 6.0f);



    }

    ImGui::PopFont();
}

void Notifications::onModuleStateChange(ModuleStateChangeEvent& event)
{
    const auto notification = Notification(event.mModule->mNames[Normal] + " was " + (event.mEnabled ? "enabled" : "disabled"), Notification::Type::Info, 3.0f);
    mNotifications.push_back(notification);
}

void Notifications::onNotifyEvent(NotifyEvent& event)
{
    mNotifications.push_back(event.mNotification);
}
