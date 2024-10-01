//
// Created by vastrakai on 9/30/2024.
//

#include "Jesus.hpp"

#include <SDK/OffsetProvider.hpp>
#include <SDK/Minecraft/World/BlockLegacy.hpp>

bool Jesus::sdjReplace(bool patch)
{
    static uintptr_t waterFunc = SigManager::WaterBlockLegacy_getCollisionShapeForCamera;
    static uintptr_t solidFunc = SigManager::ConcreteBlockLegacy_getCollisionShapeForCamera;
    static int funcIndex = OffsetProvider::BlockLegacy_getCollisionShapeForCamera;

    auto funcOffset = funcIndex * 8;

    auto block = mWaterBlock;
    if (!block) return false;

    auto vtable = *reinterpret_cast<uintptr_t*>(block);
    auto func = *reinterpret_cast<uintptr_t*>(vtable + funcOffset);

    // if patched, replace with solid
    // if not patched, replace with water
    // you can access vtable with the mVfTable field
    uintptr_t** vfTable = reinterpret_cast<uintptr_t**>(block->mVfTable);

    // Set memory protection to read-write-execute
    DWORD oldProtect;
    VirtualProtect(vfTable + funcIndex, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect);

    if (patch)
    {
        vfTable[funcIndex] = reinterpret_cast<uintptr_t*>(solidFunc);
    }
    else
    {
        vfTable[funcIndex] = reinterpret_cast<uintptr_t*>(waterFunc);
    }

    // Restore memory protection
    VirtualProtect(vfTable + funcIndex, sizeof(uintptr_t), oldProtect, &oldProtect);

}

void Jesus::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &Jesus::onBaseTickEvent>(this);
}

void Jesus::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &Jesus::onBaseTickEvent>(this);
    sdjReplace(false);
}

void Jesus::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;

    auto ci = ClientInstance::get();
    auto bs = ci->getBlockSource();

    if (mWaterBlock == nullptr)
    {
        auto pos = (*player->getPos()) - PLAYER_HEIGHT_VEC;
        pos = glm::floor(pos);

        // Look in a 3x3 area around the player including y
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                for (int z = -1; z <= 1; z++)
                {
                    auto block = bs->getBlock(glm::ivec3(pos.x + x, pos.y + y, pos.z + z));

                    if (block->toLegacy()->getBlockId() == 9 || block->toLegacy()->getBlockId() == 8)
                    {
                        mWaterBlock = block->toLegacy();
                        break;
                    }
                }
            }
        }

        if (!mWaterBlock) return;
    }

    auto cPos = *player->getPos();
    Block* currentBlock = bs->getBlock(glm::floor(cPos) - glm::vec3(0, 1, 0));
    Block* belowBlock = bs->getBlock(glm::floor(cPos) - glm::vec3(0, 2, 0));
    int blockId = currentBlock->toLegacy()->getBlockId();
    int belowBlockId = belowBlock->toLegacy()->getBlockId();
    // if player is in water and isn't holding shift

    bool shift = Keyboard::mPressedKeys[VK_SHIFT];

    if ((blockId == 9 || blockId == 8) && !shift)
    {
        spdlog::info("Applying up velocity");
        player->getStateVectorComponent()->mVelocity.y = 0.42f;
        sdjReplace(false);
    } else if (shift && (belowBlockId == 9 || belowBlockId == 8 || blockId == 9 || blockId == 8) && mShift.mValue)
    {
        spdlog::info("Applying down velocity");
        sdjReplace(false);
        player->getStateVectorComponent()->mVelocity.y = -0.42f;
    } else
    {
        spdlog::info("Applying normal velocity");
        sdjReplace(true);
    }
}

void Jesus::onRenderEvent(RenderEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) mWaterBlock = nullptr;
}
