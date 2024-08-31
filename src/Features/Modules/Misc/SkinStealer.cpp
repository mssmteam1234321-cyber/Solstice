//
// Created by vastrakai on 8/31/2024.
//

#include "SkinStealer.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/Minecraft/Actor/SerializedSkin.hpp>
#include <SDK/Minecraft/World/HitResult.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <Utils/stb_image_write.h>

std::vector<uint8_t> SkinStealer::convToPng(const std::vector<uint8_t>& data, int width, int height)
{
    std::vector<uint8_t> pngData;

    // Callback function to store the PNG data in a vector
    auto writeToVector = [](void* context, void* data, int size) {
        std::vector<uint8_t>* pngData = static_cast<std::vector<uint8_t>*>(context);
        pngData->insert(pngData->end(), (uint8_t*)data, (uint8_t*)data + size);
    };

    // Convert the raw RGBA data to PNG format and store it in the vector
    if (stbi_write_png_to_func(writeToVector, &pngData, width, height, 4, data.data(), width * 4) == 0) {
        // Handle error: PNG conversion failed
        return {};
    }

    return pngData;
}

void SkinStealer::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &SkinStealer::onBaseTickEvent>(this);
}

void SkinStealer::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &SkinStealer::onBaseTickEvent>(this);
}

void SkinStealer::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    Actor* targeted = nullptr;

    EntityId targetId = player->getLevel()->getHitResult()->mEntity.id;
    std::vector<Actor*> actors = ActorUtils::getActorList();
    for (auto actor : actors)
    {
        if (actor->mContext.mEntityId == targetId)
        {
            targeted = actor;
            break;
        }
    }

    bool rightClick = ImGui::IsMouseDown(1);
    static bool lastRightClick = false;
    if (rightClick && !lastRightClick)
    {
        if (targeted && targeted->isPlayer())
        {
            saveSkin(targeted);
        }
    }

    lastRightClick = rightClick;
}

void SkinStealer::saveSkin(Actor* actor)
{
    auto skin = actor->getSkin();
    if (!skin) return;

    const uint8_t* skinData = skin->mSkinImage.mImageBytes.data();
    int width = skin->mSkinImage.mWidth;
    int height = skin->mSkinImage.mHeight;
    int bytes = width * height * 4;
    std::vector<uint8_t> pngData = convToPng(std::vector<uint8_t>(skinData, skinData + bytes), width, height);
    if (pngData.empty()) return;

    std::string path = FileUtils::getSolsticeDir() + "Skins\\" + actor->getRawName() + "_skin.png";
    std::ofstream file(path, std::ios::binary);
    file.write((char*)pngData.data(), pngData.size());
    file.close();

    const uint8_t* capeData = skin->mCapeImage.mImageBytes.data();
    width = skin->mCapeImage.mWidth;
    height = skin->mCapeImage.mHeight;
    bytes = width * height * 4;
    if (bytes > 0)
    {
        pngData = convToPng(std::vector<uint8_t>(capeData, capeData + bytes), width, height);

        if (!pngData.empty())
        {
            path = FileUtils::getSolsticeDir() + "Skins\\" + actor->getRawName() + "_cape.png";
            std::ofstream file(path, std::ios::binary);
            file.write((char*)pngData.data(), pngData.size());
            file.close();
        }
    } else {
        ChatUtils::displayClientMessage("No cape found for " + actor->getRawName());
    }

    ChatUtils::displayClientMessage("Saved skin for " + actor->getRawName() + " to your Solstice\\Skins folder! (copied to clipboard)");
    ImGui::SetClipboardText(std::string(FileUtils::getSolsticeDir() + "Skins\\").c_str());
}
