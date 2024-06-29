#pragma once
//
// Created by vastrakai on 6/28/2024.
//


#include <entity/registry.hpp>
#include "EntityId.hpp"

class EntityRegistry;

struct EntityContext {
    EntityRegistry* mEntityRegistry;
    entt::basic_registry<EntityId>* mRegistry;
    EntityId mEntityId;

    template <typename T>
    T* getComponent() {
        return const_cast<T*>(mRegistry->try_get<T>(mEntityId));
    }


};