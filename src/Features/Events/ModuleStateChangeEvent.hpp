//
// Created by vastrakai on 6/29/2024.
//

#pragma once

#include "Event.hpp"

class Module;

class ModuleStateChangeEvent : public CancellableEvent {
public:
    Module* mModule;
    bool mEnabled;
    bool mWasEnabled;

    explicit ModuleStateChangeEvent(Module* module, bool enabled, bool wasEnabled) : CancellableEvent() {
        mModule = module;
        mEnabled = enabled;
        mWasEnabled = wasEnabled;
    }
};
