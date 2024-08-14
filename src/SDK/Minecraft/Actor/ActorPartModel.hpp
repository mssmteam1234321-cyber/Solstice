//
// Created by vastrakai on 7/25/2024.
//

#pragma once

#include <glm/glm.hpp>
#include <SDK/OffsetProvider.hpp>

struct ActorPartModel {
    glm::vec3 mPos;
    glm::vec3 mRot;
    glm::vec3 mSize;
};

struct Bone
{
    CLASS_FIELD(std::string, mBoneStr, 0x10);

    ActorPartModel* getActorPartModel()
    {
        return reinterpret_cast<ActorPartModel*>(reinterpret_cast<uintptr_t>(this + 0xEC));
    }
};

class ActorModel
{
public:
    std::map<int, ActorPartModel*> mModels;

    ActorModel() = default;
};

