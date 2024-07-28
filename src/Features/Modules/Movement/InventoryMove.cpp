//
// Created by vastrakai on 7/12/2024.
//

#include "InventoryMove.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

std::vector<unsigned char> gClrBytes = { 0xC3 };
DEFINE_PATCH_FUNC(InventoryMove::patchFunc, SigManager::PlayerMovement_clearInputState, gClrBytes);

void InventoryMove::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &InventoryMove::onBaseTickEvent>(this);
    patchFunc(true);
}

void InventoryMove::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &InventoryMove::onBaseTickEvent>(this);
    patchFunc(false);
}


void InventoryMove::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    auto input = player->getMoveInputComponent();

    bool w = Keyboard::mPressedKeys['W'];
    bool a = Keyboard::mPressedKeys['A'];
    bool s = Keyboard::mPressedKeys['S'];
    bool d = Keyboard::mPressedKeys['D'];
    bool space = Keyboard::mPressedKeys[VK_SPACE];
    bool shift = Keyboard::mPressedKeys[VK_SHIFT];
    bool pressed = w || a || s || d || space || shift;

    std::string screenName = ClientInstance::get()->getScreenName();

    bool isInChatScreen = screenName == "chat_screen";
    if (!isInChatScreen) patchFunc(true);
    else patchFunc(false);

    if (isInChatScreen || !pressed)
    {
        input->mForward = false;
        input->mBackward = false;
        input->mLeft = false;
        input->mRight = false;
        input->mIsJumping = false;
        input->mIsJumping2 = false;
        input->mMoveVector = glm::vec2(0.f, 0.f);
        input->mIsSneakDown = false;
        return;
    }

    if (screenName != "hud_screen") input->mIsMoveLocked = true;
    else input->mIsMoveLocked = false;

    input->mForward = w;
    input->mBackward = s;
    input->mLeft = a;
    input->mRight = d;
    if (screenName != "hud_screen") // I hate this game
    {
        input->mIsJumping = space;
        input->mIsJumping2 = space;
    }
    input->mIsSneakDown = shift;
    input->mMoveVector = MathUtils::getMovement();
}
