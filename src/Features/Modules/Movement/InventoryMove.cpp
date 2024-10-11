//
// Created by vastrakai on 7/12/2024.
//

#include "InventoryMove.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/KeyboardMouseSettings.hpp>

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
    if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput) return;
    auto player = event.mActor;
    auto input = player->getMoveInputComponent();
    auto& keyboard = *ClientInstance::get()->getKeyboardSettings();

    bool w = Keyboard::mPressedKeys[keyboard["key.forward"]];
    bool a = Keyboard::mPressedKeys[keyboard["key.left"]];
    bool s = Keyboard::mPressedKeys[keyboard["key.back"]];
    bool d = Keyboard::mPressedKeys[keyboard["key.right"]];
    bool space = Keyboard::mPressedKeys[keyboard["key.jump"]];
    bool shift = Keyboard::mPressedKeys[keyboard["key.sneak"]];
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
