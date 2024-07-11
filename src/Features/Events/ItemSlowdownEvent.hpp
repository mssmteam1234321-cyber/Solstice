//
// Created by vastrakai on 7/3/2024.
//

#pragma once

#include "Event.hpp"

class ItemSlowdownEvent : public CancelableEvent {
public:
    ItemSlowdownEvent() : CancelableEvent() {}
};