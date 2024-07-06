//
// Created by vastrakai on 7/5/2024.
//

#include "ChestStealer.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/ContainerScreenTickEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Inventory/ContainerManagerModel.hpp>
#include <SDK/Minecraft/Inventory/ContainerScreenController.hpp>
#include <SDK/Minecraft/Inventory/ItemStack.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <Utils/MiscUtils/MathUtils.hpp>
#include <Utils/MiscUtils/NotifyUtils.hpp>

void ChestStealer::onEnable()
{
    gFeatureManager->mDispatcher->listen<ContainerScreenTickEvent, &ChestStealer::onContainerScreenTickEvent>(this);
}

void ChestStealer::onDisable()
{
    gFeatureManager->mDispatcher->deafen<ContainerScreenTickEvent, &ChestStealer::onContainerScreenTickEvent>(this);
}

uint64_t ChestStealer::getDelay() const
{
    if (mRandomizeDelay.mValue)
    {
        return static_cast<uint64_t>(MathUtils::random(mRandomizeMin.mValue, mRandomizeMax.mValue));
    }
    return static_cast<uint64_t>(mDelay.mValue);
}

void ChestStealer::onContainerScreenTickEvent(ContainerScreenTickEvent& event) const
{
    static uint64_t lastSteal = 0;
    auto csc = event.mController;
    auto player = ClientInstance::get()->getLocalPlayer();
    auto container = player->getContainerManagerModel();

    static bool isStealing = false;

    std::vector<int> itemz = {};
    for (int i = 0; i < 54; i++) {
        ItemStack* stack = container->getSlot(i);
        if (stack && stack->mItem) itemz.push_back(i);
    }

    if (itemz.empty())
    {
        if (lastSteal + 200 < NOW) {
            isStealing = false;
            csc->_tryExit();
        }
        return;
    }

    static uint64_t delay = getDelay();

    if (!isStealing) {
        NotifyUtils::Notify("Stealing!", 1.0f + (static_cast<float>(delay) / 1000.0f * itemz.size()), Notification::Type::Info);
        isStealing = true;
    }

    if (lastSteal + delay < NOW) {
        for (const int i : itemz) {
            csc->handleAutoPlace("container_items", i);
            lastSteal = NOW;
            delay = getDelay(); // Randomize delay again
            break;
        }
    }
}
