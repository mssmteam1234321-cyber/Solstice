#pragma once
//
// Created by jcazm on 7/27/2024.
//

struct ActorRenderData {
    class Actor* actor;
    int64_t* data; // e. g. ItemStack if the actor is an ItemActor
    glm::vec3 position;
    glm::vec2 rotation;
    glm::vec2 headRot;
    bool glint;
    bool ignoreLighting;
    bool isInUI;
    float deltaTime;
    int modelObjId;
    float modelSize;
    //AnimationComponent *animationComponent;
    //MolangVariableMap *variables;

private:
    char pad[0x10];
};
