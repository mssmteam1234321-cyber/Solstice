//
// Created by vastrakai on 7/10/2024.
//

#include "RakPeerHooks.hpp"

#include <Solstice.hpp>
#include <windows.h>
#include <Features/FeatureManager.hpp>
#include <nes/event_dispatcher.hpp>
#include <SDK/SigManager.hpp>
#include <Utils/MemUtils.hpp>
#include <Features/Events/PingUpdateEvent.hpp>
#include <Features/Events/RunUpdateCycleEvent.hpp>

std::unique_ptr<Detour> RakPeerHooks::RunUpdateCycleDetour;
std::unique_ptr<Detour> RakPeerHooks::GetLastPingDetour;

float RakPeerHooks::LastPing;

void* peer = nullptr;

void RakPeerHooks::runUpdateCycle(void* _this, void* a2)
{
    peer = _this;
    auto original = RunUpdateCycleDetour->getOriginal<decltype(&runUpdateCycle)>();
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return original(_this, a2);


    auto holder = nes::make_holder<RunUpdateCycleEvent>();
    spdlog::trace("RakNet::RakPeer::RunUpdateCycle triggered RunUpdateCycle event");
    gFeatureManager->mDispatcher->trigger(holder);
    if (holder->isCancelled()) return;

    original(_this, a2);
}

__int64 RakPeerHooks::getLastPing(void* _this, void* a2)
{
    auto original = GetLastPingDetour->getOriginal<decltype(&getLastPing)>();
    auto val = original(_this, a2);

    static __int64 lastPing = 0;
    if (lastPing == val) return val;

    auto holder = nes::make_holder<PingUpdateEvent>(val);
    spdlog::trace("RakNet::RakPeer::GetLastPing triggered PingUpdate event [ping={0}]", val);
    gFeatureManager->mDispatcher->trigger(holder);

    lastPing = val;

    return val;
}

void RakPeerHooks::init(uintptr_t addr)
{
    static bool once = false;
    if (once) return;
    once = true;

    GetLastPingDetour = std::make_unique<Detour>("RakNet::RakPeer::GetLastPing", reinterpret_cast<void*>(addr), &getLastPing);
    GetLastPingDetour->enable();
}

void RakPeerHooks::init()
{
    mName = "RakPeerHooks";
    RunUpdateCycleDetour = std::make_unique<Detour>("RakNet::RakPeer::RunUpdateCycle", reinterpret_cast<void*>(SigManager::RakNet_RakPeer_runUpdateCycle), &runUpdateCycle);
    RunUpdateCycleDetour->enable();
    static std::thread t([this]() {
        while (!peer && !Solstice::mRequestEject) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (Solstice::mRequestEject) return;
        uintptr_t getAveragePingAddr = MemUtils::getAddressByIndex(reinterpret_cast<uintptr_t>(peer), 44);
        init(getAveragePingAddr);
        t.detach();
    });
}

void RakPeerHooks::shutdown()
{
    RunUpdateCycleDetour->restore();
    GetLastPingDetour->restore();
}