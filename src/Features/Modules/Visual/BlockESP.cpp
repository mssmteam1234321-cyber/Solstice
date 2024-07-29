//
// Created by vastrakai on 7/7/2024.
//

#include "BlockESP.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BlockChangedEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>

void BlockESP::onEnable()
{
    gFeatureManager->mDispatcher->listen<RenderEvent, &BlockESP::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &BlockESP::onBaesTickEvent>(this);
    gFeatureManager->mDispatcher->listen<BlockChangedEvent, &BlockESP::onBlockChangedEvent>(this);
}

void BlockESP::onDisable()
{
    gFeatureManager->mDispatcher->deafen<RenderEvent, &BlockESP::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &BlockESP::onBaesTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<BlockChangedEvent, &BlockESP::onBlockChangedEvent>(this);
}

struct DaBlock {
    AABB mBlock;
    ImColor mColor;
};

std::vector<DaBlock> blocks = {};

// Block mutex
static std::mutex blockMutex;

constexpr int REDSTONE_ORE = 73;
constexpr int REDSTONE_ORE_LIT = 74;
constexpr int DIAMOND_ORE = 56;
constexpr int EMERALD_ORE = 129;
constexpr int GOLD_ORE = 14;
constexpr int IRON_ORE = 15;
constexpr int LAPIS_ORE = 21;
constexpr int COAL_ORE = 16;
constexpr int BARRIER = 416;
constexpr int DEEPSLATE_REDSTONE_ORE = 658;
constexpr int DEEPSLATE_LIT_REDSTONE_ORE = 659;
constexpr int DEEPSLATE_DIAMOND_ORE = 660;
constexpr int DEEPSLATE_EMERALD_ORE = 662;
constexpr int DEEPSLATE_GOLD_ORE = 657;
constexpr int DEEPSLATE_IRON_ORE = 656;
constexpr int DEEPSLATE_LAPIS_ORE = 655;
constexpr int DEEPSLATE_COAL_ORE = 661;

std::unordered_map<int, ImColor> blockColors = {
    { 73, ImColor(1.f, 0.f, 0.f, 1.f) },
    { 74, ImColor(1.f, 0.f, 0.f, 1.f) },
    { 658, ImColor(1.f, 0.f, 0.f, 1.f) },
    { 659, ImColor(1.f, 0.f, 0.f, 1.f) },
    { 56, ImColor(0.f, 1.f, 1.f, 1.f) },
    { 660, ImColor(0.f, 1.f, 1.f, 1.f) },
    { 129, ImColor(0.f, 1.f, 0.f, 1.f) },
    { 662, ImColor(0.f, 1.f, 0.f, 1.f) },
    { 14, ImColor(1.f, 1.f, 0.f, 1.f) },
    { 657, ImColor(1.f, 1.f, 0.f, 1.f) },
    { 15, ImColor(1.f, 0.5f, 0.f, 1.f) },
    { 656, ImColor(1.f, 0.5f, 0.f, 1.f) },
    { 21, ImColor(0.f, 0.f, 1.f, 1.f) },
    { 655, ImColor(0.f, 0.f, 1.f, 1.f) },
    { 16, ImColor(0.f, 0.f, 0.f, 1.f) },
    { 661, ImColor(0.f, 0.f, 0.f, 1.f) }
};

bool isValidBlock(int id)
{
    return blockColors.find(id) != blockColors.end();
}

ImColor getColorFromId(int id)
{
    if (blockColors.find(id) != blockColors.end())
    {
        return blockColors[id];
    }

    return ImColor(1.f, 1.f, 1.f, 1.f);
}

void BlockESP::onBlockChangedEvent(BlockChangedEvent& event)
{
    auto dabl = BlockInfo(event.mNewBlock, event.mBlockPos);
    if (dabl.getDistance(*ClientInstance::get()->getLocalPlayer()->getPos()) > mRadius.mValue) return;

    if (isValidBlock(event.mNewBlock->mLegacy->getBlockId()))
    {
        std::lock_guard<std::mutex> lock(blockMutex);
        blocks.push_back({ dabl.getAABB(), getColorFromId(dabl.mBlock->toLegacy()->getBlockId()) });
        spdlog::debug("event.mNewBlock->mLegacy->mName: {} event.mOldBlock->mLegacy->mName: {}", event.mNewBlock->mLegacy->mName, event.mOldBlock->mLegacy->mName);
    }
    else
    {
        std::lock_guard<std::mutex> lock(blockMutex);
        std::erase_if(blocks, [&](DaBlock& block) {
            return block.mBlock == dabl.getAABB();
        });
    }
};

void BlockESP::onBaesTickEvent(BaseTickEvent& event) const
{
    static uint64_t lastUpdate = 0;
    uint64_t freq = mUpdateFrequency.mValue * 50.f;
    uint64_t now = NOW;

    if (lastUpdate + freq > now) return;

    lastUpdate = now;

    const auto player = ClientInstance::get()->getLocalPlayer();
    auto newBlocks = std::vector<DaBlock>();
    for (BlockInfo block : BlockUtils::getBlockList(*player->getPos(), mRadius.mValue))
    {
        int id = block.mBlock->toLegacy()->getBlockId();

        if (mEmerald.mValue && (id == EMERALD_ORE || id == DEEPSLATE_EMERALD_ORE))
        {
            newBlocks.push_back({ block.getAABB(), getColorFromId(id) });
        }
        else if (mDiamond.mValue && (id == DIAMOND_ORE || id == DEEPSLATE_DIAMOND_ORE))
        {
            newBlocks.push_back({ block.getAABB(), getColorFromId(id) });
        }
        else if (mGold.mValue && (id == GOLD_ORE || id == DEEPSLATE_GOLD_ORE))
        {
            newBlocks.push_back({ block.getAABB(), getColorFromId(id) });
        }
        else if (mIron.mValue && (id == IRON_ORE || id == DEEPSLATE_IRON_ORE))
        {
            newBlocks.push_back({ block.getAABB(), getColorFromId(id) });
        }
        else if (mCoal.mValue && (id == COAL_ORE || id == DEEPSLATE_COAL_ORE))
        {
            newBlocks.push_back({ block.getAABB(), getColorFromId(id) });
        }
        else if (mRedstone.mValue && (id == REDSTONE_ORE || id == REDSTONE_ORE_LIT || id == DEEPSLATE_REDSTONE_ORE || id == DEEPSLATE_LIT_REDSTONE_ORE))
        {
            newBlocks.push_back({ block.getAABB(), getColorFromId(id) });
        }
        else if (mLapis.mValue && (id == LAPIS_ORE || id == DEEPSLATE_LAPIS_ORE))
        {
            newBlocks.push_back({ block.getAABB(), getColorFromId(id) });
        }
        else continue; // Skip if not in the list
    }

    std::lock_guard<std::mutex> lock(blockMutex);
    blocks = newBlocks;
}

void BlockESP::onRenderEvent(RenderEvent& event)
{
    if (ClientInstance::get()->getMouseGrabbed()) return;
    std::lock_guard<std::mutex> lock(blockMutex);

    auto drawList = ImGui::GetBackgroundDrawList();

    for (auto& [block, color] : blocks)
    {
        std::vector<glm::vec2> points = MathUtils::getBoxPoints(block);
        std::vector<ImVec2> imPoints = {};
        for (auto point : points)
        {
            imPoints.emplace_back(point.x, point.y);
        }

        if (mRenderFilled.mValue) drawList->AddConvexPolyFilled(imPoints.data(), imPoints.size(), ImColor(color.Value.x, color.Value.y, color.Value.z, 0.25f));
        drawList->AddPolyline(imPoints.data(), imPoints.size(), color, 0, 2.0f);
    }
}