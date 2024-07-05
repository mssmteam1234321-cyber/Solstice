//
// Created by vastrakai on 7/4/2024.
//

#include "NotifyUtils.hpp"

#include <Features/FeatureManager.hpp>
#include <utility>

void NotifyUtils::Notify(std::string message, float duration, Notification::Type type)
{
    const auto notification = Notification(std::move(message), type, duration);
    auto holder = nes::make_holder<NotifyEvent>(notification);
    gFeatureManager->mDispatcher->trigger<NotifyEvent>(holder);
}
