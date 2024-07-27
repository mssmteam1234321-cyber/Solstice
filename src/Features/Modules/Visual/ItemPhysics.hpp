#pragma once
//
// Created by jcazm on 7/27/2024.
//
#include <Features/Modules/Module.hpp>

class ItemPhysics : public ModuleBase<ItemPhysics> {
    uint32_t origPosRel = 0;
    float* newPosRel = nullptr;
public:
    float speed = 8.f;
    float xMul = 18.f, yMul = 16.f, zMul = 18.f;

    std::unordered_map<Actor*, std::tuple<float, glm::vec3, glm::ivec3>> actorData;
    struct ActorRenderData* renderData = nullptr;

    ItemPhysics() : ModuleBase("ItemPhysics", "Physica for items dumabss", ModuleCategory::Visual, 0, false) {

        mNames = {
            {Lowercase, "itemphysics"},
            {LowercaseSpaced, "item physics"},
            {Normal, "ItemPhysics"},
            {NormalSpaced, "Item Physics"}
        };
    }
    static void glm_rotate(glm::mat4x4& mat, float angle, float x, float y, float z);
    void onEnable() override;
    void onDisable() override;
    void onRenderEvent(class RenderEvent& event);
    void onItemRendererEvent(class ItemRendererEvent& event);
};
