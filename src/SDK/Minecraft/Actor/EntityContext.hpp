#pragma once
//
// Created by vastrakai on 6/28/2024.
//


#include <iomanip>
#include <sstream>
#include <entity/registry.hpp>
#include <libhat.hpp>
#include <Windows.h>
#include <Utils/MemUtils.hpp>

#include "EntityId.hpp"
#include "spdlog/spdlog.h"

class EntityRegistry
{
    entt::basic_registry<EntityId>* mRegistry;
};

struct EntityContext {
    EntityRegistry* mEntityRegistry;
    entt::basic_registry<EntityId>* mRegistry;
    EntityId mEntityId;

    template <typename T>
    T* getComponent() {
        if (!this->mRegistry) return nullptr;
        return const_cast<T*>(mRegistry->try_get<T>(mEntityId));
    }

    template<typename component_t>
    hat::signature_view getAssureSignature() {
        uint32_t hash = entt::type_hash<component_t>::value();
        const auto* bytes = reinterpret_cast<uint8_t*>(&hash);

        std::stringstream ss;
        ss << "ba"; // mov edx, 0xhash

        //Add the hash bytes
        for (int i = 0; i < 4; i++) {
            ss << " " << std::setw(2) << std::setfill('0') << std::hex << (0xFF & bytes[i]);
        }

        return hat::parse_signature(ss.str()).value();
    }

    template<typename component_t>
    void* resolveAssure() {
        auto assureSig = getAssureSignature<component_t>();
        std::string componentName = typeid(component_t).name();
        auto result = reinterpret_cast<uintptr_t>(hat::find_pattern(assureSig).get());
        if (result == 0) {
            // Include the component Name and the signature
            spdlog::critical("Failed to resolve component: {}", componentName);
            MessageBoxA(nullptr, ("Failed to resolve component: " + componentName + ".").c_str(), "Error", MB_OK | MB_ICONERROR);
            return nullptr;
        }
        result += 5; // Add 5 to skip the mov edx, 0xhash instruction

        // Search for the next call instruction
        uint32_t relCallOffset = 0;
        uint64_t assureAddr = 0;
        while (true) {
            uint8_t byte = *reinterpret_cast<uint8_t*>(result);
            if (byte != 0xE8 && byte != 0xCC)
            {
                result += 1;
                continue;
            }

            // If the bytes is 0xE8 then it is a call, so we need to get the offset
            if (byte == 0xE8)
            {
                //Get the offset
                relCallOffset = *reinterpret_cast<uint32_t*>(result + 1);
                assureAddr = result + 5 + relCallOffset;
                std::span<std::byte> text = hat::process::get_section_data(hat::process::get_process_module(), ".text");

                // Check if the assure address is divisible by 16
                if (assureAddr % 16 != 0)
                {
                    result += 1;
                    continue;
                }

                // check if the address is in the .text section
                if (assureAddr >= reinterpret_cast<uintptr_t>(text.data()) && assureAddr < reinterpret_cast<uintptr_t>(text.data()) + text.size())
                {
                    break;
                }

                result += 1;
            }
        }

        // Log the resolved address
        spdlog::info("Resolved address for component: {} at {}", componentName, MemUtils::getMbMemoryString(assureAddr));
        return reinterpret_cast<void*>(assureAddr);
    }

    template<typename component_t>
    entt::basic_storage<component_t, EntityId>* assure()
    {
        using assure_t = entt::basic_storage<component_t, EntityId>* (__fastcall *)(entt::basic_registry<EntityId>*, uint32_t);
        static auto assure = reinterpret_cast<assure_t>(resolveAssure<component_t>());
        if (assure == nullptr) {
            assure = reinterpret_cast<assure_t>(resolveAssure<component_t>());
            return nullptr;
        }

        return assure(mRegistry, entt::type_hash<component_t>::value());
    }

    template<typename... type_t>
    [[nodiscard]] auto try_get() {
        if constexpr(sizeof...(type_t) == 1u) {
            auto* pool = assure<std::remove_const_t<type_t>...>();
            return (pool && pool->contains(mEntityId)) ? std::addressof(pool->get(mEntityId)) : nullptr;
        } else {
            return std::make_tuple(try_get<type_t>(mEntityId)...);
        }
    }
};
