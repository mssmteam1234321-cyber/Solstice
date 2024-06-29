//
// Created by vastrakai on 6/29/2024.
//

#pragma once

#include "Event.hpp"

class ModuleStateChangeEvent : public Event {
public:
    bool mEnabled;
    bool mWasEnabled;

    explicit ModuleStateChangeEvent(bool enabled, bool wasEnabled) : Event() {
        mEnabled = enabled;
        mWasEnabled = wasEnabled;
    }
};