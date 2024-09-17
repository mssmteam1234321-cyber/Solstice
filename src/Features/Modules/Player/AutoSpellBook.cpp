//
// Created by Tozic on 9/17/2024.
//

#include "AutoSpellBook.hpp"
#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/GameMode.hpp>
#include <SDK/Minecraft/Inventory/PlayerInventory.hpp>

void AutoSpellBook::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AutoSpellBook::onBaseTickEvent, nes::event_priority::VERY_LAST>(this);
}

void AutoSpellBook::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AutoSpellBook::onBaseTickEvent>(this);
}

void AutoSpellBook::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;

    if (player == nullptr)
        return;

    if (mLastItemUsed + (uint64_t)mDelay.mValue > NOW) {
        return;
    }

    if (player->getHealth() > 12) {
        return;
    }

    ItemStack* itemStack = nullptr;
    int itemSlot = -1;

    for (int i = 0; i < 8; i++) {
        auto item = player->getSupplies()->getContainer()->getItem(i);

        if (item && item->getItem()) {
            if (item->getCustomName() == "§r§dSpell of Life§7 [Use]") {
                itemStack = item;
                itemSlot = i;
                break;
            }
        }
    }

    if (itemStack == nullptr) {
        return;
    }

    std::string a = itemStack->getCustomName();
    if (StringUtils::containsIgnoreCase(a, "§r§dSpell of Life§7 [Use]")) {
        int currentSlot = player->getSupplies()->mSelectedSlot;
        player->getSupplies()->mSelectedSlot = itemSlot;
        player->getGameMode()->baseUseItem(itemStack);
        player->getSupplies()->mSelectedSlot = currentSlot;
        mLastItemUsed = NOW;

        if (mShowNotification.mValue)
            NotifyUtils::notify("Used life spellbook!", 3, Notification::Type::Info);
    }
};