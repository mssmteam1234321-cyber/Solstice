//
// Created by vastrakai on 6/25/2024.
//

#include "BaseTickHook.hpp"

#include <SDK/Minecraft/Actor/Actor.hpp>

std::unique_ptr<Detour> BaseTickHook::mDetour = nullptr;

void BaseTickHook::onBaseTick(Actor* actor)
{
    auto oFunc = mDetour->GetFastcall<void, Actor*>();
    oFunc(actor);
    if (actor != ClientInstance::get()->getLocalPlayer()) return;

    Solstice::console->info("BaseTickHook called!!");
}

void BaseTickHook::init()
{
    mName = "BaseTickHook";
    Solstice::console->info(__FUNCTION__ " called");
    uintptr_t* baseTick = ClientInstance::get()->getLocalPlayer()->vtable[27];
    // log func addr
    Solstice::console->info("basetick addr @ {0:x}", reinterpret_cast<uint64_t>(baseTick));
    mDetour = std::make_unique<Detour>("Actor::baseTick", reinterpret_cast<void*>(baseTick), &BaseTickHook::onBaseTick);

    /*
    Address of signature = Minecraft.Windows.exe + 0x029C5283
    "\x48\x8B\x00\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x8B\x00\x48\x8B\x00\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x45\x84", "xx?????xx????xx?????xx?xx?????xx????xx"
    "48 8B ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? FF 15 ? ? ? ? 45 84"*/
}
