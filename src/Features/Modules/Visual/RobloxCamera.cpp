//
// Created by vastrakai on 7/23/2024.
//

#include "RobloxCamera.hpp"

#include <Features/Events/LookInputEvent.hpp>
#include <Features/Events/MouseEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Options.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/Actor/Components/FlagComponent.hpp>

void RobloxCamera::onEnable()
{
    gFeatureManager->mDispatcher->listen<LookInputEvent, &RobloxCamera::onLookInputEvent>(this);
    gFeatureManager->mDispatcher->listen<MouseEvent, &RobloxCamera::onMouseEvent>(this);

    if (auto player = ClientInstance::get()->getLocalPlayer())
    {
        player->setFlag<RenderCameraFlag>(true);
        player->setFlag<CameraRenderPlayerModel>(true);
        player->setFlag<CameraRenderFirstPersonObjects>(false);
    }
}

void RobloxCamera::onDisable()
{
    gFeatureManager->mDispatcher->deafen<LookInputEvent, &RobloxCamera::onLookInputEvent>(this);
    gFeatureManager->mDispatcher->deafen<MouseEvent, &RobloxCamera::onMouseEvent>(this);

    if (auto player = ClientInstance::get()->getLocalPlayer())
    {
        player->setFlag<RenderCameraFlag>(false);
        player->setFlag<CameraRenderPlayerModel>(false);
        player->setFlag<CameraRenderFirstPersonObjects>(false);
    }

    mCurrentDistance = 0.f;
}

void RobloxCamera::onMouseEvent(MouseEvent& event)
{
    if (ClientInstance::get()->getMouseGrabbed()) return;
    if (!mScroll.mValue) return;
    if (!Keyboard::mPressedKeys[VK_CONTROL]) return;

    if (event.mActionButtonId == 4)
    {
        if(event.mButtonData == 0x78 || event.mButtonData == 0x7F)
        {
            mRadius.mValue -= mScrollIncrement.mValue;
            event.cancel();
        }
        else if (event.mButtonData == 0x88 || event.mButtonData == 0x80 || event.mButtonData == -0x78)
        {
            mRadius.mValue += mScrollIncrement.mValue;
            event.cancel();
        }
    }
}


void RobloxCamera::onLookInputEvent(LookInputEvent& event)
{
    ClientInstance::get()->getOptions()->game_thirdperson->value = 0;

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    player->setFlag<RenderCameraFlag>(true);

    auto camera = event.mFirstPersonCamera;
    glm::vec2 radRot = event.mCameraDirectLookComponent->mRotRads;

    glm::vec3 origin = camera->mOrigin;
    if (mRadius.mValue < 0.f) mRadius.mValue = 0.f;
    float targetDistance = mRadius.mValue;

    mCurrentDistance = MathUtils::lerp(mCurrentDistance, targetDistance, ImGui::GetIO().DeltaTime * 10.f);

    radRot.y = -radRot.y;

    glm::vec3 offset = {
        mCurrentDistance * cos(radRot.y) * sin(radRot.x),
        mCurrentDistance * sin(radRot.y),
        mCurrentDistance * cos(radRot.y) * cos(radRot.x)
    };

    camera->mOrigin = origin + offset;
}
